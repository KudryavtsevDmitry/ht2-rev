// vehicle.cpp - small functions of the unit at 0x52fa20 of king.masm (the
// code before copter.cpp): the control bindings, sounds at the volume of the
// options, the list of vehicles and the classes of vehicle types, getters,
// setters and saved states of the vehicle, a flashing light, and some
// constructors.
//
// Building with  nmake ASM_VEHICLE=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "car.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef int Ints[1];

ASM_PROC(FUN_52fdb0)
extern "C" void __fastcall Engine_SetKeys(BYTE* self, void*, int a, int b);   // carphys.cpp
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void __cdecl Switch_SetChecked(BYTE* obj, int v);     // world.cpp
extern "C" void FUN_545cc0(); KING_ALIAS("_FUN_545cc0", "@Car_SendKind@12")
extern "C" BYTE* __fastcall ListItem592bf0_Construct(BYTE* self, void*, BYTE** list);   // movingb.cpp
extern "C" void __fastcall Mass_Shares(float* self, void*, int wheels);
extern "C" void __fastcall Car_SetView(BYTE* self, void*, int n);
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
extern "C" Vec3* __fastcall Vec3_DivAssign(Vec3* self, void*, Vec3* ret, float s);
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")    // set a switch
extern "C" float __fastcall Ramp_Position(BYTE* self, void*);   // carphys.cpp
extern "C" void FUN_5014b0(); KING_ALIAS("_FUN_5014b0", "@Car_ReadSettings35@12")
extern "C" void FUN_501550(); KING_ALIAS("_FUN_501550", "@Car_MinWearFactor@8")
extern "C" void FUN_556110(); KING_ALIAS("_FUN_556110", "@Car_ApplyTypeVec@8")
extern "C" void FUN_5561b0(); KING_ALIAS("_FUN_5561b0", "@Car_LoadCargo@16")
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")    // named object: constructor
extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")
extern "C" int __cdecl Fx_Skid1(const void* p, const void* v, int net);   // aeffects.cpp
extern "C" int __cdecl Fx_Skid2(const void* p, const void* v, int net);

struct Position { DWORD m[8]; };    // road.h's PositionId, passed by value
struct RoomInfo {
    DWORD       flags;              // 8: the position is a place to stop
    DWORD       m_4, m_8, m_12;
    const char* module;
    const char* room;
    DWORD       m_24, m_28, m_32;
    int         m_36, m_40;
    DWORD       m_44;
};
extern "C" void __cdecl Road_RoomInfo(Position pos, RoomInfo* info);
KING_ALIAS("_Road_RoomInfo", "_Road_GetRoadParameters")

ASM_VAR(BYTE*, g_6cec78, $L_6cec78)             // +386: the sound volume in percent
ASM_VAR(int, g_696ca4, $L_696ca4)
ASM_VAR(int, g_6d205c, $L_6d205c)
ASM_VAR(int, g_6d2060, $L_6d2060)
ASM_VAR(Sound*, g_sound6d1d2c, $L_6d1d2c)
ASM_VAR(BYTE*, g_vehicleList, $L_6ef1f8)        // linked through +4 of the vehicles
ASM_VAR(int, g_6ef210, $L_6ef210)
ASM_VAR(int, g_6ef214, $L_6ef214)
ASM_VAR(int, g_6ef218, $L_6ef218)
ASM_VAR(int, g_6ef220, $L_6ef220)
ASM_VAR(int, g_6ef224, $L_6ef224)
ASM_VAR(int, g_6ef228, $L_6ef228)
ASM_VAR(int, g_6ef230, $L_6ef230)
ASM_VAR(int, g_6ef234, $L_6ef234)
ASM_VAR(int, g_6ef238, $L_6ef238)
ASM_VAR(int, g_6ef240, $L_6ef240)
ASM_VAR(int, g_6ef244, $L_6ef244)
ASM_VAR(int, g_6ef248, $L_6ef248)
ASM_VAR(int, g_typeClass1, $L_6ce334)           // first vehicle type of each class
ASM_VAR(int, g_typeClass2, $L_6ce338)
ASM_VAR(int, g_typeClass3, $L_6ce33c)
ASM_VAR(int, g_typeClass4, $L_6ce340)
ASM_VAR(int, g_typeClass5, $L_6ce344)
ASM_VAR(int, g_typeClassEnd, $L_6ce348)
ASM_VAR(int, g_6ef264, $L_6ef264)
ASM_VAR(int, g_6ef268, $L_6ef268)
ASM_VAR(int, g_6ef270, $L_6ef270)
ASM_VAR(BYTE*, g_6ef1fc, $L_6ef1fc)             // +2968: the time (double)
ASM_VAR(Ints, g_flash695fb0, $L_695fb0)         // 32 steps of a flashing light
ASM_VAR(Ints, g_flash695328, $L_695328)
ASM_VAR(Ints, g_flash695e30, $L_695e30)
ASM_VAR(AsmLabel, g_undefinedModule, $L_66d638) // "UNDEFINED_MODULE"
ASM_VAR(AsmLabel, g_undefinedRoom, $L_66d628)   // "UNDEFINED_ROOM"
ASM_VAR(AsmLabel, g_str68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, g_vtbl64f4bc, $L_64f4bc)
ASM_VAR(AsmLabel, g_vtbl64d7d0, $L_64d7d0)
ASM_VAR(AsmLabel, g_vtbl64c7dc, $L_64c7dc)

static double GameTime() { return Game_Time(); }

// ---------------------------------------------------------------------------
// Sounds, the control bindings

// plays s at v times the sound volume of the options
extern "C" void __cdecl Sound_PlayVolume(Sound* s, float v)
{
    if (!s)
        return;
    double k = g_6cec78 ? F(short, g_6cec78, 386) * 0.01 : 1.0;
    float x = (float)(v * k);
    s->Play(x, x);
}

extern "C" void* __fastcall Obj52fd90_Reset(BYTE* self, void*)
{
    CallThis(FUN_52fdb0, self);
    g_696ca4 = 0;
    return self;
}

// the state byte of action i: its first key's if that is down, else its
// second key's (+264 and +452: pointers into the control states)
extern "C" int __fastcall Controls_ActionState(BYTE* self, void*, int i)
{
    BYTE* k = F(BYTE*, self, 264 + i * 4);
    if (k && *k)
        return *k;
    k = F(BYTE*, self, 452 + i * 4);
    if (k && *k)
        return *k;
    return 0;
}

// axis i: the mean of its two bound inputs, or the one that is bound
extern "C" double __fastcall Controls_Axis(BYTE* self, void*, int i)
{
    float* a = F(float*, self, 640 + i * 4);
    if (a) {
        float* b = F(float*, self, 652 + i * 4);
        if (b)
            return ((double)*b + *a) * 0.5;
        return *a;
    }
    float* b = F(float*, self, 652 + i * 4);
    if (b)
        return *b;
    return 0.0f;
}

extern "C" int __fastcall Controls_530100(BYTE* self, void*, int x)
{
    if (F(int, self, 692) != 1)
        return 0;
    return x == 0;
}

extern "C" float __fastcall Obj_Float200(BYTE* self, void*)
{
    return F(float, self, 200);
}

// whether the room at pos has flag 4 or 8
extern "C" int __cdecl Road_IsPlace(const Position* pos)
{
    RoomInfo info;
    info.m_36 = -1;
    info.m_40 = -1;
    info.flags = 0;
    info.m_8 = 0;
    info.m_12 = 0;
    info.module = g_undefinedModule;
    info.room = g_undefinedRoom;
    info.m_24 = 0;
    info.m_28 = 0;
    Road_RoomInfo(*pos, &info);
    return (info.flags & 12) != 0;
}

// ---------------------------------------------------------------------------
// The game object at $L_6d2078 (+616: the player's vehicle)

extern "C" void __fastcall King_53b130(BYTE* self, void*)
{
    BYTE* car = F(BYTE*, self, 616);
    if (!car)
        return;
    BYTE* e = F(BYTE*, car, 21600);
    if (!e)
        return;
    Engine_SetKeys(e + 9984, 0, (int)(g_6d205c != 0), (int)(g_6d2060 != 0));
}

extern "C" int __fastcall King_53b3c0(BYTE* self, void*, int a)
{
    if (!F(BYTE*, self, 616))
        return 0;
    Sound_PlayVolume(g_sound6d1d2c, 0.4f);
    return CallVirt<int>(F(BYTE*, self, 616), 80, a);
}

extern "C" void __fastcall Obj53e490_Restart(BYTE* self, void*)
{
    F(float, self, 56) = (float)(GameTime() + 2.0f);
    F(float, self, 60) = (float)(GameTime() + 4.0f);
    double t = GameTime();
    F(int, self, 72) = 0;
    F(int, self, 40) = 0;
    F(float, self, 64) = (float)(t + 6.0f);
    CallThis(FUN_5d33d0, self, 1);
}

extern "C" void __fastcall Obj53e4d0_Select(BYTE* self, void*, int sel)
{
    for (int i = 0; i < 3; i++)
        Switch_SetChecked(F(BYTE*, self, 44 + i * 4), (int)(sel == i + 1));
}

// ---------------------------------------------------------------------------
// The list of vehicles, vehicle types

// car if it is in the list of vehicles, else NULL
extern "C" BYTE* __cdecl Car_InList(void* car)
{
    DWORD n = (DWORD)g_vehicleList;
    while (n && n - 4) {
        if (n - 4 == (DWORD)car)
            return (BYTE*)car;
        n = F(DWORD, n - 4, 8);
    }
    return 0;
}

// the same for the vehicle whose part at +16 is p
extern "C" BYTE* __cdecl Car_InListFrom16(BYTE* p)
{
    if (p)
        return Car_InList(p - 16);
    return Car_InList(0);
}

extern "C" BYTE* __cdecl Car_53f7b0(BYTE* p)
{
    if (p && F(int, p, 392) == 2 && F(int, p, 3568) == 2)
        return p - 16;
    return 0;
}

extern "C" int __fastcall Car_53fcd0(BYTE* self, void*, int v)
{
    F(int, self, 9544) = v;
    return CallThis<int>(FUN_545cc0, self, 1);
}

extern "C" void __cdecl Cars_ResetGlobals()
{
    g_6ef210 = 0;
    g_6ef214 = 0;
    g_6ef218 = 0;
    g_6ef220 = 0;
    g_6ef224 = 0;
    g_6ef228 = 0;
    g_6ef230 = 0;
    g_6ef234 = 0;
    g_6ef238 = 0;
    g_6ef240 = 0;
    g_6ef244 = 0;
    g_6ef248 = 0;
}

extern "C" void* __fastcall Obj541940_Construct(BYTE* self, void*, int a)
{
    ListItem592bf0_Construct(self, 0, (BYTE**)a);
    F(void*, self, 0) = g_vtbl64f4bc;
    return self;
}

extern "C" void* __fastcall Obj541960_Self(void* self, void*)
{
    return self;
}

extern "C" BYTE* __fastcall Obj541970_First(BYTE* self, void*)
{
    BYTE* p = *F(BYTE**, self, 12);
    return p ? p - 4 : 0;
}

extern "C" void* __fastcall Obj542440_Construct(BYTE* self, void*)
{
    Mass_Shares((float*)self, 0, 8);
    return self;
}

// sum of |state[i]| * weight[i] over the 9 values at state[0] and the 27 at
// state[14] (the state of Car_GetState, the weights at +10432 of the vehicle)
extern "C" double __fastcall Weights_Sum(const float* self, void*, const float* state)
{
    double s = 0.0f;
    for (int i = 0; i < 9; i++)
        s = s + x87_fabs(state[i]) * self[i];
    for (int i = 0; i < 27; i++)
        s = s + x87_fabs(state[14 + i]) * self[9 + i];
    return s;
}

// the class (0..5) of a vehicle type
extern "C" int __cdecl CarType_Class(int type)
{
    if (type < g_typeClass1)
        return 0;
    if (type < g_typeClass2)
        return 1;
    if (type < g_typeClass3)
        return 2;
    if (type < g_typeClass4)
        return 3;
    if (type < g_typeClass5)
        return 4;
    return type >= g_typeClassEnd ? 0 : 5;
}

// ---------------------------------------------------------------------------
// The vehicle (+21600: its engine and gearbox, +10420: its type)

extern "C" int __fastcall Car_543bb0(BYTE* self, void*)
{
    if (F(int, self, 21348))
        return g_6ef270;
    return g_carTypes[F(int, self, 10420)].m_0 >= 4000.0f ? g_6ef268 : g_6ef264;
}

extern "C" void __fastcall Car_Command(BYTE* self, void*, const int* cmd)
{
    if (cmd[0] == 1)
        F(int, self, 9544) = cmd[1];
    else if (cmd[0] == 2)
        Car_SetView(self, 0, cmd[1]);
}

extern "C" void __fastcall Car_Restore546300(BYTE* self, void*, const BYTE* p)
{
    for (int i = 0; i < 9; i++)
        F(int, F(BYTE*, self, 9536), 4 + i * 20) = p[i];
    for (int i = 0; i < 8; i++)
        F(int, self, 21120 + i * 4) = p[9 + i];
    F(int, self, 10416) = F(int, p, 17);
}

extern "C" void __fastcall Car_Restore546480(BYTE* self, void*, const BYTE* p)
{
    for (int i = 0; i < 11; i++, p += 17) {
        F(int, self, 21608 + i * 4) = p[0];
        F(int, self, 21652 + i * 12) = F(int, p, 1);
        F(int, self, 21656 + i * 12) = F(int, p, 5);
        F(int, self, 21660 + i * 12) = F(int, p, 9);
        F(int, self, 21784 + i * 4) = F(int, p, 13);
    }
}

// sets n switches from records of 20 bytes (switch, value); the second takes
// value 2 when its +8 equals its value
extern "C" void __cdecl Switches_Apply(BYTE* s, int n)
{
    if (!s)
        return;
    for (int i = 0; i < n; i++) {
        if (i == 1 && F(int, s, 24) == F(int, s, 28))
            CallC(FUN_5e1a10, F(int, s, 20), 2);
        else
            CallC(FUN_5e1a10, F(int, s, i * 20), F(int, s, i * 20 + 4));
    }
}

// the 49 floats Weights_Sum weighs; returns how many were written
extern "C" int __fastcall Car_GetState(BYTE* self, void*, float* out)
{
    for (int i = 0; i < 9; i++)
        F(int, out, i * 4) = F(int, F(BYTE*, self, 9536), 12 + i * 20);
    out[9] = Ramp_Position(self + 21152, 0);
    out[10] = Ramp_Position(self + 21204, 0);
    out[11] = Ramp_Position(self + 21256, 0);
    out[12] = Ramp_Position(self + 21308, 0);
    out[13] = (float)CallVirt<double>(self, 184);
    if (!F(BYTE*, self, 21600)) {
        memset(out + 14, 0, 35 * 4);
        return 49;
    }
    return CallThis<int>(FUN_5014b0, F(BYTE*, self, 21600), out + 14) + 14;
}

extern "C" int __fastcall Car_5480e0(BYTE* self, void*)
{
    return F(int, self, 9616);
}

extern "C" int __fastcall Obj5480f0_Get72(BYTE* self, void*)
{
    return F(int, self, 72);
}

extern "C" void* __fastcall Obj548e70_Construct(BYTE* self, void*)
{
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(float, self, 16) = 20.0f;
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    F(int, self, 40) = 0;
    F(float, self, 28) = 1.0f;
    F(int, self, 32) = 0;
    F(int, self, 48) = 0;
    F(int, self, 44) = 0;
    F(int, self, 52) = 0;
    F(int, self, 56) = 0;
    F(int, self, 36) = 0;
    F(int, self, 60) = 0;
    F(int, self, 64) = 1;
    F(int, self, 72) = 0;
    F(int, self, 76) = 0;
    F(int, self, 68) = 0;
    return self;
}

// ---------------------------------------------------------------------------
// A flashing light: +0 the light, +4 its default range, +8 the time of the
// last step, +12 the step (0..31) through the tables of the flash

extern "C" void __fastcall Flasher_SetPos(BYTE* self, void*, const int* pos)
{
    BYTE* o = F(BYTE*, self, 0);
    F(int, o, 60) = pos[0];
    F(int, o, 64) = pos[1];
    F(int, o, 68) = pos[2];
}

// steps the flash every 0.1 s; range < 0 (or NaN) is the default range
extern "C" void __fastcall Flasher_Step(BYTE* self, void*, float range)
{
    double now = F(double, g_6ef1fc, 2968);
    if (!(0.1f >= now - F(float, self, 8))) {
        F(float, self, 8) = (float)now;
        unsigned n = F(unsigned, self, 12) + 1;
        F(unsigned, self, 12) = n;
        if (n >= 32)
            F(unsigned, self, 12) = 0;
    }
    if (!(range >= 0.0f))
        range = F(float, self, 4);
    F(float, F(BYTE*, self, 0), 96) = range;
    F(int, F(BYTE*, self, 0), 108) = g_flash695fb0[F(int, self, 12)];
    F(int, F(BYTE*, self, 0), 112) = g_flash695328[F(int, self, 12)];
    F(int, F(BYTE*, self, 0), 116) = g_flash695e30[F(int, self, 12)];
}

extern "C" void __fastcall Flasher_Update(BYTE* self, void*, const int* pos, float range)
{
    Flasher_SetPos(self, 0, pos);
    Flasher_Step(self, 0, range);
}

extern "C" void __fastcall Flasher_Release(BYTE* self, void*)
{
    CallVirt(F(void*, self, 0), 36);
    void* o = F(void*, self, 0);
    if (o)
        CallVirt(o, 0, 1);              // scalar deleting destructor
}

extern "C" void __fastcall Obj5498b0_Init(BYTE* self, void*, BYTE* car)
{
    F(int, self, 0) = 0;
    F(BYTE*, self, 24) = car;
    F(BYTE*, self, 20) = 0;
    if (car) {
        F(BYTE*, self, 20) = car + 10164;
        F(int, car, 10176) = 0;
    }
    BYTE* flasher = F(BYTE*, self, 16);
    F(float, self, 28) = 0.2f;
    if (flasher) {
        Flasher_Release(flasher, 0);
        operator delete(flasher);
    }
    F(BYTE*, self, 16) = 0;
}

extern "C" void* __fastcall Obj549890_Construct(BYTE* self, void*)
{
    F(int, self, 16) = 0;
    Obj5498b0_Init(self, 0, 0);
    return self;
}

extern "C" int __fastcall Obj549bf0_Get(BYTE* self, void*)
{
    return F(int, F(BYTE*, self, 20), 12);
}

// 0.3 below 5, rising to 1 at 20
extern "C" double __cdecl Ramp_54a330(float x)
{
    if (!(x >= 5.0f))
        return 0.3f;
    if (x > 20.0f)
        return 1.0f;
    return ((double)x - 5.0f) * 0.04666666666666666 + 0.3;
}

extern "C" void __fastcall Car_54ef20(BYTE* self, void*, int a)
{
    for (int i = 0; i < 11; i++) {
        F(int, self, 21608 + i * 4) = 0;
        F(Vec3, self, 21652 + i * 12) = g_axisY;
        F(int, self, 21784 + i * 4) = 0;
    }
    float t = (float)Game_FrameTime();
    CallVirt(self + 16, 80, t, a);
}

extern "C" int __cdecl Veh_54ef90(int unused, int a, int b)
{
    return Fx_Skid1((const void*)a, (const void*)b, 1);
}

extern "C" int __cdecl Veh_54efb0(int a, int b)
{
    return Fx_Skid2((const void*)a, (const void*)b, 1);
}

extern "C" int __fastcall Car_54efd0(BYTE* self, void*, int* a, int* b)
{
    *a = F(int, self, 20596);
    *b = F(int, self, 20628);
    return F(int, self, 20632);
}

extern "C" int __fastcall Car_54f000(BYTE* self, void*, int* a, int* b)
{
    int v = F(int, self, 20600);
    if (!v) {
        *a = 0;
        *b = 0;
        return 0;
    }
    *a = v;
    *b = F(int, self, 20628);
    return F(int, self, 20632);
}

extern "C" void* __fastcall Obj5518d0_Construct(BYTE* self, void*)
{
    CallThis(FUN_5f19b0, self, (const char*)g_str68d1c0, 6, 1);
    for (int i = 0; i < 4; i++) {       // the Vec3 rows of the matrix at +64
        F(int, self, 64 + i * 12) = 0;
        F(int, self, 68 + i * 12) = 0;
        F(int, self, 72 + i * 12) = 0;
    }
    F(Matrix, self, 64) = g_identity;
    F(void*, self, 0) = g_vtbl64d7d0;
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    return self;
}

extern "C" void* __fastcall Obj551920_Construct(BYTE* self, void*, int a, BYTE* parent)
{
    CallThis(FUN_5ef6d0, self, a, 17);
    F(int, self, 48) = 0;
    F(int, self, 52) = 0;
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    F(void*, self, 0) = g_vtbl64c7dc;
    F(BYTE*, self, 64) = parent;
    F(int, self, 68) = 0;
    F(int, self, 72) = 0;
    F(int, self, 76) = 0;
    F(int, self, 80) = 0;
    F(int, self, 84) = -1;
    F(int, self, 36) = F(int, parent, 36);
    return self;
}

extern "C" double __fastcall Car_555500(BYTE* self, void*)
{
    float state[49];
    if (!F(BYTE*, self, 21600))
        return 0.0f;
    Car_GetState(self, 0, state);
    return Weights_Sum((const float*)(self + 10432), 0, state);
}

extern "C" void __fastcall Car_556040(BYTE* self, void*, int mode)
{
    int type = F(int, self, 10420);
    if (type < g_typeClass1 || type >= g_typeClass3)
        return;
    if (mode == 1) {
        if (!F(int, self, 9600))
            CallThis(FUN_556110, self);
        float v = F(float, self, 9572) * 0.14285715f;
        F(float, self, 9584) = v;
        F(float, self, 9588) = v;
        CallThis(FUN_5561b0, self, self + 9592, self + 9580);
    } else if (mode == 0 && F(int, self, 9600)) {
        F(int, self, 9584) = 0;
        F(int, self, 9588) = 0;
        CallThis(FUN_5561b0, self, self + 9592, self + 9580);
    }
}

extern "C" float __fastcall Car_556650(BYTE* self, void*)
{
    if (F(int, self, 20904))
        return F(float, self, 20296);
    return 0.0f;
}

// ---------------------------------------------------------------------------
// The object of FUN_556670: +12, +16 a value of the vehicle type, +24, +36
// amounts that Obj5569c0_Take takes from

extern "C" void* __fastcall Obj556670_Init(BYTE* self, void*, int type)
{
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 40) = 0;
    F(int, self, 44) = 0;
    F(int, self, 48) = 0;
    F(int, self, 52) = 0;
    F(int, self, 56) = 0;
    F(int, self, 24) = 0;
    F(int, self, 32) = 0;
    F(int, self, 28) = 0;
    F(int, self, 36) = 0;
    F(int, self, 20) = 0;
    const CarType* t = &g_carTypes[type];
    float v = F(float, t, 176);
    F(float, self, 12) = v;
    F(float, self, 16) = v;
    F(int, self, 0) = F(int, t, 192);
    F(int, self, 4) = F(int, t, 196);
    F(int, self, 8) = F(int, t, 200);
    return self;
}

extern "C" void __fastcall Obj556a30_Clear(BYTE* self, void*)
{
    F(int, self, 24) = 0;
    F(int, self, 40) = 0;
    F(int, self, 36) = 0;
    F(int, self, 32) = 0;
}

extern "C" void __fastcall Obj556740_Stop(BYTE* self, void*)
{
    if (F(float, self, 24) > 0.0f)
        Obj556a30_Clear(self, 0);
    F(int, self, 44) = 0;
    F(int, self, 48) = 0;
    F(int, self, 52) = 0;
    F(int, self, 56) = 0;
    F(int, self, 20) = 0;
    F(int, self, 16) = 0;
}

extern "C" void __fastcall Obj5566e0_Start(BYTE* self, void*, float a, float b, float c)
{
    if (F(int, self, 44))
        Obj556740_Stop(self, 0);
    F(float, self, 48) = a;
    F(int, self, 44) = 1;
    F(float, self, 16) = F(float, self, 12) - a;
    F(float, self, 52) = b;
    F(float, self, 56) = c;
    F(int, self, 24) = 0;
    F(int, self, 32) = F(int, self, 28);
    F(int, self, 36) = 0;
    F(float, self, 20) = b * c;
}

// takes the part t (at most 1) of the amounts at +36 and +24 into *a and *b
extern "C" void __fastcall Obj5569c0_Take(BYTE* self, void*, float t, float* a, float* b)
{
    if (!(t > 0.0f))
        return;
    if (t > 1.0f)
        t = 1.0f;
    *a = t * F(float, self, 36);
    *b = t * F(float, self, 24);
    F(float, self, 36) = F(float, self, 36) - *a;
    double left = (double)F(float, self, 24) - *b;     // compared unrounded
    F(float, self, 24) = (float)left;
    if (!(left >= F(float, self, 28)))
        F(float, self, 32) = (float)(F(float, self, 28) - left);
}

extern "C" float __fastcall Car_557000(BYTE* self, void*)
{
    float f = F(float, self, 548);
    float lim = 1.0f;
    if (!(f >= 0.5)) {
        if (!(f >= 0.0f))
            lim = 0.0f;
        else
            lim = (float)((double)f + f);
    }
    double v = 1.0f;
    if (F(BYTE*, self, 21600))
        v = CallThis<double>(FUN_501550, F(BYTE*, self, 21600));
    if (!(v >= lim))
        lim = (float)v;
    if (!(lim >= 0.0f))
        return 0.0f;
    return lim;
}

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")    // named object: find (parent, name)
ASM_PROC(FUN_4dde70)
ASM_VAR(BYTE*, g_list6d20dc, $L_6d20dc)
ASM_VAR(AsmLabel, g_6d2410, $L_6d2410)          // 1488-byte records by vehicle type
ASM_VAR(AsmLabel, g_697f9c, $L_697f9c)          // 2416-byte records by vehicle type
ASM_VAR(void*, g_696cb0, $L_696cb0)
ASM_VAR(int, g_6d1db4, $L_6d1db4)
ASM_VAR(Ints, g_696bf8, $L_696bf8)
ASM_VAR(Ints, g_696c0c, $L_696c0c)
ASM_VAR(Ints, g_696c20, $L_696c20)
ASM_VAR(double, g_64f310, $L_64f310)
ASM_VAR(int, g_minTop672d7c, $L_672d7c)         // 50
ASM_VAR(BYTE*, g_696ca0, $L_696ca0)
ASM_VAR(BYTE*, g_6cec6c, $L_6cec6c)
ASM_VAR(BYTE*, g_game, $L_6d2078)
ASM_VAR(void*, g_68b990, $L_68b990)
ASM_VAR(AsmLabel, g_matrix7025f0, $L_7025f0)    // 48 bytes: the identity
ASM_VAR(float, g_702630, $L_702630)
ASM_VAR(float, g_702634, $L_702634)
ASM_VAR(float, g_702638, $L_702638)
ASM_VAR(Ints, g_zero702650, $L_702650)          // three floats
ASM_VAR(AsmLabel, g_6d24bc, $L_6d24bc)
ASM_VAR(Ints, g_6d24e0, $L_6d24e0)

static inline void* Obj_Find(void* parent, const char* name)
{
    return CallC<void*>(FUN_5ef2b0, parent, name);
}

static inline void CopyZero3(void* to)
{
    ((int*)to)[0] = g_zero702650[0];
    ((int*)to)[1] = g_zero702650[1];
    ((int*)to)[2] = g_zero702650[2];
}

static bool NameIs(const BYTE* a, const BYTE* b)
{
    return strcmp((const char*)a, (const char*)b) == 0;
}

// a node of the list at $L_6d20dc by kind, x, y and name; else a free one
extern "C" int __cdecl Node530c40_Find(const BYTE* name, int unused, int kind, int unused2,
                                       int x, int y, BYTE** out)
{
    if (!kind || !g_list6d20dc)
        return 0;
    BYTE* first = g_list6d20dc - 4;
    for (BYTE* n = first; n;) {
        if (F(int, n, 20) == kind && F(int, n, 32) == y && F(int, n, 28) == x && NameIs(n + 36, name)) {
            *out = n;
            return 1;
        }
        BYTE* next = F(BYTE*, n, 8);
        if (!next)
            break;
        n = next - 4;
    }
    for (BYTE* n = first; n;) {
        if (F(int, n, 24) == 0 && F(int, n, 20) == (kind == 1 ? 1 : 2) && F(int, n, 32) == 0
            && F(int, n, 28) == 0) {
            *out = n;
            return 2;
        }
        BYTE* next = F(BYTE*, n, 8);
        if (!next)
            break;
        n = next - 4;
    }
    return 0;
}

// the dashboard's lamps and gauges of the vehicle type
extern "C" void __fastcall Dash_Setup(BYTE* self, void*, BYTE* car)
{
    int type = F(int, car, 10420);
    BYTE* t = (BYTE*)g_6d2410 + type * 1488;
    void* o = F(void*, t, 12);
    if (o) {
        F(void*, self, 1076) = Obj_Find(o, "GearKey");
        F(void*, self, 1080) = Obj_Find(o, "AvtoKey");
        F(void*, self, 1084) = Obj_Find(o, "ParkKey");
        F(void*, self, 1088) = Obj_Find(o, "FuelLampKey");
        F(void*, self, 1092) = Obj_Find(o, "TrailerKey");
    } else {
        F(int, self, 1076) = 0;
        F(int, self, 1080) = 0;
        F(int, self, 1084) = 0;
        F(int, self, 1088) = 0;
        F(int, self, 1092) = 0;
    }
    char buf[100];
    for (int i = 0; i < 5; i++) {
        sprintf(buf, "KMeter%dKey", i);
        F(void*, self, 1096 + i * 4) = Obj_Find(o, buf);
    }
    BYTE* v = (BYTE*)g_697f9c + type * 2416;
    F(int, self, 1064) = F(int, t, 16);
    memcpy(self + 1116, t + 28, 48);
    F(int, self, 1068) = F(int, t, 20);
    memcpy(self + 1164, t + 76, 48);
    F(int, self, 1268) = F(int, v, 212);
    F(int, self, 1260) = F(int, v, 220);
    F(int, self, 1272) = F(int, v, 216);
    F(int, self, 1264) = F(int, v, 224);
    F(int, self, 1072) = F(int, t, 24);
    memcpy(self + 1212, t + 124, 48);
    F(int, self, 1276) = F(int, v, 228);
    CallThis(FUN_4dde70, g_696cb0, car);
    F(int, self, 1280) = F(int, v, 256);
    F(int, self, 1284) = F(int, v, 260);
    F(int, self, 1288) = F(int, v, 264);
    F(int, self, 1292) = F(int, v, 268);
    g_6d1db4 = 0;
}

// the view's rectangle for the screen size
extern "C" void __fastcall View_Place(BYTE* self, void*)
{
    BYTE* scr = F(BYTE*, self, 36);
    int mode = F(int, self, 1396);
    int h = F(int, scr, 260), w = F(int, scr, 256);
    int a = mode ? F(int, self, 1368) : F(int, self, 1376);
    int b = mode ? F(int, self, 1372) : F(int, self, 1380);
    int left = g_696bf8[mode] + a;
    double t = (double)(h - 480) * g_64f310;
    t += g_696c0c[mode];
    t += b;
    int top = (int)t;
    int bottom = g_696c20[mode] + top;
    if (w >= 780 && h >= 580) {
        top += 50;
        bottom += 50;
    }
    if (left < 0)
        left = 0;
    if (top < g_minTop672d7c)
        top = g_minTop672d7c;
    if (bottom <= top)
        bottom = top + 1;
    if (bottom > h)
        bottom = h;
    if (left < 0)
        left = 0;
    int right = w > left ? w : left + 1;
    CallVirt(F(void*, self, 32), 44, left, top, right, bottom);
}

// the nearest of the points of a record of kind 11, and its name
extern "C" void __cdecl Points_Nearest(const int* rec, const float* pos, int* st)
{
    if (!rec || rec[0] != 11)
        return;
    int q = rec[1] / 10;
    if (st[0] == q)
        return;
    st[0] = q;
    int r = rec[1] - q * 10;
    st[1] = r;
    if (!r)
        return;
    float best = 1e7f;
    const float* src = (const float*)rec[3];
    for (int k = 0; k < st[1]; k++, src += 6) {
        float* a = (float*)(st + 2) + k * 3;
        float* b = (float*)(st + 35) + k * 3;
        a[0] = src[0]; a[1] = src[1]; a[2] = src[2];
        b[0] = src[3]; b[1] = src[4]; b[2] = src[5];
        float dx = a[0] - pos[0];
        float dy = a[1] - pos[1];
        double dz = (double)a[2] - pos[2];
        double d = (double)dx * dx + (double)dy * dy + dz * dz;
        if (!(d >= best)) {
            best = (float)d;
            st[32] = ((int*)a)[0]; st[33] = ((int*)a)[1]; st[34] = ((int*)a)[2];
            st[65] = ((int*)b)[0]; st[66] = ((int*)b)[1]; st[67] = ((int*)b)[2];
        }
    }
    if (((unsigned)(rec[2] - st[1] * 6) << 2) == 0)
        return;
    const char* s = (const char*)(rec[3] + st[1] * 24);
    char buf[33];
    int i = 0;
    for (; i < 32 && s[i] != ' ' && s[i]; i++)
        buf[i] = s[i];
    buf[i] = 0;
    BYTE* o = (BYTE*)Obj_Find(0, buf);
    if (!o) {
        BYTE* p = F(BYTE*, g_696ca0, 488);
        o = p ? F(BYTE*, p, 76) : 0;
    }
    strcpy((char*)(st + 68), F(char*, o, 12));
}

// the speedometer, the gear, the fuel and the lamps
extern "C" void __fastcall Dash_Draw(BYTE* self, void*, int x, int y, int* speed, int* km,
                                     float fuel, float tank, int gear, int lamp1, int lamp2)
{
    CSurface* s = F(CSurface*, F(BYTE*, F(BYTE*, self, 36), 252), 12);
    int start = speed[0] ? 0 : (speed[1] ? 1 : 2);
    if (start < 3)
        for (int k = start; k < 3; k++)
            Gfx_DrawImage(s, x + k * 10, y, F(void*, self, 700 + speed[k] * 4), 0);
    for (int i = 0; i < 5; i++) {
        if (i < 3)
            Gfx_DrawImage(s, x + i * 8, y + 15, F(void*, self, 660 + km[i] * 4), 0);
        else
            Gfx_DrawImage(s, x + i * 8 + 2, y + 15, F(void*, self, 856 + km[i] * 4), 0);
    }
    double v = (double)fuel / ((double)tank * 2.5) * 17.0;
    if (!(v <= 16.0))
        v = 16.0;
    Gfx_DrawImage(s, x + 45, y - 6, F(void*, self, 740 + (int)v * 4), 0);
    Gfx_DrawImage(s, x + 65, y + 5, F(void*, self, 812 + gear * 4), 0);
    if (lamp1)
        Gfx_DrawImage(s, x + 96, y - 8, F(void*, self, 896), 0);
    if (lamp2)
        Gfx_DrawImage(s, x + 39, y - 5, F(void*, self, 900), 0);
}

// the traffic light's setup
extern "C" void __fastcall Light_Setup(BYTE* self, void*)
{
    int k = F(int, g_6cec6c, 1316);
    F(int, self, 32) = k == 0 || k == 1;
    F(int, self, 84) = 0;
    F(int, self, 80) = 0;
    void* g = Obj_Find(0, "GreenSvetKey");
    F(void*, self, 52) = g;
    F(void*, self, 48) = g;
    F(void*, self, 44) = Obj_Find(0, "RedSvetKey");
    F(float, self, 56) = 2.0f;
    F(float, self, 60) = 2.0f;
    F(float, self, 64) = 5.0f;
    F(int, self, 72) = 0;
    F(Sound*, self, 76) = Sound_Find("alarmSound");
    BYTE* game = g_game;
    if (game) {
        if (F(BYTE, game, 1404) & 1) {
            Obj53e4d0_Select(self, 0, 0);
            BYTE* p = F(BYTE*, g_game, 616);
            BYTE* q;
            if (p && (q = F(BYTE*, p, 21600)) != 0) {
                F(BYTE, q, 9984) = 0;
                F(int, self, 36) = 1;
                Obj53e4d0_Select(self, 0, 1);
                return;
            }
        } else {
            BYTE* p = F(BYTE*, game, 616);
            BYTE* q;
            if (p && (q = F(BYTE*, p, 21600)) != 0)
                F(BYTE, q, 9984) = 2;
        }
    }
    F(int, self, 36) = 1;
    Obj53e4d0_Select(self, 0, 1);
}

static void Matrix_Init(BYTE* m)
{
    for (int k = 0; k < 4; k++)
        Vec3_ZeroThis((Vec3*)(m + k * 12), 0);
    memcpy(m, g_matrix7025f0, 48);
}

extern "C" BYTE* __fastcall Obj53f7e0_Construct(BYTE* self, void*)
{
    for (int i = 0; i < 8; i++)
        Matrix_Init(self + 272 + i * 48);
    Matrix_Init(self + 656);
    Matrix_Init(self + 704);
    Matrix_Init(self + 752);
    F(int, self, 800) = 0;
    F(int, self, 804) = 0;
    F(int, self, 808) = 0;
    Matrix_Init(self + 828);
    Matrix_Init(self + 876);
    Matrix_Init(self + 924);
    for (int i = 1076; i < 1148; i += 4)
        F(int, self, i) = 0;
    for (int i = 1148; i <= 1168; i += 4)
        F(int, self, i) = 0;
    for (int i = 0; i < 6; i++)
        Matrix_Init(self + 1196 + i * 48);
    F(int, self, 976) = 0;
    F(int, self, 972) = 0;
    for (int i = 0; i < 6; i++) {
        F(int, self, 984 + i * 4) = 0;
        F(int, self, 1008 + i * 4) = 0;
    }
    return self;
}

extern "C" BYTE* __fastcall Body53fa60_Construct(BYTE* self, void*)
{
    F(void*, self, 0) = g_68b990;
    Matrix_Init(self + 4);
    for (int i = 52; i <= 96; i += 4)
        F(int, self, i) = 0;
    for (int i = 180; i <= 212; i += 4)
        F(int, self, i) = 0;
    F(int, self, 276) = 0;
    F(int, self, 168) = 0;
    float gx = -g_702630, gy = -g_702634, gz = -g_702638;
    F(float, self, 88) = (float)(gx * (double)9.8f);
    F(float, self, 92) = (float)(gy * (double)9.8f);
    F(float, self, 96) = (float)(gz * (double)9.8f);
    memcpy(self + 4, g_matrix7025f0, 48);
    CopyZero3(self + 64);
    CopyZero3(self + 76);
    CopyZero3(self + 52);
    F(int, self, 100) = 0;
    for (int i = 0; i < 8; i++) {
        F(int, self, 104 + i * 4) = 0;
        F(int, self, 136 + i * 4) = 0;
        F(int, self, 228 + i * 4) = 0;
    }
    F(int, self, 172) = 0;
    F(int, self, 176) = 0;
    CopyZero3(self + 180);
    CopyZero3(self + 192);
    CopyZero3(self + 204);
    F(int, self, 220) = 0;
    F(int, self, 224) = 0;
    F(int, self, 260) = 0;
    F(int, self, 264) = 328;
    F(int, self, 272) = 0;
    F(int, self, 280) = 0;
    F(int, self, 284) = 0;
    F(int, self, 296) = 0;
    F(int, self, 216) = 0;
    F(int, self, 300) = 1;
    F(int, self, 320) = 0;
    F(int, self, 308) = 0;
    F(int, self, 288) = 0;
    F(int, self, 312) = -1000;
    F(int, self, 316) = 0;
    F(int, self, 292) = 0;
    F(void*, self, 0) = g_68b990;
    return self;
}

extern "C" BYTE* __fastcall Wheel541980_Construct(BYTE* self, void*)
{
    for (int i = 200; i < 296; i += 4)
        F(int, self, i) = 0;
    memset(self + 20, 0, 58);
    F(int, self, 0) = 1;
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 80) = 0;
    F(BYTE, self, 84) = 0;
    F(BYTE, self, 189) = 0;
    F(BYTE, self, 96) = 0;
    for (int i = 0; i < 8; i++) {
        F(BYTE, self, 97 + i) = 0;
        F(int, self, 108 + i * 4) = 0;
        F(int, self, 140 + i * 4) = 0;
    }
    F(DWORD, self, 196) = 0xbf000000;   // -0.5f
    for (int i = 0; i < 8; i++) {
        CopyZero3(self + 200 + i * 12);
        CopyZero3(self + 248 + i * 12);
    }
    F(int, self, 176) = 0;
    F(int, self, 172) = 0;
    F(int, self, 180) = 0;
    F(BYTE, self, 189) = 0;
    F(int, self, 184) = 0;
    F(int, self, 88) = 0;
    F(int, self, 92) = 0;
    F(int, self, 332) = 0;
    F(int, self, 304) = 0;
    F(int, self, 300) = 0;
    F(int, self, 308) = 0;
    F(int, self, 316) = 0;
    F(int, self, 320) = 0;
    F(int, self, 324) = 0;
    F(int, self, 328) = 0;
    F(BYTE, self, 188) = 1;
    F(float, self, 296) = 1.0f;
    F(int, self, 312) = 1;
    return self;
}

// the shares of the vehicle's mass, normalized
extern "C" void __fastcall Mass_Shares(float* self, void*, int wheels)
{
    self[2] = 1.5f;
    self[3] = 1.5f;
    self[0] = 0.0f;
    self[1] = 2.0f;
    for (int i = 4; i <= 8; i++)
        self[i] = 1.0f;
    self[9] = 5.0f;
    self[10] = 4.0f;
    for (int i = 0; i < 8; i++) {
        self[20 + i] = 0.0f;
        if (i < wheels) {
            self[12 + i] = 2.0f;
            self[28 + i] = 1.0f;
        } else {
            self[12 + i] = 0.0f;
            self[28 + i] = 0.0f;
        }
    }
    self[11] = 7.0f;
    double s1 = 0.0, s2 = 0.0;
    for (int i = 0; i < 9; i++)
        s1 += self[i];
    for (int i = 9; i < 36; i++)
        s2 += self[i];
    self[36] = (float)(s2 + s1);
    for (int i = 0; i < 9; i++)
        self[i] = self[i] / self[36];
    for (int i = 9; i < 36; i++)
        self[i] = self[i] / self[36];
}

// switches the vehicle's camera or mode n (0..5), counting the users of each
extern "C" void __fastcall Car_SetView(BYTE* self, void*, int n)
{
    if (F(int, self, 9548) == 5)
        return;
    int cur = F(int, self, 20396);
    if (n == cur || n < 0 || n >= 6)
        return;
    void** objs = (void**)((BYTE*)g_6d24bc + 12);
    int base = F(int, self, 10420) * 372;
    if (!objs[base + n])
        return;
    g_6d24e0[base + cur]--;
    int i = F(int, self, 20396) + F(int, self, 10420) * 372;
    if (!g_6d24e0[i])
        CallVirt(objs[i], 36);
    F(int, self, 20396) = n;
    if (!F(int, self, 388))
        CallThis(FUN_545cc0, self, 2);
    i = F(int, self, 20396) + F(int, self, 10420) * 372;
    if (!g_6d24e0[i])
        CallVirt(objs[i], 40);
    g_6d24e0[F(int, self, 20396) + F(int, self, 10420) * 372]++;
}

// the mean of the positions and of the normals of the wheels on the ground
extern "C" int __cdecl Wheels_Mean(BYTE* wheels, int n, Vec3* pos, Vec3* nrm)
{
    CopyZero3(pos);
    CopyZero3(nrm);
    int cnt = 0;
    if (n <= 0)
        return 0;
    float* p = (float*)(wheels + 360);
    for (; n; n--, p += 62) {
        if (!F(short, p, 16))
            continue;
        cnt++;
        float* a = (float*)pos;
        float x = p[-2] + a[0];
        float y = p[-1] + a[1];
        float z = a[2] + p[0];
        a[0] = x; a[1] = y; a[2] = z;
        float* b = (float*)nrm;
        x = p[1] + b[0];
        y = p[2] + b[1];
        z = p[3] + b[2];
        b[0] = x; b[1] = y; b[2] = z;
    }
    if (!cnt)
        return 0;
    float c = (float)cnt;
    Vec3 tmp;
    Vec3_DivAssign(pos, 0, &tmp, c);
    Vec3_DivAssign(nrm, 0, &tmp, c);
    return 1;
}

ASM_PROC(FUN_4e28b0)
extern "C" void FUN_4e2821(); KING_ALIAS("_FUN_4e2821", "_World_CheckPoint")
extern "C" void FUN_4fc930(); KING_ALIAS("_FUN_4fc930", "@Body4fc930_SetMode@28")
ASM_PROC(FUN_541ad0)
ASM_PROC(FUN_551970)
ASM_PROC(FUN_586df0)
ASM_PROC(FUN_546240)
ASM_PROC(FUN_501b90)
extern "C" void __fastcall Body_ClearMode(BYTE* self, void*);              // carphys.cpp
extern "C" void __fastcall Progress_SetStep(BYTE* self, void*, int step);  // movingb.cpp
ASM_VAR(AsmLabel, g_strSetVehinMode, $L_67a2b0)  // "_Vehicle::set_vehin_mode#2"
ASM_VAR(float, g_quarterPi674408, $L_674408)
ASM_VAR(double, g_64f4c8, $L_64f4c8)            // 1/127

// the mode of the vehicle: 1 a passenger in it, 2 ...
extern "C" void __fastcall Car_SetInMode(BYTE* self, void*, int mode)
{
    if (F(int, self, 10924) == 1 && mode != 1)
        Body_ClearMode(F(BYTE*, self, 21600), 0);
    if (mode == 1) {
        int m = F(int, self, 10924);
        if (m != 1 && (!F(int, self, 20908) || F(int, self, 3992) == 3)) {
            if (m == 2 && CallC<int>(FUN_4e28b0, self + 432, 100.0f))
                CallVirt(self + 16, 144, (void*)g_zero702650, 0);
            CallC(FUN_4e2821, self + 432, (const char*)g_strSetVehinMode, 100.0f);
            CallThis(FUN_4fc930, F(BYTE*, self, 21600), 3, F(int, self, 816), self + 516, self + 432,
                     self + 444);
            F(int, self, 10924) = 1;
            F(int, self, 20908) = 0;
            F(int, self, 10924) = mode;
            return;
        }
    } else if (mode == 2) {
        CopyZero3(self + 20688);
        CopyZero3(self + 20712);
    }
    F(int, self, 10924) = mode;
}

// copies the vehicle's settings into its working values
extern "C" void __fastcall Car_ApplySettings(BYTE* self, void*)
{
    F(int, self, 3572) = 2;
    CallThis(FUN_541ad0, self + 10084);
    CallVirt(self + 16, 156, F(DWORD, self, 10164) & 1);
    double t = (double)F(signed char, self, 10180) / g_quarterPi674408;
    F(int, self, 20984) = F(int, self, 10268);
    F(int, self, 21008) = F(int, self, 10388);
    F(int, self, 21024) = F(int, self, 10396);
    F(int, self, 21020) = F(int, self, 10404);
    F(float, self, 20960) = (float)(t * g_64f4c8);
    F(int, self, 20964) = F(int, self, 10260);
    F(int, self, 20988) = F(int, self, 10380);
    F(float, self, 20968) = F(float, self, 10264);
    F(float, self, 20972) = F(float, self, 10264);
    F(int, self, 21004) = F(int, self, 10384);
    F(int, self, 20980) = F(int, self, 10256);
    F(int, self, 20992) = F(int, self, 10416);
    F(int, self, 21012) = F(int, self, 10392);
    F(int, self, 20996) = F(BYTE, self, 10272);
    F(int, self, 21016) = F(int, self, 10400);
    F(int, self, 21036) = F(int, self, 10412);
    F(int, self, 21032) = F(int, self, 10408);
    DWORD v = F(DWORD, self, 10164);
    F(DWORD, self, 21000) = v & 2;
    F(DWORD, self, 20916) = v >> 3 & 1;
    F(DWORD, self, 20920) = (v >> 2 & 1) + (v >> 3 & 2);
    F(DWORD, self, 20856) = v >> 5 & 1;
    F(DWORD, self, 20860) = v >> 6 & 1;
    F(DWORD, self, 21028) = v >> 17 & 1;
    F(DWORD, self, 20928) = v >> 18 & 1;
    CallThis(FUN_551970, self);
    CallThis(FUN_586df0, self + 16);
    F(int, self, 3572) = 0;
}

static inline void Copy3(void* to, const void* from)
{
    ((int*)to)[0] = ((const int*)from)[0];
    ((int*)to)[1] = ((const int*)from)[1];
    ((int*)to)[2] = ((const int*)from)[2];
}

// the state of the vehicle `back` steps ago (of the last 50)
extern "C" void __fastcall Car_History(BYTE* self, void*, int back, int* a, int* b, void* m1,
                                       void* v1, void* m2, int* c, char* name, void* v2, void* v3,
                                       int* d)
{
    if (back < 0 || back >= 50)
        return;
    int i = F(int, self, 10936) - back;
    if (i < 0)
        i += 50;
    *a = F(int, self, 10940 + i * 4);
    *b = F(int, self, 11140 + i * 4);
    memcpy(m1, self + 12740 + i * 48, 48);
    Copy3(v1, self + 17540 + i * 12);
    memcpy(m2, self + 15140 + i * 48, 48);
    *c = F(int, self, 18140 + i * 4);
    strcpy(name, (const char*)self + 18540 + i * 33);
    Copy3(v2, self + 11540 + i * 12);
    Copy3(v3, self + 12140 + i * 12);
    *d = F(int, self, 11340 + i * 4);
}

// the progress shown for the trailer's load; redoes the cargo on changes
extern "C" void __fastcall Car_UpdateCargo(BYTE* self, void*)
{
    if (!(F(BYTE, self, 20900) & 0x80))
        return;
    BYTE* c = F(BYTE*, self, 9536);
    int a = F(int, c, 44), step;
    if (a >= 2 && F(int, c, 64) >= 2)
        step = 4;
    else if (a >= 1 && F(int, c, 64) >= 1)
        step = 3;
    else if (a >= 1 && F(int, c, 4) < 3)
        step = 2;
    else if (F(int, c, 64) >= 1 && F(int, c, 4) < 3)
        step = 1;
    else
        step = F(int, c, 24) > 0 ? 3 : 0;
    Progress_SetStep(c, 0, step);
    for (int i = 0; i < 9; i++)
        if (F(int, self, 21052 + i * 4) != F(int, F(BYTE*, self, 9536), 4 + i * 20)) {
            F(int, self, 20304) = 1;
            break;
        }
    for (int i = 0; i < 8; i++)
        if (F(int, self, 21088 + i * 4) != F(int, self, 21120 + i * 4)) {
            F(int, self, 20304) = 1;
            break;
        }
    if (F(int, self, 20304))
        CallThis(FUN_546240, self);
    for (int i = 0; i < 9; i++)
        F(int, self, 21052 + i * 4) = F(int, F(BYTE*, self, 9536), 4 + i * 20);
    for (int i = 0; i < 8; i++)
        F(int, self, 21088 + i * 4) = F(int, self, 21120 + i * 4);
    if (F(int, self, 20304)) {
        F(int, self, 20304) = 0;
        CallThis(FUN_546240, self);
    }
}

// the lights at the level *level (at most 1); whether they are on
extern "C" int __fastcall Car_SetLights(BYTE* self, void*, float* level)
{
    if (*level > 1.0f)
        *level = 1.0f;
    if (!F(BYTE*, self, 21600))
        return 0;
    CallThis(FUN_501b90, F(BYTE*, self, 21600), 0, level, F(int, self, 10468));
    CallThis(FUN_501b90, F(BYTE*, self, 21600), 1, level, F(int, self, 10472));
    if (F(int, (BYTE*)g_697f9c + F(int, self, 10420) * 2416, 300) > 0) {
        int k = 0;
        do {
            CallThis(FUN_501b90, F(BYTE*, self, 21600), 3 + k, level, F(int, self, 10480 + k * 4));
            CallThis(FUN_501b90, F(BYTE*, self, 21600), 11 + k, level, F(int, self, 10512 + k * 4));
            CallThis(FUN_501b90, F(BYTE*, self, 21600), 19 + k, level, F(int, self, 10544 + k * 4));
            k++;
        } while (k < F(int, (BYTE*)g_697f9c + F(int, self, 10420) * 2416, 300));
    }
    CallThis(FUN_501b90, F(BYTE*, self, 21600), 2, level, F(int, self, 10476));
    return *level > 0.0f;
}

// ---------------------------------------------------------------------------
// attaches the vehicle to a car: the car's engine faces against the vector at
// 702630

ASM_PROC(FUN_4eec80)    // engine: the angle of a direction (&vec)
extern "C" void __fastcall Engine_4eead0(BYTE* self, void*, int a);   // carphys.cpp

extern "C" void __fastcall Vehicle_AttachCar(BYTE* self, void*, BYTE* car)
{
    F(int, self, 20936) = 0;
    F(BYTE*, self, 21880) = car;
    F(BYTE*, self, 9528) = car ? car + 16 : 0;
    F(int, F(BYTE*, self, 21600), 10096) = 1;
    float d[3] = {-g_702630, -g_702634, -g_702638};
    float a = CallThis<float>(FUN_4eec80, F(BYTE*, car, 21600), d);
    Engine_4eead0(F(BYTE*, self, 21600), 0, F(int, &a, 0));
    F(int, self, 21888) = 0;
    F(int, self, 21892) = -1;
    F(DWORD, F(BYTE*, self, 21600), 10124) = F(DWORD, F(BYTE*, self, 21880), 3284);
    F(int, self, 21884) = 0;
}

// [FUN_557420] calls the object's member at +16: vtbl+148 (a, 0)
extern "C" void __fastcall Car_Member148(BYTE* self, void*, int a)
{
    CallVirt(self + 16, 148, a, 0);
}

// [FUN_557460] calls the object's member at +16: vtbl+156 (a)
extern "C" void __fastcall Car_Member156(BYTE* self, void*, int a)
{
    CallVirt(self + 16, 156, a);
}

ASM_VAR(int, g_6ef2b8, $L_6ef2b8)
ASM_VAR(int, g_6ef2bc, $L_6ef2bc)
ASM_VAR(int, g_6ef2c0, $L_6ef2c0)

// [FUN_557510]
extern "C" void __cdecl Car_ClearGlobals6ef2b8()
{
    g_6ef2b8 = 0;
    g_6ef2bc = 0;
    g_6ef2c0 = 0;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_589410(); KING_ALIAS("_FUN_589410", "@Progress_SetPart@12")
extern "C" void FUN_4eb6a0(); KING_ALIAS("_FUN_4eb6a0", "@Ramp_SetPosition@12")
ASM_PROC(FUN_501630)    // (thiscall on +21600) reads the 35 settings, returns their count
ASM_VAR(AsmLabel, s_67a0b0, $L_67a0b0)
ASM_VAR(AsmLabel, s_679fb8, $L_679fb8)
ASM_VAR(AsmLabel, s_67a04c, $L_67a04c)
extern "C" void __cdecl World_Fatal(const char* msg);
extern "C" int __cdecl Car_ResetSettings35(float* p);

// [FUN_5469e0..$L_546a25] resets a 49-value setup record: 9 zeros, 5 ones, then the 35 settings; returns 49
extern "C" int __cdecl Car_ResetSetup(float* p)
{
    memset(p, 0, 36);
    for (int i = 9; i < 14; i++)
        p[i] = 1.0f;
    int n = Car_ResetSettings35(p + 14) + 14;
    if (n != 49)
        World_Fatal(s_67a0b0);
    return n;
}

// [FUN_545dc0..$L_545df9] copies a {offset, size, bytes} record into the car (fatal for an offset past 21880)
extern "C" void __fastcall Car_SetBytes(BYTE* self, void*, int* rec)
{
    int off = rec[0];
    if ((unsigned)off >= 21880)
    {
        char text[100];
        sprintf(text, s_679fb8, off, 21880);
        World_Fatal(text);
    }
    memcpy(self + off, rec + 2, rec[1]);
}

// [FUN_546900..$L_5469c5] applies a 49-value setup record: 9 progress parts, 4 ramps, vtbl+188, then the settings; returns the count read
extern "C" int __fastcall Car_ApplySetup(BYTE* self, void*, DWORD* p)
{
    for (int k = 0; k < 9; k++)
        CallThis(FUN_589410, F(BYTE*, self, 9536) + k * 20, p[k]);
    CallThis(FUN_4eb6a0, self + 21152, p[9]);
    CallThis(FUN_4eb6a0, self + 21204, p[10]);
    CallThis(FUN_4eb6a0, self + 21256, p[11]);
    CallThis(FUN_4eb6a0, self + 21308, p[12]);
    CallVirt(self, 188, p[13]);
    if (!F(BYTE*, self, 21600))
        return 14;
    int n = CallThis<int>(FUN_501630, F(BYTE*, self, 21600), p + 14) + 14;
    if (n != 49)
        World_Fatal(s_67a04c);
    return n;
}


// ---------------------------------------------------------------------------
#include "x87.h"
extern "C" void FUN_5c32f0(); KING_ALIAS("_FUN_5c32f0", "@List_Add6@20")
extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_530070(); KING_ALIAS("_FUN_530070", "@Controls_ActionState@12")
extern "C" void FUN_5300b0(); KING_ALIAS("_FUN_5300b0", "@Controls_Axis@12")
extern "C" void FUN_589490(); KING_ALIAS("_FUN_589490", "@BodyLevel_Step@16")
ASM_PROC(FUN_5427a0)
ASM_PROC(FUN_501b90)
ASM_PROC(FUN_555550)
ASM_VAR(AsmLabel, g_carTypes697914, $L_697914)
ASM_VAR(AsmLabel, g_controls6d1db8, $L_6d1db8)
ASM_VAR(AsmLabel, s_679ef8, $L_679ef8)
ASM_VAR(AsmLabel, s_67a3dc, $L_67a3dc)

static double Car_Level(BYTE* self, int off, int scale)
{
    return x87_fabs(F(float, F(BYTE*, self, 21600), off)) * F(float, self, scale);
}

// [FUN_555b10] |level 15532| * +10468
extern "C" double __fastcall Car_Level1(BYTE* self, void*) { return Car_Level(self, 15532, 10468); }
// [FUN_555b50] |level 15552| * +10472
extern "C" double __fastcall Car_Level2(BYTE* self, void*) { return Car_Level(self, 15552, 10472); }
// [FUN_555d80] |level 15572| * +10476
extern "C" double __fastcall Car_Level3(BYTE* self, void*) { return Car_Level(self, 15572, 10476); }

// [FUN_556110] vtbl+128 with the car type's three floats at +104
extern "C" void __fastcall Car_ApplyTypeVec(BYTE* self, void*)
{
    BYTE* t = (BYTE*)g_carTypes697914 + F(int, self, 10420) * 2416;
    CallVirt(self, 128, F(float, t, 104), F(float, t, 108), F(float, t, 112));
}

// [FUN_543d10] registers the car with the scene's list x (+8300), notes the time and finds object 679ef8
extern "C" void __fastcall Car_Register(BYTE* self, void*, BYTE* x)
{
    CallThis(FUN_5427a0, self);
    F(int, self, 10096) = F(int, self, 384);
    F(int, self, 384)++;
    BYTE* r = CallThis<BYTE*>(FUN_5c32f0, x + 8300, F(int, self, 10096), self + 10104, F(int, self, 10092));
    F(BYTE*, self, 3484 + F(int, self, 3520) * 4) = r;
    F(int, self, 3520)++;
    F(float, self, 10932) = (float)CallC<double>(FUN_4e27a5);
    F(BYTE*, self, 10920) = CallC<BYTE*>(FUN_5ef2b0, 0, (char*)s_679ef8);
}

// [FUN_555da0] steps the four body levels (+9536 parts 1, 0, 3, 2) by x
extern "C" void __fastcall Car_StepLevels(BYTE* self, void*, DWORD x)
{
    BYTE* b = F(BYTE*, self, 9536);
    CallThis(FUN_589490, b + 20, x, F(DWORD, self, 10436));
    CallThis(FUN_589490, F(BYTE*, self, 9536), x, F(DWORD, self, 10432));
    CallThis(FUN_589490, F(BYTE*, self, 9536) + 60, x, F(DWORD, self, 10444));
    CallThis(FUN_589490, F(BYTE*, self, 9536) + 40, x, F(DWORD, self, 10440));
}

// [FUN_555b30] 501b90(1, x, +10472) on +21600
extern "C" int __fastcall Car_Set501b90a(BYTE* self, void*, DWORD x)
{
    return CallThis<int>(FUN_501b90, F(BYTE*, self, 21600), 1, x, F(DWORD, self, 10472));
}

// [FUN_555d60] 501b90(2, x, +10476) on +21600
extern "C" int __fastcall Car_Set501b90b(BYTE* self, void*, DWORD x)
{
    return CallThis<int>(FUN_501b90, F(BYTE*, self, 21600), 2, x, F(DWORD, self, 10476));
}

// [FUN_53b400] control action 0 is on
extern "C" int __cdecl Action0() { return CallThis<int>(FUN_530070, g_controls6d1db8, 0) != 0; }
// [FUN_53b420] control action 3 is on
extern "C" int __cdecl Action3() { return CallThis<int>(FUN_530070, g_controls6d1db8, 3) != 0; }
// [FUN_53b440] control action 4 is on
extern "C" int __cdecl Action4() { return CallThis<int>(FUN_530070, g_controls6d1db8, 4) != 0; }
// [FUN_53b460] control action 1 is on
extern "C" int __cdecl Action1() { return CallThis<int>(FUN_530070, g_controls6d1db8, 1) != 0; }
// [FUN_53b480] control action 2 is on
extern "C" int __cdecl Action2() { return CallThis<int>(FUN_530070, g_controls6d1db8, 2) != 0; }
// [FUN_53b4b0] control axis 0
extern "C" int __cdecl Axis0() { return CallThis<int>(FUN_5300b0, g_controls6d1db8, 0); }

// [FUN_557400] vtbl+144 of the part at +16 with (x, 67a3dc)
extern "C" int __fastcall Car_Part16Call(BYTE* self, void*, DWORD x)
{
    return CallVirt<int>(self + 16, 144, x, (char*)s_67a3dc);
}

// [FUN_5573e0] 555550 on this
extern "C" int __fastcall Car_Call555550(BYTE* self, void*, DWORD x)
{
    return CallThis<int>(FUN_555550, self, x);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_556740(); KING_ALIAS("_FUN_556740", "@Obj556740_Stop@8")
ASM_PROC(FUN_5455d0)
extern "C" void FUN_4fc920(); KING_ALIAS("_FUN_4fc920", "@Body_SetMode@12")
ASM_PROC(FUN_54e180)
extern "C" void FUN_54ef20(); KING_ALIAS("_FUN_54ef20", "@Car_54ef20@12")
ASM_PROC(FUN_54ff80)
ASM_PROC(FUN_581430)
extern "C" void FUN_5816d0(); KING_ALIAS("_FUN_5816d0", "@MovingItem_SetSpin@16")
extern "C" void FUN_581230(); KING_ALIAS("_FUN_581230", "@SceneObj_SetPosition@16")
extern "C" void FUN_561d50(); KING_ALIAS("_FUN_561d50", "@Copter_SetVec112@12")
extern "C" void FUN_561d10(); KING_ALIAS("_FUN_561d10", "@Copter_SetMatrix@16")
extern "C" void FUN_4f1e20(); KING_ALIAS("_FUN_4f1e20", "@Body_SetVelocity@12")
ASM_PROC(FUN_4f1240)
ASM_VAR(Vec3, g_vhZero702650, $L_702650)

// [FUN_530050] clears the low byte of each of the 295 words whose high byte is 0
extern "C" int __fastcall Words295_ClearEmpty(BYTE* self, void*)
{
    for (int i = 0; i < 295; i++)
        if (!self[i * 2 + 1])
            self[i * 2] = 0;
    return 0;
}

// [FUN_530020] clears the low bytes of the 295 words and the 10 records of 12 bytes at +592
extern "C" int __fastcall Words295_Reset(BYTE* self, void*)
{
    for (int i = 0; i < 295; i++)
        self[i * 2] = 0;
    for (int i = 0; i < 10; i++)
        F(int, self, 592 + i * 12) = 0;
    return Words295_ClearEmpty(self, 0);
}

// [FUN_5560e0] with +9600: updates +9556 and applies a zero vector
extern "C" void __fastcall Car_ApplyZero(BYTE* self, void*)
{
    if (F(int, self, 9600))
    {
        CallThis(FUN_556740, self + 9556);
        CallThis(FUN_5455d0, self, 0, &g_vhZero702650);
    }
}

// [FUN_557480] forwards a value to the object at +21600
extern "C" void __fastcall Car_Forward21600(BYTE* self, void*, int v)
{
    BYTE* o = F(BYTE*, self, 21600);
    if (o)
        CallThis(FUN_4fc920, o, v);
}

// [FUN_555eb0] the sum of the absolute values of 9 ints (stride 20, from +4) of the object at +9536
extern "C" int __fastcall Car_SumAbs9(BYTE* self, void*)
{
    BYTE* p = F(BYTE*, self, 9536) + 4;
    int s = 0;
    for (int i = 0; i < 9; i++, p += 20)
        s += *(int*)p < 0 ? -*(int*)p : *(int*)p;
    return s;
}

// [FUN_556a40] when 54e180 finds a value, applies it and runs 54ff80
extern "C" void __fastcall Car_ApplyFound(BYTE* self, void*)
{
    int v = 0;
    if (CallThis<int>(FUN_54e180, self, &v))
    {
        CallThis(FUN_54ef20, self, v);
        CallThis(FUN_54ff80, self);
    }
}

// [FUN_54c9d0] sets (a, b) through 581430, then updates the parts at +21588 (with +428) and +21584 (with +440)
extern "C" void __fastcall Car_Set54c9d0(BYTE* self, void*, int a, int b)
{
    CallThis(FUN_581430, self, a, b);
    if (F(BYTE*, self, 21588))
        CallThis(FUN_561d50, F(BYTE*, self, 21588), self + 428);
    if (F(BYTE*, self, 21584))
        CallThis(FUN_4f1e20, F(BYTE*, self, 21584), self + 440);
}

// [FUN_54ca20] sets (a, b) through 5816d0, then updates the part at +21588 (with +428) and +21584 (with a)
extern "C" void __fastcall Car_Set54ca20(BYTE* self, void*, int a, int b)
{
    CallThis(FUN_5816d0, self, a, b);
    if (F(BYTE*, self, 21588))
        CallThis(FUN_561d50, F(BYTE*, self, 21588), self + 428);
    if (F(BYTE*, self, 21584))
        CallThis(FUN_4f1e20, F(BYTE*, self, 21584), a);
}

// [FUN_54c880] passes (a, b) to +16 and the parts at +21604 and +21600
extern "C" void __fastcall Car_Set54c880(BYTE* self, void*, int a, int b)
{
    CallThis(FUN_581230, self + 16, a, b);
    if (F(BYTE*, self, 21604))
        CallThis(FUN_561d10, F(BYTE*, self, 21604), a, b);
    if (F(BYTE*, self, 21600))
        CallThis(FUN_4f1240, F(BYTE*, self, 21600), 1, a, b);
}


// ---------------------------------------------------------------------------
ASM_PROC(FUN_501b90)
extern "C" void FUN_4eb6a0(); KING_ALIAS("_FUN_4eb6a0", "@Ramp_SetPosition@12")
ASM_PROC(FUN_5812a0)
extern "C" void FUN_561d70(); KING_ALIAS("_FUN_561d70", "@Copter_SetVec100@12")
extern "C" void FUN_4f1dd0(); KING_ALIAS("_FUN_4f1dd0", "@Body_SetPosition@12")
extern "C" void FUN_4dde10(); KING_ALIAS("_FUN_4dde10", "@Cab_Release@8")
extern "C" void FUN_53f750(); KING_ALIAS("_FUN_53f750", "_Car_InList")
extern "C" void FUN_4e9d90(); KING_ALIAS("_FUN_4e9d90", "_WarnLog_Close")
extern "C" void FUN_4e9d36(); KING_ALIAS("_FUN_4e9d36", "_VehLogs_Close")
ASM_PROC(FUN_4f0fc0)
ASM_VAR(AsmLabel, g_vh69809c, $L_69809c)          // car types, 2416 bytes each
ASM_VAR(BYTE*, g_vh696cb0, $L_696cb0)
ASM_VAR(int, g_vh696cb8, $L_696cb8)
ASM_VAR(AsmLabel, g_vh696cbc, $L_696cbc)
ASM_VAR(AsmLabel, s_vh673340, $L_673340)
ASM_VAR(int, g_vh696c70, $L_696c70)
ASM_VAR(int, g_vh672d80, $L_672d80)
ASM_VAR(int, g_vh696c74, $L_696c74)
ASM_VAR(AsmLabel, s_vh68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, s_vh66c4e4, $L_66c4e4)
ASM_VAR(AsmLabel, s_vh67a298, $L_67a298)
ASM_VAR(AsmLabel, s_vh67a3c4, $L_67a3c4)

// the count at +44 of the car's type record
static int Vh_TypeCount(BYTE* self)
{
    return F(int, (BYTE*)g_vh69809c, F(int, self, 10420) * 2416 + 44);
}

// [FUN_555c80] passes the values at +10480 to the body (+21600) as items 3.. with a, then sets its ramp at +16164 to 1
extern "C" void __fastcall Car_PassItems3(BYTE* self, void*, int a)
{
    for (int i = 0; i < Vh_TypeCount(self); i++)
        CallThis(FUN_501b90, F(BYTE*, self, 21600), i + 3, a, F(int, self, 10480 + i * 4));
    CallThis(FUN_4eb6a0, F(BYTE*, self, 21600) + 16164, 1.0f);
}

// [FUN_555b70] passes the values at +10512 and +10544 as items 11.. and 19.., then sets the ramps at +16216 and +16268 to 1
extern "C" void __fastcall Car_PassItems11(BYTE* self, void*, int a)
{
    for (int i = 0; i < Vh_TypeCount(self); i++)
    {
        CallThis(FUN_501b90, F(BYTE*, self, 21600), i + 11, a, F(int, self, 10512 + i * 4));
        CallThis(FUN_501b90, F(BYTE*, self, 21600), i + 19, a, F(int, self, 10544 + i * 4));
    }
    CallThis(FUN_4eb6a0, F(BYTE*, self, 21600) + 16216, 1.0f);
    CallThis(FUN_4eb6a0, F(BYTE*, self, 21600) + 16268, 1.0f);
}

// [FUN_54c950] names the part (b or a default) at a position, then updates the parts at +21588 (with +416) and +21584
extern "C" void __fastcall Car_NamePart(BYTE* self, void*, Vec3* pos, const char* b)
{
    char buf[400];
    if (b)
        sprintf(buf, s_vh67a298, b, (char*)s_vh68d1c0);
    else
        sprintf(buf, s_vh67a298, (char*)s_vh68d1c0, (char*)s_vh66c4e4);
    CallThis(FUN_5812a0, self, pos, buf);
    if (F(BYTE*, self, 21588))
        CallThis(FUN_561d70, F(BYTE*, self, 21588), self + 416);
    if (F(BYTE*, self, 21584))
        CallThis(FUN_4f1dd0, F(BYTE*, self, 21584), pos);
}

// [FUN_5319f0] detaches the player's car (+616): resets its controls and flags, and closes the vehicle log when logging is on
extern "C" void __fastcall Player_DetachCar(BYTE* self, void*)
{
    CallThis(FUN_4dde10, g_vh696cb0);
    g_vh696cb8 = 0;
    sprintf(g_vh696cbc, s_vh673340);
    if (!CallC<int>(FUN_53f750, F(BYTE*, self, 616)))
        return;
    CallVirt(F(BYTE*, self, 616), 100);
    CallVirt(F(BYTE*, self, 616), 108, 0);
    F(int, F(BYTE*, self, 616), 20956) = 0;
    F(DWORD, F(BYTE*, self, 616), 20900) &= 0xffffffcbu;
    F(BYTE*, self, 616) = 0;
    F(int, self, 104) = 0;
    F(int, self, 624) = 0;
    if (g_vh696c70 || g_vh672d80 || g_vh696c74)
    {
        CallC(FUN_4e9d90);
        CallC(FUN_4e9d36);
    }
}

// [FUN_554670] applies the car's position, name and orientation to its model (+16) and parts
extern "C" void __fastcall Car_ApplyPlacement(BYTE* self, void*)
{
    BYTE* m = self + 16;
    CallThis(FUN_581230, m, F(int, self, 820), self + 564);
    CallThis(FUN_5812a0, m, self + 468, (char*)s_vh67a3c4);
    CallThis(FUN_5816d0, m, self + 480, 0);
    if (F(BYTE*, self, 21604))
        CallThis(FUN_561d10, F(BYTE*, self, 21604), F(int, self, 820), self + 564);
    if (F(BYTE*, self, 21600) && F(int, self, 10924) == 1)
    {
        CallThis(FUN_4f0fc0, F(BYTE*, self, 21600), F(int, self, 820), self + 564);
        CallThis(FUN_4f1dd0, F(BYTE*, self, 21600), self + 468);
        CallThis(FUN_4f1e20, F(BYTE*, self, 21600), self + 480);
    }
}


// ---------------------------------------------------------------------------
// Small virtual methods of the car: the trailer (+21880) adds its share to
// the car's sums, part forwarders, destructors.

extern "C" void FUN_5300b0(); KING_ALIAS("_FUN_5300b0", "@Controls_Axis@12")
ASM_PROC(FUN_556a70)
extern "C" void FUN_5561b0(); KING_ALIAS("_FUN_5561b0", "@Car_LoadCargo@16")
extern "C" void FUN_5562e0(); KING_ALIAS("_FUN_5562e0", "@Car_UnloadCargo@16")
extern "C" void FUN_54c950(); KING_ALIAS("_FUN_54c950", "@Car_NamePart@16")
extern "C" void FUN_54c9d0(); KING_ALIAS("_FUN_54c9d0", "@Car_Set54c9d0@16")
extern "C" void FUN_54ca20(); KING_ALIAS("_FUN_54ca20", "@Car_Set54ca20@16")
ASM_PROC(FUN_547030)
extern "C" void FUN_546890(); KING_ALIAS("_FUN_546890", "_Switches_Apply")
extern "C" void FUN_556410(); KING_ALIAS("_FUN_556410", "@Car_TakeCargo@20")
extern "C" void FUN_4eb6a0(); KING_ALIAS("_FUN_4eb6a0", "@Ramp_SetPosition@12")
extern "C" void FUN_557440(); KING_ALIAS("_FUN_557440", "@Thunk557440@8")
extern "C" void FUN_557450(); KING_ALIAS("_FUN_557450", "@Thunk557450@4")
extern "C" void FUN_531950(); KING_ALIAS("_FUN_531950", "@Car531950_Destroy@4")
extern "C" void FUN_5560e0(); KING_ALIAS("_FUN_5560e0", "@Car_ApplyZero@8")
extern "C" void FUN_555eb0(); KING_ALIAS("_FUN_555eb0", "@Car_SumAbs9@8")
extern "C" void FUN_555500(); KING_ALIAS("_FUN_555500", "@Car_555500@8")
extern "C" void FUN_555e10(); KING_ALIAS("_FUN_555e10", "@Car_ControlSum4@4")
extern "C" void FUN_555d80(); KING_ALIAS("_FUN_555d80", "@Car_Level3@8")
extern "C" void FUN_555d10(); KING_ALIAS("_FUN_555d10", "@Car_WheelSum@4")
extern "C" void FUN_555c20(); KING_ALIAS("_FUN_555c20", "@Car_WheelLoads@4")
extern "C" void FUN_555b50(); KING_ALIAS("_FUN_555b50", "@Car_Level2@8")
extern "C" void FUN_555b10(); KING_ALIAS("_FUN_555b10", "@Car_Level1@8")
extern "C" void FUN_555e80(); KING_ALIAS("_FUN_555e80", "@Car_ControlSum9@4")
ASM_VAR(float, g_vh64c264, $L_64c264)    // 0
ASM_VAR(float, g_vh64c258, $L_64c258)

// [$L_5565b0]
extern "C" double __fastcall Car_Get9596(BYTE* s) { return F(float, s, 9596); }
// [$L_5565c0]
extern "C" void __fastcall Car_Set9596(BYTE* s, void*, DWORD v) { F(DWORD, s, 9596) = v; }

// [$L_53b4a0]
extern "C" int __cdecl Car_ControlsAxis2() { return CallThis<int>(FUN_5300b0, g_controls6d1db8, 2); }

// [$L_55edf0] full (1.0) setting of the record at +21584, then FUN_556a70
extern "C" void __fastcall Car_55edf0(BYTE* s)
{
    F(WORD, F(BYTE*, s, 21584), 10006) = 1;
    F(DWORD, F(BYTE*, s, 21584), 10000) = 0x3f800000;
    CallThis(FUN_556a70, s);
}

// [$L_556550] forwarders of two arguments
extern "C" int __fastcall Car_556550(BYTE* s, void*, DWORD a, DWORD b) { return CallThis<int>(FUN_5561b0, s, a, b); }
// [$L_556570]
extern "C" int __fastcall Car_556570(BYTE* s, void*, DWORD a, DWORD b) { return CallThis<int>(FUN_5562e0, s, a, b); }
// [$L_55d120]
extern "C" int __fastcall Car_55d120(BYTE* s, void*, DWORD a, DWORD b) { return CallThis<int>(FUN_54c950, s, a, b); }
// [$L_55ee20]
extern "C" int __fastcall Car_55ee20(BYTE* s, void*, DWORD a, DWORD b) { return CallThis<int>(FUN_54c9d0, s, a, b); }
// [$L_55ee40]
extern "C" int __fastcall Car_55ee40(BYTE* s, void*, DWORD a, DWORD b) { return CallThis<int>(FUN_54ca20, s, a, b); }
// [$L_55ee60]
extern "C" int __fastcall Car_55ee60(BYTE* s, void*, DWORD a, DWORD b) { return CallThis<int>(FUN_547030, s, a, b); }
// [$L_556590]
extern "C" int __fastcall Car_556590(BYTE* s, void*, DWORD a, DWORD b, DWORD c) { return CallThis<int>(FUN_556410, s, a, b, c); }

// [$L_5468e0] the switches (+9520, +9516)
extern "C" int __fastcall Car_ApplySwitches(BYTE* s) { return CallC<int>(FUN_546890, F(int, s, 9520), F(int, s, 9516)); }

// [$L_5574a0] the ramps of the cab (+21600) to 1.0
extern "C" void __fastcall Car_RampA(BYTE* s)
{
    BYTE* c = F(BYTE*, s, 21600);
    if (c)
        CallThis(FUN_4eb6a0, c + 16164, 0x3f800000);
}
// [$L_5574c0]
extern "C" void __fastcall Car_RampB(BYTE* s)
{
    BYTE* c = F(BYTE*, s, 21600);
    if (c)
        CallThis(FUN_4eb6a0, c + 16216, 0x3f800000);
}

// [$L_54d640] sets +1204 and both flags of the record at +21584
extern "C" void __fastcall Car_SetFlag1204(BYTE* s, void*, int v)
{
    F(int, s, 1204) = v;
    BYTE* r = F(BYTE*, s, 21584);
    if (!r)
        return;
    WORD w = v ? 1 : 0;
    F(WORD, r, 10048) = w;
    F(WORD, F(BYTE*, s, 21584), 10050) = w;
}

// [$L_55e740] values of the trailer, 0 without one
extern "C" double __fastcall Car_Trailer136(BYTE* s) { BYTE* t = F(BYTE*, s, 21880); return t ? CallVirt<double>(t, 136) : g_vh64c264; }
// [$L_55e760]
extern "C" double __fastcall Car_Trailer140(BYTE* s) { BYTE* t = F(BYTE*, s, 21880); return t ? CallVirt<double>(t, 140) : g_vh64c264; }
// [$L_55e780]
extern "C" double __fastcall Car_Trailer144(BYTE* s) { BYTE* t = F(BYTE*, s, 21880); return t ? CallVirt<double>(t, 144) : g_vh64c264; }
// [$L_55e7a0]
extern "C" double __fastcall Car_Trailer148(BYTE* s) { BYTE* t = F(BYTE*, s, 21880); return t ? CallVirt<double>(t, 148) : g_vh64c264; }
// [$L_55ea80]
extern "C" double __fastcall Car_Trailer184(BYTE* s) { BYTE* t = F(BYTE*, s, 21880); return t ? CallVirt<double>(t, 184) : g_vh64c264; }

// [$L_55e9c0]
extern "C" double __fastcall Car_Trailer144x2(BYTE* s)
{
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return g_vh64c264;
    double v = CallVirt<double>(t, 144);
    return v + v;
}

// [$L_55e540]
extern "C" void __fastcall Car_55e540(BYTE* s)
{
    CallThis(FUN_557440, s);
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        CallVirt(t, 100);
}
// [$L_55e560]
extern "C" void __fastcall Car_55e560(BYTE* s)
{
    CallThis(FUN_557450, s);
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        CallVirt(t, 104);
}

// [$L_55e720]
extern "C" void __fastcall Car_55e720(BYTE* s)
{
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        CallVirt(t, 132);
    CallThis(FUN_5560e0, s);
}

// [$L_55e470] the car's FUN_555eb0 plus the trailer's vtbl+64
extern "C" int __fastcall Car_55e470(BYTE* s)
{
    int n = CallThis<int>(FUN_555eb0, s);
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        n += CallVirt<int>(t, 64);
    return n;
}

// the car's value (rounded to float) plus the trailer's
static double Car_PlusTrailer(BYTE* s, void (*own)(), unsigned vtbl)
{
    float v = (float)CallThis<double>(own, s);
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        return CallVirt<double>(t, vtbl) + v;
    return v;
}
// [$L_55e320]
extern "C" double __fastcall Car_Sum36(BYTE* s) { return Car_PlusTrailer(s, FUN_555e10, 36); }
// [$L_55e350]
extern "C" double __fastcall Car_Sum40(BYTE* s) { return Car_PlusTrailer(s, FUN_555d80, 40); }
// [$L_55e380]
extern "C" double __fastcall Car_Sum44(BYTE* s) { return Car_PlusTrailer(s, FUN_555d10, 44); }
// [$L_55e3b0]
extern "C" double __fastcall Car_Sum48(BYTE* s) { return Car_PlusTrailer(s, FUN_555c20, 48); }
// [$L_55e3e0]
extern "C" double __fastcall Car_Sum52(BYTE* s) { return Car_PlusTrailer(s, FUN_555b50, 52); }
// [$L_55e410]
extern "C" double __fastcall Car_Sum56(BYTE* s) { return Car_PlusTrailer(s, FUN_555b10, 56); }
// [$L_55e440]
extern "C" double __fastcall Car_Sum60(BYTE* s) { return Car_PlusTrailer(s, FUN_555e80, 60); }

// [$L_55e4b0] FUN_555500 of the car plus that of the trailer
extern "C" double __fastcall Car_Sum555500(BYTE* s)
{
    float v = (float)CallThis<double>(FUN_555500, s);
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        return CallThis<double>(FUN_555500, t) + v;
    return v;
}

// [$L_55e9e0] *p scaled by 64c258 through vtbl+152 (a, &v), then doubled back into *p
extern "C" void __fastcall Car_Scaled152(BYTE* s, void*, DWORD a, float* p)
{
    float v = (float)((double)*p * g_vh64c258);
    CallVirt(s, 152, a, &v);
    *p = (float)((double)v + v);
}
// [$L_55ea10]
extern "C" void __fastcall Car_Scaled156(BYTE* s, void*, DWORD a, float* p)
{
    float v = (float)((double)*p * g_vh64c258);
    CallVirt(s, 156, a, &v);
    *p = (float)((double)v + v);
}
// [$L_55ea40]
extern "C" void __fastcall Car_Scaled160(BYTE* s, void*, DWORD a, DWORD b, float* p)
{
    float v = (float)((double)*p * g_vh64c258);
    CallVirt(s, 160, a, b, &v);
    *p = (float)((double)v + v);
}

// [$L_531930]
extern "C" BYTE* __fastcall Car531930_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_531950, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_53f7e0(); KING_ALIAS("_FUN_53f7e0", "@Obj53f7e0_Construct@8")
extern "C" void FUN_54c820(); KING_ALIAS("_FUN_54c820", "@Car_Place@16")
extern "C" void FUN_4ff790(); KING_ALIAS("_FUN_4ff790", "@Car_StartEngine@8")
extern "C" void FUN_5573e0(); KING_ALIAS("_FUN_5573e0", "@Car_Call555550@12")
extern "C" void FUN_557400(); KING_ALIAS("_FUN_557400", "@Car_Part16Call@12")
extern "C" void FUN_557420(); KING_ALIAS("_FUN_557420", "@Car_Member148@12")
extern "C" void FUN_557460(); KING_ALIAS("_FUN_557460", "@Car_Member156@12")
extern "C" void FUN_555da0(); KING_ALIAS("_FUN_555da0", "@Car_StepLevels@12")
extern "C" void FUN_555d60(); KING_ALIAS("_FUN_555d60", "@Car_Set501b90b@12")
extern "C" void FUN_555c80(); KING_ALIAS("_FUN_555c80", "@Car_PassItems3@12")
extern "C" void FUN_555b70(); KING_ALIAS("_FUN_555b70", "@Car_PassItems11@12")
extern "C" void FUN_555b30(); KING_ALIAS("_FUN_555b30", "@Car_Set501b90a@12")
extern "C" void FUN_555a70(); KING_ALIAS("_FUN_555a70", "@Car_ResetControls@12")
extern "C" void FUN_555ee0(); KING_ALIAS("_FUN_555ee0", "@Car_StepAllLevels@12")
extern "C" void FUN_557480(); KING_ALIAS("_FUN_557480", "@Car_Forward21600@12")
extern "C" void FUN_5566e0(); KING_ALIAS("_FUN_5566e0", "@Obj5566e0_Start@20")
ASM_PROC(FUN_5455d0)
ASM_VAR(AsmLabel, g_veh6d20f0, $L_6d20f0)

// [$L_53f720] static initializer: constructs the 80 records of 1488 bytes at 6d20f0
extern "C" void __cdecl VehInit_53f720()
{
    BYTE* p = (BYTE*)g_veh6d20f0;
    for (int i = 0; i < 80; i++, p += 1488)
        CallThis(FUN_53f7e0, p);
}

// [$L_557390] with a cab (+21600) FUN_54c820 (a, b) and FUN_4ff790 of the cab, else vtbl+140 of the body at +16
extern "C" int __fastcall Car_557390(BYTE* s, void*, int a, int b, int)
{
    if (F(BYTE*, s, 21600)) {
        CallThis(FUN_54c820, s, a, b);
        return CallThis<int>(FUN_4ff790, F(BYTE*, s, 21600));
    }
    return CallVirt<int>(s + 16, 140, a, b);
}

// the trailer (+21880) first, then the car itself
static void Car_TrailerThen(BYTE* s, unsigned vtbl, void (*own)(), int a)
{
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        CallVirt(t, vtbl, a);
    CallThis(own, s, a);
}
// [$L_55e1a0]
extern "C" void __fastcall Car_55e1a0(BYTE* s, void*, int a) { Car_TrailerThen(s, 80, FUN_5573e0, a); }
// [$L_55e4e0]
extern "C" void __fastcall Car_55e4e0(BYTE* s, void*, int a) { Car_TrailerThen(s, 92, FUN_557400, a); }
// [$L_55e510]
extern "C" void __fastcall Car_55e510(BYTE* s, void*, int a) { Car_TrailerThen(s, 96, FUN_557420, a); }

// the car itself first, then the trailer (+21880)
static void Car_ThenTrailer(BYTE* s, void (*own)(), unsigned vtbl, int a)
{
    CallThis(own, s, a);
    BYTE* t = F(BYTE*, s, 21880);
    if (t)
        CallVirt(t, vtbl, a);
}
// [$L_55e1d0]
extern "C" void __fastcall Car_55e1d0(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555da0, 8, a); }
// [$L_55e200]
extern "C" void __fastcall Car_55e200(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555d60, 12, a); }
// [$L_55e230]
extern "C" void __fastcall Car_55e230(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555c80, 16, a); }
// [$L_55e260]
extern "C" void __fastcall Car_55e260(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555b70, 20, a); }
// [$L_55e290]
extern "C" void __fastcall Car_55e290(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555b30, 24, a); }
// [$L_55e2c0]
extern "C" void __fastcall Car_55e2c0(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555a70, 28, a); }
// [$L_55e2f0]
extern "C" void __fastcall Car_55e2f0(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_555ee0, 32, a); }
// [$L_55e5c0]
extern "C" void __fastcall Car_55e5c0(BYTE* s, void*, int a) { Car_ThenTrailer(s, FUN_557480, 112, a); }

// [$L_55e580] as above with vtbl+108; the trailer's cab flag 10050 is cleared
extern "C" void __fastcall Car_55e580(BYTE* s, void*, int a)
{
    CallThis(FUN_557460, s, a);
    BYTE* t = F(BYTE*, s, 21880);
    if (t) {
        CallVirt(t, 108, a);
        F(WORD, F(BYTE*, F(BYTE*, s, 21880), 21600), 10050) = 0;
    }
}

// [$L_556150] vtbl+132, FUN_5566e0 (a, b, c) of +9556, then FUN_5455d0 (+9604, copy of +9556)
extern "C" int __fastcall Car_556150(BYTE* s, void*, int a, int b, int c)
{
    CallVirt(s, 132);
    BYTE* p = s + 9556;
    CallThis(FUN_5566e0, p, a, b, c);
    DWORD v[3] = {F(DWORD, p, 0), F(DWORD, p, 4), F(DWORD, p, 8)};
    return CallThis<int>(FUN_5455d0, s, F(int, s, 9604), v);
}

// ---------------------------------------------------------------------------
// The trailer's hitch: after a trailer method the cab is pulled back along
// the hitch direction.

ASM_PROC(FUN_4eec80)
extern "C" void FUN_4eeaf0(); KING_ALIAS("_FUN_4eeaf0", "@Body_SetLoad@12")
extern "C" void FUN_581230(); KING_ALIAS("_FUN_581230", "@SceneObj_SetPosition@16")
extern "C" void FUN_561d10(); KING_ALIAS("_FUN_561d10", "@Copter_SetMatrix@16")
ASM_PROC(FUN_4f1240)
ASM_PROC(FUN_4f0fc0)
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5f9b00(); KING_ALIAS("_FUN_5f9b00", "@SceneNode5f9b00_Hit@16")
ASM_VAR(AsmLabel, g_veh702630, $L_702630)
ASM_VAR(AsmLabel, g_veh6d2114, $L_6d2114)

// the cab of the car gets FUN_4eec80 of the trailer's cab for the reversed hitch direction 702630
static void Car_PullCab(BYTE* s)
{
    const float* h = (const float*)g_veh702630;
    float v[3] = {-h[0], -h[1], -h[2]};
    float f = (float)CallThis<double>(FUN_4eec80, F(BYTE*, F(BYTE*, s, 21880), 21600), v);
    CallThis(FUN_4eeaf0, F(BYTE*, s, 21600), *(DWORD*)&f);
}

// [$L_55e920]
extern "C" void __fastcall Car_55e920(BYTE* s, void*, int a, int b, int c)
{
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return;
    CallVirt(t, 160, a, b, c);
    Car_PullCab(s);
}
// [$L_55e5f0]
extern "C" void __fastcall Car_55e5f0(BYTE* s, void*, int a, int b, int c)
{
    CallVirt(s, 132);
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return;
    CallVirt(t, 128, a, b, c);
    Car_PullCab(s);
}
// [$L_55e690]
extern "C" void __fastcall Car_55e690(BYTE* s)
{
    CallVirt(s, 132);
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return;
    CallVirt(t, 124);
    Car_PullCab(s);
}
// [$L_55e7c0]
extern "C" int __fastcall Car_55e7c0(BYTE* s, void*, int a, int b)
{
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return 0;
    int r = CallVirt<int>(t, 152, a, b);
    Car_PullCab(s);
    return r;
}
// [$L_55e870]
extern "C" int __fastcall Car_55e870(BYTE* s, void*, int a, int b)
{
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return 0;
    int r = CallVirt<int>(t, 156, a, b);
    Car_PullCab(s);
    return r;
}

// [$L_54c8d0] (a, b) to the body, the part at +21588 and in mode 1 (+10908) the cab (+21584)
extern "C" void __fastcall Car_54c8d0(BYTE* s, void*, int a, int b)
{
    CallThis(FUN_581230, s, a, b);
    if (F(BYTE*, s, 21588))
        CallThis(FUN_561d10, F(BYTE*, s, 21588), a, b);
    BYTE* c = F(BYTE*, s, 21584);
    if (!c || F(int, s, 10908) != 1)
        return;
    if (F(int, s, 892)) {
        CallThis(FUN_4f1240, c, 1, a, b);
        return;
    }
    if (F(int, c, 13124))
        CallThis(FUN_4f0fc0, c, F(int, s, 800), s + 500);
}

// [$L_546ad0] the sound of the cab model (1488-byte records at 6d2114, +36) for the gear mode (+9896)
extern "C" int __fastcall Car_546ad0(BYTE* s, void*, int a, int b)
{
    int k = -1;
    switch (F(BYTE, s, 9896)) {
    case 1: k = 2; break;
    case 2: k = 0; break;
    case 3: k = 1; break;
    }
    if (k >= 0)
        CallC(FUN_4e480c, F(int, (BYTE*)g_veh6d2114, F(int, s, 10148) * 1488 + 36), k);
    return CallThis<int>(FUN_5f9b00, s, a, b);
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_43f840(); KING_ALIAS("_FUN_43f840", "_Player_Value156")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
ASM_PROC(FUN_501b90)
extern "C" void FUN_501e30(); KING_ALIAS("_FUN_501e30", "@Wheel_StepLevel@20")
ASM_VAR(int, g_veh68b990, $L_68b990)
ASM_VAR(AsmLabel, g_veh6d2510, $L_6d2510)
ASM_VAR(AsmLabel, g_veh6d219c, $L_6d219c)
ASM_VAR(AsmLabel, g_veh6d256c, $L_6d256c)
ASM_VAR(AsmLabel, g_veh69809c, $L_69809c)

// a field of the cab model's record (1488 bytes, model +10148) at base
static int Car_ModelField(BYTE* s, BYTE* base) { return F(int, base, F(int, s, 10148) * 1488); }

// [$L_55ee80] the lamps of the cab model: the trailer's (+21608) player's lamp frame, the hitch light, the turn light
extern "C" int __fastcall Car_Lamps(BYTE* s, void*, int a, int b)
{
    BYTE* t = F(BYTE*, s, 21608);
    if (t) {
        if (F(int, t, 20636) != g_veh68b990) {
            int n = CallC<int>(FUN_43f840, F(int, t, 20636)) + 1;
            CallC(FUN_5e1a10, Car_ModelField(s, (BYTE*)g_veh6d2510 + 16), n);
        } else {
            CallC(FUN_5e1a10, Car_ModelField(s, (BYTE*)g_veh6d2510 + 16), 0);
        }
        int v = Car_ModelField(s, (BYTE*)g_veh6d219c + 92);
        if (v)
            CallC(FUN_4e480c, v, 0);
    } else {
        int v = Car_ModelField(s, (BYTE*)g_veh6d219c + 92);
        if (v)
            CallC(FUN_4e480c, v, 1);
    }
    int w = Car_ModelField(s, (BYTE*)g_veh6d256c + 336);
    if (w)
        CallC(FUN_5e1a10, w, F(int, s, 20124) == 2 ? 1 : 0);
    return CallThis<int>(FUN_547030, s, a, b);
}

// a wheel step of the cab when its level pair is equal (cab +15520.. by 20), while all still pass
static void Car_StepLevel(BYTE* s, int& ok, int off, int k, int a, int v)
{
    BYTE* c = F(BYTE*, s, 21600);
    if (F(int, c, off) != F(int, c, off + 4))
        return;
    if (ok && CallThis<int>(FUN_501e30, c, k, a, v))
        ok = 1;
    else
        ok = 0;
}
// [$L_555910] the cab's wheels follow level a: the model's wheel count (69809c +44, 2416-byte records) then the two axles
extern "C" int __fastcall Car_WheelLevels(BYTE* s, void*, int a)
{
    if (!F(BYTE*, s, 21600))
        return 0;
    CallThis(FUN_501b90, F(BYTE*, s, 21600), 2, a, F(int, s, 10476));
    int ok = 1;
    for (int i = 0, o = 0; i < F(int, (BYTE*)g_veh69809c, F(int, s, 10420) * 2416 + 44) && o < 40; i++, o += 20)
        Car_StepLevel(s, ok, 15580 + o, i + 3, a, F(int, s, 10480 + i * 4));
    Car_StepLevel(s, ok, 15520, 0, a, F(int, s, 10468));
    Car_StepLevel(s, ok, 15540, 1, a, F(int, s, 10472));
    return ok;
}

extern "C" void FUN_55ed50(); KING_ALIAS("_FUN_55ed50", "@Car_55ed50@8")
extern "C" void FUN_558930(); KING_ALIAS("_FUN_558930", "@Car_558930@8")
ASM_PROC(FUN_544df0)
extern "C" void FUN_592bf0(); KING_ALIAS("_FUN_592bf0", "@ListItem592bf0_Construct@12")
extern "C" void FUN_52fd90(); KING_ALIAS("_FUN_52fd90", "@Obj52fd90_Reset@8")
ASM_VAR(AsmLabel, g_veh64f96c, $L_64f96c)
ASM_VAR(AsmLabel, g_veh64f880, $L_64f880)
ASM_VAR(AsmLabel, g_veh64f834, $L_64f834)
ASM_VAR(AsmLabel, g_veh64f830, $L_64f830)
ASM_VAR(AsmLabel, g_veh64fb8c, $L_64fb8c)
ASM_VAR(AsmLabel, g_veh64faa0, $L_64faa0)
ASM_VAR(AsmLabel, g_veh64fa54, $L_64fa54)
ASM_VAR(AsmLabel, g_veh64fa50, $L_64fa50)
ASM_VAR(AsmLabel, g_veh6d20dc, $L_6d20dc)
ASM_VAR(AsmLabel, g_veh64f2ec, $L_64f2ec)
ASM_VAR(AsmLabel, g_veh64f2e8, $L_64f2e8)

// a truck's vtables (+0, +16, +272, +392); its trailer (+21880) is unhitched first
static void Car_DestroyWith(BYTE* s, AsmLabel& v0, AsmLabel& v16, AsmLabel& v272, AsmLabel& v392, void (*unhitch)())
{
    F(void*, s, 0) = v0;
    F(void*, s, 16) = v16;
    F(void*, s, 272) = v272;
    F(void*, s, 392) = v392;
    if (F(BYTE*, s, 21880))
        CallThis(unhitch, F(BYTE*, s, 21880));
    F(int, s, 21880) = 0;
    CallThis(FUN_544df0, s);
}
// [FUN_5587d0]
extern "C" void __fastcall Truck5587d0_Destroy(BYTE* s) { Car_DestroyWith(s, g_veh64f96c, g_veh64f880, g_veh64f834, g_veh64f830, FUN_55ed50); }
// [FUN_55ebf0]
extern "C" void __fastcall Truck55ebf0_Destroy(BYTE* s) { Car_DestroyWith(s, g_veh64fb8c, g_veh64faa0, g_veh64fa54, g_veh64fa50, FUN_558930); }

// [FUN_530120] constructor: the controls record (6d20dc) and an empty part list
extern "C" BYTE* __fastcall Controls530120_Construct(BYTE* s)
{
    CallThis(FUN_592bf0, s, (BYTE*)g_veh6d20dc);
    F(void*, s, 0) = g_veh64f2ec;
    CallThis(FUN_52fd90, s + 36);
    F(int, s, 32) = 0;
    F(int, s, 28) = 0;
    F(int, s, 24) = 0;
    F(int, s, 20) = 0;
    F(int, s, 16) = 0;
    F(void*, s, 0) = g_veh64f2e8;
    return s;
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_53e230(); KING_ALIAS("_FUN_53e230", "@Light_Setup@8")
extern "C" void FUN_5c3070(); KING_ALIAS("_FUN_5c3070", "@Table_Add6@20")
extern "C" void FUN_5319f0(); KING_ALIAS("_FUN_5319f0", "@Player_DetachCar@8")
extern "C" void FUN_4eac8e(); KING_ALIAS("_FUN_4eac8e", "@Arrays_Free@8")
extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")
ASM_VAR(AsmLabel, s_veh6792e8, $L_6792e8)
ASM_VAR(AsmLabel, g_veh64f428, $L_64f428)
ASM_VAR(AsmLabel, g_veh64f2f0, $L_64f2f0)

// [FUN_53e420] constructor (a, b): a named object (100) registered with a's media (+7976)
extern "C" BYTE* __fastcall Car53e420_Construct(BYTE* s, void*, BYTE* a, int b)
{
    CallThis(FUN_5d31e0, s, 100, (const char*)s_veh6792e8);
    F(void*, s, 0) = g_veh64f428;
    CallThis(FUN_53e230, s);
    CallThis(FUN_5c3070, a + 7976, b, s + 36, 4);
    return s;
}

// [FUN_531950] (vtable 64f2f0): its parts, the five sound holders (+408..+280)
extern "C" void __fastcall Car531950_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_veh64f2f0;
    CallThis(FUN_5319f0, s);
    CallThis(FUN_4eac8e, s + 408);
    CallThis(FUN_4eac8e, s + 376);
    CallThis(FUN_4eac8e, s + 344);
    CallThis(FUN_4eac8e, s + 312);
    CallThis(FUN_4eac8e, s + 280);
    CallThis(FUN_5d3290, s);
}

// ---------------------------------------------------------------------------
// Network messages and small constructors (exception frames dropped).

extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
ASM_VAR(short, g_veh67d10a, $L_67d10a)
ASM_VAR(BYTE*, g_veh6ef1fc, $L_6ef1fc)

static void Car_Send(const void* d, int n)
{
    BYTE pkt[32];
    CallThis(FUN_602be0, pkt);
    CallThis(FUN_603110, pkt, (int)g_veh67d10a, d, n);
    CallThis(FUN_5c3630, g_veh6ef1fc, pkt);
    CallThis(FUN_602e80, pkt);
}
// [FUN_545e20] message 5: the car (+1204), +412, the position +420 and the matrix +708
extern "C" void __fastcall Car_SendState(BYTE* s)
{
    int d[18];
    d[0] = F(int, s, 1204);
    d[1] = 5;
    d[2] = F(int, s, 412);
    d[3] = F(int, s, 420);
    d[4] = F(int, s, 424);
    d[5] = F(int, s, 428);
    for (int i = 0; i < 12; i++)
        d[6 + i] = F(int, s, 708 + i * 4);
    Car_Send(d, 72);
}
// [FUN_545cc0] message 8 of kind k (1 with +9544, 2 with +20396)
extern "C" void __fastcall Car_SendKind(BYTE* s, void*, int k)
{
    int d[4] = {F(int, s, 1204), 8, k, 0};
    int n = 12;
    if (k == 1) {
        d[3] = F(int, s, 9544);
        n = 16;
    } else if (k == 2) {
        d[3] = F(int, s, 20396);
        n = 16;
    }
    Car_Send(d, n);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

#include <string.h>
extern "C" void FUN_5c32f0(); KING_ALIAS("_FUN_5c32f0", "@List_Add6@20")
ASM_VAR(short, g_veh672d84, $L_672d84)
ASM_VAR(AsmLabel, s_veh672bd0, $L_672bd0)

// [FUN_53e320] constructor (game): the controls object registered with the game's input (+8300); message 900 tells the others
extern "C" BYTE* __fastcall Controls_Construct(BYTE* s, void*, BYTE* game)
{
    CallThis(FUN_5d31e0, s, 100, (const char*)s_veh6792e8);
    F(void*, s, 0) = g_veh64f428;
    CallThis(FUN_53e230, s);
    int id = g_veh672d84;
    CallThis(FUN_5c32f0, game + 8300, id, s + 36, 4);
    BYTE pkt[32];
    CallThis(FUN_602be0, pkt);
    BYTE d[260];
    strcpy((char*)d, (const char*)s_veh672bd0);
    F(int, d, 160) = id;
    CallThis(FUN_603110, pkt, 900, d, 250);
    CallThis(FUN_5c3630, game, pkt);
    CallThis(FUN_602e80, pkt);
    return s;
}

// [FUN_546360] message 7: the cab's 11 lamps (state byte +21608, position +21652, value +21784)
extern "C" void __fastcall Car_SendLamps(BYTE* s)
{
    if (!F(int, s, 1220) || F(int, s, 388))
        return;
    BYTE d[200];
    F(int, d, 0) = F(int, s, 1204);
    F(int, d, 4) = 7;
    int n = 8;
    for (int k = 0; k < 11; k++) {
        d[n] = F(BYTE, s, 21608 + k * 4);
        memcpy(d + n + 1, s + 21652 + k * 12, 12);
        memcpy(d + n + 13, s + 21784 + k * 4, 4);
        n += 17;
    }
    Car_Send(d, n);
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

ASM_PROC(FUN_544df0)
// [$L_543de0]
extern "C" BYTE* __fastcall Dtor543de0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_544df0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_55ebf0(); KING_ALIAS("_FUN_55ebf0", "@Truck55ebf0_Destroy@4")
// [$L_55ebd0]
extern "C" BYTE* __fastcall Dtor55ebd0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_55ebf0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_5587d0(); KING_ALIAS("_FUN_5587d0", "@Truck5587d0_Destroy@4")
// [$L_5587b0]
extern "C" BYTE* __fastcall Dtor5587b0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5587d0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_57ca80(); KING_ALIAS("_FUN_57ca80", "_Ids_Reset")
extern "C" void FUN_4eac13(); KING_ALIAS("_FUN_4eac13", "@Arrays_Fill@16")
ASM_VAR(int, g_q47_6ef200, $L_6ef200)
// [$L_4de510] resets the world state: counters +5084, +5088, +5104, +5108, +5112, the bounds +5116, +5120 to
// -1000 and the arrays at +4828
extern "C" void __fastcall World_ResetState(BYTE* s, void*)
{
    F(int, s, 5084) = 0;
    F(int, s, 5088) = 0;
    F(int, s, 5104) = 0;
    F(DWORD, s, 5120) = 0xc47a0000;
    F(DWORD, s, 5116) = 0xc47a0000;
    F(int, s, 5108) = 0;
    F(int, s, 5112) = 0;
    CallThis(FUN_4eac13, s + 4828, 0, 0);
}
// [FUN_53fd40] resets the ids, empties the vehicle list and resets the world 696cb0
extern "C" void __cdecl World_ResetAll(void)
{
    CallC(FUN_57ca80);
    g_vehicleList = 0;
    g_q47_6ef200 = 0;
    World_ResetState(g_vh696cb0, 0);
}

// ---------------------------------------------------------------------------
// Small setup procs: static vectors, a light array, the race start, a stream read.

extern "C" void FUN_57ca00(); KING_ALIAS("_FUN_57ca00", "_Race_SetMode")
ASM_PROC(FUN_4e24e8)
ASM_PROC(FUN_4de290)
ASM_PROC(FUN_53fdb0)
ASM_VAR(int, g_q48_6ef1fc, $L_6ef1fc)
// [FUN_53fcf0] starts a race (a, b, mode c, d): the mode, the race globals, 6ef1fc = a, the world 696cb0 for a,
// then 53fdb0
extern "C" void __cdecl Race_Start(int a, int b, int c, int d)
{
    CallC(FUN_57ca00, c);
    CallC(FUN_4e24e8, a, b, d);
    g_q48_6ef1fc = a;
    CallThis(FUN_4de290, g_vh696cb0, a);
    CallC(FUN_53fdb0);
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

extern "C" void FUN_5319f0(); KING_ALIAS("_FUN_5319f0", "@Player_DetachCar@8")
ASM_PROC(FUN_552940)
ASM_VAR(BYTE*, g_q49_6d2078, $L_6d2078)
// [$L_531aa0] detaches the car of the player 6d2078 unless keep
extern "C" void __cdecl Player_DetachCarUnless(int keep)
{
    if (g_q49_6d2078 && !keep)
        CallThis(FUN_5319f0, g_q49_6d2078);
}
// [$L_548100] releases the +9616 object (552940) and clears it
extern "C" void __fastcall Veh548100_ReleaseAttachment(BYTE* s, void*)
{
    if (F(int, s, 9616))
        CallThis(FUN_552940, s);
    F(int, s, 9616) = 0;
}

// ---------------------------------------------------------------------------
// Part frees and the item event handler.

extern "C" void FUN_4e260c(); KING_ALIAS("_FUN_4e260c", "_World_Fatal")
ASM_VAR(void*, g_q54_6ef250, $L_6ef250)
ASM_VAR(AsmLabel, s_q54_679f8c, $L_679f8c)
// [$L_543d90] frees the two parts (+0, +4) with their +12 blocks
extern "C" void __fastcall Veh543d90_FreeParts(BYTE* s, void*)
{
    for (int k = 0; k < 2; k++) {
        BYTE* p = F(BYTE*, s, k * 4);
        if (p) {
            if (F(void*, p, 12))
                operator delete(F(void*, p, 12));
            operator delete(p);
        }
    }
}
// [$L_545550] the item event handler: the 6ef250 hook first; items 3xxxxx with no thousands delete their +28
// object (1), a missing one is fatal
extern "C" int __cdecl Item_OnEvent(BYTE* it, int e)
{
    if (g_q54_6ef250) {
        int r = ((int(__cdecl*)(BYTE*, int))g_q54_6ef250)(it, e);
        if (r)
            return r;
    }
    int id = F(int, it, 4);
    if (id / 100000 == 3 && id % 100000 / 1000 == 0) {
        if (F(BYTE*, it, 28)) {
            CallVirt(F(BYTE*, it, 28), 0, 1);
            return 1;
        }
        CallC(FUN_4e260c, (const char*)s_q54_679f8c);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Car messages.

ASM_PROC(FUN_545ef0)
extern "C" void FUN_546300(); KING_ALIAS("_FUN_546300", "@Car_Restore546300@12")
extern "C" void FUN_546480(); KING_ALIAS("_FUN_546480", "@Car_Restore546480@12")
extern "C" void FUN_545d90(); KING_ALIAS("_FUN_545d90", "@Car_Command@12")
extern "C" void FUN_545dc0(); KING_ALIAS("_FUN_545dc0", "@Car_SetBytes@12")
extern "C" int __fastcall SceneObj_OnMessage(BYTE* s, void*, int* m);    // moving_items.cpp
ASM_VAR(AsmLabel, s_q57_679ff0, $L_679ff0)
// [$L_5464e0] handles the message m for the car (this at +16 of it): the scene object part first, then the car
// types 5, 7, 8, 9, 11 with the data (+8); 1 when handled
extern "C" int __fastcall Car_OnMessage(BYTE* s, void*, int* m)
{
    if (SceneObj_OnMessage(s, 0, m))
        return 1;
    if (m[0] != F(int, s, 1188))
        CallC(FUN_4e260c, (const char*)s_q57_679ff0);
    BYTE* car = s - 16;
    switch (m[1]) {
    case 5: CallThis(FUN_545ef0, car, m + 2); return 1;
    case 7: CallThis(FUN_546480, car, m + 2); return 1;
    case 8: CallThis(FUN_545d90, car, m + 2); return 1;
    case 9: CallThis(FUN_545dc0, car, m + 2); return 1;
    case 11: CallThis(FUN_546300, car, m + 2); return 1;
    }
    return 0;
}

static int Veh102_Abs(int v) { return v < 0 ? -v : v; }
// [$L_555e50] whether the controls (+9536) of car s have any input on +24, +44 or +64
extern "C" int __fastcall Car_HasInput(BYTE* s)
{
    BYTE* o = F(BYTE*, s, 9536);
    return (Veh102_Abs(F(int, o, 64)) + Veh102_Abs(F(int, o, 44)) + Veh102_Abs(F(int, o, 24))) != 0;
}

// ---------------------------------------------------------------------------
// Placing a car; its level steps.

extern "C" void FUN_581230(); KING_ALIAS("_FUN_581230", "@SceneObj_SetPosition@16")
extern "C" void FUN_561d10(); KING_ALIAS("_FUN_561d10", "@Copter_SetMatrix@16")
extern "C" void FUN_589490(); KING_ALIAS("_FUN_589490", "@BodyLevel_Step@16")
ASM_PROC(FUN_4f1240)
ASM_VAR(float, g_q112_64c250, $L_64c250)
struct Veh112_M { DWORD v[12]; };
// [FUN_54c820] places car s at a with matrix m: its scene object (+16), the helicopter part (+21604) and the
// body (+21600, with a copy of m)
extern "C" void __fastcall Car_Place(BYTE* s, void*, void* a, const BYTE* m)
{
    CallThis(FUN_581230, s + 16, a, m);
    if (BYTE* h = F(BYTE*, s, 21604))
        CallThis(FUN_561d10, h, a, m);
    if (BYTE* b = F(BYTE*, s, 21600)) {
        Veh112_M c = *(const Veh112_M*)m;
        CallThis(FUN_4f1240, b, 2, a, &c);
    }
}

// [FUN_555ee0] steps the 9 levels of the car's controls (+9536, 20 bytes each) by *v (at most 1) with the
// values of +10432
extern "C" void __fastcall Car_StepAllLevels(BYTE* s, void*, float* v)
{
    if (*v > g_q112_64c250)
        *v = 1.0f;
    for (int i = 0; i < 9; i++)
        CallThis(FUN_589490, F(BYTE*, s, 9536) + i * 20, v, F(int, s, 10432 + i * 4));
}

// ---------------------------------------------------------------------------
// Car lights and effects.

ASM_PROC($L_549580)
extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")
extern "C" void FUN_4ba060(); KING_ALIAS("_FUN_4ba060", "_Effect_New4daf40")
extern "C" void FUN_54a310(); KING_ALIAS("_FUN_54a310", "@Flasher_Release@8")
ASM_VAR(float, g_q129_64c938, $L_64c938)
ASM_VAR(double, g_q129_64b840, $L_64b840)
ASM_VAR(AsmLabel, g_q129_6981b0, $L_69818e)
ASM_VAR(BYTE*, g_q129_69622c, $L_69622c)
// [FUN_5565d0] the value of +9572 when +9600 is set, else the 64c938 default
extern "C" float __fastcall Car_5565d0(BYTE* s) { return F(int, s, 9600) ? F(float, s, 9572) : g_q129_64c938; }

// [FUN_549900] starts effect s (state +20's +12 from 0 to 1) at t: its end time (+28), its start (+4); a car
// (+24) of type kind 3 gets the 549580 effect
extern "C" void __fastcall CarEffect_Start(BYTE* s, void*, float t)
{
    BYTE* o = F(BYTE*, s, 20);
    if (F(int, o, 12))
        return;
    F(float, s, 28) = (float)(t + g_q129_64b840);
    F(int, o, 12) = 1;
    F(float, s, 4) = (float)CallC<double>(FUN_4e27a5);
    BYTE* car = F(BYTE*, s, 24);
    if (F(int, (BYTE*)g_q129_6981b0, 34 + F(int, car, 10420) * 2416) == 3)
        CallC(FUN_4ba060, car, $L_549580);
}

// [FUN_549b90] stops effect s (state 1 or 2 becomes 4): the stop time (+8), the 69622c sound off, the flasher
// (+16) freed
extern "C" void __fastcall CarEffect_Stop(BYTE* s)
{
    BYTE* o = F(BYTE*, s, 20);
    if (F(int, o, 12) != 1 && F(int, o, 12) != 2)
        return;
    F(int, o, 12) = 4;
    F(float, s, 8) = (float)CallC<double>(FUN_4e27a5);
    CallVirt(g_q129_69622c, 64, 0);
    if (BYTE* p = F(BYTE*, s, 16)) {
        CallThis(FUN_54a310, p);
        operator delete(p);
    }
    F(BYTE*, s, 16) = 0;
}

// ---------------------------------------------------------------------------
// Sums over the car's wheels and controls.

ASM_VAR(float, g_q158_64c264, $L_64c264)
ASM_VAR(AsmLabel, g_q158_69809c, $L_69809c)
// the wheel count of car s's type
static int Veh158_Wheels(BYTE* s) { return F(int, (BYTE*)g_q158_69809c, 44 + F(int, s, 10420) * 2416); }
// [FUN_555c20] the weighted sum of the wheels' two loads (body +15752 and +15912, every 20 bytes; weights +10512
// and +10544)
extern "C" double __fastcall Car_WheelLoads(BYTE* s)
{
    double v = g_q158_64c264;
    int n = Veh158_Wheels(s);
    const BYTE* w = F(BYTE*, s, 21600) + 15912;
    const float* d = (const float*)(s + 10544);
    for (; n > 0; n--, w += 20, d++) {
        v = x87_fabs(F(float, w, -160)) * d[-8] + v;
        v = v + x87_fabs(F(float, w, 0)) * d[0];
    }
    return v;
}

// [FUN_555d10] the weighted sum of the wheels' +15592 values (weights +10480)
extern "C" double __fastcall Car_WheelSum(BYTE* s)
{
    double v = g_q158_64c264;
    int n = Veh158_Wheels(s);
    const BYTE* w = F(BYTE*, s, 21600) + 15592;
    const float* d = (const float*)(s + 10480);
    for (; n > 0; n--, w += 20, d++)
        v = x87_fabs(F(float, w, 0)) * d[0] + v;
    return v;
}

// [FUN_555e10] the weighted sum of four controls (+9536: +32, +12, +72, +52; weights +10436.. )
extern "C" double __fastcall Car_ControlSum4(BYTE* s)
{
    const BYTE* o = F(BYTE*, s, 9536);
    double v = x87_fabs(F(float, o, 32)) * F(float, s, 10436) + x87_fabs(F(float, o, 12)) * F(float, s, 10432);
    v = v + x87_fabs(F(float, o, 72)) * F(float, s, 10444);
    return v + x87_fabs(F(float, o, 52)) * F(float, s, 10440);
}

// [FUN_555e80] the weighted sum of the 9 controls (+9536 from +12, every 20 bytes; weights +10432..)
extern "C" double __fastcall Car_ControlSum9(BYTE* s)
{
    double v = g_q158_64c264;
    const BYTE* o = F(BYTE*, s, 9536) + 12;
    const float* d = (const float*)(s + 10432);
    for (int k = 0; k < 9; k++)
        v = v + F(float, o, k * 20) * d[k];
    return v;
}

// [FUN_542250] x clamped to -lim..lim
extern "C" double __cdecl Clamp542250(float x, float lim)
{
    if (x < -lim)
        return -lim;
    if (x <= lim)
        return x;
    return lim;
}

// ---------------------------------------------------------------------------
// Resetting a car's controls.

extern "C" void FUN_5494c0(); KING_ALIAS("_FUN_5494c0", "@CarEffect_End@12")
extern "C" void FUN_4eb6e0(); KING_ALIAS("_FUN_4eb6e0", "@Ramp_Down@12")
extern "C" void FUN_4eb720(); KING_ALIAS("_FUN_4eb720", "@Ramp_FromTop@12")
extern "C" void FUN_549bf0(); KING_ALIAS("_FUN_549bf0", "@Obj549bf0_Get@8")
ASM_VAR(double, g_q169_64b840, $L_64b840)
// [FUN_555a70] resets car s with a: the body's setting (+10468), its ramp (body +15520: 0/1 from the top, 2 down
// by 0.6), the +21568 state 1 or 2 restarted (5494c0), the +21596 time from the controls' +32
extern "C" void __fastcall Car_ResetControls(BYTE* s, void*, int a)
{
    CallThis(FUN_501b90, F(BYTE*, s, 21600), 0, a, F(int, s, 10468));
    BYTE* b = F(BYTE*, s, 21600);
    int st = F(int, b, 15520);
    if (st == 0 || st == 1)
        CallThis(FUN_4eb720, b + 16060, 0);
    else if (st == 2)
        CallThis(FUN_4eb6e0, b + 16060, 0xbf19999a);
    if (CallThis<int>(FUN_549bf0, s + 21568) == 1 || CallThis<int>(FUN_549bf0, s + 21568) == 2)
        CallThis(FUN_5494c0, s, 1);
    F(float, s, 21596) = (float)(F(float, F(BYTE*, s, 9536), 32) + g_q169_64b840);
}

extern "C" void FUN_4eb5e0(); KING_ALIAS("_FUN_4eb5e0", "@Ramp_Value@8")
extern "C" void FUN_52f930(); KING_ALIAS("_FUN_52f930", "_Sound_Play3_386")
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
extern "C" void __fastcall CarEffect_Stop(BYTE* s);
ASM_VAR(BYTE*, g_q170_6d2078, $L_6d2078)
ASM_VAR(int, g_q170_6ef260, $L_6ef260)
// [FUN_5494c0] ends the running effect (+21568, state 1 or 2) of car s; with keep 0 it first uses up one unit
// of the +21360 ramp (not when it is empty; message 100 when it runs out); the player's car plays the 6ef260 sound
extern "C" void __fastcall CarEffect_End(BYTE* s, void*, int keep)
{
    int use = keep == 0;
    int st = CallThis<int>(FUN_549bf0, s + 21568);
    if (st != 1 && CallThis<int>(FUN_549bf0, s + 21568) != 2)
        return;
    if (use) {
        if (!F(int, s, 21400))
            return;
        BYTE* r = s + 21360;
        if (!(CallThis<double>(FUN_4eb5e0, r) > g_q158_64c264))
            return;
        CallThis(FUN_4eb6e0, r, 0x3f800000);
        if (!(CallThis<double>(FUN_4eb5e0, r) > g_q158_64c264))
            CallC(FUN_56ae60, CallC<const char*>(FUN_5d2e50, 100));
    }
    CarEffect_Stop(s + 21568);
    if (F(BYTE*, g_q170_6d2078, 616) == s)
        CallC(FUN_52f930, g_q170_6ef260, 1.0f);
}

// ---------------------------------------------------------------------------
// Loading cargo onto a car.

ASM_PROC(FUN_556780)
ASM_PROC(FUN_556890)
ASM_PROC(FUN_5455d0)
extern "C" void FUN_5569c0(); KING_ALIAS("_FUN_5569c0", "@Obj5569c0_Take@20")
// sets car s's total load (+9588 + +9604 + +9580) and its centre (the cargo point +9556 weighted by the two
// loads, over the total) through 5455d0
static int Veh184_ApplyLoad(BYTE* s)
{
    const float* p = (const float*)(s + 9556);
    float m = (float)(F(float, s, 9588) + F(float, s, 9604) + F(float, s, 9580));
    double k1 = F(float, s, 9588) + F(float, s, 9580);
    float ax = (float)(p[0] * k1), ay = (float)(p[1] * k1);
    double az = k1 * p[2];
    double k2 = F(float, s, 9604);
    float bx = (float)(p[0] * k2), by = (float)(p[1] * k2);
    double bz = k2 * p[2];
    float sx = (float)(bx + ax), sy = (float)(ay + by);
    double sz = az + bz;
    float v[3] = {(float)(sx / m), (float)(sy / m), (float)(sz / m)};
    return CallThis<int>(FUN_5455d0, s, m, v);
}
// [FUN_5561b0] loads cargo a1, a2 into car s's hold (556780) and updates its load; -2 without a hold (+9600)
extern "C" int __fastcall Car_LoadCargo(BYTE* s, void*, int a1, int a2)
{
    if (!F(int, s, 9600))
        return -2;
    int r = CallThis<int>(FUN_556780, s + 9556, a1, a2);
    Veh184_ApplyLoad(s);
    return r;
}
// [FUN_5562e0] unloads cargo a1, a2 (556890) like 5561b0
extern "C" int __fastcall Car_UnloadCargo(BYTE* s, void*, int a1, int a2)
{
    if (!F(int, s, 9600))
        return -2;
    int r = CallThis<int>(FUN_556890, s + 9556, a1, a2);
    Veh184_ApplyLoad(s);
    return r;
}
// [FUN_556410] takes cargo a1..a3 from the hold (5569c0) and updates the load; 0 without a hold
extern "C" int __fastcall Car_TakeCargo(BYTE* s, void*, int a1, int a2, int a3)
{
    if (!F(int, s, 9600))
        return 0;
    CallThis(FUN_5569c0, s + 9556, a1, a2, a3);
    return Veh184_ApplyLoad(s);
}

ASM_VAR(double, g_q185_64d318, $L_64d318)
ASM_VAR(double, g_q185_64b898, $L_64b898)
ASM_VAR(double, g_q185_64ca78, $L_64ca78)
ASM_VAR(float, g_q185_64c320, $L_64c320)
ASM_VAR(float, g_q185_64c32c, $L_64c32c)
ASM_VAR(float, g_q185_64c938, $L_64c938)
ASM_VAR(float, g_q185_64f744, $L_64f744)
// [FUN_549ff0] the intensity curve of effect s at its time (+20: time +8, kind +12) into *a and *b; the value
// (kinds 1, 2: saturating curves; 3, 4: exponential decays; others 0)
extern "C" double __fastcall CarEffect_Curve(BYTE* s, void*, float* a, float* b)
{
    BYTE* o = F(BYTE*, s, 20);
    float t = F(float, o, 8);
    switch (F(int, o, 12)) {
    case 1:
    case 2:
        *a = (float)(t * (F(int, o, 12) == 1 ? g_q185_64d318 : g_q185_64b898) / (t + g_q185_64c320) + g_q169_64b840);
        *b = (float)(t * g_q185_64ca78 / (t + g_q185_64c32c) + g_q185_64d318);
        return *a;
    case 4:
        *b = 0;
        *a = 1.0f;
        return x87_exp(t * t * g_q185_64c938);
    case 3:
        *a = 1.0f;
        *b = 1.0f;
        return x87_exp(t * g_q185_64f744);
    }
    *a = 0;
    *b = 0;
    return g_q158_64c264;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Car_HasInput(BYTE* s); KING_ALIAS("@q198_Car_HasInput@4", "@Car_HasInput@4")
extern "C" int __fastcall q198_Dtor543de0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor543de0_DeletingDestructor@12", "@Dtor543de0_DeletingDestructor@12")
// [$L_5574e0] forwards to Dtor543de0_DeletingDestructor for the object 16 bytes before
extern "C" int __fastcall Thunk5574e0(BYTE* s, void* d, int a0) { return q198_Dtor543de0_DeletingDestructor(s - 16, d, a0); }
// [$L_55e4a0] forwards to Car_HasInput
extern "C" int __fastcall Thunk55e4a0(BYTE* s) { return q198_Car_HasInput(s); }

ASM_VAR(float, g_q199_64c938, $L_64c938)
ASM_VAR(float, g_q199_64c264, $L_64c264)
// [$L_548120] marks car s (+9616)
extern "C" void __fastcall Car_SetFlag9616(BYTE* s) { F(int, s, 9616) = 1; }
// [$L_54d830] o's method 72 with a
extern "C" int __cdecl Obj_Call72(int a, BYTE* o) { return CallVirt<int>(o, 72, a); }
// [FUN_5565f0] the hold's value +9576, 64c938 without a hold
extern "C" float __fastcall Car_HoldValue(BYTE* s)
{
    if (!F(int, s, 9600))
        return g_q199_64c938;
    return F(float, s, 9576);
}
// [$L_556770] the cargo's load (+24)
extern "C" float __fastcall Cargo_Load(BYTE* c) { return F(float, c, 24); }
// [FUN_556610] the hold's load, 0 without a hold
extern "C" float __fastcall Car_HoldLoad(BYTE* s)
{
    if (!F(int, s, 9600))
        return g_q199_64c264;
    return Cargo_Load(s + 9556);
}
// [FUN_556630] the hold's value +9592, 0 without a hold
extern "C" float __fastcall Car_HoldValue2(BYTE* s)
{
    if (!F(int, s, 9600))
        return g_q199_64c264;
    return F(float, s, 9592);
}

extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" void FUN_4e623e(); KING_ALIAS("_FUN_4e623e", "_Query_Parts1")
ASM_PROC(FUN_57e140)
// [$L_54c6d0] the car's world query (kind 6) into the 208-byte record out: a query from the car's position +584
// and direction +536 against its body (+804), through the parts list when the model (+21584) has one (+9992) or the
// body's method 52 otherwise (on a new contact the car's method 28 is told); without a model, 57e140
extern "C" int __fastcall Car_Query(BYTE* s, void*, BYTE* out)
{
    BYTE* m = F(BYTE*, s, 21584);
    if (!m || F(int, m, 13124))
        return CallThis<int>(FUN_57e140, s, out);
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    memcpy(q + 4, g_matrix7025f0, 48);
    F(int, q, 76) = F(int, s, 584);
    F(int, q, 116) = 0;
    F(int, q, 80) = F(int, s, 588);
    F(int, q, 100) = 0;
    F(int, q, 88) = F(int, s, 536);
    F(int, q, 84) = F(int, s, 592);
    F(int, q, 92) = F(int, s, 540);
    F(int, q, 96) = F(int, s, 544);
    F(BYTE*, q, 120) = s - 16;
    F(int, q, 0) = F(int, s, 40);
    int r;
    if (F(WORD, m, 9992)) {
        r = CallC<int>(FUN_4e623e, F(BYTE*, s, 804), 6, q);
    } else {
        r = CallVirt<int>(F(BYTE*, s, 804), 52, 6, q);
        if ((r & 2) && !F(int, q, 104)) {
            F(BYTE*, q, 104) = F(BYTE*, s, 804);
            CallVirt(s, 28, 1);
        }
    }
    memcpy(out, q, 208);
    return r;
}

ASM_PROC(FUN_4de990)
ASM_PROC(FUN_5de950)
extern "C" void FUN_5da950(); KING_ALIAS("_FUN_5da950", "_Matrix_FromPose")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5d85f0(); KING_ALIAS("_FUN_5d85f0", "@Win_GetRect@12")
ASM_VAR(int, g_q234_6cec68, $L_6cec68)
struct Veh234_Mat { DWORD v[12]; };
struct Veh234_Rect { int l, t, r, b; };
// [FUN_535f80] the car's view matrix (+1568) into out; in the game view (6cec68) the camera pose is also taken
// apart and rebuilt level (the results are not used) and the view window (+32) is fitted to the screen below the top
// 672d7c (method 44 when its rectangle differs)
extern "C" BYTE* __fastcall Car_ViewMatrix(BYTE* s, void*, BYTE* out)
{
    if (!g_q234_6cec68) {
        memcpy(out, s + 1568, 48);
        return out;
    }
    Veh234_Mat m;
    CallC(FUN_4de990, &m, 0);
    double x, y, z, a, b, c;
    CallC(FUN_5de950, m, &x, &y, &z, &a, &b, &c);
    BYTE level[48], inv[48];
    CallC(FUN_5da950, level, x, y, z, 0.0, b, 0.0);
    CallC(FUN_5ac2e0, inv, level);
    BYTE view[48];
    memcpy(view, s + 1568, 48);
    BYTE* w = F(BYTE*, s, 36);
    int right = F(int, w, 256), bottom = F(int, w, 260), top = g_minTop672d7c;
    Veh234_Rect r;
    CallThis(FUN_5d85f0, F(BYTE*, s, 32), &r);
    if (r.l || r.t != top || r.r != right || r.b != bottom) {
        Veh234_Rect n = {0, top, right, bottom};
        CallVirt(F(BYTE*, s, 32), 44, n);
    }
    memcpy(out, view, 48);
    return out;
}

ASM_PROC(FUN_56ffd0)
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_532cf0(); KING_ALIAS("_FUN_532cf0", "_Points_Nearest")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
ASM_VAR(BYTE*, g_q235_6cec6c, $L_6cec6c)
ASM_VAR(AsmLabel, g_q235_6cf3a8, $L_6cf3a8)
// [$L_532f00] a message m for the player's truck (the game 6d2078, +616 the truck): 11 finds the nearest point to
// the truck, 27 changes the gear-box mode (+1412, the last two kept at +1416, +1420; mode 2 clears +20944, the
// switches with mode 3 show the hint 87 once, +1408), others go to 56ffd0 while the game view is current
extern "C" void __cdecl Game_TruckMessage(int* m)
{
    BYTE* g = g_game;
    if (!m || !g || !F(BYTE*, g, 616))
        return;
    if (m[0] == 11) {
        BYTE l[48];
        BYTE* r = CallThis<BYTE*>(FUN_57de90, F(BYTE*, g, 616) + 16, l);
        CallC(FUN_532cf0, m, r + 36, (void*)g_q235_6cf3a8);
        return;
    }
    if (m[0] != 27) {
        if (F(BYTE*, g_q235_6cec6c, 1356) == g_696ca0)
            CallC(FUN_56ffd0, m);
        return;
    }
    if (F(int, g, 1412) != m[1]) {
        F(int, g, 1420) = F(int, g, 1416);
        F(int, g_game, 1416) = F(int, g_game, 1412);
        F(int, g_game, 1412) = m[1];
        g = g_game;
    }
    if (m[1] == 2) {
        F(int, F(BYTE*, g, 616), 20944) = 0;
        g = g_game;
    }
    if (m[1] == 1) {
        F(int, g, 1408) = 0;
        int f = CallC<int>(FUN_43f620, F(int, F(BYTE*, g_game, 616), 20636));
        g = g_game;
        if (!(f & 0x8000) && F(int, g, 1420) == 3) {
            F(int, F(BYTE*, g, 616), 20944) = 34;
            g = g_game;
        }
    }
    if (m[1] == 2 && F(int, g, 1416) == 3 && !F(int, g, 1408)) {
        int f = CallC<int>(FUN_43f620, F(int, F(BYTE*, g, 616), 20636));
        if (!(f & 0x8000)) {
            CallC(FUN_56ae60, CallC<int>(FUN_5d2e50, 87));
            F(int, g_game, 1408) = 1;
        }
    }
}

ASM_PROC(FUN_557090)
ASM_PROC(FUN_4ca7e0)
extern "C" void FUN_5f9d20(); KING_ALIAS("_FUN_5f9d20", "@SceneNode5f9d20_Draw@24")
ASM_VAR(AsmLabel, g_q236_6d217c, $L_6d217c)
// [FUN_546b60] draws the car part s (the car at s - 272): its two lamps of the model record (6d2510 +8, +12) lit
// while +20652 is set and 557090 is above 64ca78 (else +20652 is cleared), the parent hook (s - 256, method 232),
// the light pair of 6d217c (+20648 its strength, none while the car's +9264 shows a lit lamp), the switch +20228 on
// with either light flag, then the node itself
extern "C" void __fastcall CarPart_Draw(BYTE* s, void*, int a, int b, int c, int d)
{
    int on = 0;
    if (F(int, s, 20652) && CallThis<double>(FUN_557090, s - 272) > g_q185_64ca78)
        on = 1;
    else
        F(int, s, 20652) = 0;
    CallC(FUN_5e1a10, Car_ModelField(s, (BYTE*)g_veh6d2510 + 8), on);
    CallC(FUN_5e1a10, Car_ModelField(s, (BYTE*)g_veh6d2510 + 12), on);
    CallVirt(s - 256, 232);
    int k = F(int, s, 20648);
    int l1 = Car_ModelField(s, (BYTE*)g_q236_6d217c + 8), l2 = Car_ModelField(s, (BYTE*)g_q236_6d217c + 12);
    if (l1 || l2) {
        int st = 0;
        if (k) {
            BYTE* e = F(BYTE*, s, 9264);
            if (!(F(int, e, 24) || (F(int, e, 44) && F(int, e, 64))))
                st = k;
        }
        CallC(FUN_4ca7e0, s - 272, l1, l2, st);
    }
    CallC(FUN_5e1a10, F(int, s, 20228), (F(int, s, 20648) || F(int, s, 20644)) ? 1 : 0);
    CallThis(FUN_5f9d20, s, a, b, c, d);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" float __fastcall q238_Car_HoldLoad(BYTE* s); KING_ALIAS("@q238_Car_HoldLoad@4", "@Car_HoldLoad@4")
extern "C" int __fastcall q238_Car_SetFlag9616(BYTE* s); KING_ALIAS("@q238_Car_SetFlag9616@4", "@Car_SetFlag9616@4")
extern "C" int __fastcall q238_Linked592cf0_Destruct(BYTE* s, void* d); KING_ALIAS("@q238_Linked592cf0_Destruct@8", "@Linked592cf0_Destruct@8")
extern "C" int __fastcall q238_Veh548100_ReleaseAttachment(BYTE* s, void* d); KING_ALIAS("@q238_Veh548100_ReleaseAttachment@8", "@Veh548100_ReleaseAttachment@8")
// [FUN_530190] forwards to Linked592cf0_Destruct
extern "C" int __fastcall Thunk530190(BYTE* s, void* d) { return q238_Linked592cf0_Destruct(s, d); }
// [$L_556540] forwards to Car_HoldLoad
extern "C" float __fastcall Thunk556540(BYTE* s) { return q238_Car_HoldLoad(s); }
// [FUN_557440] forwards to Veh548100_ReleaseAttachment
extern "C" int __fastcall Thunk557440(BYTE* s, void* d) { return q238_Veh548100_ReleaseAttachment(s, d); }
// [FUN_557450] forwards to Car_SetFlag9616
extern "C" int __fastcall Thunk557450(BYTE* s) { return q238_Car_SetFlag9616(s); }

// === the car object (FUN_531290)

ASM_VAR(AsmLabel, s_cr67899c, $L_67899c)
ASM_VAR(AsmLabel, g_crIdentity7025f0, $L_7025f0)  // a 4x3 identity matrix
ASM_VAR(AsmLabel, g_crVtbl64f2f0, $L_64f2f0)
ASM_VAR(AsmLabel, g_crHist4eaa41, $L_4eaa41)
ASM_VAR(AsmLabel, g_crHist4eaa77, $L_4eaa77)
ASM_VAR(AsmLabel, g_crHist4eaab0, $L_4eaab0)
ASM_VAR(AsmLabel, g_cr6969d0, $L_6969d0)
ASM_VAR(AsmLabel, g_cr696b98, $L_696b98)
ASM_VAR(AsmLabel, g_cr6969a0, $L_6969a0)
ASM_VAR(AsmLabel, g_cr696740, $L_696740)
ASM_VAR(AsmLabel, g_cr702650, $L_702650)
ASM_VAR(AsmLabel, g_cr696cbc, $L_696cbc)
ASM_VAR(int, g_cr696cb8, $L_696cb8)
ASM_VAR(AsmLabel, s_cr673340, $L_673340)
ASM_VAR(BYTE*, g_cr6cec70, $L_6cec70)
ASM_VAR(BYTE*, g_cr696ca0, $L_696ca0)
ASM_VAR(AsmLabel, g_crNames678748, $L_678748)  // 7 char*
ASM_VAR(AsmLabel, g_cr6d20a0, $L_6d20a0)
ASM_VAR(AsmLabel, g_cr6d20bc, $L_6d20bc)
ASM_VAR(AsmLabel, g_crDir702620, $L_702620)     // 3 floats
ASM_VAR(float, g_cr64c2f8, $L_64c2f8)
ASM_VAR(AsmLabel, s_cr67898c, $L_67898c)
ASM_VAR(AsmLabel, s_cr67897c, $L_67897c)
ASM_VAR(AsmLabel, s_cr67896c, $L_67896c)
ASM_VAR(AsmLabel, s_cr67895c, $L_67895c)
ASM_VAR(AsmLabel, s_cr67894c, $L_67894c)
ASM_VAR(AsmLabel, s_cr67893c, $L_67893c)
ASM_VAR(AsmLabel, s_cr67892c, $L_67892c)
ASM_VAR(AsmLabel, s_cr67891c, $L_67891c)
ASM_VAR(AsmLabel, s_cr67890c, $L_67890c)
ASM_VAR(AsmLabel, s_cr6788fc, $L_6788fc)
ASM_VAR(AsmLabel, s_cr6788f4, $L_6788f4)
ASM_VAR(AsmLabel, s_cr6788e8, $L_6788e8)
extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_4eab58(); KING_ALIAS("_FUN_4eab58", "@History_Construct@24")
extern "C" void FUN_5dbcc0(); KING_ALIAS("_FUN_5dbcc0", "@Camera_Get@12")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")

static void Cr_Identity(BYTE* p)
{
    memcpy(p, g_crIdentity7025f0, 48);
}

static BYTE* Cr_Find(const char* name)
{
    return CallC<BYTE*>(FUN_5ef3d0, name);
}

extern "C" {

// [FUN_531290] constructor of the car object (kind, scene object s, net): matrices, the input histories, the
// camera matrix and its inverse into the globals, the gauges by dashboard style, the sound keys
BYTE* __fastcall Car_Construct(BYTE* self, void*, int kind, BYTE* s, int net)
{
    CallThis(FUN_5d31e0, self, kind, (const char*)s_cr67899c);
    Cr_Identity(self + 52);
    for (int k = 204; k <= 224; k += 4)
        F(int, self, k) = 0;
    CallThis(FUN_4eab58, self + 280, (void*)g_crHist4eaa41, 1.0f, 0, 0);
    CallThis(FUN_4eab58, self + 312, (void*)g_crHist4eaa77, 0.2f, 0, 0);
    CallThis(FUN_4eab58, self + 344, (void*)g_crHist4eaab0, 1.0f, 0, 0);
    CallThis(FUN_4eab58, self + 376, (void*)g_crHist4eaa41, 0.3f, 0, 0);
    CallThis(FUN_4eab58, self + 408, (void*)g_crHist4eaa41, 0.3f, 0, 0);
    F(int, self, 456) = 0;
    F(int, self, 460) = 0;
    F(int, self, 464) = 0;
    static const short k_ident[10] = {472, 520, 568, 1116, 1164, 1212, 1424, 1472, 1520, 1568};
    for (int i = 0; i < 10; i++)
        Cr_Identity(self + k_ident[i]);
    F(int, self, 1620) = 0;
    F(int, self, 1624) = 0;
    F(int, self, 1628) = 0;
    F(void*, self, 0) = g_crVtbl64f2f0;
    F(BYTE*, self, 32) = s;
    F(int, self, 36) = net;
    F(int, self, 1384) = 30;
    F(int, self, 1388) = 30;
    F(int, self, 616) = 0;
    F(int, self, 624) = 0;
    BYTE m[48];
    memcpy(self + 52, CallThis<BYTE*>(FUN_5dbcc0, F(BYTE*, self, 32), m), 48);
    BYTE* p = F(BYTE*, F(BYTE*, self, 32), 488);
    F(int, self, 100) = p ? F(int, p, 76) : 0;
    F(int, self, 104) = -1;
    Cr_Identity(self + 472);
    Cr_Identity(self + 520);
    memcpy(self + 568, self + 520, 48);
    memcpy(g_cr6969d0, self + 52, 48);
    memcpy(g_cr696b98, g_cr6969d0, 48);
    memcpy(g_cr6969a0, CallC<BYTE*>(FUN_5ac2e0, m, self + 52), 48);
    memcpy(g_cr696740, g_cr702650, 12);
    sprintf((char*)g_cr696cbc, s_cr673340);
    g_cr696cb8 = 0;
    F(int, self, 40) = 0;
    F(int, self, 1404) = 254;
    int style = F(int, g_cr6cec70, 408) & 15;
    if (style == 1 || style == 2) {
        bool one = style == 1;
        F(BYTE*, self, 632) = Cr_Find(one ? s_cr67898c : s_cr67893c);
        F(BYTE*, self, 636) = Cr_Find(one ? s_cr67897c : s_cr67892c);
        F(BYTE*, self, 640) = Cr_Find(one ? s_cr67897c : s_cr67892c);
        F(BYTE*, self, 644) = Cr_Find(one ? s_cr67896c : s_cr67891c);
        F(BYTE*, self, 648) = Cr_Find(one ? s_cr67895c : s_cr67890c);
        F(BYTE*, self, 652) = Cr_Find(one ? s_cr67895c : s_cr67890c);
        F(BYTE*, self, 628) = Cr_Find(one ? s_cr67894c : s_cr6788fc);
    } else
        for (int k = 628; k <= 652; k += 4)
            F(int, self, k) = 0;
    F(int, self, 1632) = 0;
    F(int, self, 1064) = 0;
    F(int, self, 1068) = 0;
    for (int k = 1076; k <= 1112; k += 4)
        F(int, self, k) = 0;
    Cr_Identity(self + 1116);
    Cr_Identity(self + 1164);
    F(DWORD, self, 1268) = 1120403456;     // 100.0f
    F(DWORD, self, 1272) = 1082526354;     // 4.2f
    F(DWORD, self, 1260) = 1109393408;     // 40.0f
    F(DWORD, self, 1264) = 1082526354;
    F(int, self, 112) = 1;
    for (int k = 0; k < 7; k++) {
        ((int*)(char*)g_cr6d20a0)[k] = 0;
        ((int*)(char*)g_cr6d20bc)[k] = (int)strlen(((char**)(char*)g_crNames678748)[k]);
    }
    const float* dir = (const float*)(char*)g_crDir702620;
    F(float, self, 456) = -dir[0];
    F(float, self, 460) = -dir[1];
    F(float, self, 464) = -dir[2];
    F(float, self, 468) = (float)(CallC<double>(FUN_4e27a5) - g_cr64c2f8);
    F(BYTE*, self, 440) = CallC<BYTE*>(FUN_5ef2b0, 0, (const char*)s_cr6788f4);
    F(BYTE*, self, 444) = CallC<BYTE*>(FUN_5ef2b0, 0, (const char*)s_cr6788e8);
    F(int, self, 448) = F(int, g_cr696ca0, 2992);
    F(int, self, 452) = 2;
    return self;
}

} // extern "C"
