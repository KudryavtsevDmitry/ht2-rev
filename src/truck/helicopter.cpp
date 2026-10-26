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
extern "C" void FUN_5e52b0(); KING_ALIAS("_FUN_5e52b0", "@Resources_SetData@24")
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
extern "C" void FUN_5fd430(); KING_ALIAS("_FUN_5fd430", "@Sound_Play@20")    // sound: play (volume, pitch, a)
extern "C" void FUN_5b9660(); KING_ALIAS("_FUN_5b9660", "@Palette_Nearest@20")
extern "C" void FUN_5b6210(); KING_ALIAS("_FUN_5b6210", "_Pixel_Pack")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")    // a text of the resources
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")    // draws a text
extern "C" void FUN_440dd0(); KING_ALIAS("_FUN_440dd0", "@Rec440dd0_FreeVectors@8")
extern "C" int __cdecl Enum_SharpCallback(BYTE* n);

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
    CallVirt(obj, 32, (void*)Enum_SharpCallback, 0);
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

extern "C" void FUN_561800(); KING_ALIAS("_FUN_561800", "@Copter_Init0@16")
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
extern "C" void FUN_5ea1b0(); KING_ALIAS("_FUN_5ea1b0", "_Font_DrawText")    // draws a text in a box
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
extern "C" void FUN_567bb0(); KING_ALIAS("_FUN_567bb0", "@Turret_Fire@4")

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
extern "C" void FUN_5cec90(); KING_ALIAS("_FUN_5cec90", "@Fountain_Construct@56")    // a gauge's constructor
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
extern "C" void FUN_5ea1b0(); KING_ALIAS("_FUN_5ea1b0", "_Font_DrawText")    // text in a box
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
extern "C" void FUN_568090(); KING_ALIAS("_FUN_568090", "@Copter_Warnings@8")
extern "C" void FUN_566970(); KING_ALIAS("_FUN_566970", "@Copter_Frame@8")
extern "C" void FUN_567dc0(); KING_ALIAS("_FUN_567dc0", "@Copter_FindLanding@8")
ASM_PROC(FUN_563600)
extern "C" void FUN_561f30(); KING_ALIAS("_FUN_561f30", "@Copter_FlightStep@8")
extern "C" void FUN_50c3a0(); KING_ALIAS("_FUN_50c3a0", "_Shell_WriteThumbnail")    // writes the state (stream, 72, 54, 0)
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
extern "C" void FUN_5e52b0(); KING_ALIAS("_FUN_5e52b0", "@Resources_SetData@24")    // resources: send a message (id, kind, size, data)
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
extern "C" void FUN_56d6b0(); KING_ALIAS("_FUN_56d6b0", "@Thunk56d6b0@4")
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
extern "C" void FUN_5f98c0(); KING_ALIAS("_FUN_5f98c0", "@SceneNode5f98c0_Draw@16")
extern "C" void FUN_5f9d20(); KING_ALIAS("_FUN_5f9d20", "@SceneNode5f9d20_Draw@24")
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
extern "C" void FUN_5f9b00(); KING_ALIAS("_FUN_5f9b00", "@SceneNode5f9b00_Hit@16")
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

extern "C" void FUN_56f110(); KING_ALIAS("_FUN_56f110", "@Thunk56f110@4")
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

extern "C" void FUN_573d10(); KING_ALIAS("_FUN_573d10", "@Vehicle573d10_Destroy@8")
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
extern "C" void __fastcall Radio_NextMessage(BYTE* s);
// [FUN_569750] runs 56aa00 on the helicopter object 6f3438
extern "C" int __cdecl Heli_Run569750(void)
{
    return CallThis<int>((void (*)())Radio_NextMessage, g_q46_6f3438);
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

// ---------------------------------------------------------------------------
// The message queue of the radio.

extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")
// [$L_56aa00] shows the next of the 4 queued messages (+684, 128 bytes each, +1212 the current one) in +556 and
// marks the time (+32)
extern "C" void __fastcall Radio_NextMessage(BYTE* s)
{
    strcpy((char*)s + 556, (const char*)s + 684 + F(int, s, 1212) * 128);
    F(int, s, 1212) = (F(int, s, 1212) + 1) % 4;
    F(int, s, 1216) = 1;
    F(double, s, 32) = CallC<double>(FUN_6037e0);
}

extern "C" void FUN_566240(); KING_ALIAS("_FUN_566240", "@Copter_566240@8")
ASM_VAR(double, g_q180_64b780, $L_64b780)
ASM_VAR(double, g_q180_64b678, $L_64b678)
ASM_VAR(double, g_q180_64b670, $L_64b670)
ASM_VAR(double, g_q180_64ca48, $L_64ca48)
// [FUN_5668c0] the helicopter's climb state: the vertical speed share (+3448, from the velocity +100..+108 along
// the up axis +52/+64/+76), its sign (+3436), and with a landing place (566240) its position (+3468)
extern "C" void __fastcall Copter_ClimbState(BYTE* s)
{
    F(int, s, 3388) = 0;
    double d = F(float, s, 76) * F(float, s, 108) + F(float, s, 104) * F(float, s, 64);
    d = d + F(float, s, 52) * F(float, s, 100);
    double r = -d / (x87_fabs(d) + g_q180_64b780);
    F(float, s, 3448) = (float)r;
    F(int, s, 3436) = (int)(!(r >= g_q180_64b678) ? g_q180_64ca48 : g_q180_64b670);
    if (BYTE* p = CallThis<BYTE*>(FUN_566240, s)) {
        F(int, s, 3464) = 1;
        BYTE t[48];
        memcpy(s + 3468, CallThis<BYTE*>(FUN_57de90, p + 16, t) + 36, 12);
    }
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor573870_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor573870_DeletingDestructor@12", "@Dtor573870_DeletingDestructor@12")
// [$L_576c20] forwards to Dtor573870_DeletingDestructor for the object 256 bytes before
extern "C" int __fastcall Thunk576c20(BYTE* s, void* d, int a0) { return q198_Dtor573870_DeletingDestructor(s - 256, d, a0); }

// [$L_56d0d0] whether object c has its +416 set
extern "C" int __cdecl Obj56d0d0_Has416(int, int, BYTE* c) { return F(int, c, 416) != 0; }

extern "C" void FUN_5dbcc0(); KING_ALIAS("_FUN_5dbcc0", "@Camera_Get@12")
extern "C" void FUN_614f00(); KING_ALIAS("_FUN_614f00", "_Terrain_Call614960@12")
extern "C" void FUN_615670(); KING_ALIAS("_FUN_615670", "@Terrain615670_Update@12")
ASM_VAR(AsmLabel, g_q214_7025f0, $L_7025f0)
ASM_VAR(float, g_q214_702620, $L_702620)
ASM_VAR(float, g_q214_702624, $L_702624)
ASM_VAR(float, g_q214_702628, $L_702628)
ASM_VAR(AsmLabel, g_q214_702630, $L_702630)
ASM_VAR(BYTE*, g_q214_6f33b4, $L_6f33b4)
// [$L_569450] updates the terrain 6f33b4 for the camera: the direction 702620 turned by the camera's matrix and its
// position (+36), then the step with b; 1
extern "C" int __cdecl Terrain_FollowCamera(int, int b)
{
    float m[12], c[12];
    memset(m, 0, 48);
    memcpy(m, g_q214_7025f0, 48);
    memcpy(m, CallThis<float*>(FUN_5dbcc0, F(BYTE*, g_6cec6c, 1356), c), 48);
    double d0 = g_q214_702620, d1 = g_q214_702624, d2 = g_q214_702628;
    float v[3];
    v[0] = (float)(d2 * m[6] + d0 * m[0] + d1 * m[3]);
    v[1] = (float)(d2 * m[7] + d1 * m[4] + d0 * m[1]);
    v[2] = (float)(d2 * m[8] + d1 * m[5] + d0 * m[2]);
    CallStd(FUN_614f00, m + 9, v, (void*)g_q214_702630);
    CallThis(FUN_615670, g_q214_6f33b4, b);
    return 1;
}

extern "C" void FUN_568f60(); KING_ALIAS("_FUN_568f60", "_Player_CarId")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
extern "C" void FUN_5699b0(); KING_ALIAS("_FUN_5699b0", "@Messages_AddText@12")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")
extern "C" void FUN_430a80(); KING_ALIAS("_FUN_430a80", "_Ai_CleanBeforeLaw")
extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")
extern "C" void FUN_5279e0(); KING_ALIAS("_FUN_5279e0", "_Movie_Play")
extern "C" void FUN_445740(); KING_ALIAS("_FUN_445740", "_Ai_MafiaAttack")
ASM_VAR(AsmLabel, s_q216_67b938, $L_67b938)
ASM_VAR(AsmLabel, s_q216_67b928, $L_67b928)
ASM_VAR(AsmLabel, s_q216_67b91c, $L_67b91c)
ASM_VAR(AsmLabel, s_q216_67b910, $L_67b910)
ASM_VAR(AsmLabel, s_q216_67b904, $L_67b904)
ASM_VAR(AsmLabel, g_q216_6d1948, $L_6d1948)
ASM_VAR(double, g_q216_64b678, $L_64b678)
// shows message m and plays the sound object named name
static void Hel216_MessageSound(int m, const char* name)
{
    CallThis(FUN_5699b0, g_messages, CallC<int>(FUN_5d2e50, m));
    BYTE* o = CallC<BYTE*>(FUN_5ef3d0, name);
    if (o)
        CallThis(FUN_5fdaf0, o, 1.0f, 1.0f);
}
// [FUN_56ad20] the helicopter radio command c for the player's car: messages 54 and 55 with their sounds, the
// clean-up before the law (and its movie), message 73, or the mafia attack (and its movies)
extern "C" void __cdecl Copter_RadioCommand(int c)
{
    BYTE* p;
    CallC(FUN_568f60, &p);
    switch (c) {
    case 0: Hel216_MessageSound(54, (const char*)s_q216_67b938); break;
    case 1: Hel216_MessageSound(55, (const char*)s_q216_67b928); break;
    case 2:
        CallC(FUN_430a80, p);
        CallThis(FUN_530020, (void*)g_q216_6d1948);
        CallC(FUN_5279e0, (const char*)s_q216_67b91c, 5);
        break;
    case 4: CallThis(FUN_5699b0, g_messages, CallC<int>(FUN_5d2e50, 73)); break;
    case 3:
        CallC(FUN_445740, p);
        if (F(double, p, 224) > g_q216_64b678 || F(BYTE, p, 201))
            CallC(FUN_5279e0, (const char*)s_q216_67b910, 5);
        CallThis(FUN_530020, (void*)g_q216_6d1948);
        CallC(FUN_5279e0, (const char*)s_q216_67b904, 5);
        break;
    }
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
ASM_VAR(char, g_q237_6f2d98, $L_6f2d98)
extern "C" void q237_FUN_56bac0(); KING_ALIAS("_q237_FUN_56bac0", "_CopterState_Init")
extern "C" void q237_L_56bb30(); KING_ALIAS("_q237_L_56bb30", "@Vec_FreeStorage56bb30@4")
// [$L_56bb20] destroys the static object 6f2d98 at exit ($L_56bb30)
extern "C" void __cdecl Static6f2d98_Destroy(void) { CallThis(q237_L_56bb30, &g_q237_6f2d98); }
// [$L_56bb10] registers the teardown at exit
extern "C" int __cdecl Static6f2d98_AtExit(void) { return atexit((void(__cdecl*)(void))Static6f2d98_Destroy); }
// [$L_56bab0] the static initializer: FUN_56bac0, then the teardown at exit
extern "C" int __cdecl Static6f2d98_Init(void)
{
    CallC(q237_FUN_56bac0);
    return Static6f2d98_AtExit();
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Med5cf370_Destroy(BYTE* s); KING_ALIAS("@q238_Med5cf370_Destroy@4", "@Med5cf370_Destroy@4")
extern "C" int __fastcall q238_Wea5a63a0_Destroy(BYTE* s); KING_ALIAS("@q238_Wea5a63a0_Destroy@4", "@Wea5a63a0_Destroy@4")
// [FUN_56d6b0] forwards to Wea5a63a0_Destroy
extern "C" int __fastcall Thunk56d6b0(BYTE* s) { return q238_Wea5a63a0_Destroy(s); }
// [FUN_56f110] forwards to Med5cf370_Destroy
extern "C" int __fastcall Thunk56f110(BYTE* s) { return q238_Med5cf370_Destroy(s); }

// ---------------------------------------------------------------------------
// The dashboard warning lights and the screen overlays (key list, car list or
// scores, the player's status panel).

extern "C" void FUN_5b6f90(); KING_ALIAS("_FUN_5b6f90", "_Surface_FillRect")    // fills a rectangle (surface, rect, color, flags)
extern "C" void FUN_4d2b30(); KING_ALIAS("_FUN_4d2b30", "_Ai_DebugOverlay")
extern "C" void FUN_529890(); KING_ALIAS("_FUN_529890", "@Map_Draw@16")
extern "C" void FUN_56bc80(); KING_ALIAS("_FUN_56bc80", "@CopterPanel_Draw@8")
extern "C" void FUN_5b86c0(); KING_ALIAS("_FUN_5b86c0", "_Bitmap_Draw")
extern "C" void FUN_401aa0(); KING_ALIAS("_FUN_401aa0", "@Stl_Lock@8")
extern "C" int* __cdecl Player_CarId(int* out);
extern "C" int __cdecl Player_GetState(BYTE* player, void* dest);
extern "C" void __cdecl Ai_GetPlayerData(BYTE* p, BYTE* o);
extern "C" double __cdecl Player_Value136(BYTE* id);
extern "C" double __cdecl Player_CargoValueOf(BYTE* p);
extern "C" double __cdecl Player_ProfitOf(BYTE* p);
extern "C" int __cdecl Ai_GetPlayerOrderInfo(BYTE* p, unsigned idx, BYTE* oi);
extern "C" int __cdecl Ai_GetMerchandiseProperties(int id, BYTE* mp);
extern "C" int __fastcall Font_TextWidth(BYTE* self, void*, const char* s);
extern "C" float __fastcall Ramp_Position(BYTE* self, void*);
extern "C" BYTE* __fastcall Node_Next(BYTE* self, void*);
extern "C" void __fastcall Sound_Play(BYTE* s, void*, float vol, float pitch, int a3);
extern "C" double __cdecl Tsc_Now();
extern "C" void __cdecl Carat_DrawMovie();
extern "C" void __fastcall Wasm_595260(BYTE* self, void*);
extern "C" void __cdecl Ai_PoolFree(void* p, unsigned size);
extern "C" void __cdecl CarList_Draw(void* surface, int x, int y, int lines);
extern "C" void __cdecl Scores_Draw(BYTE* surface, int x, int y);
extern "C" int __cdecl Messages_Get1288();

ASM_VAR(BYTE*, g_hd_game, $L_6d2078)
ASM_VAR(int, g_hd_class0, $L_6ce330)
ASM_VAR(int, g_hd_class1, $L_6ce334)
ASM_VAR(BYTE*, g_hd_images, $L_6cecbc)         // dashboard images (+672 .. +1028)
ASM_VAR(double, g_hd_rampOn, $L_64b6e0)
ASM_VAR(double, g_hd_rampLow, $L_64da58)
ASM_VAR(BYTE*, g_hd_device, $L_720908)
ASM_VAR(BYTE*, g_hd_mainView, $L_696ca0)
ASM_VAR(BYTE*, g_hd_wnd, $L_6cec6c)
ASM_VAR(AsmLabel, g_hd_6ce4f8, $L_6ce4f8)
ASM_VAR(int, g_hd_x, $L_6ced0c)
ASM_VAR(int, g_hd_y, $L_6ced10)
ASM_VAR(BYTE*, g_hd_bmpKeys, $L_6f33d4)
ASM_VAR(BYTE*, g_hd_bmpCars, $L_6f33d8)
ASM_VAR(BYTE*, g_hd_bmpScores, $L_6f33dc)
ASM_VAR(BYTE*, g_hd_bmpStatus, $L_6f33e0)
ASM_VAR(BYTE*, g_hd_font, $L_6ced24)
ASM_VAR(BYTE*, g_hd_font2, $L_6ced2c)
ASM_VAR(AsmLabel, g_hd_keyNames, $L_675470)    // char* per key code
ASM_VAR(AsmLabel, s_hd_67c3f0, $L_67c3f0)
ASM_VAR(AsmLabel, s_hd_67c3f8, $L_67c3f8)
ASM_VAR(AsmLabel, s_hd_67c3fc, $L_67c3fc)
ASM_VAR(AsmLabel, s_hd_67c400, $L_67c400)
ASM_VAR(AsmLabel, s_hd_67c404, $L_67c404)
ASM_VAR(AsmLabel, s_hd_67c408, $L_67c408)
ASM_VAR(AsmLabel, s_hd_67c40c, $L_67c40c)
ASM_VAR(AsmLabel, s_hd_67c410, $L_67c410)
ASM_VAR(AsmLabel, s_hd_67c414, $L_67c414)
ASM_VAR(AsmLabel, s_hd_67c418, $L_67c418)
ASM_VAR(AsmLabel, s_hd_67c41c, $L_67c41c)
ASM_VAR(AsmLabel, s_hd_67c424, $L_67c424)
ASM_VAR(AsmLabel, s_hd_67c42c, $L_67c42c)
ASM_VAR(AsmLabel, s_hd_67c430, $L_67c430)
ASM_VAR(BYTE*, g_hd_6cecd8, $L_6cecd8)
ASM_VAR(double, g_hd_one, $L_64b670)
ASM_VAR(double, g_hd_barScale, $L_650320)
ASM_VAR(AsmLabel, s_hd_intFmt, $L_677a74)
ASM_VAR(double, g_hd_orderScale, $L_650318)
ASM_VAR(double, g_hd_loadScale, $L_64f340)
ASM_VAR(BYTE*, g_hd_messages, $L_6f3438)
ASM_VAR(double, g_hd_msec, $L_64b658)
ASM_VAR(float, g_hd_gauge, $L_650310)
ASM_VAR(float, g_hd_64c250, $L_64c250)
ASM_VAR(BYTE*, g_hd_6f3434, $L_6f3434)
ASM_VAR(AsmLabel, g_hd_6f68b0, $L_6f68b0)

static void Hud_Image(BYTE* surface, int x, int y, int image)
{
    CallC(FUN_5b8980, surface, x, y, F(BYTE*, g_hd_images, image), 0);
}

static void Hud_Fill(BYTE* surface, int* r, int color)
{
    CallC(FUN_5b6f90, surface, r, color, 0);
}

static BYTE* Hud_Vehicle()
{
    return F(BYTE*, F(BYTE*, g_hd_game, 616), 21600);
}

// 19 light states: +0 on, +8 a double 1.0
static void Hud_InitLights(BYTE* st)
{
    for (int k = 0; k < 19; k++) {
        F(int, st, k * 16 + 8) = 0;
        F(int, st, k * 16) = 0;
        F(int, st, k * 16 + 12) = 1072693248;
    }
}

extern "C" {

// [FUN_574950] the dashboard and its warning lights at (x, y) for the player's vehicle; alt picks the other
// dashboard picture. Vehicle types of class 1 have their own layout.
void __cdecl Hud_DrawLights(BYTE* surface, int x, int y, int alt)
{
    BYTE st[19 * 16];
    Hud_InitLights(st);
    int id;
    Player_CarId(&id);
    Player_GetState((BYTE*)id, st);
    BYTE* veh = Hud_Vehicle();
#define ON(k) F(int, st, (k) * 16)
#define RAMP(off) Ramp_Position(veh + (off), 0)
    int type = F(int, veh, 2252);
    if (type >= g_hd_class0 && type < g_hd_class1) {
        CallC(FUN_5b86c0, surface, x + 17, y + 722, F(BYTE*, g_hd_images, alt ? 1028 : 1020));
        for (int i = 0; i < 20; i++) {
            switch (i) {
            case 0: if (ON(3)) Hud_Image(surface, x + 117, y + 809, 852); break;
            case 1: if (RAMP(16216) >= g_hd_rampOn && ON(4)) Hud_Image(surface, x + 40, y + 788, 860); break;
            case 2: if (ON(5)) Hud_Image(surface, x + 43, y + 790, 868); break;
            case 3: if (ON(6)) Hud_Image(surface, x + 23, y + 755, 876); break;
            case 4: if (ON(7)) Hud_Image(surface, x + 60, y + 757, 884); break;
            case 5: if (ON(17)) Hud_Image(surface, x + 25, y + 776, 892); break;
            case 6: if (RAMP(16060) >= g_hd_rampOn && ON(0)) Hud_Image(surface, x + 33, y + 809, 900); break;
            case 7: if (ON(1)) Hud_Image(surface, x + 61, y + 809, 908); break;
            case 8: if (ON(18)) Hud_Image(surface, x + 50, y + 756, 916); break;
            case 9:
                if (RAMP(16164) >= g_hd_rampOn && ON(9) == 1) {
                    Hud_Image(surface, x + 36, y + 783, 924);
                    Hud_Image(surface, x + 114, y + 783, 924);
                }
                break;
            case 10:
                if (RAMP(16164) >= g_hd_rampOn && ON(9) == 2) {
                    Hud_Image(surface, x + 36, y + 783, 932);
                    Hud_Image(surface, x + 114, y + 783, 932);
                }
                break;
            case 11: if (ON(13)) Hud_Image(surface, x + 123, y + 770, 940); break;
            case 12: if (ON(2)) Hud_Image(surface, x + 117, y + 726, 948); break;
            case 13: if (ON(15)) Hud_Image(surface, x + 63, y + 754, 956); break;
            case 14: if (ON(16)) Hud_Image(surface, x + 61, y + 726, 964); break;
            case 15: if (ON(10)) Hud_Image(surface, x + 33, y + 726, 972); break;
            case 16: if (ON(12)) Hud_Image(surface, x + 89, y + 809, 980); break;
            case 17: if (ON(8)) Hud_Image(surface, x + 89, y + 726, 988); break;
            case 18: if (ON(11)) Hud_Image(surface, x + 33, y + 766, 996); break;
            }
        }
        if (!(RAMP(16060) >= g_hd_rampLow))
            Hud_Image(surface, x + 33, y + 809, 896);
        if (!(RAMP(16216) >= g_hd_rampOn))
            Hud_Image(surface, x + 40, y + 788, 856);
        if (F(BYTE, veh, 15508) & 3)
            Hud_Image(surface, x + 36, y + 783, 920);
        if (F(BYTE, veh, 15508) & 12)
            Hud_Image(surface, x + 114, y + 783, 920);
        if (F(int, veh, 15504) > 0)
            Hud_Image(surface, x + 68, y + 776, 1000);
        if (F(int, veh, 15512) > 0)
            Hud_Image(surface, x + 51, y + 767, 1008);
        if (CallVirt<int>(F(BYTE*, g_hd_game, 616), 64) > 3)
            Hud_Image(surface, x + 23, y + 755, 872);
        if (CallVirt<int>(F(BYTE*, g_hd_game, 616), 68) > 0)
            Hud_Image(surface, x + 25, y + 776, 888);
        return;
    }
    CallC(FUN_5b86c0, surface, x + 17, y + 722, F(BYTE*, g_hd_images, alt ? 1024 : 1016));
    for (int i = 0; i < 20; i++) {
        switch (i) {
        case 0: if (ON(3)) Hud_Image(surface, x + 117, y + 809, 684); break;
        case 1: if (RAMP(16216) >= g_hd_rampOn && ON(4)) Hud_Image(surface, x + 44, y + 791, 692); break;
        case 2: if (ON(5)) Hud_Image(surface, x + 47, y + 792, 700); break;
        case 3: if (ON(6)) Hud_Image(surface, x + 27, y + 742, 708); break;
        case 4: if (ON(7)) Hud_Image(surface, x + 28, y + 755, 716); break;
        case 5: if (ON(17)) Hud_Image(surface, x + 24, y + 786, 724); break;
        case 6: if (RAMP(16060) >= g_hd_rampOn && ON(0)) Hud_Image(surface, x + 33, y + 809, 732); break;
        case 7: if (ON(1)) Hud_Image(surface, x + 61, y + 809, 740); break;
        case 8: if (ON(18)) Hud_Image(surface, x + 61, y + 734, 748); break;
        case 9:
            if (RAMP(16164) >= g_hd_rampOn && ON(9) == 1) {
                Hud_Image(surface, x + 41, y + 786, 756);
                Hud_Image(surface, x + 103, y + 786, 756);
                Hud_Image(surface, x + 126, y + 786, 756);
            }
            break;
        case 10:
            if (RAMP(16164) >= g_hd_rampOn && ON(9) == 2) {
                Hud_Image(surface, x + 41, y + 786, 764);
                Hud_Image(surface, x + 103, y + 786, 764);
                Hud_Image(surface, x + 126, y + 786, 764);
            }
            break;
        case 11: if (ON(13)) Hud_Image(surface, x + 71, y + 785, 772); break;
        case 12: if (ON(2)) Hud_Image(surface, x + 117, y + 726, 780); break;
        case 13: if (ON(15)) Hud_Image(surface, x + 21, y + 747, 788); break;
        case 14: if (ON(16)) Hud_Image(surface, x + 117, y + 754, 796); break;
        case 15: if (ON(10)) Hud_Image(surface, x + 89, y + 754, 804); break;
        case 16: if (ON(12)) Hud_Image(surface, x + 89, y + 809, 812); break;
        case 17: if (ON(8)) Hud_Image(surface, x + 89, y + 726, 820); break;
        case 18: if (ON(11)) Hud_Image(surface, x + 38, y + 737, 828); break;
        }
    }
    if (!(RAMP(16060) >= g_hd_rampLow))
        Hud_Image(surface, x + 33, y + 809, 728);
    if (!(RAMP(16216) >= g_hd_rampOn))
        Hud_Image(surface, x + 44, y + 791, 688);
    if (F(BYTE, veh, 15508) & 3)
        Hud_Image(surface, x + 41, y + 786, 752);
    if (F(BYTE, veh, 15508) & 12)
        Hud_Image(surface, x + 103, y + 786, 752);
    if (F(BYTE, veh, 15508) & 48)
        Hud_Image(surface, x + 126, y + 786, 752);
    if (F(int, veh, 15504) > 0)
        Hud_Image(surface, x + 34, y + 773, 832);
    if (F(int, veh, 15512) > 0)
        Hud_Image(surface, x + 34, y + 763, 840);
    if (CallVirt<int>(F(BYTE*, g_hd_game, 616), 64) > 3)
        Hud_Image(surface, x + 27, y + 742, 704);
    if (CallVirt<int>(F(BYTE*, g_hd_game, 616), 68) > 0)
        Hud_Image(surface, x + 24, y + 786, 720);
#undef ON
#undef RAMP
}

// $L_5756f0: the overlays over the game screen: the key list, the car list or the scores, and the player's
// status panel (cargo/profit bars, current orders, load gauges and the dashboard)
void __cdecl Hud_DrawOverlays()
{
    BYTE* dev = g_hd_device;
    (*(int(__stdcall**)(BYTE*, int, int))(*(BYTE**)dev + 80))(dev, 14, 1);
    BYTE* surf = F(BYTE*, g_hd_mainView, 12);
    CallC(FUN_4d2b30, surf);
    int n = 0;
    BYTE* w = g_hd_wnd;
    if (F(int, w, 1292) && F(BYTE*, w, 1356) == g_hd_mainView)
        CallThis(FUN_529890, (BYTE*)g_hd_6ce4f8, surf, F(int, w, 1292) - 1);

    if (F(int, g_hd_wnd, 1296)) {
        Carat_DrawMovie();
        CallC(FUN_5b86c0, surf, g_hd_x, g_hd_y, g_hd_bmpKeys);
        int xx = g_hd_x + 25, yy = g_hd_y + 462;
        int* keys = (int*)(g_hd_wnd + 528);
        for (BYTE* node = F(BYTE*, F(BYTE*, g_hd_wnd, 524), 288); node; node = Node_Next(node, 0)) {
            int key = F(int, node, 28) - 1000;
            int ly = yy + n % 18 * 20 + 10;
            int lx = xx + n / 18 * 170;
            CallC(FUN_5ea170, surf, lx, ly, g_hd_font, F(const char*, node, 12), 6, 0);
            CallC(FUN_5ea170, surf, lx + 111, ly, g_hd_font, ((const char**)g_hd_keyNames)[keys[key]], 6, 0);
            n++;
        }
        Menu_DrawEntry((int)surf, xx, yy, 97, s_hd_67c430, n++);
        Menu_DrawEntry((int)surf, xx, yy, 32, s_hd_67c42c, n++);
        Menu_DrawEntry((int)surf, xx, yy, 31, s_hd_67c424, n++);
        Menu_DrawEntry((int)surf, xx, yy, 84, s_hd_67c41c, n++);
        Menu_DrawEntry((int)surf, xx, yy, 16, s_hd_67c418, n++);
        Menu_DrawEntry((int)surf, xx, yy, 20, s_hd_67c414, n++);
        Menu_DrawEntry((int)surf, xx, yy, 19, s_hd_67c410, n++);
        Menu_DrawEntry((int)surf, xx, yy, 66, s_hd_67c40c, n++);
        Menu_DrawEntry((int)surf, xx, yy, 106, s_hd_67c408, n++);
        Menu_DrawEntry((int)surf, xx, yy, 107, s_hd_67c404, n++);
        Menu_DrawEntry((int)surf, xx, yy, 104, s_hd_67c400, n++);
        Menu_DrawEntry((int)surf, xx, yy, 105, s_hd_67c3fc, n++);
        Menu_DrawEntry((int)surf, xx, yy, 14, s_hd_67c3f8, n++);
        Menu_DrawEntry((int)surf, xx, yy, 108, s_hd_67c3f0, n);
    }

    if (F(int, g_hd_wnd, 1284)) {
        Carat_DrawMovie();
        if (F(int, g_hd_wnd, 1316) == 0) {
            CallC(FUN_5b86c0, surf, g_hd_x, g_hd_y, g_hd_bmpCars);
            CarList_Draw(surf, g_hd_x + 17, g_hd_y + 471, 10);
        } else {
            CallC(FUN_5b86c0, surf, g_hd_x, g_hd_y, g_hd_bmpScores);
            Scores_Draw(surf, g_hd_x + 17, g_hd_y + 545);
        }
    }

    if (F(int, g_hd_wnd, 1288)) {
        Carat_DrawMovie();
        int r[4] = {0, 0, 0, 0};        // left, top, right, bottom
        int load1 = 0, load2 = 0;
        CallC(FUN_5b86c0, surf, g_hd_x, g_hd_y, g_hd_bmpStatus);
        int bx = g_hd_x + 17, by = g_hd_y + 492;
        BYTE pd[192];
        int id;
        Player_CarId(&id);
        Ai_GetPlayerData((BYTE*)id, pd);
        CallC(FUN_5ea1b0, surf, bx + 2, by, 148, g_hd_font, g_hd_6cecd8 + 632, 5, 0);
        CallC(FUN_5ea1b0, surf, bx + 30, by + 228, 145, g_hd_font, F(BYTE*, pd, 20), 5, 0);

        Player_CarId(&id);
        double value = Player_Value136((BYTE*)id);
        Player_CarId(&id);
        double total = Player_CargoValueOf((BYTE*)id) + value;
        if (!(total >= g_hd_one))
            total = 1.0;
        Player_CarId(&id);
        int bar1 = (int)(Player_Value136((BYTE*)id) * g_hd_barScale / total);
        Player_CarId(&id);
        int bar2 = (int)(Player_ProfitOf((BYTE*)id) * g_hd_barScale / total);
        if (bar1 < 0)
            bar1 = 0;
        if (bar2 < 0)
            bar2 = 0;
        if (bar1 > 158)
            bar1 = 158;
        if (bar2 > 158)
            bar2 = 158;
        r[0] = bx;
        r[1] = by + 95;
        r[2] = bar1 + bx;
        r[3] = by + 114;
        Hud_Fill(surf, r, 5);
        r[0] = bx + 158 - bar2;
        r[2] = bx + 158;
        Hud_Fill(surf, r, 6);

        char buf[160];
        Player_CarId(&id);
        sprintf(buf, s_hd_intFmt, (int)Player_Value136((BYTE*)id));
        CallC(FUN_5ea170, surf, g_hd_x + 23, g_hd_y + 605, Messages_Get1288(), buf, 5, 0);
        Player_CarId(&id);
        sprintf(buf, s_hd_intFmt, (int)Player_CargoValueOf((BYTE*)id));
        int width = Font_TextWidth(g_hd_font, 0, buf);
        CallC(FUN_5ea170, surf, r[2] - width - 5, g_hd_y + 605, Messages_Get1288(), buf, 5, 0);

        // the order info (+0 merchandise, +8 amount, +100/+104/+112 texts, +120/+128 doubles; vectors at
        // +52/+64/+76/+88 that it owns)
        BYTE oi[188];
        F(int, oi, 0) = -1;
        F(int, oi, 8) = 0;
        F(int, oi, 12) = 0;
        F(int, oi, 16) = -1;
        F(int, oi, 20) = -1;
        F(int, oi, 24) = 0;
        F(int, oi, 28) = 0;
        F(int, oi, 32) = 0;
        F(int, oi, 36) = 0;
        F(int, oi, 40) = 0;
        F(int, oi, 44) = -1;
        F(int, oi, 48) = -1;
        for (int k = 52; k <= 112; k += 4)
            F(int, oi, k) = 0;
        for (int k = 120; k <= 176; k += 4)
            F(int, oi, k) = 0;
        F(int, oi, 180) = 1072693248;
        F(BYTE, oi, 184) = 0;
        Player_CarId(&id);
        if (Ai_GetPlayerOrderInfo((BYTE*)id, 0, oi) == 0) {
            sprintf(buf, s_hd_intFmt, (int)F(double, oi, 120));
            CallC(FUN_5ea170, surf, bx + 110, by + 60, g_hd_font, buf, 4, 0);
            r[0] = g_hd_x + 17;
            r[2] = (int)(F(double, oi, 128) * g_hd_orderScale / F(double, oi, 120) + r[0]);
            r[1] = g_hd_y + 557;
            r[3] = g_hd_y + 565;
            Hud_Fill(surf, r, 5);
        }
        int listed = 0;
        BYTE mp[40];
        F(int, mp, 0) = 0;
        for (unsigned idx = 0;; idx++) {
            Player_CarId(&id);
            if (Ai_GetPlayerOrderInfo((BYTE*)id, idx, oi))
                break;
            Ai_GetMerchandiseProperties(F(int, oi, 0), mp);
            load1 = (int)(F(double, mp, 16) * F(double, oi, 8) * g_hd_loadScale / F(double, pd, 176) + load1);
            load2 = (int)(F(double, mp, 24) * F(double, oi, 8) * g_hd_loadScale / F(double, pd, 168) + load2);
            if (listed == 0) {
                BYTE st[19 * 16];
                Hud_InitLights(st);
                Player_CarId(&id);
                Player_GetState((BYTE*)id, st);
                if (F(int, st, 160) && F(int, oi, 104)) {
                    Sound_Play(F(BYTE*, g_hd_messages, 1284), 0, 1.0f, 1.0f, 1);
                    if ((int)(Tsc_Now() * g_hd_msec) % 2)
                        CallC(FUN_5ea170, surf, bx + 3, by + 52, g_hd_font2, F(const char*, oi, 104), 4, 0);
                    else
                        CallC(FUN_5ea170, surf, bx + 3, by + 52, g_hd_font, F(const char*, oi, 100), 4, 0);
                } else
                    CallC(FUN_5ea170, surf, bx + 3, by + 52, g_hd_font, F(const char*, oi, 100), 4, 0);
                CallC(FUN_5ea170, surf, bx + 3, by + 64, g_hd_font, F(const char*, oi, 112), 4, 0);
            }
            listed = 1;
        }

        // the load gauges: two pairs of bars, the load and the ramp positions
        r[0] = bx;
        r[1] = by + 136;
        r[2] = bx + load1;
        r[3] = by + 144;
        Hud_Fill(surf, r, 5);
        r[2] = (int)((double)Ramp_Position(Hud_Vehicle() + 16164, 0) * g_hd_gauge + r[0]);
        r[1] = by + 166;
        r[3] = by + 174;
        Hud_Fill(surf, r, 5);
        r[2] = (int)((double)Ramp_Position(Hud_Vehicle() + 16060, 0) * g_hd_gauge + r[0]);
        r[1] = by + 196;
        r[3] = by + 204;
        Hud_Fill(surf, r, 5);
        r[0] = bx + 79;
        r[1] = by + 136;
        r[2] = bx + 79 + load2;
        r[3] = by + 144;
        Hud_Fill(surf, r, 5);
        r[2] = (int)((double)Ramp_Position(Hud_Vehicle() + 16216, 0) * g_hd_gauge + r[0]);
        r[1] = by + 166;
        r[3] = by + 174;
        Hud_Fill(surf, r, 5);
        r[2] = (int)(((double)g_hd_64c250 - F(float, Hud_Vehicle(), 15552)) * g_hd_gauge + r[0]);
        r[1] = by + 196;
        r[3] = by + 204;
        Hud_Fill(surf, r, 5);

        Hud_DrawLights(surf, g_hd_x, g_hd_y, 0);
        CallC(FUN_5b86c0, surf, g_hd_x + 17, g_hd_y + 422, F(BYTE*, g_hd_images, 672));
        for (int v = 88; v >= 52; v -= 12)
            if (unsigned size = F(BYTE*, oi, v + 8) - F(BYTE*, oi, v))
                Ai_PoolFree(F(BYTE*, oi, v), size);
    }

    if (g_hd_6f3434)
        CallThis(FUN_56bc80, g_hd_6f3434);
    Wasm_595260((BYTE*)g_hd_6f68b0, 0);
    dev = g_hd_device;
    (*(int(__stdcall**)(BYTE*, int, int))(*(BYTE**)dev + 80))(dev, 14, 1);
}

}  // extern "C"

// ---------------------------------------------------------------------------
// Starting the game world and creating its objects (vehicles, effects).

extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_41eb70(); KING_ALIAS("_FUN_41eb70", "@Iter_Players@8")
extern "C" void FUN_42db90(); KING_ALIAS("_FUN_42db90", "_AI_FreeList68b99c")
extern "C" void FUN_434780(); KING_ALIAS("_FUN_434780", "_Ai_StartGame")
extern "C" void FUN_437520(); KING_ALIAS("_FUN_437520", "_Ai_GetPositionId")
extern "C" void FUN_438d70(); KING_ALIAS("_FUN_438d70", "_Road_GetRoadParameters")
extern "C" void FUN_43a780(); KING_ALIAS("_FUN_43a780", "_Ai_GetOrientationF")
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")
extern "C" void FUN_43f690(); KING_ALIAS("_FUN_43f690", "_Player_SetFlags")
extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")
extern "C" void FUN_4cd670(); KING_ALIAS("_FUN_4cd670", "@Fx_ObjectInit@44")
extern "C" void FUN_4d0b10(); KING_ALIAS("_FUN_4d0b10", "@Effect4d0b10_Construct@20")
extern "C" void FUN_4d2790(); KING_ALIAS("_FUN_4d2790", "@Effect4d2790_Construct@64")
extern "C" void FUN_4e0660(); KING_ALIAS("_FUN_4e0660", "_Obj_HookHandlers")
extern "C" void FUN_4e5ba7(); KING_ALIAS("_FUN_4e5ba7", "_Query_HitObject")
extern "C" void FUN_50bc90(); KING_ALIAS("_FUN_50bc90", "_Str_ToWide")
extern "C" void FUN_51a4a0(); KING_ALIAS("_FUN_51a4a0", "_Shell_ScreenTitle")
extern "C" void FUN_51a4e0(); KING_ALIAS("_FUN_51a4e0", "_Shell_CarTypeIndex")
extern "C" void FUN_51bfa0(); KING_ALIAS("_FUN_51bfa0", "_Save_OpenStorage@8")
extern "C" void FUN_52ea90(); KING_ALIAS("_FUN_52ea90", "_CarType_Number")
extern "C" void FUN_53fcd0(); KING_ALIAS("_FUN_53fcd0", "@Car_53fcd0@12")
extern "C" void FUN_543c00(); KING_ALIAS("_FUN_543c00", "@Car_SetView@12")
extern "C" void FUN_546900(); KING_ALIAS("_FUN_546900", "@Car_ApplySetup@12")
extern "C" void FUN_571d20(); KING_ALIAS("_FUN_571d20", "_Copter_LoadStreams")
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
extern "C" void FUN_5c4910(); KING_ALIAS("_FUN_5c4910", "@Media_Clear5c4910@8")
extern "C" void FUN_5dc2d0(); KING_ALIAS("_FUN_5dc2d0", "@App_SubPath@16")
extern "C" void FUN_5e50d0(); KING_ALIAS("_FUN_5e50d0", "@Resources_NewRow@12")
extern "C" void FUN_5e5130(); KING_ALIAS("_FUN_5e5130", "@Resources_Create@16")
extern "C" void FUN_5e52b0(); KING_ALIAS("_FUN_5e52b0", "@Resources_SetData@24")
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_603cb0(); KING_ALIAS("_FUN_603cb0", "_Ini_GetDouble")
extern "C" void FUN_60e0f0(); KING_ALIAS("_FUN_60e0f0", "@MarineTask_Construct@12")
extern "C" void FUN_6142f0(); KING_ALIAS("_FUN_6142f0", "@TerrainView_Construct@40")
extern "C" void FUN_6148a0(); KING_ALIAS("_FUN_6148a0", "@Terrain_FreeObjects@8")
extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);
extern "C" void FUN_4d08d0(); KING_ALIAS("_FUN_4d08d0", "_Fx_MakePlayerVehicle")    // the player's vehicle for an effect (world, effect, player id)
extern "C" void FUN_4ebd00(); KING_ALIAS("_FUN_4ebd00", "@PhysBody_Construct@64")    // constructs a vehicle (scene object, matrix, type)
extern "C" void FUN_5291a0(); KING_ALIAS("_FUN_5291a0", "@Map_BuildRoads@8")
extern "C" void FUN_544920(); KING_ALIAS("_FUN_544920", "@NetVehicle_Construct@40")    // constructs a network vehicle
ASM_PROC(FUN_5a44c0)    // constructs a scene item (world, object, object, n, scale, 0, matrix, 1)
extern "C" void FUN_5d2540(); KING_ALIAS("_FUN_5d2540", "@NameHash_Add@16")    // registers a named callback (name, function)

ASM_VAR(BYTE*, g_ws_wnd, $L_6cec6c)
ASM_VAR(BYTE*, g_ws_mainView, $L_696ca0)
ASM_VAR(int, g_ws_6f33ac, $L_6f33ac)
ASM_VAR(int, g_ws_6f3498, $L_6f3498)
ASM_VAR(int, g_ws_6f3438, $L_6f3438)
ASM_VAR(int, g_ws_videoMode, $L_6ced70)
ASM_VAR(int, g_ws_carTypes, $L_6ce34c)
ASM_VAR(void*, g_ws_prevCreate, $L_6f3418)     // the create hook replaced by Game_CreateObject
ASM_VAR(DWORD, g_ws_prevHook2, $L_6f341c)
extern "C" int __cdecl MissionObj_Event(int* e, int a);
ASM_VAR(AsmLabel, g_ws_569550, _Terrain_RayCallback)
ASM_VAR(AsmLabel, s_ws_iniFile, $L_672998)
ASM_VAR(char*, g_ws_iniPath, $L_696868)
ASM_VAR(int, g_ws_696900, $L_696900)
ASM_VAR(BYTE*, g_ws_696340, $L_696340)
ASM_VAR(int, g_ws_6f340c, $L_6f340c)
ASM_VAR(double, g_ws_6f32e8, $L_6f32e8)
ASM_VAR(double, g_ws_6f32f0, $L_6f32f0)
ASM_VAR(double, g_ws_6f32f8, $L_6f32f8)
ASM_VAR(double, g_ws_6f3300, $L_6f3300)
ASM_VAR(double, g_ws_6f3308, $L_6f3308)
ASM_VAR(int, g_ws_6f3310, $L_6f3310)
ASM_VAR(int, g_ws_6f3314, $L_6f3314)
ASM_VAR(int, g_ws_6f3318, $L_6f3318)
ASM_VAR(int, g_ws_6f331c, $L_6f331c)
ASM_VAR(int, g_ws_6f3320, $L_6f3320)
ASM_VAR(AsmLabel, s_ws_67c310, $L_67c310)
ASM_VAR(AsmLabel, s_ws_67c304, $L_67c304)
ASM_VAR(AsmLabel, s_ws_67c2fc, $L_67c2fc)
ASM_VAR(AsmLabel, s_ws_67c2f4, $L_67c2f4)
ASM_VAR(BYTE*, g_ws_6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_ws_6cec9c, $L_6cec9c)
ASM_VAR(double, g_ws_64b5a0, $L_64b5a0)
ASM_VAR(double, g_ws_64b738, $L_64b738)
ASM_VAR(double, g_ws_64b6e0, $L_64b6e0)
ASM_VAR(double, g_ws_64b958, $L_64b958)
ASM_VAR(AsmLabel, s_ws_roomName, $L_67c2e8)       // "room_tb_000"
ASM_VAR(AsmLabel, s_ws_tb, $L_67c2e4)             // "tb"
ASM_VAR(int, g_ws_67b734, $L_67b734)
ASM_VAR(BYTE*, g_ws_6cec94, $L_6cec94)
ASM_VAR(AsmLabel, g_ws_6d24bc, $L_6d24bc)
ASM_VAR(AsmLabel, s_ws_672bb4, $L_672bb4)
ASM_VAR(AsmLabel, s_ws_67c2d8, $L_67c2d8)
ASM_VAR(AsmLabel, s_ws_67c2cc, $L_67c2cc)
ASM_VAR(AsmLabel, s_ws_6729a8, $L_6729a8)
ASM_VAR(AsmLabel, s_ws_672994, $L_672994)
ASM_VAR(int, g_ws_696328, $L_696328)
ASM_VAR(int, g_ws_696334, $L_696334)
ASM_VAR(AsmLabel, s_ws_66c874, $L_66c874)
ASM_VAR(BYTE*, g_ws_6cecd8, $L_6cecd8)
ASM_VAR(AsmLabel, g_ws_saveName, $L_6f2d18)
ASM_VAR(AsmLabel, s_ws_676848, $L_676848)
ASM_VAR(AsmLabel, s_ws_67683c, $L_67683c)
ASM_VAR(AsmLabel, s_ws_67c2b0, $L_67c2b0)
ASM_VAR(AsmLabel, s_ws_67c298, $L_67c298)
ASM_VAR(AsmLabel, g_ws_6ce4f8, $L_6ce4f8)
ASM_VAR(AsmLabel, g_ws_71e820, $L_71e820)
ASM_VAR(AsmLabel, s_ws_67c280, $L_67c280)
ASM_VAR(AsmLabel, s_ws_67c268, $L_67c268)
ASM_VAR(BYTE*, g_ws_marine, $L_6f33b0)
ASM_VAR(BYTE*, g_ws_terrain, $L_6f33b4)
ASM_VAR(AsmLabel, s_ws_676880, $L_676880)
ASM_VAR(AsmLabel, s_ws_676870, $L_676870)
ASM_VAR(AsmLabel, s_ws_67c258, $L_67c258)
ASM_VAR(AsmLabel, s_ws_67c23c, $L_67c23c)
ASM_VAR(AsmLabel, s_ws_67c22c, $L_67c22c)
ASM_VAR(AsmLabel, s_ws_67c24c, $L_67c24c)
ASM_VAR(AsmLabel, s_ws_rb, $L_6703e8)
ASM_VAR(BYTE*, g_ws_device, $L_720908)
ASM_VAR(int, g_ws_6f34d4, $L_6f34d4)
ASM_VAR(int, g_ws_created, $L_6f34ac)

ASM_VAR(int, g_ws_noPlayer, $L_68b990)
ASM_VAR(float, g_ws_702630, $L_702630)
ASM_VAR(float, g_ws_702634, $L_702634)
ASM_VAR(float, g_ws_702638, $L_702638)
ASM_VAR(AsmLabel, s_ws_66d638, $L_66d638)
ASM_VAR(AsmLabel, s_ws_66d628, $L_66d628)
ASM_VAR(AsmLabel, s_ws_67c3b8, $L_67c3b8)
ASM_VAR(AsmLabel, s_ws_67c3a8, $L_67c3a8)
ASM_VAR(AsmLabel, s_ws_67c398, $L_67c398)
ASM_VAR(int, g_ws_67f878, $L_67f878)
ASM_VAR(AsmLabel, g_ws_6d2258, $L_6d2258)
ASM_VAR(AsmLabel, g_ws_vt650250, $L_650250)
ASM_VAR(AsmLabel, g_ws_vt650164, $L_650164)
ASM_VAR(AsmLabel, g_ws_vt650118, $L_650118)
ASM_VAR(AsmLabel, g_ws_vt650114, $L_650114)
ASM_VAR(AsmLabel, g_ws_7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_ws_vt6500e4, $L_6500e4)
ASM_VAR(AsmLabel, g_ws_vt650098, $L_650098)
ASM_VAR(AsmLabel, g_ws_vt650068, $L_650068)
ASM_VAR(AsmLabel, g_ws_vt65001c, $L_65001c)
ASM_VAR(AsmLabel, g_ws_vt64ffec, $L_64ffec)
ASM_VAR(AsmLabel, g_ws_vt64ffa0, $L_64ffa0)
ASM_VAR(AsmLabel, g_ws_vt64ff70, $L_64ff70)
ASM_VAR(AsmLabel, g_ws_vt64ff24, $L_64ff24)
ASM_VAR(AsmLabel, s_ws_67c390, $L_67c390)
ASM_VAR(AsmLabel, s_ws_67c384, $L_67c384)
ASM_VAR(AsmLabel, s_ws_67c37c, $L_67c37c)
ASM_VAR(AsmLabel, s_ws_67c370, $L_67c370)
ASM_VAR(AsmLabel, s_ws_67c364, $L_67c364)
ASM_VAR(AsmLabel, s_ws_67c358, $L_67c358)
ASM_VAR(AsmLabel, s_ws_67c34c, $L_67c34c)
ASM_VAR(AsmLabel, s_ws_67c340, $L_67c340)
ASM_VAR(AsmLabel, s_ws_67c334, $L_67c334)
ASM_VAR(AsmLabel, s_ws_67c328, $L_67c328)
ASM_VAR(AsmLabel, s_ws_67c31c, $L_67c31c)

struct WsMat { DWORD d[12]; };      // a matrix passed by value
struct WsPos { DWORD d[8]; };       // a PositionId passed by value

static inline BYTE* Ws_Find(BYTE* scene, const char* name)
{
    return CallC<BYTE*>(FUN_5ef2b0, scene, name);
}

static inline int Ws_Com(void* com, unsigned off)
{
    return (*(int(__stdcall**)(void*))(*(BYTE**)com + off))(com);
}

extern "C" int __cdecl Game_CreateObject(BYTE* obj, int flags);

// a network vehicle (kind 100 in multiplayer or a remote one, kind 101 in multiplayer)
static BYTE* Ws_NetVehicle(BYTE* desc, BYTE* scene)
{
    BYTE* v = (BYTE*)operator new(21880);
    if (v) {
        int d8 = F(int, desc, 8);
        Matrix m;
        WsMat placed = *(WsMat*)CallC<Matrix*>(FUN_5aba40, &m, (Matrix*)(g_ws_6d2258 + 344 + F(int, desc, 0) % 1000 * 1488),
                                               (Matrix*)(desc + 248));
        WsMat own = F(WsMat, desc, 248);
        CallThis(FUN_544920, v, g_ws_mainView, F(int, desc, 0) % 1000, scene, &own, &placed, d8, 0, 0);
        F(void*, v, 0) = g_ws_vt650250;
        F(void*, v, 16) = g_ws_vt650164;
        F(void*, v, 272) = g_ws_vt650118;
        F(void*, v, 392) = g_ws_vt650114;
    }
    return v;
}

static void Ws_NetVehicleSetup(BYTE* v, BYTE* obj, BYTE* desc)
{
    F(int, v, 360) = F(int, obj, 4);
    CallThis(FUN_53fcd0, v, F(int, obj, 4) % 1000);
    CallThis(FUN_543c00, v, F(int, desc, 0) / 1000);
    CallVirt(v + 16, 24, 1);
}

// a scene item of kinds 1-4: size bytes, constructed on the object named at desc+nameOff with the one at desc+objOff
static BYTE* Ws_Item(unsigned size, BYTE* desc, int nameOff, int objOff)
{
    BYTE* p = (BYTE*)operator new(size);
    if (p) {
        WsMat ident = *(WsMat*)(BYTE*)g_ws_7025f0;
        BYTE* s1 = Ws_Find(0, (char*)desc + nameOff);
        BYTE* s2 = Ws_Find(0, (char*)desc + objOff);
        CallThis(FUN_5a44c0, p, F(BYTE*, g_ws_wnd, 1356), s2, s1, 100, 1073741824, 0, ident, 1);
    }
    return p;
}

extern "C" {

// [$L_571f20] starts the game world (self = the world): screen memory budget, resource rows, camera settings from
// the ini file, the player (network: a placed room object; else the AI with the saved game), the marine task,
// the terrain and its three files, and the replay position
void __fastcall Game_StartWorld(BYTE* self, void*)
{
    g_ws_6f33ac = 0;
    g_ws_6f3498 = 0;
    g_ws_6f3438 = 0;
    F(BYTE*, g_ws_wnd, 1356) = g_ws_mainView;
    switch (g_ws_videoMode) {
    case 0: F(int, self, 2704) = 460800; break;
    case 1: F(int, self, 2704) = 716800; break;
    case 2: F(int, self, 2704) = 1536000; break;
    case 3: F(int, self, 2704) = 2048000; break;
    case 4: F(int, self, 2704) = 0x5dc000; break;
    }
    for (int i = 0; i < g_ws_carTypes; i++)
        CallThis(FUN_5e50d0, self + 704, CallC<int>(FUN_52ea90, i) + 200);
    BYTE* res = self + 704;
    CallThis(FUN_5e50d0, res, 300);
    CallThis(FUN_5e50d0, res, 100);
    CallThis(FUN_5e50d0, res, 101);
    CallThis(FUN_5e50d0, res, 1);
    CallThis(FUN_5e50d0, res, 2);
    CallThis(FUN_5e50d0, res, 3);
    CallThis(FUN_5e50d0, res, 4);
    g_ws_prevCreate = F(void*, self, 2708);
    F(void*, self, 2708) = (void*)Game_CreateObject;
    g_ws_prevHook2 = F(DWORD, self, 2712);
    F(void*, self, 2712) = (void*)MissionObj_Event;
    g_ws_696340 = 0;
    g_ws_6f340c = g_ws_696900 * 8;
    CallC(FUN_603cb0, (char*)s_ws_iniFile, (char*)s_ws_67c310, 10.0, &g_ws_6f32e8, g_ws_iniPath);
    CallC(FUN_603cb0, (char*)s_ws_iniFile, (char*)s_ws_67c304, 0.3, &g_ws_6f3300, g_ws_iniPath);
    CallC(FUN_603cb0, (char*)s_ws_iniFile, (char*)s_ws_67c2fc, 0.7, &g_ws_6f32f0, g_ws_iniPath);
    CallC(FUN_603cb0, (char*)s_ws_iniFile, (char*)s_ws_67c2f4, 0.1, &g_ws_6f3308, g_ws_iniPath);
    BYTE* w = g_ws_wnd;
    g_ws_6f32f8 = (F(int, w, 1316) == 0 ? F(short, g_ws_6cec9c, 404) : F(short, g_ws_6cec98, 392)) * g_ws_64b5a0;
    double farv = (F(int, w, 1316) == 0 ? F(short, g_ws_6cec9c, 402) : F(short, g_ws_6cec98, 390)) * g_ws_64b5a0;
    g_ws_6f3310 = 0;
    g_ws_6f3314 = 0;
    g_ws_6f3318 = 1;
    g_ws_6f331c = -1;
    g_ws_6f3320 = 0;
    g_ws_6f32f0 = g_ws_6f32f8 * g_ws_64b738;
    g_ws_6f3300 = farv * g_ws_64b6e0 + g_ws_64b958;

    if (F(int, w, 1300) <= 1) {
        if (F(int, w, 1316) != 0 && F(int, g_ws_6cec98, 384) >= 2) {
            // network game: the player's room object
            g_ws_6f340c = g_ws_696900;
            F(int, g_ws_mainView, 7944) = 1;
            int n = g_ws_6f340c;
            int odd = n % 2;
            unsigned __int64 bits = odd ? 0x3FF9C3FB4D12D84Aull : 0xBFF9C3FB4D12D84Aull;
            double angle = *(double*)&bits;
            float pos[3] = {(float)(odd * 40 - 20), (float)(n * 10 - 40), 0};
            Matrix mt, mr, m;
            Matrix* t = CallC<Matrix*>(FUN_5abc40, &mt, *(Vec3Bits*)pos);
            Matrix* r = CallC<Matrix*>(FUN_5ac0c0, &mr, (float)angle);
            CallC(FUN_5aba40, &m, r, t);
            strcpy(F(char*, self, 488) + 44, s_ws_roomName);
            strcpy(F(char*, self, 488) + 12, s_ws_tb);
            int id = g_ws_6f340c + 100000;
            g_ws_67b734 = id;
            int type = CallC<int>(FUN_51a4e0);
            int b = F(int, g_ws_6cec94, 812);
            CallThis(FUN_5e5130, g_ws_mainView + 704, id, (char*)s_ws_tb);
            BYTE* row = CallThis<BYTE*>(FUN_5e5370, g_ws_mainView + 704, id);
            BYTE* info = ((BYTE**)(g_ws_6d24bc + 12))[b + type * 372];
            if (info)
                F(int, row, 16) = F(int, info, 16);
            else
                Sys_Fatal(s_ws_672bb4, 0);
            void* player;
            CallThis(FUN_41eb70, &player);
            BYTE obj[300];
            CallThis(FUN_4cd670, obj, type + b * 1000, g_ws_6f340c, (char*)s_ws_roomName, &m, player, 0, 0, 0, 0);
            CallThis(FUN_5e52b0, res, id, (char*)s_ws_tb, 300, obj);
        } else {
            char buf[32];
            GetPrivateProfileStringA(s_ws_iniFile, s_ws_67c2d8, s_ws_6729a8, buf, 32, g_ws_iniPath);
            if (strcmp(buf, s_ws_672994) == 0)
                g_ws_696328 = 1;
            GetPrivateProfileStringA(s_ws_iniFile, s_ws_67c2cc, s_ws_6729a8, buf, 32, g_ws_iniPath);
            if (strcmp(buf, s_ws_672994) == 0)
                g_ws_696334 = 1;
            F(int, g_ws_mainView, 7944) = 1;
            CallC(FUN_42db90);
            w = g_ws_wnd;
            int mode = 0;
            if (F(int, w, 1316) == 1)
                mode = 3;
            else if (F(int, w, 1316) == 2)
                mode = 1;
            // the start settings: +0 a double, +8 the title, +12 the player name, +16, +20, +24 a byte flag
            BYTE set[28];
            F(double, set, 0) = 1.0;
            F(int, set, 8) = 0;
            F(void*, set, 12) = s_ws_66c874;
            F(int, set, 16) = 1;
            F(int, set, 20) = 0;
            F(BYTE, set, 24) = 0;
            F(double, set, 0) = (F(int, w, 1316) == 0 ? F(short, g_ws_6cec9c, 400) : F(short, g_ws_6cec98, 388)) * g_ws_64b5a0;
            if (F(int, w, 1316) != 0) {
                F(char*, set, 8) = CallC<char*>(FUN_51a4a0);
                w = g_ws_wnd;
            }
            F(BYTE*, set, 12) = g_ws_6cecd8 + 632;
            void* saved = 0;
            void* stg = 0;
            void* sub = 0;
            F(BYTE, set, 24) = F(int, w, 1312) != 0;
            if (g_ws_saveName[0]) {
                stg = CallStd<void*>(FUN_51bfa0, g_ws_6cecd8 + 632, 1);
                if (!stg)
                    Sys_Fatal(s_ws_676848, 0);
                (*(int(__stdcall**)(void*, wchar_t*, void*, DWORD, void*, DWORD, void**))(*(BYTE**)stg + 24))(
                    stg, CallC<wchar_t*>(FUN_50bc90, (char*)s_ws_67683c), 0, 16, 0, 0, &sub);
                if (!sub) {
                    Ws_Com(stg, 8);
                    Sys_Fatal(s_ws_67c2b0, 0);
                }
                (*(int(__stdcall**)(void*, wchar_t*, void*, DWORD, void*, DWORD, void**))(*(BYTE**)sub + 24))(
                    sub, CallC<wchar_t*>(FUN_50bc90, (char*)g_ws_saveName), 0, 16, 0, 0, &saved);
                if (!saved)
                    Sys_Fatal(s_ws_67c298, 0);
                CallC(FUN_571d20, saved);
            }
            CallC(FUN_434780, saved, mode, set);
            if (saved)
                Ws_Com(saved, 8);
            if (stg)
                Ws_Com(stg, 8);
            CallThis(FUN_5291a0, g_ws_6ce4f8);
        }
    }

    CallC(FUN_4e0660, self);
    CallThis(FUN_5d2540, g_ws_71e820, (char*)s_ws_67c280, (void*)Terrain_FollowCamera);
    CallThis(FUN_5d2540, g_ws_71e820, (char*)s_ws_67c268, (void*)g_ws_569550);
    BYTE* marine = (BYTE*)operator new(160);
    g_ws_marine = marine ? CallThis<BYTE*>(FUN_60e0f0, marine, g_ws_mainView) : 0;
    const char* dir = F(int, g_ws_wnd, 1316) ? s_ws_676870 : s_ws_676880;
    FILE* files[3];
    const char* names[3] = {s_ws_67c258, s_ws_67c23c, s_ws_67c22c};
    for (int k = 0; k < 3; k++) {
        char* path = CallThis<char*>(FUN_5dc2d0, F(BYTE*, g_ws_wnd, 1356), dir, names[k]);
        files[k] = fopen(path, s_ws_rb);
        if (!files[k])
            Sys_Fatal(path, s_ws_67c24c);
    }
    BYTE* old = g_ws_terrain;
    if (old) {
        CallThis(FUN_6148a0, old);
        operator delete(old);
    }
    BYTE* t = (BYTE*)operator new(112);
    if (t) {
        DWORD v[3] = {1163780096, 3314327552u, 0};      // 3550, -4480, 0
        t = CallThis<BYTE*>(FUN_6142f0, t, v[0], v[1], v[2], 1161527296, 1161527296, files, 3, g_ws_device);
    }
    g_ws_terrain = t;
    CallThis(FUN_5c4910, self);
    if (F(int, g_ws_wnd, 1300) > 1) {
        FILE* f = F(FILE*, self, 7928);
        F(int, self, 7960) = 0;
        F(int, self, 7932) = 1;
        F(int, self, 7964) = 1072693248;
        F(int, self, 7940) = 0;
        fseek(f, -8, 2);
        g_ws_6f34d4 = CallC<int>(FUN_5ab140, F(FILE*, self, 7928));
        fseek(F(FILE*, self, 7928), 0, 0);
        CallVirt(self, 216);
    }
    g_ws_created = 0;
}

// $L_5728f0: the world's object creation hook (obj +4 kind*1000+n, +12 world, +24 description, +28 the created
// object); flags bit 0: create, else only mark the player. The hook it replaced runs first.
int __cdecl Game_CreateObject(BYTE* obj, int flags)
{
    if (g_ws_prevCreate && ((int(__cdecl*)(BYTE*, int))g_ws_prevCreate)(obj, flags))
        return 1;
    int kind = F(int, obj, 4) / 1000;
    if (kind == 100) {
        BYTE* desc = F(BYTE*, obj, 24);
        if (!(flags & 1)) {
            if (F(int, desc, 296) != g_ws_noPlayer) {
                int id = F(int, desc, 296);
                int f = CallC<int>(FUN_43f620, id);
                CallC(FUN_43f690, id, f | 16);
            }
            return 1;
        }
        if (F(int, g_ws_wnd, 1300) > 1 || F(int, desc, 12) == 2) {
            BYTE* scene = Ws_Find(0, (char*)desc + 16);
            BYTE* v = Ws_NetVehicle(desc, scene);
            Ws_NetVehicleSetup(v, obj, desc);
            if (F(int, g_ws_wnd, 1300) > 1 && F(int, desc, 12))
                g_ws_696340 = v;
            F(BYTE*, obj, 28) = v;
            g_ws_created++;
            return 1;
        }
        BYTE* scene = Ws_Find(0, (char*)desc + 16);
        if (F(int, desc, 296) != g_ws_noPlayer) {
            BYTE pd[200];
            CallC(FUN_43fc40, F(int, desc, 296), pd);
            if (F(int, pd, 8) != 1) {
                WsPos pos;
                CallC(FUN_437520, &pos, F(int, desc, 296));
                // the room info: +20 the room object's name
                BYTE room[44];
                F(int, room, 0) = 0;
                F(int, room, 8) = 0;
                F(int, room, 12) = 0;
                F(void*, room, 16) = s_ws_66d638;
                F(void*, room, 20) = s_ws_66d628;
                F(int, room, 24) = 0;
                F(int, room, 28) = 0;
                F(int, room, 36) = -1;
                F(int, room, 40) = -1;
                CallC(FUN_438d70, pos, room);
                BYTE* roomObj = Ws_Find(0, F(char*, room, 20));
                BYTE orient[64];
                BYTE* o = CallC<BYTE*>(FUN_43a780, orient, pos);
                float d[3];
                d[2] = (float)((double)g_ws_702638 + F(float, o, 44));
                d[1] = (float)((double)g_ws_702634 + F(float, o, 40));
                d[0] = (float)((double)g_ws_702630 + F(float, o, 36));
                o = CallC<BYTE*>(FUN_43a780, orient, pos);
                void* hit = CallC<void*>(FUN_4e5ba7, roomObj, o + 36, d);
                hit = CallC<void*>(FUN_4e5ba7, hit, d, desc + 284);
                if (hit)
                    scene = (BYTE*)hit;
            }
        }
        if (!scene) {
            char buf[200];
            BYTE* world = F(BYTE*, obj, 12);
            sprintf(buf, s_ws_67c3b8, desc + 16, F(int, world, 4016), F(int, world, 4012), (double)F(float, desc, 284),
                    (double)F(float, desc, 288));
            Sys_Fatal(s_ws_67c3a8, buf);
        }
        BYTE* fx = (BYTE*)operator new(16584);
        if (fx) {
            WsMat m = F(WsMat, desc, 248);
            int e = F(int, desc, 0) % 1000;
            fx = CallThis<BYTE*>(FUN_4d2790, fx, e, scene, m);
        }
        F(int, fx, 16492) = 1;
        g_ws_67f878 = F(int, obj, 4);
        BYTE* v = CallC<BYTE*>(FUN_4d08d0, g_ws_mainView, fx, F(int, desc, 296));
        int pl = F(int, desc, 296);
        if (pl != g_ws_noPlayer && F(int, (BYTE*)pl, 84) != F(int, obj, 4))
            Sys_Fatal(s_ws_67c398, 0);
        CallThis(FUN_543c00, v, F(int, desc, 0) / 1000);
        CallThis(FUN_546900, v, desc + 48);
        CallThis(FUN_53fcd0, v, F(int, obj, 4) % 1000);
        F(BYTE*, obj, 28) = v;
        g_ws_created++;
        return 1;
    }
    if (kind == 101) {
        BYTE* desc = F(BYTE*, obj, 24);
        if (!(flags & 1))
            return 0;
        BYTE* v;
        if (F(int, g_ws_wnd, 1300) > 1) {
            BYTE* scene = Ws_Find(0, (char*)desc + 16);
            v = Ws_NetVehicle(desc, scene);
            Ws_NetVehicleSetup(v, obj, desc);
        } else {
            BYTE* car = (BYTE*)operator new(16424);
            if (car) {
                WsMat m = F(WsMat, desc, 248);
                int e = F(int, desc, 0) % 1000;
                car = CallThis<BYTE*>(FUN_4ebd00, car, Ws_Find(0, (char*)desc + 16), m, e);
            }
            g_ws_67f878 = F(int, obj, 4);
            v = (BYTE*)operator new(21952);
            if (v)
                v = CallThis<BYTE*>(FUN_4d0b10, v, g_ws_mainView, car, g_ws_noPlayer);
            CallThis(FUN_543c00, v, F(int, desc, 0) / 1000);
            CallThis(FUN_546900, v, desc + 48);
            CallThis(FUN_53fcd0, v, F(int, obj, 4) % 1000);
        }
        F(BYTE*, obj, 28) = v;
        return 0;
    }
    if (kind == 1 || kind == 2) {
        BYTE* desc = F(BYTE*, obj, 24);
        BYTE* p = Ws_Item(416, desc, 44, 12);
        if (p) {
            F(void*, p, 0) = kind == 1 ? g_ws_vt6500e4 : g_ws_vt650068;
            F(void*, p, 256) = kind == 1 ? g_ws_vt650098 : g_ws_vt65001c;
            F(int, p, 356) = F(int, desc, 0);
            F(int, p, 408) = F(int, desc, 4);
            strcpy((char*)p + 376, (char*)desc + 44);
            if (kind == 1)
                F(BYTE*, p, 352) = Ws_Find(Ws_Find(0, s_ws_67c384), s_ws_67c390);
            else
                F(BYTE*, p, 352) = Ws_Find(Ws_Find(0, s_ws_67c370), s_ws_67c37c);
            F(int, p, 360) = F(int, desc, 8);
        }
        F(int, p, 344) = F(int, obj, 4);
        F(BYTE*, obj, 28) = p;
        return 1;
    }
    BYTE* p;
    if (kind == 3) {
        BYTE* desc = F(BYTE*, obj, 24);
        p = Ws_Item(416, desc, 40, 8);
        if (p) {
            F(void*, p, 0) = g_ws_vt64ffec;
            F(void*, p, 256) = g_ws_vt64ffa0;
            F(int, p, 356) = F(int, desc, 0);
            F(int, p, 408) = 0;
            strcpy((char*)p + 376, (char*)desc + 40);
            F(BYTE*, p, 352) = Ws_Find(Ws_Find(0, s_ws_67c358), s_ws_67c364);
            F(int, p, 360) = F(int, desc, 4);
        }
    } else if (kind == 4) {
        BYTE* desc = F(BYTE*, obj, 24);
        p = Ws_Item(424, desc, 44, 12);
        if (p) {
            F(void*, p, 0) = g_ws_vt64ff70;
            F(void*, p, 256) = g_ws_vt64ff24;
            F(int, p, 368) = F(int, desc, 0);
            F(int, p, 416) = F(int, desc, 4);
            strcpy((char*)p + 384, (char*)desc + 44);
            F(BYTE*, p, 352) = Ws_Find(Ws_Find(0, s_ws_67c340), s_ws_67c34c);
            F(BYTE*, p, 356) = Ws_Find(Ws_Find(0, s_ws_67c340), s_ws_67c334);
            F(BYTE*, p, 360) = Ws_Find(Ws_Find(0, s_ws_67c340), s_ws_67c328);
            F(BYTE*, p, 364) = Ws_Find(Ws_Find(0, s_ws_67c340), s_ws_67c31c);
            F(int, p, 372) = F(int, desc, 8);
        }
    } else
        return 0;
    CallVirt(p, 24, 1);
    F(int, p, 344) = F(int, obj, 4);
    F(BYTE*, obj, 28) = p;
    return 1;
}

}  // extern "C"

// === copter screen start (FUN_56e5a0)

ASM_VAR(AsmLabel, s_cs67c164, $L_67c164)
ASM_VAR(AsmLabel, s_cs67c150, $L_67c150)
ASM_VAR(AsmLabel, s_cs67c124, $L_67c124)
ASM_VAR(AsmLabel, s_cs67c110, $L_67c110)
ASM_VAR(AsmLabel, s_cs67c0fc, $L_67c0fc)
ASM_VAR(AsmLabel, s_cs67c0e8, $L_67c0e8)
ASM_VAR(AsmLabel, s_cs67c0d4, $L_67c0d4)
ASM_VAR(AsmLabel, s_cs67c0c0, $L_67c0c0)
ASM_VAR(AsmLabel, s_cs67c0ac, $L_67c0ac)
ASM_VAR(AsmLabel, s_cs67c098, $L_67c098)
ASM_VAR(AsmLabel, s_cs67c084, $L_67c084)
ASM_VAR(AsmLabel, s_cs67c070, $L_67c070)
ASM_VAR(AsmLabel, s_cs67c05c, $L_67c05c)
ASM_VAR(AsmLabel, s_cs67c048, $L_67c048)
ASM_VAR(AsmLabel, s_cs67c034, $L_67c034)
ASM_VAR(AsmLabel, s_cs67c024, $L_67c024)
ASM_VAR(AsmLabel, s_cs67c014, $L_67c014)
ASM_VAR(AsmLabel, s_cs67c004, $L_67c004)
ASM_VAR(AsmLabel, s_cs67bff0, $L_67bff0)
ASM_VAR(AsmLabel, s_cs67bfe0, $L_67bfe0)
ASM_VAR(AsmLabel, s_cs67bfd0, $L_67bfd0)
ASM_VAR(AsmLabel, s_cs67bfc0, $L_67bfc0)
ASM_VAR(AsmLabel, s_cs67bfb0, $L_67bfb0)
ASM_VAR(AsmLabel, s_cs67bfa0, $L_67bfa0)
ASM_VAR(AsmLabel, s_cs67bf90, $L_67bf90)
ASM_VAR(AsmLabel, s_cs67bf80, $L_67bf80)
ASM_VAR(AsmLabel, s_cs67bf70, $L_67bf70)
ASM_VAR(AsmLabel, s_cs67bf60, $L_67bf60)
ASM_VAR(AsmLabel, s_cs67bf4c, $L_67bf4c)
ASM_VAR(AsmLabel, s_cs67bf38, $L_67bf38)
ASM_VAR(AsmLabel, s_cs67bf0c, $L_67bf0c)
ASM_VAR(AsmLabel, s_cs67bef8, $L_67bef8)
ASM_VAR(AsmLabel, s_cs67bee4, $L_67bee4)
ASM_VAR(AsmLabel, s_cs67bed0, $L_67bed0)
ASM_VAR(AsmLabel, s_cs67bebc, $L_67bebc)
ASM_VAR(AsmLabel, s_cs67bea8, $L_67bea8)
ASM_VAR(AsmLabel, s_cs67be94, $L_67be94)
ASM_VAR(AsmLabel, s_cs67be80, $L_67be80)
ASM_VAR(AsmLabel, s_cs67be6c, $L_67be6c)
ASM_VAR(AsmLabel, s_cs67be58, $L_67be58)
ASM_VAR(AsmLabel, s_cs67be44, $L_67be44)
ASM_VAR(AsmLabel, s_cs67be30, $L_67be30)
ASM_VAR(AsmLabel, s_cs67be1c, $L_67be1c)
ASM_VAR(AsmLabel, s_cs67be0c, $L_67be0c)
ASM_VAR(AsmLabel, s_cs67bdfc, $L_67bdfc)
ASM_VAR(AsmLabel, s_cs67bdec, $L_67bdec)
ASM_VAR(AsmLabel, s_cs67bdd8, $L_67bdd8)
ASM_VAR(AsmLabel, s_cs67bdc8, $L_67bdc8)
ASM_VAR(AsmLabel, s_cs67bdb8, $L_67bdb8)
ASM_VAR(AsmLabel, s_cs67bda8, $L_67bda8)
ASM_VAR(AsmLabel, s_cs67bd98, $L_67bd98)
ASM_VAR(AsmLabel, s_cs67bd88, $L_67bd88)
ASM_VAR(AsmLabel, s_cs67bd78, $L_67bd78)
ASM_VAR(AsmLabel, s_cs67bd68, $L_67bd68)
ASM_VAR(AsmLabel, s_cs67bd58, $L_67bd58)
ASM_VAR(AsmLabel, s_cs67bd48, $L_67bd48)
ASM_VAR(AsmLabel, g_cs6f332c, $L_6f332c)
ASM_VAR(AsmLabel, g_cs6f32e4, $L_6f32e4)
ASM_VAR(AsmLabel, g_cs6f3330, $L_6f3330)
ASM_VAR(AsmLabel, g_cs6f2df4, $L_6f2df4)
ASM_VAR(AsmLabel, g_cs6f2de4, $L_6f2de4)
ASM_VAR(AsmLabel, g_cs6f2de8, $L_6f2de8)
ASM_VAR(AsmLabel, g_cs6f2dec, $L_6f2dec)
ASM_VAR(AsmLabel, g_cs6f2df0, $L_6f2df0)
ASM_VAR(AsmLabel, g_cs6f3258, $L_6f3258)
ASM_VAR(AsmLabel, g_cs6f325c, $L_6f325c)
ASM_VAR(AsmLabel, g_cs6f3260, $L_6f3260)
ASM_VAR(AsmLabel, g_cs6f3264, $L_6f3264)
ASM_VAR(AsmLabel, g_cs6f3380, $L_6f3380)
ASM_VAR(AsmLabel, g_cs6f2c50, $L_6f2c50)
ASM_VAR(AsmLabel, g_cs6f337c, $L_6f337c)
ASM_VAR(AsmLabel, g_cs6f2d08, $L_6f2d08)
ASM_VAR(AsmLabel, g_cs6f2d14, $L_6f2d14)
ASM_VAR(AsmLabel, g_cs6f3378, $L_6f3378)
ASM_VAR(AsmLabel, g_cs6f3268, $L_6f3268)
ASM_VAR(AsmLabel, g_cs6f33a8, $L_6f33a8)
ASM_VAR(AsmLabel, g_cs6f2e38, $L_6f2e38)
ASM_VAR(AsmLabel, g_cs6f2e3c, $L_6f2e3c)
ASM_VAR(AsmLabel, g_cs6f2e40, $L_6f2e40)
ASM_VAR(AsmLabel, g_cs6f2e44, $L_6f2e44)
ASM_VAR(AsmLabel, g_cs6f2e48, $L_6f2e48)
ASM_VAR(AsmLabel, g_cs6f2e4c, $L_6f2e4c)
ASM_VAR(AsmLabel, g_cs6f2e50, $L_6f2e50)
ASM_VAR(AsmLabel, g_cs6f2e54, $L_6f2e54)
static char* const k_csNames2[] = {s_cs67c164, s_cs67c150, s_cs67c124, s_cs67c110, s_cs67c0fc, s_cs67c0e8, s_cs67c0d4, s_cs67c0c0, s_cs67c0ac, s_cs67c098, s_cs67c084, s_cs67c070, s_cs67c05c, s_cs67c048, s_cs67c034, s_cs67c024, s_cs67c014, s_cs67c004, s_cs67bff0, s_cs67bfe0, s_cs67bfd0, s_cs67bfc0, s_cs67bfb0, s_cs67bfa0, s_cs67bf90, s_cs67bf80, s_cs67bf70, s_cs67bf60};
static char* const k_csNames[] = {s_cs67bf4c, s_cs67bf38, s_cs67bf0c, s_cs67bef8, s_cs67bee4, s_cs67bed0, s_cs67bebc, s_cs67bea8, s_cs67be94, s_cs67be80, s_cs67be6c, s_cs67be58, s_cs67be44, s_cs67be30, s_cs67be1c, s_cs67be0c, s_cs67bdfc, s_cs67bdec, s_cs67bdd8, s_cs67bdc8, s_cs67bdb8, s_cs67bda8, s_cs67bd98, s_cs67bd88, s_cs67bd78, s_cs67bd68, s_cs67bd58, s_cs67bd48};
static char* const k_csSlots[] = {g_cs6f332c, g_cs6f32e4, g_cs6f3330, g_cs6f2df4, g_cs6f2de4, g_cs6f2de8, g_cs6f2dec, g_cs6f2df0, g_cs6f3258, g_cs6f325c, g_cs6f3260, g_cs6f3264, g_cs6f3380, g_cs6f2c50, g_cs6f337c, g_cs6f2d08, g_cs6f2d14, g_cs6f3378, g_cs6f3268, g_cs6f33a8, g_cs6f2e38, g_cs6f2e3c, g_cs6f2e40, g_cs6f2e44, g_cs6f2e48, g_cs6f2e4c, g_cs6f2e50, g_cs6f2e54};
ASM_VAR(AsmLabel, s_cs67c138, $L_67c138)         // "...%d%d" mode 2 frame names
ASM_VAR(AsmLabel, s_cs67bf20, $L_67bf20)
ASM_VAR(AsmLabel, g_csFrames, $L_6f2f0c)          // 24 x 6 frames
ASM_VAR(AsmLabel, s_cs672998, $L_672998)
ASM_VAR(AsmLabel, s_cs675dd8, $L_675dd8)
ASM_VAR(AsmLabel, s_cs6729a8, $L_6729a8)
ASM_VAR(AsmLabel, s_cs67bd40, $L_67bd40)
ASM_VAR(AsmLabel, s_cs67bd18, $L_67bd18)
ASM_VAR(AsmLabel, s_cs67bd2c, $L_67bd2c)
ASM_VAR(AsmLabel, s_cs67bbdc, $L_67bbdc)
ASM_VAR(AsmLabel, g_csVtbl64fe80, $L_64fe80)
ASM_VAR(AsmLabel, g_csTv6f68b0, $L_6f68b0)
ASM_VAR(char*, g_csIniPath, $L_696868)
ASM_VAR(int, g_cs67b734, $L_67b734)
ASM_VAR(int, g_cs696340, $L_696340)
ASM_VAR(int, g_cs696328, $L_696328)
ASM_VAR(BYTE*, g_cs696338, $L_696338)
ASM_VAR(BYTE*, g_csShell, $L_6cec6c)
ASM_VAR(BYTE*, g_cs6cec74, $L_6cec74)
ASM_VAR(BYTE*, g_cs6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_cs6cecb4, $L_6cecb4)
ASM_VAR(BYTE*, g_cs6d1f10, $L_6d1f10)
ASM_VAR(BYTE*, g_csGame, $L_6d2078)
ASM_VAR(BYTE*, g_cs6ef1f8, $L_6ef1f8)
ASM_VAR(int, g_csNoCar, $L_68b990)
ASM_VAR(int, g_cs6f331c, $L_6f331c)
ASM_VAR(int, g_cs6f3320, $L_6f3320)
ASM_VAR(int, g_cs6f33b8, $L_6f33b8)
ASM_VAR(int, g_cs6f33bc, $L_6f33bc)
ASM_VAR(int, g_cs6f3404, $L_6f3404)
ASM_VAR(int, g_cs6f34b0, $L_6f34b0)
ASM_VAR(BYTE*, g_cs6f3430, $L_6f3430)
ASM_VAR(BYTE*, g_cs6f3434, $L_6f3434)
ASM_VAR(BYTE*, g_cs6f3438, $L_6f3438)
ASM_VAR(BYTE*, g_cs6f33ec, $L_6f33ec)
ASM_VAR(AsmLabel, g_cs6f3338, $L_6f3338)
ASM_VAR(AsmLabel, s_cs67bd04, $L_67bd04)
ASM_VAR(AsmLabel, s_cs67bcf0, $L_67bcf0)
ASM_VAR(AsmLabel, s_cs67bcdc, $L_67bcdc)
ASM_VAR(AsmLabel, s_cs67bcc8, $L_67bcc8)
ASM_VAR(AsmLabel, s_cs67bcb4, $L_67bcb4)
ASM_VAR(AsmLabel, s_cs67bca0, $L_67bca0)
ASM_VAR(AsmLabel, s_cs67bc8c, $L_67bc8c)
ASM_VAR(AsmLabel, s_cs67bc78, $L_67bc78)
ASM_VAR(AsmLabel, s_cs67bc68, $L_67bc68)
ASM_VAR(AsmLabel, s_cs67bc58, $L_67bc58)
ASM_VAR(AsmLabel, s_cs67bc48, $L_67bc48)
ASM_VAR(AsmLabel, s_cs67bc38, $L_67bc38)
ASM_VAR(AsmLabel, s_cs67bc20, $L_67bc20)
ASM_VAR(AsmLabel, s_cs67bc08, $L_67bc08)
ASM_VAR(AsmLabel, s_cs67bbf8, $L_67bbf8)
ASM_VAR(AsmLabel, s_cs67bbe8, $L_67bbe8)
ASM_VAR(AsmLabel, g_cs6f33c4, $L_6f33c4)
static char* const k_csHud[8][2] = {
    {s_cs67bd04, s_cs67bcf0}, {s_cs67bcdc, s_cs67bcc8}, {s_cs67bcb4, s_cs67bca0}, {s_cs67bc8c, s_cs67bc78},
    {s_cs67bc68, s_cs67bc58}, {s_cs67bc48, s_cs67bc38}, {s_cs67bc20, s_cs67bc08}, {s_cs67bbf8, s_cs67bbe8},
};
extern "C" void FUN_541960(); KING_ALIAS("_FUN_541960", "@Obj541960_Self@8")
extern "C" void FUN_4d8ae0(); KING_ALIAS("_FUN_4d8ae0", "_Vehicle_Release")
extern "C" void FUN_4d8b20(); KING_ALIAS("_FUN_4d8b20", "_Vehicle_LoadResource")
extern "C" void FUN_531cc0(); KING_ALIAS("_FUN_531cc0", "@Viewer_AttachVehicle@12")
extern "C" void FUN_55f4e0(); KING_ALIAS("_FUN_55f4e0", "_Vehicle_FindTaker")
extern "C" void FUN_56f6d0(); KING_ALIAS("_FUN_56f6d0", "_Copter_ApplyCargo")
extern "C" void FUN_4ebaa0(); KING_ALIAS("_FUN_4ebaa0", "@Engine_SetKeys@16")
extern "C" void FUN_598da0(); KING_ALIAS("_FUN_598da0", "@Wasm_Get108@8")
extern "C" void FUN_597570(); KING_ALIAS("_FUN_597570", "@WorldObj_Attach@12")
extern "C" void FUN_5975e0(); KING_ALIAS("_FUN_5975e0", "@SceneObj_Place@20")
extern "C" void FUN_597a20(); KING_ALIAS("_FUN_597a20", "@TvCamera_CreateMonitor@8")
extern "C" void FUN_598bd0(); KING_ALIAS("_FUN_598bd0", "@WorldObj_SetTarget@12")
extern "C" void FUN_56afe0(); KING_ALIAS("_FUN_56afe0", "@Notice_Set@16")
extern "C" void FUN_4ce0d0(); KING_ALIAS("_FUN_4ce0d0", "@Effect4ce0d0_Construct@20")
extern "C" void FUN_56d810(); KING_ALIAS("_FUN_56d810", "_CopterWorld_Setup")
extern "C" void FUN_56d7d0(); KING_ALIAS("_FUN_56d7d0", "_Flags_Set6ce37c")
extern "C" void FUN_483990(); KING_ALIAS("_FUN_483990", "_AI_StartXCycleTask")
extern "C" BYTE* __fastcall CopterTask_Construct(BYTE* self, void*, BYTE* v);
extern "C" BYTE* __fastcall CopterScreen_Construct(BYTE* self, int, BYTE* game);

static BYTE* CopterStart_Find(const char* name)
{
    return CallC<BYTE*>(FUN_5ef3d0, name);
}

extern "C" {

// [FUN_56e5a0] starts the copter screen for the vehicle v: resets the shell counters, takes the player's car,
// finds the screen's pictures and sounds (another set in game mode 2), sets up the TV camera, the notice, the
// message objects and the effect, and starts the AI cycle task.
void __cdecl CopterScreen_Start(BYTE* v)
{
    char* iniPath = g_csIniPath;
    g_cs6f33b8 = g_cs6f331c;
    g_cs6f33bc = g_cs6f3320;
    F(int, g_csShell, 1308) = 0;
    F(int, g_csShell, 1292) = 0;
    F(int, g_csShell, 1288) = 0;
    F(int, g_csShell, 1284) = 0;
    F(int, g_csShell, 1296) = 0;
    g_cs6f3404 = 0;
    g_cs6f34b0 = 0;
    if (g_cs696340 == 0) {
        BYTE* o = g_cs6ef1f8 ? g_cs6ef1f8 - 4 : 0;
        BYTE* r = CallThis<BYTE*>(FUN_541960, o);
        CallC(FUN_4d8ae0, F(int, r, 360));
        CallC(FUN_4d8b20, g_cs67b734);
    }
    CallThis(FUN_531cc0, g_csGame, g_cs696340);
    int id;
    if (*CallC<int*>(FUN_568f60, &id) != g_csNoCar) {
        BYTE info[192];
        int car;
        car = *CallC<int*>(FUN_568f60, &car);
        CallC(FUN_43fc40, car, info);
        if (F(int, info, 24) >= 0)
            CallC(FUN_55f4e0, F(BYTE*, g_csGame, 616), 0);
    }
    CallC(FUN_56f6d0);
    if (F(short, g_cs6cec74, 396))
        F(int, g_csGame, 1404) |= 0x100;
    else
        F(int, g_csGame, 1404) &= ~0x100;
    if (g_cs696328)
        CallThis(FUN_4ebaa0, F(BYTE*, F(BYTE*, g_csGame, 616), 21600) + 9984, 1, 1);

    int mode2 = CallC<int>(FUN_568fd0) == 2;
    char* const* names = mode2 ? k_csNames2 : k_csNames;
    const int n = sizeof k_csSlots / sizeof k_csSlots[0];
    for (int i = 0; i < 2; i++)
        *(BYTE**)k_csSlots[i] = CopterStart_Find(names[i]);
    BYTE** frame = (BYTE**)(char*)g_csFrames;
    for (int a = 0; a < 24; a++)
        for (int b = 0; b < 6; b++) {
            char buf[256];
            sprintf(buf, mode2 ? s_cs67c138 : s_cs67bf20, a, b);
            *frame++ = CopterStart_Find(buf);
        }
    for (int i = 2; i < n; i++)
        *(BYTE**)k_csSlots[i] = CopterStart_Find(names[i]);

    BYTE* tv = CallThis<BYTE*>(FUN_598da0, g_csTv6f68b0);
    F(double, F(BYTE*, tv, 472), 40) = 0.2;
    F(double, F(BYTE*, v, 472), 40) = 0.2;
    char value[60];
    GetPrivateProfileStringA(s_cs672998, s_cs675dd8, s_cs6729a8, value, 60, iniPath);
    CallThis(FUN_597570, g_csTv6f68b0, g_cs696340);
    BYTE* p488 = F(BYTE*, v, 488);
    int target = p488 ? F(int, p488, 76) : 0;
    BYTE cam[48];
    BYTE* c = CallThis<BYTE*>(FUN_5dbcc0, v, cam);
    CallThis(FUN_5975e0, g_csTv6f68b0, target, c, 0);
    CallThis(FUN_597a20, g_csTv6f68b0);
    CallThis(FUN_598bd0, g_csTv6f68b0, 1);
    if (F(int, g_csShell, 1300) > 1 && F(int, g_csGame, 1400) != 0)
        *g_cs6d1f10 = 1;

    BYTE* p = (BYTE*)operator new(64);
    p = p ? CopterTask_Construct(p, 0, v) : 0;
    g_cs6f3430 = p;
    if (F(int, g_csShell, 1316) == 2)
        CallThis(FUN_56afe0, p, 2, 0);

    BYTE* o = (BYTE*)operator new(40);
    if (o) {
        CallThis(FUN_5d31e0, o, 9, (char*)s_cs67bd40);
        F(void*, o, 0) = g_csVtbl64fe80;
        F(int, o, 32) = -1;
        F(int, o, 36) = -1;
        CallThis(FUN_5d3570, v + 2992, o);
        CallThis(FUN_5d33d0, o, 1);
    }
    g_cs6f3434 = o;

    p = (BYTE*)operator new(1296);
    g_cs6f3438 = p ? CopterScreen_Construct(p, 0, v) : 0;

    p = (BYTE*)operator new(48);
    p = p ? CallThis<BYTE*>(FUN_4ce0d0, p, v, (char*)s_cs67bd18, (char*)s_cs67bd2c) : 0;
    g_cs696338 = p;
    CallVirt(p, 24, 1);
    CallC(FUN_56d810, v);
    CallC(FUN_56d7d0, 882);
    if (!(F(int, g_csShell, 1300) > 1)
        && (F(int, g_csShell, 1316) == 0 || F(int, g_cs6cec98, 384) != 2))
        CallC(FUN_483990);

    for (int i = 0; i < 8; i++)
        ((BYTE**)(char*)g_cs6f33c4)[i] = CopterStart_Find(k_csHud[i][CallC<int>(FUN_568fd0) == 2 ? 0 : 1]);
    g_cs6f33ec = CopterStart_Find(s_cs67bbdc);
    memset(g_cs6f3338, 0, 64);
    if (F(int, g_csShell, 1316) == 1)
        CallVirt(g_cs6cecb4, 36);
}

} // extern "C"

// === copter screen panels (FUN_56bc80)

ASM_VAR(BYTE*, g_cp696ca0, $L_696ca0)
ASM_VAR(int, g_cpX, $L_6ced14)
ASM_VAR(int, g_cpY, $L_6ced18)
ASM_VAR(int, g_cpX2, $L_6ced1c)
ASM_VAR(int, g_cpY2, $L_6ced20)
ASM_VAR(BYTE*, g_cpFont, $L_6ced24)
ASM_VAR(BYTE*, g_cpFont2, $L_6ced2c)
ASM_VAR(BYTE*, g_cpImages, $L_6cecbc)
ASM_VAR(BYTE*, g_cpImg6f33c4, $L_6f33c4)
ASM_VAR(BYTE*, g_cpImg6f33c8, $L_6f33c8)
ASM_VAR(BYTE*, g_cpImg6f33cc, $L_6f33cc)
ASM_VAR(BYTE*, g_cpImg6f33d0, $L_6f33d0)
ASM_VAR(BYTE, g_cp6f2dd8, $L_6f2dd8)
ASM_VAR(char*, g_cp6f2da4, $L_6f2da4)
ASM_VAR(double, g_cp6f2da8, $L_6f2da8)
ASM_VAR(int, g_cp6f2db0, $L_6f2db0)
ASM_VAR(int, g_cp6f2db4, $L_6f2db4)
ASM_VAR(double, g_cp6f2db8, $L_6f2db8)
ASM_VAR(int, g_cp6f2dc0, $L_6f2dc0)
ASM_VAR(int, g_cp6f2dd0, $L_6f2dd0)
ASM_VAR(int, g_cp6f2dd4, $L_6f2dd4)
ASM_VAR(int, g_cp6f2f08, $L_6f2f08)
ASM_VAR(int, g_cp68cf6c, $L_68cf6c)
ASM_VAR(int, g_cpNoCar, $L_68b990)
ASM_VAR(BYTE*, g_cpNotice, $L_6f3430)
ASM_VAR(AsmLabel, g_cp7025f0, $L_7025f0)
ASM_VAR(double, g_cpMsec, $L_64b658)
ASM_VAR(double, g_cpOne, $L_64b670)
ASM_VAR(double, g_cp64b6a0, $L_64b6a0)
ASM_VAR(AsmLabel, s_cp67ba18, $L_67ba18)
ASM_VAR(AsmLabel, s_cp66c8cc, $L_66c8cc)
ASM_VAR(AsmLabel, s_cp67ba10, $L_67ba10)
ASM_VAR(AsmLabel, s_cp67ba08, $L_67ba08)
ASM_VAR(AsmLabel, s_cp67ba00, $L_67ba00)
ASM_VAR(AsmLabel, s_cp67b8f8, $L_67b8f8)
ASM_VAR(AsmLabel, s_cp677a74, $L_677a74)
extern "C" void FUN_4d06a0(); KING_ALIAS("_FUN_4d06a0", "@Fx_InitState@8")
extern "C" void FUN_41be60(); KING_ALIAS("_FUN_41be60", "_Ai_GetPresentData")
extern "C" void FUN_4401d0(); KING_ALIAS("_FUN_4401d0", "_Ai_GetMerchandiseProperties")
extern "C" void FUN_441950(); KING_ALIAS("_FUN_441950", "_Ai_GetOrderInfo")
extern "C" void FUN_440f70(); KING_ALIAS("_FUN_440f70", "_Ai_GetTownInfo")
extern "C" void FUN_43f8d0(); KING_ALIAS("_FUN_43f8d0", "_Player_PlaceField20")
extern "C" void FUN_576c00(); KING_ALIAS("_FUN_576c00", "@Obj576c00_DeletingDestructor@12")
extern "C" void FUN_44b090(); KING_ALIAS("_FUN_44b090", "@Vec1_Free@8")
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")

static __declspec(noinline) int Cp_Int(double d)
{
    return (int)d;
}

static int Cp_TimeOdd(double k)
{
    return Cp_Int(F(double, g_cp696ca0, 2968) * k) % 2 != 0;
}

static void Cp_Text(int x, int y, BYTE* font, const char* text, int align = 5)
{
    CallC(FUN_5ea170, F(BYTE*, g_cp696ca0, 12), x, y, font, text, align, 0);
}

static void Cp_Image(int x, int y, BYTE* image)
{
    CallC(FUN_5b86c0, F(BYTE*, g_cp696ca0, 12), x, y, image);
}

static const char* Cp_Str(int id)
{
    return CallC<const char*>(FUN_5d2e50, id);
}

static void Cp_Notice(int text)
{
    BYTE* n = g_cpNotice;
    if (n && F(int, n, 60) == 0)
        CallThis(FUN_56afe0, n, 8, Cp_Str(text));
}

// the lines of an order info below its name at (x, y): price, time, rates and damage
static void Cp_OrderLines(int x, int y, BYTE* oi, BYTE* mp, char* buf, const char* priceFmt, const char* timeFmt)
{
    sprintf(buf, priceFmt, Cp_Int(F(double, oi, 120)));
    Cp_Text(x, y + 55, g_cpFont, buf);
    int t = Cp_Int(F(double, oi, 136));
    sprintf(buf, timeFmt, t / 60, t % 60);
    Cp_Text(x, y + 75, g_cpFont, buf);
    sprintf(buf, Cp_Str(70), F(double, oi, 144) * g_cpMsec);
    Cp_Text(x, y + 95, g_cpFont, buf);
    sprintf(buf, Cp_Str(71), F(int, oi, 152), F(int, oi, 156));
    Cp_Text(x, y + 115, g_cpFont, buf);
    sprintf(buf, s_cp67ba08, Cp_Int((g_cpOne - F(double, mp, 32)) * g_cp64b6a0));
    Cp_Text(x, y + 135, g_cpFont, buf);
}

extern "C" {

// [FUN_56bc80] draws the copter screen's panel of mode +32 (17 an order, 18 the player's state, 19 a town's
// orders, 20/25 a present, 24 another vehicle) for the id at +36, then clears the mode
void __fastcall CopterPanel_Draw(BYTE* self, void*)
{
    char buf[136];
    switch (F(int, self, 32)) {
    case 18: {
        if (F(int, self, 36) < 0)
            break;
        int x = g_cpX, y = g_cpY;
        Cp_Image(x, y, g_cpImg6f33c8);
        int tx = x + 103, ty = y + 585;
        BYTE st[19 * 16];
        Hud_InitLights(st);
        int id;
        Player_GetState((BYTE*)*Player_CarId(&id), st);
        if (F(int, st, 160) && (g_cp6f2dd8 & 4) && Cp_TimeOdd(1.0))
            Cp_Text(tx, ty + 15, g_cpFont2, Cp_Str(109));
        else if (F(int, st, 160) && (g_cp6f2dd8 & 8) && Cp_TimeOdd(1.0))
            Cp_Text(tx, ty + 15, g_cpFont2, Cp_Str(110));
        else
            Cp_Text(tx, ty + 15, g_cpFont, g_cp6f2da4);
        sprintf(buf, s_cp67ba18, g_cp6f2db0, g_cp6f2db4);
        Cp_Text(tx, ty + 35, g_cpFont, buf);
        sprintf(buf, s_cp66c8cc, Cp_Int(g_cp6f2db8));
        Cp_Text(tx + 45, ty + 55, g_cpFont, buf);
        sprintf(buf, s_cp66c8cc, Cp_Int(g_cp6f2da8));
        Cp_Text(tx + 45, ty + 75, g_cpFont, buf);
        Cp_Notice(58);
        break;
    }
    case 24: {
        if (F(int, self, 36) < 0)
            break;
        int x = g_cpX, y = g_cpY;
        Cp_Image(x, y, g_cpImg6f33cc);
        if (g_cp6f2dc0 == g_cpNoCar)
            break;
        int tx = x + 103, ty = y + 565;
        BYTE info[192];
        CallC(FUN_43fc40, g_cp6f2dc0, info);
        Cp_Text(tx, ty + 15, g_cpFont, F(char*, info, 48));
        sprintf(buf, s_cp67ba18, g_cp6f2dd0, g_cp6f2dd4);
        Cp_Text(tx, ty + 35, g_cpFont, buf);
        Cp_Text(tx, ty + 55, g_cpFont, g_cp6f2da4);
        sprintf(buf, s_cp67ba18, g_cp6f2db0, g_cp6f2db4);
        Cp_Text(tx, ty + 75, g_cpFont, buf);
        sprintf(buf, s_cp66c8cc, Cp_Int(g_cp6f2db8));
        Cp_Text(tx + 44, ty + 95, g_cpFont, buf);
        sprintf(buf, s_cp66c8cc, Cp_Int(g_cp6f2da8));
        Cp_Text(tx + 44, ty + 115, g_cpFont, buf);
        int k = CallC<int>(FUN_43f8d0, g_cp6f2dc0);
        Cp_Image(x + 98, g_cpY + 454, F(BYTE*, g_cpImages, k * 4 + 368));
        Cp_Notice(72);
        break;
    }
    case 20:
    case 25: {
        if (g_cp6f2f08 == g_cp68cf6c)
            break;
        // present data: +4 a 4x3 matrix, +80 the order info
        BYTE pd[280];
        F(int, pd, 0) = 0;
        memcpy(pd + 4, g_cp7025f0, 48);
        for (int k = 52; k <= 68; k += 4)
            F(int, pd, k) = 0;
        F(int, pd, 72) = -1;
        F(int, pd, 76) = -1;
        CallThis(FUN_4d06a0, pd + 80);
        F(int, pd, 272) = -1;
        F(int, pd, 276) = -1;
        int id;
        CallC(FUN_41be60, *Player_CarId(&id), g_cp6f2f08, pd);
        BYTE* oi = pd + 80;
        int merch = F(int, oi, 0);
        if (merch != -1) {
            int x = g_cpX, y = g_cpY;
            Cp_Image(x, y, g_cpImg6f33c4);
            BYTE mp[40];
            F(int, mp, 0) = 0;
            CallC(FUN_4401d0, merch, mp);
            int tx = x + 125, ty = y + 557;
            Cp_Text(tx, ty + 15, g_cpFont, F(char*, oi, 100));
            if (Cp_TimeOdd(2.0))
                Cp_Text(tx, ty + 35, g_cpFont, F(char*, oi, 112));
            Cp_OrderLines(tx, ty, oi, mp, buf, s_cp66c8cc, s_cp67ba10);
        }
        CallThis(FUN_44b090, oi + 88);
        CallThis(FUN_404420, oi + 76);
        CallThis(FUN_404420, oi + 64);
        CallThis(FUN_404420, oi + 52);
        break;
    }
    case 17: {
        int v = F(int, self, 36);
        if (v % 100 == 0) {
            Cp_Image(g_cpX, g_cpY, g_cpImg6f33c4);
            Cp_Text(g_cpX + 100, g_cpY + 732, g_cpFont, Cp_Str(56), 4);
            break;
        }
        BYTE oi[192];
        CallThis(FUN_4d06a0, oi);
        int id;
        CallC(FUN_441950, *Player_CarId(&id), v / 100, v % 100, oi);
        int merch = F(int, oi, 0);
        if (merch != -1) {
            int x = g_cpX, y = g_cpY;
            Cp_Image(x, y, g_cpImg6f33c4);
            int tx = x + 125, ty = y + 557;
            sprintf(buf, s_cp67ba00, Cp_Str(60), F(int, self, 36) % 100);
            Cp_Text(x + 100, ty + 175, g_cpFont, buf, 4);
            BYTE mp[40];
            F(int, mp, 0) = 0;
            CallC(FUN_4401d0, merch, mp);
            BYTE st[19 * 16];
            Hud_InitLights(st);
            Player_GetState((BYTE*)*Player_CarId(&id), st);
            if (F(int, st, 160) && F(char*, oi, 104) && Cp_TimeOdd(1.0))
                Cp_Text(tx, ty + 15, g_cpFont2, F(char*, oi, 104));
            else
                Cp_Text(tx, ty + 15, g_cpFont, F(char*, oi, 100));
            Cp_Text(tx, ty + 35, g_cpFont, F(char*, oi, 112));
            Cp_OrderLines(tx, ty, oi, mp, buf, s_cp677a74, s_cp67b8f8);
        }
        if (unsigned size = F(BYTE*, oi, 96) - F(BYTE*, oi, 88))
            Ai_PoolFree(F(BYTE*, oi, 88), size);
        CallThis(FUN_44b090, oi + 76);
        CallThis(FUN_44b090, oi + 64);
        CallThis(FUN_44b090, oi + 52);
        break;
    }
    case 19: {
        Cp_Image(g_cpX2 + 2, g_cpY2, g_cpImg6f33d0);
        // town info: +20 a vector of 192-byte orders, +32 their count, +36 the first
        BYTE ti[48];
        for (int k = 8; k <= 40; k += 4)
            F(int, ti, k) = 0;
        int id;
        CallC(FUN_440f70, *Player_CarId(&id), F(int, self, 36), ti);
        int x = g_cpX2 + 49;
        int count = F(int, ti, 32);
        int y = g_cpY2 + 551;
        for (int i = 0; i < count; i++, y += 20) {
            BYTE* o = F(BYTE*, ti, 36) + i * 192;
            Cp_Text(x, y, g_cpFont, F(char*, o, 100));
            Cp_Text(x + 102, y, g_cpFont, F(char*, o, 108));
            Cp_Text(x + 192, y, g_cpFont, F(char*, o, 112));
            int t = Cp_Int(F(double, o, 168));
            sprintf(buf, s_cp67b8f8, t / 60, t % 60);
            Cp_Text(x + 280, y, g_cpFont, buf);
        }
        BYTE* b = F(BYTE*, ti, 20);
        for (BYTE* p = b; p != F(BYTE*, ti, 24); p += 192)
            CallThis(FUN_576c00, p, 0);
        if (int n = (int)(F(BYTE*, ti, 28) - b) / 192)
            Ai_PoolFree(b, n * 192);
        break;
    }
    }
    F(int, self, 32) = -1;
}

} // extern "C"

// === the copter's on-screen notice (FUN_56b0a0)

ASM_VAR(BYTE*, g_ntView, $L_69684c)
ASM_VAR(BYTE*, g_ntFont, $L_6ced24)
ASM_VAR(BYTE*, g_ntMessages, $L_6f3438)
ASM_VAR(BYTE*, g_nt6cecd8, $L_6cecd8)
ASM_VAR(BYTE, g_nt696904, $L_696904)
ASM_VAR(int, g_nt696890, $L_696890)
ASM_VAR(int, g_nt696c54, $L_696c54)
ASM_VAR(int, g_nt6f3480, $L_6f3480)
ASM_VAR(int, g_nt6f3484, $L_6f3484)
ASM_VAR(int, g_nt6f3488, $L_6f3488)
ASM_VAR(int, g_nt6f348c, $L_6f348c)
ASM_VAR(double, g_nt64b700, $L_64b700)
ASM_VAR(double, g_ntOne, $L_64b670)
ASM_VAR(AsmLabel, s_nt66c8cc, $L_66c8cc)
ASM_VAR(AsmLabel, s_nt677a74, $L_677a74)
ASM_VAR(AsmLabel, s_nt67b998, $L_67b998)
ASM_VAR(AsmLabel, s_nt67b988, $L_67b988)
ASM_VAR(AsmLabel, s_nt67b978, $L_67b978)
ASM_VAR(AsmLabel, s_nt67b9a0, $L_67b9a0)
ASM_VAR(AsmLabel, s_nt67b9b0, $L_67b9b0)
ASM_VAR(AsmLabel, s_nt67b9c0, $L_67b9c0)
ASM_VAR(AsmLabel, s_nt67b9d0, $L_67b9d0)
ASM_VAR(AsmLabel, s_nt67b9e0, $L_67b9e0)
ASM_VAR(AsmLabel, s_nt67b9f0, $L_67b9f0)
ASM_VAR(AsmLabel, s_nt6731dc, $L_6731dc)
extern "C" void FUN_5b8980(); KING_ALIAS("_FUN_5b8980", "_Gfx_DrawImage")    // an image with a transparent color (surface, x, y, image, key)
extern "C" void FUN_5ea0e0(); KING_ALIAS("_FUN_5ea0e0", "@Font_TextWidth@12")
extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")

static __declspec(noinline) int Nt_Int(double d)
{
    return (int)d;
}

static const char* Nt_Str(int id)
{
    return CallC<const char*>(FUN_5d2e50, id);
}

static int Nt_Width(const char* text)
{
    return CallThis<int>(FUN_5ea0e0, g_ntFont, text);
}

static void Nt_Text(int x, int y, BYTE* font, const char* text)
{
    CallC(FUN_5ea170, F(BYTE*, g_ntView, 12), x, y, font, text, 6, 0);
}

static int Nt_ViewW()
{
    return CallVirt<int>(g_ntView, 20);
}

static int Nt_ViewH()
{
    return CallVirt<int>(g_ntView, 24);
}

extern "C" {

// [FUN_56b0a0] draws the notice of kind +60 (1..10): one or two lines of text centered under its image (+44), the
// copter results (5) or a score panel (10); kind 2 also sends message 951, kind 8 is shown once
void __fastcall Notice_Draw(BYTE* self, void*)
{
    int kind = F(int, self, 60);
    if (kind == 0)
        return;
    const char* lines[2];
    int n = 0;
    char buf[80];
    char buf2[36];
    switch (kind) {
    case 1:
        lines[0] = Nt_Str(7);
        n = 1;
        break;
    case 2: {
        lines[0] = Nt_Str(8);
        n = 1;
        BYTE msg[32];
        CallThis(FUN_602be0, msg);
        BYTE data[4];
        data[0] = g_nt696904;
        data[1] = 0;
        CallThis(FUN_603110, msg, 951, data, 3);
        CallC(FUN_6021b0, F(BYTE*, msg, 4), F(BYTE*, msg, 8) - F(BYTE*, msg, 4));
        CallThis(FUN_602e80, msg);
        break;
    }
    case 3:
        sprintf(buf, Nt_Str(3), F(int, self, 52), F(int, self, 56));
        lines[0] = buf;
        n = 1;
        break;
    case 6:
        lines[0] = Nt_Str(4);
        lines[1] = Nt_Str(5);
        n = 2;
        break;
    case 7:
        sprintf(buf, Nt_Str(6), F(int, self, 52));
        lines[0] = buf;
        n = 1;
        break;
    case 8:
    case 9:
        lines[0] = F(const char*, self, 48);
        n = 1;
        break;
    case 5: {
        int car;
        int on = !(CallC<DWORD>(FUN_43f620, *CallC<int*>(FUN_568f60, &car)) & 0x1000);
        int a = F(int, self, 52);
        const char* name;
        if (CallC<int>(FUN_568fd0) == 2)
            name = a == 0 ? s_nt67b9d0 : a == 1 && on ? s_nt67b9e0 : s_nt67b9f0;
        else
            name = a == 0 ? s_nt67b9a0 : a == 1 && on ? s_nt67b9b0 : s_nt67b9c0;
        BYTE* img = CallC<BYTE*>(FUN_5ef3d0, name);
        Nt_ViewW();
        int y = (Nt_ViewH() - F(WORD, img, 6)) / 2 + 20;
        int x = Nt_ViewW() - F(WORD, img, 4);
        CallC(FUN_5b8980, F(BYTE*, g_ntView, 12), x, y, img, -1);
        BYTE* font = F(BYTE*, g_ntMessages, 1288);
        if (a > 0) {
            sprintf(buf, s_nt66c8cc, F(int, self, 56) % 100);
            Nt_Text(x + 165, y + 249, font, buf);
            Nt_Text(x + 129, y + 274, F(BYTE*, g_ntMessages, 1288), (const char*)(g_nt6cecd8 + 632));
            int t = F(int, self, 56) / 100 / 60;
            if (g_nt696890 == 2) {
                sprintf(buf, s_nt67b998, t);
                Nt_Text(x + 125, y + 299, F(BYTE*, g_ntMessages, 1288), buf);
            } else {
                sprintf(buf, s_nt66c8cc, t);
                Nt_Text(x + 165, y + 299, F(BYTE*, g_ntMessages, 1288), buf);
            }
        } else {
            sprintf(buf, s_nt66c8cc, F(int, self, 56));
            Nt_Text(x + 209, y + 153, font, buf);
            sprintf(buf, s_nt66c8cc, g_nt6f3480);
            Nt_Text(x + 172, y + 216, F(BYTE*, g_ntMessages, 1288), buf);
            sprintf(buf, s_nt66c8cc, g_nt6f3484);
            Nt_Text(x + 172, y + 195, F(BYTE*, g_ntMessages, 1288), buf);
            sprintf(buf, s_nt66c8cc, g_nt6f3480 - g_nt6f3484);
            Nt_Text(x + 172, y + 279, F(BYTE*, g_ntMessages, 1288), buf);
        }
        if (F(int, self, 16) == 2)
            if (BYTE* o = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_nt6731dc))
                CallThis(FUN_5fdaf0, o, 1.0f, 1.0f);
        return;
    }
    case 10: {
        sprintf(buf2, CallC<int>(FUN_568fd0) == 2 ? s_nt67b988 : s_nt67b978, F(int, self, 52));
        BYTE* img = CallC<BYTE*>(FUN_5ef3d0, buf2);
        if (!img)
            break;
        Nt_ViewW();
        Nt_ViewH();
        int x = Nt_ViewW() - F(WORD, img, 4);
        CallC(FUN_5b8980, F(BYTE*, g_ntView, 12), x, 50, img, -1);
        sprintf(buf2, s_nt677a74, F(int, self, 56));
        int tw = Nt_Width(buf2);
        int alt = g_nt696c54 == 0;
        int a = F(int, self, 52);
        int y = a < 9 || a == 20 ? 148 : a < 12 ? 157 : a == 16 ? 261 : a < 20 ? 431 : 0;
        if (y)
            Nt_Text((F(WORD, img, 4) - tw) / 2 + x, y, g_ntFont, buf2);
        if (a == 14) {
            tw = Nt_Width(F(const char*, self, 48));
            Nt_Text((F(WORD, img, 4) - tw) / 2 + x, 160, g_ntFont, F(const char*, self, 48));
        } else if (a == 10 || a == 16) {
            double k = alt ? g_nt64b700 : g_ntOne;
            sprintf(buf2, s_nt66c8cc, Nt_Int(g_nt6f3488 * k));
            Nt_Text(a == 10 ? x + 82 : x + 137, a == 10 ? 272 : 142, g_ntFont, buf2);
            k = alt ? g_nt64b700 : g_ntOne;
            sprintf(buf2, s_nt66c8cc, Nt_Int(g_nt6f348c * k));
            Nt_Text(a == 10 ? x + 82 : x + 137, a == 10 ? 318 : 158, g_ntFont, buf2);
        } else if (a == 15) {
            Nt_Width(F(const char*, self, 48));
            CallC(FUN_5ea1b0, F(BYTE*, g_ntView, 12), x + 45, 227, 190, g_ntFont, F(const char*, self, 48), 6, 0);
        }
        return;
    }
    }
    // the box: the image centered at the bottom, the lines below its top
    int w = Nt_ViewW();
    BYTE* img = F(BYTE*, self, 44);
    int x = (w - F(WORD, img, 20)) / 2;
    int y = Nt_ViewH() - F(WORD, img, 22);
    CallC(FUN_5b86c0, F(BYTE*, g_ntView, 12), x, y, img);
    y += 20;
    for (int i = 0; i < n; i++, y += 10)
        Nt_Text((w - Nt_Width(lines[i])) / 2, y, g_ntFont, lines[i]);
    if (F(int, self, 60) == 8)
        F(int, self, 60) = 0;
}

} // extern "C"

// === setting up the world around the vehicle (FUN_56d810)

ASM_VAR(BYTE*, g_wsShell, $L_6cec6c)
ASM_VAR(BYTE*, g_ws6cec78, $L_6cec78)
ASM_VAR(BYTE*, g_ws6cec70, $L_6cec70)
ASM_VAR(int, g_ws6cec68, $L_6cec68)
ASM_VAR(int, g_ws7208d4, $L_7208d4)
ASM_VAR(int, g_ws6ced70, $L_6ced70)
ASM_VAR(double, g_ws64b930, $L_64b930)
ASM_VAR(int, g_ws6f3408, $L_6f3408)
ASM_VAR(BYTE*, g_wsWeather6f33ac, $L_6f33ac)
ASM_VAR(BYTE*, g_wsWipers6f3498, $L_6f3498)
ASM_VAR(int, g_ws6f3318, $L_6f3318)
ASM_VAR(BYTE*, g_ws6f34a4, $L_6f34a4)
ASM_VAR(BYTE*, g_ws6f34a8, $L_6f34a8)
ASM_VAR(double, g_ws6f32f8, $L_6f32f8)
ASM_VAR(AsmLabel, g_wsLight6f32e8, $L_6f32e8)   // 4 dwords
ASM_VAR(AsmLabel, g_wsLight6f3300, $L_6f3300)   // 4 dwords
ASM_VAR(int, g_ws696890, $L_696890)
ASM_VAR(DWORD, g_ws696874, $L_696874)
ASM_VAR(AsmLabel, s_ws6733c4, $L_6733c4)
ASM_VAR(AsmLabel, s_ws673384, $L_673384)
ASM_VAR(AsmLabel, s_ws6733ac, $L_6733ac)
ASM_VAR(AsmLabel, s_ws67bb08, $L_67bb08)
ASM_VAR(AsmLabel, s_ws67bb10, $L_67bb10)
ASM_VAR(AsmLabel, s_ws67bb18, $L_67bb18)
ASM_VAR(AsmLabel, s_ws67bb20, $L_67bb20)
ASM_VAR(AsmLabel, s_ws67bb28, $L_67bb28)
ASM_VAR(AsmLabel, s_ws67bb00, $L_67bb00)
ASM_VAR(AsmLabel, s_ws672994, $L_672994)
ASM_VAR(AsmLabel, s_ws67baf4, $L_67baf4)
ASM_VAR(AsmLabel, s_ws67bae4, $L_67bae4)
ASM_VAR(AsmLabel, s_ws676834, $L_676834)
ASM_VAR(AsmLabel, s_ws676838, $L_676838)
ASM_VAR(AsmLabel, s_ws67b69c, $L_67b69c)
ASM_VAR(AsmLabel, s_ws67bae0, $L_67bae0)
ASM_VAR(AsmLabel, s_ws6751b8, $L_6751b8)
ASM_VAR(AsmLabel, s_ws67bad4, $L_67bad4)
ASM_VAR(AsmLabel, s_ws67bacc, $L_67bacc)
extern "C" void FUN_5ff890(); KING_ALIAS("_FUN_5ff890", "_Sce_SetRootDir")
extern "C" void FUN_5ffc30(); KING_ALIAS("_FUN_5ffc30", "_Set683c58")
extern "C" void FUN_5ffa30(); KING_ALIAS("_FUN_5ffa30", "_Music_Start")
extern "C" void FUN_574030(); KING_ALIAS("_FUN_574030", "_Colors_Init")
extern "C" void FUN_4e5ea8(); KING_ALIAS("_FUN_4e5ea8", "_World_ClearHits")
extern "C" void FUN_5972d0(); KING_ALIAS("_FUN_5972d0", "_Pyramid_SetSize")
extern "C" void FUN_5a7430(); KING_ALIAS("_FUN_5a7430", "@WeatherObj_Construct@32")
extern "C" void FUN_5a8960(); KING_ALIAS("_FUN_5a8960", "@Obj5a8960_Set@40")
extern "C" void FUN_5a5720(); KING_ALIAS("_FUN_5a5720", "_Set7025b0")
extern "C" void FUN_56d560(); KING_ALIAS("_FUN_56d560", "@Wipers_Construct@12")
extern "C" void FUN_5a8e70(); KING_ALIAS("_FUN_5a8e70", "_Set6fb3e8")
extern "C" void FUN_5a3e00(); KING_ALIAS("_FUN_5a3e00", "@Worobj5a3e00_Construct@28")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")

extern "C" {

// [FUN_56d810] sets up the world for the vehicle v: the music (network games), colors, the weather object with
// its light, the wipers, the cabin pictures and sky mode by the truck model, the horizon distances
void __cdecl CopterWorld_Setup(BYTE* v)
{
    if (F(int, g_wsShell, 1344)) {
        char dir[80];
        GetPrivateProfileStringA(s_ws6733c4, s_ws673384, s_ws6733ac, dir, 79, g_csIniPath);
        CallC(FUN_5ff890, dir);
        int n = F(short, g_ws6cec78, 384) * F(short, g_ws6cec78, 382);
        CallC(FUN_5ffc30, n * g_ws64b930);
        if (F(short, g_ws6cec78, 384) > 5)
            CallC(FUN_5ffa30);
    }
    g_ws6f3408 = 0;
    CallC(FUN_574030, *F(BYTE**, F(BYTE*, v, 468), 100));
    CallC(FUN_4e5ea8);
    CallC(FUN_5972d0, v);
    F(int, F(BYTE*, v, 472), 84) = 0;
    F(int, F(BYTE*, v, 472), 88) = 0;
    F(int, F(BYTE*, CallThis<BYTE*>(FUN_598da0, g_csTv6f68b0), 472), 84) = 0;
    F(int, F(BYTE*, CallThis<BYTE*>(FUN_598da0, g_csTv6f68b0), 472), 88) = 0;
    BYTE* w = (BYTE*)operator new(584);
    if (w)
        w = CallThis<BYTE*>(FUN_5a7430, w, v, (const char*)s_ws67bb08, (const char*)s_ws67bb10,
                            (const char*)s_ws67bb18, (const char*)s_ws67bb20, (const char*)s_ws67bb28);
    g_wsWeather6f33ac = w;
    const DWORD* l1 = (const DWORD*)(char*)g_wsLight6f32e8;
    const DWORD* l2 = (const DWORD*)(char*)g_wsLight6f3300;
    CallThis(FUN_5a8960, w, l1[0], l1[1], l1[2], l1[3], l2[0], l2[1], l2[2], l2[3]);
    F(float, g_wsWeather6f33ac, 544) = (float)g_ws6f32f8;
    if (F(int, g_wsShell, 1316))
        F(DWORD, g_wsWeather6f33ac, 572) |= 0x10000;
    char yes[12];
    GetPrivateProfileStringA(s_cs672998, s_ws67bb00, s_cs6729a8, yes, 10, F(char*, v, 640));
    F(int, g_wsWeather6f33ac, 568) = strcmp(yes, s_ws672994) == 0;
    if (g_ws6cec68) {
        if (g_ws7208d4 && (g_ws6ced70 == 4 || g_ws6ced70 == 3) && F(short, g_ws6cec70, 420) > 90
            && F(short, g_ws6cec70, 422) > 90)
            CallC(FUN_5a5720, 1);
        BYTE* wp = (BYTE*)operator new(216);
        wp = wp ? CallThis<BYTE*>(FUN_56d560, wp, v) : 0;
        g_wsWipers6f3498 = wp;
        F(BYTE*, g_wsWeather6f33ac, 496) = wp;
        F(int, g_wsWipers6f3498, 212) = g_ws6f3318;
    }
    if (g_wsWipers6f3498) {
        CallVirt(g_wsWipers6f3498, 24, 1);
        F(int, g_wsWipers6f3498, 164) = 0;
    }
    g_ws6f34a4 = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_ws67baf4);
    g_ws6f34a8 = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_ws67bae4);
    BYTE* p = F(BYTE*, v, 488);
    const char* model = F(const char*, F(BYTE*, p ? F(BYTE*, p, 76) : 0, 36), 60);
    if (strcmp(model, s_ws676834) && strcmp(model, s_ws676838) && strcmp(model, s_ws67b69c)) {
        if (!strcmp(model, s_ws67bae0) || !strcmp(model, s_ws6751b8))
            CallC(FUN_5a8e70, 7, 0);
        F(int, g_wsWeather6f33ac, 576) = 0;
        if (g_ws6f34a4)
            CallVirt(g_ws6f34a4, 40);
    } else {
        F(int, g_wsWeather6f33ac, 576) = 1;
        if (g_ws6f34a8)
            CallVirt(g_ws6f34a8, 40);
        CallC(FUN_5a8e70, strcmp(model, s_ws67b69c) ? 6 : 5, 0);
    }
    BYTE* o = (BYTE*)operator new(56);
    if (o)
        o = CallThis<BYTE*>(FUN_5a3e00, o, v, (const char*)s_ws67bad4, 8, 0, 1072693248);
    CallVirt(o, 24, 1);
    BYTE* t = F(BYTE*, CallThis<BYTE*>(FUN_598da0, g_csTv6f68b0), 472);
    F(int, t, 32) = 0;
    F(DWORD, t, 36) = 1080623104;          // +32: 200.0
    F(int, F(BYTE*, v, 472), 32) = 0;
    F(DWORD, F(BYTE*, v, 472), 36) = 1083129856;   // +32: 1000.0
    CallC(FUN_5e1a10, CallC<BYTE*>(FUN_5ef2b0, 0, (const char*)s_ws67bacc), g_ws696890 == 0 ? 1 : 0);
    g_ws696874 &= ~0x40u;
}

} // extern "C"

// === the copter's network messages (FUN_56d0f0)

ASM_VAR(BYTE*, g_nm696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_nm6ceca8, $L_6ceca8)
ASM_VAR(int, g_nm672984, $L_672984)
ASM_VAR(int, g_nm67b730, $L_67b730)
ASM_VAR(AsmLabel, s_nm67ba60, $L_67ba60)
ASM_VAR(AsmLabel, s_nm68d1c0, $L_68d1c0)
extern "C" void FUN_50c7f0(); KING_ALIAS("_FUN_50c7f0", "_Carat_SetFlag")
extern "C" void FUN_51f380(); KING_ALIAS("_FUN_51f380", "@Shell_SetMode@12")
extern "C" void FUN_576660(); KING_ALIAS("_FUN_576660", "_Set6f3338")
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5a40b0(); KING_ALIAS("_FUN_5a40b0", "@Node_Position@12")
extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")
ASM_PROC(FUN_5a4f00)
extern "C" void FUN_5a3fd0(); KING_ALIAS("_FUN_5a3fd0", "@Node_Matrix@12")
extern "C" void FUN_4ba0c0(); KING_ALIAS("_FUN_4ba0c0", "_Effect_New4dd2a0")
extern "C" void FUN_56f910(); KING_ALIAS("_FUN_56f910", "_Game_GoToRoom")

// the world resource of id
static BYTE* Nm_Resource(int id)
{
    return CallThis<BYTE*>(FUN_5e5370, CallVirt<BYTE*>(g_nm696ca0, 112), id);
}

struct Nm48 { DWORD d[12]; };
ASM_VAR(AsmLabel, g_nm7025f0, $L_7025f0)

static void Cp_Identity48(BYTE* m)
{
    memcpy(m, g_nm7025f0, 48);
}

extern "C" {

// [FUN_56d0f0] handles a network message of the copter screen (id, data, size): flags, screen modes, the
// weather light, object states and their effects, positions; 0
int __cdecl CopterNet_Handle(int id, BYTE* data, int size)
{
    switch (id) {
    case 951:
        CallC(FUN_50c7f0, (int)data[0]);
        break;
    case 952:
        if (data[0] == 6)
            F(int, g_nm696ca0, 3036) = 1;
        else
            CallThis(FUN_51f380, g_nm6ceca8, (int)data[0]);
        break;
    case 953:
        CallC(FUN_576660, (int)data[0], F(int, data, 1));
        break;
    case 955:
        if (g_wsWeather6f33ac && (F(int, g_wsShell, 1300) > 1 || F(int, g_wsShell, 1316) == 2)) {
            CallThis(FUN_5a8960, g_wsWeather6f33ac, (double)F(float, data, 0), (double)F(float, data, 4), 24.0, 1.0);
            if (size > 8)
                F(int, g_wsWeather6f33ac, 544) = F(int, data, 8);
        }
        break;
    case 956: {
        int v = F(int, data, 4);
        BYTE* r = Nm_Resource(F(int, data, 0));
        if (!r)
            break;
        F(int, F(BYTE*, r, 24), 4) = v;
        BYTE* s = F(BYTE*, r, 28);
        if (!s)
            break;
        F(int, s, 408) = v;
        if (v != 2)
            break;
        int pos[3] = {0, 0, 0};
        CallThis(FUN_5a40b0, s, pos);
        BYTE* e = (BYTE*)operator new(192);
        if (e) {
            BYTE* root = CallThis<BYTE*>(FUN_5ef640, s + 256);
            e = CallThis<BYTE*>(FUN_5a4f00, e, F(int, g_wsShell, 1356), root, pos[0], pos[1], pos[2], 0, 1073741824,
                                (const char*)s_nm67ba60, (const char*)s_nm68d1c0, 3, 2576980378u, 1069128089);
        }
        CallVirt(e, 24, 1);
        break;
    }
    case 961:
    case 962: {
        int v = F(int, data, 4);
        BYTE* r = Nm_Resource(F(int, data, 0));
        if (!r)
            break;
        if (id == 961)
            F(int, F(BYTE*, r, 24), 4) = v;
        if (BYTE* s = F(BYTE*, r, 28))
            F(int, s, 408) = v;
        break;
    }
    case 963: {
        int v = F(int, data, 4);
        BYTE* r = Nm_Resource(F(int, data, 0));
        if (!r)
            break;
        BYTE* s = F(BYTE*, r, 28);
        if (!s)
            break;
        if (F(int, s, 416) == 0 && v != 0) {
            int bits = 0;
            for (int k = v; k; k >>= 1)
                bits++;
            BYTE m[48];
            Cp_Identity48(m);
            CallThis(FUN_5a3fd0, s, m);
            CallC(FUN_4ba0c0, m, bits / 2, (void*)Obj56d0d0_Has416, s);
        }
        F(int, s, 416) = v;
        break;
    }
    case 958: {
        int extra = size > 68 ? F(int, data, 116) : 0;
        g_nm672984 = F(int, data, 0);
        Nm48 m = *(const Nm48*)(data + 68);
        CallC(FUN_56f910, F(int, data, 0), extra, data + 4, data + 36, m);
        break;
    }
    case 959:
        if (F(int, g_wsShell, 1300) > 1)
            g_nm672984 = F(int, data, 0);
        break;
    case 960:
        g_nm67b730 = data[0];
        break;
    }
    return 0;
}

} // extern "C"

// === boarding a car from the copter (FUN_56fc10)

ASM_VAR(BYTE*, g_bc6d2078, $L_6d2078)
ASM_VAR(int, g_bc68b990, $L_68b990)
ASM_VAR(BYTE*, g_bc6f3434, $L_6f3434)
ASM_VAR(AsmLabel, g_bc6f2d98, $L_6f2d98)
ASM_VAR(int, g_bc6f33f4, $L_6f33f4)
ASM_VAR(AsmLabel, g_bc6f68b0, $L_6f68b0)
ASM_VAR(int, g_bc6f2dc0, $L_6f2dc0)
ASM_VAR(float, g_bc64c32c, $L_64c32c)
ASM_VAR(AsmLabel, g_bc7025f0, $L_7025f0)
extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")
extern "C" void FUN_43fc40(); KING_ALIAS("_FUN_43fc40", "_Ai_GetPlayerData")
extern "C" void FUN_441d70(); KING_ALIAS("_FUN_441d70", "_Ai_GetParkingInfo")
extern "C" void FUN_4738c0(); KING_ALIAS("_FUN_4738c0", "_Player_SwapVehicles")
extern "C" void FUN_476bc0(); KING_ALIAS("_FUN_476bc0", "_Player_Hire")
extern "C" void FUN_4d90c0(); KING_ALIAS("_FUN_4d90c0", "_Fx_PlaceObjects")
extern "C" void FUN_4e5ea8(); KING_ALIAS("_FUN_4e5ea8", "_World_ClearHits")
extern "C" void FUN_4ebaa0(); KING_ALIAS("_FUN_4ebaa0", "@Engine_SetKeys@16")
extern "C" void FUN_531cc0(); KING_ALIAS("_FUN_531cc0", "@Viewer_AttachVehicle@12")
extern "C" void FUN_541960(); KING_ALIAS("_FUN_541960", "@Obj541960_Self@8")
extern "C" void FUN_541970(); KING_ALIAS("_FUN_541970", "@Obj541970_First@8")
extern "C" void FUN_55f4e0(); KING_ALIAS("_FUN_55f4e0", "_Vehicle_FindTaker")
extern "C" void FUN_568f60(); KING_ALIAS("_FUN_568f60", "_Player_CarId")
extern "C" void FUN_56fae0(); KING_ALIAS("_FUN_56fae0", "_Net_SendObjectMessage")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_597570(); KING_ALIAS("_FUN_597570", "@WorldObj_Attach@12")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")

static int Bc_Copy(const int* id)
{
    int t;
    CallThis(FUN_41d300, &t, id);
    return t;
}

static int Bc_Flags(int id) { return CallC<int>(FUN_43f620, id); }

struct Bc_M48 { DWORD d[12]; };

extern "C" {

// [FUN_56fc10] single player: the first car near the player (closer than 64c32c) with a driver, not hidden
// (0x100) and of the kinds 64 or 0x400 becomes the parking target (6f3434, 6f2d98); with 6f33f4 set the player
// boards it: kind 64 swaps vehicles (the player's car left with the old driver), kind 0x400 goes through 476bc0
void __cdecl Copter_BoardNearby()
{
    if (F(int, g_wsShell, 1300) > 1 || F(int, g_wsShell, 1316))
        return;
    BYTE* me = F(BYTE*, g_bc6d2078, 616);
    BYTE* o = 0;
    BYTE* res = 0;
    int id = g_bc68b990;
    for (BYTE* it = CallThis<BYTE*>(FUN_541970, me); it;) {
        BYTE* c = CallThis<BYTE*>(FUN_541960, it);
        if (c != F(BYTE*, g_bc6d2078, 616) && F(int, c, 20636) != g_bc68b990) {
            id = F(int, c, 20636);
            if (!(Bc_Flags(id) & 0x100)) {
                res = CallThis<BYTE*>(FUN_5e5370, F(BYTE*, g_wsShell, 1356) + 704, F(int, (BYTE*)id, 84));
                if ((Bc_Flags(id) & 64) || (Bc_Flags(id) & 0x400)) {
                    BYTE m1[48], m2[48];
                    float cp[3];
                    memcpy(cp, CallThis<BYTE*>(FUN_57de90, c + 16, m1) + 36, 12);
                    const float* mp =
                        (const float*)(CallThis<BYTE*>(FUN_57de90, F(BYTE*, g_bc6d2078, 616) + 16, m2) + 36);
                    float d[3] = {mp[0] - cp[0], mp[1] - cp[1], mp[2] - cp[2]};
                    if (CallC<double>(FUN_5ab4d0, d) < g_bc64c32c) {
                        o = c;
                        break;
                    }
                }
            }
        }
        it = F(BYTE*, it, 8);
        if (!it)
            break;
        it -= 4;
    }
    if (!o)
        return;
    F(int, g_bc6f3434, 36) = -1;
    if (CallC<int>(FUN_441d70, Bc_Copy(&id), (BYTE*)g_bc6f2d98))
        return;
    F(int, g_bc6f3434, 32) = (Bc_Flags(id) & 1024) ? 24 : 18;
    F(int, g_bc6f3434, 36) = 0;
    if (!g_bc6f33f4)
        return;
    g_bc6f33f4 = 0;
    int car;
    CallC(FUN_568f60, &car);
    BYTE data[176];
    CallC(FUN_43fc40, Bc_Copy(&id), data);
    if (id == g_bc68b990)
        return;
    if (Bc_Flags(id) & 64) {
        CallC(FUN_4d90c0);
        CallC(FUN_4e5ea8);
        int a = Bc_Copy(&id);
        int b = Bc_Copy(&car);
        if (CallC<int>(FUN_4738c0, b, a, 0))
            return;
        BYTE* p = F(BYTE*, g_bc6d2078, 616);
        if (F(int, p, 21880))
            CallC(FUN_55f4e0, p, 0);
        Bc_M48 m = *(Bc_M48*)(BYTE*)g_bc7025f0;
        CallC(FUN_56fae0, F(int, data, 36), F(int, res, 4), 0, 0, m);
        CallThis(FUN_531cc0, g_bc6d2078, o);
        F(WORD, F(BYTE*, o, 21600), 10048) = 1;
        F(int, F(BYTE*, o, 21600), 10080) = 1;
        F(int, o, 20636) = car;
        CallThis(FUN_597570, (BYTE*)g_bc6f68b0, o);
        CallThis(FUN_4ebaa0, F(BYTE*, me, 21600) + 9984, 1, 1);
        F(WORD, F(BYTE*, me, 21600), 10054) = 100;
        F(WORD, F(BYTE*, me, 21600), 10048) = 0;
        F(int, F(BYTE*, me, 21600), 10080) = 0;
        F(int, me, 20636) = g_bc6f2dc0;
    } else if (Bc_Flags(id) & 0x400) {
        int a = Bc_Copy(&id);
        CallC(FUN_476bc0, a, id);
    }
}

} // extern "C"

// === the weather's update ($L_576830)

ASM_VAR(int, g_wu6cec68, $L_6cec68)
ASM_VAR(BYTE*, g_wu6d2078, $L_6d2078)
ASM_VAR(BYTE*, g_wu6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_wu696ca0, $L_696ca0)
ASM_VAR(int, g_wu67b730, $L_67b730)
ASM_VAR(DWORD, g_wu6f6938, $L_6f6938)
ASM_VAR(double, g_wu64b7a8, $L_64b7a8)
ASM_VAR(float, g_wu64fdd0, $L_64fdd0)
ASM_VAR(AsmLabel, g_wu6d1db8, $L_6d1db8)
ASM_VAR(AsmLabel, g_wu7025f0, $L_7025f0)
extern "C" void FUN_530070(); KING_ALIAS("_FUN_530070", "@Controls_ActionState@12")
extern "C" void FUN_5a58c0(); KING_ALIAS("_FUN_5a58c0", "@Obj5a58c0_Start@8")
extern "C" void FUN_5a6420(); KING_ALIAS("_FUN_5a6420", "@Dial_Update@4")
extern "C" void FUN_5a9d40(); KING_ALIAS("_FUN_5a9d40", "@Wave_Value@16")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")

// the weather level of 6f33ac
static double Wu_Level() { return CallThis<double>(FUN_5a9d40, g_wsWeather6f33ac, 0, 0); }

// records network message 960 with the wipers state
static void Wu_Send(BYTE* w)
{
    BYTE msg[40], b[2] = {F(BYTE, w, 212), 0};
    CallThis(FUN_602be0, msg);
    CallThis(FUN_603110, msg, 960, b, 2);
    CallThis(FUN_5c3630, g_wu696ca0, msg);
    CallThis(FUN_602e80, msg);
}

extern "C" {

// [$L_576830] updates weather w (thiscall): with precipitation (level above 64b7a8) the drops shown outside
// (+164 bit 8) and on the glass (bit 16) by the camera mode and detail, else the effect restarts; the
// wipers key (action 38) toggles +212 (sent to the other players), their sound (+208); the car's speed
// into +184; bit 2 when it rains and the camera is under 64fdd0
void __fastcall Weather_Update(BYTE* w)
{
    BYTE* pl = g_wu6d2078;
    int outside, inCab;
    if (!g_wu6cec68) {
        outside = inCab = !F(int, pl, 1400) && !F(int, pl, 1396);
    } else {
        outside = F(int, pl, 1400) == 0;
        inCab = F(int, pl, 1396) == 0;
    }
    DWORD& f = F(DWORD, w, 164);
    if (Wu_Level() > g_wu64b7a8) {
        bool detail = F(short, g_wu6cec70, 420) > 50;
        if (outside && detail)
            f |= 8;
        else
            f &= ~8u;
        if ((!inCab || !outside) && detail)
            f |= 16;
        else
            f &= ~16u;
        f &= ~64u;
    } else {
        CallThis(FUN_5a58c0, w);
        f &= ~2u;
    }
    for (int off = 124; off <= 132; off += 8)
        if (BYTE* o = F(BYTE*, w, off)) {
            if (outside)
                F(BYTE, o, 40) &= 254;
            else
                F(BYTE, o, 40) |= 1;
        }
    if (CallThis<int>(FUN_530070, (BYTE*)g_wu6d1db8, 38)) {
        if (F(int, g_wsShell, 1300) == 1)
            Wu_Send(w);
        F(int, w, 212) ^= 1;
        if (F(int, g_wsShell, 1300) == 1)
            Wu_Send(w);
    }
    if (F(int, g_wsShell, 1300) > 1)
        F(int, w, 212) = g_wu67b730;
    if (F(int, w, 168)) {
        if (F(int, w, 212))
            f &= ~4u;
        else {
            f |= 4;
            if (F(BYTE*, w, 208) && outside)
                CallThis(FUN_5fdaf0, F(BYTE*, w, 208), 0.6f, 0.6f);
        }
        F(int, w, 168) = 0;
    }
    g_wu6f6938 = f;
    if (BYTE* car = F(BYTE*, g_wu6d2078, 616))
        F(double, w, 184) = CallC<double>(FUN_5ab4d0, car + 432);
    CallThis(FUN_5a6420, w);
    BYTE m[48];
    memcpy(m, (BYTE*)g_wu7025f0, 48);
    if (BYTE* car = F(BYTE*, g_wu6d2078, 616)) {
        BYTE t[48];
        memcpy(m, CallThis<BYTE*>(FUN_57de90, car + 16, t), 48);
    }
    if (Wu_Level() > g_wu64b7a8 && F(float, m, 44) < g_wu64fdd0)
        f |= 2;
    else
        f &= ~2u;
}

} // extern "C"

// === the score table (FUN_574530)

ASM_VAR(int, g_sc67b75c, $L_67b75c)
ASM_VAR(int, g_sc6ce9f0, $L_6ce9f0)
ASM_VAR(AsmLabel, g_sc6ce9f4, $L_6ce9f4)
ASM_VAR(double, g_sc64b780, $L_64b780)

extern "C" {

// [FUN_574530] fills the score table (names, values, count): in single play (+384 of 6cec98 below 2) the moving
// vehicles with a score (+128 over 64b780) that aren't hidden (0x4000), ascending; in mode 2 the network
// players (6ce9f4, 16 bytes each) when one has over 1, descending; 67b75c = 2 for an empty table, else whether
// the first is the player
void __cdecl Score_Build()
{
    g_scoreCount = 0;
    char** names = (char**)g_scoreNames;
    double* values = g_scoreValues;
    if (F(int, g_6cec98, 384) < 2) {
        int n = 0;
        int cur;
        Players_First(&cur);
        BYTE info[256];
        while (cur != g_noVehicle68b990h && n < 60) {
            CallC(FUN_43fc40, cur, info);
            int type = F(int, info, 8);
            if ((type == 1 || type == 2) && F(double, info, 128) > g_sc64b780 &&
                !(CallC<DWORD>(FUN_43f620, cur) & 0x4000)) {
                names[n] = F(char*, info, 48);
                values[n] = F(double, info, 128);
                n++;
            }
            int nx;
            cur = *Players_Next(&nx, cur);
        }
        g_scoreCount = n;
    } else {
        bool any = false;
        int k = g_sc6ce9f0;
        for (int i = 0; i < k; i++) {
            names[i] = (char*)g_sc6ce9f4 + i * 16;
            values[i] = g_6f3338[i];
            if (g_6f3338[i] > g_hel64b670)
                any = true;
        }
        if (any)
            g_scoreCount = k;
    }
    int last = g_scoreCount - 1;
    bool swapped;
    do {
        swapped = false;
        for (int j = 0; j < last; j++) {
            int mode = F(int, g_6cec98, 384);
            if ((mode < 2 && values[j] > values[j + 1]) || (mode == 2 && values[j] < values[j + 1])) {
                double v = values[j];
                values[j] = values[j + 1];
                values[j + 1] = v;
                char* t = names[j];
                names[j] = names[j + 1];
                names[j + 1] = t;
                swapped = true;
            }
        }
    } while (swapped);
    if (!g_scoreCount) {
        g_sc67b75c = 2;
        return;
    }
    const char* me;
    BYTE info[256];
    if (F(int, g_6cec98, 384) < 2) {
        int id;
        CallC(FUN_43fc40, *Player_CarId(&id), info);
        me = F(char*, info, 48);
    } else
        me = (const char*)(g_hd_6cecd8 + 632);
    g_sc67b75c = names[0] && !strcmp(names[0], me) ? 1 : 0;
}

} // extern "C"

// === the radio messages of the helicopter screen (the update $L_56aa70)

extern "C" void FUN_41fa00(); KING_ALIAS("_FUN_41fa00", "_Ai_LooseSemitrailer")
ASM_VAR(double, g_rm6f3470, $L_6f3470)
ASM_VAR(double, g_rm6f3478, $L_6f3478)
ASM_VAR(double, g_rm64b938, $L_64b938)
ASM_VAR(double, g_rm64b958, $L_64b958)

static double Rm_Now() { return CallC<double>(FUN_6037e0); }

extern "C" {

// [$L_56aa70] radio screen s: the semitrailer state (+1268: 1 coupled, 2 just let loose, 0 after 64b5f0 s);
// the message typer (+1196 the shown characters, +32 its time) takes the next queued line (4 of 128 bytes at
// +44, +1204 the next) into +556 and, past 1100, moves it into the history (+684, +1208 the next)
void __fastcall Radio_Update(BYTE* s)
{
    BYTE* car = F(BYTE*, g_hd_game, 616);
    if (car && CallVirt<int>(car, 4) == 1) {
        if (F(int, car, 21880))
            F(int, s, 1268) = 1;
        else if (F(int, s, 1268) == 1) {
            int id;
            CallC(FUN_41fa00, *Player_CarId(&id));
            F(int, s, 1268) = 2;
            g_rm6f3470 = Rm_Now();
        } else if (F(int, s, 1268) == 2) {
            double lim = g_rm6f3470 + g_64b5f0;
            if (Rm_Now() > lim)
                F(int, s, 1268) = 0;
        }
    } else
        F(int, s, 1268) = 0;
    if (F(int, s, 1196) != 1) {
        double lim = F(double, s, 32) + g_rm64b938;
        if (Rm_Now() > lim) {
            double now = Rm_Now();
            int n = F(int, s, 1196);
            double x = now - F(double, s, 32) + g_rm6f3478;
            if (!n) {
                F(int, s, 1196) = 1;
                F(int, s, 1276) = 0;
                g_rm6f3478 = 0.0;
            } else {
                F(int, s, 1196) = (int)(x * g_rm64b958 + n);
                g_rm6f3478 = (double)((int)x % 50);
            }
            F(double, s, 32) = now;
        }
    } else {
        char* line = (char*)s + 44 + F(int, s, 1204) * 128;
        if (line[0]) {
            strcpy((char*)s + 556, line);
            line[0] = 0;
            F(int, s, 1204) = (F(int, s, 1204) + 1) % 4;
            F(int, s, 1196) = 2;
            F(double, s, 32) = Rm_Now();
            F(int, s, 1216) = 0;
            if (F(BYTE*, s, 1280))
                CallThis(FUN_5fdaf0, F(BYTE*, s, 1280), 0.2f, 0.2f);
        } else if (F(int, s, 1216)) {
            double lim = F(double, s, 32) + g_64b5f0;
            if (Rm_Now() > lim)
                F(int, s, 1216) = 0;
        }
    }
    if (F(int, s, 1196) > 1100) {
        F(int, s, 1196) = 0;
        F(double, s, 32) = Rm_Now();
        int k = F(int, s, 1208)++;
        strcpy((char*)s + 684 + k * 128, (const char*)s + 556);
        F(int, s, 1208) %= 4;
        F(char, s, 556) = 0;
    }
    CallThis(FUN_5d32f0, s, 1);
}

} // extern "C"

// === loading a room module for the helicopter screen ($L_56dde0)

extern "C" void FUN_5ead00(); KING_ALIAS("_FUN_5ead00", "@Room_Each@16")
extern "C" void FUN_5ef390(); KING_ALIAS("_FUN_5ef390", "@Room_FindObject@12")
extern "C" void FUN_5a9d40(); KING_ALIAS("_FUN_5a9d40", "@Wave_Value@16")
extern "C" void FUN_616770(); KING_ALIAS("_FUN_616770", "@Terrain616770_SetCorners@16")
extern "C" void FUN_615230(); KING_ALIAS("_FUN_615230", "_Terrain_Call614f70@12")
extern "C" void FUN_42aba0(); KING_ALIAS("_FUN_42aba0", "_Ai_OnModuleLoadX")
ASM_VAR(BYTE*, g_ml6f3420, $L_6f3420)
ASM_VAR(char*, g_ml6f3424, $L_6f3424)
ASM_VAR(int, g_ml7025bc, $L_7025bc)
ASM_VAR(int, g_ml7208cc, $L_7208cc)
extern "C" int __cdecl AnmObj_OnLoad(BYTE* node, void* ctx);    // the per-object callback
ASM_VAR(AsmLabel, s_ml676838, $L_676838)
ASM_VAR(AsmLabel, s_ml676834, $L_676834)
ASM_VAR(AsmLabel, s_ml67bb58, $L_67bb58)    // "skyMirrorKey"
ASM_VAR(AsmLabel, s_ml67bb30, $L_67bb30)
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);

extern "C" {

// [$L_56dde0] module load handler s for room r (+60 its name): every object through 56e060, the sky mirror
// switch by the truck's wave, for the two terrain rooms (676838, 676834) the terrain's corners ter000, ter050,
// ter100 and the camera direction; then the AI's hook and, with 7208cc, the records named like 67bb30 get flag
// 0x2000 and 0.65 at +104
void __fastcall Copter_OnModuleLoad(BYTE* s, void*, BYTE* r)
{
    g_ml6f3420 = s;
    g_ml6f3424 = F(char*, r, 60);
    CallThis(FUN_5ead00, r, (void*)AnmObj_OnLoad, 0);
    BYTE* o = CallThis<BYTE*>(FUN_5ef390, r, (const char*)s_ml67bb58);
    g_ml7025bc = 0;
    if (o && F(int, o, 16) == 18 && g_6f33ac) {
        double n = CallVirt<int>(o, 60);
        CallC(FUN_5e1a10, o, (int)(CallThis<double>(FUN_5a9d40, (BYTE*)g_6f33ac, 0, 0) * n));
    }
    const char* name = F(const char*, r, 60);
    if (!strcmp(name, s_ml676838) || !strcmp(name, s_ml676834)) {
        BYTE* t[3] = {CallC<BYTE*>(FUN_5ef3d0, "ter000"), CallC<BYTE*>(FUN_5ef3d0, "ter050"),
                      CallC<BYTE*>(FUN_5ef3d0, "ter100")};
        CallThis(FUN_616770, g_terrain6f33b4, t, 3);
        float m[12], c[12];
        memset(m, 0, 48);
        memcpy(m, g_q214_7025f0, 48);
        memcpy(m, CallThis<float*>(FUN_5dbcc0, F(BYTE*, g_6cec6c, 1356), c), 48);
        double d0 = g_q214_702620, d1 = g_q214_702624, d2 = g_q214_702628;
        float v[3];
        v[0] = (float)(d2 * m[6] + d0 * m[0] + d1 * m[3]);
        v[1] = (float)(d2 * m[7] + d1 * m[4] + d0 * m[1]);
        v[2] = (float)(m[8] * d2 + m[5] * d1 + m[2] * d0);
        CallStd(FUN_615230, m + 9, v, (void*)g_q214_702630);
    }
    CallC(FUN_42aba0, F(char*, r, 60));
    if (g_ml7208cc)
        for (int i = 0; i < F(int, r, 72); i++) {
            BYTE* e = F(BYTE*, r, 76) + i * 124;
            if (!strncmp((const char*)e, s_ml67bb30, 13)) {
                F(DWORD, e, 32) |= 0x2000;
                F(DWORD, e, 104) = 0x3F266666;      // 0.65
            }
        }
}

} // extern "C"

// === the load of the player's orders (FUN_56f6d0)

extern "C" void FUN_520000(); KING_ALIAS("_FUN_520000", "_Car_ApplyPlayerSettings")

extern "C" {

// [FUN_56f6d0] gives the player's car (vtable +168) the cargo of its orders: the sums of the goods' two values
// (+16, +24 of their properties) times the amounts (+8); then applies the player's car settings
void __cdecl Copter_ApplyCargo()
{
    float a = 0, b = 0;
    BYTE oi[192];
    CallThis(FUN_4d06a0, oi);
    int id;
    Player_CarId(&id);
    if (id != g_noVehicle68b990h) {
        BYTE mp[40];
        F(int, mp, 0) = 0;
        for (unsigned i = 0; !Ai_GetPlayerOrderInfo((BYTE*)id, i, oi); i++) {
            CallC(FUN_4401d0, F(int, oi, 0), mp);
            a = (float)(F(double, mp, 16) * F(double, oi, 8) + a);
            b = (float)(F(double, mp, 24) * F(double, oi, 8) + b);
        }
        BYTE* car = F(BYTE*, g_hd_game, 616);
        ((void(__fastcall*)(BYTE*, void*, float*, float*))(*(void***)car)[42])(car, 0, &b, &a);
    }
    CallC(FUN_520000, F(BYTE*, g_hd_game, 616));
    static const int k_vecs[] = {88, 76, 64, 52};
    for (int off : k_vecs)
        if (int n = (int)(F(BYTE*, oi, off + 8) - F(BYTE*, oi, off)))
            Ai_PoolFree(F(BYTE*, oi, off), n);
}

} // extern "C"

// === going to a room (FUN_56f910)

extern "C" void FUN_531cc0(); KING_ALIAS("_FUN_531cc0", "@Viewer_AttachVehicle@12")    // the game takes object x as the player's
extern "C" void Gr_Attach(); KING_ALIAS("_Gr_Attach", "@WorldObj_Attach@12")
extern "C" void Gr_ScenesFind(); KING_ALIAS("_Gr_ScenesFind", "@Scenes_Find@12")
extern "C" void Gr_LoadRoom(); KING_ALIAS("_Gr_LoadRoom", "@Fw_LoadRoom@16")
extern "C" void Gr_ResRelease(); KING_ALIAS("_Gr_ResRelease", "@ResRef_Release@8")
ASM_VAR(BYTE*, g_gr6d2078, $L_6d2078)
ASM_VAR(int, g_gr6f34b0, $L_6f34b0)
ASM_VAR(AsmLabel, g_gr6f68b0, $L_6f68b0)
ASM_VAR(int, g_gr675d3c, $L_675d3c)
ASM_VAR(int, g_gr675d38, $L_675d38)
struct Gr_Mat { DWORD m[12]; };

extern "C" {

// [FUN_56f910] goes to object id in room (scene, area): an object already loaded becomes the player's (with the
// scene to come back to when the player's scene differs); else (after the pending load +484 ends) the world
// (696ca0) loads the room, the object to take (675d3c) and frame f (+652) kept
void __cdecl Game_GoToRoom(int id, int, const char* scene, const char* area, Gr_Mat f)
{
    g_gr6f34b0 = 0;
    if (scene && !*scene)
        scene = 0;
    if (area && !*area)
        area = 0;
    BYTE* res = CallThis<BYTE*>(FUN_5e5370, g_696ca0 + 704, id);
    if (res && F(int, res, 28)) {
        BYTE* pl = F(BYTE*, g_gr6d2078, 616);
        if (!pl)
            return;
        if (scene) {
            BYTE* o = F(BYTE*, pl, 308);
            if (strcmp(scene, F(const char*, o, 60)))
                g_gr6f34b0 = CallThis<int>(Gr_ScenesFind, g_696ca0, F(const char*, o, 60));
        }
        int x = F(int, res, 28);
        CallThis(FUN_531cc0, g_gr6d2078, x);
        CallThis(Gr_Attach, (BYTE*)g_gr6f68b0, x);
        return;
    }
    BYTE* w = g_696ca0;
    if (F(int, F(BYTE*, w, 484), 20)) {
        while (!F(int, F(BYTE*, g_696ca0, 484), 16))
            Sleep(10);
        CallThis(Gr_ResRelease, F(BYTE*, g_696ca0, 484));
        w = g_696ca0;
    }
    g_gr675d3c = id;
    g_gr675d38 = F(int, w, 2992);
    memcpy(w + 652, &f, 48);
    strcpy(F(char*, g_696ca0, 488) + 44, area);
    strcpy(F(char*, g_696ca0, 488) + 12, scene);
    CallThis(Gr_Attach, (BYTE*)g_gr6f68b0, 0);
    CallThis(Gr_LoadRoom, g_696ca0, scene, area);
}

} // extern "C"

// === collecting the sharp corners near a point (56f450)

extern "C" void Ec_GetWorld(); KING_ALIAS("_Ec_GetWorld", "@Node_GetWorld@12")
ASM_VAR(BYTE*, g_ec6f34c4, $L_6f34c4)
ASM_VAR(AsmLabel, s_ecSharp, $L_67c18c)       // "sharp"
ASM_VAR(AsmLabel, g_ec7025f0, $L_7025f0)

extern "C" {

// [$L_56f450] scene walk of Enum_56f3f0: a "sharp" sphere (kind 9, shape 7) within its radius plus 6f3254 of the
// point 6f3248 is added (at most 30) with its frame's node (the parent's, kind 4, or the item being walked 6f34c4);
// an item (kind 17, not hidden) is walked through with itself as 6f34c4; a switch (18) answers its selection
int __cdecl Enum_SharpCallback(BYTE* n)
{
    if ((F(BYTE, n, 40) & 1) || g_6f34c0 >= 30)
        return -1;
    if (F(int, n, 16) == 9 && F(char*, n, 12) && !strncmp(F(char*, n, 12), (const char*)s_ecSharp, 5) &&
        F(int, n, 64) == 7) {
        float m[12];
        memcpy(m, (BYTE*)g_ec7025f0, 48);
        BYTE* fr = 0;
        BYTE* p = F(BYTE*, n, 44);
        if (F(int, p, 16) == 4) {
            if ((fr = F(BYTE*, p, 60)) != 0)
                CallThis(Ec_GetWorld, fr, m);
        } else if (g_ec6f34c4) {
            fr = g_ec6f34c4;
            CallThis(Ec_GetWorld, fr, m);
        }
        double r = *F(float*, n, 60);
        const float* c = F(const float*, n, 72);
        double wx = (double)m[0] * c[0] + (double)m[6] * c[2] + (double)m[3] * c[1] + m[9];
        float wy = (float)((double)m[1] * c[0] + (double)m[7] * c[2] + (double)m[4] * c[1] + m[10]);
        float wz = (float)((double)m[2] * c[0] + (double)m[8] * c[2] + (double)m[5] * c[1] + m[11]);
        float d[3] = {(float)(wx - *(float*)&g_6f3248), (float)((double)wy - *(float*)&g_6f324c),
                      (float)((double)wz - *(float*)&g_6f3250)};
        double l = CallC<double>(FUN_5ab4d0, d);
        if (!(l > *(float*)&g_6f3254 + r)) {
            g_6f2e58[g_6f34c0] = n;
            g_6f326c[g_6f34c0] = (int)fr;
            g_6f34c0++;
        }
        return -1;
    }
    if (F(int, n, 16) == 17) {
        if (F(BYTE, n, 40) & 8)
            return -1;
        g_ec6f34c4 = F(BYTE*, n, 68);
        CallVirt(F(BYTE*, n, 64), 32, (void*)Enum_SharpCallback, 0);
        g_ec6f34c4 = 0;
        return -1;
    }
    if (F(int, n, 16) == 18) {
        int i = F(BYTE*, n, 60) ? F(int, F(BYTE*, n, 60), 56) : F(int, n, 56);
        if (i >= CallVirt<int>(n, 60))
            i = CallVirt<int>(n, 60) - 1;
        return i;
    }
    return -1;
}

} // extern "C"

// [$L_56bb30] frees the storage of a vector (begin +0, capacity end +8)
extern "C" void __fastcall Vec_FreeStorage56bb30(BYTE** v)
{
    if (int n = (int)(v[2] - v[0]))
        CallC(FUN_44b4a0, v[0], n);
}

extern "C" void Rr_FreeVectors(); KING_ALIAS("_Rr_FreeVectors", "@Rec440dd0_FreeVectors@8")
// [$L_56cbc0] frees the vector of 192-byte route records at +20 (each with its vectors)
extern "C" void __fastcall Routes_Free(BYTE* s)
{
    BYTE** v = (BYTE**)(s + 20);
    for (BYTE* e = v[0]; e != v[1]; e += 192)
        CallThis(Rr_FreeVectors, e);
    if (int n = (int)(v[2] - v[0]) / 192)
        CallC(FUN_44b4a0, v[0], n * 192);
}

// === a mission object's events (a handler at $L_573e40)

extern "C" void Mo_SetFlags(); KING_ALIAS("_Mo_SetFlags", "_Player_SetFlags")
ASM_VAR(void*, g_mo6f341c, $L_6f341c)
ASM_VAR(int, g_mo6f34ac, $L_6f34ac)

// clears the bits of player id's flags
static void Mo_Clear(int id, DWORD bits)
{
    CallC(Mo_SetFlags, id, CallC<DWORD>(FUN_43f620, id) & ~bits);
}

extern "C" {

// [$L_573e40] handles event e (after the hook 6f341c, which may take it) for its object (+28): code 100 deletes it
// (counted in 6f34ac; a player's car outside a network game drops flag 1) or without one drops flag 16 of the
// owner's (+24) player; 101 and 1..4 delete it; 1 when handled
int __cdecl MissionObj_Event(int* e, int a)
{
    if (g_mo6f341c && CallC<int>((void (*)())g_mo6f341c, e, a))
        return 1;
    int code = e[1] / 1000;
    BYTE* o = (BYTE*)e[7];
    switch (code) {
    case 100:
        if (o) {
            if (!(F(int, g_6cec6c, 1300) >= 2 || (F(int, g_6cec6c, 1316) && F(int, g_6cec98, 384) >= 2))) {
                int id = F(int, o, 20636);
                if (id != g_noCar)
                    Mo_Clear(id, 1);
            }
            if (o)
                CallVirt(o, 0, 1);
            g_mo6f34ac--;
            e[7] = 0;
        } else {
            int id = F(int, (BYTE*)e[6], 296);
            if (id != g_noCar)
                Mo_Clear(id, 16);
        }
        return 1;
    case 101:
        if (o) {
            CallVirt(o, 0, 1);
            e[7] = 0;
        }
        return 1;
    case 1:
    case 2:
    case 3:
    case 4:
        if (o)
            CallVirt(o, 0, 1);
        return 1;
    }
    return 0;
}

} // extern "C"

// === a turret firing at its target (FUN_567bb0)

extern "C" int __cdecl rand(void);
extern "C" void FUN_567440(); KING_ALIAS("_FUN_567440", "_Bullet_Fly")
extern "C" void Tf_GameTime(); KING_ALIAS("_Tf_GameTime", "_Game_Time")
extern "C" void Tf_Matrix1(); KING_ALIAS("_Tf_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Tf_Normalized(); KING_ALIAS("_Tf_Normalized", "_Vec3_Normalized")
ASM_VAR(double, g_tf64fd40, $L_64fd40)
ASM_VAR(double, g_tf64fd38, $L_64fd38)
ASM_VAR(double, g_tf64b670, $L_64b670)

static double Tf_Now() { return CallC<double>(Tf_GameTime); }

extern "C" {

// [FUN_567bb0] turret s fires (+3284 while busy) at its target (+3248) when both reload times (+3288, +3292) are
// past 64fd40: the target's position in s's frame (+48 inverse rows), x scattered at random, gives the aim, whose
// direction scaled by twice |y| and turned into the world (+0 rows, +36 the muzzle) is the shot (FUN_567440 from
// +96, +3480, 2.0); the older reload time restarts
void __fastcall Turret_Fire(BYTE* s)
{
    F(int, s, 3284) = 1;
    if (Tf_Now() - F(float, s, 3288) < g_tf64fd40 || Tf_Now() - F(float, s, 3292) < g_tf64fd40)
        return;
    BYTE* t = F(BYTE*, s, 3248);
    if (!t) {
        F(float, s, 3288) = (float)Tf_Now();
        F(float, s, 3292) = (float)Tf_Now();
        F(int, s, 3284) = 0;
        return;
    }
    BYTE m[48];
    const float* P = (const float*)(CallThis<BYTE*>(Tf_Matrix1, t + 16, m) + 36);
    double lx = (double)F(float, s, 52) * P[0] + (double)F(float, s, 76) * P[2] + (double)F(float, s, 64) * P[1]
        + F(float, s, 88);
    float L[3];
    L[2] = (float)((double)F(float, s, 56) * P[0] + (double)F(float, s, 80) * P[2] + (double)F(float, s, 68) * P[1]
        + F(float, s, 92));
    L[0] = (float)((double)F(float, s, 72) * P[2] + (double)F(float, s, 60) * P[1] + (double)P[0] * F(float, s, 48)
        + F(float, s, 84));
    L[1] = (float)lx;
    L[0] = (float)((rand() * g_tf64fd38 + g_tf64b670) * L[0] * g_64b840);
    float nb[3];
    const float* n = CallC<const float*>(Tf_Normalized, nb, L);
    double ay = x87_fabs(L[1]);
    float ax = (float)(n[0] * ay), am = (float)(n[1] * ay);
    double az = n[2] * ay;
    float q[3] = {F(float, s, 36), F(float, s, 40), F(float, s, 44)};
    float bx = ax + ax, by = am + am;
    double bz = az + az;
    float d[3];
    d[0] = (float)(bz * F(float, s, 24) + (double)by * F(float, s, 12) + (double)bx * F(float, s, 0) + F(float, s, 36));
    d[1] = (float)(bz * F(float, s, 28) + (double)by * F(float, s, 16) + (double)bx * F(float, s, 4) + F(float, s, 40));
    d[2] = (float)(bz * F(float, s, 32) + (double)by * F(float, s, 20) + (double)bx * F(float, s, 8) + F(float, s, 44));
    CallC(FUN_567440, F(int, s, 96), q, d, F(int, s, 3480), 0x40000000u);
    if (F(float, s, 3288) > F(float, s, 3292))
        F(float, s, 3292) = (float)Tf_Now();
    else
        F(float, s, 3288) = (float)Tf_Now();
    F(int, s, 3284) = 0;
}

} // extern "C"

// === the helicopter's flight step (FUN_561f30)

extern "C" void Hf_FrameTime(); KING_ALIAS("_Hf_FrameTime", "_Game_FrameTime")
extern "C" void Hf_Time(); KING_ALIAS("_Hf_Time", "_Game_Time")
extern "C" void Hf_NormalizeThis(); KING_ALIAS("_Hf_NormalizeThis", "@Vec3_NormalizeThis@8")
extern "C" void Hf_ToAngles(); KING_ALIAS("_Hf_ToAngles", "_Matrix_ToAngles")
extern "C" void Hf_MulAssign(); KING_ALIAS("_Hf_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Hf_FromPose(); KING_ALIAS("_Hf_FromPose", "_Matrix_FromPose")
extern "C" void Hf_HitObject(); KING_ALIAS("_Hf_HitObject", "_Query_HitObject")
ASM_VAR(float, g_hf6f2c3c, $L_6f2c3c)
ASM_VAR(float, g_hf67b288, $L_67b288)
ASM_VAR(float, g_hf67b298, $L_67b298)
ASM_VAR(float, g_hf67b2a0, $L_67b2a0)
ASM_VAR(float, g_hf67b2a4, $L_67b2a4)
ASM_VAR(double, g_hf64b840, $L_64b840)
ASM_VAR(float, g_hf64c264, $L_64c264)
ASM_VAR(double, g_hf64b660, $L_64b660)
ASM_VAR(double, g_hf64d338, $L_64d338)
ASM_VAR(float, g_hf64c450, $L_64c450)
ASM_VAR(float, g_hf64c2f8, $L_64c2f8)
ASM_VAR(float, g_hf64c258, $L_64c258)
ASM_VAR(double, g_hf64b678, $L_64b678)
ASM_VAR(double, g_hf64b780, $L_64b780)
ASM_VAR(double, g_hf64b670, $L_64b670)
ASM_VAR(double, g_hf64b5b8, $L_64b5b8)
ASM_VAR(double, g_hf64d0c0, $L_64d0c0)
ASM_VAR(double, g_hf64fcc8, $L_64fcc8)
ASM_VAR(double, g_hf64f3d8, $L_64f3d8)

static Vec3 Hf_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_561f30] the helicopter s's step: lift and thrust along its forward (+128, +3448 / +3452) and up axes,
// limited, damped by +3212 .. +3224 (velocity +100, spin +112, turn +120), the rolling frame of its spin and
// its new pose and frame (+0 / +48) and object (+96)
void __fastcall Copter_FlightStep(BYTE* s, void*)
{
    float dt = (float)CallC<double>(Hf_FrameTime);
    CallC<double>(Hf_Time);
    const float* M = (const float*)s;
    const float* Y = &g_axisY.x;
    const float* Z = &g_axisZ.x;
    Vec3 fwd, up, side;
    fwd.x = (float)((double)Y[0] * M[0] + (double)Y[2] * M[6] + (double)Y[1] * M[3]);
    fwd.y = (float)((double)Y[0] * M[1] + (double)Y[2] * M[7] + (double)Y[1] * M[4]);
    fwd.z = 0;
    up.x = (float)((double)Z[0] * M[0] + (double)Z[2] * M[6] + (double)Z[1] * M[3]);
    up.y = (float)((double)Z[0] * M[1] + (double)Z[2] * M[7] + (double)Z[1] * M[4]);
    up.z = (float)((double)Z[2] * M[8] + (double)Z[1] * M[5] + (double)Z[0] * M[2]);
    CallThis(Hf_NormalizeThis, &fwd);
    side = Hf_Set((double)fwd.y * Z[2] - (double)Z[1] * fwd.z, (double)Z[0] * fwd.z - (double)fwd.x * Z[2],
                  (double)fwd.x * Z[1] - (double)fwd.y * Z[0]);
    F(float, s, 128) = (float)((double)g_hf6f2c3c * F(float, s, 3448));
    double x, y, z, head, pitch, roll;
    CallC(Hf_ToAngles, *(const Matrix*)s, &x, &y, &z, &head, &pitch, &roll);
    pitch = g_hf67b288 + pitch;
    float pos[3] = {(float)x, (float)y, (float)z};
    float k = (float)((double)g_hf67b298 * F(float, s, 3452));
    float bank;
    pitch = Copter_Tilt(s, 0, dt, (float)pitch, &fwd.x, &bank);
    double turn = (double)g_hel67b290 * F(float, s, 3444) * dt + F(float, s, 120);
    float tf = (float)turn;
    F(float, s, 120) = tf;
    if (turn > g_hel67b294)
        F(float, s, 120) = g_hel67b294;
    else if (tf < -(double)g_hel67b294)
        F(float, s, 120) = (float)-(double)g_hel67b294;
    if (F(int, s, 3488))
    {
        if ((double)g_hf67b2a0 * g_hf64b840 > x87_fabs(pitch))
            F(int, s, 3488) = 0;
        else
        {
            double d = (double)F(float, s, 108) * fwd.z + (double)F(float, s, 104) * fwd.y + (double)F(float, s, 100) * fwd.x;
            int a = F(float, s, 128) > g_hf64c264 ? 1 : 0;
            int b = d > g_hf64c264 ? 1 : 0;
            if (a != b)
            {
                if (x87_fabs(d) > g_hf64b660)
                    bank = 0;
                else
                    bank = (float)(bank * g_hf64d338);
            }
        }
    }
    double fx = (double)fwd.x * bank;
    float fy = (float)((double)fwd.y * bank), fz = (float)((double)bank * fwd.z);
    double ux = (double)up.x * k;
    float uy = (float)((double)up.y * k), uz = (float)((double)up.z * k);
    Vec3 Fv = Hf_Set(ux + fx, (double)fy + uy, (double)fz + uz);
    double L = Vec3_Length(&Fv);
    if (L > g_hf64c450)
    {
        double nx = Fv.x / L;
        float ny = (float)(Fv.y / L), nz = (float)(Fv.z / L);
        Fv = Hf_Set(nx * g_hf64c450, (double)ny * g_hf64c450, (double)nz * g_hf64c450);
    }
    Vec3 vel = Hf_Set((double)Fv.x * dt + F(float, s, 100), (double)Fv.y * dt + F(float, s, 104), (double)Fv.z * dt + F(float, s, 108));
    double L2 = Vec3_Length(&vel);
    if (L2 > g_hf64c2f8)
    {
        Vec3 u = Hf_Set(vel.x / L2, vel.y / L2, vel.z / L2);
        vel = Hf_Set((double)u.x * g_hf64c2f8, (double)u.y * g_hf64c2f8, (double)u.z * g_hf64c2f8);
    }
    Vec3 u = Hf_Set((double)Fv.x * dt, (double)Fv.y * dt, (double)Fv.z * dt);
    Vec3 w = Hf_Set((double)u.x * dt, (double)u.y * dt, (double)u.z * dt);
    Vec3 h = Hf_Set((double)w.x * g_hf64c258, (double)w.y * g_hf64c258, (double)w.z * g_hf64c258);
    Vec3 p = Hf_Set((double)dt * F(float, s, 100), (double)dt * F(float, s, 104), (double)dt * F(float, s, 108));
    Vec3 q = Hf_Set((double)p.x + h.x, (double)p.y + h.y, (double)p.z + h.z);
    float npos[3];
    npos[0] = (float)((double)q.x + pos[0]);
    npos[1] = (float)((double)q.y + pos[1]);
    npos[2] = (float)((double)q.z + pos[2]);
    Vec3 c1 = Hf_Set((double)vel.z * Z[1] - (double)vel.y * Z[2], (double)vel.x * Z[2] - (double)vel.z * Z[0],
                     (double)vel.y * Z[0] - (double)vel.x * Z[1]);
    Vec3 c2 = Hf_Set((double)c1.y * Z[2] - (double)c1.z * Z[1], (double)c1.z * Z[0] - (double)c1.x * Z[2],
                     (double)c1.x * Z[1] - (double)c1.y * Z[0]);
    Vec3 n;
    Vec3_Normalized(&n, &c2);
    Vec3 cr = Hf_Set((double)n.y * fwd.z - (double)n.z * fwd.y, (double)n.z * fwd.x - (double)n.x * fwd.z,
                     (double)n.x * fwd.y - (double)n.y * fwd.x);
    double dd = (double)vel.x * fwd.x + (double)vel.y * fwd.y + (double)vel.z * fwd.z;
    Vec3 along = Hf_Set(dd * fwd.x, (double)fwd.y * dd, dd * fwd.z);
    double asn = x87_asin(cr.z);
    float ang = (float)((double)n.x * fwd.x + (double)n.z * fwd.z + (double)n.y * fwd.y > g_hf64b678 ? asn : -asn);
    float L3 = (float)Vec3_Length(&along);
    double r = L3 > g_hf64b780 ? g_hf64b670 : L3 * g_hf64b5b8;
    Matrix rz;
    Vec3 t1;
    vel = *Vec3_RotateBy(&t1, &vel, Matrix_RotationZ(&rz, (float)(r * ang)));
    Vec3 fw2, sd, zv, t2, t3;
    Vec3_Scaled(&fw2, &fwd, (float)((double)vel.x * fwd.x + (double)vel.y * fwd.y + (double)vel.z * fwd.z));
    float e = (float)(L3 > g_hf64d0c0 ? g_hf64b670 : L3 * g_hf64fcc8);
    Vec3_Scaled(&sd, &side, (float)Vec3_Dot(&vel, &side));
    zv = Hf_Set(0, 0, vel.z);
    CallThis(Hf_MulAssign, &sd, &t2, F(float, s, 3212));
    CallThis(Hf_MulAssign, &fw2, &t2, F(float, s, 3216));
    sd.z = (float)((double)sd.z * F(float, s, 3220));
    const Vec3* nv = Vec3_Add(&t3, Vec3_Add(&t2, &sd, &fw2), &zv);
    F(Vec3, s, 100) = *nv;
    float d3224 = F(float, s, 3224);
    float s112 = (float)((double)d3224 * F(float, s, 112));
    F(float, s, 112) = s112;
    float s116 = (float)((double)F(float, s, 3224) * F(float, s, 116));
    F(float, s, 116) = s116;
    if (!F(int, s, 3432))
        F(float, s, 120) = (float)((double)F(float, s, 3224) * F(float, s, 120));
    float lo = (float)-(double)g_hf67b2a4;
    if (lo > roll)
        roll = lo;
    if (g_hf67b2a4 < roll)
        roll = g_hf67b2a4;
    double yy = roll - (double)e * F(float, s, 3444) * g_hf64f3d8;
    if ((yy < g_hf67b2a4 && yy > lo) || (yy > g_hf67b2a4 && yy < roll) || (yy < lo && yy > roll))
        roll = yy;
    double s3224 = F(float, s, 3224);
    roll = s3224 * s3224 * roll;
    F(float, s, 3232) = (float)pitch;
    pitch = pitch - g_hf67b288;
    Vec3 spin = Hf_Set(s112, s116, 0), st;
    float rollAng = (float)Vec3_Length(Vec3_Scaled(&st, &spin, dt));
    Matrix look, pose, m52, m196, rz2;
    Matrix_LookAt(&look, g_zeroVec, spin, rollAng);
    CallC(Hf_FromPose, &pose, 0.0, 0.0, 0.0, head, pitch, roll);
    Matrix_RotationZ(&rz2, (float)((double)dt * F(float, s, 120)));
    Matrix_Multiply(&m196, Matrix_Multiply(&m52, &look, &pose), &rz2);
    m196.m[9] = npos[0];
    m196.m[10] = npos[1];
    m196.m[11] = npos[2];
    BYTE* o = CallC<BYTE*>(Hf_HitObject, F(BYTE*, s, 96), pos, npos);
    if (!o)
        o = F(BYTE*, s, 96);
    F(BYTE*, s, 96) = o;
    memcpy(s, &m196, 48);
    Matrix inv;
    memcpy(s + 48, Matrix_Inverse(&inv, (const Matrix*)s), 48);
    F(int, s, 3540) = 0;
}

} // extern "C"

// === the helicopter heading for its target (FUN_5665c0)

extern "C" void Ht_ClearAhead(); KING_ALIAS("_Ht_ClearAhead", "@Copter_SideDrift@12")
extern "C" void Ht_NodePos(); KING_ALIAS("_Ht_NodePos", "_Node_Position")
extern "C" void Ht_MulAssign(); KING_ALIAS("_Ht_MulAssign", "@Vec3_MulAssign@16")
ASM_VAR(double, g_ht64b678, $L_64b678)
ASM_VAR(double, g_ht64b670, $L_64b670)
ASM_VAR(double, g_ht64b660, $L_64b660)
ASM_VAR(float, g_ht64c320, $L_64c320)
ASM_VAR(float, g_ht64c450, $L_64c450)
ASM_VAR(float, g_ht64c2f8, $L_64c2f8)

static Vec3 Ht_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_5665c0] helicopter s flying to its target +3256: 1 while the way (level, by FUN_562c20 along its side
// axis) is clear farther than the target; else it starts the approach (flags +3384 .. +3464), its speed factor
// +3448 by distance and turn rate, the next waypoint (+3468, from the route stack +144 or the target) popped once
// reached; 0
int __fastcall Copter_HeadForTarget(BYTE* s, void*)
{
    F(int, s, 3148) = 1;
    Vec3 d, tmp;
    d.x = (float)((double)F(float, s, 3256) - F(float, s, 36));
    d.y = (float)((double)F(float, s, 3260) - F(float, s, 40));
    d.z = 0;
    float L = (float)Vec3_Length(&d);
    if (L > g_ht64b678)
        CallThis(Ht_MulAssign, &d, &tmp, (float)(g_ht64b670 / L));
    const float* M = (const float*)s;
    const float* Y = &g_axisY.x;
    const float* Z = &g_axisZ.x;
    double fx = (double)Y[2] * M[6] + (double)Y[1] * M[3] + (double)Y[0] * M[0];
    double fy = (double)Y[2] * M[7] + (double)Y[1] * M[4] + (double)Y[0] * M[1];
    double fz = (double)Y[2] * M[8] + (double)Y[1] * M[5] + (double)Y[0] * M[2];
    float cx = (float)(fz * Z[1] - fy * Z[2]);
    float cy = (float)(fx * Z[2] - fz * Z[0]);
    double cz = fy * Z[0] - fx * Z[1];
    Vec3 e = Ht_Set((double)cy * Z[2] - cz * Z[1], cz * Z[0] - (double)cx * Z[2], (double)cx * Z[1] - (double)cy * Z[0]), n;
    Vec3_Normalized(&n, &e);
    if (CallThis<double>(Ht_ClearAhead, s, &n) + g_ht64c320 > L)
        return 1;
    F(int, s, 3392) = 1;
    F(int, s, 3440) = 0;
    F(int, s, 3384) = 1;
    F(int, s, 3388) = 0;
    F(int, s, 3436) = 1;
    F(int, s, 3464) = 1;
    double t = x87_fabs((double)F(float, s, 108) * F(float, s, 76) + (double)F(float, s, 104) * F(float, s, 64)
                        + (double)F(float, s, 52) * F(float, s, 100)) + g_hel64b788;
    F(float, s, 3448) = (float)(g_ht64b660 / t * ((double)L + g_ht64c450) / ((double)L + g_ht64c2f8));
    short idx = Stack_Below(s + 144, 0);
    Vec3* P = &F(Vec3, s, 3468);
    if (idx != -1)
    {
        Vec3 q;
        *P = *CallC<const Vec3*>(Ht_NodePos, &q, (int)idx);
    }
    else
        *P = F(Vec3, s, 3256);
    Vec3 d2 = Ht_Set((double)P->x - F(float, s, 36), (double)P->y - F(float, s, 40), (double)P->z - F(float, s, 44));
    if (Vec3_Length(&d2) < g_bc64c32c)
        Stack_Pop(s + 144, 0);
    return 0;
}

} // extern "C"

// === a ray against the terrain (a callback at $L_569550)

extern "C" void Rt_SegmentHits(); KING_ALIAS("_Rt_SegmentHits", "@Terrain_SegmentHits@28")
extern "C" void Rt_ValueFree(); KING_ALIAS("_Rt_ValueFree", "@Value_Free@4")
extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
ASM_VAR(BYTE, g_rt6f2d10, $L_6f2d10)
ASM_VAR(DWORD, g_rtHitA, $L_6f2cf8)                // the hit's description {1, 10, 0, buffer}
ASM_VAR(DWORD, g_rtHitB, $L_6f3150)                // the hit's value {8, ?, terrain}

static void __cdecl Rt_FreeA()
{
    if ((&g_rtHitA)[3])
        operator delete((void*)(&g_rtHitA)[3]);
}

static void __cdecl Rt_FreeB() { CallThis(Rt_ValueFree, &g_rtHitB); }

extern "C" {

// [$L_569550] ray r (from +76 to +88, kind +0) against the terrain (6f33b4): on a hit its point +52 and normal
// +64 are set and it takes the terrain's hit records (+100, +108); 1 then
int __cdecl Terrain_RayCallback(int, BYTE* r)
{
    if (!CallThis<int>(Rt_SegmentHits, g_terrain6f33b4, r + 76, r + 88, F(int, r, 0), r + 52, r + 64))
        return 0;
    DWORD* A = &g_rtHitA;
    if (!(g_rt6f2d10 & 1))
    {
        A[3] = 0;
        g_rt6f2d10 |= 1;
        atexit(Rt_FreeA);
    }
    A[0] = 1;
    A[1] = 10;
    A[2] = 0;
    A[3] = 0;
    F(DWORD*, r, 100) = A;
    DWORD* B = &g_rtHitB;
    if (!(g_rt6f2d10 & 2))
    {
        B[0] = 0;
        g_rt6f2d10 |= 2;
        atexit(Rt_FreeB);
    }
    B[0] = 8;
    B[2] = (DWORD)g_terrain6f33b4;
    F(DWORD*, r, 108) = B;
    return 1;
}

} // extern "C"

// === the helicopter's frame by its state (FUN_566970)

extern "C" void Cs_HeadFor(); KING_ALIAS("_Cs_HeadFor", "@Copter_HeadForTarget@8")
extern "C" void Cs_InList(); KING_ALIAS("_Cs_InList", "_Car_InList")
extern "C" void Cs_MulAssign(); KING_ALIAS("_Cs_MulAssign", "@Vec3_MulAssign@16")
ASM_VAR(double, g_cs2_64b6e0, $L_64b6e0)
ASM_VAR(double, g_cs2_64b918, $L_64b918)
ASM_VAR(float, g_cs2_64d2f8, $L_64d2f8)
ASM_VAR(float, g_cs2_64c258, $L_64c258)
ASM_VAR(float, g_cs2_64c350, $L_64c350)
ASM_VAR(double, g_cs2_64b678, $L_64b678)
ASM_VAR(double, g_cs2_64b670, $L_64b670)
ASM_VAR(double, g_cs2_64d318, $L_64d318)
ASM_VAR(double, g_cs2_64b5b8, $L_64b5b8)
ASM_VAR(double, g_cs2_64b6d0, $L_64b6d0)
ASM_VAR(double, g_cs2_64ed90, $L_64ed90)
ASM_VAR(double, g_cs2_64f760, $L_64f760)
ASM_VAR(double, g_cs2_64d0f0, $L_64d0f0)
ASM_VAR(float, g_cs2_64c450, $L_64c450)
ASM_VAR(double, g_cs2_64b838, $L_64b838)
ASM_VAR(double, g_cs2_64b720, $L_64b720)
ASM_VAR(double, g_cs2_64b780, $L_64b780)
ASM_VAR(float, g_cs2_64c96c, $L_64c96c)

static void Cs_ClearSteer(BYTE* s)
{
    F(int, s, 3436) = 0;
    F(int, s, 3440) = 0;
    F(int, s, 3432) = 0;
    F(int, s, 3452) = 0;
    F(int, s, 3448) = 0;
    F(int, s, 3444) = 0;
}

// the point off frame m by v
static Vec3 Cs_Through(const Matrix* m, const float* v)
{
    const float* M = m->m;
    Vec3 r;
    r.x = (float)((double)v[2] * M[6] + (double)v[1] * M[3] + (double)v[0] * M[0] + M[9]);
    r.y = (float)((double)v[2] * M[7] + (double)v[1] * M[4] + (double)v[0] * M[1] + M[10]);
    r.z = (float)((double)v[2] * M[8] + (double)v[1] * M[5] + (double)v[0] * M[2] + M[11]);
    return r;
}

// the steering of a lean (+3448) toward side q, -1 / 1 into +3436
static void Cs_Lean(BYTE* s, double q, double k)
{
    float v = (float)(q * k / (x87_fabs(q) + g_cs2_64b780));
    F(int, s, 3388) = 0;
    F(float, s, 3448) = v;
    F(int, s, 3436) = v < g_cs2_64b678 ? -1 : 1;
}

extern "C" {

// [FUN_566970] helicopter s's frame: its target car (+3248, its trailer +3252) checked, the controls (+3384..)
// reset, then by state (+3236): 1, 2 hovering with the axes and throttle settling; 3 climbing; 4 and 11 heading for
// the target; 5 the aim point +3468 at the target; 6, 7, 10 following it (+3256 area, radius +3268) by lean and
// speed (+3436, +3448, sub-state +3244), giving up outside; a pending state (+3240) after 6 / 10
void __fastcall Copter_Frame(BYTE* s, void*)
{
    F(int, s, 3148) = 0;
    BYTE* t = F(BYTE*, s, 3248);
    if (t)
    {
        t = CallC<BYTE*>(Cs_InList, t);
        F(BYTE*, s, 3248) = t;
        if (t && CallThis<int>((void (*)())F(void**, t, 0)[1], t) == 1)
            F(int, s, 3252) = F(int, F(BYTE*, s, 3248), 21880);
        else
            F(int, s, 3252) = 0;
    }
    if (!F(BYTE*, s, 3248))
    {
        int st = F(int, s, 3236);
        if (st == 6 || st == 5 || st == 7)
            CallThis(FUN_5662f0, s, 3);
    }
    F(int, s, 3392) = 1;
    F(int, s, 3388) = 1;
    F(int, s, 3384) = 1;
    Cs_ClearSteer(s);
    switch (F(int, s, 3236))
    {
    case 4:
        if (CallThis<int>(Cs_HeadFor, s))
            CallThis(FUN_5662f0, s, 3);
        Copter_566550(s, 0);
        break;
    case 11:
        if (CallThis<int>(Cs_HeadFor, s))
            Copter_ClimbState(s);
        Copter_566550(s, 0);
        break;
    case 3:
        Copter_ClimbState(s);
        Copter_566550(s, 0);
        break;
    case 5:
        Cs_ClearSteer(s);
        t = F(BYTE*, s, 3248);
        if (t)
        {
            F(int, s, 3464) = 1;
            Matrix m;
            F(Vec3, s, 3468) = F(Vec3, SceneObj_Matrix1(t + 16, 0, &m), 36);
        }
        break;
    case 6:
    case 7:
    case 10:
    {
        float dx = (float)((double)F(float, s, 3256) - F(float, s, 36));
        float dy = (float)((double)F(float, s, 3260) - F(float, s, 40));
        float r = F(float, s, 3268);
        if ((double)r * r < (double)dx * dx + (double)dy * dy)
        {
            F(int, s, 3248) = 0;
            F(int, s, 3252) = 0;
            CallThis(FUN_5662f0, s, 11);
            break;
        }
        float q = (float)((double)F(float, s, 108) * F(float, s, 76) + (double)F(float, s, 104) * F(float, s, 64)
                          + (double)F(float, s, 52) * F(float, s, 100));
        t = F(BYTE*, s, 3248);
        Vec3 tv = F(Vec3, t, 432);
        float spd = (float)Vec3_Length(&tv);
        Cs_ClearSteer(s);
        t = F(BYTE*, s, 3248);
        if (!t)
            break;
        F(int, s, 3464) = 1;
        tv = F(Vec3, t, 432);
        double spd2 = Vec3_Length(&tv);
        float k = 30.0f;
        if (F(int, s, 3236) == 10)
            k = (float)((spd2 + g_cs2_64b6e0) * g_cs2_64b918 / (spd2 + g_cs2_64d2f8));
        float off[3], off2[3], lead[3];
        off[0] = (float)((double)g_axisY.x * k);
        off[1] = (float)((double)g_axisY.y * k);
        off[2] = (float)((double)g_axisY.z * k);
        t = F(BYTE*, s, 3248);
        const Vec3& tv3 = F(Vec3, t, 432);
        lead[0] = (float)((double)tv3.x * g_cs2_64c258);
        lead[1] = (float)((double)tv3.y * g_cs2_64c258);
        lead[2] = (float)((double)tv3.z * g_cs2_64c258);
        Matrix m1, m2;
        Vec3 P1 = Cs_Through(SceneObj_Matrix1(t + 16, 0, &m1), off);
        for (int j = 0; j < 3; j++)
            off2[j] = (float)((double)off[j] * g_cs2_64c350);
        const Matrix* M2 = SceneObj_Matrix1(F(BYTE*, s, 3248) + 16, 0, &m2);
        const float* N = M2->m;
        Vec3& aim = F(Vec3, s, 3468);
        aim.x = (float)((double)off2[2] * N[6] + (double)off2[1] * N[3] + (double)off2[0] * N[0] + N[9] + lead[0]);
        aim.y = (float)((double)lead[1] + (float)((double)off2[2] * N[7] + (double)off2[1] * N[4] + (double)off2[0] * N[1] + N[10]));
        aim.z = (float)((double)lead[2] + (float)((double)off2[2] * N[8] + (double)off2[1] * N[5] + (double)off2[0] * N[2] + N[11]));
        Vec3 e;
        e.x = (float)((double)P1.x - F(float, s, 36));
        e.y = (float)((double)P1.y - F(float, s, 40));
        e.z = 0;
        float dist = (float)Vec3_Length(&e);
        if (dist > g_cs2_64b678)
        {
            Vec3 tmp;
            CallThis(Cs_MulAssign, &e, &tmp, (float)(g_cs2_64b670 / dist));
        }
        int ts = Copter_TargetState(s, 0);
        if (ts)
        {
            F(int, s, 3240) = F(int, s, 3236);
            if (ts == -1)
            {
                F(int, s, 3240) = 0;
                CallThis(FUN_5662f0, s, 4);
            }
            else if (ts == 1)
                CallThis(FUN_5662f0, s, 4);
            else if (ts == 2)
                CallThis(FUN_5662f0, s, 3);
            break;
        }
        int st = F(int, s, 3236);
        F(int, s, 3388) = 0;
        if (st == 10)
        {
            double sp = spd < g_cs2_64b670 ? g_cs2_64b670 : (double)spd;
            float turn = (float)(x87_fabs((double)q) / sp * g_cs2_64d318);
            float sk = (float)((spd + g_cs2_64b5b8) / (spd + g_cs2_64b6d0));
            double dd = dist < g_cs2_64b670 ? g_cs2_64b670 : (double)dist;
            float nearK = (float)(g_cs2_64ed90 / dd);
            Matrix m3;
            const float* T = SceneObj_Matrix1(F(BYTE*, s, 3248) + 16, 0, &m3)->m;
            double x1 = (double)F(float, s, 60) * T[10] + (double)F(float, s, 72) * T[11] + (double)F(float, s, 48) * T[9] + F(float, s, 84);
            float x2 = (float)((double)F(float, s, 64) * T[10] + (double)F(float, s, 76) * T[11] + (double)F(float, s, 52) * T[9] + F(float, s, 88));
            x1 = x87_fabs(x1);
            if (x1 < x2 * g_cs2_64b6e0)
                F(int, s, 3284) = 1;
            if (x1 > x2 * g_cs2_64f760)
                F(int, s, 3284) = 0;
            int sub = F(int, s, 3244);
            if (sub == 0)
            {
                F(int, s, 3436) = 1;
                F(float, s, 3448) = (float)(g_cs2_64b670 / ((double)nearK + turn + g_cs2_64d0f0) * sk * sk);
                if (dist > g_cs2_64c450)
                    break;
                F(int, s, 3244) = 1;
                sub = 1;
            }
            if (sub == 1)
            {
                t = F(BYTE*, s, 3248);
                Vec3 tp = F(Vec3, t, 432);
                Matrix m4;
                const Matrix* M4 = SceneObj_Matrix1(t + 16, 0, &m4);
                aim.x = (float)((double)tp.x + M4->m[9]);
                aim.y = (float)((double)tp.y + M4->m[10]);
                aim.z = (float)((double)tp.z + M4->m[11]);
                F(int, s, 3436) = 1;
                F(float, s, 3448) = (float)((double)sk * sk * sk * g_cs2_64b838);
                if (dist > k)
                    F(int, s, 3244) = 0;
            }
        }
        else if (st == 6)
        {
            if (dist < g_cs2_64b918)
                Cs_Lean(s, q, g_cs2_64b720);
            else
            {
                double sp = spd < g_cs2_64b5b8 ? g_cs2_64b5b8 : (double)spd;
                double a = x87_fabs((double)q) / sp * g_cs2_64d318;
                double dd = dist < g_cs2_64b670 ? g_cs2_64b670 : (double)dist;
                F(int, s, 3436) = 1;
                F(float, s, 3448) = (float)(g_cs2_64d318 / (g_cs2_64ed90 / dd + a + g_cs2_64d0f0));
            }
        }
        else
        {
            if (dist < g_cs2_64b918)
                Cs_Lean(s, q, g_cs2_64c96c);
            else
            {
                double dd = dist < g_cs2_64b670 ? g_cs2_64b670 : (double)dist;
                F(int, s, 3436) = 1;
                F(float, s, 3448) = (float)(g_cs2_64b5b8 / (g_cs2_64b918 / dd + g_cs2_64d0f0));
            }
        }
        break;
    }
    case 1:
    case 2:
        Copter_UpdateAxes((DWORD*)(s + 3372), 0);
        if (F(int, s, 3440) > 0 || F(int, s, 3460) > 20)
        {
            double x = Game_FrameTime() * F(int, s, 3440);
            F(int, s, 3460) = (int)((x / (Game_FrameTime() + g_cs2_64b670) + g_cs2_64b670) * F(int, s, 3460));
        }
        F(int, s, 3440) = 0;
        F(int, s, 3452) = 0;
        break;
    }
    int p = F(int, s, 3240);
    if (p == 10 || p == 6)
    {
        int st = F(int, s, 3236);
        if (st == 4 || st == 3)
        {
            if (!Copter_TargetState(s, 0))
                CallThis(FUN_5662f0, s, F(int, s, 3240));
        }
        else
            F(int, s, 3240) = 0;
    }
}

} // extern "C"

// === the helicopter's warnings to its target (FUN_568090)

extern "C" void Cw_State20944(); KING_ALIAS("_Cw_State20944", "_Car_State20944")
extern "C" void Cw_Chase6(); KING_ALIAS("_Cw_Chase6", "@Copter_Chase6@12")
extern "C" void Cw_Chase10(); KING_ALIAS("_Cw_Chase10", "@Copter_Chase10@12")
extern "C" void Cw_Chase7(); KING_ALIAS("_Cw_Chase7", "@Copter_Chase7@12")
extern "C" void Cw_Radio(); KING_ALIAS("_Cw_Radio", "_Copter_RadioCommand")
extern "C" void Cw_Matrix2(); KING_ALIAS("_Cw_Matrix2", "@SceneObj_Matrix2@12")
extern "C" void Cw_Car555500(); KING_ALIAS("_Cw_Car555500", "@Car_555500@8")
ASM_VAR(BYTE*, g_cw6d2078, $L_6d2078)               // the player
ASM_VAR(BYTE*, g_cw6ef1fc, $L_6ef1fc)               // the game (its clock +2968)
ASM_VAR(float, g_cw64c448, $L_64c448)
ASM_VAR(float, g_cw64c450, $L_64c450)
ASM_VAR(float, g_cw64c32c, $L_64c32c)
ASM_VAR(float, g_cw64d2f8, $L_64d2f8)
ASM_VAR(float, g_cw64d4e8, $L_64d4e8)
ASM_VAR(double, g_cw64d0f0, $L_64d0f0)
ASM_VAR(double, g_cw64b670, $L_64b670)
ASM_VAR(double, g_cw64da58, $L_64da58)

// the target's warning count +20944 up one, said over the radio as w when it is the player's car
static void Cw_Warn(BYTE* s, int radio, int w)
{
    if (radio && F(BYTE*, s, 3248) == F(BYTE*, g_cw6d2078, 616))
        CallC(Cw_Radio, w);
    F(int, F(BYTE*, s, 3248), 20944)++;
}

static void Cw_GiveUp(BYTE* s)
{
    int st = F(int, s, 3236);
    if (st == 6 || st == 10 || st == 7)
        CallThis(FUN_5662f0, s, 11);
}

extern "C" {

// [FUN_568090] helicopter s on target car +3248: the warning stage +3344 (since +3348) from the car's count
// +20944 (warnings, tens for its state), radio calls when it is the player, the chase by stage (6, 10, 7: follow,
// hover, block) and giving up (state 11) when it got away, stopped or is gone
void __fastcall Copter_Warnings(BYTE* s, void*)
{
    BYTE* car = F(BYTE*, s, 3248);
    if (!car || !CallC<int>(Cw_State20944, car))
    {
        F(int, s, 3344) = 0;
        Cw_GiveUp(s);
        return;
    }
    int mode = 0;
    if (Copter_TargetState(s, 0))
        F(int, s, 3344) = 0;
    else
    {
        if (F(int, s, 3344) == 0)
            F(int, s, 3344) = 1;
        if (F(int, s, 3344) == 1)
            F(float, s, 3348) = (float)Game_Time();
        BYTE* c = F(BYTE*, s, 3248);
        int r = F(int, c, 20944) % 10;
        int e = F(int, s, 3344);
        switch (r)
        {
        case 0:
            F(int, s, 3344) = 0;
            break;
        case 1:
            if (e > 2 && e != 6)
                F(int, s, 3344) = 1;
            break;
        case 2:
            if (e > 3 && e != 6)
                F(int, s, 3344) = 1;
            if (F(int, s, 3344) == 2)
            {
                if (F(int, c, 20944) < 10)
                    F(float, s, 3348) = (float)Game_Time();
                F(int, s, 3344) = 3;
            }
            break;
        case 3:
            if (e > 4 && e != 6)
                F(int, s, 3344) = 3;
            e = F(int, s, 3344);
            if (e < 4 && e > 1)
            {
                F(float, s, 3348) = (float)Game_Time();
                F(int, s, 3344) = 4;
            }
            break;
        case 4:
            if (e < 5 && e > 1)
                F(int, s, 3344) = 5;
            break;
        }
        switch (F(int, s, 3344))
        {
        case 1:
            mode = 0;
            if (!Copter_TargetState(s, 0))
                F(int, s, 3344) = 2;
            break;
        case 2:
        {
            mode = 1;
            Matrix m;
            const float* M = SceneObj_Matrix1(F(BYTE*, s, 3248) + 16, 0, &m)->m;
            Vec3 d;
            d.x = (float)((double)M[9] - F(float, s, 36));
            d.y = (float)((double)M[10] - F(float, s, 40));
            d.z = (float)((double)M[11] - F(float, s, 44));
            if (Vec3_Length(&d) < g_cw64c448 && r < 2)
                Cw_Warn(s, 1, 0);
            break;
        }
        case 3:
            mode = 1;
            if (Game_Time() - F(float, s, 3348) > g_cw64c450 && F(int, F(BYTE*, s, 3248), 20944) < 10)
                Cw_Warn(s, 1, 1);
            break;
        case 4:
            if (Game_Time() - F(float, s, 3348) > g_cw64c32c)
                Cw_Warn(s, 0, 0);
            break;
        case 5:
            mode = 2;
            break;
        case 6:
            mode = 3;
            break;
        }
    }
    BYTE* c = F(BYTE*, s, 3248);
    if (F(int, c, 20944) < 30 && F(int, s, 3344) > 2)
    {
        Vec3 v = F(Vec3, c, 432);
        if (Vec3_Length(&v) < g_cw64d0f0 && !Copter_TargetState(s, 0))
        {
            // the target stopped: still close in front of the player and lately warned, else let it go
            Matrix m2;
            BYTE* pc = F(BYTE*, g_cw6d2078, 616);
            const float* M = CallThis<const Matrix*>(Cw_Matrix2, pc + 16, &m2)->m;
            double px = F(float, s, 36), py = F(float, s, 40), pz = F(float, s, 44);
            Vec3 p;
            p.y = (float)(M[4] * py + M[7] * pz + M[1] * px + M[10]);
            p.z = (float)(M[5] * py + M[8] * pz + M[2] * px + M[11]);
            p.x = (float)(M[3] * py + M[6] * pz + M[0] * px + M[9]);
            double len = Vec3_Length(&F(Vec3, s, 100));
            bool lost = false;
            if (p.y > x87_fabs((double)p.x) && p.y > g_cw64c32c && len < g_cw64d2f8)
                lost = true;
            else if (Vec3_Length(&p) < g_cw64d4e8 || F(double, g_cw6ef1fc, 2968) - F(float, s, 3352) > g_cw64b670)
                lost = true;
            if (!lost)
            {
                F(int, s, 3344) = 6;
                mode = 3;
            }
            else
            {
                F(int, s, 3344) = 0;
                if (F(BYTE*, s, 3248) == F(BYTE*, g_cw6d2078, 616))
                    CallC(Cw_Radio, 2);
                F(int, F(BYTE*, s, 3248), 20944) = 0;
                F(int, s, 3248) = 0;
                F(int, s, 3252) = 0;
                F(int, s, 3352) = 0;
                mode = -1;
            }
        }
        else
        {
            int r = F(int, F(BYTE*, s, 3248), 20944) % 10;
            if (r == 2)
                F(int, s, 3344) = 3;
            else if (r == 3)
                F(int, s, 3344) = 4;
            else if (r == 4)
                F(int, s, 3344) = 5;
            F(float, s, 3352) = (float)F(double, g_cw6ef1fc, 2968);
        }
    }
    c = F(BYTE*, s, 3248);
    if (c && F(int, c, 20944) > 30 && F(int, c, 20944) < 40 && CallThis<double>(Cw_Car555500, c) > g_cw64da58)
    {
        F(int, F(BYTE*, s, 3248), 20944) = 0;
        F(int, s, 3248) = 0;
        F(int, s, 3252) = 0;
        Cw_GiveUp(s);
        return;
    }
    if (mode == 3)
    {
        if (F(int, s, 3236) != 7)
            CallThis(Cw_Chase7, s, F(BYTE*, s, 3248));
        return;
    }
    if (mode == 2)
    {
        if (F(int, s, 3236) != 10)
            CallThis(Cw_Chase10, s, F(BYTE*, s, 3248));
        return;
    }
    if (mode == 1)
    {
        if (F(int, s, 3236) != 6)
            CallThis(Cw_Chase6, s, F(BYTE*, s, 3248));
        return;
    }
    if (mode == -1)
        Cw_GiveUp(s);
}

} // extern "C"

// === the copter's landing place (FUN_567dc0)

struct CpPos { DWORD d[8]; };
struct CpRoom
{
    DWORD flags, m_4;
    double width;
    const char* module;
    const char* room;
    double m_24;
    int m_32, m_36, m_40, m_44;
};
extern "C" void Cp_PushRoad(); KING_ALIAS("_Cp_PushRoad", "@Route_PushRoad@16")
extern "C" void Cp_Make(); KING_ALIAS("_Cp_Make", "_AI_Make449c40")
extern "C" CpPos* __cdecl Cp_PosIdRV(CpPos* out, int room, float x, float y, float z); KING_ALIAS("_Cp_PosIdRV", "_Ai_GetPositionIdRV")
extern "C" void __cdecl Cp_Room(CpPos pos, CpRoom* out); KING_ALIAS("_Cp_Room", "_Road_GetRoadParameters")
extern "C" float* __cdecl Cp_Coords(float* out, CpPos pos); KING_ALIAS("_Cp_Coords", "_Ai_GetCoordinatesPos")
extern "C" void Cp_Name(); KING_ALIAS("_Cp_Name", "_Name68d0d8")
extern "C" void Cp_Find(); KING_ALIAS("_Cp_Find", "_Scene_FindObject")
extern "C" void Cp_SetMatrix(); KING_ALIAS("_Cp_SetMatrix", "@Copter_SetMatrix@16")
extern "C" void Cp_Set100(); KING_ALIAS("_Cp_Set100", "@Copter_SetVec100@12")
extern "C" void Cp_Set112(); KING_ALIAS("_Cp_Set112", "@Copter_SetVec112@12")
ASM_FUNC(Cp_5662f0, FUN_5662f0)
ASM_VAR(Vec3, g_cpUp, $L_702630)
ASM_VAR(Vec3, g_cpEye, $L_702650)
ASM_VAR(char, g_cp66d638, $L_66d638)
ASM_VAR(char, g_cp66d628, $L_66d628)
ASM_VAR(float, g_cp64fd4c, $L_64fd4c)
ASM_VAR(float, g_cp64fd48, $L_64fd48)
ASM_VAR(float, g_cp64c450, $L_64c450)

static void Cp_Place(BYTE* s, BYTE* obj, const Matrix* m)
{
    CallThis(Cp_SetMatrix, s, obj, m);
    CallThis(Cp_Set100, s, &g_cpEye);
    CallThis(Cp_Set112, s, &g_cpEye);
}

static double Cp_Dist2(const Vec3& a, const float* b)
{
    float dx = (float)((double)a.x - b[0]), dy = (float)((double)a.y - b[1]);
    double dz = (double)a.z - b[2];
    return (double)dx * dx + (double)dy * dy + dz * dz;
}

extern "C" {

// [FUN_567dc0] copter s (position +36, route +144 started unless +3148) finds the road under it: none sends it
// state 4; far from it, it is set over the road's point; else, away from its pad (+3256), on the pad object named
// by +3272
void __fastcall Copter_FindLanding(BYTE* s, void*)
{
    if (!F(int, s, 3148))
        CallThis(Cp_PushRoad, s + 144, F(int, F(BYTE*, s, 96), 12), s + 36);
    int room;
    CallC(Cp_Make, &room, F(int, F(BYTE*, s, 96), 12));
    if (room == -1)
    {
        if (F(int, s, 3236) != 4)
            CallThis(Cp_5662f0, s, 4);
        return;
    }
    const float* p = (const float*)(s + 36);
    CpPos pid;
    Cp_PosIdRV(&pid, room, p[0], p[1], p[2]);
    if (pid.d[0] == (DWORD)-1)
    {
        CallThis(Cp_5662f0, s, 4);
        return;
    }
    CpRoom info;
    info.flags = 0;
    info.width = 0;
    info.module = &g_cp66d638;
    info.room = &g_cp66d628;
    info.m_24 = 0;
    info.m_36 = -1;
    info.m_40 = -1;
    Cp_Room(pid, &info);
    Vec3 c;
    Cp_Coords(&c.x, pid);
    if (Cp_Dist2(c, p) > g_cp64fd4c)
    {
        Vec3 t;
        t.x = (float)(g_cpUp.x * (double)g_cp64c450 + c.x);
        float uy = (float)(g_cpUp.y * (double)g_cp64c450), uz = (float)(g_cpUp.z * (double)g_cp64c450);
        t.y = (float)((double)uy + c.y);
        t.z = (float)((double)uz + c.z);
        Matrix T;
        Cp_Place(s, F(BYTE*, s, 96), Matrix_Translation(&T, t));
        return;
    }
    if (Cp_Dist2(F(Vec3, s, 3256), p) < g_cp64fd48)
        return;
    BYTE* obj = CallC<BYTE*>(Cp_Find, 0, CallC<const char*>(Cp_Name, F(int, s, 3272)));
    if (!obj)
        return;
    Matrix T;
    Cp_Place(s, obj, Matrix_Translation(&T, F(Vec3, s, 3256)));
}

} // extern "C"

// === a bullet's flight (FUN_567440)

extern "C" void Sh_Query(); KING_ALIAS("_Sh_Query", "_Query_4e7476")
extern "C" void Sh_InList(); KING_ALIAS("_Sh_InList", "_Car_InList")
extern "C" void Sh_InList16(); KING_ALIAS("_Sh_InList16", "_Car_InListFrom16")
extern "C" void Sh_54efb0(); KING_ALIAS("_Sh_54efb0", "_Veh_54efb0")
extern "C" void Sh_Ramp(); KING_ALIAS("_Sh_Ramp", "@Ramp_Value@8")
extern "C" void Sh_555500(); KING_ALIAS("_Sh_555500", "@Car_555500@8")
extern "C" void Sh_Flasher(); KING_ALIAS("_Sh_Flasher", "@CarEffect_StartFlasher@4")
extern "C" void Sh_Camera(); KING_ALIAS("_Sh_Camera", "@View_CameraMatrix@12")
extern "C" void Sh_Play(); KING_ALIAS("_Sh_Play", "_Sound_PlayRange")
extern "C" void Sh_NodeInit(); KING_ALIAS("_Sh_NodeInit", "@Node_Init@8")
extern "C" void Sh_M1(); KING_ALIAS("_Sh_M1", "@SceneObj_Matrix1@12")
extern "C" void Sh_M2(); KING_ALIAS("_Sh_M2", "@SceneObj_Matrix2@12")
extern "C" void Sh_MulAssign(); KING_ALIAS("_Sh_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Sh_Set(); KING_ALIAS("_Sh_Set", "@Vec3_SetThis@20")
extern "C" void Sh_AddAssign(); KING_ALIAS("_Sh_AddAssign", "@Vec3_AddAssign@16")
ASM_VAR(float, g_sh67b2b0, $L_67b2b0)
ASM_VAR(double, g_sh64fd30, $L_64fd30)
ASM_VAR(Vec3, g_shUp, $L_702630)
ASM_VAR(float, g_sh67b2ac, $L_67b2ac)
ASM_VAR(float, g_sh64fd2c, $L_64fd2c)
ASM_VAR(float, g_sh67b2b4, $L_67b2b4)
ASM_VAR(float, g_sh64fd28, $L_64fd28)
ASM_VAR(float, g_sh64c254, $L_64c254)
ASM_VAR(double, g_sh64d338, $L_64d338)
ASM_VAR(double, g_sh64b840, $L_64b840)
ASM_VAR(double, g_sh64d0f0, $L_64d0f0)
ASM_VAR(double, g_sh64b670, $L_64b670)
ASM_VAR(double, g_sh64d668, $L_64d668)
ASM_VAR(double, g_sh64b5a0, $L_64b5a0)
ASM_VAR(AsmLabel, g_shDust, $L_67b604)

extern "C" {

// [FUN_567440] a bullet of object o from `from` to `to` (query mask mask, strength power): a car hit gets a push
// along the flight (16000 - rand() random, from the car's mass), turned at the hit point, its scrape sound and
// sometimes its flasher; ground hits a puff of bulletDust; the hit and the shot's sounds play at the camera
void __cdecl Bullet_Fly(int o, BYTE* from, BYTE* to, int mask, float power)
{
    __declspec(align(8)) BYTE node[208];
    CallThis(Sh_NodeInit, node);
    int r = CallC<int>(Sh_Query, o, mask, 0, from, to, node);
    int hit = r & 1;
    BYTE* c = 0;
    const Vec3& P = F(Vec3, node, 52);
    if (hit && F(BYTE*, node, 116) && (c = CallC<BYTE*>(Sh_InList16, F(BYTE*, node, 116))) != 0 &&
        CallC<BYTE*>(Sh_InList, c))
    {
        const Vec3* A = (const Vec3*)from;
        const Vec3* B = (const Vec3*)to;
        Vec3 d, n;
        d.x = (float)((double)B->x - A->x);
        d.y = (float)((double)B->y - A->y);
        d.z = (float)((double)B->z - A->z);
        n = *Vec3_Normalized(&n, &d);
        double k = (16000 - rand()) * (double)g_sh67b2b0 * g_sh64fd30;
        float uz = (float)(k * g_shUp.z), uy = (float)(k * g_shUp.y);
        double s = g_sh67b2ac;
        float zf = (float)((double)(float)(n.z * s) + uz);
        float yf = (float)((double)(float)(n.y * s) + uy);
        double X = n.x * s + k * g_shUp.x;
        float mass = F(float, c, 3284);
        double g = x87_sqrt(mass * (double)g_sh64fd2c) * power;
        Vec3 J;
        J.z = (float)(zf * g);
        J.y = (float)(yf * g);
        double Xg = X * g;
        J.x = (float)Xg;
        Vec3 v = F(Vec3, c, 432), nv;
        nv.z = (float)((double)v.z + (float)(J.z / (double)mass));
        nv.y = (float)((double)v.y + (float)(J.y / (double)mass));
        nv.x = (float)((double)v.x + (float)(Xg / mass));
        BYTE* body = c + 16;
        Matrix t1;
        const float* m1 = CallThis<const Matrix*>(Sh_M1, body, &t1)->m;
        double rz = (double)P.z - m1[11], ry = (double)P.y - m1[10], rx = (double)P.x - m1[9];
        float tz = (float)(J.y * rx - ry * J.x);
        float ty = (float)(rz * J.x - J.z * rx);
        float tx = (float)(J.z * ry - J.y * rz);
        double ts = g_sh67b2b4;
        Vec3 T, tmp;
        T.x = (float)(tx * ts);
        T.y = (float)(ty * ts);
        T.z = (float)(tz * ts);
        CallVirt(body, 144, &nv, 0);
        Vec3 w0 = F(Vec3, c, 456);
        CallThis(Sh_AddAssign, &T, &tmp, &w0);
        CallVirt(body, 152, &T, 0);
        Vec3 vp = F(Vec3, c, 432);
        const float* m2 = CallThis<const Matrix*>(Sh_M2, body, &t1)->m;
        Vec3 L;
        CallThis(Sh_Set, &L,
                     (float)((double)P.y * m2[3] + (double)P.z * m2[6] + (double)P.x * m2[0] + m2[9]),
                     (float)((double)P.y * m2[4] + (double)P.z * m2[7] + (double)P.x * m2[1] + m2[10]),
                     (float)((double)P.y * m2[5] + (double)P.z * m2[8] + (double)P.x * m2[2] + m2[11]));
        Vec3 W = F(Vec3, c, 444), wl, rot;
        CallThis(Sh_Set, &wl, (float)((double)L.z * W.y - (double)W.z * L.y),
                     (float)((double)W.z * L.x - (double)L.z * W.x), (float)((double)W.x * L.y - (double)W.y * L.x));
        Vec3 wl2 = wl;
        const Matrix* M1 = CallThis<const Matrix*>(Sh_M1, body, &t1);
        CallThis(Sh_AddAssign, &vp, &tmp, Vec3_RotateBy(&rot, &wl2, M1));
        CallC(Sh_54efb0, &P, &vp);
        BYTE* ramp = c + 21256;
        float kr = (float)(CallThis<double>(Sh_Ramp, ramp) * g_sh64fd28);
        CallThis(Sh_MulAssign, &J, &tmp, kr);
        CallVirt(body, 164, &P, &J);
        float rr = (float)(rand() * (double)g_sh64c254);
        CallThis<double>(Sh_555500, c);
        double lim = g_sh64b670 - CallThis<double>(Sh_Ramp, ramp) * power * g_sh64d668 / (power + g_sh64d0f0);
        if (lim < rr)
            CallThis(Sh_Flasher, c + 21568);
        F(int, c, 21648) = 1;
        F(Vec3, c, 21772) = P;
        F(float, c, 21824) = 1.0f;
    }
    Matrix cam;
    CallThis(Sh_Camera, g_6ef1fc, &cam);
    float vol = g_ws6cec78 ? (float)(F(short, g_ws6cec78, 386) * g_sh64b5a0) : (float)g_sh64b670;
    if (hit)
    {
        Vec3 h;
        Vec3_MulMatrix(&h, &P, &cam);
        CallC(Sh_Play, g_hitgrndSound, 0x40000000, *(int*)&vol, &h);
        if (!c)
        {
            int src = r & 2 ? F(int, node, 104) : o;
            BYTE* q = (BYTE*)operator new(192);
            BYTE* e = 0;
            if (q)
                e = CallThis<BYTE*>(FUN_5a4f00, q, g_6ef1fc, src, P.x, P.y, P.z, 0, 0x3FC00000, (const char*)g_shDust,
                                    (const char*)g_str68d1c0, 3, 0x9999999Au, 0x3fb99999);
            CallVirt(e, 24, 1);
        }
    }
    Vec3 f;
    Vec3_MulMatrix(&f, (const Vec3*)from, &cam);
    CallC(Sh_Play, g_shootSound, 0x40400000, *(int*)&vol, &f);
}

} // extern "C"

// === the game's messages to the player's side (FUN_56ffd0)

extern "C" void Gm_CarId(); KING_ALIAS("_Gm_CarId", "_Player_CarId")
extern "C" void Gm_Flags(); KING_ALIAS("_Gm_Flags", "_Player_Flags")
extern "C" void Gm_MarkHuman(); KING_ALIAS("_Gm_MarkHuman", "_AI_MarkHuman")
extern "C" void Gm_FinishPlayer(); KING_ALIAS("_Gm_FinishPlayer", "_Ai_FinishPlayer")
extern "C" void Gm_HumanFinished(); KING_ALIAS("_Gm_HumanFinished", "_Ai_HumanFinished")
extern "C" void Gm_NoticeShow(); KING_ALIAS("_Gm_NoticeShow", "_Notice_Show")
extern "C" void Gm_NoticeSet(); KING_ALIAS("_Gm_NoticeSet", "@Notice_Set@16")
extern "C" void Gm_Text(); KING_ALIAS("_Gm_Text", "_Table71e858_Get")
extern "C" void Gm_RampHigh(); KING_ALIAS("_Gm_RampHigh", "@Ramp_High@8")
extern "C" void Gm_RampPos(); KING_ALIAS("_Gm_RampPos", "@Ramp_Position@8")
extern "C" void Gm_RampSet(); KING_ALIAS("_Gm_RampSet", "@Ramp_SetValue@12")
extern "C" void Gm_Value136(); KING_ALIAS("_Gm_Value136", "_Player_Value136")
extern "C" void Gm_Set136(); KING_ALIAS("_Gm_Set136", "_Player_Set136")
extern "C" void Gm_Message(); KING_ALIAS("_Gm_Message", "_Messages_Add569720")
extern "C" void Gm_MessageAdd(); KING_ALIAS("_Gm_MessageAdd", "_Messages_Add")
extern "C" void Gm_FindObj(); KING_ALIAS("_Gm_FindObj", "_Objects_Find")
extern "C" void Gm_SoundRestart(); KING_ALIAS("_Gm_SoundRestart", "@Sound_Restart@16")
extern "C" void Gm_Cargo(); KING_ALIAS("_Gm_Cargo", "_Driver_CargoFactor")
extern "C" void Gm_ClearFlags(); KING_ALIAS("_Gm_ClearFlags", "_Player_ClearFlags")
extern "C" void Gm_53b3c0(); KING_ALIAS("_Gm_53b3c0", "@King_53b3c0@12")
extern "C" void Gm_M1(); KING_ALIAS("_Gm_M1", "@SceneObj_Matrix1@12")
extern "C" void Gm_First(); KING_ALIAS("_Gm_First", "_List68cf6c_First")
extern "C" void Gm_Next(); KING_ALIAS("_Gm_Next", "_List68cf6c_Next")
extern "C" void Gm_Field8(); KING_ALIAS("_Gm_Field8", "_Rec_Field8")
extern "C" void Gm_ResGet(); KING_ALIAS("_Gm_ResGet", "@Resources_Get@12")
extern "C" void Gm_ResAdd(); KING_ALIAS("_Gm_ResAdd", "@Resources_AddRecord@20")
extern "C" void Gm_ResDelete(); KING_ALIAS("_Gm_ResDelete", "@Resources_Delete@12")
extern "C" void Gm_Length(); KING_ALIAS("_Gm_Length", "_Vec3_Length")
extern "C" void Gm_FindTaker(); KING_ALIAS("_Gm_FindTaker", "_Vehicle_FindTaker")
extern "C" void Gm_LoadPresent(); KING_ALIAS("_Gm_LoadPresent", "_Ai_LoadPresent")
extern "C" void Gm_ApplyCargo(); KING_ALIAS("_Gm_ApplyCargo", "_Copter_ApplyCargo")
extern "C" void Gm_568f90(); KING_ALIAS("_Gm_568f90", "_Game_568f90")
extern "C" void Gm_PickLater(); KING_ALIAS("_Gm_PickLater", "_Ai_PickPresentLater")
extern "C" void Gm_Log(); KING_ALIAS("_Gm_Log", "_Sys_Log")
extern "C" void Gm_Fatal(); KING_ALIAS("_Gm_Fatal", "_Sys_Fatal")
extern "C" void Gm_ForEach(); KING_ALIAS("_Gm_ForEach", "_Array_ForEach@16")
extern "C" void Gm_ZeroThis(); KING_ALIAS("_Gm_ZeroThis", "@Vec3_ZeroThis@8")
extern "C" void Gm_InitState(); KING_ALIAS("_Gm_InitState", "@Fx_InitState@8")
extern "C" void Gm_Present(); KING_ALIAS("_Gm_Present", "_Ai_GetPresentData")
extern "C" void Gm_Vec1Free(); KING_ALIAS("_Gm_Vec1Free", "@Vec1_Free@8")
extern "C" void Gm_VecBFree(); KING_ALIAS("_Gm_VecBFree", "@VecB_FreeStorage@8")
extern "C" void Gm_HasOrder(); KING_ALIAS("_Gm_HasOrder", "_Ai_HasOrderForTown")
extern "C" void Gm_LoadOrder(); KING_ALIAS("_Gm_LoadOrder", "_Ai_LoadOrder")
extern "C" void Gm_OrderInfo(); KING_ALIAS("_Gm_OrderInfo", "_Ai_GetOrderInfo")
extern "C" void Gm_Set6fb3e8(); KING_ALIAS("_Gm_Set6fb3e8", "_Set6fb3e8")
extern "C" void Gm_Restart(); KING_ALIAS("_Gm_Restart", "@Obj53e490_Restart@8")
extern "C" void Gm_Near(); KING_ALIAS("_Gm_Near", "_Fx_VehicleNear")
extern "C" void Gm_GetState(); KING_ALIAS("_Gm_GetState", "_Player_GetState")
extern "C" void Gm_Send962(); KING_ALIAS("_Gm_Send962", "_Copter_Send962")
extern "C" void Gm_Fine(); KING_ALIAS("_Gm_Fine", "_Ai_FineForSpeeding")
ASM_VAR(BYTE*, gm_6f3498, $L_6f3498)
ASM_VAR(BYTE*, gm_6f33ac, $L_6f33ac)
ASM_VAR(BYTE*, gm_6f34a4, $L_6f34a4)
ASM_VAR(BYTE*, gm_6f34a8, $L_6f34a8)
ASM_VAR(BYTE*, gm_6cec6c, $L_6cec6c)
ASM_VAR(int, gm_66c6e0, $L_66c6e0)
ASM_VAR(int, gm_6f33b8, $L_6f33b8)
ASM_VAR(BYTE*, gm_6cec98, $L_6cec98)
ASM_VAR(int, gm_6f33bc, $L_6f33bc)
ASM_VAR(int, gm_6f33f4, $L_6f33f4)
ASM_VAR(BYTE*, gm_6d2078, $L_6d2078)
ASM_VAR(BYTE*, gm_6cecac, $L_6cecac)
ASM_VAR(BYTE*, gm_6cecb0, $L_6cecb0)
ASM_VAR(BYTE*, gm_6f3430, $L_6f3430)
ASM_VAR(BYTE*, gm_696848, $L_696848)
ASM_VAR(DWORD, gm_6f33fc, $L_6f33fc)
ASM_VAR(void*, gm_68cf6c, $L_68cf6c)
ASM_VAR(void*, gm_6f2f08, $L_6f2f08)
ASM_VAR(int, gm_6f3400, $L_6f3400)
ASM_VAR(BYTE*, gm_6f3434, $L_6f3434)
ASM_VAR(BYTE*, gm_696ca0, $L_696ca0)
ASM_VAR(int, gm_6f33c0, $L_6f33c0)
ASM_VAR(char, gm_68d168, $L_68d168)
ASM_VAR(double, gm_6f34c8, $L_6f34c8)
ASM_VAR(BYTE*, gm_696338, $L_696338)
ASM_VAR(int, gm_6f34d0, $L_6f34d0)
ASM_VAR(int, gm_6f3488, $L_6f3488)
ASM_VAR(int, gm_6f348c, $L_6f348c)
ASM_VAR(int, gm_6f3428, $L_6f3428)
ASM_VAR(BYTE, gm_7025f0, $L_7025f0)
ASM_VAR(Vec3, gm_702620, $L_702620)
ASM_VAR(double, gm_64b6e0, $L_64b6e0)
ASM_VAR(double, gm_64b6d0, $L_64b6d0)
ASM_VAR(double, gm_64b670, $L_64b670)
ASM_VAR(double, gm_64b678, $L_64b678)
ASM_VAR(float, gm_64c250, $L_64c250)
ASM_VAR(double, gm_64b5a0, $L_64b5a0)
ASM_VAR(double, gm_64b848, $L_64b848)
ASM_VAR(double, gm_64b940, $L_64b940)
ASM_VAR(double, gm_64b5b8, $L_64b5b8)
ASM_VAR(double, gm_64d0c0, $L_64d0c0)
ASM_VAR(float, gm_64c32c, $L_64c32c)
ASM_VAR(float, gm_64c264, $L_64c264)
ASM_VAR(float, gm_64c40c, $L_64c40c)
ASM_VAR(float, gm_64ff20, $L_64ff20)
ASM_VAR(double, gm_64ff18, $L_64ff18)
ASM_VAR(double, gm_64d0e0, $L_64d0e0)
ASM_VAR(double, gm_64d0e8, $L_64d0e8)
ASM_VAR(float, gm_64c320, $L_64c320)
ASM_VAR(float, gm_64fd28, $L_64fd28)
ASM_VAR(double, gm_64b868, $L_64b868)

static int Gm_Me()
{
    int id;
    CallC(Gm_CarId, &id);
    return id;
}

static const char* Gm_T(int n)
{
    return CallC<const char*>(Gm_Text, n);
}

static void Gm_Sound(const char* name)
{
    CallThis(Gm_SoundRestart, CallC<BYTE*>(Gm_FindObj, name), 0.5f, 0.5f);
}

static void Gm_Notice(int text)
{
    CallThis(Gm_NoticeSet, gm_6f3430, 8, Gm_T(text));
}

static BYTE* Gm_Car()
{
    return F(BYTE*, gm_6d2078, 616);
}

static Vec3 Gm_Pos(BYTE* o)
{
    Matrix t;
    return F(Vec3, CallThis<BYTE*>(Gm_M1, o, &t), 36);
}

static float Gm_Dist(BYTE* o, BYTE* c)
{
    Vec3 a = Gm_Pos(o), b = Gm_Pos(c), d;
    d.x = (float)((double)a.x - b.x);
    d.y = (float)((double)a.y - b.y);
    d.z = (float)((double)a.z - b.z);
    return (float)CallC<double>(Gm_Length, &d);
}

// the car's facing (702620 turned by its frame)
static Vec3 Gm_Facing(BYTE* car)
{
    Matrix t;
    const float* m = CallThis<const Matrix*>(Gm_M1, car + 16, &t)->m;
    const Vec3& f = gm_702620;
    Vec3 r;
    r.x = (float)((double)f.z * m[6] + (double)f.y * m[3] + (double)f.x * m[0]);
    r.y = (float)((double)f.z * m[7] + (double)f.y * m[4] + (double)f.x * m[1]);
    r.z = (float)((double)f.z * m[8] + (double)f.y * m[5] + (double)f.x * m[2]);
    return r;
}

// the car has room for an order or present: vtbl+136 above +164 and +140 above +148
static bool Gm_HasRoom(BYTE* car)
{
    float a = (float)CallVirt<double>(car, 164);
    if (!(CallVirt<double>(car, 136) > a))
        return false;
    float b = (float)CallVirt<double>(car, 148);
    return CallVirt<double>(car, 140) > b;
}

// the 280-byte present record Ai_GetPresentData fills
static void Gm_PresentInit(BYTE* P)
{
    F(int, P, 0) = 0;
    CallStd(Gm_ForEach, P + 4, 12, 4, Gm_ZeroThis);
    memcpy(P + 4, &gm_7025f0, 48);
    memset(P + 52, 0, 12);
    F(int, P, 64) = 0;
    F(int, P, 68) = 0;
    F(int, P, 72) = -1;
    F(int, P, 76) = -1;
    CallThis(Gm_InitState, P + 80);
    F(int, P, 272) = -1;
    F(int, P, 276) = -1;
}

// with 6f33f4 set: who's vtbl+36 when the cab's +13100 is below 64b6e0; else the notice 306 when not shown yet
static void Gm_Waiting(BYTE* cab, BYTE* who)
{
    if (gm_6f33f4)
    {
        if (F(float, cab, 13100) < gm_64b6e0)
        {
            CallVirt(who, 36);
            F(int, gm_6f3430, 60) = 0;
        }
    }
    else if (gm_6f3430 && !F(int, gm_6f3430, 60) && gm_696848 != who)
        Gm_Notice(306);
}

extern "C" {

// [FUN_56ffd0] message m (+0 id, +4 value, +12 data) for the player's side: laps, refuelling (14 / 21) and repairs
// (15) paid from the player's money, presents and orders picked up (20, 25 / 17), notices, the copter's sounds,
// fines for speeding past cameras (30) and the like; 0 when not handled
int __cdecl Game_PlayerMessage(int* m)
{
    int id = m[0];
    if ((unsigned)id > 3264)
        return 0;
    if (id == 3264)
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        gm_6f3428 = 1;
        return 1;
    }
    if ((unsigned)(id - 3) > 27)
        return 0;
    BYTE* car;
    switch (id)
    {
    case 3:
        return 1;
    case 4:
        if (gm_6f3498)
            F(DWORD, gm_6f3498, 164) &= ~0x12u;
        if (gm_6f33ac && F(int, gm_6f33ac, 500))
            F(DWORD, gm_6f33ac, 572) |= 128;
        return 1;
    case 5:
        if (gm_6f33ac && F(int, gm_6f33ac, 500))
            F(DWORD, gm_6f33ac, 572) |= 128;
        return 1;
    case 6:
        F(int, gm_6f33ac, 576) = m[1];
        if (!m[1])
        {
            if (gm_6f34a4)
                CallVirt(gm_6f34a4, 40);
        }
        else if (gm_6f34a8)
            CallVirt(gm_6f34a8, 40);
        return 1;
    case 7:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        if (gm_66c6e0 != 1 || !(CallC<int>(Gm_Flags, Gm_Me()) & 0x4000))
            return 1;
        int lap = m[1];
        if (lap != gm_6f33b8 + 1)
        {
            if (lap == 2 && !gm_6f33b8)
                CallC(Gm_NoticeShow, 6, 0, 0, 0);
            return 1;
        }
        CallC(Gm_MarkHuman, lap);
        gm_6f33b8 = m[1];
        int last = 4, laps = 3;
        if (F(int, gm_6cec6c, 1316) && F(int, gm_6cec98, 384) == 1)
        {
            laps = 1;
            last = 2;
        }
        if (gm_6f33b8 != last)
        {
            if (gm_6f33b8 > 0)
                CallC(Gm_NoticeShow, 7, 0, 0, 0);
            return 1;
        }
        if (++gm_6f33bc == laps)
        {
            gm_6f33bc = 0;
            CallC(Gm_FinishPlayer, Gm_Me());
        }
        else
            CallC(Gm_NoticeShow, 3, 0, gm_6f33bc, laps);
        gm_6f33b8 = -1;
        return 1;
    }
    case 14:
    case 21:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        BYTE* cab = F(BYTE*, Gm_Car(), 21600);
        if (!F(int, gm_6cec6c, 1304))
        {
            Gm_Waiting(cab, gm_6cecac);
            return 1;
        }
        // refuelling
        BYTE* ramp = cab + 16112;
        double hi = CallThis<double>(Gm_RampHigh, ramp);
        double x = (gm_64c250 - CallThis<double>(Gm_RampPos, ramp)) * hi;
        if (!(x > gm_64b6d0))
            return 1;
        int n = (int)(x + x);
        double money = CallC<double>(Gm_Value136, Gm_Me());
        if (!(money > gm_64b670))
            return 1;
        double sub = gm_64b678;
        if (n > money)
        {
            sub = x - money * gm_64b6e0;
            n = (int)money;
        }
        CallThis(Gm_RampSet, F(BYTE*, Gm_Car(), 21600) + 16112, (float)(hi - sub));
        CallC(Gm_Set136, Gm_Me(), money - n);
        char buf[400];
        sprintf(buf, Gm_T(11), n);
        CallC(Gm_Message, buf);
        Gm_Sound("refuelSound");
        return 1;
    }
    case 15:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        BYTE* cab = F(BYTE*, Gm_Car(), 21600);
        Gm_Waiting(cab, gm_6cecb0);
        if (!F(int, gm_6cec6c, 1304))
            return 1;
        // repairs
        gm_6f33fc |= 0x8000;
        car = Gm_Car();
        double dmg = CallVirt<double>(car, 84);
        if (!(dmg > gm_64b5a0))
            return 1;
        double money = CallC<double>(Gm_Value136, Gm_Me());
        int n = (int)(dmg * gm_64b848);
        CallVirt(car, 116);
        CallVirt(Gm_Car(), 120);
        double cargo = 0;
        if (CallC<int>(Gm_Flags, Gm_Me()) & 0x2000000)
            cargo = CallC<double>(Gm_Cargo, Gm_Me());
        double cost = cargo + n;
        double left;
        if (money < cost)
        {
            n = (int)money;
            left = dmg - money * gm_64b940;
        }
        else
        {
            dmg += gm_64b5b8;
            n = (int)cost;
            if (CallC<int>(Gm_Flags, Gm_Me()) & 0x2000000)
            {
                CallC(Gm_MessageAdd, Gm_T(119));
                CallC(Gm_ClearFlags, Gm_Me());
            }
            left = dmg;
        }
        CallThis(Gm_53b3c0, gm_6d2078, (float)left);
        if (!F(int, gm_6cec6c, 1316))
        {
            CallC(Gm_Set136, Gm_Me(), money - n);
            char buf[400];
            sprintf(buf, Gm_T(10), n);
            CallC(Gm_Message, buf);
        }
        Gm_Sound("repairSound");
        return 1;
    }
    case 16:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        float h = *(float*)m[3];
        if (Gm_Pos(Gm_Car() + 16).z < h)
            gm_6f3428 = 1;
        return 1;
    }
    case 25:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        gm_6f2f08 = gm_68cf6c;
        car = Gm_Car();
        if (CallVirt<int>(car, 4) != 1 || F(int, Gm_Car(), 21880))
            return 1;
        void* best = gm_68cf6c;
        double bestd = 1000.0;
        void* it;
        CallC(Gm_First, &it);
        while (it != gm_68cf6c)
        {
            if (CallC<int>(Gm_Field8, it) & 32)
            {
                BYTE* rec = CallThis<BYTE*>(Gm_ResGet, F(BYTE*, gm_6cec6c, 1356) + 704, *F(int*, it, 8));
                BYTE* o = F(BYTE*, rec, 28);
                if (o)
                {
                    double d = Gm_Dist(o + 16, Gm_Car() + 16);
                    if (d < gm_64d0c0 && d < bestd)
                    {
                        bestd = d;
                        best = it;
                    }
                }
            }
            void* nx;
            it = *CallC<void**>(Gm_Next, &nx, it);
        }
        if (best == gm_68cf6c)
            return 1;
        int f = CallC<int>(Gm_Field8, best) & 64;
        if (f)
        {
            F(int, gm_6f3434, 32) = m[0];
            gm_6f2f08 = best;
        }
        if (!gm_6f3400)
            return 1;
        gm_6f3400 = 0;
        if (CallC<int>(Gm_FindTaker, Gm_Car(), 0))
            return 1;
        if (f)
        {
            double out;
            CallC(Gm_LoadPresent, Gm_Me(), best, &out);
            CallC(Gm_ApplyCargo);
            CallC(Gm_568f90);
        }
        CallC(Gm_PickLater, Gm_Me(), best);
        gm_6f2f08 = gm_68cf6c;
        return 1;
    }
    case 20:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        void* it;
        CallC(Gm_First, &it);
        for (;;)
        {
            if (it == gm_68cf6c)
                return 1;
            int f = CallC<int>(Gm_Field8, it);
            if ((f & 64) && !(f & 32))
            {
                BYTE* rec = CallThis<BYTE*>(Gm_ResGet, F(BYTE*, gm_6cec6c, 1356) + 704, *F(int*, it, 8));
                if (!rec)
                    CallC(Gm_Log, "present absent, in my opinion", *F(int*, it, 8));
                else if (F(BYTE*, rec, 28) && Gm_Dist(F(BYTE*, rec, 28), Gm_Car() + 16) < gm_64c32c)
                    break;
            }
            void* nx;
            it = *CallC<void**>(Gm_Next, &nx, it);
        }
        __declspec(align(8)) BYTE P[288];
        Gm_PresentInit(P);
        CallC(Gm_Present, Gm_Me(), it, P);
        car = Gm_Car();
        void* next = it;
        if (gm_6f33f4 && Gm_HasRoom(car))
        {
            double out;
            CallC(Gm_LoadPresent, Gm_Me(), it, &out);
            CallC(Gm_ApplyCargo);
            Gm_Sound("orderSound");
            CallC(Gm_568f90);
            gm_6f33f4 = 0;
            BYTE* rec = CallThis<BYTE*>(Gm_ResGet, gm_696ca0 + 704, *F(int*, it, 8));
            CallC(Gm_PickLater, Gm_Me(), it);
            if (!rec)
                CallC(Gm_Fatal, "Incorrect vrPresentId", 0);
            if (BYTE* o = F(BYTE*, rec, 28))
            {
                CallVirt(o, 28, 0);
                CallThis(Gm_ResAdd, gm_696ca0 + 704, 0, rec, 0);
                CallThis(Gm_ResDelete, gm_696ca0 + 704, F(int, rec, 4));
            }
            next = gm_68cf6c;
        }
        else
        {
            if (gm_6f3430 && !F(int, gm_6f3430, 60))
                Gm_Notice(57);
            F(int, gm_6f3434, 32) = m[0];
        }
        gm_6f2f08 = next;
        CallThis(Gm_Vec1Free, P + 168);
        CallThis(Gm_Vec1Free, P + 156);
        CallThis(Gm_Vec1Free, P + 144);
        CallThis(Gm_VecBFree, P + 132);
        return 1;
    }
    case 17:
    {
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        int a = m[1];
        if (a % 100 == 0 && (CallC<int>(Gm_HasOrder, Gm_Me(), a / 100) & 0xFF))
        {
            if (CallVirt<double>(Gm_Car(), 164) > gm_64c264)
            {
                double k = -10.0, out;
                CallC(Gm_LoadOrder, Gm_Me(), m[1] / 100, m[1] % 100, &k, &out);
                CallC(Gm_ApplyCargo);
                CallC(Gm_568f90);
            }
            F(int, gm_6f3434, 32) = m[0];
            F(int, gm_6f3434, 36) = m[1];
            return 1;
        }
        __declspec(align(8)) BYTE Q[200];
        CallThis(Gm_InitState, Q);
        CallC(Gm_OrderInfo, Gm_Me(), m[1] / 100, m[1] % 100, Q);
        if (F(int, Q, 0) != -1)
        {
            car = Gm_Car();
            if (gm_6f33f4 && Gm_HasRoom(car))
            {
                double k = 10.0, out;
                CallC(Gm_LoadOrder, Gm_Me(), m[1] / 100, m[1] % 100, &k, &out);
                CallC(Gm_ApplyCargo);
                Gm_Sound("orderSound");
                CallC(Gm_568f90);
                gm_6f33f4 = 0;
            }
            if (gm_6f3430 && !F(int, gm_6f3430, 60))
                Gm_Notice(57);
            F(int, gm_6f3434, 32) = m[0];
            F(int, gm_6f3434, 36) = m[1];
        }
        CallThis(Gm_Vec1Free, Q + 88);
        CallThis(Gm_Vec1Free, Q + 76);
        CallThis(Gm_Vec1Free, Q + 64);
        CallThis(Gm_VecBFree, Q + 52);
        return 1;
    }
    case 19:
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        if (F(float, F(BYTE*, Gm_Car(), 21600), 13100) > gm_64c40c)
            return 0;
        F(int, gm_6f3434, 32) = m[0];
        F(int, gm_6f3434, 36) = m[1];
        return 1;
    case 22:
        if (gm_6f3498)
            F(DWORD, gm_6f3498, 164) &= ~0x12u;
        if (gm_6f33ac)
        {
            if (F(int, gm_6f33ac, 500))
                F(DWORD, gm_6f33ac, 572) |= 128;
            if (gm_6f33ac)
                F(DWORD, gm_6f33ac, 572) |= 64;
        }
        gm_6f33c0 = 1;
        return 1;
    case 23:
    {
        const float* p = (const float*)m[3];
        if (!gm_6f33ac || !p)
            return 1;
        Vec3 c = Gm_Pos(Gm_Car() + 16);
        double d = ((double)c.z * p[2] + (double)c.y * p[1] + (double)c.x * p[0] + p[3]) * gm_64ff20;
        if (d < gm_64c264)
            d = gm_64c264;
        else if (d > gm_64c250)
            d = gm_64c250;
        CallC(Gm_Set6fb3e8, m[1] - 1, (float)(gm_64c250 - d));
        return 1;
    }
    case 26:
        if (F(int, gm_6cec6c, 1300) > 1)
            return 0;
        if (!m[1])
        {
            if (gm_68d168)
                CallThis(Gm_Restart, F(BYTE*, gm_6d2078, 1632));
            return 1;
        }
        if (CallC<int>(Gm_Flags, Gm_Me()) & 0x4000)
            CallC(Gm_HumanFinished);
        return 1;
    case 28:
        gm_6f34c8 = 0.4;
        return 1;
    case 29:
    {
        int a = m[1];
        int on = a % 10 == 0 ? F(int, gm_696338, 36) : F(int, gm_696338, 36) ^ 1;
        int k = a / 10;
        if (k == 4)
        {
            if (!gm_6f34d0)
            {
                Vec3 c = Gm_Pos(Gm_Car() + 16);
                if (CallC<int>(Gm_Near, c.x, c.y, c.z))
                {
                    BYTE* pr = (BYTE*)Gm_Me();
                    F(double, pr, 248) = F(double, pr, 248) + gm_64ff18;
                }
            }
            gm_6f34d0 = 1;
            return 1;
        }
        if (k == 1 || k == 3)
            on ^= 1;
        gm_6f34d0 = on ? 1 : 0;
        return 1;
    }
    case 30:
    {
        car = Gm_Car();
        BYTE* cab = F(BYTE*, car, 21600);
        if (!cab)
            return 1;
        const float* p = (const float*)m[3];
        if (m[1] == -1)
        {
            Matrix t;
            const float* M = CallThis<const Matrix*>(Gm_M1, car + 16, &t)->m;
            const Vec3& f = gm_702620;
            double fx = (double)f.z * M[6] + (double)f.y * M[3] + (double)f.x * M[0];
            double fy = (double)f.z * M[7] + (double)f.y * M[4] + (double)f.x * M[1];
            double fz = (double)f.z * M[8] + (double)f.y * M[5] + (double)f.x * M[2];
            if (p[3] * fz + p[2] * fy + p[1] * fx > gm_64b678)
                F(float, car, 20952) = 0.5f;
            return 1;
        }
        double speed = F(float, cab, 13100);
        Vec3 fw = Gm_Facing(car);
        __declspec(align(8)) BYTE st[304];
        for (int i = 0; i < 19; i++)
        {
            F(int, st, i * 16) = 0;
            F(double, st, i * 16 + 8) = 1.0;
        }
        CallC(Gm_GetState, Gm_Me(), st);
        if (F(int, st, 256))
            return 1;
        if (!(p[1] * (double)fw.x + p[3] * (double)fw.z + (double)fw.y * p[2] > gm_64b678))
            return 1;
        F(float, Gm_Car(), 20952) = 0.5f;
        BYTE* rec = CallThis<BYTE*>(Gm_ResGet, gm_696ca0 + 704, m[1]);
        double lim = speed * gm_64d0e0;
        if (!(p[0] < lim) || !rec)
            return 1;
        BYTE* o = F(BYTE*, rec, 28);
        if (!o || (F(BYTE, o, 296) & 1) || F(int, o, 408))
            return 1;
        F(int, o, 408) = 1;
        F(int, o, 368) = F(int, gm_696ca0, 2968);
        F(int, o, 372) = F(int, gm_696ca0, 2972);
        CallC(Gm_Send962, F(int, o, 344), 0);
        CallC(Gm_Send962, F(int, o, 344), F(int, o, 408));
        gm_6f3488 = (int)p[0] - 5;
        gm_6f348c = (int)lim;
        int k = 1;
        double over = (double)p[0] - gm_64c320;
        if ((over + gm_64fd28) * gm_64d0e8 < speed)
            k = 3;
        else if ((over + gm_64c32c) * gm_64d0e8 < speed)
            k = 2;
        CallC(Gm_Fine, Gm_Me(), k * gm_64b868);
        return 1;
    }
    default:
        return 0;
    }
}

} // extern "C"

// === the dashboard (FUN_569ab0)

extern "C" void Hd_Text(); KING_ALIAS("_Hd_Text", "_Text_Draw")
extern "C" void Hd_Str(); KING_ALIAS("_Hd_Str", "_Table71e858_Get")
extern "C" void Hd_Width(); KING_ALIAS("_Hd_Width", "@Font_TextWidth@12")
extern "C" void Hd_Bitmap(); KING_ALIAS("_Hd_Bitmap", "_Bitmap_Draw")
extern "C" void Hd_Image(); KING_ALIAS("_Hd_Image", "_Gfx_DrawImage")
extern "C" void Hd_DrawText(); KING_ALIAS("_Hd_DrawText", "_Font_DrawText")
extern "C" void Hd_PlayerData(); KING_ALIAS("_Hd_PlayerData", "_Ai_GetPlayerData")
extern "C" void Hd_Flags(); KING_ALIAS("_Hd_Flags", "_Player_Flags")
extern "C" void Hd_M1(); KING_ALIAS("_Hd_M1", "@SceneObj_Matrix1@12")
extern "C" void Hd_Normalize(); KING_ALIAS("_Hd_Normalize", "@Vec3_NormalizeThis@8")
extern "C" void Hd_Sound(); KING_ALIAS("_Hd_Sound", "@Sound_Play@20")
ASM_VAR(BYTE*, g_hd69684c, $L_69684c)
ASM_VAR(BYTE*, g_hd696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_hd696848, $L_696848)
ASM_VAR(BYTE*, g_hd6cecac, $L_6cecac)
ASM_VAR(BYTE*, g_hd6cecb0, $L_6cecb0)
ASM_VAR(BYTE*, g_hd6cecb4, $L_6cecb4)
ASM_VAR(int, g_hdBlink, $L_6f3440)
ASM_VAR(int, g_hdBeep, $L_6f343c)
ASM_VAR(BYTE*, g_hd6d2078, $L_6d2078)
ASM_VAR(int, g_hd696c54, $L_696c54)
ASM_VAR(int, g_hdPos, $L_67b72c)
ASM_VAR(int, g_hdOf, $L_6f3444)
ASM_VAR(int, g_hdCount, $L_6f3458)
ASM_VAR(double, g_hdTime, $L_6f3460)
ASM_VAR(double, g_hdBest, $L_6f3468)
ASM_VAR(int, g_hdSpeed, $L_6f3448)
ASM_VAR(int, g_hd6f3328, $L_6f3328)
ASM_VAR(int, g_hdGear, $L_6f344c)
ASM_VAR(int, g_hdAuto, $L_6f3450)
ASM_VAR(int, g_hdRpm, $L_6f3454)
ASM_VAR(int, g_hdSignal, $L_6f345c)
ASM_VAR(int, g_hd68b990, $L_68b990)
ASM_VAR(int, g_hd6ce330, $L_6ce330)
ASM_VAR(int, g_hd6ce334, $L_6ce334)
ASM_VAR(BYTE*, g_hd6cec98, $L_6cec98)
ASM_VAR(BYTE*, g_hd6cec6c, $L_6cec6c)
ASM_VAR(double, g_hd696348, $L_696348)
ASM_VAR(int, g_hdLap, $L_6f33b8)
ASM_VAR(Vec3, g_hdZero, $L_702650)
ASM_VAR(double, g_hd64b658, $L_64b658)
ASM_VAR(double, g_hd64b670, $L_64b670)
ASM_VAR(double, g_hd64d0e0, $L_64d0e0)
ASM_VAR(float, g_hd64fe30, $L_64fe30)
ASM_VAR(double, g_hd64b678, $L_64b678)
ASM_VAR(double, g_hd64fe28, $L_64fe28)
ASM_VAR(double, g_hd64b958, $L_64b958)
ASM_VAR(float, g_hd64c250, $L_64c250)
ASM_VAR(double, g_hd64fe20, $L_64fe20)
ASM_VAR(double, g_hd64ca80, $L_64ca80)
ASM_VAR(double, g_hd64b6a0, $L_64b6a0)
ASM_VAR(double, g_hd64b7f8, $L_64b7f8)
ASM_VAR(double, g_hd64d338, $L_64d338)
ASM_VAR(double, g_hd64b6e0, $L_64b6e0)
ASM_VAR(float, g_hd64fe1c, $L_64fe1c)
ASM_VAR(float, g_hd64fe18, $L_64fe18)
ASM_VAR(float, g_hd64fe14, $L_64fe14)
ASM_VAR(float, g_hd64fe10, $L_64fe10)
ASM_VAR(float, g_hd64c450, $L_64c450)
ASM_VAR(float, g_hd64c264, $L_64c264)
ASM_VAR(double, g_hd64b6f8, $L_64b6f8)
ASM_VAR(double, g_hd64fe08, $L_64fe08)
ASM_VAR(double, g_hd64fe00, $L_64fe00)
ASM_VAR(double, g_hd64fdf8, $L_64fdf8)

static BYTE* g_hdSurf;

static void Hd_Draw(int x, int y, BYTE* font, const char* s)
{
    CallC(Hd_Text, g_hdSurf, x, y, font, s, 0, 0);
}

static const char* Hd_T(int n)
{
    return CallC<const char*>(Hd_Str, n);
}

// digits moved up to the dashboard font's lit digits
static void Hd_Lit(char* s)
{
    for (; *s; s++)
        *s += 16;
}

static int Hd_W(BYTE* font, const char* s)
{
    return CallThis<int>(Hd_Width, font, s);
}

extern "C" {

// [FUN_569ab0] dashboard d (fonts +1288 / +1292, frames +1276) on the screen surface: refreshed every so many frames
// (blinking 6f3440 on), the race position, laps and times, the speed (km/h or mph), the gear, the revs, the lights,
// damage and temperature, then either the turn signal or, in the network race, the arrow to the next checkpoint;
// a beep when something blinks, and the message line (+556, its scroll +1196) in its frame
void __fastcall Dash_Draw(BYTE* d, void*)
{
    BYTE* scr = g_hd69684c;
    CallVirt(F(BYTE*, d, 40), 20);
    int W = CallVirt<int>(scr, 20);
    int bx = 170, by = 50, x0 = -10, bgx = -479;
    if (W > 1204)
    {
        bx = 1161;
        by = 11;
        bgx = 0;
        x0 = 469;
    }
    else if (W > 800)
    {
        bx = 682;
        by = 11;
        bgx = -479;
    }
    else if (W > 640)
    {
        bx = 250;
        by = 50;
        x0 = 70;
        bgx = -399;
    }
    g_hdSurf = F(BYTE*, scr, 12);
    int frame = ++F(int, d, 1276);
    BYTE* r = g_hd696ca0;
    double t = F(double, r, 2968) / (F(int, r, 2992) + 1);
    if (t < g_hd64b658)
        t = g_hd64b658;
    int period = (int)(g_hd64b670 / t);
    if (g_hd696848 == g_hd6cecb0 || g_hd696848 == g_hd6cecac || g_hd696848 == g_hd6cecb4)
        period = 1;
    int rem = frame % period;
    char buf[100];
    bool skip = false;
    if (rem == 0)
        g_hdBlink ^= 1;
    else if (rem != 1 && frame >= 10)
        skip = true;
    BYTE* f1 = 0;
    BYTE* f2 = 0;
    if (!skip)
    {
        g_hdBeep = 0;
        CallC(Hd_Bitmap, g_hdSurf, bgx, 0, F(BYTE*, d, 1256));
        BYTE* car = F(BYTE*, g_hd6d2078, 616);
        if (!car)
            return;
        int mph = g_hd696c54 == 0;
        f1 = F(BYTE*, d, 1288);
        f2 = F(BYTE*, d, 1292);
        if (F(int, d, 1276) % period == 0 || F(int, d, 1276) < 10)
        {
            int* plp = &F(int, car, 20636);
            if (*plp != g_hd68b990)
            {
                BYTE pd[256];
                CallC(Hd_PlayerData, *plp, pd);
                if (pd[136])
                {
                    g_hdPos = F(int, (BYTE*)*plp, 528) + 1;
                    g_hdOf = F(int, (BYTE*)*plp, 524) + 1;
                }
                else
                    g_hdPos = -1;
                int n = (F(int, (BYTE*)*plp, 148) - F(int, (BYTE*)*plp, 144)) >> 3;
                g_hdTime = F(double, pd, 120);
                g_hdCount = n;
                if (n > 0)
                    g_hdBest = F(double, pd, 64);
            }
            double div = mph ? 1.6 : 1.0;
            Vec3 v = F(Vec3, car, 432);
            g_hdSpeed = (int)(Vec3_Length(&v) * g_hd64d0e0 / div);
            g_hd6f3328 = F(int, car, 21020);
            g_hdGear = F(int, car, 20996);
            g_hdAuto = F(int, car, 21024);
            g_hdRpm = (int)(F(float, car, 20968) * (double)g_hd64fe30);
            g_hdSignal = F(int, (BYTE*)*plp, 660);
        }
        if (F(int, car, 20636) != g_hd68b990)
        {
            if (g_hdPos >= 0)
            {
                Hd_Draw(x0 + 26, 23, f1, Hd_T(89));
                sprintf(buf, "%d", g_hdPos);
                Hd_Lit(buf);
                Hd_Draw(x0 + 26, 43, f2, buf);
                const char* of = Hd_T(95);
                Hd_Draw(x0 - Hd_W(f1, Hd_T(95)) / 2 + 50, 38, f1, of);
                sprintf(buf, "%d", g_hdOf);
                Hd_Lit(buf);
                Hd_Draw(x0 + 63, 43, f2, buf);
            }
            Hd_Draw(x0 + 85, 23, f1, Hd_T(90));
            int s = (int)g_hdTime;
            sprintf(buf, "%02d:%02d", s / 60, s % 60);
            Hd_Lit(buf);
            Hd_Draw(x0 + 85, 43, f2, buf);
            if (g_hdCount > 0)
            {
                Hd_Draw(x0 + 154, 23, f1, Hd_T(101));
                sprintf(buf, "%d", g_hdCount);
                Hd_Draw(x0 + 210, 23, f1, buf);
                int b = (int)g_hdBest;
                sprintf(buf, "%02d:%02d", b / 60, b % 60);
                Hd_Lit(buf);
                Hd_Draw(x0 + 158, 43, f2, buf);
            }
        }
        sprintf(buf, "%d", g_hdSpeed);
        Hd_Draw(x0 - Hd_W(f2, buf) + 287, 38, f2, buf);
        Hd_Draw(x0 + 291, 40, f1, mph ? "MpH" : Hd_T(93));
        if (F(int, car, 21600) && g_hd6f3328 && g_hdBlink)
            Hd_Draw(x0 + 315, 40, f2, "[\\");
        int g = g_hdGear;
        int gc = g > 1 ? g + 47 : (g ? 78 : 82);
        sprintf(buf, g_hdAuto ? Hd_T(91) : Hd_T(92), gc);
        Hd_Draw(x0 + 361 + (g_hdAuto ? 15 : 0), 23, f1, buf);
        sprintf(buf, "%d", g_hdRpm);
        Hd_Lit(buf);
        Hd_Draw(x0 - Hd_W(f2, buf) + 386, 43, f2, buf);
        Hd_Draw(x0 + 386, 40, f1, Hd_T(94));
        int lights = F(int, d, 1268);
        const char* ls;
        if (!lights)
            ls = "KL";
        else if (lights == 1)
            ls = "MN";
        else
        {
            g_hdBeep = 1;
            ls = g_hdBlink ? "KL" : "MN";
        }
        Hd_Draw(x0 + 458, 40, f2, ls);
        double dmg = F(float, car, 20988);
        int n = 0;
        if (dmg > g_hd64b678)
        {
            n = 2 - (int)(dmg * g_hd64fe28);
            if (n > 9)
                n = 9;
        }
        sprintf(buf, "?%c", n + 80);
        if (n > 3 || g_hdBlink)
            Hd_Draw(x0 + 512, 40, f2, buf);
        if (dmg < g_hd64b958)
            g_hdBeep = 1;
        int m = (int)((g_hd64c250 - (double)F(float, car, 20992)) * g_hd64fe20);
        if (m < 0)
            m = 0;
        else if (m > 9)
            m = 9;
        BYTE* cab = F(BYTE*, car, 21600);
        if (cab && F(int, cab, 2252) >= g_hd6ce330 && F(int, cab, 2252) < g_hd6ce334)
            sprintf(buf, "%c%c", 95, m + 80);
        else
            sprintf(buf, "O%c", m + 80);
        if (m > 3 || g_hdBlink)
            Hd_Draw(x0 + 553, 40, f2, buf);
        int ax = x0 + 608;
        int pl = F(int, car, 20636);
        if (pl != g_hd68b990 && (CallC<DWORD>(Hd_Flags, pl) & 0x2000))
        {
            if (!(F(int, g_hd6cec98, 384) == 2 && F(int, g_hd6cec6c, 1316)))
            {
                double st = g_hd696348;
                if (st > g_hd64ca80 && st < g_hd64b6a0)
                    g_hdBeep = 1;
                bool on = !(st > g_hd64b6a0);
                if (on && st > g_hd64b7f8)
                    on = ((int)(((st - g_hd64b7f8) * g_hd64d338 + g_hd64b6e0) * F(double, g_hd696ca0, 2968)) % 2) != 0;
                if (on)
                {
                    static const char* const arrows[4] = {"<", "]", ";", "="};
                    if ((unsigned)(g_hdSignal - 1) <= 3)
                        Hd_Draw(ax, 40, f2, arrows[g_hdSignal - 1]);
                }
            }
        }
        else if (F(int, g_hd6cec6c, 1316) && F(int, g_hd6cec98, 384) == 1)
        {
            Matrix M;
            CallThis(Hd_M1, F(BYTE*, g_hd6d2078, 616) + 16, &M);
            Vec3 dv;
            if (g_hdLap == 0 || g_hdLap == 1)
            {
                float tx = g_hdLap == 0 ? g_hd64fe1c : g_hd64fe14, ty = g_hdLap == 0 ? g_hd64fe18 : g_hd64fe10;
                dv.x = (float)((double)tx - M.m[9]);
                dv.y = (float)((double)ty - M.m[10]);
                dv.z = (float)((double)M.m[11] - M.m[11]);
            }
            else
                dv = g_hdZero;
            if (Vec3_Length(&dv) > g_hd64c450)
            {
                CallThis(Hd_Normalize, &dv);
                Matrix inv;
                const float* q = Matrix_Inverse(&inv, &M)->m;
                float lx = (float)((double)dv.z * q[6] + (double)dv.y * q[3] + (double)dv.x * q[0]);
                double ly = (double)dv.z * q[7] + (double)dv.y * q[4] + (double)dv.x * q[1];
                const char* a;
                if (ly > g_hd64c264)
                    a = lx > g_hd64b6f8 ? "de" : lx > g_hd64fe08 ? "bc" : lx > g_hd64fe00 ? "`a" : lx > g_hd64fdf8 ? "no" : "lm";
                else
                    a = lx > g_hd64b6f8 ? "de" : lx > g_hd64fe08 ? "fg" : lx > g_hd64fe00 ? "hi" : lx > g_hd64fdf8 ? "jk" : "lm";
                Hd_Draw(ax, 40, f2, a);
            }
        }
    }
    f1 = F(BYTE*, d, 1288);
    if (g_hdBeep)
        CallThis(Hd_Sound, F(BYTE*, d, 1284), 0.6f, 0.6f, 0);
    Hd_Draw(20, 20, f1, " ");
    int sc = F(int, d, 1196);
    if (sc == 1)
    {
        if (!F(int, d, 1216))
            return;
        CallC(Hd_Image, g_hdSurf, bx, by, F(BYTE*, d, 1264), -1);
        CallC(Hd_DrawText, g_hdSurf, bx + 34, by + 23, 247, f1, d + 556, 0, 0);
        return;
    }
    CallC(Hd_Image, g_hdSurf, bx, by, F(BYTE*, d, 1264), -1);
    sc = F(int, d, 1196);
    if (sc > 1 && sc < 20 && ((sc / 2) & 1))
        CallC(Hd_Image, g_hdSurf, bx + 5, by + 6, F(BYTE*, d, 1260), -1);
    sc = F(int, d, 1196);
    if (sc == 1)
        return;
    char line[160];
    int i = 0;
    for (; i < sc - 1; i++)
    {
        char c = F(char, d, 556 + i);
        line[i] = c;
        if (!c)
        {
            if (sc < 1000)
                F(int, d, 1196) = 1000;
            break;
        }
    }
    line[i] = 0;
    int w = Hd_W(f1, line);
    int off = 0;
    if (w > 247)
        off = (w - 247) / F(int, f1, 24) + 1;
    Hd_Draw(bx + 34, by + 23, f1, line + off);
}

} // extern "C"

// === the helicopter's first initialization (FUN_561800 with its tails up to $L_561a7c)

extern "C" void Hi_Base(); KING_ALIAS("_Hi_Base", "@Copter561530_Init@8")
extern "C" void Hi_Room(); KING_ALIAS("_Hi_Room", "_AI_Make449c40")
extern "C" void Hi_RotX(); KING_ALIAS("_Hi_RotX", "_Matrix_RotationX")
extern "C" void Hi_Fatal(); KING_ALIAS("_Hi_Fatal", "_World_Fatal")
extern "C" void Hi_Log(); KING_ALIAS("_Hi_Log", "_Sys_Log")
extern "C" void Hi_Find(); KING_ALIAS("_Hi_Find", "_Scene_FindObject")
extern "C" void Hi_Stack(); KING_ALIAS("_Hi_Stack", "@Stack_Init@16")
struct HiPos { int d[8]; };
extern "C" int __cdecl Hi_PosRV(HiPos* out, int room, Vec3 p); KING_ALIAS("_Hi_PosRV", "_Ai_GetPositionIdRV")
extern "C" int __cdecl Hi_PosV(HiPos* out, Vec3 p); KING_ALIAS("_Hi_PosV", "_Ai_GetPositionIdV")
extern "C" int __cdecl Hi_RoadParams(HiPos pos, BYTE* info); KING_ALIAS("_Hi_RoadParams", "_Road_GetRoadParameters")
ASM_VAR(Vec3, g_hiZero, $L_702650)
ASM_VAR(Vec3, g_hiUp, $L_702630)
ASM_VAR(Vec3, g_hiAhead, $L_702620)
ASM_VAR(char, g_hiNoRoom, $L_67b34c)
ASM_VAR(char, g_hiNoRoad, $L_67b2dc)
ASM_VAR(BYTE**, g_hiRoads, $L_68cf38)

static int Hi_RoomOf(BYTE* a)
{
    int r[8];
    CallC(Hi_Room, r, F(void*, a, 12));
    return r[0];
}

extern "C" {

// [FUN_561800..$L_561a7c] helicopter s in room a with frame m: the base, its frame (+0) and inverse (+48), the
// axes, the nose (+3316, the ahead axis tilted down), the room's position on the road (a fatal error when the room or
// the road is unknown); off the road a scene object of the road's room is required, else +3548 is set; on the road
// the stack (+144) of the road's ends
void __fastcall Copter_Init0(BYTE* s, void*, BYTE* a, const Matrix* m)
{
    CallThis(Hi_Base, s);
    F(BYTE*, s, 96) = a;
    F(Vec3, s, 3256) = F(Vec3, m, 36);
    F(int, s, 3272) = Hi_RoomOf(a);
    memcpy(s, m, 48);
    Matrix t;
    memcpy(s + 48, Matrix_Inverse(&t, (const Matrix*)s), 48);
    F(Vec3, s, 3504) = g_hiZero;
    F(Vec3, s, 3516) = g_hiUp;
    F(Vec3, s, 3528) = g_hiAhead;
    DWORD ab = 3192459227u;
    const float* R = CallC<const Matrix*>(Hi_RotX, &t, *(float*)&ab)->m;
    const Vec3& v = g_hiAhead;
    Vec3 n;
    n.x = (float)((double)v.z * R[6] + (double)v.y * R[3] + (double)v.x * R[0]);
    n.y = (float)((double)v.z * R[7] + (double)v.y * R[4] + (double)v.x * R[1]);
    n.z = (float)((double)v.z * R[8] + (double)v.y * R[5] + (double)v.x * R[2]);
    F(Vec3, s, 3316) = n;
    F(int, s, 3236) = 1;
    F(int, s, 3148) = 0;
    F(BYTE*, s, 3356) = F(BYTE*, s, 96);
    F(Vec3, s, 3360) = F(Vec3, s, 3256);
    int room = Hi_RoomOf(F(BYTE*, s, 96));
    if (room == -1)
        CallC(Hi_Fatal, &g_hiNoRoom);
    HiPos pos;
    Hi_PosRV(&pos, room, F(Vec3, s, 36));
    if (pos.d[0] == -1)
    {
        char buf[256];
        sprintf(buf, &g_hiNoRoad, F(const char*, F(BYTE*, s, 96), 12), (double)F(float, s, 36),
                (double)F(float, s, 40), (double)F(float, s, 44));
        Hi_PosV(&pos, F(Vec3, s, 36));
        if (pos.d[0] == -1)
            CallC(Hi_Fatal, buf);
        CallC(Hi_Log, buf, 0);
        BYTE info[48];
        memset(info, 0, sizeof info);
        F(const char*, info, 16) = "UNDEFINED_MODULE";
        F(const char*, info, 20) = "UNDEFINED_ROOM";
        F(int, info, 36) = -1;
        F(int, info, 40) = -1;
        Hi_RoadParams(pos, info);
        if (!CallC<BYTE*>(Hi_Find, 0, F(const char*, info, 20)))
        {
            F(int, s, 3548) = 1;
            return;
        }
    }
    BYTE* e = g_hiRoads[pos.d[0]];
    CallThis(Hi_Stack, s + 144, (int)F(short, F(BYTE*, e, 112), 16), (int)F(short, F(BYTE*, e, 116), 16));
}

} // extern "C"

// === a module's animated objects ("anmobj" and 7 digits) registered as world resources ($L_56e060 with its
//     tails up to $L_56e54f, a node walk's callback)

extern "C" void Ao_Fatal(); KING_ALIAS("_Ao_Fatal", "_Sys_Fatal")
extern "C" void Ao_Create(); KING_ALIAS("_Ao_Create", "@Resources_Create@16")
extern "C" void Ao_Get(); KING_ALIAS("_Ao_Get", "@Resources_Get@12")
extern "C" void Ao_SetData(); KING_ALIAS("_Ao_SetData", "@Resources_SetData@24")
extern "C" void Ao_Charge(); KING_ALIAS("_Ao_Charge", "@ResTable_Charge@12")
extern "C" void Ao_Root(); KING_ALIAS("_Ao_Root", "@Node_Root@8")
extern "C" void Ao_World(); KING_ALIAS("_Ao_World", "@Node_GetWorld@12")
extern "C" void Ao_CarType(); KING_ALIAS("_Ao_CarType", "_CarType_FromNumber")
extern "C" long __cdecl atol(const char* s);
ASM_VAR(BYTE*, g_ao6cec6c, $L_6cec6c)
ASM_VAR(int, g_ao6968f0, $L_6968f0)
ASM_VAR(int, g_ao696908, $L_696908)
ASM_VAR(int, g_ao696900, $L_696900)
ASM_VAR(BYTE, g_ao6ced98, $L_6ced98)
ASM_VAR(BYTE*, g_ao696ca0, $L_696ca0)
ASM_VAR(AsmLabel, g_ao7025f0, $L_7025f0)
ASM_VAR(Vec3, g_aoZero, $L_702650)
ASM_VAR(char, g_aoBadSub, $L_67bb68)

extern "C" {

// [$L_56e060..$L_56e54f] node (kind 17, named "anmobj" + 7 digits: kind (id / 100000), subtype ((id / 1000) %
// 100), number) of the module being loaded: kind 2 (a parked car, skipped in mode 2 and for other net players'
// shares) gets a 156-byte record (its frame, module and room names), kind 0 one of 76 or 72 bytes by its subtype
// (1 .. 4); the node marked done (+40 bit 0); -1 to go on
int __cdecl AnmObj_OnLoad(BYTE* node, void*)
{
    const char* name = F(const char*, node, 12);
    if (!name || strncmp(name, "anmobj", 6))
        return -1;
    for (int k = 6; k < 13; k++)
        if (name[k] < '0' || name[k] > '9')
            CallC(Ao_Fatal, "incorrect anmobject name", 0);
    int id = atol(name + 6);
    int sub = id / 1000 % 100;
    int kind = id / 100000;
    if (kind != 0 && kind != 2)
        return -1;
    if (F(int, node, 16) != 17)
        CallC(Ao_Fatal, "incorrect anmobject type", 0);
    BYTE* res = g_ml6f3420 + 704;
    if (kind == 2)
    {
        if (F(int, g_ao6cec6c, 1300) == 2 || (g_ao6968f0 && id % 1000 % g_ao696908 != g_ao696900))
        {
            F(BYTE, node, 40) |= 1;
            return -1;
        }
        int r = CallThis<int>(Ao_Create, res, id, g_ml6f3424);
        BYTE* rec = CallThis<BYTE*>(Ao_Get, g_ml6f3420 + 704, id);
        int t = CallC<int>(Ao_CarType, sub);
        if (BYTE* e = ((BYTE**)&g_ao6ced98)[t * 11])
            F(int, rec, 16) = F(int, e, 16);
        if (r)
        {
            if (r == -2)
                F(BYTE, node, 40) |= 1;
            return -1;
        }
        float M[12];
        memcpy(M, g_ao7025f0, 48);
        CallThis(Ao_World, F(BYTE*, node, 68), M);
        const char* room = F(const char*, F(BYTE*, node, 64), 12);
        const char* mod = F(const char*, CallThis<BYTE*>(Ao_Root, node), 12);
        BYTE data[156];
        memset(data, 0, sizeof data);
        F(int, data, 0) = sub;
        F(int, data, 4) = id % 1000;
        F(Vec3, data, 20) = g_aoZero;
        F(Vec3, data, 32) = g_aoZero;
        memcpy(data + 44, M, 48);
        strncpy((char*)data + 92, mod, 31);
        strncpy((char*)data + 124, room, 31);
        F(BYTE, node, 40) |= 1;
        CallThis(Ao_SetData, g_ao696ca0 + 704, id, F(int, F(BYTE*, node, 36), 60), 156, data);
        CallThis(Ao_Charge, rec, g_ao696ca0 + 704);
        return -1;
    }
    int r = CallThis<int>(Ao_Create, res, id, g_ml6f3424);
    if (r)
    {
        if (r == -2)
            F(BYTE, node, 40) |= 1;
        return -1;
    }
    BYTE* rec = CallThis<BYTE*>(Ao_Get, g_ml6f3420 + 704, id);
    if (sub < 1 || sub > 4)
    {
        char buf[100];
        sprintf(buf, &g_aoBadSub, sub, name, 1);
        CallC(Ao_Fatal, buf, 0);
    }
    BYTE data[76];
    memset(data, 0, sizeof data);
    const char* mod = F(const char*, CallThis<BYTE*>(Ao_Root, node), 12);
    int net = F(int, g_ao6cec6c, 1300) <= 1;
    strncpy((char*)data + 12, mod, 31);
    F(int, data, 0) = id;
    int size = 76;
    if (sub == 3)
    {
        strncpy((char*)data + 40, name, 31);
        F(int, data, 4) = net;
        size = 72;
    }
    else
    {
        F(int, data, 4) = 0;
        strncpy((char*)data + 44, name, 31);
        F(int, data, 8) = net;
    }
    CallThis(Ao_SetData, g_ml6f3420 + 704, id, F(int, F(BYTE*, node, 36), 60), size, data);
    F(BYTE, node, 40) |= 1;
    CallThis(Ao_Charge, rec, g_ao696ca0 + 704);
    return -1;
}

} // extern "C"

// === the vehicle made for an effect's description (FUN_4d08d0 with its tail $L_4d0ab3)

extern "C" void Pv_Data(); KING_ALIAS("_Pv_Data", "_Ai_GetPlayerData")
extern "C" void Pv_MsgNew(); KING_ALIAS("_Pv_MsgNew", "@DpMessage_Construct@8")
extern "C" void Pv_MsgWrite(); KING_ALIAS("_Pv_MsgWrite", "@Msg_Write120@20")
extern "C" void Pv_Record(); KING_ALIAS("_Pv_Record", "@Media_RecordBlock@12")
extern "C" void Pv_MsgFree(); KING_ALIAS("_Pv_MsgFree", "@MsgBuf_Destroy@8")
extern "C" void Pv_Effect(); KING_ALIAS("_Pv_Effect", "@Effect4d25f0_Construct@20")
extern "C" void Pv_Cop(); KING_ALIAS("_Pv_Cop", "@CopCar_Construct@20")
extern "C" void Pv_Car(); KING_ALIAS("_Pv_Car", "@Effect4d0e30_Construct@20")
extern "C" void Pv_Fatal(); KING_ALIAS("_Pv_Fatal", "_Sys_Fatal")
extern "C" void Pv_Settings(); KING_ALIAS("_Pv_Settings", "_Car_ApplyPlayerSettings")
ASM_VAR(BYTE*, g_pv68b990, $L_68b990)
ASM_VAR(int, g_pv6f3310, $L_6f3310)
ASM_VAR(int, g_pv6f3314, $L_6f3314)
ASM_VAR(BYTE*, g_pv6cec6c, $L_6cec6c)
ASM_VAR(int, g_pv6ce330, $L_6ce330)
ASM_VAR(int, g_pv6ce334, $L_6ce334)
ASM_VAR(int, g_pv6ce338, $L_6ce338)
ASM_VAR(int, g_pv6ce33c, $L_6ce33c)
ASM_VAR(char, g_pvNoCar, $L_6729c8)

extern "C" {

// [FUN_4d08d0..$L_4d0ab3] the vehicle of description fx (its type +2252) in world w for player entry pl: for the
// local player its gearbox (+10008) and, in a recorded game, a 959 message; an effect vehicle, a cop car or a car
// by the type's range, its settings applied, shown and marked (+312 bit 3)
BYTE* __cdecl Fx_MakePlayerVehicle(BYTE* w, BYTE* fx, BYTE* pl)
{
    int local = 0;
    if (pl != g_pv68b990)
    {
        BYTE pd[192];
        CallC(Pv_Data, pl, pd);
        if (F(int, pd, 8) == 1)
        {
            local = 1;
            F(short, fx, 10008) = g_pv6f3310 ? 2 : 0;
            if (F(int, g_pv6cec6c, 1300) == 1)
            {
                __declspec(align(8)) BYTE msg[32];
                CallThis(Pv_MsgNew, msg);
                int v = F(int, pl, 84);
                CallThis(Pv_MsgWrite, msg, 959, &v, 4);
                CallThis(Pv_Record, w, msg);
                CallThis(Pv_MsgFree, msg);
            }
        }
    }
    int type = F(int, fx, 2252);
    BYTE* v;
    if (type >= g_pv6ce338 && type < g_pv6ce33c)
    {
        BYTE* m = (BYTE*)operator new(21960);
        v = m ? CallThis<BYTE*>(Pv_Effect, m, w, fx, pl) : 0;
    }
    else if (type >= g_pv6ce330 && type < g_pv6ce334)
    {
        BYTE* m = (BYTE*)operator new(21952);
        v = m ? CallThis<BYTE*>(Pv_Cop, m, w, fx, pl) : 0;
    }
    else
    {
        BYTE* m = (BYTE*)operator new(21880);
        v = m ? CallThis<BYTE*>(Pv_Car, m, w, fx, pl) : 0;
    }
    if (!v)
        CallC(Pv_Fatal, &g_pvNoCar, 0);
    if (local)
        F(int, v, 20920) = g_pv6f3314;
    CallC(Pv_Settings, v);
    CallVirt(v + 16, 24, 1);
    F(BYTE, v, 312) |= 8;
    return v;
}

} // extern "C"

// === four byte vectors of a record freed ($L_4d0760, reached by jumps from the unwind code)

static void Vf_Free(BYTE* v)
{
    BYTE* b = F(BYTE*, v, 0);
    if (unsigned n = (unsigned)(F(BYTE*, v, 8) - b))
        Ai_PoolFree(b, n);
}

extern "C" {

// [$L_4d0760] s's vectors at +168, +156, +144 and +132 (begin, end, capacity) given back to the STL pool
void __fastcall Rec4d0760_FreeVectors(BYTE* s, void*)
{
    BYTE* v = s + 80;
    Vf_Free(v + 88);
    Vf_Free(v + 76);
    Vf_Free(v + 64);
    Vf_Free(v + 52);
}

} // extern "C"

// === a vehicle's destructor saving a fresh record of it (FUN_573d10)

extern "C" void Vd_Value(); KING_ALIAS("_Vd_Value", "@Car_555500@8")
extern "C" void Vd_M1(); KING_ALIAS("_Vd_M1", "@SceneObj_Matrix1@12")
extern "C" void Vd_Get800(); KING_ALIAS("_Vd_Get800", "@SceneObj_Get800@8")
extern "C" void Vd_Name(); KING_ALIAS("_Vd_Name", "@Fx_Get12@8")
extern "C" void Vd_9544(); KING_ALIAS("_Vd_9544", "@Fx_Get9544@8")
extern "C" void Vd_20396(); KING_ALIAS("_Vd_20396", "@Fx_Get20396@8")
extern "C" void Vd_Type(); KING_ALIAS("_Vd_Type", "@Fx_Get10420@8")
extern "C" void Vd_Init(); KING_ALIAS("_Vd_Init", "@Fx_ObjectInit@44")
extern "C" void Vd_SetData(); KING_ALIAS("_Vd_SetData", "@Resources_SetData@24")
extern "C" void Vd_Destruct(); KING_ALIAS("_Vd_Destruct", "@Vehicle_Destruct@4")
ASM_VAR(AsmLabel, g_vd650250, $L_650250)
ASM_VAR(AsmLabel, g_vd650118, $L_650118)
ASM_VAR(AsmLabel, g_vd650114, $L_650114)
ASM_VAR(AsmLabel, g_vd650164, $L_650164)

extern "C" {

// [FUN_573d10] vehicle s destroyed: its 300-byte object record (an empty state, its frame in its own object) saved
// under its id (+360), then the vehicle part destructed
void __fastcall Vehicle573d10_Destroy(BYTE* s, void*)
{
    BYTE* it = s + 16;
    F(void*, s, 0) = &g_vd650250;
    F(void*, s, 272) = &g_vd650118;
    F(void*, s, 392) = &g_vd650114;
    F(void*, it, 0) = &g_vd650164;
    __declspec(align(8)) BYTE state[196];
    memset(state, 0, sizeof state);
    float f = (float)CallThis<double>(Vd_Value, s);
    int pl = F(int, s, 20636);
    Matrix W;
    const Matrix* w = CallThis<const Matrix*>(Vd_M1, it, &W);
    const char* name = CallThis<const char*>(Vd_Name, CallThis<BYTE*>(Vd_Get800, it));
    int b = CallThis<int>(Vd_9544, s);
    int r = CallThis<int>(Vd_20396, s);
    int k = CallThis<int>(Vd_Type, s) + r * 1000;
    __declspec(align(8)) BYTE data[300];
    CallThis(Vd_Init, data, k, b, name, w, pl, state, *(int*)&f, 0, 0);
    CallThis(Vd_SetData, F(BYTE*, s, 52) + 704, F(int, s, 360), F(int, F(BYTE*, s, 308), 60), 300, data);
    CallThis(Vd_Destruct, s);
}

} // extern "C"

// === how far the copter drifts sideways before it stops (FUN_562c20 with its tail $L_562e94)

extern "C" void Cd_FrameTime(); KING_ALIAS("_Cd_FrameTime", "_Game_FrameTime")
extern "C" void Cd_Tilt(); KING_ALIAS("_Cd_Tilt", "@Copter_Tilt@24")
ASM_VAR(float, g_cd6f2c3c, $L_6f2c3c)
ASM_VAR(double, g_cd64b670, $L_64b670)
ASM_VAR(double, g_cd64ca48, $L_64ca48)
ASM_VAR(double, g_cd64b5b8, $L_64b5b8)
ASM_VAR(double, g_cd64b6e0, $L_64b6e0)
ASM_VAR(float, g_cd64c264, $L_64c264)
ASM_VAR(float, g_cd64d2f8, $L_64d2f8)
ASM_VAR(char, g_cdFmt1, $L_67b410)
ASM_VAR(char, g_cdFmt2, $L_67b3d0)
ASM_VAR(char, g_cdFmt3, $L_67b3bc)
ASM_VAR(char, g_cdFmt, $L_67a2e8)

extern "C" {

// [FUN_562c20..$L_562e94] copter s banked against its side speed (s +100 in its frame's x, +52 .. +108) by the
// tilt it would take (+128, 6f2c3c a step) stepped in tenths of frames towards p until the speed is spent (at
// most 1000 steps): the distance; fatal with the state when it never stops
float __fastcall Copter_SideDrift(BYTE* s, void*, const float* p)
{
    double d = (double)F(float, s, 76) * F(float, s, 108) + (double)F(float, s, 104) * F(float, s, 64) +
               (double)F(float, s, 52) * F(float, s, 100);
    float sg = (float)(d < g_cd64c264 ? g_cd64ca48 : g_cd64b670);
    double dd = d;
    float v = (float)x87_fabs(d);
    float tilt = F(float, s, 3232);
    float t = 0;
    float dist = 1.0f;
    float acc = 123456.0f;
    int saved = F(int, s, 128);
    float dt = (float)(CallC<double>(Cd_FrameTime) * g_cd64d2f8);
    F(float, s, 128) = (float)-(g_cd6f2c3c * sg);
    for (int i = 0; v > g_cd64b5b8;)
    {
        tilt = (float)CallThis<double>(Cd_Tilt, s, dt, tilt, p, &acc);
        dist = (float)((double)dt * dt * acc * g_cd64b6e0 + ((double)dt * v + dist));
        v = (float)(((double)dt * acc * sg + v) * F(float, s, 3216));
        t = (float)((double)dt + t);
        if (++i >= 1000)
            break;
    }
    if (v > g_cd64b5b8)
    {
        char a[200], b[200], c[200], all[488];
        sprintf(a, &g_cdFmt1, (double)F(float, s, 3232), (double)tilt, (double)F(float, s, 128), (double)sg,
                (double)F(float, s, 3216), (double)F(float, s, 3228));
        sprintf(b, &g_cdFmt2, (double)F(float, s, 100), (double)F(float, s, 104), (double)F(float, s, 108),
                (double)p[0], (double)p[1], (double)p[2], dd, (double)v, (double)acc, (double)dist, (double)dt,
                (double)t);
        sprintf(c, &g_cdFmt3, (double)F(float, s, 36), (double)F(float, s, 40), (double)F(float, s, 44));
        sprintf(all, &g_cdFmt, a, b, c);
        World_Fatal(all);
    }
    F(int, s, 128) = saved;
    return dist;
}

} // extern "C"

// === where a circle about a center touches a line in a plane (FUN_562eb0 with its tails $L_563376, $L_56337b)

ASM_PROC(FUN_4e7889)
extern "C" void Ct_Set(); KING_ALIAS("_Ct_Set", "@Vec3_SetThis@20")
extern "C" void Ct_MulAssign(); KING_ALIAS("_Ct_MulAssign", "@Vec3_MulAssign@16")
ASM_VAR(double, g_ct64b670, $L_64b670)
ASM_VAR(double, g_ct64d960, $L_64d960)
ASM_VAR(double, g_ct64b678, $L_64b678)
ASM_VAR(double, g_ct64b5a0, $L_64b5a0)
ASM_VAR(char, g_ctBad, $L_67b45c)

extern "C" {

// [FUN_562eb0..$L_56337b] circle about c through p (radius r) and the line through a along plane normal n: the
// two crossing points in the plane (by the quadratic 4e7889, the larger of n's x / y as divisor), the one on the
// side of u taken: *at that point, *angle the arc from p (its asin times r), *dir u x the unit radius; the root
// count, 0 when the line misses; fatal when the point is off the circle
int __cdecl Copter_CircleTouch(const float* a, const float* n, const float* p, const float* u, const float* c,
                               float* at, float* angle, float* dir)
{
    float w[3] = {(float)((double)c[0] - p[0]), (float)((double)c[1] - p[1]), (float)((double)c[2] - p[2])};
    double rl = Vec3_Length((const Vec3*)w);
    float r = (float)rl;
    float inv = (float)(g_ct64b670 / rl);
    double e0 = (double)a[0] - c[0], e1 = (double)a[1] - c[1];
    float e2 = (float)((double)a[2] - c[2]);
    float f[3] = {(float)((double)p[0] - c[0]), (float)((double)p[1] - c[1]), (float)((double)p[2] - c[2])};
    double g0 = f[0] - e0, g1 = f[1] - e1, g2 = (double)f[2] - e2;
    if (x87_fabs(n[0] * g0 + (g2 * n[2] + g1 * n[1])) > (double)r)
        return 0;
    double de = n[0] * e0 + (e1 * n[1] + (double)n[2] * e2);
    float x1, y1, x2, y2;
    int cnt;
    if (x87_fabs(n[1]) < x87_fabs(n[0]))
    {
        float h = (float)(de / n[0] - f[0]);
        float qa = (float)((double)n[1] * n[1] / ((double)n[0] * n[0]) + g_ct64b670);
        float qb = (float)(((double)h * n[1] / n[0] + f[1]) * g_ct64d960);
        float qc = (float)(((double)f[1] * f[1] + (double)h * h) - (double)r * r);
        cnt = CallC<int>(FUN_4e7889, qa, qb, qc, &y1, &y2);
        if (!cnt)
            return 0;
        double t = (double)f[0] + h;
        x1 = (float)(t - (double)y1 * n[1] / n[0]);
        x2 = (float)(t - (double)y2 * n[1] / n[0]);
    }
    else
    {
        float h = (float)(de / n[1] - f[1]);
        float qa = (float)((double)n[0] * n[0] / ((double)n[1] * n[1]) + g_ct64b670);
        float qb = (float)(((double)h * n[0] / n[1] + f[0]) * g_ct64d960);
        float qc = (float)(((double)f[0] * f[0] + (double)h * h) - (double)r * r);
        cnt = CallC<int>(FUN_4e7889, qa, qb, qc, &x1, &x2);
        if (!cnt)
            return 0;
        double t = (double)f[1] + h;
        y1 = (float)(t - (double)x1 * n[0] / n[1]);
        y2 = (float)(t - (double)x2 * n[0] / n[1]);
    }
    float kx = (float)((double)x1 - f[0]), ky = (float)((double)y1 - f[1]);
    double z1 = -(double)f[2];
    float cx = (float)(z1 * u[1] - (double)ky * u[2]);
    double cy = (double)kx * u[2] - z1 * u[0];
    double cz = (double)ky * u[0] - (double)kx * u[1];
    if ((double)n[0] * cx + (cz * n[2] + cy * n[1]) < g_ct64b678)
    {
        at[0] = x1;
        at[1] = y1;
    }
    else
    {
        at[0] = x2;
        at[1] = y2;
    }
    at[2] = 0;
    float mf[3] = {-f[0], -f[1], -f[2]};
    float kk[3] = {(float)((double)at[0] - f[0]), (float)((double)at[1] - f[1]), (float)((double)at[2] - f[2])};
    float M[3] = {(float)((double)mf[1] * kk[2] - (double)mf[2] * kk[1]),
                  (float)((double)mf[2] * kk[0] - (double)kk[2] * mf[0]),
                  (float)((double)kk[1] * mf[0] - (double)mf[1] * kk[0])};
    float lk = (float)Vec3_Length((const Vec3*)kk);
    double lm = Vec3_Length((const Vec3*)mf);
    if (x87_fabs((double)lk - r) * inv > g_ct64b5a0 || x87_fabs(lm - r) * inv > g_ct64b5a0)
    {
        char buf[500];
        sprintf(buf, &g_ctBad, lm, (double)lk, (double)r);
        World_Fatal(buf);
    }
    float tmp[3];
    CallThis(Ct_MulAssign, M, tmp, (float)((double)inv * inv));
    double dm = (double)u[0] * M[0] + ((double)M[2] * u[2] + (double)M[1] * u[1]);
    *angle = (float)(x87_fabs(x87_asin(dm)) * r);
    float kn[3];
    CallThis(Ct_Set, kn, (float)((double)kk[0] * inv), (float)((double)kk[1] * inv), (float)((double)kk[2] * inv));
    Vec3 d;
    memcpy(dir, Vec3_Cross(&d, (const Vec3*)u, (const Vec3*)kn), 12);
    CallThis(Ct_Set, tmp, (float)((double)c[0] + at[0]), (float)((double)c[1] + at[1]),
             (float)((double)c[2] + at[2]));
    memcpy(at, tmp, 12);
    return cnt;
}

} // extern "C"
