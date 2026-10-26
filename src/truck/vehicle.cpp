// vehicle.cpp - small functions of the unit at 0x52fa20 of king.exe (the
// code before copter.cpp): the control bindings, sounds at the volume of the
// options, the list of vehicles and the classes of vehicle types, getters,
// setters and saved states of the vehicle, a flashing light, and some
// constructors.

#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "car.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef int Ints[1];

extern "C" void FUN_52fdb0(); KING_ALIAS("_FUN_52fdb0", "@ControlPreset_Default@4")
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

KING_VAR(BYTE*, g_6cec78, 6cec78)             // +386: the sound volume in percent
KING_VAR(int, g_696ca4, 696ca4)
KING_VAR(int, g_6d205c, 6d205c)
KING_VAR(int, g_6d2060, 6d2060)
KING_VAR(Sound*, g_sound6d1d2c, 6d1d2c)
KING_VAR(BYTE*, g_vehicleList, 6ef1f8)        // linked through +4 of the vehicles
KING_VAR(int, g_6ef210, 6ef210)
KING_VAR(int, g_6ef214, 6ef214)
KING_VAR(int, g_6ef218, 6ef218)
KING_VAR(int, g_6ef220, 6ef220)
KING_VAR(int, g_6ef224, 6ef224)
KING_VAR(int, g_6ef228, 6ef228)
KING_VAR(int, g_6ef230, 6ef230)
KING_VAR(int, g_6ef234, 6ef234)
KING_VAR(int, g_6ef238, 6ef238)
KING_VAR(int, g_6ef240, 6ef240)
KING_VAR(int, g_6ef244, 6ef244)
KING_VAR(int, g_6ef248, 6ef248)
KING_VAR(int, g_typeClass1, 6ce334)           // first vehicle type of each class
KING_VAR(int, g_typeClass2, 6ce338)
KING_VAR(int, g_typeClass3, 6ce33c)
KING_VAR(int, g_typeClass4, 6ce340)
KING_VAR(int, g_typeClass5, 6ce344)
KING_VAR(int, g_typeClassEnd, 6ce348)
KING_VAR(int, g_6ef264, 6ef264)
KING_VAR(int, g_6ef268, 6ef268)
KING_VAR(int, g_6ef270, 6ef270)
KING_VAR(BYTE*, g_6ef1fc, 6ef1fc)             // +2968: the time (double)
KING_VAR(Ints, g_flash695fb0, 695fb0)         // 32 steps of a flashing light
KING_VAR(Ints, g_flash695328, 695328)
KING_VAR(Ints, g_flash695e30, 695e30)
KING_VAR(AsmLabel, g_undefinedModule, 66d638) // "UNDEFINED_MODULE"
KING_VAR(AsmLabel, g_undefinedRoom, 66d628)   // "UNDEFINED_ROOM"
KING_VAR(AsmLabel, g_str68d1c0, 68d1c0)
KING_VAR(AsmLabel, g_vtbl64f4bc, 64f4bc)
KING_VAR(AsmLabel, g_vtbl64d7d0, 64d7d0)
KING_VAR(AsmLabel, g_vtbl64c7dc, 64c7dc)

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
extern "C" void FUN_4dde70(); KING_ALIAS("_FUN_4dde70", "@CabView_Setup@12")
KING_VAR(BYTE*, g_list6d20dc, 6d20dc)
KING_VAR(AsmLabel, g_6d2410, 6d2410)          // 1488-byte records by vehicle type
KING_VAR(AsmLabel, g_697f9c, 697f9c)          // 2416-byte records by vehicle type
KING_VAR(void*, g_696cb0, 696cb0)
KING_VAR(int, g_6d1db4, 6d1db4)
KING_VAR(Ints, g_696bf8, 696bf8)
KING_VAR(Ints, g_696c0c, 696c0c)
KING_VAR(Ints, g_696c20, 696c20)
KING_VAR(double, g_64f310, 64f310)
KING_VAR(int, g_minTop672d7c, 672d7c)         // 50
KING_VAR(BYTE*, g_696ca0, 696ca0)
KING_VAR(BYTE*, g_6cec6c, 6cec6c)
KING_VAR(BYTE*, g_game, 6d2078)
KING_VAR(void*, g_68b990, 68b990)
KING_VAR(AsmLabel, g_matrix7025f0, 7025f0)    // 48 bytes: the identity
KING_VAR(float, g_702630, 702630)
KING_VAR(float, g_702634, 702634)
KING_VAR(float, g_702638, 702638)
KING_VAR(Ints, g_zero702650, 702650)          // three floats
KING_VAR(AsmLabel, g_6d24bc, 6d24bc)
KING_VAR(Ints, g_6d24e0, 6d24e0)

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

extern "C" void FUN_4e28b0(); KING_ALIAS("_FUN_4e28b0", "_Vec3_OutOfBox")
extern "C" void FUN_4e2821(); KING_ALIAS("_FUN_4e2821", "_World_CheckPoint")
extern "C" void FUN_4fc930(); KING_ALIAS("_FUN_4fc930", "@Body4fc930_SetMode@28")
extern "C" void FUN_541ad0(); KING_ALIAS("_FUN_541ad0", "@NetVehIn_Get@4")
extern "C" void FUN_551970(); KING_ALIAS("_FUN_551970", "@Car_Effects@8")
extern "C" void FUN_586df0(); KING_ALIAS("_FUN_586df0", "@MovingItem_Frame@4")
ASM_FUNC(FUN_546240, @Vehicle_RecordState@8)
extern "C" void FUN_501b90(); KING_ALIAS("_FUN_501b90", "@Control_Move@20")
extern "C" void __fastcall Body_ClearMode(BYTE* self, void*);              // carphys.cpp
extern "C" void __fastcall Progress_SetStep(BYTE* self, void*, int step);  // movingb.cpp
KING_VAR(AsmLabel, g_strSetVehinMode, 67a2b0)  // "_Vehicle::set_vehin_mode#2"
KING_VAR(float, g_quarterPi674408, 674408)
KING_VAR(double, g_64f4c8, 64f4c8)            // 1/127

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

extern "C" void FUN_4eec80(); KING_ALIAS("_FUN_4eec80", "@Trailer_SaddleLoad@12")    // engine: the angle of a direction (&vec)
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

KING_VAR(int, g_6ef2b8, 6ef2b8)
KING_VAR(int, g_6ef2bc, 6ef2bc)
KING_VAR(int, g_6ef2c0, 6ef2c0)

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
extern "C" void FUN_501630(); KING_ALIAS("_FUN_501630", "@Cab_ReadSettings@12")    // (thiscall on +21600) reads the 35 settings, returns their count
KING_VAR(AsmLabel, s_67a0b0, 67a0b0)
KING_VAR(AsmLabel, s_679fb8, 679fb8)
KING_VAR(AsmLabel, s_67a04c, 67a04c)
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
extern "C" void FUN_5427a0(); KING_ALIAS("_FUN_5427a0", "@Car_Reset@8")
extern "C" void FUN_501b90(); KING_ALIAS("_FUN_501b90", "@Control_Move@20")
extern "C" void FUN_555550(); KING_ALIAS("_FUN_555550", "@Vehicle_LowerStep@12")
KING_VAR(AsmLabel, g_carTypes697914, 697914)
KING_VAR(AsmLabel, g_controls6d1db8, 6d1db8)
KING_VAR(AsmLabel, s_679ef8, 679ef8)
KING_VAR(AsmLabel, s_67a3dc, 67a3dc)

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
extern "C" void FUN_5455d0(); KING_ALIAS("_FUN_5455d0", "@Car_AddMass@16")
extern "C" void FUN_4fc920(); KING_ALIAS("_FUN_4fc920", "@Body_SetMode@12")
extern "C" void FUN_54e180(); KING_ALIAS("_FUN_54e180", "@Car_Step@12")
extern "C" void FUN_54ef20(); KING_ALIAS("_FUN_54ef20", "@Car_54ef20@12")
extern "C" void FUN_54ff80(); KING_ALIAS("_FUN_54ff80", "@Car_AfterPhysics@4")
extern "C" void FUN_581430(); KING_ALIAS("_FUN_581430", "@Item_SetSpin@16")
extern "C" void FUN_5816d0(); KING_ALIAS("_FUN_5816d0", "@MovingItem_SetSpin@16")
extern "C" void FUN_581230(); KING_ALIAS("_FUN_581230", "@SceneObj_SetPosition@16")
extern "C" void FUN_561d50(); KING_ALIAS("_FUN_561d50", "@Copter_SetVec112@12")
extern "C" void FUN_561d10(); KING_ALIAS("_FUN_561d10", "@Copter_SetMatrix@16")
extern "C" void FUN_4f1e20(); KING_ALIAS("_FUN_4f1e20", "@Body_SetVelocity@12")
extern "C" void FUN_4f1240(); KING_ALIAS("_FUN_4f1240", "@Body_PutDown@20")
KING_VAR(Vec3, g_vhZero702650, 702650)

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
extern "C" void FUN_501b90(); KING_ALIAS("_FUN_501b90", "@Control_Move@20")
extern "C" void FUN_4eb6a0(); KING_ALIAS("_FUN_4eb6a0", "@Ramp_SetPosition@12")
extern "C" void FUN_5812a0(); KING_ALIAS("_FUN_5812a0", "@Item_SetVelocity@16")
extern "C" void FUN_561d70(); KING_ALIAS("_FUN_561d70", "@Copter_SetVec100@12")
extern "C" void FUN_4f1dd0(); KING_ALIAS("_FUN_4f1dd0", "@Body_SetPosition@12")
extern "C" void FUN_4dde10(); KING_ALIAS("_FUN_4dde10", "@Cab_Release@8")
extern "C" void FUN_53f750(); KING_ALIAS("_FUN_53f750", "_Car_InList")
extern "C" void FUN_4e9d90(); KING_ALIAS("_FUN_4e9d90", "_WarnLog_Close")
extern "C" void FUN_4e9d36(); KING_ALIAS("_FUN_4e9d36", "_VehLogs_Close")
extern "C" void FUN_4f0fc0(); KING_ALIAS("_FUN_4f0fc0", "@Body_PlaceAt@16")
KING_VAR(AsmLabel, g_vh69809c, 69809c)          // car types, 2416 bytes each
KING_VAR(BYTE*, g_vh696cb0, 696cb0)
KING_VAR(int, g_vh696cb8, 696cb8)
KING_VAR(AsmLabel, g_vh696cbc, 696cbc)
KING_VAR(AsmLabel, s_vh673340, 673340)
KING_VAR(int, g_vh696c70, 696c70)
KING_VAR(int, g_vh672d80, 672d80)
KING_VAR(int, g_vh696c74, 696c74)
KING_VAR(AsmLabel, s_vh68d1c0, 68d1c0)
KING_VAR(AsmLabel, s_vh66c4e4, 66c4e4)
KING_VAR(AsmLabel, s_vh67a298, 67a298)
KING_VAR(AsmLabel, s_vh67a3c4, 67a3c4)

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
extern "C" void FUN_556a70(); KING_ALIAS("_FUN_556a70", "@CarBody_Step@4")
extern "C" void FUN_5561b0(); KING_ALIAS("_FUN_5561b0", "@Car_LoadCargo@16")
extern "C" void FUN_5562e0(); KING_ALIAS("_FUN_5562e0", "@Car_UnloadCargo@16")
extern "C" void FUN_54c950(); KING_ALIAS("_FUN_54c950", "@Car_NamePart@16")
extern "C" void FUN_54c9d0(); KING_ALIAS("_FUN_54c9d0", "@Car_Set54c9d0@16")
extern "C" void FUN_54ca20(); KING_ALIAS("_FUN_54ca20", "@Car_Set54ca20@16")
extern "C" void FUN_547030(); KING_ALIAS("_FUN_547030", "@Car_Draw@16")
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
KING_VAR(float, g_vh64c264, 64c264)    // 0
KING_VAR(float, g_vh64c258, 64c258)

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
extern "C" void FUN_5455d0(); KING_ALIAS("_FUN_5455d0", "@Car_AddMass@16")
KING_VAR(AsmLabel, g_veh6d20f0, 6d20f0)

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

extern "C" void FUN_4eec80(); KING_ALIAS("_FUN_4eec80", "@Trailer_SaddleLoad@12")
extern "C" void FUN_4eeaf0(); KING_ALIAS("_FUN_4eeaf0", "@Body_SetLoad@12")
extern "C" void FUN_581230(); KING_ALIAS("_FUN_581230", "@SceneObj_SetPosition@16")
extern "C" void FUN_561d10(); KING_ALIAS("_FUN_561d10", "@Copter_SetMatrix@16")
extern "C" void FUN_4f1240(); KING_ALIAS("_FUN_4f1240", "@Body_PutDown@20")
extern "C" void FUN_4f0fc0(); KING_ALIAS("_FUN_4f0fc0", "@Body_PlaceAt@16")
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5f9b00(); KING_ALIAS("_FUN_5f9b00", "@SceneNode5f9b00_Hit@16")
KING_VAR(AsmLabel, g_veh702630, 702630)
KING_VAR(AsmLabel, g_veh6d2114, 6d2114)

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
extern "C" void FUN_501b90(); KING_ALIAS("_FUN_501b90", "@Control_Move@20")
extern "C" void FUN_501e30(); KING_ALIAS("_FUN_501e30", "@Wheel_StepLevel@20")
KING_VAR(int, g_veh68b990, 68b990)
KING_VAR(AsmLabel, g_veh6d2510, 6d2510)
KING_VAR(AsmLabel, g_veh6d219c, 6d219c)
KING_VAR(AsmLabel, g_veh6d256c, 6d256c)
KING_VAR(AsmLabel, g_veh69809c, 69809c)

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
extern "C" void FUN_544df0(); KING_ALIAS("_FUN_544df0", "@Vehicle_Destruct@4")
extern "C" void FUN_592bf0(); KING_ALIAS("_FUN_592bf0", "@ListItem592bf0_Construct@12")
extern "C" void FUN_52fd90(); KING_ALIAS("_FUN_52fd90", "@Obj52fd90_Reset@8")
KING_VAR(AsmLabel, g_veh64f96c, 64f96c)
KING_VAR(AsmLabel, g_veh64f880, 64f880)
KING_VAR(AsmLabel, g_veh64f834, 64f834)
KING_VAR(AsmLabel, g_veh64f830, 64f830)
KING_VAR(AsmLabel, g_veh64fb8c, 64fb8c)
KING_VAR(AsmLabel, g_veh64faa0, 64faa0)
KING_VAR(AsmLabel, g_veh64fa54, 64fa54)
KING_VAR(AsmLabel, g_veh64fa50, 64fa50)
KING_VAR(AsmLabel, g_veh6d20dc, 6d20dc)
KING_VAR(AsmLabel, g_veh64f2ec, 64f2ec)
KING_VAR(AsmLabel, g_veh64f2e8, 64f2e8)

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
KING_VAR(AsmLabel, s_veh6792e8, 6792e8)
KING_VAR(AsmLabel, g_veh64f428, 64f428)
KING_VAR(AsmLabel, g_veh64f2f0, 64f2f0)

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
KING_VAR(short, g_veh67d10a, 67d10a)
KING_VAR(BYTE*, g_veh6ef1fc, 6ef1fc)

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
KING_VAR(short, g_veh672d84, 672d84)
KING_VAR(AsmLabel, s_veh672bd0, 672bd0)

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

extern "C" void FUN_544df0(); KING_ALIAS("_FUN_544df0", "@Vehicle_Destruct@4")
// [$L_543de0]
extern "C" BYTE* __fastcall Dtor543de0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_544df0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}
// [$L_5574f0] [$L_557500] the same deleting destructor reached through the
// object's second and third base (this adjusted by -272 and -392)
extern "C" BYTE* __fastcall Dtor543de0_Adjust272(BYTE* s, void* d, int flags) { return Dtor543de0_DeletingDestructor(s - 272, d, flags); }
extern "C" BYTE* __fastcall Dtor543de0_Adjust392(BYTE* s, void* d, int flags) { return Dtor543de0_DeletingDestructor(s - 392, d, flags); }

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
KING_VAR(int, g_q47_6ef200, 6ef200)
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
ASM_FUNC(FUN_4e24e8, _World_InitTables)
extern "C" void FUN_4de290(); KING_ALIAS("_FUN_4de290", "@Cab_Setup@12")
extern "C" void FUN_53fdb0(); KING_ALIAS("_FUN_53fdb0", "_CarTypes_FindParts")
KING_VAR(int, g_q48_6ef1fc, 6ef1fc)
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
extern "C" void FUN_552940(); KING_ALIAS("_FUN_552940", "@Car_LampShadows@8")
KING_VAR(BYTE*, g_q49_6d2078, 6d2078)
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
KING_VAR(void*, g_q54_6ef250, 6ef250)
KING_VAR(AsmLabel, s_q54_679f8c, 679f8c)
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

extern "C" void FUN_545ef0(); KING_ALIAS("_FUN_545ef0", "@Car_PlaceByRecord@12")
extern "C" void FUN_546300(); KING_ALIAS("_FUN_546300", "@Car_Restore546300@12")
extern "C" void FUN_546480(); KING_ALIAS("_FUN_546480", "@Car_Restore546480@12")
extern "C" void FUN_545d90(); KING_ALIAS("_FUN_545d90", "@Car_Command@12")
extern "C" void FUN_545dc0(); KING_ALIAS("_FUN_545dc0", "@Car_SetBytes@12")
extern "C" int __fastcall SceneObj_OnMessage(BYTE* s, void*, int* m);    // moving_items.cpp
KING_VAR(AsmLabel, s_q57_679ff0, 679ff0)
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
extern "C" void FUN_4f1240(); KING_ALIAS("_FUN_4f1240", "@Body_PutDown@20")
KING_VAR(float, g_q112_64c250, 64c250)
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

extern "C" void $L_549580(); KING_ALIAS("_$L_549580", "_Car_FlasherKick")
extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")
extern "C" void FUN_4ba060(); KING_ALIAS("_FUN_4ba060", "_Effect_New4daf40")
extern "C" void FUN_54a310(); KING_ALIAS("_FUN_54a310", "@Flasher_Release@8")
KING_VAR(float, g_q129_64c938, 64c938)
KING_VAR(double, g_q129_64b840, 64b840)
KING_VAR(AsmLabel, g_q129_6981b0, 69818e)
KING_VAR(BYTE*, g_q129_69622c, 69622c)
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

KING_VAR(float, g_q158_64c264, 64c264)
KING_VAR(AsmLabel, g_q158_69809c, 69809c)
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
KING_VAR(double, g_q169_64b840, 64b840)
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
KING_VAR(BYTE*, g_q170_6d2078, 6d2078)
KING_VAR(int, g_q170_6ef260, 6ef260)
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

extern "C" void FUN_556780(); KING_ALIAS("_FUN_556780", "@Limit_Share@16")
extern "C" void FUN_556890(); KING_ALIAS("_FUN_556890", "@Tanks_Fill@16")
extern "C" void FUN_5455d0(); KING_ALIAS("_FUN_5455d0", "@Car_AddMass@16")
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

KING_VAR(double, g_q185_64d318, 64d318)
KING_VAR(double, g_q185_64b898, 64b898)
KING_VAR(double, g_q185_64ca78, 64ca78)
KING_VAR(float, g_q185_64c320, 64c320)
KING_VAR(float, g_q185_64c32c, 64c32c)
KING_VAR(float, g_q185_64c938, 64c938)
KING_VAR(float, g_q185_64f744, 64f744)
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

KING_VAR(float, g_q199_64c938, 64c938)
KING_VAR(float, g_q199_64c264, 64c264)
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
extern "C" void FUN_57e140(); KING_ALIAS("_FUN_57e140", "@Object_QueryShape@12")
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

extern "C" void FUN_4de990(); KING_ALIAS("_FUN_4de990", "_Cab_EyeFrame")
extern "C" void FUN_5de950(); KING_ALIAS("_FUN_5de950", "_Matrix_ToAngles")
extern "C" void FUN_5da950(); KING_ALIAS("_FUN_5da950", "_Matrix_FromPose")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5d85f0(); KING_ALIAS("_FUN_5d85f0", "@Win_GetRect@12")
KING_VAR(int, g_q234_6cec68, 6cec68)
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

extern "C" void FUN_56ffd0(); KING_ALIAS("_FUN_56ffd0", "_Game_PlayerMessage")
extern "C" void FUN_43f620(); KING_ALIAS("_FUN_43f620", "_Player_Flags")
extern "C" void FUN_56ae60(); KING_ALIAS("_FUN_56ae60", "_Messages_Add")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_532cf0(); KING_ALIAS("_FUN_532cf0", "_Points_Nearest")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
KING_VAR(BYTE*, g_q235_6cec6c, 6cec6c)
KING_VAR(AsmLabel, g_q235_6cf3a8, 6cf3a8)
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

extern "C" void FUN_557090(); KING_ALIAS("_FUN_557090", "@Car_WorkingOrder@4")
extern "C" void FUN_4ca7e0(); KING_ALIAS("_FUN_4ca7e0", "_Fx_CarShadows")
extern "C" void FUN_5f9d20(); KING_ALIAS("_FUN_5f9d20", "@SceneNode5f9d20_Draw@24")
KING_VAR(AsmLabel, g_q236_6d217c, 6d217c)
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

KING_VAR(AsmLabel, s_cr67899c, 67899c)
KING_VAR(AsmLabel, g_crIdentity7025f0, 7025f0)  // a 4x3 identity matrix
KING_VAR(AsmLabel, g_crVtbl64f2f0, 64f2f0)
extern "C" double __cdecl Window_Ramp(float x);
extern "C" double __cdecl Window_Parabola(float x);
extern "C" double __cdecl Window_Root(float x);
KING_VAR(AsmLabel, g_cr6969d0, 6969d0)
KING_VAR(AsmLabel, g_cr696b98, 696b98)
KING_VAR(AsmLabel, g_cr6969a0, 6969a0)
KING_VAR(AsmLabel, g_cr696740, 696740)
KING_VAR(AsmLabel, g_cr702650, 702650)
KING_VAR(AsmLabel, g_cr696cbc, 696cbc)
KING_VAR(int, g_cr696cb8, 696cb8)
KING_VAR(AsmLabel, s_cr673340, 673340)
KING_VAR(BYTE*, g_cr6cec70, 6cec70)
KING_VAR(BYTE*, g_cr696ca0, 696ca0)
KING_VAR(AsmLabel, g_crNames678748, 678748)  // 7 char*
KING_VAR(AsmLabel, g_cr6d20a0, 6d20a0)
KING_VAR(AsmLabel, g_cr6d20bc, 6d20bc)
KING_VAR(AsmLabel, g_crDir702620, 702620)     // 3 floats
KING_VAR(float, g_cr64c2f8, 64c2f8)
KING_VAR(AsmLabel, s_cr67898c, 67898c)
KING_VAR(AsmLabel, s_cr67897c, 67897c)
KING_VAR(AsmLabel, s_cr67896c, 67896c)
KING_VAR(AsmLabel, s_cr67895c, 67895c)
KING_VAR(AsmLabel, s_cr67894c, 67894c)
KING_VAR(AsmLabel, s_cr67893c, 67893c)
KING_VAR(AsmLabel, s_cr67892c, 67892c)
KING_VAR(AsmLabel, s_cr67891c, 67891c)
KING_VAR(AsmLabel, s_cr67890c, 67890c)
KING_VAR(AsmLabel, s_cr6788fc, 6788fc)
KING_VAR(AsmLabel, s_cr6788f4, 6788f4)
KING_VAR(AsmLabel, s_cr6788e8, 6788e8)
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
    CallThis(FUN_4eab58, self + 280, (void*)Window_Ramp, 1.0f, 0, 0);
    CallThis(FUN_4eab58, self + 312, (void*)Window_Parabola, 0.2f, 0, 0);
    CallThis(FUN_4eab58, self + 344, (void*)Window_Root, 1.0f, 0, 0);
    CallThis(FUN_4eab58, self + 376, (void*)Window_Ramp, 0.3f, 0, 0);
    CallThis(FUN_4eab58, self + 408, (void*)Window_Ramp, 0.3f, 0, 0);
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

// === the car's dashboard setup (FUN_532430)

KING_VAR(AsmLabel, g_cdEmpty68d1c0, 68d1c0)
KING_VAR(AsmLabel, g_cd702650, 702650)
KING_VAR(AsmLabel, g_cd702640, 702640)
KING_VAR(BYTE*, g_cdShell6cec6c, 6cec6c)
KING_VAR(AsmLabel, g_cd6cf428, 6cf428)
KING_VAR(AsmLabel, g_cd6cf3b0, 6cf3b0)
KING_VAR(AsmLabel, g_cd6cf4ac, 6cf4ac)
KING_VAR(AsmLabel, g_cd6cf434, 6cf434)
KING_VAR(AsmLabel, g_cd6cf4b8, 6cf4b8)
KING_VAR(int, g_cd6cf3ac, 6cf3ac)
KING_VAR(int, g_cd6cf3a8, 6cf3a8)
KING_VAR(int, g_cd6d20e0, 6d20e0)
KING_VAR(int, g_cd6d20e4, 6d20e4)
KING_VAR(float, g_cd6d20e8, 6d20e8)
KING_VAR(AsmLabel, s_cd678acc, 678acc)
KING_VAR(AsmLabel, s_cd678ac8, 678ac8)
KING_VAR(AsmLabel, s_cd67305c, 67305c)
KING_VAR(AsmLabel, s_cd678ab4, 678ab4)
KING_VAR(AsmLabel, s_cd678aa0, 678aa0)
KING_VAR(AsmLabel, s_cd678a90, 678a90)
KING_VAR(AsmLabel, s_cd678a80, 678a80)
KING_VAR(AsmLabel, s_cd678a70, 678a70)
KING_VAR(AsmLabel, s_cd678a60, 678a60)
KING_VAR(AsmLabel, s_cd678a50, 678a50)
KING_VAR(AsmLabel, s_cd678a40, 678a40)
KING_VAR(AsmLabel, s_cd678a30, 678a30)
KING_VAR(AsmLabel, s_cd678a24, 678a24)
KING_VAR(AsmLabel, s_cd678a14, 678a14)
KING_VAR(AsmLabel, s_cd678a0c, 678a0c)
extern "C" void FUN_5e8c60(); KING_ALIAS("_FUN_5e8c60", "_Font_LoadFile")
extern "C" void FUN_532330(); KING_ALIAS("_FUN_532330", "@View_Place@8")
extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")

static BYTE* Cd_Find(const char* fmt, const char* prefix, int i)
{
    char name[40];
    sprintf(name, fmt, prefix, i);
    return CallC<BYTE*>(FUN_5ef3d0, name);
}

extern "C" {

// [FUN_532430] sets up the car (thiscall): its matrices, the dashboard pictures by dashboard style (+904
// numbered, +660.. gauges, +740.. lights), the light direction in vehicle space, the font, the view
void __fastcall Car_SetupDashboard(BYTE* self, void*)
{
    Cr_Identity(self + 1424);
    Cr_Identity(self + 1568);
    Cr_Identity(self + 1520);
    F(int, self, 1616) = 0;
    memcpy(self + 1620, g_cd702650, 12);
    F(DWORD, self, 44) = 1232348160;           // 1e6f
    F(int, self, 1392) = 0;
    F(int, self, 1060) = 19;
    char prefix[20];
    strcpy(prefix, g_cdEmpty68d1c0);
    if ((F(DWORD, g_cr6cec70, 408) & 15) == 1)
        strcpy(prefix, s_cd678acc);
    if ((F(DWORD, g_cr6cec70, 408) & 15) == 2)
        strcpy(prefix, s_cd678ac8);
    char name[40];
    for (int i = 1; i <= 39; i++) {
        sprintf(name, s_cd67305c);
        if ((F(DWORD, g_cr6cec70, 408) & 15) == 1)
            sprintf(name, s_cd678ab4, i);
        if ((F(DWORD, g_cr6cec70, 408) & 15) == 2)
            sprintf(name, s_cd678aa0, i);
        F(BYTE*, self, 900 + i * 4) = Cr_Find(name);
    }
    sprintf(name, s_cd67305c);
    F(int, self, 1396) = 0;
    F(int, self, 108) = 0;
    for (int i = 0; i < 10; i++) {
        F(BYTE*, self, 700 + i * 4) = Cd_Find(s_cd678a90, prefix, i);
        F(BYTE*, self, 660 + i * 4) = Cd_Find(s_cd678a80, prefix, i);
        F(BYTE*, self, 856 + i * 4) = Cd_Find(s_cd678a70, prefix, i);
        F(BYTE*, self, 816 + i * 4) = Cd_Find(s_cd678a60, prefix, i);
    }
    F(BYTE*, self, 812) = Cd_Find(s_cd678a50, prefix, 0);
    for (int i = 0; i < 18; i++)
        F(BYTE*, self, 740 + i * 4) = Cd_Find(s_cd678a40, prefix, i);
    F(BYTE*, self, 896) = Cd_Find(s_cd678a30, prefix, 0);
    F(BYTE*, self, 900) = Cd_Find(s_cd678a24, prefix, 0);
    F(BYTE*, self, 656) = Cd_Find(s_cd678a14, prefix, 0);
    F(int, self, 1376) = 0;
    F(int, self, 1380) = 0;
    F(int, self, 1400) = F(int, g_cdShell6cec6c, 1352);
    memcpy(g_cd6cf428, self + 88, 12);
    memcpy(g_cd6cf3b0, self + 88, 12);
    const float* d = (const float*)(char*)g_crDir702620;
    float v[3];
    for (int k = 0; k < 3; k++)
        v[k] = (float)((double)d[2] * F(float, self, 76 + k * 4) + (double)d[1] * F(float, self, 64 + k * 4)
                       + (double)d[0] * F(float, self, 52 + k * 4));
    memcpy(g_cd6cf4ac, v, 12);
    memcpy(g_cd6cf434, v, 12);
    strcpy((char*)g_cd6cf4b8, g_cdEmpty68d1c0);
    g_cd6cf3ac = 1;
    g_cd6cf3a8 = 0;
    if (F(BYTE, self, 1404) & 64)
        F(BYTE*, self, 40) = CallC<BYTE*>(FUN_5e8c60, (const char*)s_cd678a0c);
    if (!F(BYTE*, self, 40))
        F(BYTE*, self, 40) = F(BYTE*, F(BYTE*, self, 32), 2864);
    CallThis(FUN_532330, self);
    g_cd6d20e0 = 0;
    F(int, self, 112) = 1;
    memcpy(self + 216, g_cd702640, 12);
    memcpy(self + 204, self + 88, 12);
    strcpy((char*)(self + 228), F(const char*, F(BYTE*, self, 100), 12));
    g_cd6d20e4 = 0;
    g_cd6d20e8 = (float)F(double, F(BYTE*, self, 32), 2968);
    CallThis(FUN_5d33d0, self, 1);
}

} // extern "C"

// === the network vehicle constructor (FUN_544920)

KING_VAR(AsmLabel, g_nvTypes6d20f0, 6d20f0)     // 1488 bytes per vehicle type
KING_VAR(AsmLabel, g_nvTypes697b30, 697b30)     // 1488 bytes per vehicle type
KING_VAR(AsmLabel, g_nv6ef1f8, 6ef1f8)
KING_VAR(AsmLabel, g_nvVtbl64f654, 64f654)
KING_VAR(AsmLabel, g_nvVtbl64f568, 64f568)
KING_VAR(AsmLabel, g_nvVtbl64f51c, 64f51c)
KING_VAR(AsmLabel, g_nvVtbl64f518, 64f518)
extern "C" void FUN_541940(); KING_ALIAS("_FUN_541940", "@Obj541940_Construct@12")
extern "C" void FUN_58b720(); KING_ALIAS("_FUN_58b720", "@Body58b720_Construct@52")
extern "C" void FUN_556670(); KING_ALIAS("_FUN_556670", "@Obj556670_Init@12")
extern "C" void FUN_541980(); KING_ALIAS("_FUN_541980", "@Wheel541980_Construct@8")
extern "C" void FUN_542440(); KING_ALIAS("_FUN_542440", "@Obj542440_Construct@8")
extern "C" void FUN_53fa60(); KING_ALIAS("_FUN_53fa60", "@Body53fa60_Construct@8")
extern "C" void FUN_548e70(); KING_ALIAS("_FUN_548e70", "@Obj548e70_Construct@8")
extern "C" void FUN_4eb460(); KING_ALIAS("_FUN_4eb460", "@Ramp_Construct@8")
extern "C" void FUN_549890(); KING_ALIAS("_FUN_549890", "@Obj549890_Construct@8")
extern "C" void FUN_542740(); KING_ALIAS("_FUN_542740", "_CarType_Class")
extern "C" void FUN_5427a0(); KING_ALIAS("_FUN_5427a0", "@Car_Reset@8")
extern "C" void FUN_5c3070(); KING_ALIAS("_FUN_5c3070", "@Table_Add6@20")

static void Nv_Identities(BYTE* p, int n)
{
    for (int i = 0; i < n; i++)
        Cr_Identity(p + i * 48);
}

extern "C" {

// [FUN_544920] constructor of a vehicle of type (view, type, scene object, owner, placement matrix, a6, a7, setup
// table or 0 for the type's own): the body, its matrices and wheels, ramps and parts, registered in view +7976
BYTE* __fastcall NetVehicle_Construct(BYTE* self, void*, BYTE* view, int type, int scene, int owner,
                                      const BYTE* place, int a6, int a7, BYTE* setup)
{
    CallThis(FUN_541940, self, (void*)g_nv6ef1f8);
    BYTE* tab = setup ? setup : F(BYTE*, g_nvTypes6d20f0, type * 1488);
    const int* t20 = (const int*)(tab + 20);
    BYTE* t = (BYTE*)g_nvTypes697b30 + type * 1488;
    CallThis(FUN_58b720, self + 16, view, scene, tab, owner, place, t20[3], 100, F(int, t, 152), F(int, t, 156),
             a6, a7);
    CallThis(FUN_556670, self + 9556, type);
    Nv_Identities(self + 9652, 8);
    Cr_Identity(self + 10036);
    CallThis(FUN_541980, self + 10084);
    CallThis(FUN_542440, self + 10432);
    memset(self + 10616, 0, 192);
    F(int, self, 10812) = 0;
    F(int, self, 10816) = 0;
    F(int, self, 10820) = 0;
    memset(self + 11540, 0, 1200);
    Nv_Identities(self + 12740, 50);
    Nv_Identities(self + 15140, 50);
    memset(self + 17540, 0, 600);
    Cr_Identity(self + 20212);
    memset(self + 20332, 0, 48);
    F(int, self, 20548) = 0;
    F(int, self, 20552) = 0;
    F(int, self, 20556) = 0;
    memset(self + 20560, 0, 36);
    CallThis(FUN_53fa60, self + 20636);
    CallThis(FUN_548e70, self + 20960);
    F(int, self, 21040) = 0;
    F(int, self, 21044) = 0;
    F(int, self, 21048) = 0;
    for (int i = 0; i < 8; i++)
        CallThis(FUN_4eb460, self + 21152 + i * 52);
    CallThis(FUN_549890, self + 21568);
    memset(self + 21652, 0, 132);
    F(int, self, 21876) = 0;
    F(int, self, 21872) = 0;
    F(int, self, 20808) = scene;
    memcpy(self + 20640, place, 48);
    F(void*, self, 0) = g_nvVtbl64f654;
    F(void*, self, 16) = g_nvVtbl64f568;
    F(void*, self, 272) = g_nvVtbl64f51c;
    F(void*, self, 392) = g_nvVtbl64f518;
    F(int, self, 10420) = type;
    F(int, self, 21600) = 0;
    F(int, self, 21604) = 0;
    F(int, self, 9548) = CallC<int>(FUN_542740, type);
    if (setup)
        F(int, self, 9548) = 5;
    CallThis(FUN_5427a0, self);
    F(int, self, 10096) = F(int, self, 384);
    F(int, self, 384)++;
    int r = CallThis<int>(FUN_5c3070, view + 7976, F(int, self, 10096), self + 10104, F(int, self, 10092));
    F(int, self, 3484 + F(int, self, 3520) * 4) = r;
    F(int, self, 3520)++;
    F(int, self, 10924) = 0;
    return self;
}

} // extern "C"

// === the vehicle's destructor (FUN_544df0)

KING_VAR(AsmLabel, g_vd64f654, 64f654)
KING_VAR(AsmLabel, g_vd64f51c, 64f51c)
KING_VAR(AsmLabel, g_vd64f518, 64f518)
KING_VAR(AsmLabel, g_vd64f568, 64f568)
KING_VAR(BYTE*, g_vd6ef1fc, 6ef1fc)
KING_VAR(int, g_vd6ef1f0, 6ef1f0)
KING_VAR(BYTE*, g_vd6ef204, 6ef204)
KING_VAR(AsmLabel, g_vd6d24e0, 6d24e0)
KING_VAR(AsmLabel, g_vd6d24bc, 6d24bc)
KING_VAR(char, s_vd679f54, 679f54)
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_4eac8e(); KING_ALIAS("_FUN_4eac8e", "@Arrays_Free@8")
extern "C" void FUN_53f750(); KING_ALIAS("_FUN_53f750", "_Car_InList")
extern "C" void FUN_57def0(); KING_ALIAS("_FUN_57def0", "@SceneObj_Get800@8")
extern "C" void FUN_58b870(); KING_ALIAS("_FUN_58b870", "@Body58b870_Destroy@8")
extern "C" void FUN_592cf0(); KING_ALIAS("_FUN_592cf0", "@Linked592cf0_Destruct@8")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5e52b0(); KING_ALIAS("_FUN_5e52b0", "@Resources_SetData@24")

typedef void (__cdecl* Vd_Callback)(int);

// a record (+12 buffer) owned by a pointer
static void Vd_FreeRec(BYTE* r)
{
    if (!r)
        return;
    if (F(void*, r, 12))
        operator delete(F(void*, r, 12));
    operator delete(r);
}

extern "C" {

// [FUN_544df0] destructor of vehicle s: the callback +20956; a network car (+9548 = 5) leaves its state (model,
// trailer, position, matrix, names) in the world's resource table and drops its trailer (+21604); others drop
// their cab (+21600) and the use count of their model; then the two records +21872 / +21876 and the bases
void __fastcall Vehicle_Destruct(BYTE* s)
{
    F(void*, s, 0) = g_vd64f654;
    F(void*, s, 272) = g_vd64f51c;
    F(void*, s, 392) = g_vd64f518;
    F(void*, s, 16) = g_vd64f568;
    if (Vd_Callback cb = F(Vd_Callback, s, 20956))
        cb(0);
    if (F(int, s, 9548) == 5) {
        int kind = 3, other = 0;
        DWORD speed = 1142292480;
        if (BYTE* t = F(BYTE*, s, 21604)) {
            kind = F(int, t, 3236);
            BYTE* c = CallC<BYTE*>(FUN_53f750, F(int, t, 3248));
            speed = F(DWORD, F(BYTE*, s, 21604), 3268);
            if (c && !F(int, c, 1220))
                other = F(int, c, 9544);
        }
        int obj = F(int, F(BYTE*, s, 336), 12);
        int name = F(int, CallThis<BYTE*>(FUN_57def0, s + 16), 12);
        BYTE r[160];
        F(int, r, 0) = F(int, s, 10420);
        F(int, r, 4) = kind;
        F(int, r, 8) = other;
        F(int, r, 12) = F(int, s, 388);
        F(int, r, 16) = F(int, s, 380);
        for (int k = 0; k < 4; k++)
            CallThis(FUN_4bd4d0, r + 44 + k * 12);
        memcpy(r + 20, s + 432, 12);
        memcpy(r + 32, s + 444, 12);
        memcpy(r + 44, s + 516, 48);
        F(DWORD, r, 92) = speed;
        strncpy((char*)r + 96, (const char*)name, 31);
        strncpy((char*)r + 128, (const char*)obj, 31);
        CallThis(FUN_5e52b0, g_vd6ef1fc + 704, name + 300000, F(int, F(BYTE*, s, 308), 60), 160, r);
        if (BYTE* t = F(BYTE*, s, 21604)) {
            CallThis(FUN_5e48f0, t);
            operator delete(t);
        }
        F(BYTE*, s, 21604) = 0;
        if (--g_vd6ef1f0 == 0)
            CallVirt(g_vd6ef204, 36);
    } else {
        if (BYTE* c = F(BYTE*, s, 21600)) {
            CallThis(FUN_4eac8e, c + 6992);
            CallThis(FUN_4eac8e, c + 6960);
            operator delete(c);
        }
        F(BYTE*, s, 21600) = 0;
        int at = F(int, s, 10420) * 1488 + F(int, s, 20396) * 4;
        int* n = (int*)((BYTE*)g_vd6d24e0 + at);
        --*n;
        if (*n == 0)
            if (BYTE* o = F(BYTE*, (BYTE*)g_vd6d24bc + 12, at))
                CallVirt(o, 36);
        if (*n < 0) {
            char text[64];
            sprintf(text, &s_vd679f54, F(int, s, 10420), *n);
        }
    }
    Vd_FreeRec(F(BYTE*, s, 21872));
    Vd_FreeRec(F(BYTE*, s, 21876));
    CallThis(FUN_58b870, s ? s + 16 : (BYTE*)0);
    CallThis(FUN_592cf0, s);
}

} // extern "C"

// === packing a vehicle's state for the network (FUN_541e80)

KING_VAR(float, g_pk64d680, 64d680)
KING_VAR(float, g_pk64f508, 64f508)
KING_VAR(double, g_pk64f500, 64f500)
KING_VAR(double, g_pk64b780, 64b780)
KING_VAR(double, g_pk64f4f8, 64f4f8)
KING_VAR(double, g_pk64f4f0, 64f4f0)
KING_VAR(double, g_pk64f4e8, 64f4e8)
KING_VAR(float, g_pk64c268, 64c268)
KING_VAR(double, g_pk64eba0, 64eba0)
KING_VAR(char, s_pk679e6c, 679e6c)
KING_VAR(char, s_pk679e5c, 679e5c)
KING_VAR(char, s_pk679e34, 679e34)
KING_VAR(char, s_pk679dfc, 679dfc)
KING_VAR(char, s_pk679dc0, 679dc0)
extern "C" void FUN_4e260c(); KING_ALIAS("_FUN_4e260c", "_World_Fatal")
extern "C" void FUN_4e41d9(); KING_ALIAS("_FUN_4e41d9", "_Angle_F2Pi")

// the float with bits u
static float Pk_F(DWORD u)
{
    float f;
    memcpy(&f, &u, 4);
    return f;
}

// a byte from v clamped to +-lim and scaled
static BYTE Pk_Byte(float v, float lim, double scale) { return (BYTE)(int)(Clamp542250(v, lim) * scale); }

// an angle checked (where), clamped to +-lim
static double Pk_Angle(float a, const char* where, float lim)
{
    float t = (float)CallC<double>(FUN_4e41d9, a, where);
    return Clamp542250(t, lim);
}

extern "C" {

// [FUN_541e80] packs vehicle state p (thiscall) into its byte buffer (+20): header words, flags +80 with the gear
// +84, the kind and steering byte, per wheel its id, angle (10 bits) and load (6 bits), the contact bits, four
// body angles and four pairs of suspension values, the light bits; the player's car (+80 bit 1) adds speeds,
// controls and flags. The size must match +4 (or +8 / at most 58), else the world stops with an error
void __fastcall Vehicle_PackState(BYTE* p)
{
    BYTE* b = p + 20;
    F(WORD, p, 20) = F(WORD, p, 8);
    F(WORD, p, 22) = F(WORD, p, 0);
    if (F(BYTE, p, 84) > 7)
        F(BYTE, p, 84) = 7;
    F(DWORD, p, 80) |= (DWORD)F(BYTE, p, 84) << 7;
    F(DWORD, p, 24) = F(DWORD, p, 80);
    unsigned kind = F(unsigned, p, 92);
    BYTE k = kind <= 4 ? (BYTE)kind : 0;
    F(BYTE, p, 28) = (BYTE)(k - ((BYTE)(int)(F(float, p, 88) * (double)g_pk64d680) << 3));
    F(BYTE, p, 29) = F(BYTE, p, 96);
    int at = 10;
    for (int i = 0; i < F(int, p, 16); i++) {
        b[at++] = F(BYTE, p, 97 + i);
        double a = Pk_Angle(F(float, p, 108 + i * 4), &s_pk679e6c, Pk_F(1078530011));
        int v = (int)(a * g_pk64f508 * g_pk64f500) & 1023;
        b[at++] = (BYTE)v;
        b[at] = (BYTE)(v >> 8);
        int w = (int)(Clamp542250(F(float, p, 140 + i * 4), Pk_F(1060320051)) * g_pk64b780);
        b[at] |= (BYTE)(w << 2);
        at++;
    }
    b[at++] = (BYTE)((((F(BYTE, p, 193) << 1 | F(BYTE, p, 192)) << 1 | F(BYTE, p, 191)) << 1) | F(BYTE, p, 190));
    for (int i = 0; i < 4; i++) {
        float d = F(float, p, 196) - F(float, p, 208 + i * 12);
        b[at++] = (BYTE)(int)(Pk_Angle(d, &s_pk679e5c, Pk_F(1078530000)) * g_pk64f4f8);
    }
    for (int i = 0; i < 4; i++) {
        b[at++] = Pk_Byte(F(float, p, 248 + i * 12), 1.0f, g_pk64f4f0);
        b[at++] = Pk_Byte(F(float, p, 252 + i * 12), 1.0f, g_pk64f4f0);
    }
    b[at++] = (BYTE)((F(BYTE, p, 188) << 4) + F(BYTE, p, 189));
    char text[100];
    if (!(F(BYTE, p, 80) & 1)) {
        F(WORD, p, 20) = (WORD)at;
        if (at != F(int, p, 4)) {
            sprintf(text, &s_pk679e34, at, F(int, p, 4));
            CallC(FUN_4e260c, text);
        }
        return;
    }
    b[at++] = Pk_Byte(F(float, p, 176), 100.0f, g_pk64f4e8);
    F(WORD, b, at) = (WORD)(int)(F(float, p, 172) * (double)g_pk64c268);
    at += 2;
    b[at++] = Pk_Byte(F(float, p, 180), 100.0f, g_pk64f4e8);
    b[at++] = Pk_Byte(F(float, p, 184), 1.0f, g_pk64eba0);
    b[at++] = (BYTE)(int)(F(float, p, 296) * g_pk64eba0);
    if (F(int, p, 308) < 0 || F(int, p, 308) > 7)
        CallC(FUN_4e260c, &s_pk679dfc);
    BYTE f = (BYTE)(((F(BYTE, p, 316) << 4) | F(BYTE, p, 308)) << 2);
    if (F(int, p, 312))
        f |= 32;
    if (F(int, p, 304))
        f |= 2;
    if (F(int, p, 300))
        f |= 1;
    b[at++] = f;
    b[at++] = F(int, p, 320) != 0;
    b[at++] = Pk_Byte(F(float, p, 324), 1.0f, g_pk64eba0);
    b[at++] = Pk_Byte(F(float, p, 328), 1.0f, g_pk64eba0);
    if (at > 58 || at != F(int, p, 8)) {
        sprintf(text, &s_pk679dc0, at, 58, F(int, p, 8));
        CallC(FUN_4e260c, text);
    }
}

} // extern "C"

// === stepping a vehicle's lowering (FUN_555550)

KING_VAR(float, g_lw64c250, 64c250)
KING_VAR(float, g_lw64c264, 64c264)
KING_VAR(double, g_lw64b840, 64b840)
KING_VAR(AsmLabel, g_lw69809c, 69809c)
extern "C" void FUN_4eb6e0(); KING_ALIAS("_FUN_4eb6e0", "@Ramp_Down@12")
extern "C" void FUN_4eb720(); KING_ALIAS("_FUN_4eb720", "@Ramp_FromTop@12")
extern "C" void FUN_501b90(); KING_ALIAS("_FUN_501b90", "@Control_Move@20")
extern "C" void FUN_5494c0(); KING_ALIAS("_FUN_5494c0", "@CarEffect_End@12")
extern "C" void FUN_549bf0(); KING_ALIAS("_FUN_549bf0", "@Obj549bf0_Get@8")
extern "C" void FUN_555500(); KING_ALIAS("_FUN_555500", "@Car_555500@8")
extern "C" void FUN_555f30(); KING_ALIAS("_FUN_555f30", "@Car_SetLights@12")
extern "C" void FUN_589490(); KING_ALIAS("_FUN_589490", "@BodyLevel_Step@16")

// the cab part k moved by x towards v
static void Lw_Cab(BYTE* car, int k, float* x, int v) { CallThis(FUN_501b90, F(BYTE*, car, 21600), k, x, v); }

// the body level at +off of +9536 stepped by x towards v
static void Lw_Body(BYTE* car, int off, float* x, int v) { CallThis(FUN_589490, F(BYTE*, car, 9536) + off, x, v); }

// the wheel links from i on (3 parts each, from slot k), up to the count of the car's model and below limit
static int Lw_Links(BYTE* car, int i, float* x, int limit)
{
    int n = F(int, (BYTE*)g_lw69809c + F(int, car, 10420) * 2416, 44);
    for (int k = i + 11; i < n && k < limit; i++, k++) {
        Lw_Cab(car, k - 8, x, F(int, car, 10480 + i * 4));
        Lw_Cab(car, k, x, F(int, car, 10512 + i * 4));
        Lw_Cab(car, k + 8, x, F(int, car, 10544 + i * 4));
    }
    return i;
}

extern "C" {

// [FUN_555550] steps the lowering of vehicle car by x (thiscall): above 64c250 it restarts at 1; below 64c264
// only the lights and the virtual +32 step; else the effects end, the cab parts, ramps (+16060), body levels
// (+9536) and wheel links move; 0 when x reaches 64c264, else 1
int __fastcall Vehicle_LowerStep(BYTE* car, void*, float x)
{
    if (x > g_lw64c250)
        x = 1.0f;
    else if (x < g_lw64c264) {
        if (F(BYTE, car, 32) & 1) {
            CallThis(FUN_555f30, car, &x);
            CallVirt(car, 32, &x);
        } else {
            CallVirt(car, 32, &x);
            CallThis(FUN_555f30, car, &x);
        }
        return x == g_lw64c264 ? 0 : 1;
    }
    int st = CallThis<int>(FUN_549bf0, car + 21568);
    if (st == 1 || CallThis<int>(FUN_549bf0, car + 21568) == 2)
        CallThis(FUN_5494c0, car, 1);
    CallThis<double>(FUN_555500, car);
    BYTE* cab = F(BYTE*, car, 21600);
    if (cab) {
        Lw_Cab(car, 0, &x, F(int, car, 10468));
        int t = F(int, F(BYTE*, car, 21600), 15520);
        if (t == 0 || t == 1)
            CallThis(FUN_4eb720, F(BYTE*, car, 21600) + 16060, 0);
        else if (t == 2)
            CallThis(FUN_4eb6e0, F(BYTE*, car, 21600) + 16060, -0.6f);
        Lw_Cab(car, 1, &x, F(int, car, 10472));
    }
    Lw_Body(car, 20, &x, F(int, car, 10436));
    F(float, car, 21596) = (float)(F(float, F(BYTE*, car, 9536), 32) + g_lw64b840);
    int i = 0;
    if (F(BYTE*, car, 21600) && F(int, (BYTE*)g_lw69809c + F(int, car, 10420) * 2416, 44) > 0)
        i = Lw_Links(car, 0, &x, 13);
    Lw_Body(car, 0, &x, F(int, car, 10432));
    Lw_Body(car, 60, &x, F(int, car, 10444));
    Lw_Body(car, 40, &x, F(int, car, 10440));
    if (F(BYTE*, car, 21600))
        Lw_Links(car, i, &x, 0x7fffffff);
    Lw_Body(car, 160, &x, F(int, car, 10464));
    Lw_Body(car, 140, &x, F(int, car, 10460));
    Lw_Body(car, 120, &x, F(int, car, 10456));
    Lw_Body(car, 100, &x, F(int, car, 10452));
    Lw_Body(car, 80, &x, F(int, car, 10448));
    Lw_Cab(car, 2, &x, F(int, car, 10476));
    return x == g_lw64c264 ? 0 : 1;
}

} // extern "C"

// === preparing the dashboard (FUN_53d530)

KING_VAR(int, g_db696c54, 696c54)
KING_VAR(int, g_db6d20ec, 6d20ec)
KING_VAR(int, g_db6cec68, 6cec68)
KING_VAR(double, g_db64b670, 64b670)
KING_VAR(double, g_db64f420, 64f420)
KING_VAR(double, g_db64b5b8, 64b5b8)
KING_VAR(double, g_db64d0e0, 64d0e0)
KING_VAR(float, g_db64f418, 64f418)
extern "C" void FUN_4eb690(); KING_ALIAS("_FUN_4eb690", "@Ramp_Position@8")
extern "C" void FUN_4ff5b0(); KING_ALIAS("_FUN_4ff5b0", "@Car_TopSpeed@12")
extern "C" void FUN_53d040(); KING_ALIAS("_FUN_53d040", "@Dash_Draw@44")
extern "C" void FUN_53d1f0(); KING_ALIAS("_FUN_53d1f0", "@Car_SetGauges@52")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")

extern "C" {

// [FUN_53d530] draws dashboard d (thiscall: surface, mode) for its car (+616): the speed (car, trailer, cab
// fallbacks; km/h or mph by 696c54) as three digits (9s when too fast), the odometer as five, the gear,
// lamps and the ramp; single player through 53d040, network through 53d1f0
void __fastcall Dash_Prepare(BYTE* d, void*, int a1, int a2)
{
    DWORD speedBits = 0;
    int ramp = 0, cab7188 = 1, rampLow = 0, gear = 0, flag = 0, odo = 0, lamps = 0;
    DWORD rpm = 1101004800, rampPos = 1065353216;
    float& speed = *(float*)&speedBits;
    BYTE* car = F(BYTE*, d, 616);
    if (car) {
        double scale = g_db696c54 ? g_db64b670 : g_db64f420;
        odo = (int)(F(float, car, 20980) * scale);
        lamps = F(int, car, 20996);
        rpm = F(DWORD, car, 20976);
        gear = F(int, car, 20972);
        if (BYTE* cab = F(BYTE*, car, 21600)) {
            cab7188 = F(int, cab, 7188);
            ramp = F(int, cab, 15440);
            double r = CallThis<double>(FUN_4eb690, cab + 16112);
            *(float*)&rampPos = (float)r;
            rampLow = r < g_db64b5b8;
        }
        BYTE* c = F(BYTE*, d, 616);
        BYTE* cab = F(BYTE*, c, 21600);
        if (!cab)
            speedBits = F(DWORD, c, 20964);
        else if (BYTE* t = F(BYTE*, c, 21604)) {
            float v[3];
            memcpy(v, t + 100, 12);
            speed = (float)CallC<double>(FUN_5ab4d0, v);
        } else if (F(int, cab, 13124))
            speed = (float)CallC<double>(FUN_5ab4d0, c + 3316);
        else {
            speedBits = F(int, cab, 13412) ? F(DWORD, cab, 13096) : F(DWORD, cab, 13104);
            if (g_db6d20ec)
                speed = (float)CallThis<double>(FUN_4ff5b0, cab, lamps);
        }
        if (CallVirt<int>(F(BYTE*, d, 616), 4) == 1 && F(int, F(BYTE*, d, 616), 21880))
            flag = 1;
    }
    double scale = g_db696c54 ? g_db64b670 : g_db64f420;
    double v = speed * scale;
    speed = (float)v;
    int k = (int)(v * g_db64d0e0);
    k = k < 0 ? -k : k;
    int sd[3], od[5];
    if (v < g_db64f418) {
        sd[0] = k / 100;
        sd[1] = (k - 100 * sd[0]) / 10;
        sd[2] = k - 10 * (10 * sd[0] + sd[1]);
    } else
        sd[0] = sd[1] = sd[2] = 9;
    int n = odo, p;
    od[0] = n / 100000;
    od[1] = (n - od[0] * 100000) / 10000;
    p = od[1] + 10 * od[0];
    od[2] = (n - p * 10000) / 1000;
    p = od[2] + 10 * p;
    od[3] = (n - p * 1000) / 100;
    p = od[3] + 10 * p;
    od[4] = (n - p * 100) / 10;
    if (!g_db6cec68)
        CallThis(FUN_53d040, d, a1, a2, sd, od, gear, rpm, lamps, cab7188, ramp);
    else
        CallThis(FUN_53d1f0, d, a1, a2, speedBits, od, gear, lamps, cab7188, ramp, rampPos, rampLow, flag);
}

} // extern "C"

// === the vehicle's history ring (FUN_554710)

extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")

// the reset point: the matrix m and the name of model +816
static void Vh_ResetPoint(BYTE* c, const BYTE* m, const char* name)
{
    memcpy(c + 20212, m, 48);
    strcpy((char*)c + 20260, name);
}

extern "C" {

// [FUN_554710] every +10928 seconds records vehicle c (thiscall) into the 50-entry ring (+10936 the index):
// the time, flags, matrix, cab wheels, matrix +20640, mode +10924, model and its name, speed, spin and the
// forward speed; the reset point (+20192 flags, +20212 matrix, +20260 name) follows the mode: 2 and 5 take
// the current place, 1 the place five samples back once five samples in a row were stable
void __fastcall Vehicle_RecordHistory(BYTE* c)
{
    if (!(CallC<double>(FUN_4e27a5) - F(float, c, 10932) > F(float, c, 10928)))
        return;
    float t = (float)CallC<double>(FUN_4e27a5);
    int i = (F(int, c, 10936) + 1) % 50;
    F(float, c, 10932) = t;
    F(int, c, 10936) = i;
    F(float, c, 11140 + i * 4) = t;
    F(DWORD, c, 10940 + i * 4) = F(DWORD, c, 32);
    memcpy(c + 12740 + i * 48, c + 516, 48);
    if (BYTE* cab = F(BYTE*, c, 21600))
        memcpy(c + 17540 + i * 12, cab + 11072, 12);
    memcpy(c + 15140 + i * 48, c + 20640, 48);
    F(int, c, 18140 + i * 4) = F(int, c, 10924);
    F(BYTE*, c, 18340 + i * 4) = F(BYTE*, c, 816);
    strcpy((char*)c + 18540 + i * 33, F(const char*, F(BYTE*, c, 816), 12));
    memcpy(c + 11540 + i * 12, c + 432, 12);
    memcpy(c + 12140 + i * 12, c + 444, 12);
    const float* v = (const float*)(c + 432);
    F(float, c, 11340 + i * 4) = (float)(F(float, c, 640) * (double)v[2] + v[1] * (double)F(float, c, 628) +
                                         F(float, c, 616) * (double)v[0]);
    int* flags = (int*)(c + 20192);
    switch (F(int, c, 10924)) {
    case 1: {
        for (int k = 4; k > 0; k--)
            flags[k] = flags[k - 1];
        flags[0] = 0;
        BYTE* cab = F(BYTE*, c, 21600);
        if (F(int, cab, 13124))
            return;
        bool ok = true;
        for (int k = 0; k < F(int, c, 10428); k++)
            if (!F(WORD, cab, 11112 + k * 248))
                ok = false;
        if (ok && !F(int, cab, 13416))
            flags[0] = 1;
        for (int k = 0; k < 5; k++)
            if (!flags[k])
                return;
        int j = (F(int, c, 10936) + 45) % 50;
        Vh_ResetPoint(c, c + 12740 + j * 48, (const char*)c + 18540 + j * 33);
        return;
    }
    case 2:
    case 5:
        for (int k = 0; k < 5; k++)
            flags[k] = 0;
        Vh_ResetPoint(c, c + 516, F(const char*, F(BYTE*, c, 816), 12));
        return;
    default:
        for (int k = 0; k < 5; k++)
            flags[k] = 0;
    }
}

} // extern "C"

// === a vehicle's light switches (FUN_546d10)

KING_VAR(AsmLabel, g_ls6d2114, 6d2114)
extern "C" void FUN_4e48a2(); KING_ALIAS("_FUN_4e48a2", "_Switch_SetChecked")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")

extern "C" {

// [FUN_546d10] sets the light switches of vehicle c's model (6d2114 + 1488 per model): the lamps on when dark
// (+20900 bit 7 clear) or the body's lamps (+9536: +144, +124) are off, by the low (+20856) and high (+20860)
// beams; the brake light (+20916) as state 2 and on its four switches; the cab light by +20920 / +20916
void __fastcall Vehicle_SetLightSwitches(BYTE* c)
{
    BYTE* t = (BYTE*)g_ls6d2114 + F(int, c, 10420) * 1488;
    bool dark = !(F(DWORD, c, 20900) & 0x80);
    BYTE* body = F(BYTE*, c, 9536);
    bool a = dark || !F(int, body, 144), b = dark || !F(int, body, 124);
    int brake = F(int, c, 20916) ? 2 : 0;
    CallC(FUN_4e48a2, F(BYTE*, t, 8), a && F(int, c, 20856) ? 1 : 0);
    CallC(FUN_4e48a2, F(BYTE*, t, 24), a && F(int, c, 20860) ? 1 : 0);
    CallC(FUN_4e48a2, F(BYTE*, t, 28), b && F(int, c, 20860) ? 1 : 0);
    CallC(FUN_4e48a2, F(BYTE*, t, 8), b && F(int, c, 20856) ? 1 : brake);
    CallC(FUN_4e48a2, F(BYTE*, t, 12), b && F(int, c, 20856) ? 1 : brake);
    for (int i = 0; i < 4; i++)
        CallC(FUN_5e1a10, F(BYTE*, t, 120 + i * 4), F(int, c, 20916) ? 1 : 0);
    CallC(FUN_5e1a10, F(BYTE*, t, 216), 0);
    CallC(FUN_5e1a10, F(BYTE*, t, 220), 0);
    CallC(FUN_5e1a10, F(BYTE*, c, 20500), (F(int, c, 20920) || F(int, c, 20916)) ? 1 : 0);
}

} // extern "C"

// === the damage keys of a vehicle (FUN_5465c0)

extern "C" {

// [FUN_5465c0] *out = the 9 damage key records (20 bytes: the scene object DamageXKey, +4, the level count +8,
// +12, the step +16) for the parts 1, FC, FR, FL, R, L, BR, BL, BC; *count = 9
void __cdecl Vehicle_MakeDamageKeys(BYTE** out, int* count)
{
    static const char* const k_parts[9] = {"1", "FC", "FR", "FL", "R", "L", "BR", "BL", "BC"};
    static const int k_levels[9] = {4, 3, 3, 3, 1, 1, 3, 3, 1};
    BYTE* k = (BYTE*)operator new(180);
    if (k)
        for (int i = 0; i < 9; i++) {
            F(int, k, i * 20) = 0;
            F(int, k, i * 20 + 4) = 0;
            F(int, k, i * 20 + 8) = 1;
            F(int, k, i * 20 + 12) = 0;
            F(float, k, i * 20 + 16) = 0.05f;
        }
    *out = k;
    *count = 9;
    for (int i = 0; i < *count; i++) {
        F(int, k, i * 20 + 4) = 0;
        F(int, k, i * 20 + 12) = 0;
        F(int, k, i * 20) = 0;
    }
    for (int i = 0; i < *count; i++) {
        char name[64];
        sprintf(name, "Damage%sKey", k_parts[i]);
        F(void*, k, i * 20) = CallC<void*>(FUN_5ef2b0, 0, name);
        F(int, k, i * 20 + 4) = 0;
        F(int, k, i * 20 + 12) = 0;
    }
    for (int i = 0; i < 9; i++) {
        F(float, k, i * 20 + 16) = i == 1 ? 0.05f : 0.1f;
        F(int, k, i * 20 + 8) = k_levels[i];
    }
}

} // extern "C"

// === choosing a control preset (FUN_531000)

KING_VAR(int, g_cp6d207c, 6d207c)         // the preset count
KING_VAR(AsmLabel, g_cp6cf4d8, 6cf4d8)    // the presets, 696 bytes each
KING_VAR(AsmLabel, g_cp6d1948, 6d1948)    // the key states
KING_VAR(AsmLabel, g_cp6d2038, 6d2038)
KING_VAR(const char*, g_cp678744, 678744)
KING_VAR(int, g_cp6d206c, 6d206c)
KING_VAR(int, g_cp6d2064, 6d2064)
KING_VAR(int, g_cp696c74, 696c74)
KING_VAR(int, g_cp6f4b80, 6f4b80)
extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")
extern "C" void FUN_578100(); KING_ALIAS("_FUN_578100", "_Joy_SetAutoCenter")
extern "C" void FUN_578d50(); KING_ALIAS("_FUN_578d50", "_FF_578d50")
extern "C" void FUN_577fc0(); KING_ALIAS("_FUN_577fc0", "_Joy_Acquire")

static void Cp_Mark(BYTE* key)
{
    if (key)
        key[1] = 1;
}

extern "C" {

// [FUN_531000] makes preset i the current controls (6d1db8): the key states cleared, the keys bound to the
// actions in use marked, for the preset named like the default (678744) also the last preset's 8 keys at +480;
// restarts the joystick's auto centre and force feedback; 1 for a bad index
int __cdecl Controls_SelectPreset(int i)
{
    if (i < 0 || i >= g_cp6d207c)
        return 1;
    BYTE* c = (BYTE*)g_controls6d1db8;
    BYTE* k = (BYTE*)g_cp6d1948;
    memcpy(c, (BYTE*)g_cp6cf4d8 + i * 696, 696);
    for (BYTE* p = k + 1; p < k + 591; p += 2)
        *p = 0;
    for (BYTE* p = k + 600; p < k + 720; p += 12)
        F(int, p, 0) = 0;
    CallThis(FUN_530020, k);
    for (int j = 0; j < 47; j++)
        if (F(int, k, 712 + j * 4)) {
            Cp_Mark(F(BYTE*, c, 264 + j * 4));
            Cp_Mark(F(BYTE*, c, 452 + j * 4));
        }
    BYTE* g = (BYTE*)g_cp6d2038;
    for (int j = 0; j < 3; j++)
        if (F(int, g, 24 + j * 4)) {
            if (BYTE* a = F(BYTE*, g, j * 4))
                F(int, a, 8) = 1;
            if (BYTE* a = F(BYTE*, g, 12 + j * 4))
                F(int, a, 8) = 1;
        }
    if (!strcmp((const char*)c, g_cp678744)) {
        g_cp6d206c = 1;
        g_cp6d2064 = 4;
        for (int j = 0; j < 8; j++)
            Cp_Mark(F(BYTE*, (BYTE*)g_cp6cf4d8 + g_cp6d207c * 696, 480 + j * 4));
    }
    CallC(FUN_578100, 0);
    CallC(FUN_577fc0, 0);
    CallC(FUN_578d50, F(int, c, 260));
    if (g_cp696c74) {
        g_696ca4 = 1;
        g_cp6f4b80 = 1;
        CallC(FUN_577fc0, 1);
    } else if (g_696ca4 && g_cp6f4b80 == 1)
        CallC(FUN_577fc0, 1);
    CallC(FUN_578100, 1);
    return 0;
}

} // extern "C"

// === the start lights (the update $L_53e510)

extern "C" void FUN_4e4ac4(); KING_ALIAS("_FUN_4e4ac4", "_Sound_PlayIf3")
extern "C" void FUN_4e5220(); KING_ALIAS("_FUN_4e5220", "_Sound_PlayIf")
extern "C" void FUN_53e4d0(); KING_ALIAS("_FUN_53e4d0", "@Obj53e4d0_Select@12")
extern "C" void FUN_56d6e0(); KING_ALIAS("_FUN_56d6e0", "_Copter_ResetPlayers")
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
KING_VAR(float, g_sl64d2f8, 64d2f8)
KING_VAR(double, g_sl64b5a0, 64b5a0)
KING_VAR(float, g_sl64c320, 64c320)
KING_VAR(double, g_sl64d300, 64d300)

// lets the race start: the player's car body (+21600) state 2 at +9984, the copter's players reset
static void Sl_Go()
{
    if (BYTE* car = F(BYTE*, g_game, 616))
        if (BYTE* b = F(BYTE*, car, 21600))
            F(BYTE, b, 9984) = 2;
    CallC(FUN_56d6e0);
}

// the blinking light of phase n: on (value) for the counts n % 3 == k below lim, else off
static int Sl_Blink(float t, float from, int k, int lim, int on)
{
    int n = (int)((t - from) * g_sl64d2f8 + g_sl64b5a0);
    return n % 3 == k && n < lim ? on : 0;
}

extern "C" {

// [$L_53e510] start lights s: state +72 (0 wait, 2 blinking amber, 3 green, 4 done) by the game time against
// the switch times +56, +60, +64, +68; the shown light +36 (-1 off, 2 with a beep, 3 green with the start
// sound once (+40) and the start (+80)) is selected; without lights (+84) and the race flag the start is direct
void __fastcall StartLights_Update(BYTE* s)
{
    BYTE* g = g_game;
    if (!g)
        return;
    if (!F(int, s, 84) && !(F(BYTE, g, 1404) & 1)) {
        if (F(int, s, 80))
            Sl_Go();
        return;
    }
    if (F(int, s, 32) && !F(int, s, 84) && F(int, s, 72) < 4) {
        float t = (float)CallC<double>(FUN_4e27a5);
        if (F(int, s, 72) == 3) {
            if (t > F(float, s, 68)) {
                F(int, s, 72) = 4;
                F(int, s, 36) = -1;
            } else
                F(int, s, 36) = 3;
        }
        if (F(int, s, 72) == 2) {
            if (t > F(float, s, 64)) {
                F(int, s, 72) = 3;
                F(float, s, 64) = t;
                F(int, s, 36) = 3;
                F(float, s, 68) = t + g_sl64c320;
            } else
                F(int, s, 36) = Sl_Blink(t, F(float, s, 60), 2, 9, 2);
        }
        if (F(int, s, 72) == 1) {
            if (t > F(float, s, 60)) {
                F(int, s, 72) = 2;
                F(float, s, 60) = t;
                F(int, s, 36) = 2;
                F(float, s, 64) = t + g_sl64d2f8;
            } else
                F(int, s, 36) = Sl_Blink(t, F(float, s, 56), 0, 5, 1);
        }
        if (F(int, s, 72) == 0 && !(t < F(float, s, 56))) {
            F(int, s, 72) = 2;
            F(float, s, 56) = t;
            F(float, s, 60) = t;
            F(int, s, 36) = 0;
            F(float, s, 64) = (float)(t + g_sl64d300);
        }
    }
    int light = F(int, s, 36);
    if (light == 2)
        CallC(FUN_4e4ac4, F(int, s, 76), 1.0f, 1.0f, 0);
    if (F(int, s, 36) == -1) {
        CallThis(FUN_53e4d0, s, 3);
        if (F(int, s, 84))
            CallThis(FUN_5d32f0, s, 1);
        return;
    }
    if (F(int, s, 36) == 3) {
        if (!F(int, s, 40)) {
            F(int, s, 40) = 1;
            CallC(FUN_4e5220, F(int, s, 76), 1.0f, 1.0f);
            if (F(int, s, 80))
                Sl_Go();
        }
        CallThis(FUN_53e4d0, s, 3);
    } else
        CallThis(FUN_53e4d0, s, F(int, s, 36));
    CallThis(FUN_5d32f0, s, 1);
}

} // extern "C"

// === drawing the cab view (the update $L_4df300)

extern "C" void FUN_4de230(); KING_ALIAS("_FUN_4de230", "@Cab_SetState@12")
extern "C" void FUN_5da080(); KING_ALIAS("_FUN_5da080", "@View_RenderFrame@4")
extern "C" void FUN_4eacce(); KING_ALIAS("_FUN_4eacce", "@History_Record@16")    // a dashboard text line: set (0, text)
KING_VAR(BYTE*, g_cv69684c, 69684c)
KING_VAR(BYTE*, g_cv6f33ac, 6f33ac)
KING_VAR(int, g_cv696cb4, 696cb4)
KING_VAR(double, g_cv64d6c0, 64d6c0)
KING_VAR(double, g_cv64b6e0, 64b6e0)
KING_VAR(AsmLabel, s_cv672f40, 672f40)
KING_VAR(AsmLabel, s_cv672f24, 672f24)
KING_VAR(AsmLabel, s_cv672f08, 672f08)

// fits window w (its rectangle r) to the screen below 672d7c (method 44 when its rectangle differs)
static void Cv_Fit(BYTE* w, const Veh234_Rect& r, int right, int bottom)
{
    if (r.l || r.t != g_minTop672d7c || r.r != right || r.b != bottom) {
        Veh234_Rect n = {0, g_minTop672d7c, right, bottom};
        CallVirt(w, 44, n);
    }
}

extern "C" {

// [$L_4df300] cab view s in the game (6cec68): fits it and its 3D window (+4820) to the screen; in a network
// lobby (+1400) clears the three status lines; else selects the gear switch and the cab state of the player's
// car and draws the frame with the ambient light pulsing with the hour
void __fastcall Cab_Update(BYTE* s)
{
    if (!g_q234_6cec68)
        return;
    BYTE* g = g_game;
    if (g && F(BYTE*, g, 616)) {
        int w = CallVirt<int>(g_cv69684c, 20), h = CallVirt<int>(g_cv69684c, 24);
        Veh234_Rect r1, r2;
        CallThis(FUN_5d85f0, F(BYTE*, s, 4820), &r1);
        CallThis(FUN_5d85f0, s, &r2);
        Cv_Fit(s, r2, w, h);
        Cv_Fit(F(BYTE*, s, 4820), r1, w, h);
        if (F(int, g_game, 1400)) {
            if (BYTE* o = F(BYTE*, s, 544))
                F(BYTE, o, 40) |= 1;
            F(int, s, 5084) = 0;
            F(int, s, 5088) = 0;
            CallThis(FUN_4eacce, s + 4724, 0, (const char*)s_cv672f40);
            CallThis(FUN_4eacce, s + 4756, 0, (const char*)s_cv672f24);
            CallThis(FUN_4eacce, s + 4788, 0, (const char*)s_cv672f08);
            return;
        }
    }
    BYTE* car = F(BYTE*, g_game, 616);
    BYTE* body = F(BYTE*, car, 9536);
    int v = F(int, body, 24);
    if (v == F(int, body, 28))
        v = 2;
    CallC(FUN_5e1a10, g_cv696cb4, v);
    F(int, F(BYTE*, s, 548), 84) = F(int, F(BYTE*, g_game, 616), 20396);
    CallThis(FUN_4de230, s, F(int, F(BYTE*, g_game, 616), 20396) + 1);
    double t = x87_sin(F(double, g_cv6f33ac, 88) * g_cv64d6c0);
    double k = t * t * g_cv64b6e0 + g_cv64b6e0;
    BYTE* c = F(BYTE*, s, 472);
    float a = (float)F(double, c, 48), b = (float)F(double, c, 56), d = (float)F(double, c, 64);
    F(double, c, 48) = k;
    F(double, F(BYTE*, s, 472), 56) = k;
    F(double, F(BYTE*, s, 472), 64) = k;
    CallThis(FUN_5da080, s);
    F(double, F(BYTE*, s, 472), 48) = a;
    F(double, F(BYTE*, s, 472), 56) = b;
    F(double, F(BYTE*, s, 472), 64) = d;
}

} // extern "C"

// === a car's step (FUN_556a70)

extern "C" void FUN_556a40(); KING_ALIAS("_FUN_556a40", "@Car_ApplyFound@8")
extern "C" void FUN_552940(); KING_ALIAS("_FUN_552940", "@Car_LampShadows@8")
extern "C" void FUN_556c60(); KING_ALIAS("_FUN_556c60", "@Car_CanSleep@4")    // whether the car is driven locally
extern "C" void FUN_580dd0(); KING_ALIAS("_FUN_580dd0", "@Item_Rest@4")    // the moving body's step
extern "C" void FUN_58f560(); KING_ALIAS("_FUN_58f560", "@Body_FindOverlap@8")
KING_VAR(float, g_cs64da74, 64da74)

extern "C" {

// [FUN_556a70] step of car body s (the car at -16) when active (+20920): driven here, its remote copy (+21584) is
// updated and it stops (+3976 = 3) when its leader (+3508) is close, then the body moves; otherwise it takes the
// remote copy's values (+20512..+20580); then the pending reset (+10564) and the history
void __fastcall CarBody_Step(BYTE* s)
{
    if (!F(int, s, 20920))
        return;
    BYTE* car = s - 16;
    if (CallThis<int>(FUN_556c60, car)) {
        if (BYTE* r = F(BYTE*, s, 21584)) {
            F(int, r, 10112) = 1;
            CallVirt(F(BYTE*, s, 21584), 0);
        }
        BYTE* o = F(BYTE*, s, 3508);
        if (F(int, s, 3976) != 3 && o) {
            float x = F(float, o, 3372) - F(float, s, 536), y = F(float, o, 3376) - F(float, s, 540);
            double z = (double)F(float, o, 3380) - F(float, s, 544);
            if ((double)x * x + (double)y * y + z * z < g_cs64da74 && !CallThis<int>(FUN_58f560, s))
                F(int, s, 3976) = 3;
        }
        CallThis(FUN_580dd0, s);
    } else {
        CallThis(FUN_556a40, car);
        if (BYTE* r = F(BYTE*, s, 21584)) {
            F(int, s, 20512) = F(int, r, 9996);
            F(int, s, 20516) = F(int, r, 10012);
            F(int, s, 20520) = F(int, r, 10000);
            F(int, s, 20524) = F(short, r, 10004);
            F(int, s, 20528) = F(short, r, 10006);
            memcpy(s + 20532, s + 20660, 12);
            memcpy(s + 20544, s + 536, 12);
            memcpy(s + 20556, s + 416, 12);
            memcpy(s + 20568, s + 428, 12);
        }
    }
    if (F(int, s, 10564)) {
        CallThis(FUN_552940, car);
        F(int, s, 10564) = 0;
    }
    Vehicle_RecordHistory(car);
}

} // extern "C"

// === placing a car body (FUN_545ef0)

extern "C" void FUN_58d2b0(); KING_ALIAS("_FUN_58d2b0", "@Body_UpdatePlanes@8")
KING_VAR(AsmLabel, g_cpModels, 697b30)      // 2416 bytes per car model

// the model of car s
static BYTE* Cp_Model(BYTE* s) { return (BYTE*)g_cpModels + F(int, s, 10420) * 2416; }

extern "C" {

// [FUN_545ef0] places car s by record r (+0 an id to +412 and +3284, +4 the position to +420 and +3304, +16 the
// frame to +708, its inverse to +756) and moves the model's wheel points (+684 count, +688), body points (+160,
// +168) and extra points (+164, +408, after the body ones) into the world (+3848, +3608)
void __fastcall Car_PlaceByRecord(BYTE* s, void*, const BYTE* r)
{
    F(int, s, 412) = F(int, r, 0);
    memcpy(s + 420, r + 4, 12);
    const float* m = (const float*)(s + 708);
    memcpy(s + 708, r + 16, 48);
    BYTE inv[48];
    memcpy(s + 756, CallC<BYTE*>(FUN_5ac2e0, inv, m), 48);
    F(int, s, 3284) = F(int, s, 412);
    memcpy(s + 3304, s + 420, 12);
    for (int i = 0; i < F(int, Cp_Model(s), 684); i++) {
        const float* p = (const float*)(Cp_Model(s) + 688 + i * 12);
        float* o = (float*)(s + 3848 + i * 12);
        float x = (float)((double)p[2] * m[6] + (double)p[0] * m[0] + (double)p[1] * m[3] + m[9]);
        float y = (float)((double)p[2] * m[7] + (double)p[1] * m[4] + (double)m[1] * p[0] + m[10]);
        float z = (float)((double)p[2] * m[8] + (double)m[2] * p[0] + (double)p[1] * m[5] + m[11]);
        o[0] = x, o[1] = y, o[2] = z;
    }
    for (int i = 0; i < F(int, Cp_Model(s), 160); i++) {
        const float* p = (const float*)(Cp_Model(s) + 168 + i * 12);
        float* o = (float*)(s + 3608 + i * 12);
        float x = (float)((double)m[0] * p[0] + (double)p[1] * m[3] + (double)p[2] * m[6] + m[9]);
        float y = (float)((double)p[2] * m[7] + (double)p[1] * m[4] + (double)m[1] * p[0] + m[10]);
        float z = (float)((double)m[8] * p[2] + (double)m[5] * p[1] + (double)m[2] * p[0] + m[11]);
        o[0] = x, o[1] = y, o[2] = z;
    }
    for (int j = 0; j < F(int, Cp_Model(s), 164); j++) {
        const float* p = (const float*)(Cp_Model(s) + 408 + j * 12);
        float x = (float)((double)p[2] * m[6] + (double)p[1] * m[3] + (double)p[0] * m[0] + m[9]);
        float y = (float)((double)p[0] * m[1] + (double)p[2] * m[7] + (double)p[1] * m[4] + m[10]);
        float z = (float)((double)p[2] * m[8] + (double)p[1] * m[5] + (double)p[0] * m[2] + m[11]);
        float* o = (float*)(s + 3608 + (F(int, Cp_Model(s), 160) + j) * 12);
        o[0] = x, o[1] = y, o[2] = z;
    }
    CallThis(FUN_58d2b0, s + 16);
}

} // extern "C"

// === the cab gauges (FUN_53d1f0)

extern "C" void FUN_4e48a2(); KING_ALIAS("_FUN_4e48a2", "_Switch_SetChecked")
extern "C" void FUN_4e48e2(); KING_ALIAS("_FUN_4e48e2", "_Frame_ApplyChecked")
extern "C" void Cg_MatMul(); KING_ALIAS("_Cg_MatMul", "_Matrix_Multiply")
extern "C" void Cg_RotY(); KING_ALIAS("_Cg_RotY", "_Matrix_RotationY")
KING_VAR(float, g_cg64c250, 64c250)

// frame f (if any) set to a turn by angle a about y of the rest frame base
static void Cg_Turn(int f, float a, const BYTE* base)
{
    if (!f)
        return;
    float r[12], o[12];
    CallC(Cg_MatMul, o, CallC<float*>(Cg_RotY, r, a), base);
    CallC(FUN_4e48e2, f, 0, o);
}

extern "C" {

// [FUN_53d1f0] sets car s's gauges: speed a3, revs a5 and a9 turn the needles; outside the cab (+1400) those of s
// (+1064.., scaled by +1260..+1276, the first two at most full), inside those of the cab 696cb0 (+4876.., scaled by
// +5068..), with its lamps (a6, not a7, a8, a10) and the five switches a4[0..4]
void __fastcall Car_SetGauges(BYTE* s, void*, int a1, int a2, float a3, const int* a4, float a5, int a6, int a7,
                              int a8, float a9, int a10, int a11)
{
    if (!F(int, s, 1400)) {
        float sp = (float)((a3 < 0 ? -(double)a3 : (double)a3) / F(float, s, 1268));
        double rv = a5 / (double)F(float, s, 1260);
        float t = (float)(a9 * (double)F(float, s, 1276));
        if (sp > g_cg64c250)
            sp = 1.0f;
        if (rv > g_cg64c250)
            rv = g_cg64c250;
        sp = (float)(sp * (double)F(float, s, 1272));
        float r = (float)(rv * F(float, s, 1264));
        if (F(int, s, 1064)) {
            float o[12], m[12];
            CallC(Cg_MatMul, m, CallC<float*>(Cg_RotY, o, r), s + 1116);
            CallC(FUN_4e48e2, F(int, s, 1064), 0, m);
        }
        Cg_Turn(F(int, s, 1068), sp, s + 1164);
        Cg_Turn(F(int, s, 1072), t, s + 1212);
        return;
    }
    BYTE* c = (BYTE*)g_vh696cb0;
    if (!F(int, c, 4860))
        return;
    float sp = (float)((a3 < 0 ? -(double)a3 : (double)a3) * F(float, c, 5076));
    float r = (float)(a5 * (double)F(float, c, 5068));
    float t = (float)(a9 * (double)F(float, c, 5072));
    Cg_Turn(F(int, c, 4876), r, c + 4924);
    Cg_Turn(F(int, c, 4880), sp, c + 4972);
    Cg_Turn(F(int, c, 4884), t, c + 5020);
    c = (BYTE*)g_vh696cb0;
    if (int w = F(int, c, 4888))
        CallC(FUN_4e48a2, w, a6);
    if (int w = F(int, (BYTE*)g_vh696cb0, 4892))
        CallC(FUN_4e48a2, w, a7 == 0);
    if (int w = F(int, (BYTE*)g_vh696cb0, 4896))
        CallC(FUN_4e48a2, w, a8);
    if (int w = F(int, (BYTE*)g_vh696cb0, 4900))
        CallC(FUN_4e48a2, w, a10);
    for (int off = 4920, i = 0; off > 4900; off -= 4, i++)
        if (int w = F(int, (BYTE*)g_vh696cb0, off))
            CallC(FUN_4e480c, w, a4[i]);
}

} // extern "C"

// === following the leader (FUN_548ec0)

extern "C" void FUN_4fc9b0(); KING_ALIAS("_FUN_4fc9b0", "@Body_SetControlMode@28")
extern "C" void FUN_57dec0(); KING_ALIAS("_FUN_57dec0", "@SceneObj_Matrix2@12")
KING_VAR(float, g_fl64f730, 64f730)
KING_VAR(double, g_fl64f738, 64f738)
KING_VAR(double, g_fl64b668, 64b668)
KING_VAR(double, g_fl64b678, 64b678)
KING_VAR(BYTE, g_fl6796b0, 6796b0)

extern "C" {

// [FUN_548ec0] car s behind its leader (+3524): its body (+21600) drives itself (mode 3) when close behind it
// (or, in the cab, the body has a contact) or the race is on (+15448), else mode 1; it stops (+3992 = 3) when it
// catches up with a leader without a driver (+1204), or one that is driven when close
void __fastcall Car_FollowLeader(BYTE* s)
{
    BYTE* l = F(BYTE*, s, 3524);
    BYTE* c = F(BYTE*, s, 21600);
    if (!l) {
        CallThis(FUN_4fc9b0, c, 1, F(int, s, 816), s + 516, s + 432, s + 444);
        return;
    }
    BYTE tmp[48];
    const float* m = CallThis<const float*>(FUN_57dec0, l, tmp);
    const float* p = (const float*)(s + 552);
    float qx = (float)((double)m[3] * p[1] + (double)m[6] * p[2] + (double)m[0] * p[0] + m[9]);
    float qy = (float)((double)m[4] * p[1] + (double)m[7] * p[2] + (double)m[1] * p[0] + m[10]);
    double qz = (double)m[5] * p[1] + (double)m[8] * p[2] + (double)m[2] * p[0] + m[11];
    float d2 = (float)((double)qx * qx + qx * (double)qx + (double)qy * qy);
    int hit = 0;
    if (F(int, c, 9784) == 3 && (double)qy * qy + (double)qx * qx + qz * qz < g_fl64f738)
        for (int i = 0; i < F(int, c, 13120); i++) {
            BYTE* e = c + 13452 + i * 4;
            if (hit || F(int, e, -32) && !F(int, e, 0) || F(int, e, -72))
                hit = 1;
        }
    int mode = 1;
    if (F(BYTE*, s, 3524) && F(int, F(BYTE*, s, 3524), 1204) && d2 < g_fl64f730 || hit || F(int, c, 15448))
        mode = 3;
    CallThis(FUN_4fc9b0, c, mode, F(int, s, 816), s + 516, s + 432, s + 444);
    l = F(BYTE*, s, 3524);
    bool stop;
    if (F(int, l, 1204))
        stop = d2 < g_fl64f730;
    else {
        float t = (float)((double)F(float, l, 3272) + F(float, s, 3288));
        const float* q = (const float*)(s + 432);
        float d[3] = {(float)((double)q[0] - F(float, l, 3300)), (float)((double)q[1] - F(float, l, 3304)),
                      (float)((double)q[2] - F(float, l, 3308))};
        float e[3] = {(float)((double)p[0] - F(float, l, 3372)), (float)((double)p[1] - F(float, l, 3376)),
                      (float)((double)p[2] - F(float, l, 3380))};
        double r = t;
        if ((double)d[2] * e[2] + (double)d[1] * e[1] + (double)e[0] * d[0] < g_fl64b678)
            r = CallC<double>(FUN_5ab4d0, d) * F(double, g_6ef1fc, 2960) + t;
        stop = r * r * g_fl64b668 > (double)e[0] * e[0] + (double)e[1] * e[1] + (double)e[2] * e[2];
    }
    if (stop && !(g_fl6796b0 & 4) && F(int, s, 3992) != 3 && !CallThis<int>(FUN_58f560, s + 16))
        F(int, s, 3992) = 3;
}

} // extern "C"

// === the car's working order (FUN_557090)

KING_VAR(double, g_wo64b6e0, 64b6e0)
KING_VAR(double, g_wo64b5b8, 64b5b8)
KING_VAR(double, g_wo64d318, 64d318)
KING_VAR(double, g_wo64d0f0, 64d0f0)
KING_VAR(double, g_wo64b670, 64b670)
KING_VAR(float, g_wo64c250, 64c250)
KING_VAR(float, g_wo64c264, 64c264)

extern "C" {

// [FUN_557090] how well car s works, 0..1: the least of its fuel (+20608, double the level +548 when below a half),
// 0 while being repaired (+21568), each of the nine damages of its record (+9536: damage / limit pairs, weighted)
// and their combined product
double __fastcall Car_WorkingOrder(BYTE* s)
{
    float x = (float)F(int, s, 20608);
    if (F(float, s, 548) < g_wo64b6e0)
        x = F(float, s, 548) < g_wo64c264 ? 0 : (float)((double)F(float, s, 548) + F(float, s, 548));
    float y = 1.0f;
    if (CallThis<int>(FUN_549bf0, s + 21568))
        y = 0;
    float m = y < x ? y : x;
    const BYTE* c = F(BYTE*, s, 9536);
    static const double* const k[9] = {&g_wo64b5b8, &g_wo64b6e0, &g_wo64b6e0, &g_wo64b6e0, &g_wo64d318,
                                       &g_wo64d318, &g_wo64d0f0, &g_wo64d0f0, &g_wo64d0f0};
    float p = 0;
    for (int j = 0; j < 9; j++) {
        double r = (double)F(int, c, 4 + j * 20) / F(int, c, 8 + j * 20);
        if (r < 0)
            r = -r;
        float q = (float)(g_wo64b670 - r * *k[j]);
        if (q < m)
            m = q;
        p = j ? (float)(((double)q + g_wo64c250) * p * g_wo64b6e0) : (float)(((double)q + g_wo64c250) * g_wo64b6e0);
    }
    if (p < m)
        m = p;
    if (m < g_wo64c264)
        m = g_wo64c264;
    return m;
}

} // extern "C"

// === listing the control presets (FUN_530d40)

extern "C" void Cl_strnicmp(); KING_ALIAS("_Cl_strnicmp", "__strnicmp")
extern "C" void Cl_sprintf(); KING_ALIAS("_Cl_sprintf", "_sprintf")
KING_VAR(const char*, g_cl678740, 678740)
KING_VAR(int, g_cl6d2074, 6d2074)
KING_VAR(int, g_cl6f4b90, 6f4b90)
KING_VAR(AsmLabel, g_clDevices, 6f3528)      // 568 bytes each
KING_VAR(char, g_cl6d1b94, 6d1b94)
KING_VAR(char, g_cl6d1b58, 6d1b58)
KING_VAR(char, g_cl6d1b5a, 6d1b5a)
KING_VAR(char, g_cl6d1b4e, 6d1b4e)
KING_VAR(char, g_cl6d1b50, 6d1b50)
KING_VAR(char, g_cl6d1b8e, 6d1b8e)
KING_VAR(char, g_cl6d1b90, 6d1b90)
KING_VAR(char, g_cl6d1b92, 6d1b92)

extern "C" {

// [FUN_530d40] builds the presets (from 1) of the 6f4b90 input devices: each from its saved node (by its name +308
// and ids +40..+46) and named after it with its device index (+260); the first device named like 678740 also
// gets a second, default preset (678744, default keys at +480)
void __cdecl Controls_ListPresets()
{
    g_cp6d207c = 1;
    g_cl6d2074 = (int)strlen(g_cl678740);
    for (int i = 0, k = 1; i < g_cl6f4b90; i++, k++, g_cp6d207c++) {
        BYTE* d = (BYTE*)g_clDevices + i * 568;
        const char* name = (const char*)d + 308;
        BYTE* found = 0;
        Node530c40_Find((const BYTE*)name, F(int, d, 0), F(WORD, d, 40), F(WORD, d, 42), F(WORD, d, 44), F(WORD, d, 46),
                        &found);
        BYTE* p = (BYTE*)g_cp6cf4d8 + g_cp6d207c * 696;
        if (found) {
            memcpy(p, found + 36, 696);
            strcpy((char*)p, name);
        }
        F(int, p, 260) = i;
        if (CallC<int>(Cl_strnicmp, name, g_cl678740, g_cl6d2074))
            continue;
        CallC(Cl_sprintf, p, name);
        if (g_cp6d207c == k) {
            i--;
            k--;
            continue;
        }
        strcpy((char*)p, g_cp678744);
        char* keys[8] = {&g_cl6d1b94, &g_cl6d1b58, &g_cl6d1b5a, &g_cl6d1b4e,
                         &g_cl6d1b50, &g_cl6d1b8e, &g_cl6d1b90, &g_cl6d1b92};
        memcpy(p + 480, keys, 32);
    }
}

} // extern "C"

// === the room object of a point (FUN_533340)

extern "C" void* __cdecl AI_Make449c40(void* out, void* x);
extern "C" int* __cdecl Ai_GetPositionIdV(int* out, float x, float y, float z);
extern "C" int* __cdecl Ai_GetPositionIdRV(int* out, int room, float x, float y, float z);
KING_VAR(double, g_ro64b678, 64b678)

// whether position p is nowhere (no road, no node, t 0, no object)
static bool Ro_IsNull(const Position* p)
{
    const int* m = (const int*)p;
    return m[0] == -1 && m[1] == -1 && *(const double*)(m + 2) == g_ro64b678 && !m[4] && !m[5] && !m[6];
}

// *obj = the room object of position p (its room's name in r); whether one was found
static bool Ro_Find(const Position* p, RoomInfo* r, void** obj)
{
    Road_RoomInfo(*p, r);
    *obj = CallC<void*>(FUN_5ef2b0, 0, r->room);
    return *obj != 0;
}

extern "C" {

// [FUN_533340] the room object (*obj) at point v: by its position on room id's roads (*pos), else, when anywhere
// is allowed, by its nearest position; 1 or 2 when found that way, else 0
int __cdecl Room_ObjectAt(void* id, const float* v, void** obj, Position* pos, int anywhere)
{
    *obj = 0;
    int res = 0;
    RoomInfo ri = {0};
    ri.module = (const char*)g_undefinedModule;
    ri.room = (const char*)g_undefinedRoom;
    ri.m_36 = -1;
    ri.m_40 = -1;
    void* r = id;
    AI_Make449c40(&r, id);
    if ((int)r != -1) {
        int t[8];
        memcpy(pos, Ai_GetPositionIdRV(t, (int)r, v[0], v[1], v[2]), 32);
        if (!Ro_IsNull(pos) && Ro_Find(pos, &ri, obj))
            res = 1;
    }
    if (!*obj && anywhere) {
        int t[8];
        memcpy(pos, Ai_GetPositionIdV(t, v[0], v[1], v[2]), 32);
        if (!Ro_IsNull(pos) && Ro_Find(pos, &ri, obj))
            return 2;
    }
    return res;
}

} // extern "C"

// === placing a car and its trailer (FUN_55b590)

extern "C" void Ct_HitObject(); KING_ALIAS("_Ct_HitObject", "_Query_HitObject")
extern "C" void Ct_SetOrientation(); KING_ALIAS("_Ct_SetOrientation", "@Body_SetOrientation@16")
extern "C" void Ct_SetInMode(); KING_ALIAS("_Ct_SetInMode", "@Car_SetInMode@12")

// car c placed by frame m on ground g (asleep in a replay: mode 4, the frame kept at +20640)
static void Ct_Place(BYTE* c, int g, const BYTE* m)
{
    if (F(int, F(BYTE*, c, 21600), 13124)) {
        CallThis(Ct_SetInMode, c, 4);
        memcpy(c + 20640, m, 48);
    } else {
        CallThis(Ct_SetInMode, c, 1);
        CallThis(Ct_SetOrientation, F(BYTE*, c, 21600), g, m);
    }
}

extern "C" {

// [FUN_55b590] places car s at frame a and its trailer (+21880) at frame b, each on the ground found on the way from
// its present position (else its present ground, the body woken)
void __fastcall Car_PlaceWithTrailer(BYTE* s, void*, const BYTE* a, const BYTE* b)
{
    BYTE* t = F(BYTE*, s, 21880);
    BYTE m[48];
    memcpy(m, F(BYTE*, s, 21600) + 16, 48);
    int g1 = CallC<int>(Ct_HitObject, F(int, F(BYTE*, s, 21600), 244), m + 36, a + 36);
    memcpy(m, F(BYTE*, t, 21600) + 16, 48);
    int g2 = CallC<int>(Ct_HitObject, F(int, F(BYTE*, t, 21600), 244), m + 36, b + 36);
    if (!g2) {
        g2 = F(int, F(BYTE*, t, 21600), 244);
        CallVirt(t + 16, 28, 1);
    }
    if (!g1) {
        g1 = F(int, F(BYTE*, s, 21600), 244);
        CallVirt(s + 16, 28, 1);
    }
    Ct_Place(s, g1, a);
    Ct_Place(t, g2, b);
}

} // extern "C"

// === the 55eaa0 truck's constructor

extern "C" void Tc_Base(); KING_ALIAS("_Tc_Base", "_FUN_543e00")
extern "C" void Tc_IntCopy(); KING_ALIAS("_Tc_IntCopy", "@Int_Copy@12")
extern "C" void Tc_SetAuto(); KING_ALIAS("_Tc_SetAuto", "@Car_SetAuto@12")

// [FUN_55eaa0] constructor (a1, a2, a3 copied, a4): the vehicle base, the 15 dwords at +21888 cleared, this
// truck's vtables, no trailer (+21880), +20936 = 1; the cab (+21600) put on manual, started, +10006 = 1 and
// +10000 = 1.0; the type's vector applied
extern "C" BYTE* __fastcall Truck55eaa0_Construct(BYTE* s, void*, int a1, int a2, int a3, int a4)
{
    DWORD copy;
    CallThis(Tc_IntCopy, &copy, &a3);
    CallThis(Tc_Base, s, a1, a2, copy, a4);
    memset(s + 21888, 0, 60);
    F(void*, s, 0) = g_veh64fb8c;
    F(void*, s, 16) = g_veh64faa0;
    F(void*, s, 272) = g_veh64fa54;
    F(void*, s, 392) = g_veh64fa50;
    F(int, s, 21880) = 0;
    F(int, s, 20936) = 1;
    CallThis(Tc_SetAuto, F(BYTE*, s, 21600), 0);
    CallThis(FUN_4ff790, F(BYTE*, s, 21600));
    F(WORD, F(BYTE*, s, 21600), 10006) = 1;
    F(DWORD, F(BYTE*, s, 21600), 10000) = 0x3F800000;
    CallThis(FUN_556110, s);
    return s;
}

// === the game's input messages and cheat codes (FUN_532950)

extern "C" void FUN_4e4468(); KING_ALIAS("_FUN_4e4468", "_Clock_Text")
extern "C" void Ck_SetCursor(); KING_ALIAS("_Ck_SetCursor", "_Input_SetCursorPos")
KING_VAR(AsmLabel, g_ck6d1948, 6d1948)    // key states, 2 bytes each (down, held)
KING_VAR(BYTE*, g_ck6d1ed8, 6d1ed8)
KING_VAR(BYTE*, g_ck6d1f94, 6d1f94)
KING_VAR(int, g_ck6d20e4, 6d20e4)
KING_VAR(float, g_ck6d20e8, 6d20e8)
KING_VAR(AsmLabel, g_ck678764, 678764)    // the codes: typed forwards (else backwards)
KING_VAR(AsmLabel, g_ck6d2084, 6d2084)    // active
KING_VAR(AsmLabel, g_ck6d20bc, 6d20bc)    // text
KING_VAR(AsmLabel, g_ck6d20a0, 6d20a0)    // progress
KING_VAR(AsmLabel, g_ck678748, 678748)    // length
KING_VAR(int, g_ck6d1ba0, 6d1ba0)
KING_VAR(int, g_ck6d1bac, 6d1bac)
KING_VAR(int, g_ck696850, 696850)
KING_VAR(int, g_ck696854, 696854)
KING_VAR(float, g_ck6d1b9c, 6d1b9c)
KING_VAR(float, g_ck6d1ba8, 6d1ba8)
KING_VAR(float, g_ck6d1b98, 6d1b98)
KING_VAR(float, g_ck6d1ba4, 6d1ba4)
KING_VAR(BYTE, g_ck6d1b48, 6d1b48)
KING_VAR(BYTE, g_ck6d1b4c, 6d1b4c)
KING_VAR(BYTE*, g_ck6cec6c, 6cec6c)
KING_VAR(float, g_ck64c250, 64c250)
KING_VAR(float, g_ck64c938, 64c938)
KING_VAR(double, g_ck64b6e0, 64b6e0)
KING_VAR(char, s_ck678b00, 678b00)
KING_VAR(char, s_ck672ec8, 672ec8)    // ".\truck.ini"
KING_VAR(char, s_ck67305c, 67305c)    // "$"
KING_VAR(char, s_ck678af8, 678af8)    // "source"
KING_VAR(char, s_ck6733c4, 6733c4)    // "INSTALL"
KING_VAR(char, s_ck678ae4, 678ae4)
KING_VAR(char, s_ck678adc, 678adc)
KING_VAR(char, s_ck678ad0, 678ad0)

// a mouse coordinate as -1..1 (the axis scale, flipped when set), clamped to [64c938, 64c250]
static float Ck_Axis(int v, int size, float scale, int flip)
{
    double a = ((double)v / size - g_ck64b6e0) * scale;
    a += a;
    if (flip)
        a = -a;
    if (a > g_ck64c250)
        return g_ck64c250;
    if (a < g_ck64c938)
        return g_ck64c938;
    return (float)a;
}

// recentres the cursor unless the game runs windowed (+1300 > 1)
static void Ck_Recentre()
{
    if (F(int, g_ck6cec6c, 1300) <= 1)
        CallC(Ck_SetCursor, g_ck696850 / 2, g_ck696854 / 2);
}

extern "C" {

// [FUN_532950] handles input message m (+0 the kind, +2/+4 the mouse, +6 the key) for the game s: key states
// (6d1948), the debug position text on the two position keys, the seven cheat codes (the sixth only on a
// developer install), the mouse axes (6d1b98, 6d1ba4) and buttons (6d1b48, 6d1b4c)
void __fastcall Game_Input(BYTE* s, void*, const short* m)
{
    BYTE* keys = (BYTE*)g_ck6d1948;
    switch (m[0]) {
    case 256: {
        int c = m[3];
        if (c == 81)
            CallC(FUN_5e48f0);
        BYTE* k = keys + c * 2;
        k[0] = 1;
        BYTE* car = F(BYTE*, s, 616);
        if (car && F(BYTE*, car, 21600) && (k == g_ck6d1ed8 || k == g_ck6d1f94)) {
            BYTE* w = F(BYTE*, s, 32);
            char where[100], text[100];
            g_ck6d20e4 = F(int, w, 2992);
            g_ck6d20e8 = (float)F(double, w, 2968);
            CallC(FUN_4e4468, where);
            w = F(BYTE*, s, 32);
            sprintf(text, &s_ck678b00, F(int, w, 2992), F(double, w, 2968), where);
        }
        int* fwd = (int*)g_ck678764;
        int* act = (int*)g_ck6d2084;
        const char** str = (const char**)g_ck6d20bc;
        int* prog = (int*)g_ck6d20a0;
        int* len = (int*)g_ck678748;
        for (int i = 0; i < 7; i++) {
            bool hit = false;
            if (!act[i] && str[i])
                hit = fwd[i] ? str[i][prog[i]] == c : str[i][len[i] - prog[i] - 1] == c;
            if (!hit) {
                prog[i] = 0;
                continue;
            }
            if (++prog[i] != len[i])
                continue;
            if (i == 5) {
                char buf[100];
                GetPrivateProfileStringA(&s_ck6733c4, &s_ck678af8, &s_ck67305c, buf, 100, &s_ck672ec8);
                if (_strnicmp(buf, &s_ck678ae4, 19) && _strnicmp(buf, &s_ck678adc, 7)
                    && _strnicmp(buf, &s_ck678ad0, 11))
                    continue;
            }
            act[i] = 1;
        }
        break;
    }
    case 257:
        if (keys[m[3] * 2 + 1])
            keys[m[3] * 2] = 0;
        break;
    case 512:
        g_ck6d1b98 = Ck_Axis(m[1], g_ck696850, g_ck6d1b9c, g_ck6d1ba0);
        g_ck6d1ba4 = Ck_Axis(m[2], g_ck696854, g_ck6d1ba8, g_ck6d1bac);
        break;
    case 513:
        g_ck6d1b48 = 1;
        Ck_Recentre();
        break;
    case 514:
        g_ck6d1b48 = 0;
        break;
    case 516:
        g_ck6d1b4c = 1;
        Ck_Recentre();
        break;
    case 517:
        g_ck6d1b4c = 0;
        break;
    }
}

} // extern "C"

// === a car's centre (FUN_542280)

extern "C" void Cc_PointsNext(); KING_ALIAS("_Cc_PointsNext", "@Points_Next@12")
KING_VAR(AsmLabel, g_cc6978c8, 6978c8)

// [FUN_542280] the centre of car s (+21040): x and y halfway across its body points' extent (from 0), z the
// height of its frame (+716.. by the type's column) less the mean half wheel size (+10584, count +10424, also
// stored per wheel at +20740) and the type's offset
extern "C" void __fastcall Car_Centre(BYTE* s)
{
    float lo[2] = {0, 0}, hi[2] = {0, 0};
    float p[3] = {0, 0, 0};
    while (CallThis<int>(Cc_PointsNext, s + 16, p))
        for (int k = 0; k < 2; k++) {
            if (p[k] < lo[k])
                lo[k] = p[k];
            if (p[k] > hi[k])
                hi[k] = p[k];
        }
    double cx = ((double)lo[0] + hi[0]) * g_vh64c258;
    float cy = (float)(((double)lo[1] + hi[1]) * g_vh64c258);
    int n = F(int, s, 10424);
    double sum = g_vh64c264;
    for (int i = 0; i < n; i++)
        sum += F(float, s, 10584 + i * 4) * (double)g_vh64c258;
    for (int i = 0; i < F(int, s, 10424); i++)
        F(float, s, 20740 + i * 4) = (float)(sum / F(int, s, 10424));
    unsigned t = F(unsigned, s, 10420) * 2416;
    const BYTE* ty = (const BYTE*)g_carTypes697914 + t;
    double z = F(float, s, 740) * (double)F(float, ty, 260) + F(float, ty, 256) * (double)F(float, s, 728)
        + F(float, s, 716) * (double)F(float, ty, 252) + F(float, s, 752);
    double h = sum / F(int, s, 10424) + F(float, (const BYTE*)g_cc6978c8 + t, 4);
    F(float, s, 21040) = (float)cx;
    F(float, s, 21044) = cy;
    F(float, s, 21048) = (float)-(h - z);
}

// === whether a car can fall asleep (FUN_556c60)

extern "C" void Cs_LenSq(); KING_ALIAS("_Cs_LenSq", "_Vec3_LengthSq4e298b")
extern "C" void Cs_Len(); KING_ALIAS("_Cs_Len", "_Vec3_Length")
KING_VAR(double, g_cs64b658, 64b658)
KING_VAR(double, g_cs64f7b8, 64f7b8)

// the squared length of a - b (three floats each)
static double Cs_DiffSq(const BYTE* s, int a, int b)
{
    float d[3] = {F(float, s, a) - F(float, s, b), F(float, s, a + 4) - F(float, s, b + 4), F(float, s, a + 8) - F(float, s, b + 8)};
    return CallC<double>(Cs_LenSq, d);
}

extern "C" {

// [FUN_556c60] whether car s may sleep: not held (+1220); its cab (+21600) idle (speed +9996 at most 64b5a0, no
// +13416) with the same controls as stored (+20528..); its frame, position, velocity and spin nearly unchanged since
// they were stored (+20548.., +20560.., +20572.., +20584..) and slow; no awake neighbour (+3524) about to reach it
int __fastcall Car_CanSleep(BYTE* s)
{
    if (F(int, s, 1220))
        return 0;
    if (BYTE* c = F(BYTE*, s, 21600)) {
        if (F(float, c, 9996) > g_sl64b5a0 || F(int, c, 13416))
            return 0;
        if (F(float, s, 20528) != F(float, c, 9996) || F(float, s, 20532) != F(float, c, 10012)
            || F(float, s, 20536) != F(float, c, 10000) || F(int, s, 20540) != F(short, c, 10004)
            || F(int, s, 20544) != F(short, c, 10006))
            return 0;
    }
    if (Cs_DiffSq(s, 20548, 20676) > g_cs64b658 || Cs_DiffSq(s, 20560, 552) > g_cs64b658
        || Cs_DiffSq(s, 20572, 432) > g_sl64b5a0 || Cs_DiffSq(s, 20584, 444) > g_cs64f7b8
        || CallC<double>(Cs_LenSq, s + 432) > g_sl64b5a0)
        return 0;
    for (int i = 0; i < 6; i++) {
        BYTE* o = F(BYTE*, s, 3524 + i * 4);
        if (!o || !F(int, o, 1204))
            continue;
        float rp[3] = {F(float, o, 3372) - F(float, s, 552), F(float, o, 3376) - F(float, s, 556),
            F(float, o, 3380) - F(float, s, 560)};
        float dt = (float)F(double, g_6ef1fc, 2960);
        double vx = (double)F(float, o, 3300) - F(float, s, 432);
        float vy = F(float, o, 3304) - F(float, s, 436), vz = F(float, o, 3308) - F(float, s, 440);
        float rv[3] = {(float)(vx * dt), vy * dt, vz * dt};
        float a = CallC<float>(Cs_Len, rp);
        if (CallC<double>(Cs_Len, rv) + F(float, o, 3272) + F(float, s, 3288) > a)
            return 0;
    }
    return 1;
}

} // extern "C"

// === the cab view's needle and sway (FUN_5354d0)

extern "C" int __cdecl rand(void);
extern "C" void Cv_556650(); KING_ALIAS("_Cv_556650", "@Car_556650@8")
KING_VAR(float, g_cv64f380, 64f380)
KING_VAR(double, g_cv64b6d0, 64b6d0)
KING_VAR(double, g_cv64f378, 64f378)
KING_VAR(double, g_cv64f370, 64f370)
KING_VAR(double, g_cv64b6f0, 64b6f0)
KING_VAR(double, g_cv64ed90, 64ed90)
KING_VAR(double, g_cv64f368, 64f368)
KING_VAR(double, g_cv64f360, 64f360)
KING_VAR(double, g_cv64f358, 64f358)
KING_VAR(double, g_cv64f350, 64f350)
KING_VAR(double, g_cv64f348, 64f348)
KING_VAR(double, g_cv64f340, 64f340)
KING_VAR(float, g_cv64c2f8, 64c2f8)
KING_VAR(float, g_cv64d980, 64d980)
KING_VAR(float, g_cv64f338, 64f338)
KING_VAR(float, g_cv64c664, 64c664)
KING_VAR(float, g_cv64c32c, 64c32c)
KING_VAR(double, g_cv64f330, 64f330)
KING_VAR(double, g_cv64f328, 64f328)
KING_VAR(double, g_cv64f320, 64f320)
KING_VAR(float, g_cv64f31c, 64f31c)
KING_VAR(float, g_cv64f318, 64f318)
KING_VAR(float, g_cv64c258, 64c258)
KING_VAR(double, g_cv64b670, 64b670)
KING_VAR(Vec3, g_cv702650, 702650)

static int Cv_Clamp(int v, int lim)
{
    if (v < -lim)
        v = -lim;
    if (v > lim)
        v = lim;
    return v;
}

extern "C" {

// [FUN_5354d0] the cab view s: the speedometer needle frame (+1060, 0..38) from the car's speed (+20960, finer
// steps near zero), then (unless +1400 sets the view band instead) the view sway (+1376, +1380) following the cab's
// tilt (+13228, the speed +556650) at a rate limited by the frame time, shaken by the cab's four vibrations, kept
// within +1384/+1388, and the lean (+1368, +1372) by +1396
void __fastcall CabView_Update(BYTE* s)
{
    Vec3 v = g_cv702650;
    int shake = 0;
    BYTE* car = F(BYTE*, s, 616);
    if (car) {
        int r = (int)(F(float, car, 20960) * g_cv64f380);
        int sg = r < 0 ? -1 : 1;
        double a = x87_fabs((double)r);
        int n;
        if (!(a > g_cv64b6d0))
            n = r + 19;
        else if (!(a > g_cv64f378))
            n = (r - 5 * sg) / 2 + 5 * sg + 19;
        else if (!(a > g_cv64f370))
            n = 10 * sg + 19;
        else if (!(a > g_cv64b6f0))
            n = 11 * sg + 19;
        else if (!(a > g_cv64ed90))
            n = 12 * sg + 19;
        else if (!(a > g_cv64f368))
            n = 13 * sg + 19;
        else if (!(a > g_cv64f360))
            n = 14 * sg + 19;
        else if (!(a > g_cv64f358))
            n = 15 * sg + 19;
        else if (!(a > g_cv64f350))
            n = 16 * sg + 19;
        else if (!(a > g_cv64f348))
            n = 17 * sg + 19;
        else if (!(a > g_cv64f340))
            n = 18 * sg + 19;
        else
            n = 19 * sg + 19;
        F(int, s, 1060) = n;
        if (BYTE* cab = F(BYTE*, car, 21600)) {
            v = F(Vec3, cab, 13228);
            for (int i = 0; i < 4; i++)
                shake = (int)(F(float, cab, 15248 + i * 4) * g_cv64c2f8 + shake);
        }
    }
    if (F(int, s, 1060) < 0)
        F(int, s, 1060) = 0;
    if (F(int, s, 1060) >= 39)
        F(int, s, 1060) = 38;
    if (F(int, s, 1400)) {
        int h = g_ck696854, w = g_ck696850;
        int half = h / 2;
        int top = half - 190, bottom = half + 160;
        if (top < g_minTop672d7c)
            top = g_minTop672d7c;
        if (bottom > h)
            bottom = h;
        int rc[4] = {0, top, w, bottom};
        struct Rc16 { int r[4]; } band;
        memcpy(&band, rc, 16);
        CallVirt(F(BYTE*, s, 32), 44, band);
        return;
    }
    float dt = (float)Game_FrameTime();
    double az = x87_fabs(v.z);
    float ty = 0;
    int tx = 0;
    if (az > g_sl64b5a0) {
        tx = (int)(v.x / v.z * g_cv64d980);
        ty = (float)(v.y / v.z * g_cv64f338);
    }
    double f = CallThis<float>(Cv_556650, F(BYTE*, s, 616)) * (double)g_cv64c664 + ty;
    int cy = F(int, s, 1380);
    int ny;
    if (x87_fabs((double)cy) > x87_fabs(f))
        ny = (int)((cy + f) * g_cv64c258);
    else
        ny = (int)f;
    int lim = (int)(dt * g_cv64c32c);
    int dx = Cv_Clamp(tx - F(int, s, 1376), lim);
    int dy = Cv_Clamp(ny - cy, lim);
    F(int, s, 1376) += dx;
    F(int, s, 1380) = cy + dy;
    int w = g_ck696850, h = g_ck696854;
    if (shake) {
        double k = shake;
        F(int, s, 1376) = (int)((g_cv64b670 - rand() * g_cv64f330) * w * k * g_cv64f328 + F(int, s, 1376));
        F(int, s, 1380) = (int)((g_cv64b670 - rand() * g_cv64f330) * h * k * g_cv64f320 + F(int, s, 1380));
    }
    F(int, s, 1376) = Cv_Clamp(F(int, s, 1376), F(int, s, 1384));
    F(int, s, 1380) = Cv_Clamp(F(int, s, 1380), F(int, s, 1388));
    int lx = tx, ly = ny;
    int d = F(int, s, 1396);
    if (d < 0) {
        lx = ly = 0;
        if (az > g_sl64b5a0) {
            BYTE* cab = F(BYTE*, F(BYTE*, s, 616), 21600);
            lx = (int)(F(float, cab, 13232) / v.z * g_cv64c664);
            ly = (int)(F(float, cab, 13228) / v.z * g_cv64f338);
        }
    } else if (d > 0) {
        lx = ly = 0;
        if (az > g_sl64b5a0) {
            BYTE* cab = F(BYTE*, F(BYTE*, s, 616), 21600);
            lx = (int)(F(float, cab, 13232) / v.z * g_cv64f31c);
            ly = (int)(F(float, cab, 13228) / v.z * g_cv64f318);
        }
    }
    View_Place(s, 0);
    F(int, s, 1368) = Cv_Clamp(lx, F(int, s, 1384));
    F(int, s, 1372) = ly;
    if (ly < -F(int, s, 1388))
        F(int, s, 1372) = -F(int, s, 1388);
    if (F(int, s, 1372) > F(int, s, 1388))
        F(int, s, 1372) = F(int, s, 1388);
}

} // extern "C"

// === sparks from a car's exhausts (FUN_536100)

extern "C" void Sx_Multiply(); KING_ALIAS("_Sx_Multiply", "_Matrix_Multiply")
extern "C" void Sx_Burst(); KING_ALIAS("_Sx_Burst", "_Fx_SparkBurst")
KING_VAR(AsmLabel, g_sx697f9c, 697f9c)
KING_VAR(float, g_sx64c450, 64c450)
KING_VAR(double, g_sx64b780, 64b780)

// point p (three floats) placed by matrix w
static void Sx_Place(float* o, const float* w, const float* p)
{
    o[0] = (float)((double)w[6] * p[2] + (double)w[3] * p[1] + (double)w[0] * p[0] + w[9]);
    o[1] = (float)((double)w[7] * p[2] + (double)w[4] * p[1] + (double)w[1] * p[0] + w[10]);
    o[2] = (float)((double)w[2] * p[0] + (double)w[8] * p[2] + (double)w[5] * p[1] + w[11]);
}

extern "C" {

// [FUN_536100] sprays sparks with velocity v (scaled by 64c450, turned by the car's frame +3352, plus its velocity
// +3316) from car c's type exhaust points (type +12, and +36 when its z is below 64b780 in size)
void __cdecl Car_SparkBurst(BYTE* c, const float* v)
{
    if (!c)
        return;
    float m[12], w[12];
    memcpy(m, c + 708, 48);
    const float* n = (const float*)(c + 3352);
    CallC(Sx_Multiply, w, m, n);
    double vx = v[0] * (double)g_sx64c450, vy = v[1] * (double)g_sx64c450, vz = v[2] * (double)g_sx64c450;
    float qx = (float)(vz * n[6] + vy * n[3] + vx * n[0]);
    float qy = (float)(vz * n[7] + vy * n[4] + vx * n[1]);
    double qz = vz * n[8] + vy * n[5] + vx * n[2];
    float vel[3] = {qx + F(float, c, 3316), qy + F(float, c, 3320), (float)(qz + F(float, c, 3324))};
    const BYTE* ty = (const BYTE*)g_sx697f9c + F(int, c, 10420) * 2416;
    float pos[3];
    Sx_Place(pos, w, (const float*)(ty + 12));
    CallC(Sx_Burst, pos, vel, 1);
    if (!(x87_fabs(F(float, ty, 44)) < g_sx64b780))
        return;
    Sx_Place(pos, w, (const float*)(ty + 36));
    CallC(Sx_Burst, pos, vel, 1);
}

} // extern "C"

// === filling two tanks (FUN_556890)

KING_VAR(float, g_tk64c264, 64c264)
KING_VAR(float, g_tk64c250, 64c250)

extern "C" {

// [FUN_556890] pours *a into tank 1 (level +36 of +20) and *b into tank 2 (+24 of +16; first the buffer +32 is
// settled against it, by +28): both as far as the fuller tank allows, the rest left in *a and *b; returns which
// tank filled up (1, 2) or 0
int __fastcall Tanks_Fill(BYTE* e, void*, float* a, float* b)
{
    if (!(*a > g_tk64c264) || !(*b > g_tk64c264))
        return 0;
    if (*b > F(float, e, 28)) {
        F(float, e, 24) = F(float, e, 24) + F(float, e, 32);
        *b = *b - F(float, e, 32);
        F(float, e, 32) = 0;
    } else {
        F(float, e, 32) = F(float, e, 32) - *b;
        F(float, e, 24) = F(float, e, 24) + *b;
        *b = 0;
    }
    float r1 = (F(float, e, 20) - F(float, e, 36)) / *a;
    float r2 = (F(float, e, 16) - F(float, e, 24)) / *b;
    double t = g_tk64c250;
    int full = 0;
    if (r1 < g_tk64c250 || r2 < g_tk64c250) {
        if (r1 < r2)
            t = r1, full = 1;
        else
            t = r2, full = 2;
    }
    F(float, e, 36) = (float)(t * *a + F(float, e, 36));
    F(float, e, 24) = (float)(t * *b + F(float, e, 24));
    if (F(float, e, 36) > F(float, e, 20))
        F(float, e, 36) = F(float, e, 20);
    if (F(float, e, 24) > F(float, e, 16))
        F(float, e, 24) = F(float, e, 16);
    F(int, e, 40) = 0;
    *a = (float)(*a - t * *a);
    *b = (float)(*b - t * *b);
    return full;
}

} // extern "C"

// === the felt gravity in the cab (FUN_53b180)

extern "C" void Fg_Matrix2(); KING_ALIAS("_Fg_Matrix2", "@SceneObj_Matrix2@12")
extern "C" void Fg_NormalizeThis(); KING_ALIAS("_Fg_NormalizeThis", "@Vec3_NormalizeThis@8")
extern "C" void Fg_Normalized(); KING_ALIAS("_Fg_Normalized", "_Vec3_Normalized")
KING_VAR(float, g_fg702630, 702630)
KING_VAR(float, g_fg702634, 702634)
KING_VAR(float, g_fg702638, 702638)
KING_VAR(float, g_fg678790, 678790)
KING_VAR(float, g_fg678794, 678794)
KING_VAR(float, g_fg678798, 678798)
KING_VAR(float, g_fg64d998, 64d998)

extern "C" {

// [FUN_53b180] the down direction felt in the cab (678790): from the cab's tilt (+13228, when +13124) negated
// back, else gravity (702630) with x and y flipped (the car's acceleration is worked out but, as in the original,
// not used); gravity itself without a car or cab
void __fastcall Cab_FeltGravity(BYTE* s)
{
    float x = g_fg702630, y = g_fg702634, z = g_fg702638;
    BYTE* car = F(BYTE*, s, 616);
    if (!car || !F(BYTE*, car, 21600)) {
        g_fg678798 = z;
        g_fg678794 = y;
        g_fg678790 = x;
        return;
    }
    BYTE* cab = F(BYTE*, car, 21600);
    if (F(int, cab, 13124)) {
        float t[3];
        const float* n = CallC<const float*>(Fg_Normalized, t, cab + 13228);
        x = -n[0];
        y = -n[1];
        z = -n[2];
    } else {
        double dt = Game_FrameTime();
        float dx = F(float, car, 432) - F(float, car, 468), dy = F(float, car, 436) - F(float, car, 472);
        float dz = F(float, car, 440) - F(float, car, 476);
        float ax = (float)(dx / dt), ay = (float)(dy / dt);
        double az = dz / dt;
        double gx = (double)g_fg702630 * g_fg64d998;
        float gy = g_fg702634 * g_fg64d998, gz = g_fg702638 * g_fg64d998;
        float d[3] = {(float)(ax - gx), ay - gy, (float)(az - gz)};
        BYTE m[48];
        const float* M = CallThis<const float*>(Fg_Matrix2, car + 16, m);
        float e[3];
        e[0] = (float)((double)d[2] * M[6] + (double)d[1] * M[3] + (double)d[0] * M[0]);
        e[1] = (float)((double)d[2] * M[7] + (double)d[1] * M[4] + (double)d[0] * M[1]);
        e[2] = (float)((double)d[2] * M[8] + (double)d[1] * M[5] + (double)d[0] * M[2]);
        CallThis(Fg_NormalizeThis, e);
    }
    g_fg678798 = z;
    g_fg678790 = -x;
    g_fg678794 = -y;
}

} // extern "C"

// === a car's flasher light (FUN_549c00)

extern "C" void Fl_Update(); KING_ALIAS("_Fl_Update", "@Flasher_Update@16")
extern "C" void Fl_Release(); KING_ALIAS("_Fl_Release", "@Flasher_Release@8")
KING_VAR(float, g_fl6796c4, 6796c4)
KING_VAR(float, g_fl64c320, 64c320)

// m * p plus m's translation
static void Fl_Place(float* o, const float* m, const float* p)
{
    o[0] = (float)((double)m[6] * p[2] + (double)m[3] * p[1] + (double)m[0] * p[0] + m[9]);
    o[1] = (float)((double)m[7] * p[2] + (double)m[4] * p[1] + (double)m[1] * p[0] + m[10]);
    o[2] = (float)((double)m[8] * p[2] + (double)m[5] * p[1] + (double)m[2] * p[0] + m[11]);
}

extern "C" {

// [FUN_549c00] steps flasher f of car +24 (state record +20: state +12, elapsed +8): while on (1, 2) its light
// (+16, state 2) follows the car's type light point (+60), and after 6796c4 it goes out (3, the time +12, the light
// released); state 4 ends after 64c320; the elapsed time since the state's start (+4, +8, +12)
void __fastcall Flasher_StepLight(BYTE* f)
{
    float el = 0;
    BYTE* r = F(BYTE*, f, 20);
    if (F(int, r, 12) != 1 && F(int, r, 12) != 2)
        F(int, f, 0) = 0;
    else {
        el = (float)(GameTime() - F(float, f, 4));
        if (F(int, F(BYTE*, f, 20), 12) == 2 && F(BYTE*, f, 16)) {
            BYTE* car = F(BYTE*, f, 24);
            float m[12];
            memcpy(m, car + 708, 48);
            const float* p = (const float*)((BYTE*)g_697f9c + F(int, car, 10420) * 2416 + 60);
            float w[3], q[3];
            Fl_Place(w, m, p);
            const float* M = CallThis<const float*>(FUN_57de90, car + 16, m);
            Fl_Place(q, M, w);
            CallThis(Fl_Update, F(BYTE*, f, 16), q, 0xBF800000u);
        }
        if (el > g_fl6796c4) {
            F(int, F(BYTE*, f, 20), 12) = 3;
            F(float, f, 12) = (float)GameTime();
            if (BYTE* l = F(BYTE*, f, 16)) {
                CallThis(Fl_Release, l);
                operator delete(l);
            }
            F(BYTE*, f, 16) = 0;
        }
    }
    if (F(int, F(BYTE*, f, 20), 12) == 4) {
        el = (float)(GameTime() - F(float, f, 8));
        if (el > g_fl64c320)
            F(int, F(BYTE*, f, 20), 12) = 0;
    }
    int st = F(int, F(BYTE*, f, 20), 12);
    if (st == 1 || st == 2)
        el = (float)(GameTime() - F(float, f, 4));
    if (F(int, F(BYTE*, f, 20), 12) == 4)
        el = (float)(GameTime() - F(float, f, 8));
    if (F(int, F(BYTE*, f, 20), 12) == 3)
        el = (float)(GameTime() - F(float, f, 12));
    F(float, F(BYTE*, f, 20), 8) = el;
}

} // extern "C"

// === starting a flasher light (FUN_549960)

extern "C" void FUN_54a120(); KING_ALIAS("_FUN_54a120", "@Flasher_Construct@16")
KING_VAR(float, g_fl64f740, 64f740)
KING_VAR(float, g_fl64c250, 64c250)

extern "C" {

// [FUN_549960] starts flasher effect f (state 0 to 2, +28 = 1, its start +4; a car of type kind 3 gets the 549580
// effect): a new light (+16) at the car's type light point, as bright as sqrt(+3284 * 64f740) up to 1
void __fastcall CarEffect_StartFlasher(BYTE* f)
{
    if (F(int, F(BYTE*, f, 20), 12))
        return;
    BYTE* car = F(BYTE*, f, 24);
    if (F(int, (BYTE*)g_q129_6981b0, 34 + F(int, car, 10420) * 2416) == 3)
        CallC(FUN_4ba060, car, $L_549580);
    F(DWORD, f, 28) = 0x3F800000;
    F(int, F(BYTE*, f, 20), 12) = 2;
    F(float, f, 4) = (float)GameTime();
    car = F(BYTE*, f, 24);
    const float* p = (const float*)((BYTE*)g_697f9c + F(int, car, 10420) * 2416 + 60);
    float m[12];
    memcpy(m, car + 708, 48);
    float w[3], q[3];
    Fl_Place(w, m, p);
    const float* M = CallThis<const float*>(FUN_57de90, car + 16, m);
    Fl_Place(q, M, w);
    float k = (float)x87_sqrt(F(float, F(BYTE*, f, 24), 3284) * (double)g_fl64f740);
    if (k > g_fl64c250)
        k = 1.0f;
    BYTE* l = (BYTE*)operator new(16);
    F(BYTE*, f, 16) = l ? CallThis<BYTE*>(FUN_54a120, l, q, *(DWORD*)&k) : 0;
}

} // extern "C"

// === damage events of a car body (FUN_54d0b0)

extern "C" void Dh_DivComponents(); KING_ALIAS("_Dh_DivComponents", "_Vec3_DivComponents")
extern "C" void Dh_MovingOf(); KING_ALIAS("_Dh_MovingOf", "_Moving_Of")
KING_VAR(float, g_dh64c320, 64c320)
KING_VAR(float, g_dh64c970, 64c970)
KING_VAR(float, g_dh64c250, 64c250)
KING_VAR(float, g_dh64c264, 64c264)
KING_VAR(float, g_dh64c450, 64c450)
KING_VAR(double, g_dh64b5a0, 64b5a0)
KING_VAR(double, g_dh64d338, 64d338)
KING_VAR(double, g_dh64b670, 64b670)
KING_VAR(double, g_dh64b840, 64b840)

static void Dh_Copy(BYTE* d, const BYTE* v)
{
    memcpy(d, v, 12);
}

extern "C" {

// [FUN_54d0b0] for car s with a live body (+21600 at state 1): a hit (+908) of strength m from the change of its
// velocity (+432 against +468) and spin (+444 against the torque +480 over inertia +420) sets a hit slot at the
// contact +980 (front 0, its sides 1 and 2 by the contact's place, a scrape 3 when the contact moves along the
// normal +992 less than +904); the damage of the body's parts (+15336, over +412) slot 7; the hit mode +1428
// (1, 4, 5, 6) slot 8 or 9 with strength +1432; any of them sends the lamps
void __fastcall Car_HitEvents(BYTE* s)
{
    BYTE* body = F(BYTE*, s, 21600);
    if (!body || F(int, body, 13484) != 1)
        return;
    int any = 0;
    if (F(int, s, 908)) {
        float q[3], w[3];
        q[0] = (float)((double)F(float, s, 672) * F(float, s, 484) + (double)F(float, s, 684) * F(float, s, 488) +
            (double)F(float, s, 660) * F(float, s, 480));
        q[1] = (float)((double)F(float, s, 676) * F(float, s, 484) + (double)F(float, s, 688) * F(float, s, 488) +
            (double)F(float, s, 664) * F(float, s, 480));
        q[2] = (float)((double)F(float, s, 680) * F(float, s, 484) + (double)F(float, s, 692) * F(float, s, 488) +
            (double)F(float, s, 668) * F(float, s, 480));
        any = 1;
        CallC(Dh_DivComponents, w, q, s + 420);
        float r1[3] = {F(float, s, 432) - F(float, s, 468), F(float, s, 436) - F(float, s, 472),
            F(float, s, 440) - F(float, s, 476)};
        float r2[3] = {F(float, s, 444) - w[0], F(float, s, 448) - w[1], F(float, s, 452) - w[2]};
        float l1 = (float)CallC<double>(FUN_5ab4d0, r1);
        float m = (float)((CallC<double>(FUN_5ab4d0, r2) * g_dh64c320 + l1) * g_dh64c970);
        double mm;
        if (CallC<int>(Dh_MovingOf, F(int, s, 1044)))
            mm = m < g_dh64c250 ? x87_sqrt(x87_sqrt((double)m + m)) : m;
        else
            mm = m - g_dh64b5a0;
        if (mm < g_dh64d338)
            return;
        const float* c = (const float*)(s + 980);
        float px = (float)((double)F(float, s, 636) * c[2] + (double)F(float, s, 624) * c[1] +
            (double)c[0] * F(float, s, 612) + F(float, s, 648));
        float py = (float)((double)F(float, s, 616) * c[0] + (double)F(float, s, 640) * c[2] +
            (double)F(float, s, 628) * c[1] + F(float, s, 652));
        double pz = (double)F(float, s, 620) * c[0] + (double)F(float, s, 644) * c[2] + (double)F(float, s, 632) * c[1] +
            F(float, s, 656);
        float cx = (float)(pz * F(float, s, 448) - (double)py * F(float, s, 452));
        double cy = (double)px * F(float, s, 452) - pz * F(float, s, 444);
        float cz = (float)((double)py * F(float, s, 444) - (double)px * F(float, s, 448));
        float vx = cx + F(float, s, 432);
        double vy = cy + F(float, s, 436), vz = (double)cz + F(float, s, 440);
        float v[3];
        v[0] = (float)(vz * F(float, s, 540) + vy * F(float, s, 528) + (double)vx * F(float, s, 516));
        v[1] = (float)(vz * F(float, s, 544) + vy * F(float, s, 532) + (double)vx * F(float, s, 520));
        v[2] = (float)(vz * F(float, s, 548) + vy * F(float, s, 536) + (double)vx * F(float, s, 524));
        if (mm > g_dh64b670)
            mm = g_dh64c250;
        double dn = (double)F(float, s, 1000) * v[2] + (double)F(float, s, 996) * v[1] + (double)F(float, s, 992) * v[0];
        if (F(float, s, 904) * g_dh64b840 > x87_fabs(dn)) {
            F(int, s, 21620) = 1;
            F(DWORD, s, 21796) = 0x3F800000;
            Dh_Copy(s + 21688, s + 980);
        }
        double ax = x87_fabs(px);
        if (py > ax && l1 < g_dh64c320) {
            F(float, s, 21784) = (float)mm;
            Dh_Copy(s + 21652, s + 980);
            F(int, s, 21608) = 1;
        } else if (-ax > py) {
            F(float, s, 21792) = (float)mm;
            Dh_Copy(s + 21676, s + 980);
            F(int, s, 21616) = 1;
        } else {
            F(float, s, 21788) = (float)mm;
            Dh_Copy(s + 21664, s + 980);
            F(int, s, 21612) = 1;
        }
    }
    body = F(BYTE*, s, 21600);
    if (body && !F(int, body, 13124)) {
        int n = F(int, s, 10428), hit = 0;
        double acc = g_dh64c264;
        any = 1;
        if (n > 0) {
            double k = (double)F(float, s, 412) * g_dh64c450;
            for (int j = 0; j < n; j++) {
                int d = F(int, body, 15336 + j * 4);
                if (d > 0)
                    hit = 1;
                acc = d / k + acc;
            }
            if (hit) {
                if (acc > g_dh64c250)
                    acc = g_dh64c250;
                F(float, s, 21812) = (float)acc;
                F(int, s, 21636) = 1;
                Dh_Copy(s + 21736, s + 552);
            }
        }
    }
    int mode = F(int, s, 1428);
    if (mode == 1 || mode == 4 || mode == 5 || mode == 6) {
        int k = mode == 5 || mode == 6 ? 9 : 8;
        F(int, s, 21608 + k * 4) = 1;
        if (F(float, s, 1432) > g_dh64c250)
            F(DWORD, s, 21784 + k * 4) = 0x3F800000;
        else
            F(DWORD, s, 21784 + k * 4) = F(DWORD, s, 1432);
        Dh_Copy(s + 21652 + k * 12, s + 980);
        Car_SendLamps(s);
    } else if (any)
        Car_SendLamps(s);
}

} // extern "C"

// === the camera's eased view angle (FUN_4de550)

extern "C" void Fv_AddSample(); KING_ALIAS("_Fv_AddSample", "@History_Record@16")
extern "C" void Fv_FilterValue(); KING_ALIAS("_Fv_FilterValue", "@Filter_Value@4")
KING_VAR(float, g_fv696d10, 696d10)
KING_VAR(int, g_fv696d14, 696d14)
KING_VAR(float, g_fv6d1b98, 6d1b98)
KING_VAR(int, g_fv696c60, 696c60)
KING_VAR(float, g_fv672d88, 672d88)
KING_VAR(AsmLabel, s_fv672ed4, 672ed4)
KING_VAR(float, g_fv64c264, 64c264)
KING_VAR(float, g_fv64c250, 64c250)
KING_VAR(float, g_fv64d67c, 64d67c)
KING_VAR(float, g_fv64d678, 64d678)
KING_VAR(float, g_fv64d674, 64d674)
KING_VAR(float, g_fv64d670, 64d670)
KING_VAR(float, g_fv64d664, 64d664)
KING_VAR(float, g_fv64d660, 64d660)
KING_VAR(double, g_fv64b828, 64b828)
KING_VAR(double, g_fv64b6e0, 64b6e0)
KING_VAR(double, g_fv64b670, 64b670)
KING_VAR(double, g_fv64b5a0, 64b5a0)
KING_VAR(double, g_fv64d668, 64d668)

// an ease from 0 to 1 over x in 0..1: (sin((x - 0.5) * pi) + 1) / 2
static double Fv_Ease(double x)
{
    return (x87_sin((x - g_fv64b6e0) * g_fv64b828) + g_fv64b670) * g_fv64b6e0;
}

extern "C" {

// [FUN_4de550] eases the view angle 696d10 toward its target by the camera settings (696cb0: angles +5124 /
// +5136 / +5148 for zooming in, normal and out, eased over the switch times +5104 / +5116 / +5120) by the zoom
// direction (+1396 of the game), plus the player's body roll (filtered, when 696c60), within the in / out
// angles and by at most twice the frame's step; 0 without a game
float __cdecl Camera_ViewAngle()
{
    BYTE* gm = g_game;
    if (!gm)
        return g_fv64c264;
    float T = (float)Game_Time();
    BYTE* c = (BYTE*)g_696cb0;
    float x = (T - F(float, c, 5104)) * g_fv64d67c;
    float step = (float)(F(double, g_696ca0, 2960) * g_fv64b828);
    double k = g_fv64c250;
    if (x < g_fv64c250)
        k = Fv_Ease(x);
    if (g_fv696d14 && x87_fabs(g_fv6d1b98) < g_fv64b5a0)
        g_fv696d14 = 0;
    float target;
    int dir = F(int, gm, 1396);
    if (dir < 0) {
        target = (float)(((double)F(float, c, 5124) - F(float, c, 5136)) * k + F(float, c, 5136)) * g_fv672d88;
        double lim = step * g_fv64b6e0 + g_fv696d10;
        if (target < lim)
            target = (float)lim;
    } else if (dir > 0) {
        target = (float)(((double)F(float, c, 5148) - F(float, c, 5136)) * k + F(float, c, 5136)) * g_fv672d88;
        double lim = g_fv696d10 - step * g_fv64b6e0;
        if (!(target <= lim))
            target = (float)lim;
    } else {
        double a1 = F(float, c, 5108) * g_fv64d67c;
        if (a1 > g_fv64c250)
            a1 = g_fv64c250;
        double a2 = F(float, c, 5112) * g_fv64d67c;
        if (a2 > g_fv64c250)
            a2 = g_fv64c250;
        float u = (float)((T - F(float, c, 5120)) * g_fv64d678 + (g_fv64c250 - a1));
        float w = (float)((T - F(float, c, 5116)) * g_fv64d678 + (g_fv64c250 - a2));
        if (w > g_fv64c250 && u > g_fv64c250)
            target = F(float, c, 5136) * g_fv672d88;
        else if (u < g_fv64c250)
            target = g_fv672d88 * (float)((F(float, c, 5136) - F(float, c, 5148)) * Fv_Ease(u) + F(float, c, 5148));
        else
            target = g_fv672d88 * (float)((F(float, c, 5136) - F(float, c, 5124)) * Fv_Ease(w) + F(float, c, 5124));
    }
    BYTE* body;
    if (gm && F(BYTE*, gm, 616) && (body = F(BYTE*, F(BYTE*, gm, 616), 21600)) && g_fv696c60) {
        double v = -F(float, body, 13160);
        float r;
        if (v > g_fv64d674)
            r = (float)(v - g_fv64d674);
        else if (v < g_fv64d670)
            r = (float)(v + g_fv64d674);
        else
            r = 0;
        CallThis(Fv_AddSample, c + 4828, *(DWORD*)&r, s_fv672ed4);
        double f = CallThis<float>(Fv_FilterValue, (BYTE*)g_696cb0 + 4828) *
            (double)F(float, F(BYTE*, F(BYTE*, g_game, 616), 21600), 13104) * g_fv64d668;
        if (f > g_fv64d664)
            f = g_fv64d664;
        else if (f < g_fv64d660)
            f = g_fv64d660;
        target = (float)(f + target);
        c = (BYTE*)g_696cb0;
    }
    double hi = g_fv672d88 * F(float, c, 5124);
    if (target > hi)
        target = (float)hi;
    double lo = g_fv672d88 * F(float, c, 5148);
    if (target < lo)
        target = (float)lo;
    double d = target - g_fv696d10;
    step = step + step;
    if (d > step)
        d = step;
    else if (d < -step)
        d = -step;
    g_fv696d10 = (float)(d + g_fv696d10);
    return g_fv696d10;
}

} // extern "C"

// === dumping the scene objects to the log (FUN_53ece0)

extern "C" int __cdecl sprintf(char*, const char*, ...);
extern "C" void Do_WarnOpen(); KING_ALIAS("_Do_WarnOpen", "_WarnLog_Open")
extern "C" void Do_WarnClose(); KING_ALIAS("_Do_WarnClose", "_WarnLog_Close")
extern "C" void Do_Log(); KING_ALIAS("_Do_Log", "_VehLog_Value2")
extern "C" void Do_Matrix1(); KING_ALIAS("_Do_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Do_Matrix2(); KING_ALIAS("_Do_Matrix2", "@SceneObj_Matrix2@12")
extern "C" void Do_Get800(); KING_ALIAS("_Do_Get800", "@SceneObj_Get800@8")
extern "C" void Do_Matrix548(); KING_ALIAS("_Do_Matrix548", "@SceneObj_Matrix548@12")
KING_VAR(BYTE*, g_do6f653c, 6f653c)
KING_VAR(BYTE*, g_do6d2078, 6d2078)
KING_VAR(char, s_do67953c, 67953c)
KING_VAR(char, s_do679458, 679458)
KING_VAR(char, s_do679440, 679440)
KING_VAR(char, s_do67948c, 67948c)
KING_VAR(char, s_do679524, 679524)
KING_VAR(char, s_do679434, 679434)
KING_VAR(char, s_do67941c, 67941c)
KING_VAR(char, s_do679404, 679404)

// logs the three floats at v with format fmt
static void Do_Three(char* buf, const char* fmt, const float* v)
{
    sprintf(buf, fmt, (double)v[0], (double)v[1], (double)v[2]);
    CallC(Do_Log, buf);
}

// logs the 4x3 matrix m: its first row with fmt, the others with 67948c
static void Do_Matrix(char* buf, const char* fmt, const float* m)
{
    Do_Three(buf, fmt, m);
    Do_Three(buf, &s_do67948c, m + 3);
    Do_Three(buf, &s_do67948c, m + 6);
    Do_Three(buf, &s_do67948c, m + 9);
}

extern "C" {

// [FUN_53ece0] writes every scene object of the list 6f653c to the log: its name, state, two frames, its
// speed along its frame and its velocity and spin
void __cdecl SceneObjs_Dump()
{
    if (!g_696ca0 || !g_do6d2078)
        return;
    CallC(Do_WarnOpen);
    char buf[300];
    for (BYTE* n = g_do6f653c; n && n - 4; n = F(BYTE*, n - 4, 8)) {
        BYTE* p = n - 4;
        BYTE* o = p - 376;
        sprintf(buf, &s_do67953c, F(int, F(BYTE*, p, -56), 12), o, F(int, p, -360));
        CallC(Do_Log, buf);
        sprintf(buf, &s_do679458, F(int, CallThis<BYTE*>(Do_Get800, o), 12));
        CallC(Do_Log, buf);
        float t[12], m[12];
        memcpy(m, CallThis<float*>(Do_Matrix1, o, t), 48);
        Do_Matrix(buf, &s_do679440, m);
        memcpy(m, CallThis<float*>(Do_Matrix548, o, t), 48);
        Do_Matrix(buf, &s_do679524, m);
        float v[3] = {F(float, o, 416), F(float, o, 420), F(float, o, 424)};
        const float* M2 = CallThis<float*>(Do_Matrix2, o, t);
        sprintf(buf, &s_do679434, (double)v[0] * M2[1] + (double)v[1] * M2[4] + (double)v[2] * M2[7]);
        CallC(Do_Log, buf);
        Do_Three(buf, &s_do67941c, v);
        float w[3] = {F(float, o, 428), F(float, o, 432), F(float, o, 436)};
        Do_Three(buf, &s_do679404, w);
    }
    CallC(Do_WarnClose);
}

} // extern "C"

// === finding the car types' parts in the scene (FUN_53fdb0)

extern "C" int __cdecl sprintf(char*, const char*, ...);
extern "C" char* __cdecl strerror(int);
extern "C" void __cdecl Car_CopyTypeNames();
extern "C" void Cp_SaveTypes(); KING_ALIAS("_Cp_SaveTypes", "_CarTypes_Read")
extern "C" void Cp_LoadTypes(); KING_ALIAS("_Cp_LoadTypes", "_ObjTypes_Read")
extern "C" void Cp_FindOrFail(); KING_ALIAS("_Cp_FindOrFail", "_Scene_FindOrFail")
extern "C" void Cp_LocalChecked(); KING_ALIAS("_Cp_LocalChecked", "_Frame_GetLocalChecked")
extern "C" void Cp_Translation(); KING_ALIAS("_Cp_Translation", "_Matrix_Translation")
extern "C" void Cp_Inverse(); KING_ALIAS("_Cp_Inverse", "_Matrix_Inverse")
extern "C" void Cp_Local(); KING_ALIAS("_Cp_Local", "_Frame_GetLocal")
extern "C" void Cp_Find(); KING_ALIAS("_Cp_Find", "_Scene_FindObject")
extern "C" void Cp_ObjFind(); KING_ALIAS("_Cp_ObjFind", "_Objects_Find")
KING_VAR(int, g_cp697860, 697860)    // set: no type editor library
KING_VAR(int, g_cp6ce32c, 6ce32c)    // the number of car types
KING_VAR(int, g_cp6ce338, 6ce338)    // the default car type
KING_VAR(int, g_cp6cec68, 6cec68)    // fill missing types from the default
KING_VAR(BYTE, g_cp697868, 697868)   // the car types, 2416 bytes each
KING_VAR(BYTE, g_cp6d20f0, 6d20f0)   // the cars' scene parts, 1488 bytes each
KING_VAR(BYTE, g_cp698aea, 698aea)
KING_VAR(BYTE, g_cp6989e0, 6989e0)
KING_VAR(BYTE, g_cp698a00, 698a00)
KING_VAR(BYTE, g_cp698a0c, 698a0c)
KING_VAR(BYTE, g_cp698a1c, 698a1c)
KING_VAR(BYTE, g_cp696bc8, 696bc8)
KING_VAR(float, g_cp702630, 702630)
KING_VAR(float, g_cp702634, 702634)
KING_VAR(float, g_cp702638, 702638)
KING_VAR(float, g_cp702640, 702640)
KING_VAR(float, g_cp702644, 702644)
KING_VAR(float, g_cp702648, 702648)
KING_VAR(BYTE, g_cp702650, 702650)
KING_VAR(BYTE, g_cp7025f0, 7025f0)
KING_VAR(float, g_cp64c264, 64c264)
KING_VAR(float, g_cp64c258, 64c258)
KING_VAR(double, g_cp64ca78, 64ca78)
KING_VAR(double, g_cp64b5b8, 64b5b8)
KING_VAR(BYTE*, g_cp6ef208, 6ef208)
KING_VAR(BYTE*, g_cp6ef204, 6ef204)
KING_VAR(int, g_cp6ef1f0, 6ef1f0)
KING_VAR(BYTE*, g_cp6ef254, 6ef254)
KING_VAR(char, s_cp67531c, 67531c)
KING_VAR(char, s_cp675308, 675308)
KING_VAR(char, s_cp6752f8, 6752f8)
KING_VAR(char, s_cp6752d0, 6752d0)
KING_VAR(char, s_cp679c28, 679c28)
KING_VAR(char, s_cp679c18, 679c18)
KING_VAR(char, s_cp679be4, 679be4)
KING_VAR(char, s_cp679bd0, 679bd0)
KING_VAR(char, s_cp679b84, 679b84)
KING_VAR(char, s_cp674da0, 674da0)
KING_VAR(char, s_cp679b78, 679b78)
KING_VAR(char, s_cp679b40, 679b40)
KING_VAR(char, s_cp679b08, 679b08)
KING_VAR(char, s_cp679ac4, 679ac4)
KING_VAR(char, s_cp679a80, 679a80)
KING_VAR(char, s_cp679a3c, 679a3c)
KING_VAR(char, s_cp679a2c, 679a2c)
KING_VAR(char, s_cp679a20, 679a20)
KING_VAR(char, s_cp679a18, 679a18)
KING_VAR(char, s_cp679a10, 679a10)
KING_VAR(char, s_cp679a04, 679a04)
KING_VAR(char, s_cp6799f4, 6799f4)
KING_VAR(char, s_cp6799e4, 6799e4)
KING_VAR(char, s_cp6799d4, 6799d4)
KING_VAR(char, s_cp6799c8, 6799c8)
KING_VAR(char, s_cp6799b8, 6799b8)
KING_VAR(char, s_cp6799ac, 6799ac)
KING_VAR(char, s_cp6799a0, 6799a0)
KING_VAR(char, s_cp679994, 679994)
KING_VAR(char, s_cp679988, 679988)
KING_VAR(char, s_cp679978, 679978)
KING_VAR(char, s_cp679968, 679968)
KING_VAR(char, s_cp678460, 678460)
KING_VAR(char, s_cp678450, 678450)
KING_VAR(char, s_cp67995c, 67995c)
KING_VAR(char, s_cp679948, 679948)
KING_VAR(char, s_cp679940, 679940)
KING_VAR(char, s_cp679938, 679938)
KING_VAR(char, s_cp679930, 679930)
KING_VAR(char, s_cp679928, 679928)
KING_VAR(char, s_cp679920, 679920)
KING_VAR(char, s_cp679914, 679914)
KING_VAR(char, s_cp679908, 679908)
KING_VAR(char, s_cp672e38, 672e38)
KING_VAR(char, s_cp6798e4, 6798e4)
KING_VAR(char, s_cp6798d4, 6798d4)
KING_VAR(char, s_cp6798c4, 6798c4)
KING_VAR(char, s_cp6798b0, 6798b0)
KING_VAR(char, s_cp678474, 678474)
KING_VAR(char, s_cp678468, 678468)
KING_VAR(char, s_cp672a18, 672a18)
KING_VAR(char, s_cp672a24, 672a24)
KING_VAR(char, s_cp6798a4, 6798a4)
KING_VAR(char, s_cp679894, 679894)
KING_VAR(char, s_cp679884, 679884)
KING_VAR(char, s_cp679874, 679874)
KING_VAR(char, s_cp679864, 679864)
KING_VAR(char, s_cp67985c, 67985c)
KING_VAR(char, s_cp679850, 679850)
KING_VAR(char, s_cp679844, 679844)
KING_VAR(char, s_cp679838, 679838)
KING_VAR(char, s_cp67982c, 67982c)
KING_VAR(char, s_cp679820, 679820)
KING_VAR(char, s_cp679814, 679814)
KING_VAR(char, s_cp679800, 679800)
KING_VAR(char, s_cp6797c0, 6797c0)
KING_VAR(char, s_cp672f78, 672f78)
KING_VAR(char, s_cp6797b4, 6797b4)
KING_VAR(char, s_cp6797a8, 6797a8)
KING_VAR(char, s_cp67979c, 67979c)
KING_VAR(char, s_cp679790, 679790)
KING_VAR(char, s_cp679784, 679784)
KING_VAR(char, s_cp6731ec, 6731ec)
KING_VAR(char, s_cp673298, 673298)
KING_VAR(char, s_cp673288, 673288)
KING_VAR(char, s_cp679774, 679774)
KING_VAR(char, s_cp679768, 679768)
KING_VAR(char, s_cp67975c, 67975c)
KING_VAR(char, s_cp679748, 679748)
KING_VAR(char, s_cp679734, 679734)
KING_VAR(char, s_cp679720, 679720)
KING_VAR(char, s_cp679718, 679718)
KING_VAR(char, s_cp679710, 679710)
KING_VAR(char, s_cp679708, 679708)
KING_VAR(char, s_cp679700, 679700)
KING_VAR(char, s_cp6796f8, 6796f8)
KING_VAR(char, s_cp6796f0, 6796f0)

static BYTE* Cp_F(BYTE* parent, const char* name)
{
    return CallC<BYTE*>(Cp_Find, parent, name);
}

static void Cp_GetLocal(BYTE* o, void* m)
{
    CallC(Cp_Local, o, m);
}

// part name of the car into *slot, its frame into m (identity when missing)
static void Cp_Part(BYTE* C, int slot, int frame, char* buf, const char* fmt, const char* name)
{
    sprintf(buf, fmt, name);
    BYTE* o = Cp_F(0, buf);
    F(BYTE*, C, slot) = o;
    if (o)
        Cp_GetLocal(o, C + frame);
    else
        memcpy(C + frame, &g_cp7025f0, 48);
}

// the place of point object fmt (name, k) into *slot and its position into pos (zero when missing)
static void Cp_Point(BYTE* C, int slot, BYTE* pos, char* buf, const char* fmt, const char* name, int k, float* m)
{
    sprintf(buf, fmt, name, k);
    BYTE* o = Cp_F(0, buf);
    F(BYTE*, C, slot) = o;
    if (o) {
        Cp_GetLocal(o, m);
        memcpy(pos, m + 9, 12);
    } else
        memcpy(pos, &g_cp702650, 12);
}

static void Cp_Fail(const char* text)
{
    World_Fatal(text);
}

extern "C" {

// [FUN_53fdb0] for every car type finds its parts in the scene: the body, the wheels (up to 8; their frames give
// the wheel positions and the height range of the body), lights, doors, mirrors, cameras, exhausts and more,
// checks the type's heights against the wheels (fatal when wrong), and, with the type editor library loaded,
// writes the wheel points to the type's ini; then the world's shared parts
void __cdecl CarTypes_FindParts()
{
    BYTE* lib = 0;
    typedef void (__cdecl* EditFn)(BYTE*, int, const char*, const char*);
    EditFn edit = 0;
    if (!g_cp697860) {
        lib = (BYTE*)LoadLibraryA(&s_cp67531c);
        if (!lib)
            Cp_Fail(&s_cp675308);
        else {
            edit = (EditFn)GetProcAddress((HMODULE)lib, &s_cp6752f8);
            if (!edit)
                Cp_Fail(&s_cp6752d0);
        }
    }
    char buf[300], name[164], err1[200], err2[200], key[100], val[100], key2[100], err3[300], err4[300];
    char name3[100], pname[100], pname2[100], pname3[100];
    float tmp[12], m[12];
    for (int i = 0; i < g_cp6ce32c; i++) {
        BYTE* T = &g_cp697868 + i * 2416;
        BYTE* C = &g_cp6d20f0 + i * 1488;
        const char* tname = (const char*)(T + 2302);
        strcpy(name, tname);
        const char* sname = (const char*)(T + 2342);
        BYTE* body = Cp_F(0, sname);
        F(BYTE*, C, 0) = body;
        if (!body) {
            sprintf(buf, &s_cp679c28, i + 1, sname);
            Cp_Fail(buf);
            continue;
        }
        int& total = F(int, T, 2148);
        int& count = F(int, T, 2144);
        int kind = F(int, T, 2372);
        total = 0;
        count = 0;
        float lo = 100.0f, hi = -100.0f;
        bool bad = false;
        for (char j = 0; j < 8; j++) {
            sprintf(buf, &s_cp679c18, name, (int)j);
            BYTE* o = Cp_F(0, buf);
            F(BYTE*, C, 4 + j * 4) = o;
            if (o && total == count) {
                Cp_GetLocal(o, C + 272 + j * 48);
                float v = (float)(int)F(float, C, 312 + j * 48);
                if (lo > v)
                    lo = v;
                if (hi < v)
                    hi = v;
                count++;
            } else {
                if (j < 2 && F(int, T, 2372) == 3) {
                    sprintf(err1, &s_cp679be4, buf);
                    Cp_Fail(err1);
                }
                int k = F(int, T, 2372);
                if (j < 4 && (k == 4 || k == 2 || k == 1 || k == 0)) {
                    sprintf(err2, &s_cp679be4, buf);
                    Cp_Fail(err2);
                }
                sprintf(buf, &s_cp679bd0, name, (int)j);
                o = Cp_F(0, buf);
                F(BYTE*, C, 4 + j * 4) = o;
                if (!o)
                    break;
                Cp_GetLocal(o, C + 272 + j * 48);
            }
            total++;
        }
        if (F(float, T, 276) == g_cp64c264) {
            sprintf(buf, &s_cp679b84, sname);
            Cp_Fail(buf);
            continue;
        }
        const float sx = g_cp702640, sy = g_cp702644, sz = g_cp702648;
        if (F(int, T, 860)) {
            memcpy(C + 704, T, 48);
            memcpy(C + 752, CallC<void*>(Cp_Inverse, tmp, C + 704), 48);
            if (!F(int, T, 712)) {
                for (char j = 0; j < total; j++) {
                    float off[3];
                    if (j == 0) {
                        float w = F(float, T, 184);
                        off[0] = -sx * w, off[1] = w * -sy, off[2] = w * -sz;
                    } else if (j == 1) {
                        float w = F(float, T, 184);
                        off[0] = w * sx, off[1] = w * sy, off[2] = w * sz;
                    } else if (j & 1) {
                        float w = F(float, T, 188);
                        off[0] = w * sx, off[1] = w * sy, off[2] = w * sz;
                    } else {
                        float w = F(float, T, 188);
                        off[0] = w * -sx, off[1] = w * -sy, off[2] = w * -sz;
                    }
                    const float* t = (const float*)(C + 308 + j * 48);
                    float p[3] = {off[0] + t[0], off[1] + t[1], off[2] + t[2]};
                    BYTE* dst = j < count ? T + 424 + j * 12 : T + 520 + (j - count) * 12;
                    memcpy(dst, p, 12);
                }
                if (count < 8) {
                    memcpy(T + 424 + count * 12, T + 2012, 12);
                    F(float, T, 432 + count * 12) = (float)(F(float, T, 432) - g_cp64ca78);
                }
            }
        } else {
            float w = F(float, T, 184);
            float ax = sx * w, ay = sy * w;
            double az = (double)w * sz;
            const float* P = (const float*)(T + 424);
            float hx = ax * g_cp64c258, hy = ay * g_cp64c258;
            float qx = F(float, C, 308) - hx, qy = F(float, C, 312) - hy;
            double qz = F(float, C, 316) - az * g_cp64c258;
            float rx = qx - P[0], ry = qy - P[1], rz = (float)(qz - P[2]);
            memcpy(C + 704, CallC<void*>(Cp_Translation, tmp, *(DWORD*)&rx, *(DWORD*)&ry, *(DWORD*)&rz), 48);
            memcpy(C + 752, CallC<void*>(Cp_Inverse, m, C + 704), 48);
        }
        if (edit) {
            const char* ini = (const char*)(T + 2212);
            const char* sec = (const char*)(T + 2272);
            SetFileAttributesA(ini, FILE_ATTRIBUTE_NORMAL);
            int k = 0;
            for (char j = 0; j < count; j++) {
                if (j >= 2 && j < count - 2)
                    continue;
                float h = F(float, T, 100);
                const float* P = (const float*)(T + 424 + j * 12);
                float* d = (float*)(T + 880 + F(int, T, 872) * 12);
                d[0] = P[0] - g_cp702630 * h, d[1] = P[1] - g_cp702634 * h;
                d[2] = (float)(P[2] - (double)h * g_cp702638);
                sprintf(key, &s_cp674da0, k);
                sprintf(val, &s_cp679b78, (double)d[0], (double)d[1], (double)d[2]);
                k++;
                if (!WritePrivateProfileStringA(sec, key, val, ini)) {
                    sprintf(err3, &s_cp679b40, ini, sec, strerror(GetLastError()));
                    Cp_Fail(err3);
                }
                F(int, T, 872)++;
            }
            for (; k < 8; k++) {
                sprintf(key2, &s_cp674da0, k);
                if (!WritePrivateProfileStringA(sec, key2, 0, ini)) {
                    sprintf(err4, &s_cp679b08, sec, ini, strerror(GetLastError()));
                    Cp_Fail(err4);
                }
            }
            edit(T + 880, F(int, T, 872), ini, sec);
        }
        float d = (float)((hi - lo) * g_cp64b5b8);
        float a = F(float, T, 40), b = F(float, T, 292);
        float lo2 = d + lo, hi2 = hi - d;
        if (kind != 3) {
            bool msgB = false, msgA = false;
            if (a < lo2)
                msgA = true;
            else if (a > hi2 || b < lo || b > hi) {
                if (a > hi2 || kind == 1 || kind == 4) {
                    if (a > hi2)
                        msgA = true;
                    else
                        msgB = true;
                }
            }
            if (msgA || msgB) {
                if (msgA)
                    sprintf(buf, &s_cp679a80, name, (double)a, (double)hi2, (double)lo2);
                else
                    sprintf(buf, &s_cp679ac4, name, (double)b, (double)hi, (double)lo);
                Cp_Fail(buf);
            }
        }
        if (F(int, T, 2372) == 3) {
            float c = F(float, T, 2016);
            if (a < lo2 || a > c) {
                sprintf(buf, &s_cp679a80, name, (double)a, (double)c, (double)lo);
                Cp_Fail(buf);
            } else if (b < lo || b > c) {
                sprintf(buf, &s_cp679a3c, name, (double)b, (double)c, (double)lo);
                Cp_Fail(buf);
            }
        }
        memcpy(C + 800, T + 848, 12);
        kind = F(int, T, 2372);
        F(int, C, 40) = 0;
        BYTE* o = 0;
        if (kind != 3 && kind != 4) {
            sprintf(buf, &s_cp679a2c, tname);
            o = CallC<BYTE*>(Cp_FindOrFail, 0, buf, 0);
        }
        F(BYTE*, C, 36) = o;
        if (o)
            Cp_GetLocal(o, C + 656);
        else
            memcpy(C + 656, &g_cp7025f0, 48);
        F(int, C, 140) = 0, F(int, C, 172) = 0, F(int, C, 204) = 0, F(int, C, 208) = 0, F(int, C, 212) = 0;
        F(int, C, 144) = 0, F(int, C, 176) = 0, F(int, C, 216) = 0, F(int, C, 220) = 0, F(int, C, 224) = 0;
        kind = F(int, T, 2372);
        if (kind != 3 && kind != 4) {
            sprintf(buf, &s_cp679a20, name);
            BYTE* p = CallC<BYTE*>(Cp_FindOrFail, 0, buf, 0);
            F(BYTE*, C, 148) = Cp_F(p, &s_cp679a18);
            F(BYTE*, C, 152) = Cp_F(p, &s_cp679a10);
        }
        sprintf(buf, &s_cp679a04, name);
        BYTE* p = Cp_F(0, buf);
        F(BYTE*, C, 44) = p ? Cp_F(p, &s_cp6799f4) : 0;
        F(BYTE*, C, 60) = p ? Cp_F(p, &s_cp6799e4) : 0;
        F(BYTE*, C, 180) = p ? Cp_F(p, &s_cp6799d4) : 0;
        F(BYTE*, C, 188) = p ? Cp_F(p, &s_cp6799c8) : 0;
        F(BYTE*, C, 192) = p ? Cp_F(p, &s_cp6799b8) : 0;
        F(BYTE*, C, 228) = p ? Cp_F(p, &s_cp6799ac) : 0;
        F(BYTE*, C, 236) = p ? Cp_F(p, &s_cp6799a0) : 0;
        F(BYTE*, C, 240) = p ? Cp_F(p, &s_cp679994) : 0;
        sprintf(buf, &s_cp679988, name);
        p = Cp_F(0, buf);
        F(BYTE*, C, 48) = p ? Cp_F(p, &s_cp679978) : 0;
        F(BYTE*, C, 64) = p ? Cp_F(p, &s_cp679968) : 0;
        F(BYTE*, C, 184) = p ? Cp_F(p, &s_cp6799d4) : 0;
        F(BYTE*, C, 196) = p ? Cp_F(p, &s_cp6799c8) : 0;
        F(BYTE*, C, 200) = p ? Cp_F(p, &s_cp6799b8) : 0;
        F(BYTE*, C, 232) = p ? Cp_F(p, &s_cp6799ac) : 0;
        F(BYTE*, C, 244) = p ? Cp_F(p, &s_cp6799a0) : 0;
        F(BYTE*, C, 248) = p ? Cp_F(p, &s_cp679994) : 0;
        memcpy(m, &g_cp7025f0, 48);
        sprintf(buf, &s_cp678460, name);
        BYTE* g = CallC<BYTE*>(Cp_FindOrFail, 0, buf, name);
        F(BYTE*, C, 68) = g;
        F(BYTE*, C, 72) = CallC<BYTE*>(Cp_FindOrFail, g, &s_cp678450, name);
        for (char j = 0; j < total; j++) {
            sprintf(buf, &s_cp67995c, name, (int)j);
            sprintf(pname, &s_cp679948, (int)j);
            F(int, C, 76 + j * 4) = 0;
            F(BYTE*, C, 108 + j * 4) = CallC<BYTE*>(Cp_FindOrFail, 0, pname, name);
        }
        static const char* const lamps[4] = {&s_cp679940, &s_cp679938, &s_cp679930, &s_cp679928};
        for (int k = 0; k < 4; k++) {
            strcpy(buf, lamps[k]);
            F(BYTE*, C, 156 + k * 4) = Cp_F(0, buf);
        }
        F(BYTE*, C, 260) = Cp_F(F(BYTE*, C, 0), &s_cp679920);
        F(BYTE*, C, 252) = Cp_F(F(BYTE*, C, 0), &s_cp679914);
        F(BYTE*, C, 256) = Cp_F(F(BYTE*, C, 0), &s_cp679908);
        char* name2 = (char*)(T + 2322);
        sprintf(buf, &s_cp672e38, name2);
        kind = F(int, T, 2372);
        if (kind != 3 && kind != 4) {
            BYTE* sh = Cp_F(0, buf);
            F(BYTE*, C, 812) = sh;
            if (!sh) {
                strcpy(name2, (const char*)(&g_cp698aea + g_cp6ce338 * 2416));
                sprintf(name3, &s_cp672e38, name2);
                sh = Cp_F(0, name3);
                F(BYTE*, C, 812) = sh;
                if (g_cp6cec68) {
                    if (!sh) {
                        sprintf(buf, &s_cp6798e4, name3);
                        Cp_Fail(buf);
                    }
                    if (g_cp6cec68) {
                        int base = g_cp6ce338 * 2416;
                        memcpy(T + 2100, &g_cp698a0c + base, 16);
                        memcpy(T + 2056, &g_cp6989e0 + base, 20);
                        for (int q = 0; q < 3; q++) {
                            F(int, T, 2076 + q * 4) = F(int, &g_cp698a00 + base, -12 + q * 4);
                            F(int, T, 2088 + q * 4) = F(int, &g_cp698a00 + base, q * 4);
                        }
                        memcpy(T + 2116, &g_cp698a1c + base, 28);
                    }
                }
            }
            sprintf(buf, &s_cp6798d4, name2);
            F(BYTE*, C, 816) = Cp_F(0, buf);
            sprintf(buf, &s_cp6798c4, name2);
            F(BYTE*, C, 820) = Cp_F(0, buf);
            sprintf(buf, &s_cp6798b0, name2);
            F(BYTE*, C, 824) = Cp_F(0, buf);
            static const int frames[3] = {828, 876, 924};
            for (int q = 0; q < 3; q++) {
                BYTE* f = F(BYTE*, C, 816 + q * 4);
                if (f) {
                    CallC(Cp_LocalChecked, f, C + frames[q]);
                    F(int, C, 972 + q * 4) = 1;
                } else {
                    memcpy(C + frames[q], &g_cp7025f0, 48);
                    F(int, C, 972 + q * 4) = 0;
                }
            }
        }
        sprintf(buf, &s_cp678474, tname);
        F(BYTE*, C, 984) = CallC<BYTE*>(Cp_ObjFind, buf);
        F(int, C, 1008) = 0;
        if (!F(BYTE*, C, 984)) {
            for (int k = 1; k <= 6; k++) {
                sprintf(buf, &s_cp678468, tname, k);
                F(BYTE*, C, 984 + (k - 1) * 4) = CallC<BYTE*>(Cp_ObjFind, buf);
                F(int, C, 1008 + (k - 1) * 4) = 0;
            }
        } else
            for (int k = 0; k < 5; k++) {
                F(int, C, 988 + k * 4) = 0;
                F(int, C, 1012 + k * 4) = 0;
            }
        F(BYTE*, C, 1064) = Cp_F(F(BYTE*, C, 0), &s_cp672a18);
        F(BYTE*, C, 1068) = Cp_F(F(BYTE*, C, 0), &s_cp672a24);
        F(BYTE*, C, 264) = Cp_F(F(BYTE*, C, 0), &s_cp6798a4);
        for (int q = 0; q < 3; q++) {
            Cp_Point(C, 1032 + q * 4, C + 1076 + q * 12, pname2, &s_cp679894, tname, q + 1, m);
            Cp_Point(C, 1044 + q * 4, C + 1112 + q * 12, pname3, &s_cp679884, tname, q + 1, m);
        }
        sprintf(buf, &s_cp679874, tname);
        BYTE* e = Cp_F(0, buf);
        F(BYTE*, C, 1056) = e;
        if (e) {
            Cp_GetLocal(e, m);
            memcpy(C + 1148, m + 9, 12);
        } else
            memcpy(C + 1148, &g_cp702650, 12);
        sprintf(buf, &s_cp679864, tname);
        e = Cp_F(0, buf);
        F(BYTE*, C, 1060) = e;
        if (e) {
            Cp_GetLocal(e, m);
            memcpy(C + 1160, m + 9, 12);
        } else
            memcpy(C + 1160, &g_cp702650, 12);
        F(BYTE*, C, 1072) = Cp_F(0, &s_cp67985c);
        Cp_Part(C, 1172, 1196, buf, &s_cp679850, tname);
        Cp_Part(C, 1176, 1244, buf, &s_cp679844, tname);
        sprintf(buf, &s_cp679850, tname);
        Cp_Part(C, 1180, 1292, buf, &s_cp679838, tname);
        Cp_Part(C, 1184, 1340, buf, &s_cp67982c, tname);
        Cp_Part(C, 1188, 1388, buf, &s_cp679820, tname);
        Cp_Part(C, 1192, 1436, buf, &s_cp679814, tname);
        F(BYTE*, C, 1484) = Cp_F(F(BYTE*, C, 0), &s_cp679800);
    }
    if (lib)
        FreeLibrary((HMODULE)lib);
    if (!g_cp697860) {
        Sleep(12000);
        CallC(Cp_SaveTypes, &g_cp696bc8, 1);
        Car_CopyTypeNames();
        CallC(Cp_LoadTypes, &g_cp696bc8, 0);
        Cp_Fail(&s_cp6797c0);
    }
    g_cp6ef208 = Cp_F(0, &s_cp672f78);
    g_cp6ef204 = CallC<BYTE*>(Cp_ObjFind, &s_cp6797b4);
    g_cp6ef1f0 = 0;
    BYTE** w = &g_cp6ef254;
    w[0] = Cp_F(0, &s_cp6797a8);
    w[1] = Cp_F(0, &s_cp67979c);
    w[2] = Cp_F(0, &s_cp679790);
    w[3] = CallC<BYTE*>(Cp_ObjFind, &s_cp679784);
    w[4] = CallC<BYTE*>(Cp_ObjFind, &s_cp6731ec);
    w[5] = CallC<BYTE*>(Cp_ObjFind, &s_cp673298);
    w[6] = CallC<BYTE*>(Cp_ObjFind, &s_cp673288);
    w[7] = CallC<BYTE*>(Cp_ObjFind, &s_cp679774);
    w[8] = Cp_F(0, &s_cp679768);
    w[9] = Cp_F(0, &s_cp67975c);
    w[10] = CallC<BYTE*>(Cp_ObjFind, &s_cp679748);
    w[11] = CallC<BYTE*>(Cp_ObjFind, &s_cp679734);
    w[12] = CallC<BYTE*>(Cp_ObjFind, &s_cp679720);
    w[13] = Cp_F(0, &s_cp679718);
    w[14] = Cp_F(0, &s_cp679710);
    w[17] = Cp_F(0, &s_cp679708);
    w[18] = Cp_F(0, &s_cp679700);
    w[15] = Cp_F(0, &s_cp6796f8);
    w[16] = Cp_F(0, &s_cp6796f0);
}

} // extern "C"

// === the vehicle's state reset (FUN_5427a0, called by Car_Register)

extern "C" void FUN_57dd70(); KING_ALIAS("_FUN_57dd70", "@SceneObj_InitState@24")
extern "C" void FUN_58ce80(); KING_ALIAS("_FUN_58ce80", "@Points_Add@20")
extern "C" void FUN_58b550(); KING_ALIAS("_FUN_58b550", "@Plane_SetWithPoint@12")
extern "C" void FUN_58d290(); KING_ALIAS("_FUN_58d290", "@Records_58d290@8")
extern "C" void FUN_4eb580(); KING_ALIAS("_FUN_4eb580", "@Ramp_SetRange@24")
extern "C" void FUN_5498b0(); KING_ALIAS("_FUN_5498b0", "@Obj5498b0_Init@12")
extern "C" void FUN_541e80(); KING_ALIAS("_FUN_541e80", "@Vehicle_PackState@4")
extern "C" void FUN_542450(); KING_ALIAS("_FUN_542450", "@Mass_Shares@12")
extern "C" void FUN_5465c0(); KING_ALIAS("_FUN_5465c0", "_Vehicle_MakeDamageKeys")
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_542580(); KING_ALIAS("_FUN_542580", "@Car_SetInertia@8")
KING_VAR(AsmLabel, g_cr6d20f4, 6d20f4)            // per car type, 1488 bytes each
KING_VAR(int, g_cr6ef1f0, 6ef1f0)
KING_VAR(BYTE*, g_cr6ef204, 6ef204)
KING_VAR(float, g_cr64d7b8, 64d7b8)
KING_VAR(float, g_cr6796bc, 6796bc)
KING_VAR(float, g_cr64c254, 64c254)
KING_VAR(float, g_cr64f510, 64f510)
KING_VAR(float, g_cr64daac, 64daac)
KING_VAR(char, s_cr679ef0, 679ef0)
KING_VAR(char, s_cr679790, 679790)
KING_VAR(char, s_cr67975c, 67975c)
KING_VAR(char, s_cr679edc, 679edc)
KING_VAR(char, s_cr679ec4, 679ec4)
KING_VAR(char, s_cr679ebc, 679ebc)
KING_VAR(char, s_cr679eac, 679eac)
KING_VAR(char, s_cr679ea4, 679ea4)
KING_VAR(char, s_cr679ea0, 679ea0)

// out = m (rotation +0, position +36) applied to p, summed in the original's order
static void Cr_Place(const float* m, const float* p, float* out)
{
    out[0] = (float)((double)m[6] * p[2] + (double)m[3] * p[1] + (double)p[0] * m[0] + m[9]);
    out[1] = (float)((double)m[7] * p[2] + (double)m[1] * p[0] + (double)m[4] * p[1] + m[10]);
    out[2] = (float)((double)m[8] * p[2] + (double)m[2] * p[0] + (double)m[5] * p[1] + m[11]);
}

static void Cr_Copy3(BYTE* to, const void* from) { memcpy(to, from, 12); }

// the two ends (0 and 1) of the ramp at r
static void Cr_Ramps(BYTE* r, float b0, float c0, float d0, float b1, float c1, float d1)
{
    CallThis(FUN_4eb580, r, 0, b0, c0, d0);
    CallThis(FUN_4eb580, r, 1, b1, c1, d1);
}

extern "C" {

// [FUN_5427a0] resets the vehicle from its type (+10420; B = 6978c8 table, 2416 bytes, C = 6d20f4 table, 1488
// bytes): scene state, wheels (+10424) and their frames, body points and the height range, the axle points,
// planes, scene objects, damage keys, timers, the ramps, the 50 trail records and the type names
void __fastcall Car_Reset(BYTE* s, void*)
{
    int idx = F(int, s, 10420);
    BYTE* B = (BYTE*)g_carTypes697914 - 76 + idx * 2416;
    BYTE* C = (BYTE*)g_cr6d20f4 + idx * 1488;
    F(int, s, 9544) = 0;
    F(int, s, 20384) = 0;
    F(int, s, 20380) = 0;
    F(int, s, 3584) = 2;
    F(int, s, 224) |= 1;
    CallThis(FUN_57dd70, s + 16, F(int, B, 0), F(int, B, 316), F(int, B, 320), F(int, B, 324));
    CallThis(FUN_542580, s);
    F(int, s, 10424) = F(int, B, 2052);
    F(int, s, 10428) = F(int, B, 2048);
    for (int i = 0; i < F(int, s, 10424); i++)
    {
        memcpy(s + 9652 + i * 48, g_matrix7025f0, 48);
        F(int, s, 9620 + i * 4) = F(int, C, i * 4);
        if (i < F(int, s, 10424) - F(int, s, 10428))
        {
            BYTE* p = s + 10616 + i * 12;
            Cr_Copy3(p, B + 424 + i * 12);
            float o[3];
            Cr_Place((const float*)(C + 748), (const float*)p, o);
            memcpy(s + 10712 + i * 12, o, 12);
        }
    }
    F(int, s, 9616) = 0;
    for (int i = 0; i < F(int, s, 10424); i++)
    {
        if (i < F(int, s, 10428))
            F(float, s, 10584 + i * 4) = (float)((double)F(float, B, 652 + i * 4) - F(float, B, 620 + i * 4));
        else
            F(int, s, 10584 + i * 4) = F(int, s, 10580 + F(int, s, 10428) * 4);
        F(int, s, 20864 + i * 4) = 0;
        F(int, s, 10824 + i * 4) = 0;
        F(int, s, 10856 + i * 4) = -1;
    }
    F(int, s, 3992) = 3;
    F(int, s, 3996) = 0;
    F(int, s, 1212) |= 1;
    F(int, s, 10924) = 0;
    F(int, s, 1208) = 0;
    CallC(FUN_4e480c, F(BYTE*, C, 68), 1);
    F(int, s, 10920) = 0;
    F(int, s, 908) = 0;
    F(int, s, 20304) = 0;
    F(int, s, 10808) = 0;
    Cr_Copy3(s + 10812, g_zero702650);
    float* R = (float*)(s + 708);
    memcpy(s + 756, C + 700, 48);
    memcpy(R, C + 748, 48);
    {
        Matrix mt, mr, m1, m2;
        Matrix_Translation(&mt, F(Vec3, B, 300));
        Matrix_Multiply(&m1, Matrix_RotationX(&mr, F(float, B, 312)), &mt);
        memcpy(s + 10036, Matrix_Multiply(&m2, &m1, (Matrix*)R), 48);
    }
    float pt[3];
    for (int j = 0; j < F(int, B, 1300); j++)
    {
        const float* q = (const float*)(B + 1304 + j * 12);
        pt[1] = (float)((double)R[7] * q[2] + (double)R[1] * q[0] + (double)R[4] * q[1] + R[10]);
        pt[2] = (float)((double)R[2] * q[0] + (double)q[2] * R[8] + (double)q[1] * R[5] + R[11]);
        pt[0] = (float)((double)q[2] * R[6] + (double)R[0] * q[0] + (double)R[3] * q[1] + R[9]);
        CallThis(FUN_58ce80, s + 16, 1, pt, 0);
    }
    float lo = 10000.0f, hi = -10000.0f;
    for (int pass = 0; pass < 2; pass++)
    {
        int n = F(int, B, pass ? 780 : 776);
        for (int j = 0; j < n; j++)
        {
            const float* q = (const float*)(B + (pass ? 1024 : 784) + j * 12);
            pt[1] = (float)((double)R[7] * q[2] + (double)R[4] * q[1] + (double)R[1] * q[0] + R[10]);
            pt[2] = (float)((double)R[8] * q[2] + (double)R[5] * q[1] + (double)R[2] * q[0] + R[11]);
            pt[0] = (float)((double)R[6] * q[2] + (double)R[3] * q[1] + (double)R[0] * q[0] + R[9]);
            CallThis(FUN_58ce80, s + 16, 0, pt, pass ? 3 : 0);
            double h = (double)q[1] * R[3] + (double)q[2] * R[6] + (double)q[0] * R[0] + R[9];
            if (lo > h)
                lo = (float)h;
            if (!(hi >= h))
                hi = (float)h;
        }
    }
    F(int, s, 3604) = 0;
    F(float, s, 3292) = 0.5f;
    F(float, s, 3296) = F(float, s, 3300) = (float)(((double)hi - lo) * g_vh64c258);
    Cr_Place(R, (const float*)(C + 796), (float*)(s + 3452));
    float* M2 = (float*)(s + 756);
    for (int k = 0; k < 2; k++)
    {
        double v = (double)F(float, s, 3292) * 2 - F(float, B, 4);
        float xf = (float)((double)(float)(v * g_702630) + F(float, B, 328));
        float yf = (float)((double)(float)(g_702634 * v) + F(float, B, 332));
        double z = v * g_702638 + F(float, B, 336);
        float* P = (float*)(s + 20356 + k * 12);
        P[0] = 0;
        P[1] = (float)(((double)F(float, C, k ? 452 : 356) + F(float, C, k ? 404 : 308)) * g_vh64c258);
        P[2] = (float)(z * M2[8] + (double)yf * M2[5] + (double)xf * M2[2] + M2[11]);
    }
    {
        const float* P = (const float*)(s + 20356);
        float* o = (float*)(s + 20332);
        o[0] = (float)((double)R[6] * P[2] + (double)R[3] * P[1] + (double)R[0] * P[0] + R[9]);
        o[1] = (float)((double)R[7] * P[2] + (double)R[4] * P[1] + (double)R[1] * P[0] + R[10]);
        o[2] = (float)((double)R[8] * P[2] + (double)R[5] * P[1] + (double)R[2] * P[0] + R[11]);
        Cr_Place(R, (const float*)(s + 20368), (float*)(s + 20344));
        Cr_Copy3(s + 3464, s + 20332);
    }
    F(int, s, 20328) = 1;
    int np = 0;
    for (; np < F(int, B, 1672); np++)
        CallThis(FUN_58b550, s + 4564 + np * 28, B + 1352 + np * 16);
    F(int, s, 5148) = np;
    CallThis(FUN_58d290, s + 16);
    CallThis(FUN_58d2b0, s + 16);
    F(int, s, 10888) = 9;
    F(BYTE*, s, 10892) = CallC<BYTE*>(FUN_5ef2b0, 0, &s_cr679ef0);
    F(BYTE*, s, 10896) = CallC<BYTE*>(FUN_5ef2b0, 0, &s_cr679790);
    F(BYTE*, s, 10900) = CallC<BYTE*>(FUN_5ef2b0, 0, &s_cr67975c);
    F(float, s, 10908) = (float)CallC<double>(FUN_4e27a5);
    F(float, s, 10904) = (float)CallC<double>(FUN_4e27a5);
    F(float, s, 20524) = (float)(CallC<double>(FUN_4e27a5) - g_cr64d7b8);
    CallC(FUN_5465c0, s + 9536, s + 9532);
    F(int, s, 20900) |= 128;
    for (int i = 0; i < F(int, s, 9532); i++)
        F(int, s, 21052 + i * 4) = F(int, F(BYTE*, s, 9536), i * 20 + 4);
    for (int i = 0; i < 8; i++)
    {
        F(int, s, 21120 + i * 4) = 0;
        F(int, s, 21052 + (F(int, s, 9532) + i) * 4) = 0;
    }
    F(int, s, 9552) = F(int, s, 412);
    CallThis(FUN_5455d0, s, 0, s + 792);
    F(float, s, 20300) = 0.5f;
    F(int, s, 20296) = 0;
    for (int i = 0; i < 11; i++)
    {
        F(int, s, 21608 + i * 4) = 0;
        F(int, s, 21784 + i * 4) = 0;
        Cr_Copy3(s + 21652 + i * 12, g_zero702650);
        F(float, s, 21828 + i * 4) = (float)(CallC<double>(FUN_4e27a5) - g_cr64d7b8);
    }
    F(int, s, 20976) = F(int, B, 172);
    int* wl = &F(int, s, 20400);
    if (idx >= g_typeClass3 && idx < g_typeClass4)
    {
        wl[0] = 2; wl[1] = 2; wl[2] = 0; wl[3] = 1;
    }
    else
    {
        wl[0] = 0; wl[1] = 1; wl[2] = 2; wl[3] = 3;
        if (F(int, s, 10428) == 6)
        {
            wl[2] = 4; wl[3] = 5;
        }
    }
    F(float, s, 10280) = (float)((double)F(float, B, 336) - F(float, B, 620));
    for (int i = 0; i < 4; i++)
    {
        s[10274 + i] = 0;
        int k = wl[i];
        double w = F(float, B, 620 + k * 4);
        float a = (float)(w * g_702630), b = (float)(g_702634 * w);
        double c = w * g_702638;
        const float* P = (const float*)(B + 328 + k * 12);
        float* o = (float*)(s + 10284 + i * 12);
        o[0] = (float)((double)P[0] - a);
        o[1] = (float)((double)P[1] - b);
        o[2] = (float)(P[2] - c);
        Cr_Copy3(s + 10332 + i * 12, &g_702630);
        F(double, s, 20456 + i * 8) = (double)rand() * g_cr6796bc * g_cr64c254;
    }
    F(int, s, 20308) = 0;
    F(int, s, 20312) = 0;
    s[10180] = 0;
    F(int, s, 10100) = F(int, s, 10424);
    for (int i = 0; i < F(int, s, 10424); i++)
    {
        s[10181 + i] = 0;
        F(int, s, 10192 + i * 4) = 0;
    }
    s[10168] = s[3992];
    F(int, s, 10164) = 0;
    s[10273] = 0;
    F(int, s, 10088) = (F(int, s, 10424) + 8) * 3;
    F(int, s, 10092) = F(int, s, 10088) + 10;
    CallThis(FUN_541e80, s + 10084);
    // the original's search over the 6 kinds never moves past kind 0
    F(int, s, 20396) = 0;
    if (F(int, s, 9548) != 5)
    {
        int at = idx * 372 + F(int, s, 20396);
        BYTE* o = ((BYTE**)((BYTE*)g_6d24bc + 12))[at];
        if (!g_6d24e0[at] && o)
            CallVirt(o, 40);
        g_6d24e0[at]++;
    }
    else
    {
        if (!g_cr6ef1f0 && g_cr6ef204)
            CallVirt(g_cr6ef204, 40);
        g_cr6ef1f0++;
    }
    for (int i = 0; i < 3; i++)
    {
        F(int, s, 20428 + i * 4) = 0;
        F(int, s, 20440 + i * 4) = 0;
    }
    F(int, s, 20416) = 0;
    F(int, s, 20420) = 0;
    F(int, s, 20424) = 0;
    F(BYTE*, s, 20488) = CallC<BYTE*>(FUN_5ef3d0, &s_cr679edc);
    F(BYTE*, s, 20492) = CallC<BYTE*>(FUN_5ef3d0, &s_cr679ec4);
    F(BYTE*, s, 20496) = CallC<BYTE*>(FUN_5ef2b0, 0, &s_cr679ebc);
    F(BYTE*, s, 20500) = CallC<BYTE*>(FUN_5ef2b0, 0, &s_cr679eac);
    F(BYTE*, s, 20504) = CallC<BYTE*>(FUN_5ef2b0, 0, &s_cr679ea4);
    Cr_Ramps(s + 21152, 1, 1, 1, 0.3f, 0.3f, 1);
    Cr_Ramps(s + 21204, 1, 1, 1, 0.3f, 0.3f, 1);
    Cr_Ramps(s + 21256, 2, 1, 1, 1, 0.2f, 1);
    Cr_Ramps(s + 21308, 0, 1, 1, 0, 1, 1);
    Cr_Ramps(s + 21360, 0, 0, 1, 0, 4, 4);
    Cr_Ramps(s + 21412, 0, 1, 1, 0, 1, 1);
    Cr_Ramps(s + 21464, 0, 1, 1, 0, 1, 1);
    Cr_Ramps(s + 21516, 0, 1, 1, 0, 1, 1);
    F(float, s, 20508) = (float)((double)rand() * g_cr64f510);
    F(int, s, 20512) = 0;
    F(int, s, 20516) = 0;
    F(int, s, 20520) = 0;
    CallThis(FUN_5498b0, s + 21568, s);
    int wheels = F(int, s, 10424);
    F(int, s, 4000) = 0;
    F(int, s, 20604) = 0;
    F(int, s, 20608) = 1;
    F(int, s, 20612) = 0;
    F(int, s, 20600) = 0;
    F(float, s, 20616) = 10.0f;
    F(int, s, 20620) = 0;
    F(float, s, 20624) = 60.0f;
    F(int, s, 20596) = 0;
    F(int, s, 20628) = 0;
    F(int, s, 20632) = 0;
    CallThis(FUN_542450, s + 10432, wheels);
    if (F(float, B, 0) > g_cr64daac && _strnicmp(&s_cr679ea0, (const char*)(B + 2206), 3))
        F(int, s, 20900) |= 2;
    F(int, s, 10928) = 0;
    F(int, s, 10936) = 0;
    for (int i = 0; i < 50; i++)
    {
        F(int, s, 10940 + i * 4) = 0;
        F(int, s, 11140 + i * 4) = 0;
        Cr_Copy3(s + 17540 + i * 12, &g_702630);
        memcpy(s + 12740 + i * 48, g_matrix7025f0, 48);
        memcpy(s + 15140 + i * 48, g_matrix7025f0, 48);
        F(int, s, 18140 + i * 4) = 0;
        s[18540 + i * 33] = 0;
        Cr_Copy3(s + 11540 + i * 12, s + 432);
        Cr_Copy3(s + 12140 + i * 12, s + 444);
    }
    memcpy(s + 20212, s + 516, 48);
    strcpy((char*)(s + 20260), F(char*, F(BYTE*, s, 816), 12));
    memcpy(s + 12740, s + 516, 48);
    F(int, s, 18340) = F(int, s, 816);
    strcpy((char*)(s + 18540), F(char*, F(BYTE*, s, 816), 12));
    for (int i = 0; i < 5; i++)
        F(int, s, 20192 + i * 4) = 0;
}

} // extern "C"

// === drawing the vehicle (FUN_547030)

extern "C" void FUN_5f25c0(); KING_ALIAS("_FUN_5f25c0", "@Node_GetWorld@12")
extern "C" void FUN_5ae580(); KING_ALIAS("_FUN_5ae580", "@Render_PushMatrix@16")
extern "C" void FUN_5ae8a0(); KING_ALIAS("_FUN_5ae8a0", "@Clip_Pop@8")
extern "C" void FUN_556c60(); KING_ALIAS("_FUN_556c60", "@Car_CanSleep@4")
extern "C" void FUN_561c30(); KING_ALIAS("_FUN_561c30", "@Copter_UpdateLights@8")
extern "C" void FUN_5f98c0(); KING_ALIAS("_FUN_5f98c0", "@SceneNode5f98c0_Draw@16")
extern "C" void FUN_546d10(); KING_ALIAS("_FUN_546d10", "@Vehicle_SetLightSwitches@4")
extern "C" void FUN_5e19b0(); KING_ALIAS("_FUN_5e19b0", "_Frame_Apply")
extern "C" void FUN_43f840(); KING_ALIAS("_FUN_43f840", "_Player_Value156")
extern "C" void FUN_57def0(); KING_ALIAS("_FUN_57def0", "@SceneObj_Get800@8")
extern "C" void FUN_439580(); KING_ALIAS("_FUN_439580", "_Ai_GetCoordinatesPos")
extern "C" void FUN_5b73a0(); KING_ALIAS("_FUN_5b73a0", "_Window_DrawLine")
extern "C" void FUN_5b4fc0(); KING_ALIAS("_FUN_5b4fc0", "@View_SphereClip@12")
extern "C" void FUN_590d20(); KING_ALIAS("_FUN_590d20", "@Item_DrawBody@16")
KING_VAR(int, g_cd696be4, 696be4)                 // the debug level
KING_VAR(float, g_cd674408, 674408)
KING_VAR(double, g_cd64f4c8, 64f4c8)
KING_VAR(double, g_cd64eba8, 64eba8)
KING_VAR(double, g_cd64f718, 64f718)
KING_VAR(double, g_cd64b670, 64b670)
KING_VAR(float, g_cd64c250, 64c250)
KING_VAR(float, g_cd64c404, 64c404)
KING_VAR(float, g_cd6969a4, 6969a4)
KING_VAR(float, g_cd6969b0, 6969b0)
KING_VAR(float, g_cd6969bc, 6969bc)
KING_VAR(float, g_cd6969c8, 6969c8)

// the debug view's window (6ef1fc +464 +1948)
static BYTE* Cd_Window() { return F(BYTE*, F(BYTE*, g_6ef1fc, 464), 1948); }

// screen point of (x, y, z) seen through the view, rounded as the original
static void Cd_Screen(BYTE* w, double x, double y, double z, int* sx, int* sy)
{
    int ww = F(WORD, w, 0), wh = F(WORD, w, 2);
    *sx = (int)(x / y * ww * g_vh64c258 + ww / 2);
    *sy = (int)(wh / 2 - z / y * wh * g_vh64c258);
}

// a cross (color c) and a diagonal cross (color 4 or d) around (x, y)
static void Cd_Cross(int x, int y, int c, int d)
{
    CallC(FUN_5b73a0, Cd_Window(), x, y - 4, x, y + 4, c, 0);
    CallC(FUN_5b73a0, Cd_Window(), x - 4, y, x + 4, y, c, 0);
    CallC(FUN_5b73a0, Cd_Window(), x - 3, y - 3, x + 3, y + 3, d, 0);
    CallC(FUN_5b73a0, Cd_Window(), x + 3, y - 3, x - 3, y + 3, d, 0);
}

extern "C" {

// marks the car type's point q (body frame +436, camera +244) in the debug view through view matrix m
static void Cd_MarkPoint(BYTE* s, const float* q, const float* m, int c, int d)
{
    double a = (double)F(float, s, 448) * q[1] + (double)F(float, s, 460) * q[2] + (double)q[0] * F(float, s, 436) + F(float, s, 472);
    double b = (double)F(float, s, 452) * q[1] + (double)F(float, s, 440) * q[0] + (double)F(float, s, 464) * q[2] + F(float, s, 476);
    double c3 = (double)F(float, s, 456) * q[1] + (double)F(float, s, 444) * q[0] + (double)F(float, s, 468) * q[2] + F(float, s, 480);
    float u = (float)(c3 * F(float, s, 268) + b * F(float, s, 256) + a * F(float, s, 244) + F(float, s, 280));
    float v = (float)(a * F(float, s, 248) + c3 * F(float, s, 272) + b * F(float, s, 260) + F(float, s, 284));
    double w = a * F(float, s, 252) + c3 * F(float, s, 276) + b * F(float, s, 264) + F(float, s, 288);
    float X = (float)((double)m[0] * u + m[6] * w + (double)m[3] * v + m[9]);
    float Y = (float)(w * m[7] + (double)m[4] * v + (double)m[1] * u + m[10]);
    double Z = w * m[8] + (double)m[5] * v + (double)m[2] * u + m[11];
    if (Y > g_cd64c250)
    {
        int sx, sy;
        Cd_Screen(Cd_Window(), X, Y, Z, &sx, &sy);
        Cd_Cross(sx, sy, c, d);
    }
}

// [FUN_547030] draws the vehicle (this = car + 272) with (a, ctx): its shadow box, the light and wheel frames,
// the gauges, and at debug levels above 1 marks of its points in the debug view
int __fastcall Car_Draw(BYTE* s, void*, int a, BYTE* ctx)
{
    if (F(BYTE*, s, 68))
    {
        BYTE m[48];
        memcpy(m, g_matrix7025f0, 48);
        int r = CallThis<int>(FUN_5f25c0, F(BYTE*, s, 68), m);
        CallThis(FUN_5ae580, ctx, m, r);
        int box[4];
        memcpy(box, s + 20, 16);
        if (CallThis<int>(FUN_5b4fc0, ctx, box))
            F(int, s, 10308) = 1;
        CallThis(FUN_5ae8a0, ctx);
    }
    if (F(int, s, 9344))
        return 0;
    F(int, s, 3300) = 1;
    int sw = 0;
    if (g_cd696be4 > 1)
    {
        BYTE* o = F(BYTE*, s, 21328);
        if (F(int, s, 10652) != 1 || !o)
            sw = 4;
        else if (CallThis<int>(FUN_556c60, s - 272))
            sw = 3;
        else
        {
            int k = F(int, o, 9784);
            if (k == 1)
                sw = 2;
            else if (k == 3 && !F(int, s, 948))
                sw = 1;
        }
    }
    int idx = F(int, s, 10148);
    BYTE* C = (BYTE*)g_cr6d20f4 + idx * 1488;
    CallC(FUN_5e1a10, F(BYTE*, C, 256), sw);
    if (F(int, s, 9276) == 5)
    {
        if (F(BYTE*, s, 21332))
            CallThis(FUN_561c30, F(BYTE*, s, 21332));
        return CallThis<int>(FUN_5f98c0, s, a, ctx);
    }
    F(int, F(BYTE*, s, -224), 84) = F(int, s, 20124);
    CallThis(FUN_546d10, s - 272);
    if (F(int, s, 116))
    {
        for (int i = 0; i < F(int, s, 10152); i++)
        {
            BYTE* M = s + 9380 + i * 48;
            Matrix t;
            memcpy(M, Matrix_RotationX(&t, F(float, s, 9920 + i * 4)), 48);
            if (i < 2)
            {
                Matrix rz, t2;
                Matrix_RotationZ(&rz, (float)((double)(signed char)s[9908] * g_cd674408 * g_cd64f4c8));
                memcpy(M, Matrix_Multiply(&t2, (Matrix*)M, &rz), 48);
            }
            Cr_Copy3(M + 36, C + 304 + i * 48);
            F(float, M, 44) = (float)(F(float, M, 44) - (double)s[9909 + i] * F(float, s, 10312 + i * 4) * g_cd64eba8);
        }
    }
    for (int i = 0; i < F(int, s, 10152); i++)
        if (F(BYTE*, s, 9348 + i * 4))
            CallC(FUN_5e19b0, F(BYTE*, s, 9348 + i * 4), 0, s + 9380 + i * 48);
    for (int i = 0; i < F(int, s, 10156); i++)
        CallC(FUN_5e1a10, F(BYTE*, C, 104 + i * 4), F(int, s, 20848 + i * 4));
    CallC(FUN_5e1a10, F(BYTE*, s, 20232), F(int, s, 21076) ? 1 : 0);
    {
        Matrix ry, rx, m1, m2;
        Matrix_RotationY(&ry, F(float, s, 20040));
        Matrix_RotationX(&rx, F(float, s, 20036));
        Matrix_Multiply(&m1, &rx, &ry);
        Matrix_Multiply(&m2, &m1, (Matrix*)(C + 652));
        CallC(FUN_5e19b0, F(BYTE*, C, 32), 0, &m2);
    }
    if (F(int, s, 20248))
    {
        CallC(FUN_5e1a10, F(BYTE*, C, 1060), 1);
        int g = (int)(F(float, s, 20240) * g_cd64f718 + g_cd64b670);
        if (g == 7)
            g = 1;
        CallC(FUN_5e1a10, F(BYTE*, C, 1064), g);
        for (int k = 0; k < 2; k++)
        {
            Matrix mt, rz, m;
            Matrix_Translation(&mt, F(Vec3, C, 1144 + k * 12));
            Matrix_RotationZ(&rz, (float)((double)F(float, s, 20244) + F(float, s, 20244)));
            Matrix_Multiply(&m, &rz, &mt);
            CallC(FUN_5e19b0, F(BYTE*, C, 1052 + k * 4), 0, &m);
        }
    }
    else
    {
        CallC(FUN_5e1a10, F(BYTE*, C, 1060), 0);
        CallC(FUN_5e1a10, F(BYTE*, C, 1064), 0);
    }
    if (idx >= g_typeClass1 && idx < g_typeClass2 && F(void*, s, 20364) != g_68b990)
        CallC(FUN_5e1a10, F(BYTE*, C, 1068), CallC<int>(FUN_43f840, F(BYTE*, s, 20364)) + 1);
    else if (idx >= g_typeClass3 && idx < g_typeClass4
             && (!F(BYTE*, s, 21328) || F(int, F(BYTE*, s, 21328), 10104) == 2))
        ;
    else
        CallC(FUN_5e1a10, F(BYTE*, C, 1068), 0);
    int ret = CallThis<int>(FUN_590d20, s, a, ctx);
    float M[12];
    BYTE* W = F(BYTE*, g_6ef1fc, 464);
    if (g_cd696be4 == 3)
    {
        BYTE* o = F(BYTE*, s, 21328);
        if (!o)
            goto done;
        memcpy(M, W + 2240 + F(int, W, 8224) * 48, 48);
        for (int j = 0; j < F(int, s, 10156); j++)
        {
            const float* p = (const float*)(F(BYTE*, s, 21328) + 10880 + j * 12);
            double x = (double)M[0] * p[0] + (double)M[6] * p[2] + (double)M[3] * p[1] + M[9];
            float y = (float)((double)M[7] * p[2] + (double)M[4] * p[1] + (double)M[1] * p[0] + M[10]);
            float z = (float)((double)M[8] * p[2] + (double)M[5] * p[1] + (double)M[2] * p[0] + M[11]);
            if (y > g_cd64c250)
            {
                int sx, sy;
                Cd_Screen(Cd_Window(), x, y, z, &sx, &sy);
                Cd_Cross(sx, sy, 0, 4);
            }
        }
    }
    else if (g_cd696be4 <= 1)
        goto done;
    if (!F(BYTE*, s, 21328))
        goto done;
    W = F(BYTE*, g_6ef1fc, 464);
    memcpy(M, W + 2240 + F(int, W, 8224) * 48, 48);
    {
        BYTE* B = (BYTE*)g_carTypes697914 - 76 + F(int, s, 10148) * 2416;
        idx = F(int, s, 10148);
        if (idx >= g_typeClass2 && idx < g_typeClass3)
            Cd_MarkPoint(s, (const float*)(B + 1904), M, 5, 4);
        idx = F(int, s, 10148);
        if (idx >= g_typeClass3 && idx < g_typeClass4)
            Cd_MarkPoint(s, (const float*)(B + 1916), M, 6, 7);
    }
    {
        Position pos = {{(DWORD)-1, (DWORD)-1, 0, 0, 0, 0, 0, (DWORD)-1}};
        void* found;
        BYTE* so = CallThis<BYTE*>(FUN_57def0, s - 256);
        Room_ObjectAt(F(void*, so, 12), (const float*)(s + 280), &found, &pos, 0);
        if (!found)
            goto done;
        float r[3];
        CallC(FUN_439580, r, pos);
        double X = (double)r[0] * M[0] + (double)r[2] * M[6] + (double)r[1] * M[3] + M[9];
        float Y = (float)((double)r[2] * M[7] + (double)r[1] * M[4] + (double)M[1] * r[0] + M[10]);
        float Z = (float)((double)r[2] * M[8] + (double)r[1] * M[5] + (double)M[2] * r[0] + M[11]);
        if (Y > g_cd64c250)
        {
            BYTE* w = Cd_Window();
            int sx, sy;
            Cd_Screen(w, X, Y, Z, &sx, &sy);
            double e = x87_fabs((double)g_cd6969bc * r[2] + (double)g_cd6969b0 * r[1] + (double)g_cd6969a4 * r[0] + g_cd6969c8);
            int d = (int)(g_cd64c404 / (e + g_cd64c250) + g_cd64c250);
            int top = sy - d, bot = d + sy, l = sx - d, rt = d + sx;
            CallC(FUN_5b73a0, w, l, top, rt, top, 6, 0);
            CallC(FUN_5b73a0, Cd_Window(), l, bot, rt, bot, 6, 0);
            CallC(FUN_5b73a0, Cd_Window(), l, top, l, bot, 6, 0);
            CallC(FUN_5b73a0, Cd_Window(), rt, top, rt, bot, 6, 0);
            CallC(FUN_5b73a0, Cd_Window(), l, top, rt, bot, 7, 0);
            CallC(FUN_5b73a0, Cd_Window(), rt, top, l, bot, 7, 0);
        }
    }
done:
    F(int, s, 3300) = 0;
    return ret;
}

} // extern "C"

// === the impulse between two touching bodies (FUN_557530)

extern "C" double __fastcall Matrix_Determinant(const Matrix* self, void*);
extern "C" float* __cdecl Vec3_DivScalar(float* out, const float* v, float k);
KING_VAR(int, g_bp67a4a8, 67a4a8)                 // which pair of results to give
KING_VAR(double, g_bp64b658, 64b658)
KING_VAR(float, g_bp64c250, 64c250)

// m's rows are the three axes crossed with r
static void Bp_CrossRows(Matrix* m, const Vec3* r)
{
    Vec3_Cross((Vec3*)&m->m[0], &g_axisX, r);
    Vec3_Cross((Vec3*)&m->m[3], &g_axisY, r);
    Vec3_Cross((Vec3*)&m->m[6], &g_axisZ, r);
}

extern "C" {

// [FUN_557530] the velocities of bodies A and B (masses ma, mb, contact arms ra, rb, velocities va, vb, spins
// wa, wb, frames fa, fb, inertia matrices ia, ib scaled by sa, sb) after their contact: by 67a4a8, 2 gives
// (A's velocity, 0, A's spin, 0), 1 gives (A's velocity, B's velocity, A's spin, B's spin), else the four of an
// earlier solve; a near singular system leaves them unchanged
void __cdecl Body_PairImpulse(float ma, float mb, const Vec3* ra, const Vec3* rb, const Vec3* va, const Vec3* vb,
                              const Vec3* wa, const Vec3* wb, const Matrix* fa, const Matrix* fb,
                              const Matrix* ia, const Matrix* ib, const Vec3* sa, const Vec3* sb,
                              Vec3* o1, Vec3* o2, Vec3* o3, Vec3* o4)
{
    Matrix m864 = g_identity, m708 = g_identity, m804 = g_identity, m516, m336, m1104;
    Matrix m96, m48, m456, m756, m288, m240, m144, m564, m192, m636, m384;
    Matrix_ScaleColumns(&m96, ia, sa);
    Matrix_ScaleColumns(&m48, ib, sb);
    Matrix_Multiply(&m864, &m96, fa);
    Matrix_Multiply(&m456, &m48, fb);
    Matrix_Inverse(&m756, &m864);
    m1104 = *Matrix_Inverse(&m288, &m456);
    Bp_CrossRows(&m708, ra);
    Bp_CrossRows(&m804, rb);
    Vec3 v1164, v1140, v1252, v1228, v1240, v1200, v1188;
    Vec3_RotateBy(&v1164, wa, &m756);
    {
        const float* m = m1104.m;
        v1140.z = (float)((double)m[8] * wb->z + (double)m[5] * wb->y + (double)m[2] * wb->x);
        v1140.y = (float)((double)m[7] * wb->z + (double)m[4] * wb->y + (double)m[1] * wb->x);
        v1140.x = (float)((double)m[0] * wb->x + (double)m[6] * wb->z + (double)m[3] * wb->y);
    }
    Vec3_Add(&v1228, Vec3_Cross(&v1252, &v1164, ra), va);
    Vec3_Add(&v1240, Vec3_Cross(&v1252, &v1140, rb), vb);
    Vec3_Scaled(&v1240, &v1240, mb);
    Vec3_Scaled(&v1228, &v1228, ma);
    Vec3_Add(&v1240, &v1228, &v1240);
    double sum = (double)ma + mb;
    v1200.x = (float)(v1240.x / sum);
    v1200.y = (float)(v1240.y / sum);
    v1200.z = (float)(v1240.z / sum);
    float sumf = (float)sum;
    v1188 = v1200;
    Matrix_Multiply(&m288, &m864, &m1104);
    Matrix_Multiply(&m756, &m288, &m804);
    Matrix_ScaleRows(&m240, &m756, mb);
    Matrix_ScaleRows(&m144, &m708, ma);
    m516 = *Matrix_SubRows(&m564, &m144, &m240);
    if (x87_fabs(Matrix_Determinant(&m516, 0)) < g_bp64b658)
    {
        *o1 = *va;
        *o2 = *vb;
        *o3 = *wa;
        *o4 = *wb;
        return;
    }
    m336 = *Matrix_Inverse(&m564, &m516);
    Vec3 v1152, v1056, v1128, v1212, v948, v960, v900, v1008, v1032, v924, v996, v1044, v648, v660, v876;
    Vec3 v1020, v984, v816, v1176, v888, v1116, v972;
    Vec3_Scaled(&v1240, vb, mb);
    Vec3_Scaled(&v1228, va, ma);
    Vec3_Add(&v1152, &v1228, &v1240);
    Vec3_Add(&v1056, wa, wb);
    Vec3_RotateBy(&v1240, &v1056, &m1104);
    Vec3_Scaled(&v1252, Vec3_Cross(&v1200, &v1240, rb), mb);
    Vec3_Scaled(&v1128, &v1188, sumf);
    Vec3_Sub(&v948, Vec3_Sub(&v1212, &v1128, &v1152), &v1252);
    Vec3_RotateBy(&v960, &v948, &m336);
    Vec3_RotateBy(&v1128, Vec3_RotateBy(&v1212, &v960, &m864), &m1104);
    Vec3_Sub(&v900, Vec3_RotateBy(&v1200, &v1056, &m1104), &v1128);
    Vec3_Sub(&v1008, &v1188, Vec3_Cross(&v1212, &v960, ra));
    Vec3_Sub(&v1032, &v1188, Vec3_Cross(&v1212, &v900, rb));
    Vec3_RotateBy(&v924, &v960, &m864);
    Vec3_RotateBy(&v996, &v900, &m456);
    float inv = (float)(g_bp64c250 / mb);
    Vec3_Normalized(&v1044, ra);
    Matrix_Multiply(&m144, Matrix_Multiply(&m564, &m864, &m1104), &m804);
    m636 = *Matrix_SubRows(&m192, Matrix_ScaleRows(&m240, &m708, (float)((double)inv * ma)), &m144);
    m384.Identity();
    m636.m[2] = v1044.x;
    m636.m[5] = v1044.y;
    m636.m[8] = v1044.z;
    m384 = *Matrix_Inverse(&m564, &m636);
    Vec3_Scaled(&v648, Vec3_Sub(&v1128, &v1152, Vec3_Scaled(&v1212, &v1188, ma)), inv);
    Vec3_Cross(&v660, Vec3_RotateBy(&v1212, &v1056, &m1104), rb);
    Vec3_Sub(&v876, Vec3_Sub(&v1212, &v1188, &v648), &v660);
    v1020 = v876;
    v1020.z = (float)Vec3_Dot(&v1164, &v1044);
    Vec3_RotateBy(&v984, &v1020, &m384);
    Vec3_RotateBy(&v816, Vec3_Sub(&v1128, &v1056, Vec3_RotateBy(&v1212, &v984, &m864)), &m1104);
    Vec3_Sub(&v1176, &v1188, Vec3_Cross(&v1212, &v984, ra));
    Vec3_DivScalar(&v888.x, &Vec3_Sub(&v1128, &v1152, Vec3_Scaled(&v1212, &v1176, ma))->x, mb);
    Vec3_RotateBy(&v1116, &v984, &m864);
    Vec3_RotateBy(&v972, &v816, &m456);
    Vec3 zero;
    zero.x = zero.y = zero.z = 0;
    if (g_bp67a4a8 == 2)
    {
        *o1 = v1176;
        *o2 = zero;
        *o3 = v1116;
        *o4 = zero;
    }
    else if (g_bp67a4a8 == 1)
    {
        *o1 = v1176;
        *o2 = v888;
        *o3 = v1116;
        *o4 = v972;
    }
    else
    {
        *o1 = v1008;
        *o2 = v1032;
        *o3 = v924;
        *o4 = v996;
    }
}

} // extern "C"

// === the vehicle's sparks, wheel dust and vapour (FUN_551970)

extern "C" void FUN_4e5ba7(); KING_ALIAS("_FUN_4e5ba7", "_Query_HitObject")
extern "C" void FUN_4bac90(); KING_ALIAS("_FUN_4bac90", "_Effect_New59c420")
extern "C" void FUN_4bb050(); KING_ALIAS("_FUN_4bb050", "_Fx_WheelDust")
extern "C" void FUN_4ba660(); KING_ALIAS("_FUN_4ba660", "_Fx_SparksByStrength")
extern "C" void FUN_4baf90(); KING_ALIAS("_FUN_4baf90", "_Effect_NewVapor")
extern "C" void FUN_549bf0(); KING_ALIAS("_FUN_549bf0", "@Obj549bf0_Get@8")
extern "C" void FUN_549ff0(); KING_ALIAS("_FUN_549ff0", "@CarEffect_Curve@16")
extern "C" void FUN_5ab710(); KING_ALIAS("_FUN_5ab710", "@Vec3_AddAssign@16")
extern "C" void FUN_549e60(); KING_ALIAS("_FUN_549e60", "@Lamp_Flash@12")
KING_VAR(double, g_fx64b678, 64b678)
KING_VAR(double, g_fx64b670, 64b670)
KING_VAR(double, g_fx64b780, 64b780)
KING_VAR(float, g_fx64c264, 64c264)
KING_VAR(double, g_fx64b5b8, 64b5b8)
KING_VAR(float, g_fx64d2f8, 64d2f8)
KING_VAR(float, g_fx64c2fc, 64c2fc)
KING_VAR(double, g_fx64d318, 64d318)
KING_VAR(double, g_fx64ca78, 64ca78)
KING_VAR(double, g_fx64b7a8, 64b7a8)
KING_VAR(float, g_fx64c254, 64c254)
KING_VAR(double, g_fx64b6e0, 64b6e0)
KING_VAR(double, g_fx64d0f0, 64d0f0)
KING_VAR(float, g_fx64c250, 64c250)
KING_VAR(float, g_fx64c938, 64c938)

// d = the axis x of sign sg (a's +328 + 12 k against 64c264) turned by m
static void Fx_SideDir(const float* m, double t, Vec3* d)
{
    double sg = t > g_fx64c264 ? 1 : -1;
    float u = (float)(sg * g_axisX.x), v = (float)(sg * g_axisX.y);
    double w = sg * g_axisX.z;
    d->x = (float)(w * m[6] + (double)v * m[3] + (double)u * m[0]);
    d->y = (float)(w * m[7] + (double)v * m[4] + (double)m[1] * u);
    d->z = (float)(w * m[8] + (double)v * m[5] + (double)m[2] * u);
}

static double Fx_RandSigned() { return rand() * (double)g_fx64c254 - g_fx64b6e0; }

extern "C" {

// [FUN_551970] the vehicle's effects of the frame (with +20900 bit 8; every n-th frame in network games):
// exhaust puffs, wheel dust of the wheels (+10428) or of the four driven ones, sparks from the body
// (+21568's state 1..4), and vapour at both ends with +10164's bits 15/16
void __fastcall Car_Effects(BYTE* s, void*)
{
    if (!(F(int, s, 20900) & 0x100))
        return;
    if (F(int, g_6cec6c, 1300) > 1)
    {
        double x = F(double, g_6ef1fc, 7960);
        if (x == g_fx64b678)
            return;
        int n = (int)(g_fx64b670 / x87_fabs(x));
        if (n < 1)
            n = 1;
        if (F(int, g_6ef1fc, 2992) % n)
            return;
    }
    int idx = F(int, s, 10420);
    BYTE* T = (BYTE*)g_carTypes697914 - 76 + idx * 2416;
    Matrix mm;
    Matrix_Multiply(&mm, (Matrix*)(s + 708), (Matrix*)(s + 516));
    const float* m = mm.m;
    CallC(FUN_4e5ba7, F(BYTE*, s, 816), s + 552, &mm.m[9]);
    if ((s[20900] & 2) && (F(int, s, 10164) & 0x400))
    {
        const float* t = (const float*)(T + 1748);
        for (int e = 0; e < 2; e++)
        {
            const float* q = t + 3 + e * 6;
            const float* a = q + 3;
            if (!(x87_fabs(q[2]) < g_fx64b780))
                continue;
            Vec3 d, p;
            if (!e)
            {
                d.y = (float)((double)m[1] * a[0] + (double)m[7] * a[2] + (double)m[4] * a[1]);
                d.z = (float)((double)m[2] * a[0] + (double)m[8] * a[2] + (double)m[5] * a[1]);
                d.x = (float)((double)m[6] * a[2] + (double)m[3] * a[1] + (double)m[0] * a[0]);
                p.y = (float)((double)m[1] * q[0] + (double)m[7] * q[2] + (double)m[4] * q[1] + m[10]);
                p.z = (float)((double)m[2] * q[0] + (double)m[8] * q[2] + (double)m[5] * q[1] + m[11]);
            }
            else
            {
                d.y = (float)((double)m[7] * a[2] + (double)m[4] * a[1] + (double)m[1] * a[0]);
                d.z = (float)((double)m[8] * a[2] + (double)m[5] * a[1] + (double)m[2] * a[0]);
                d.x = (float)((double)m[6] * a[2] + (double)m[3] * a[1] + (double)m[0] * a[0]);
                p.y = (float)((double)m[7] * q[2] + (double)m[4] * q[1] + (double)m[1] * q[0] + m[10]);
                p.z = (float)((double)m[8] * q[2] + (double)m[5] * q[1] + (double)m[2] * q[0] + m[11]);
            }
            p.x = (float)((double)m[0] * q[0] + (double)m[6] * q[2] + (double)m[3] * q[1] + m[9]);
            CallC(FUN_4bac90, &p, &d, 0, 1);
        }
    }
    if (F(BYTE*, s, 21600))
    {
        for (int i = 0; i < F(int, s, 10428); i++)
        {
            Vec3 d;
            Fx_SideDir(m, F(float, T, 328 + i * 12), &d);
            float f = F(float, s, 10224 + i * 4);
            if (f > g_fx64b5b8)
                CallC(FUN_4bb050, F(BYTE*, s, 21600) + 11172 + i * 248, &d, f, 1);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            int k = F(int, s, 20400 + i * 4);
            Vec3 d;
            Fx_SideDir(m, F(float, T, 328 + k * 12), &d);
            if ((s[10273] & (1 << i)) && F(float, s, 10224 + k * 4) > g_fx64b5b8)
            {
                const float* P = (const float*)(s + 10284 + i * 12);
                Vec3 p;
                p.x = (float)((double)m[3] * P[1] + (double)m[6] * P[2] + (double)m[0] * P[0] + m[9]);
                p.y = (float)((double)m[4] * P[1] + (double)m[7] * P[2] + (double)m[1] * P[0] + m[10]);
                p.z = (float)((double)m[5] * P[1] + (double)m[8] * P[2] + (double)m[2] * P[0] + m[11]);
                CallC(FUN_4bb050, &p, &d, F(float, s, 10224 + i * 4), 1);
            }
        }
    }
    BYTE* body = s + 21568;
    int st = CallThis<int>(FUN_549bf0, body);
    if (st >= 1 && st <= 4)
    {
        float amp, out;
        int n = (int)(CallThis<double>(FUN_549ff0, body, &amp, &out) * F(float, T, 1264) * g_fx64d2f8);
        if (n > 0)
        {
            int clamp;
            if ((double)F(float, T, 1812) - F(float, T, 304) > g_fx64c264)
            {
                amp = (float)((double)amp * F(float, T, 1264));
                clamp = 1;
            }
            else
            {
                amp = (float)((double)amp * F(float, T, 1264) * g_fx64c2fc);
                clamp = 0;
            }
            float strength = (float)(out * g_fx64d318 + g_fx64ca78);
            st = CallThis<int>(FUN_549bf0, body);
            if ((st == 1 || st == 2) && strength > g_fx64b7a8)
                CallThis(FUN_549e60, body, 1);
            Vec3 hiV = F(Vec3, T, 1276), loV = F(Vec3, T, 1288);
            float h = (float)((double)F(float, T, 1272) + g_fx64c250);
            for (; n; n--)
            {
                float r1 = (float)(Fx_RandSigned() * amp);
                float r2 = (float)(Fx_RandSigned() * amp);
                double r3 = Fx_RandSigned() * g_fx64d0f0;
                Vec3 vy, vx;
                vy.x = (float)((double)g_axisY.x * r2);
                vy.y = (float)((double)g_axisY.y * r2);
                vy.z = (float)((double)g_axisY.z * r2);
                vx.x = (float)((double)g_axisX.x * r1);
                vx.y = (float)((double)g_axisX.y * r1);
                vx.z = (float)((double)g_axisX.z * r1);
                float a0 = (float)((double)(float)(g_axisZ.x * r3) + F(float, T, 1808));
                float a1 = (float)((double)(float)(g_axisZ.y * r3) + F(float, T, 1812));
                double a2 = g_axisZ.z * r3 + F(float, T, 1816);
                float b0 = (float)((double)a0 + vx.x), b1 = (float)((double)vx.y + a1);
                a2 += vx.z;
                Vec3 P, V;
                P.x = (float)((double)b0 + vy.x);
                P.y = (float)((double)vy.y + b1);
                P.z = (float)(vy.z + a2);
                V.x = (float)(h * (double)g_axisZ.x + F(float, s, 432));
                V.y = (float)((double)(float)(g_axisZ.y * (double)h) + F(float, s, 436));
                V.z = (float)((double)(float)(h * (double)g_axisZ.z) + F(float, s, 440));
                float e = (float)((double)F(float, T, 304) - P.y);
                if (e < g_fx64c250 && e > g_fx64c938)
                {
                    Vec3 up, tmp;
                    up.x = (float)((double)g_axisY.x + g_axisY.x);
                    up.y = (float)((double)g_axisY.y + g_axisY.y);
                    up.z = (float)((double)g_axisY.z + g_axisY.z);
                    CallThis(FUN_5ab710, &P, &tmp, &up);
                    const float* R = (const float*)(s + 516);
                    double c0 = (double)g_axisY.y * R[3] + (double)g_axisY.z * R[6] + (double)g_axisY.x * R[0];
                    float c1 = (float)((double)g_axisY.y * R[4] + (double)g_axisY.z * R[7] + (double)g_axisY.x * R[1]);
                    float c2 = (float)((double)g_axisY.y * R[5] + (double)g_axisY.z * R[8] + (double)g_axisY.x * R[2]);
                    e = (float)((double)e + g_fx64c250);
                    Vec3 push;
                    push.x = (float)(c0 * e);
                    push.y = (float)((double)c1 * e);
                    push.z = (float)((double)c2 * e);
                    CallThis(FUN_5ab710, &V, &tmp, &push);
                }
                if (clamp)
                {
                    if (P.x > hiV.x)
                        P.x = hiV.x;
                    if (P.x < loV.x)
                        P.x = loV.x;
                    if (P.y > hiV.y)
                        P.y = hiV.y;
                    if (P.y < loV.y)
                        P.y = loV.y;
                }
                else
                {
                    double top = hiV.y + g_fx64b6e0;
                    if (P.y > top)
                    {
                        P.y = (float)top;
                        P.x = (float)(Fx_RandSigned() * hiV.x);
                    }
                    else if (P.y < loV.y)
                    {
                        P.y = loV.y;
                        P.x = (float)(Fx_RandSigned() * hiV.x);
                    }
                    else
                    {
                        if (P.x > g_fx64c264)
                            P.x = (float)(hiV.x + g_fx64b5b8);
                        if (P.x < g_fx64c264)
                            P.x = (float)(loV.x - g_fx64b5b8);
                    }
                }
                if (P.z > hiV.z)
                    P.z = hiV.z;
                if (P.z < loV.z)
                    P.z = loV.z;
                const float* M = (const float*)(s + 708);
                Vec3 w, t;
                w.z = (float)((double)P.y * M[5] + (double)P.z * M[8] + (double)P.x * M[2] + M[11]);
                w.y = (float)((double)P.y * M[4] + (double)P.z * M[7] + (double)P.x * M[1] + M[10]);
                w.x = (float)((double)P.z * M[6] + (double)P.y * M[3] + (double)P.x * M[0] + M[9]);
                P = *Vec3_MulMatrix(&t, &w, (Matrix*)(s + 516));
                CallC(FUN_4ba660, &P, &V, strength, 1);
            }
        }
    }
    int f = F(int, s, 10164);
    if (!(f & 0x18000))
        return;
    float sc = (f & 0x10000) ? 2.0f : 0.5f;
    double h2 = F(float, T, 4);
    Vec3 a, b, w, t1, t2, w2;
    a.x = (float)(h2 * g_axisZ.x);
    a.y = (float)(g_axisZ.y * h2);
    a.z = (float)(h2 * g_axisZ.z);
    b.x = (float)((double)F(float, T, 328) - a.x);
    b.y = (float)((double)F(float, T, 332) - a.y);
    b.z = (float)((double)F(float, T, 336) - a.z);
    Vec3_MulMatrix(&w, &b, &mm);
    Vec3_Sub(&t2, (const Vec3*)(T + 340), Vec3_Scaled(&t1, &g_axisZ, F(float, T, 4)));
    Vec3_MulMatrix(&w2, &t2, &mm);
    CallC(FUN_4baf90, &w, sc, 1);
    CallC(FUN_4baf90, &w2, sc, 1);
}

} // extern "C"

// === the vehicle's step of the frame (FUN_54e180)

extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
extern "C" void FUN_54d690(); KING_ALIAS("_FUN_54d690", "@Car_ApplySettings@8")
extern "C" void FUN_5dbec0(); KING_ALIAS("_FUN_5dbec0", "_Light_Set@24")
extern "C" void FUN_549c00(); KING_ALIAS("_FUN_549c00", "@Flasher_StepLight@4")
extern "C" void FUN_53f7b0(); KING_ALIAS("_FUN_53f7b0", "_Car_53f7b0")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
extern "C" void FUN_549900(); KING_ALIAS("_FUN_549900", "@CarEffect_Start@12")
extern "C" void FUN_5494c0(); KING_ALIAS("_FUN_5494c0", "@CarEffect_End@12")
extern "C" void FUN_4eb690(); KING_ALIAS("_FUN_4eb690", "@Ramp_Position@8")
extern "C" void FUN_4eb6a0(); KING_ALIAS("_FUN_4eb6a0", "@Ramp_SetPosition@12")
extern "C" void FUN_476930(); KING_ALIAS("_FUN_476930", "_Player_GetState")
extern "C" void FUN_4769e0(); KING_ALIAS("_FUN_4769e0", "_Vehicle_SetTable516")
extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
extern "C" void FUN_4e5964(); KING_ALIAS("_FUN_4e5964", "_Query_4e5964")
extern "C" void FUN_445f60(); KING_ALIAS("_FUN_445f60", "_Ai_GetRightDirection")
extern "C" void FUN_439320(); KING_ALIAS("_FUN_439320", "_Ai_GetTangent")
extern "C" void FUN_533510(); KING_ALIAS("_FUN_533510", "_Road_IsPlace")
extern "C" void FUN_590520(); KING_ALIAS("_FUN_590520", "@Body_UpdateLying@4")
KING_VAR(BYTE, g_cs6796b0, 6796b0)                // switches: 1 run, 2 no place flag, 4 reset gear 3
KING_VAR(float, g_cs64d7b8, 64d7b8)
KING_VAR(float, g_cs64c450, 64c450)
KING_VAR(float, g_cs64c320, 64c320)
KING_VAR(float, g_cs6796c4, 6796c4)
KING_VAR(double, g_cs64b840, 64b840)
KING_VAR(double, g_cs64b6d0, 64b6d0)
KING_VAR(double, g_cs64b738, 64b738)
KING_VAR(double, g_cs64b678, 64b678)
KING_VAR(double, g_cs64b5a0, 64b5a0)
KING_VAR(float, g_cs64c264, 64c264)
KING_VAR(char, s_cs66d638, 66d638)
KING_VAR(char, s_cs66d628, 66d628)

// world point of v in car c's frame (+516, position +552) summed as one order of the original
static void Cs_WorldA(BYTE* s, const float* v, float* o)
{
    o[0] = (float)((double)F(float, s, 528) * v[1] + (double)F(float, s, 540) * v[2] + (double)v[0] * F(float, s, 516) + F(float, s, 552));
    o[1] = (float)((double)F(float, s, 532) * v[1] + (double)F(float, s, 520) * v[0] + (double)F(float, s, 544) * v[2] + F(float, s, 556));
    o[2] = (float)((double)F(float, s, 536) * v[1] + (double)F(float, s, 524) * v[0] + (double)F(float, s, 548) * v[2] + F(float, s, 560));
}

// the same with the other orders
static void Cs_WorldB(BYTE* s, const float* v, float* o)
{
    o[0] = (float)((double)F(float, s, 540) * v[2] + (double)F(float, s, 528) * v[1] + (double)v[0] * F(float, s, 516) + F(float, s, 552));
    o[1] = (float)((double)F(float, s, 544) * v[2] + (double)F(float, s, 532) * v[1] + (double)F(float, s, 520) * v[0] + F(float, s, 556));
    o[2] = (float)((double)F(float, s, 548) * v[2] + (double)F(float, s, 536) * v[1] + (double)F(float, s, 524) * v[0] + F(float, s, 560));
}

static void Cs_WorldC(BYTE* s, const float* v, float* o)
{
    o[0] = (float)((double)F(float, s, 540) * v[2] + (double)F(float, s, 528) * v[1] + (double)v[0] * F(float, s, 516) + F(float, s, 552));
    o[1] = (float)((double)F(float, s, 544) * v[2] + (double)F(float, s, 520) * v[0] + (double)F(float, s, 532) * v[1] + F(float, s, 556));
    o[2] = (float)((double)F(float, s, 548) * v[2] + (double)F(float, s, 524) * v[0] + (double)F(float, s, 536) * v[1] + F(float, s, 560));
}

// whether the edge test between p1 and p2 is free for both heights (+3300, +3292)
static int Cs_EdgeFree(BYTE* s, const float* p1, const float* p2, BYTE* node)
{
    BYTE* q = CallC<BYTE*>(FUN_4e5ba7, F(BYTE*, s, 816), s + 552, p2);
    if (!q)
        q = F(BYTE*, s, 816);
    BYTE* me = s + 16;
    if (CallC<int>(FUN_4e5964, me, q, p1, p2, node, F(int, s, 3300)) & 1)
        return 0;
    if (CallC<int>(FUN_4e5964, me, q, p1, p2, node, F(int, s, 3292)) & 1)
        return 0;
    return 1;
}

extern "C" {

// [FUN_54e180] the vehicle's step (*out gets flags 8 grounded, 16 on a road, 32, 64 and 256): lights, the
// lift effects of touching cars, the edge steps for gear 2, the wheels' ground flags, and the road position
// (+5160 half width, +5168 point, +5192 tangent, +5180 side, +5204 not a place); 0 when it isn't run
int __fastcall Car_Step(BYTE* s, void*, DWORD* out)
{
    if (!(g_cs6796b0 & 1))
    {
        CallThis(FUN_5d32f0, s + 16, 1);
        return 0;
    }
    if (F(int, s, 388))
    {
        CallThis(FUN_54d690, s);
        return 0;
    }
    F(int, s, 3572) = 2;
    CallVirt(s + 16, 52);
    CallThis(FUN_590520, s + 16);
    *out = 0;
    F(int, s, 20316) = 1;
    F(int, s, 4000) = 0;
    F(int, s, 20320) = 0;
    F(int, s, 20324) = 0;
    if (F(BYTE*, s, 21600))
        F(int, F(BYTE*, s, 21600), 10704) = 0;
    CallStd(FUN_5dbec0, F(int, s, 816), F(int, s, 552), F(int, s, 556), F(int, s, 560), (int)(void*)&Obj_Call72, (int)s);
    if (F(int, s, 20320))
        *out |= 32;
    if (((g_cs6796b0 & 4) && F(int, s, 3992) == 3) || F(int, s, 21604))
        F(int, s, 3992) = 2;
    if (F(int, s, 20900) & 0x100)
    {
        BYTE* body = s + 21568;
        CallThis(FUN_549c00, body);
        int st = CallThis<int>(FUN_549bf0, body);
        if (st != 1 && st != 2)
        {
            BYTE* hit = 0;
            for (int i = 0; i < 6; i++)
            {
                BYTE* o = F(BYTE*, s, 3524 + i * 4);
                if (!o)
                    break;
                BYTE* c = CallC<BYTE*>(FUN_53f7b0, o);
                if (F(float, s, 3548 + i * 4) > g_cs64d7b8)
                    break;
                if (!c)
                    continue;
                int cs = CallThis<int>(FUN_549bf0, c + 21568);
                if (cs != 1 && cs != 2)
                    continue;
                BYTE tmp[48];
                const float* M = CallThis<float*>(FUN_57de90, c + 16, tmp);
                const float* q = (const float*)((BYTE*)g_carTypes697914 - 76 + F(int, c, 10420) * 2416 + 1808);
                double cx = (double)M[6] * q[2] + (double)M[3] * q[1] + (double)q[0] * M[0] + M[9];
                float cy = (float)((double)M[1] * q[0] + (double)M[7] * q[2] + (double)M[4] * q[1] + M[10]);
                float cz = (float)((double)M[2] * q[0] + (double)M[8] * q[2] + (double)M[5] * q[1] + M[11]);
                const float* p = (const float*)((BYTE*)g_carTypes697914 - 76 + F(int, s, 10420) * 2416 + 1808);
                double sx = (double)F(float, s, 540) * p[2] + (double)F(float, s, 528) * p[1] + (double)p[0] * F(float, s, 516) + F(float, s, 552);
                float sy = (float)((double)F(float, s, 544) * p[2] + (double)F(float, s, 520) * p[0] + (double)F(float, s, 532) * p[1] + F(float, s, 556));
                float sz = (float)((double)F(float, s, 548) * p[2] + (double)F(float, s, 524) * p[0] + (double)F(float, s, 536) * p[1] + F(float, s, 560));
                float dx = (float)(sx - cx), dy = (float)((double)sy - cy);
                double dz = (double)sz - cz;
                if ((double)dx * dx + (double)dy * dy + dz * dz < g_cs64c450)
                    hit = c;
            }
            if (hit)
            {
                if (CallC<double>(FUN_4e27a5) - F(float, s, 20524) > g_cs64c320)
                    CallThis(FUN_549900, body, 0);
            }
            else
                F(float, s, 20524) = (float)CallC<double>(FUN_4e27a5);
        }
        else
        {
            float r = (float)(F(double, g_6ef1fc, 2960) / g_cs6796c4);
            BYTE* o = F(BYTE*, s, 21600);
            if (o)
            {
                CallThis(FUN_4eb6a0, o + 16060, (float)(CallThis<double>(FUN_4eb690, o + 16060) - r));
                o = F(BYTE*, s, 21600);
                CallThis(FUN_4eb6a0, o + 16112, (float)(CallThis<double>(FUN_4eb690, o + 16112) - r * g_cs64b840));
            }
            if (F(double, g_6ef1fc, 2968) - F(float, s, 21572) > g_cs64b6d0)
            {
                CallThis(FUN_5494c0, s, 0);
                BYTE rec[19 * 16];
                for (int i = 0; i < 19; i++)
                {
                    F(int, rec, i * 16 + 8) = 0;
                    F(int, rec, i * 16) = 0;
                    F(int, rec, i * 16 + 12) = 0x3ff00000;
                }
                if (F(void*, s, 20636) != g_68b990)
                {
                    CallC(FUN_476930, F(BYTE*, s, 20636), rec);
                    double& d = F(double, rec, 200);
                    if (!(d > g_cs64b678) || !((d -= g_cs64b738) > g_cs64b678))
                        F(int, rec, 192) = 0;
                    CallC(FUN_4769e0, F(BYTE*, s, 20636), rec);
                }
            }
        }
    }
    CallVirt(s + 16, 56);
    if (F(int, s, 3992) == 2)
    {
        float dz = (float)((double)F(float, s, 440) * F(float, s, 640) + (double)F(float, s, 436) * F(float, s, 628) + (double)F(float, s, 616) * F(float, s, 432));
        BYTE node[19 * 16];
        CallThis(FUN_5ef5c0, node);
        int a = x87_fabs(dz) > g_cs64b5a0 ? 1 : 0;
        int dir = (dz > g_cs64c264 ? 1 : -1) * a;
        float p1[3], p2[3];
        if (F(int, s, 20328) == 1 && dir == -1)
        {
            Cs_WorldA(s, (const float*)(s + 20332), p1);
            Cs_WorldB(s, (const float*)(s + 20344), p2);
            if (Cs_EdgeFree(s, p1, p2, node))
            {
                F(int, s, 20328) = -1;
                Cr_Copy3(s + 3464, s + 20344);
            }
        }
        if (F(int, s, 20328) == -1 && dir == 1)
        {
            Cs_WorldC(s, (const float*)(s + 20344), p1);
            Cs_WorldB(s, (const float*)(s + 20332), p2);
            if (Cs_EdgeFree(s, p1, p2, node))
            {
                F(int, s, 20328) = 1;
                Cr_Copy3(s + 3464, s + 20332);
            }
        }
        if (F(int, s, 20324))
            *out |= 64;
    }
    else
    {
        double c = g_cs64c264;
        for (int i = 0; i < 4; i++)
        {
            const float* n = (const float*)(s + 3848 + i * 12);
            int* flag = &F(int, s, 3896 + i * 4);
            double c1 = (double)F(float, s, 448) * n[2] - (double)n[1] * F(float, s, 452);
            float c2 = (float)((double)F(float, s, 452) * n[0] - (double)F(float, s, 444) * n[2]);
            double e1 = (double)F(float, s, 436) * F(float, s, 624) + (double)F(float, s, 432) * F(float, s, 612) + (double)F(float, s, 440) * F(float, s, 636);
            float e2 = (float)((double)F(float, s, 440) * F(float, s, 640) + (double)F(float, s, 436) * F(float, s, 628) + (double)F(float, s, 432) * F(float, s, 616));
            *flag = 1;
            double u = c1 + e1;
            float w = (float)((double)c2 + e2);
            if (!(u < c))
            {
                if (!(w < c) && !(n[0] > c) && !(n[1] > c))
                    *flag = 0;
                if (!(w > c) && !(n[0] > c) && !(n[1] < c))
                    *flag = 0;
            }
            if (!(u > c))
            {
                if (!(w < c) && !(n[0] < c) && !(n[1] > c))
                    *flag = 0;
                if (!(w > c) && !(n[0] < c) && !(n[1] < c))
                    *flag = 0;
            }
        }
    }
    BYTE* o = F(BYTE*, s, 21600);
    *out |= (o && !F(int, o, 13124)) ? 8 : 0;
    if (F(int, s, 20316) && F(void*, s, 20636) != g_68b990)
    {
        Position pos = {{(DWORD)-1, (DWORD)-1, 0, 0, 0, 0, 0, (DWORD)-1}};
        void* found = 0;
        Room_ObjectAt(F(void*, F(BYTE*, s, 816), 12), (const float*)(s + 552), &found, &pos, 0);
        if (!found)
            F(int, s, 20316) = 0;
        else
        {
            Position t;
            pos = *CallC<Position*>(FUN_445f60, &t, F(BYTE*, s, 20636), pos);
            RoomInfo ri;
            ri.flags = 0;
            ri.m_8 = 0;
            ri.m_12 = 0;
            ri.module = &s_cs66d638;
            ri.room = &s_cs66d628;
            ri.m_24 = 0;
            ri.m_28 = 0;
            ri.m_36 = -1;
            ri.m_40 = -1;
            Road_RoomInfo(pos, &ri);
            F(float, s, 5160) = (float)(*(double*)&ri.m_8 * g_fx64b6e0);
            float v[3];
            Cr_Copy3(s + 5168, CallC<float*>(FUN_439580, v, pos));
            Vec3 tg, nt;
            Vec3* b = (Vec3*)(s + 5192);
            *b = *Vec3_Normalized(&nt, CallC<Vec3*>(FUN_439320, &tg, pos));
            F(int, s, 5204) = CallC<int>(FUN_533510, &pos) ? 0 : 1;
            Vec3_Cross((Vec3*)(s + 5180), b, &g_axisZ);
        }
    }
    if (!(g_cs6796b0 & 2) && F(int, s, 20316))
        *out |= 16;
    F(int, s, 912) = F(int, s, 908);
    F(BYTE, s, 908) &= 0xfa;
    if (F(int, s, 21604))
        *out |= 256;
    return 1;
}

} // extern "C"

// === the vehicle's contact state of the frame (FUN_548250)

extern "C" void FUN_4e6490(); KING_ALIAS("_FUN_4e6490", "_Vec3_PlaneHit")
extern "C" void FUN_548130(); KING_ALIAS("_FUN_548130", "_Wheels_Mean")
extern "C" void FUN_4bcd00(); KING_ALIAS("_FUN_4bcd00", "@Trail_Follow@24")
extern "C" void FUN_56f240(); KING_ALIAS("_FUN_56f240", "_Mine_Explode")
extern "C" void FUN_5ab620(); KING_ALIAS("_FUN_5ab620", "@Vec3_MulAssign@16")
extern "C" void FUN_4e41d9(); KING_ALIAS("_FUN_4e41d9", "_Angle_F2Pi")
KING_VAR(BYTE*, g_ct6961f8, 6961f8)               // the trails
KING_VAR(int, g_ct6cec68, 6cec68)
KING_VAR(char, s_ct67a114, 67a114)
KING_VAR(double, g_ct64f728, 64f728)
KING_VAR(float, g_ct64f720, 64f720)

// v in car s's frame +612 (position +648), then through the view frame +756 (position +792 when pos)
static void Ct_Through(BYTE* s, const float* v, float* o, bool pos)
{
    double a, b, c;
    if (pos)
    {
        a = (double)F(float, s, 624) * v[1] + (double)F(float, s, 636) * v[2] + (double)F(float, s, 612) * v[0] + F(float, s, 648);
        b = (double)F(float, s, 628) * v[1] + (double)F(float, s, 616) * v[0] + (double)F(float, s, 640) * v[2] + F(float, s, 652);
        c = (double)F(float, s, 632) * v[1] + (double)F(float, s, 620) * v[0] + (double)F(float, s, 644) * v[2] + F(float, s, 656);
    }
    else
    {
        a = (double)F(float, s, 636) * v[2] + (double)F(float, s, 624) * v[1] + (double)v[0] * F(float, s, 612);
        b = (double)F(float, s, 640) * v[2] + (double)F(float, s, 628) * v[1] + (double)F(float, s, 616) * v[0];
        c = (double)F(float, s, 644) * v[2] + (double)F(float, s, 632) * v[1] + (double)F(float, s, 620) * v[0];
    }
    double x = c * F(float, s, 780) + b * F(float, s, 768) + a * F(float, s, 756);
    double y = c * F(float, s, 784) + b * F(float, s, 772) + a * F(float, s, 760);
    double z = c * F(float, s, 788) + b * F(float, s, 776) + a * F(float, s, 764);
    if (pos)
    {
        x += F(float, s, 792);
        y += F(float, s, 796);
        z += F(float, s, 800);
    }
    o[0] = (float)x;
    o[1] = (float)y;
    o[2] = (float)z;
}

// wheel i off the ground: no flag, a zero point and an upright normal
static void Ct_Off(BYTE* s, int i)
{
    s[10274 + i] = 0;
    Cr_Copy3(s + 10284 + i * 12, g_zero702650);
    Cr_Copy3(s + 10332 + i * 12, &g_702630);
}

extern "C" {

// [FUN_548250] fills the record out from the physics state w (the wheels at +352, 248 bytes each): touching
// wheels and their mean point, plane and normal, the heights of the free wheels, trails, the horn bits of
// +10164, the four driven wheels' points and normals, a mine hit, and 8 values of +21600
void __fastcall Car_TakeContacts(BYTE* s, void*, BYTE* out, BYTE* w)
{
    Vec3 sum = g_zeroVec, p[3] = {}, tmp;
    int cnt = 0, n[3] = {0, 0, 0};
    int i = 0;
    for (; i < F(int, s, 10428); i++)
    {
        BYTE* W = w + 352 + i * 248;
        if (F(WORD, W, 24))
        {
            CallThis(FUN_5ab710, &sum, &tmp, W);
            cnt++;
            for (int k = 0; k < 3; k++)
                if (!n[k]++)
                {
                    p[k] = *(Vec3*)W;
                    break;
                }
        }
        int t = F(int, w, 2716 + i * 4);
        F(int, out, 228 + i * 4) = F(WORD, W, 24) && t ? t : 0;
    }
    for (; i < F(int, s, 10424); i++)
        F(int, out, 228 + i * 4) = 0;
    if (out)
    {
        memcpy(out + 4, w, 48);
        F(int, out, 100) = F(int, w, 2424);
        for (int j = 0; j < F(int, s, 10428); j++)
        {
            char buf[200];
            F(int, out, 136 + j * 4) = F(int, w, 2428 + j * 4);
            sprintf(buf, &s_ct67a114, F(char*, F(BYTE*, s, 336), 12), j);
            CallC<double>(FUN_4e41d9, F(int, w, 2428 + j * 4), buf);
            F(int, out, 104 + j * 4) = F(int, w, 2392 + j * 4);
        }
        int mean = 0;
        Vec3 a = {}, b = {};
        if (F(int, s, 10424) > F(int, s, 10428))
            mean = CallC<int>(FUN_548130, w, cnt, &a, &b);
        const float* m = (const float*)w;
        for (int j = 0; j < F(int, s, 10424) - F(int, s, 10428); j++)
        {
            int k = F(int, s, 10428) + j;
            float& h = F(float, out, 104 + k * 4);
            if (!mean)
            {
                F(int, out, 104 + k * 4) = F(int, s, 10584 + k * 4);
                continue;
            }
            const float* q = (const float*)(s + 10712 + j * 12);
            Vec3 P, D, r;
            P.x = (float)((double)m[0] * q[0] + (double)m[3] * q[1] + (double)m[6] * q[2] + m[9]);
            P.y = (float)((double)m[4] * q[1] + (double)m[1] * q[0] + (double)m[7] * q[2] + m[10]);
            P.z = (float)((double)q[2] * m[8] + (double)m[5] * q[1] + (double)m[2] * q[0] + m[11]);
            D.x = (float)((double)g_axisZ.z * m[6] + (double)g_axisZ.y * m[3] + (double)g_axisZ.x * m[0]);
            D.y = (float)((double)g_axisZ.x * m[1] + (double)g_axisZ.z * m[7] + (double)g_axisZ.y * m[4]);
            D.z = (float)((double)g_axisZ.x * m[2] + (double)g_axisZ.z * m[8] + (double)g_axisZ.y * m[5]);
            CallC(FUN_4e6490, &r, &a, &b, &P, &D);
            double z = (double)r.x * m[14] + (double)r.z * m[20] + (double)r.y * m[17] + m[23];
            h = (float)((double)q[2] - F(float, w, 2756) - z);
            if (h > F(float, s, 10584 + k * 4))
                h = F(float, s, 10584 + k * 4);
            else if (h < g_fx64c264)
                h = 0;
        }
        Cr_Copy3(out + 52, w + 96);
        Cr_Copy3(out + 64, w + 108);
        Cr_Copy3(out + 76, w + 132);
        F(int, out, 172) = F(int, w, 348);
        if (!cnt)
            F(int, out, 176) = 0;
        else if (!F(int, w, 2388))
        {
            F(int, out, 176) = 1;
            Vec3* nrm = (Vec3*)(out + 192);
            if (cnt >= 3)
            {
                double ax = (double)p[2].x - p[0].x, ay = (double)p[2].y - p[0].y, az = (double)p[2].z - p[0].z;
                float bx = (float)((double)p[1].x - p[0].x), by = (float)((double)p[1].y - p[0].y);
                double bz = (double)p[1].z - p[0].z;
                Vec3 c, r;
                c.x = (float)(az * by - ay * bz);
                c.y = (float)(bz * ax - az * bx);
                c.z = (float)(ay * bx - by * ax);
                *nrm = *Vec3_Normalized(&r, &c);
                double d = (double)nrm->z * F(float, w, 344) + (double)nrm->y * F(float, w, 340) + (double)nrm->x * F(float, w, 336);
                CallThis(FUN_5ab620, nrm, &tmp, (float)(d > g_cs64b678 ? 1 : -1));
            }
            else
                *nrm = F(Vec3, w, 336);
            if (F(float, out, 200) < g_fx64b6e0)
                F(int, out, 176) = 0;
            else
            {
                double c = cnt;
                F(float, out, 180) = (float)(sum.x / c);
                F(float, out, 184) = (float)(sum.y / c);
                F(float, out, 188) = (float)(sum.z / c);
                F(int, out, 204) = F(int, w, 2352);
                F(int, out, 208) = F(int, w, 2356);
                F(int, out, 212) = F(int, w, 2348);
            }
        }
        if ((out[264] & 16) && g_ct6961f8)
        {
            for (int j = 0; j < F(int, s, 10428); j++)
            {
                int* e = &F(int, w, 2784 + j * 4);
                if (!*e)
                    continue;
                BYTE* t = w + 2848 + j * 208;
                if (!g_ct6cec68)
                    F(BYTE*, t, 104) = s ? s + 272 : 0;
                int k = (j + 2) % 4;
                float v = F(float, w, 2816 + j * 4);
                if (v > F(float, w, 2816 + k * 4))
                    v = (float)(((double)v + F(float, w, 2816 + k * 4)) * g_vh64c258);
                CallThis(FUN_4bcd00, g_ct6961f8, t, v, j, *e - 1);
                *e = 0;
            }
        }
        int horn = F(int, w, 4648);
        if (horn && (out[264] & 64))
        {
            F(int, s, 10164) |= horn == 2 ? 0x10000 : 0x8000;
            F(int, w, 4648) = 0;
        }
        else
            F(int, s, 10164) &= ~0x8000;
        F(int, s, 10164) &= ~0x10000;
    }
    F(int, out, 220) = F(int, w, 2772);
    F(int, out, 224) = F(int, w, 2776);
    F(int, out, 260) = F(int, w, 2780);
    if (F(int, out, 216) > 2)
        F(int, out, 216) = 2;
    int g = 2;
    if (!F(int, w, 4636))
        g = F(int, out, 216) - 1;
    F(int, out, 216) = g < 0 ? 0 : g;
    Cr_Copy3(out + 88, w + 2336);
    s[10273] = 0;
    for (int i = 0; i < 4; i++)
    {
        int k = F(int, s, 20400 + i * 4);
        BYTE* W = w + 352 + k * 248;
        if (!F(WORD, W, 24))
        {
            Ct_Off(s, i);
            continue;
        }
        s[10273] |= 1 << i;
        int* o = F(int*, W, 132);
        s[10274 + i] = o && *o == 1 ? 1 : 0;
        Ct_Through(s, (const float*)W, (float*)(s + 10284 + i * 12), true);
        Ct_Through(s, (const float*)(W + 12), (float*)(s + 10332 + i * 12), false);
        if (x87_fabs((double)F(float, s, 10292 + i * 12) - F(float, s, 10280)) > g_ct64f728)
            Ct_Off(s, i);
    }
    BYTE* mine = F(BYTE*, w, 4712);
    if (mine)
    {
        CallC(FUN_56f240, mine);
        Vec3 v;
        v.x = (float)((double)F(float, w, 4716) * g_ct64f720);
        v.y = (float)((double)F(float, w, 4720) * g_ct64f720);
        v.z = (float)((double)F(float, w, 4724) * g_ct64f720);
        CallVirt(s + 16, 164, w + 4728, &v);
    }
    for (int i = 0; i < 8; i++)
        F(int, s, 21120 + i * 4) = F(int, F(BYTE*, s, 21600), 15580 + i * 20);
}

} // extern "C"

// === moving the vehicle's centre of mass (FUN_5455d0)

extern "C" void FUN_542280(); KING_ALIAS("_FUN_542280", "@Car_Centre@4")
extern "C" void FUN_54c820(); KING_ALIAS("_FUN_54c820", "@Car_Place@16")
extern "C" void FUN_545e20(); KING_ALIAS("_FUN_545e20", "@Car_SendState@4")
extern "C" void FUN_4ee540(); KING_ALIAS("_FUN_4ee540", "@Body_SetMass@20")

static Vec3 Cm_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_5455d0] adds mass m at point *v to the vehicle (+9552 its mass): the new centre makes the frames
// +756 / +708, and the type's points, axle planes, wheels and the body are placed again from it
int __fastcall Car_AddMass(BYTE* s, void*, float m, const Vec3* v)
{
    Matrix m192;
    Matrix_Multiply(&m192, (const Matrix*)(s + 708), (const Matrix*)(s + 516));
    int idx = F(int, s, 10420);
    BYTE* B = (BYTE*)g_carTypes697914 - 76 + idx * 2416;
    BYTE* C = (BYTE*)g_cr6d20f4 + idx * 1488;
    double w = F(float, s, 9552);
    float total = (float)(w + m);
    const float* c = (const float*)(C + 736);
    float vx = (float)((double)v->x * m), vy = (float)((double)v->y * m), vz = (float)((double)v->z * m);
    float cx = (float)(c[0] * w), cy = (float)(c[1] * w);
    double cz = c[2] * w;
    float sx = (float)((double)cx + vx), sy = (float)((double)vy + cy);
    double sz = cz + vz;
    Vec3 ctr = Cm_Set(sx / (double)total, sy / (double)total, sz / total);
    F(float, s, 412) = total;
    F(float, s, 3284) = total;
    CallThis(FUN_542580, s);
    Matrix t;
    memcpy(s + 756, Matrix_Translation(&t, ctr), 48);
    memcpy(s + 708, Matrix_Inverse(&t, (const Matrix*)(s + 756)), 48);
    const float* R = (const float*)(s + 708);
    {
        const float* A = (const float*)(C + 796);
        F(Vec3, s, 3452) = Cm_Set((double)R[6] * A[2] + (double)R[3] * A[1] + (double)R[0] * A[0] + R[9],
                                  (double)R[7] * A[2] + (double)R[4] * A[1] + (double)R[1] * A[0] + R[10],
                                  (double)R[8] * A[2] + (double)R[5] * A[1] + (double)R[2] * A[0] + R[11]);
    }
    {
        const float* p = (const float*)(s + 20356);
        F(Vec3, s, 20332) = Cm_Set((double)R[6] * p[2] + (double)R[3] * p[1] + (double)p[0] * R[0] + R[9],
                                   (double)R[7] * p[2] + (double)R[1] * p[0] + (double)R[4] * p[1] + R[10],
                                   (double)R[8] * p[2] + (double)R[2] * p[0] + (double)R[5] * p[1] + R[11]);
        p = (const float*)(s + 20368);
        F(Vec3, s, 20344) = Cm_Set((double)R[6] * p[2] + (double)R[3] * p[1] + (double)R[0] * p[0] + R[9],
                                   (double)R[7] * p[2] + (double)R[4] * p[1] + (double)R[1] * p[0] + R[10],
                                   (double)R[8] * p[2] + (double)R[5] * p[1] + (double)R[2] * p[0] + R[11]);
    }
    F(Vec3, s, 3464) = F(Vec3, s, F(int, s, 20328) ? 20332 : 20344);
    for (int j = 0; j < F(int, B, 1300); j++)
    {
        const float* q = (const float*)(B + 1304 + j * 12);
        F(Vec3, s, 3848 + j * 12) = Cm_Set((double)q[2] * R[6] + (double)q[1] * R[3] + (double)q[0] * R[0] + R[9],
                                           (double)q[1] * R[4] + (double)q[0] * R[1] + (double)q[2] * R[7] + R[10],
                                           (double)q[1] * R[5] + (double)q[0] * R[2] + (double)q[2] * R[8] + R[11]);
    }
    Vec3 r;
    int n1 = F(int, B, 776);
    for (int j = 0; j < F(int, (BYTE*)g_carTypes697914 - 76 + F(int, s, 10420) * 2416, 776); j++)
        F(Vec3, s, 3608 + j * 12) = *Vec3_MulMatrix(&r, (const Vec3*)(B + 784 + j * 12), (const Matrix*)R);
    for (int j = 0; j < F(int, (BYTE*)g_carTypes697914 - 76 + F(int, s, 10420) * 2416, 780); j++)
    {
        n1 = F(int, (BYTE*)g_carTypes697914 - 76 + F(int, s, 10420) * 2416, 776);
        F(Vec3, s, 3608 + (n1 + j) * 12) = *Vec3_MulMatrix(&r, (const Vec3*)(B + 1024 + j * 12), (const Matrix*)R);
    }
    for (int j = 0; j < F(int, s, 10424) - F(int, s, 10428); j++)
        F(Vec3, s, 10712 + j * 12) = *Vec3_MulMatrix(&r, (const Vec3*)(s + 10616 + j * 12), (const Matrix*)R);
    CallThis(FUN_58d2b0, s + 16);
    CallThis(FUN_542280, s);
    if (F(BYTE*, s, 21600))
        CallThis(FUN_4ee540, F(BYTE*, s, 21600), total, s + 756, s + 420);
    Matrix m288;
    Matrix_Multiply(&m288, (const Matrix*)(s + 756), &m192);
    float hit[3];
    BYTE* q = CallC<BYTE*>(FUN_4e5ba7, F(BYTE*, s, 816), s + 552, hit);
    if (!q)
    {
        q = F(BYTE*, s, 816);
        CallVirt(s + 16, 28, 1);
    }
    CallThis(FUN_54c820, s, q, &m288);
    {
        Matrix mt, mr, m1, m2;
        Matrix_Translation(&mt, F(Vec3, B, 300));
        Matrix_Multiply(&m1, Matrix_RotationX(&mr, F(float, B, 312)), &mt);
        memcpy(s + 10036, Matrix_Multiply(&m2, &m1, (const Matrix*)R), 48);
    }
    if (F(int, s, 388))
        return F(int, s, 388);
    return CallThis<int>(FUN_545e20, s);
}

} // extern "C"

// === a hit on the vehicle (a method at $L_554c20)

extern "C" void Vh_RampValue(); KING_ALIAS("_Vh_RampValue", "@Ramp_Value@8")
extern "C" void Vh_CabHit(); KING_ALIAS("_Vh_CabHit", "@Cab_Hit@20")
extern "C" void Vh_Advance(); KING_ALIAS("_Vh_Advance", "@Progresses_Advance@24")
extern "C" void Vh_MulAssign(); KING_ALIAS("_Vh_MulAssign", "@Vec3_MulAssign@16")
extern "C" void FUN_54f040(); KING_ALIAS("_FUN_54f040", "@Car_GiveWay@8")
extern "C" void FUN_4bb4c0(); KING_ALIAS("_FUN_4bb4c0", "_Fx_DebrisBurst")
KING_VAR(float, g_vh64d4e8, 64d4e8)
KING_VAR(double, g_vh64b5b8, 64b5b8)
KING_VAR(double, g_vh64b840, 64b840)
KING_VAR(float, g_vh64c40c, 64c40c)
KING_VAR(float, g_vh64f7b0, 64f7b0)
KING_VAR(float, g_vh64c32c, 64c32c)
KING_VAR(double, g_vh64d0f0, 64d0f0)
KING_VAR(float, g_vh64c450, 64c450)
KING_VAR(float, g_vh64ed88, 64ed88)
KING_VAR(double, g_vh64ed80, 64ed80)
KING_VAR(double, g_vh64b670, 64b670)
KING_VAR(float, g_vh64c320, 64c320)
KING_VAR(float, g_vh64c250, 64c250)
KING_VAR(double, g_vh64ed78, 64ed78)
KING_VAR(double, g_vh64b6e0, 64b6e0)
KING_VAR(double, g_vh64f7a8, 64f7a8)
KING_VAR(double, g_vh64f7a0, 64f7a0)

// e to the -(y * y), the way the original's inline code works it out
X87 double Vh_ExpNegSq(double y)
{
    __asm {
        fld y
        fld st(0)
        fmul st(0), st(1)
        fchs
        fldl2e
        fmulp st(1), st(0)
        fld st(0)
        frndint
        fxch st(1)
        fsub st(0), st(1)
        f2xm1
        fld1
        faddp st(1), st(0)
        fscale
        fstp st(1)
        fstp st(1)
    }
}

// damage progress k by v (scaled down for +21540 when half), counting in *n and *flag
static void Vh_Damage(BYTE* s, int k, double q, float strength, bool half, int* n, int* flag)
{
    if (!(q > g_vh64c264))
        return;
    float e = (float)(q * q * strength);
    if (half && F(int, s, 21540))
        e = (float)(e * g_vh64b840);
    CallThis(Vh_Advance, s, k, e, n, flag);
}

extern "C" {

// [$L_554c20] a hit on vehicle s (owner at -16) at point p with impulse dir: hit effects, the cab, damage of
// the 8 kinds of parts by the hit's direction in the body frame, and debris thrown from the point
void __fastcall Car_Hit(BYTE* s, void*, const Vec3* p, const Vec3* dir)
{
    if (!(s[20884] & 128) || F(int, s, 372))
        return;
    BYTE* owner = s - 16;
    CallThis(FUN_54f040, owner);
    double r = CallThis<double>(Vh_RampValue, s + 21136);
    Vec3 imp;
    imp.x = (float)(r * dir->x);
    imp.y = (float)(r * dir->y);
    imp.z = (float)(r * dir->z);
    double L = Vec3_Length(&imp) / F(float, s, 396);
    float Lf = (float)L;
    if (L > g_vh64d4e8)
    {
        F(float, s, 20604) = (float)F(double, g_6ef1fc, 2968);
        F(int, s, 20592) = 0;
    }
    if (F(int, s, 21584) && L > g_vh64b5b8 && !F(int, s, 1028) && x87_fabs(F(float, s, 984)) < g_vh64b840
        && !F(int, s, 10792))
    {
        F(int, s, 10792) = 1;
        F(Vec3, s, 10796) = *p;
    }
    if (L < g_vh64c40c)
        return;
    CallVirt(owner, 180, p, &imp);
    float k = 1.0f;
    BYTE* B = (BYTE*)g_carTypes697914 - 76 + F(int, s, 10404) * 2416;
    if (F(int, B, 2280) == 3)
        k = F(int, s, 1028) ? 0.1f : 0.3f;
    if ((F(DWORD, s, 20884) & 0x100) && F(int, s, 892) && !F(int, s, 1028))
    {
        if ((double)k * g_vh64f7b0 < Lf || ((double)k * g_vh64c32c < Lf && F(float, s, 532) < g_vh64d0f0))
            CallThis(FUN_549900, s + 21552, Lf);
    }
    double x = (double)Lf - g_vh64c40c;
    double w = g_vh64b670 - Vh_ExpNegSq((x - g_vh64c450) * g_vh64ed88) * g_vh64ed80;
    float strength = (float)(((x * g_vh64d4e8) / (x + g_vh64c320) + g_vh64c250) * w * x);
    Vec3 v, n;
    v.x = (float)((double)F(float, s, 608) * p->y + (double)F(float, s, 620) * p->z + (double)F(float, s, 596) * p->x + F(float, s, 632));
    v.y = (float)((double)F(float, s, 600) * p->x + (double)F(float, s, 612) * p->y + (double)F(float, s, 624) * p->z + F(float, s, 636));
    v.z = (float)((double)F(float, s, 604) * p->x + (double)F(float, s, 616) * p->y + (double)F(float, s, 628) * p->z + F(float, s, 640));
    Vec3_Normalized(&n, &v);
    if (F(BYTE*, s, 21584))
        CallThis(Vh_CabHit, F(BYTE*, s, 21584), &v, &imp, strength);
    if (x87_fabs(v.y) < v.z && x87_fabs(v.x) < v.z)
        return;
    int cnt = 0, flag = 0;
    if (n.y > g_vh64c264)
    {
        float e = (float)((double)n.y * n.y * strength);
        if (F(int, s, 21540))
            e = (float)(e * g_vh64b840);
        CallThis(Vh_Advance, s, 1, e, &cnt, &flag);
        CallThis(Vh_Advance, s, 2, e, &cnt, &flag);
        CallThis(Vh_Advance, s, 3, e, &cnt, &flag);
    }
    Vh_Damage(s, 2, ((double)n.x + n.y) * g_vh64ed78, strength, true, &cnt, &flag);
    Vh_Damage(s, 3, n.y * g_vh64ed78 - n.x * g_vh64ed78, strength, true, &cnt, &flag);
    int before = cnt;
    if (cnt > 0)
    {
        F(int, s, 21616) = 1;
        F(Vec3, s, 21708) = *p;
        F(float, s, 21792) = 1.0f;
    }
    Vh_Damage(s, 8, -(double)n.y, strength, false, &cnt, &flag);
    Vh_Damage(s, 6, n.x * g_vh64b6e0 - n.y * g_vh64f7a8, strength, false, &cnt, &flag);
    Vh_Damage(s, 7, n.y * g_vh64f7a0 - n.x * g_vh64b6e0, strength, false, &cnt, &flag);
    if (n.x > g_vh64c264)
        CallThis(Vh_Advance, s, 4, (float)((double)n.x * strength), &cnt, &flag);
    if (-(double)n.x > g_vh64c264)
        CallThis(Vh_Advance, s, 5, (float)(-(double)n.x * strength), &cnt, &flag);
    if (before < cnt)
    {
        if (cnt - before == 1)
        {
            F(int, s, 21608) = 1;
            F(Vec3, s, 21684) = *p;
            F(float, s, 21784) = 1.0f;
        }
        else
        {
            F(int, s, 21612) = 1;
            F(Vec3, s, 21696) = *p;
            F(float, s, 21788) = 1.0f;
        }
    }
    if (!cnt)
        return;
    const float* w4 = (const float*)(s + 428);
    double cx = (double)w4[1] * v.z - (double)w4[2] * v.y;
    double cy = (double)w4[2] * v.x - (double)w4[0] * v.z;
    double cz = (double)w4[0] * v.y - (double)w4[1] * v.x;
    const float* R = (const float*)(s + 500);
    float wx = (float)(cz * R[6] + cy * R[3] + cx * R[0]);
    float wy = (float)(cx * R[1] + cz * R[7] + cy * R[4]);
    double wz = cx * R[2] + cz * R[8] + cy * R[5];
    Vec3 vel, t;
    vel.x = (float)((double)wx + F(float, s, 416));
    vel.y = (float)((double)wy + F(float, s, 420));
    vel.z = (float)(wz + F(float, s, 424));
    int kind = cnt < 2 ? 4 : (cnt >= 3 ? 10 : 7);
    if (flag)
        kind += kind;
    float size = 0.15f;
    float spd = (float)Vec3_Length(&vel);
    if (flag)
    {
        DWORD bits = 0x3e666667;
        memcpy(&size, &bits, 4);
        spd = (float)((x87_sqrt(flag) + g_vh64b670) * spd);
    }
    CallThis(Vh_MulAssign, &vel, &t, 0.5f);
    double lim = spd * g_vh64b5b8;
    if (vel.z < lim)
        vel.z = (float)lim;
    CallC(FUN_4bb4c0, p, s + 976, &vel, spd, kind, size, 1);
}

} // extern "C"

// === whether a trailer can be hitched (FUN_55f000)

// p through the 12-float frame M (with its position)
static Vec3 Th_Frame(const float* M, const float* p)
{
    Vec3 v;
    v.z = (float)((double)p[2] * M[8] + (double)p[1] * M[5] + (double)p[0] * M[2] + M[11]);
    v.y = (float)((double)p[2] * M[7] + (double)p[1] * M[4] + (double)p[0] * M[1] + M[10]);
    v.x = (float)((double)p[2] * M[6] + (double)p[1] * M[3] + (double)p[0] * M[0] + M[9]);
    return v;
}

extern "C" {

// [FUN_55f000] 0 when trailer t can be hitched to car c, else why not: 1 one missing, 2 not a car and a trailer,
// 3 either without +21600, 4 c already towing (+21880), 5 the hitch points (types +156 / +168, the trailer's
// raised by the car type's +204) too far apart, 6 their heights over the ground (+21048) too different,
// 7 facing too differently
int __cdecl Trailer_HitchCheck(BYTE* c, BYTE* t)
{
    if (!t || !c)
        return 1;
    if (CallVirt<int>(c, 4) != 1 || CallVirt<int>(t, 4) != 2)
        return 2;
    if (!F(int, t, 21600) || !F(int, c, 21600))
        return 3;
    if (F(int, c, 21880))
        return 4;
    BYTE* tc = (BYTE*)g_697f9c + F(int, c, 10420) * 2416;
    BYTE* tt = (BYTE*)g_697f9c + F(int, t, 10420) * 2416;
    Matrix m;
    float M[12];
    memcpy(M, c + 708, 48);
    Vec3 p = Th_Frame(M, (const float*)(tc + 156));
    Vec3 ha = Th_Frame(CallThis<const float*>(FUN_57de90, c + 16, &m), &p.x);
    memcpy(M, t + 708, 48);
    float k = F(float, tc, 204);
    Vec3 v;
    v.z = (float)((double)k * g_axisY.z);
    v.y = (float)((double)k * g_axisY.y);
    v.x = (float)((double)k * g_axisY.x);
    const float* o = (const float*)(tt + 168);
    Vec3 cc;
    cc.z = (float)((double)v.z + o[2]);
    cc.y = (float)((double)v.y + o[1]);
    cc.x = (float)((double)v.x + o[0]);
    Vec3 q = Th_Frame(M, &cc.x);
    Vec3 hb = Th_Frame(CallThis<const float*>(FUN_57de90, t + 16, &m), &q.x);
    Vec3 d;
    Vec3_Sub(&d, &ha, &hb);
    if (Vec3_Dot(&d, &d) > g_cr64c2f8)
        return 5;
    Vec3 r;
    float z1 = (float)((double)Vec3_MulMatrix(&r, (const Vec3*)(tt + 168), (const Matrix*)(t + 708))->z - F(float, t, 21048));
    double z2 = (double)Vec3_MulMatrix(&r, (const Vec3*)(tc + 156), (const Matrix*)(c + 708))->z - F(float, c, 21048);
    if (x87_fabs(z1 - z2) > g_wo64d0f0)
        return 6;
    Vec3 fa, fb;
    Vec3_RotateBy(&fa, &g_axisY, CallThis<const Matrix*>(FUN_57de90, c + 16, &m));
    Vec3_RotateBy(&fb, &g_axisY, CallThis<const Matrix*>(FUN_57de90, t + 16, &m));
    if (Vec3_Dot(&fa, &fb) < g_lw64b840)
        return 7;
    return 0;
}

} // extern "C"

// === the lamp the car looks at (FUN_54cb90)

extern "C" void Lk_TransformVecs(); KING_ALIAS("_Lk_TransformVecs", "_Obj_TransformVecs")
extern "C" void Lk_Negated(); KING_ALIAS("_Lk_Negated", "_Vec3_Negated")
extern "C" void Lk_AddAssign(); KING_ALIAS("_Lk_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Lk_SubAssign(); KING_ALIAS("_Lk_SubAssign", "@Vec3_SubAssign@16")
KING_VAR(double, g_lk64f760, 64f760)
KING_VAR(double, g_lk64b678, 64b678)
KING_VAR(char, s_lk67a2d8, 67a2d8)
KING_VAR(char, s_lk67a2cc, 67a2cc)

extern "C" {

// [FUN_54cb90] car s's ray (from its point +3464 in both frames, +564 and +516, along its forward) into the world
// +10920 (kind 7); a hit object (of kind 77 when +10924 is 1) has its two switches named after it turned on, the
// second showing which side (1 .. 4 by the hit normal) the car looks at
void __fastcall Car_LookAtSign(BYTE* s, void*)
{
    if (!F(BYTE*, s, 10920))
        return;
    BYTE nd[256];
    CallThis(FUN_5ef5c0, nd);
    const float* p = (const float*)(s + 3464);
    memcpy(nd + 4, &g_identity, 48);
    float* from = (float*)(nd + 76);
    from[0] = (float)((double)F(float, s, 576) * p[1] + (double)F(float, s, 588) * p[2] + (double)p[0] * F(float, s, 564) + F(float, s, 600));
    from[1] = (float)((double)F(float, s, 568) * p[0] + (double)F(float, s, 580) * p[1] + (double)F(float, s, 592) * p[2] + F(float, s, 604));
    from[2] = (float)((double)F(float, s, 572) * p[0] + (double)F(float, s, 584) * p[1] + (double)F(float, s, 596) * p[2] + F(float, s, 608));
    const float* Y = &g_axisY.x;
    Vec3 u, q, w, tmp;
    u.x = (float)((double)Y[2] * F(float, s, 540) + (double)Y[1] * F(float, s, 528) + (double)Y[0] * F(float, s, 516));
    u.y = (float)((double)Y[2] * F(float, s, 544) + (double)Y[1] * F(float, s, 532) + (double)Y[0] * F(float, s, 520));
    u.z = (float)((double)Y[2] * F(float, s, 548) + (double)Y[1] * F(float, s, 536) + (double)Y[0] * F(float, s, 524));
    q.z = (float)((double)F(float, s, 548) * p[2] + (double)F(float, s, 536) * p[1] + (double)F(float, s, 524) * p[0] + F(float, s, 560));
    q.y = (float)((double)F(float, s, 544) * p[2] + (double)F(float, s, 532) * p[1] + (double)F(float, s, 520) * p[0] + F(float, s, 556));
    q.x = (float)((double)F(float, s, 540) * p[2] + (double)F(float, s, 528) * p[1] + (double)p[0] * F(float, s, 516) + F(float, s, 552));
    CallThis(Lk_AddAssign, nd + 88, &tmp, &q);
    w.x = (float)((double)F(float, s, 552) - F(float, s, 600));
    w.y = (float)((double)F(float, s, 556) - F(float, s, 604));
    w.z = (float)((double)F(float, s, 560) - F(float, s, 608));
    if ((double)w.z * u.z + (double)w.y * u.y + (double)w.x * u.x > g_lk64b678)
        CallThis(Lk_AddAssign, nd + 88, &tmp, &u);
    else
        CallThis(Lk_SubAssign, nd + 88, &tmp, &u);
    F(int, nd, 0) = F(int, s, 3300);
    F(int, nd, 116) = 0;
    F(int, nd, 100) = 0;
    int res = CallVirt<int>(F(BYTE*, s, 10920), 52, 7, nd);
    CallC(Lk_TransformVecs, nd);
    int* h = F(int*, nd, 100);
    if (!(res & 1) || !h)
        return;
    if (F(int, s, 10924) == 1 && h[0] != 77)
        return;
    char b1[32], b2[32];
    sprintf(b1, &s_lk67a2d8, h[1]);
    sprintf(b2, &s_lk67a2cc, h[1]);
    BYTE* a = CallC<BYTE*>(FUN_5ef2b0, 0, b1);
    BYTE* b = CallC<BYTE*>(FUN_5ef2b0, 0, b2);
    if (!a)
        return;
    CallC(FUN_4e480c, a, 1);
    if (!b)
        return;
    Vec3 n;
    n.x = (float)-(double)F(float, nd, 64);
    n.y = (float)-(double)F(float, nd, 68);
    n.z = (float)-(double)F(float, nd, 72);
    if (Vec3_Dot(&n, &g_axisY) > g_lk64f760)
        CallC(FUN_4e480c, b, 1);
    else if (Vec3_Dot(&n, &g_axisX) > g_lk64f760)
        CallC(FUN_4e480c, b, 2);
    else if (Vec3_Dot(&n, CallC<const Vec3*>(Lk_Negated, &tmp, &g_axisY)) > g_lk64f760)
        CallC(FUN_4e480c, b, 3);
    else if (Vec3_Dot(&n, CallC<const Vec3*>(Lk_Negated, &tmp, &g_axisX)) > g_lk64f760)
        CallC(FUN_4e480c, b, 4);
}

} // extern "C"

// === the hitch's impulse between car and trailer (FUN_55cc30)

extern "C" void Hj_SetThis(); KING_ALIAS("_Hj_SetThis", "@Vec3_SetThis@20")
extern "C" void Hj_ZeroThis(); KING_ALIAS("_Hj_ZeroThis", "@Vec3_ZeroThis@8")
typedef void(__cdecl* Hj_Solver)(int, int, void*, void*, void*, void*, void*, void*, void*, void*, void*, void*,
                                 void*, void*, void*, void*, void*, void*);
KING_VAR(Hj_Solver, g_hj67a4ac, 67a4ac)

extern "C" {

// [FUN_55cc30] when car s has a pending hitch event (+21888) with its trailer +21880: both hitch points (car
// type +156, trailer type +168 raised by the car type's +204) in body axes, the bodies' frames, masses, velocities
// and spins go to the hitch solver 67a4ac, whose impulses and turns go to both bodies (+16 methods 144 / 152)
void __fastcall Car_HitchImpulse(BYTE* s, void*)
{
    if (!F(int, s, 21888))
        return;
    BYTE* t = F(BYTE*, s, 21880);
    if (!t)
        return;
    F(int, s, 21888) = 0;
    int mt = F(int, t, 3284);
    F(int, t, 21884) = 0;
    int mc = F(int, s, 3284);
    float A1[12], B1[12], A2[12], B2[12], R[12];
    memcpy(A1, s + 1808, 48);
    memcpy(B1, t + 1808, 48);
    memcpy(A2, s + 1856, 48);
    memcpy(B2, t + 1856, 48);
    Vec3 va = F(Vec3, s, 3304), vb = F(Vec3, t, 3304);
    memcpy(R, s + 708, 48);
    BYTE* tc = (BYTE*)g_697f9c + F(int, s, 10420) * 2416;
    BYTE* tt = (BYTE*)g_697f9c + F(int, t, 10420) * 2416;
    const float* h = (const float*)(tc + 156);
    float P[3];
    P[0] = (float)((double)R[0] * h[0] + (double)R[6] * h[2] + (double)R[3] * h[1] + R[9]);
    P[1] = (float)((double)R[1] * h[0] + (double)R[7] * h[2] + (double)R[4] * h[1] + R[10]);
    P[2] = (float)((double)R[2] * h[0] + (double)R[8] * h[2] + (double)R[5] * h[1] + R[11]);
    double k = F(float, tc, 204);
    memcpy(R, t + 708, 48);
    float ky = (float)(k * g_axisY.y);
    float cx = (float)((double)(float)(k * g_axisY.x) + F(float, tt, 168));
    float cy = (float)((double)ky + F(float, tt, 172));
    double cz = k * g_axisY.z + F(float, tt, 176);
    Vec3 Qt, Pl, Ql, wa, wb, tmp;
    Qt.z = (float)(cz * R[8] + (double)cy * R[5] + (double)R[2] * cx + R[11]);
    Qt.y = (float)(cz * R[7] + (double)cy * R[4] + (double)R[1] * cx + R[10]);
    Qt.x = (float)(cz * R[6] + (double)cy * R[3] + (double)R[0] * cx + R[9]);
    Pl.z = (float)((double)A1[8] * P[2] + (double)A1[5] * P[1] + (double)A1[2] * P[0]);
    Pl.y = (float)((double)A1[7] * P[2] + (double)A1[4] * P[1] + (double)A1[1] * P[0]);
    Pl.x = (float)((double)A1[6] * P[2] + (double)A1[3] * P[1] + (double)A1[0] * P[0]);
    Vec3_RotateBy(&Ql, &Qt, (const Matrix*)B1);
    Vec3 ga = F(Vec3, s, 21896), gb = F(Vec3, t, 21888);
    Vec3_RotateBy(&wa, Vec3_MulComponents(&tmp, &F(Vec3, s, 21908), &va), (const Matrix*)A1);
    Vec3_RotateBy(&wb, Vec3_MulComponents(&tmp, &F(Vec3, t, 21900), &vb), (const Matrix*)B1);
    Vec3 o1 = g_zeroVec, o3 = g_zeroVec, o2, o4;
    CallThis(Hj_ZeroThis, &o2);
    CallThis(Hj_ZeroThis, &o4);
    g_hj67a4ac(mc, mt, &Pl, &Ql, &ga, &gb, &wa, &wb, A1, B1, A2, B2, &va, &vb, &o1, &o3, &o2, &o4);
    CallVirt(s + 16, 144, &o1, 0);
    CallVirt(s + 16, 152, &o2, 0);
    CallVirt(t + 16, 144, &o3, 0);
    CallVirt(t + 16, 152, &o4, 0);
}

} // extern "C"

// === the cab view's sway with the road (FUN_4decc0)

extern "C" void Cw_GameTime(); KING_ALIAS("_Cw_GameTime", "_Game_Time")
extern "C" void Cw_FrameApply(); KING_ALIAS("_Cw_FrameApply", "_Frame_ApplyChecked")
KING_VAR(BYTE*, g_cw6d2078, 6d2078)
KING_VAR(BYTE*, g_cw696cac, 696cac)
KING_VAR(float, g_cw696d10, 696d10)
KING_VAR(char, s_cw672eec, 672eec)
KING_VAR(char, s_cw672edc, 672edc)
KING_VAR(double, g_cw64d688, 64d688)
KING_VAR(double, g_cw64d6b8, 64d6b8)
KING_VAR(double, g_cw64d6b0, 64d6b0)
KING_VAR(float, g_cw64c260, 64c260)
KING_VAR(float, g_cw64d6ac, 64d6ac)
KING_VAR(float, g_cw64d6a8, 64d6a8)
KING_VAR(double, g_cw64b898, 64b898)
KING_VAR(float, g_cw64c264, 64c264)
KING_VAR(double, g_cw64b6a8, 64b6a8)
KING_VAR(double, g_cw64b6e0, 64b6e0)
KING_VAR(double, g_cw64b6b8, 64b6b8)
KING_VAR(float, g_cw64d6a4, 64d6a4)
KING_VAR(float, g_cw64d6a0, 64d6a0)
KING_VAR(double, g_cw64d698, 64d698)
KING_VAR(float, g_cw64d670, 64d670)
KING_VAR(float, g_cw64c408, 64c408)
KING_VAR(float, g_cw64d694, 64d694)
KING_VAR(float, g_cw64c304, 64c304)
KING_VAR(float, g_cw64d690, 64d690)

static float Cw_Bits(DWORD b)
{
    float f;
    memcpy(&f, &b, 4);
    return f;
}

extern "C" {

// [FUN_4decc0] cab view s in the game (6cec68): its seat frame (+560 + 48 * +4720, turned with the cab's tilt
// +21600 +10720) and the camera (FUN_4de990) swayed by the cab's bounce and roll through the filters +4756,
// +4788 and, with option 0x100, leaned and shifted by the car's pitch and roll (+3360, +3372) through +4724;
// then applied to the camera node (696cac +80) and the seat (+556). Also the look-back times (+5104 ..)
void __fastcall Cab_ViewSway(BYTE* s, void*)
{
    BYTE* g = g_cw6d2078;
    if (!g_q234_6cec68 || !g || !F(BYTE*, g, 616) || F(int, g, 1400))
        return;
    if (F(BYTE*, s, 544))
        F(BYTE*, s, 544)[40] &= 0xfe;
    float M[12];
    memcpy(M, s + 560 + F(int, s, 4720) * 48, 48);
    BYTE* car = F(BYTE*, g, 616);
    if (F(BYTE*, car, 21600))
    {
        Matrix rz, r;
        Matrix_RotationZ(&rz, (float)(F(float, F(BYTE*, car, 21600), 10720) * g_cw64d688));
        memcpy(M, Matrix_Multiply(&r, &rz, (const Matrix*)M), 48);
    }
    int look = F(int, g, 1396);
    if (look < 0)
    {
        F(float, s, 5116) = (float)CallC<double>(Cw_GameTime);
        F(float, s, 5112) = (float)(CallC<double>(Cw_GameTime) - F(float, s, 5104));
    }
    else if (look > 0)
    {
        F(float, s, 5120) = (float)CallC<double>(Cw_GameTime);
        F(float, s, 5108) = (float)(CallC<double>(Cw_GameTime) - F(float, s, 5104));
    }
    else
        F(float, s, 5104) = (float)CallC<double>(Cw_GameTime);
    float C[12];
    CallC(FUN_4de990, C, 1);
    g = g_cw6d2078;
    car = F(BYTE*, g, 616);
    BYTE* tr = F(BYTE*, car, 21600);
    if (tr)
    {
        Vec3 P = F(Vec3, tr, 13228);
        const float* R = (const float*)(car + 708);
        double L = Vec3_Length(&F(Vec3, tr, 10868));
        const float* d = (const float*)(g + 556);
        float a = (float)((double)R[1] * d[0] + (double)R[7] * d[2] + (double)R[4] * d[1] + R[10]);
        float b = (float)((double)R[2] * d[0] + (double)R[8] * d[2] + (double)R[5] * d[1] + R[11]);
        float aL = (float)(a * L);
        double x = ((L * b + P.z) + g_cw64d6b8) * g_cw64d6b0;
        float xf = (float)(x * x87_fabs(x));
        if (xf > g_cw64c260)
            xf = Cw_Bits(0x3e4ccccd);
        else if (xf < g_cw64d6ac)
            xf = Cw_Bits(0xbe4ccccd);
        CallThis(FUN_4eacce, s + 4756, xf, &s_cw672eec);
        double f1 = CallThis<double>(Fv_FilterValue, s + 4756);
        C[11] = (float)(C[11] + f1);
        double y = ((double)aL + P.y) * g_cw64d6a8 * g_cw64b898;
        float yf = (float)(y * x87_fabs(y));
        if (yf < g_cw64d6ac)
            yf = Cw_Bits(0xbe4ccccd);
        else if (yf > g_cw64c260)
            yf = Cw_Bits(0x3e4ccccd);
        CallThis(FUN_4eacce, s + 4788, yf, &s_cw672eec);
        double f2 = CallThis<double>(Fv_FilterValue, s + 4788);
        if (f2 > g_cw64c260)
            f2 = g_cw64c260;
        else if (f2 < g_cw64d6ac)
            f2 = g_cw64d6ac;
        C[10] = (float)(C[10] + f2);
        g = g_cw6d2078;
    }
    if (F(DWORD, g, 1404) & 0x100)
    {
        float c = (float)x87_fabs(x87_cos(g_cw696d10));
        BYTE* cr = F(BYTE*, g, 616);
        float v = F(float, cr, 3360);
        double k;
        if (v > g_cw64c264)
            k = v > g_cw64b898 ? (double)v : 0.8;
        else
            k = v >= g_cw64b6a8 ? -(double)v : -0.8;
        float p1 = (float)(x87_asin(v) * c * k * g_cw64b898);
        float v2 = F(float, cr, 3372);
        double k2;
        if (v2 > g_cw64c264)
            k2 = v2 > g_cw64b6e0 ? (double)v2 : 0.5;
        else
            k2 = v2 >= g_cw64b6b8 ? -(double)v2 : 0.5;
        float p2 = (float)-(x87_asin(v2) * c * k2);
        if (p1 > g_cw64d6a4)
            p1 = Cw_Bits(1048971922);
        else if (p1 < g_cw64d6a0)
            p1 = Cw_Bits(3196455570u);
        if (p2 > g_cw64d698)
            p2 = Cw_Bits(1034147594);
        else if (p2 < g_cw64d670)
            p2 = Cw_Bits(3188066962u);
        CallThis(FUN_4eacce, s + 4724, p1, &s_cw672edc);
        double f3d = CallThis<double>(Fv_FilterValue, s + 4724);
        float f3 = (float)f3d;
        float tx = (float)(f3d * g_cw64c408);
        float ty = (float)(p2 * (double)g_cw64d694);
        float tz = (float)(((double)p2 * p2 + (double)f3 * f3) * g_cw64d694);
        if (tx > g_cw64c304)
            tx = Cw_Bits(0x3dcccccd);
        else if (tx < g_cw64d690)
            tx = Cw_Bits(0xbdcccccd);
        if (tz > g_cw64c304)
            tz = Cw_Bits(0x3dcccccd);
        else if (tz < g_cw64d690)
            tz = Cw_Bits(0xbdcccccd);
        Vec3 t;
        t.x = tx;
        t.y = ty;
        t.z = tz;
        Matrix T, RY, RX, m1, m2, m3;
        Matrix_Translation(&T, t);
        Matrix_RotationY(&RY, f3);
        Matrix_RotationX(&RX, p2);
        Matrix_Multiply(&m3, Matrix_Multiply(&m2, Matrix_Multiply(&m1, &RX, &RY), (const Matrix*)C), &T);
        memcpy(C, &m3, 48);
    }
    CallC(Cw_FrameApply, F(BYTE*, g_cw696cac, 80), 0, C);
    CallC(Cw_FrameApply, F(BYTE*, s, 556), 0, M);
}

} // extern "C"

// === sharing out two demands by their limits (FUN_556780)

KING_VAR(float, g_sd64c264, 64c264)
KING_VAR(float, g_sd64c250, 64c250)

extern "C" {

// [FUN_556780] limiter s (+16, +20 the two limits, +28 the second's floor) takes what it can of demands *a and *b
// in one ratio: the taken parts (+36, +24, the floor topped up by +32) leave them; 0 when either is empty, else
// 1 / 2 by which limit bound it (0 neither)
int __fastcall Limit_Share(BYTE* s, void*, float* a, float* b)
{
    if (!(*a > g_sd64c264) || !(*b > g_sd64c264))
    {
        F(int, s, 36) = 0;
        F(int, s, 24) = 0;
        F(float, s, 32) = F(float, s, 28);
        return 0;
    }
    double bv = *b;
    F(int, s, 32) = 0;
    if (bv < F(float, s, 28))
        F(float, s, 32) = (float)((double)F(float, s, 28) - bv);
    double B = bv + F(float, s, 32);
    float r1 = (float)(F(float, s, 20) / (double)*a);
    float r2 = (float)(F(float, s, 16) / B);
    double k = g_sd64c250;
    int which = 0;
    if (r1 < g_sd64c250 || r2 < g_sd64c250)
    {
        if (r1 < r2)
        {
            k = r1;
            which = 1;
        }
        else
        {
            k = r2;
            which = 2;
        }
    }
    F(float, s, 36) = (float)(k * *a);
    double t = k * B - F(float, s, 32);
    F(float, s, 24) = (float)t;
    if (F(float, s, 36) > F(float, s, 20))
        F(float, s, 36) = F(float, s, 20);
    if (t > F(float, s, 16))
        F(float, s, 24) = F(float, s, 16);
    *a = (float)((double)*a - F(float, s, 36));
    *b = (float)((double)*b - F(float, s, 24));
    F(int, s, 40) = 0;
    return which;
}

} // extern "C"

// === the vehicle's frame step (a method at $L_54a380)


extern "C" void Vs_AddAssign(); KING_ALIAS("_Vs_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Vs_SubAssign(); KING_ALIAS("_Vs_SubAssign", "@Vec3_SubAssign@16")
extern "C" void Vs_NormalizeThis(); KING_ALIAS("_Vs_NormalizeThis", "@Vec3_NormalizeThis@8")
extern "C" void Vs_ContactMode(); KING_ALIAS("_Vs_ContactMode", "@Body_SetContactMode@12")
extern "C" void Vs_CopterUpdate(); KING_ALIAS("_Vs_CopterUpdate", "@Copter_UpdateAll@8")
extern "C" void Vs_CheckPoint(); KING_ALIAS("_Vs_CheckPoint", "_World_CheckPoint")
extern "C" void Vs_ItemFly(); KING_ALIAS("_Vs_ItemFly", "@MovingItem_Fly@4")
extern "C" void Vs_Upright(); KING_ALIAS("_Vs_Upright", "@Body_CheckUpright@8")
extern "C" void Vs_GroundStep(); KING_ALIAS("_Vs_GroundStep", "@Body_GroundStep@12")
extern "C" void Vs_SetPosition(); KING_ALIAS("_Vs_SetPosition", "@Body_SetPosition@12")
extern "C" void Vs_SetVelocity(); KING_ALIAS("_Vs_SetVelocity", "@Body_SetVelocity@12")
extern "C" void Vs_GetVec9012(); KING_ALIAS("_Vs_GetVec9012", "@Body_GetVec9012@12")
extern "C" void Vs_PlayerFlags(); KING_ALIAS("_Vs_PlayerFlags", "_Player_Flags")
extern "C" void Vs_AddContact(); KING_ALIAS("_Vs_AddContact", "@Engine_AddContact@20")
extern "C" void Vs_FollowLeader(); KING_ALIAS("_Vs_FollowLeader", "@Car_FollowLeader@4")
extern "C" void Vs_TakeContacts(); KING_ALIAS("_Vs_TakeContacts", "@Car_TakeContacts@16")
extern "C" void Vs_RampPosition(); KING_ALIAS("_Vs_RampPosition", "@Ramp_Position@8")
extern "C" void Vs_Car555500(); KING_ALIAS("_Vs_Car555500", "@Car_555500@8")
extern "C" void Vs_OutOfBox(); KING_ALIAS("_Vs_OutOfBox", "_Vec3_OutOfBox")
extern "C" void Vs_Decompose(); KING_ALIAS("_Vs_Decompose", "_Matrix_Decompose6F")
extern "C" void __cdecl Vs_Gauge(Vec3 pos, Vec3 v, double t, int alt); KING_ALIAS("_Vs_Gauge", "_Gauge_Create")
extern "C" void __cdecl Vs_Sound(Vec3 pos, Vec3 dir, double len, int flags); KING_ALIAS("_Vs_Sound", "_Sound_56ef60")
extern "C" void __cdecl Vs_Burst(Vec3 p, Vec3 v, Vec3 w, int flags, float size); KING_ALIAS("_Vs_Burst", "_Fx_CrashBurst")
extern "C" void FUN_5491f0(); KING_ALIAS("_FUN_5491f0", "_Car_ResetSane")
KING_VAR(AsmLabel, g_vs67a28c, 67a28c)
KING_VAR(AsmLabel, g_vs67a280, 67a280)
KING_VAR(AsmLabel, g_vs67a264, 67a264)
KING_VAR(AsmLabel, g_vs67a258, 67a258)
KING_VAR(AsmLabel, g_vs67a24c, 67a24c)
KING_VAR(AsmLabel, g_vs67a240, 67a240)
KING_VAR(AsmLabel, g_vs67a224, 67a224)
KING_VAR(AsmLabel, g_vs67a208, 67a208)
KING_VAR(char, g_vs67a1d8, 67a1d8)
KING_VAR(char, g_vs67a1a4, 67a1a4)
KING_VAR(char, g_vs67a170, 67a170)
KING_VAR(AsmLabel, g_vs69809c, 69809c)
KING_VAR(AsmLabel, g_vs6979b4, 6979b4)
KING_VAR(AsmLabel, g_vs6d219c, 6d219c)
KING_VAR(int, g_vs6ce388, 6ce388)
KING_VAR(float, g_vs64c264, 64c264)
KING_VAR(float, g_vs64c258, 64c258)
KING_VAR(float, g_vs64c938, 64c938)
KING_VAR(float, g_vs64c250, 64c250)
KING_VAR(float, g_vs64f75c, 64f75c)
KING_VAR(float, g_vs64f758, 64f758)
KING_VAR(float, g_vs64c308, 64c308)
KING_VAR(float, g_vs64c25c, 64c25c)
KING_VAR(float, g_vs64f754, 64f754)
KING_VAR(float, g_vs64c338, 64c338)
KING_VAR(float, g_vs674408, 674408)
KING_VAR(float, g_vs64f750, 64f750)
KING_VAR(float, g_vs64c458, 64c458)
KING_VAR(double, g_vs64be98, 64be98)
KING_VAR(double, g_vs64d318, 64d318)
KING_VAR(double, g_vs64b678, 64b678)
KING_VAR(double, g_vs64b668, 64b668)
KING_VAR(double, g_vs64b5b8, 64b5b8)
KING_VAR(double, g_vs64b6e0, 64b6e0)
KING_VAR(double, g_vs64d0f0, 64d0f0)
KING_VAR(double, g_vs64f3d8, 64f3d8)
KING_VAR(double, g_vs64b840, 64b840)
KING_VAR(double, g_vs64b788, 64b788)
KING_VAR(double, g_vs64b838, 64b838)
KING_VAR(double, g_vs64b5a0, 64b5a0)
KING_VAR(double, g_vs64b658, 64b658)
KING_VAR(double, g_vs64b6a0, 64b6a0)
KING_VAR(double, g_vs64f748, 64f748)
KING_VAR(double, g_vs64f4c8, 64f4c8)
KING_VAR(double, g_vs64eba8, 64eba8)

// t turned back by the 3x3 part of m: x = m0 tx + m3 ty + m6 tz ...
static Vec3 Vs_Back(const float* m, const Vec3& t)
{
    Vec3 r;
    r.x = (float)((double)m[6] * t.z + (double)m[3] * t.y + (double)m[0] * t.x);
    r.y = (float)((double)m[1] * t.x + (double)m[7] * t.z + (double)m[4] * t.y);
    r.z = (float)((double)m[8] * t.z + (double)m[2] * t.x + (double)m[5] * t.y);
    return r;
}

// the frame velocity +440 from the spin +428 scaled by +404, through the 3x3 at m
static void Vs_SpinToFrame(BYTE* s, const float* m)
{
    Vec3 t;
    Vec3_MulComponents(&t, &F(Vec3, s, 428), &F(Vec3, s, 404));
    F(Vec3, s, 440) = Vs_Back(m, t);
}

static void Vs_Speed(BYTE* s)
{
    float v = (float)Vec3_Length(&F(Vec3, s, 416));
    F(float, s, 20948) = v;
    F(float, s, 10244) = v;
}

static Vec3 Vs_Scaled(const Vec3& v, float k)
{
    Vec3 r;
    r.x = (float)((double)k * v.x);
    r.y = (float)((double)k * v.y);
    r.z = (float)((double)k * v.z);
    return r;
}

// the frame m from the moving frame +548: turned by the spin +428 over dt and moved by dt times +416
static void Vs_Integrate(BYTE* s, float dt, Matrix* m)
{
    Matrix tr, rot, m1, m2;
    Matrix_Translation(&tr, Vs_Scaled(F(Vec3, s, 416), dt));
    float len = (float)(Vec3_Length(&F(Vec3, s, 428)) * dt);
    Matrix_LookAt(&rot, g_zeroVec, F(Vec3, s, 428), len);
    Matrix_Multiply(&m1, &rot, &F(Matrix, s, 548));
    *m = *Matrix_Multiply(&m2, &m1, &tr);
}

extern "C" {

// [$L_54a380] vehicle s's frame step by its mode (+10908): 0 a free flying frame, 5 riding the helicopter
// (+21588), 2 and 3 the network frames (+20624..), 1 and 4 the physics body (+21584) with its readings copied out
// (speeds, gears, wheels, the steering share +21016 / +21020 from the wheels on ice or sand); then the cab frame
// angles (+3160..), the frame records +3300.., the wheel smoke gauges, the wheels' scrape sounds and bursts, and the
// gauge needles (+10164.., +9636 frames)
void __fastcall Vehicle_FrameStep(BYTE* s, void*)
{
    F(DWORD, s, 10148) &= ~2u;
    float dt = (float)Game_FrameTime();
    F(int, s, 10258) = 0;
    Matrix Mw = F(Matrix, s, 548);
    F(DWORD, s, 1196) |= 8;
    int mode = F(int, s, 10908);
    BYTE* B = F(BYTE*, s, 21584);
    const float* W = Mw.m;
    bool step = false;                              // went through the contact mode
    int ok = 0;
    if (mode == 0)
    {
        Vec3 tmp, d = Vs_Scaled(F(Vec3, s, 788), dt);
        CallThis(Vs_AddAssign, s + 416, &tmp, &d);
        CallThis(Vs_ContactMode, s, 0);
        Vs_Integrate(s, dt, &Mw);
        F(int, s, 20836) = 0;
        Vs_Speed(s);
    }
    else if (mode == 5)
    {
        BYTE* c = F(BYTE*, s, 21588);
        if (c)
        {
            CallThis(Vs_CopterUpdate, c);
            c = F(BYTE*, s, 21588);
            F(int, s, 20792) = F(int, c, 96);
            memcpy(s + 20624, F(BYTE*, s, 21588), 48);
            CallC(Vs_CheckPoint, s + 20660, &g_vs67a28c, 10000.0f);
            Mw = F(Matrix, s, 20624);
            F(Vec3, s, 20672) = F(Vec3, F(BYTE*, s, 21588), 100);
            F(Vec3, s, 20696) = F(Vec3, F(BYTE*, s, 21588), 112);
            F(Vec3, s, 416) = F(Vec3, s, 20672);
            F(Vec3, s, 428) = F(Vec3, s, 20696);
            Vs_SpinToFrame(s, W);
        }
    }
    else if (mode == 2)
    {
        int n = F(int, s, 10408);
        double sum = g_vs64c264;
        for (int k = 0; k < n; k++)
            sum += F(float, s, 10568 + 4 * k) * (double)g_vs64c258;
        for (int k = 0; k < F(int, s, 10408); k++)
            F(float, s, 20724 + 4 * k) = (float)(sum / F(int, s, 10408));
        CallC(Vs_CheckPoint, s + 20660, &g_vs67a280, 10000.0f);
        double nx = -(double)F(float, s, 21024), ny = -(double)F(float, s, 21028), nz = -(double)F(float, s, 21032);
        const float* A = (const float*)(s + 20624);
        Mw = F(Matrix, s, 20624);
        Mw.m[9] = (float)(nz * A[6] + ny * A[3] + nx * A[0] + A[9]);
        Mw.m[10] = (float)(nz * A[7] + ny * A[4] + nx * A[1] + A[10]);
        Mw.m[11] = (float)(nz * A[8] + ny * A[5] + nx * A[2] + A[11]);
        CallC(Vs_CheckPoint, s + 20672, &g_vs67a264, 150.0f);
        F(Vec3, s, 416) = F(Vec3, s, 20672);
        F(Vec3, s, 428) = F(Vec3, s, 20696);
        Vs_SpinToFrame(s, W);
        CallThis(Vs_ContactMode, s, 1);
        F(int, s, 20836) = 0;
        Vs_Speed(s);
    }
    else if (mode == 3)
    {
        CallC(Vs_CheckPoint, s + 20660, &g_vs67a258, 10000.0f);
        Mw = F(Matrix, s, 20624);
        CallThis(Vs_ContactMode, s, 1);
        F(int, s, 20836) = 0;
        Vs_Speed(s);
    }
    else if (mode == 1 || mode == 4)
    {
        if (!B)
        {
            CallThis(Vs_ItemFly, s);
            CallThis(Vs_ContactMode, s, 1);
            return;
        }
        step = true;
        F(int, s, 21020) = 0;
        F(int, s, 10396) = 0;
        F(int, s, 21016) = 0;
        F(int, s, 10392) = 0;
        if (F(int, B, 13124) && !CallThis<int>(Vs_Upright, B))
        {
            // the body lies over: the frame runs on its own
            if (mode == 1)
            {
                CallThis(Vs_GroundStep, s, 0);
                if (F(int, s, 896))
                {
                    const float* Y = &g_axisY.x;
                    double a = (double)Y[2] * F(float, s, 524) + (double)Y[1] * F(float, s, 512) + (double)Y[0] * F(float, s, 500);
                    double b = (double)Y[2] * F(float, s, 528) + (double)Y[1] * F(float, s, 516) + (double)Y[0] * F(float, s, 504);
                    float c = (float)((double)Y[2] * F(float, s, 532) + (double)Y[1] * F(float, s, 520) + (double)Y[0] * F(float, s, 508));
                    double dot = (double)F(float, s, 976) * a + ((double)F(float, s, 984) * c + (double)F(float, s, 980) * b);
                    if (x87_fabs(dot) > g_vs64be98)
                    {
                        double d = x87_fabs((double)c) - g_vs64be98;
                        float px = (float)(d * g_axisX.x), py = (float)(d * g_axisX.y);
                        double pz = d * g_axisX.z;
                        Vec3 off;
                        off.x = (float)((double)px + px);
                        off.y = (float)((double)py + py);
                        off.z = (float)(pz + pz);
                        if (c > g_vs64c264)
                        {
                            off.x = -off.x;
                            off.y = -off.y;
                            off.z = -off.z;
                        }
                        const float* R = (const float*)(s + 596);
                        const Vec3& v = F(Vec3, s, 440);
                        Vec3 w, t, tmp;
                        w.x = (float)((double)R[6] * v.z + (double)R[3] * v.y + (double)v.x * R[0]);
                        w.y = (float)((double)R[7] * v.z + (double)R[4] * v.y + (double)R[1] * v.x);
                        w.z = (float)((double)R[8] * v.z + (double)R[5] * v.y + (double)R[2] * v.x);
                        F(Vec3, s, 428) = *Vec3_DivComponents(&t, &w, &F(Vec3, s, 404));
                        CallThis(Vs_AddAssign, s + 428, &tmp, &off);
                        Vs_SpinToFrame(s, (const float*)(s + 500));
                    }
                }
                Vs_Integrate(s, dt, &Mw);
                Matrix inv;
                Matrix_Inverse(&inv, &Mw);
                F(int, s, 20836) = 0;
                CallThis(Vs_SetPosition, F(BYTE*, s, 21584), s + 416);
                CallThis(Vs_SetVelocity, F(BYTE*, s, 21584), s + 440);
                Vs_Speed(s);
            }
            else
            {
                CallC(Vs_CheckPoint, s + 20660, &g_vs67a24c, 10000.0f);
                Mw = F(Matrix, s, 20624);
                Vec3 t;
                float v = (float)Vec3_Length(CallThis<Vec3*>(Vs_GetVec9012, F(BYTE*, s, 21584), &t));
                F(float, s, 20948) = v;
                F(float, s, 10244) = v;
            }
            B = F(BYTE*, s, 21584);
            ok = 0;
            F(float, s, 20952) = F(float, s, 10248) = F(float, B, 15420);
            F(int, s, 20956) = F(int, B, 15424);
            F(int, s, 20984) = 0;
        }
        else
        {
            F(DWORD, s, 1196) &= ~8u;
            F(int, B, 10064) = F(int, s, 21436);
            F(int, B, 10068) = F(int, s, 21436);
            F(int, B, 10060) = F(int, s, 21436);
            F(int, B, 10116) = (CallC<int>(Vs_PlayerFlags, F(int, s, 20620)) & 0x2000000) ? 1 : 0;
            if (F(int, s, 20920))
            {
                if (F(short, B, 10048) == 0)
                {
                    if (F(int, B, 15448) && F(int, B, 9784) == 1)
                    {
                        Vec3 p, pt;
                        p.x = (float)(F(float, B, 15452) / (double)dt);
                        p.y = (float)(F(float, B, 15456) / (double)dt);
                        p.z = (float)(F(float, B, 15460) / (double)dt);
                        const float* R = (const float*)(s + 596);
                        const float* v = (const float*)(B + 15464);
                        pt.x = (float)((double)R[6] * v[2] + (double)R[3] * v[1] + (double)R[0] * v[0] + R[9]);
                        pt.y = (float)((double)R[7] * v[2] + (double)R[4] * v[1] + (double)R[1] * v[0] + R[10]);
                        pt.z = (float)((double)R[8] * v[2] + (double)R[5] * v[1] + (double)R[2] * v[0] + R[11]);
                        CallThis(Vs_AddContact, B + 9984, &pt, &p, &g_zeroVec);
                    }
                    CallThis(Vs_FollowLeader, s - 16);
                }
                BYTE* body = F(BYTE*, s, 21584);
                CallThis((void (*)())F(void**, body, 0)[0], body);
            }
            B = F(BYTE*, s, 21584);
            CallThis(Vs_TakeContacts, s - 16, s + 20620, B + 10736);
            F(Vec3, s, 416) = F(Vec3, s, 20672);
            F(Vec3, s, 428) = F(Vec3, s, 20696);
            F(Vec3, s, 788) = F(Vec3, s, 20708);
            CallC(Vs_CheckPoint, s + 20660, &g_vs67a240, 10000.0f);
            Mw = F(Matrix, s, 20624);
            Vs_SpinToFrame(s, W);
            Vec3 d;
            d.x = (float)((double)F(float, s, 20660) - F(float, s, 536));
            d.y = (float)((double)F(float, s, 20664) - F(float, s, 540));
            d.z = (float)((double)F(float, s, 20668) - F(float, s, 544));
            if (Vec3_Length(&d) < g_vs64b5b8)
                Mw = F(Matrix, s, 20624);
            if (F(float, s, 20680) < g_vs64c938)
                F(Vec3, s, 416) = F(Vec3, s, 20672);
            for (int k = 0; k < F(int, s, 10412); k++)
            {
                BYTE* wb = F(BYTE*, s, 21584) + k * 248;
                if (F(short, wb, 11112))
                {
                    F(Vec3, s, 1176) = F(Vec3, wb, 11100);
                    F(Vec3, s, 1164) = F(Vec3, F(BYTE*, s, 21584) + k * 248, 11088);
                }
            }
            // contacts stay on only after a calm second, upright, and away from the car in front
            ok = 1;
            if (F(int, s, 896) || Game_Time() - F(float, s, 904) < g_vs64c250 || W[8] < g_vs64d318)
                ok = 0;
            else if (BYTE* o = F(BYTE*, s, 3508))
            {
                float dpx = (float)((double)F(float, s, 416) - F(float, o, 3300));
                float dpy = (float)((double)F(float, s, 420) - F(float, o, 3304));
                float dpz = (float)((double)F(float, s, 424) - F(float, o, 3308));
                float dvx = (float)((double)F(float, s, 536) - F(float, o, 3372));
                float dvy = (float)((double)F(float, s, 540) - F(float, o, 3376));
                float dvz = (float)((double)F(float, s, 544) - F(float, o, 3380));
                double r = (double)F(float, o, 3272) + F(float, s, 40);
                if ((double)dpz * dvz + (double)dpy * dvy + (double)dvx * dpx < g_vs64b678)
                    r += x87_sqrt((double)dpx * dpx + (double)dpy * dpy + (double)dpz * dpz) * dt;
                if (r * r * g_vs64b668 > F(float, s, 3532))
                    ok = 0;
            }
            B = F(BYTE*, s, 21584);
            F(float, s, 20948) = F(float, s, 10244) = F(float, B, 13104);
            F(float, s, 20952) = F(float, s, 10248) = F(float, B, 15420);
            F(int, s, 20956) = F(int, B, 15424);
            F(int, s, 20960) = F(int, B, 15428);
            F(float, s, 20964) = F(float, s, 10240) = (float)F(double, B, 13112);
            int g = F(int, B, 15320);
            F(int, s, 20980) = g;
            s[10256] = (BYTE)g;
            F(float, s, 20968) = F(float, s, 10252) = F(float, B, 15416);
            F(float, s, 20972) = F(float, s, 10364) = (float)CallThis<double>(Vs_RampPosition, F(BYTE*, s, 21584) + 16112);
            F(float, s, 20976) = F(float, s, 10400) = (float)CallThis<double>(Vs_Car555500, s - 16);
            B = F(BYTE*, s, 21584);
            F(int, s, 20992) = F(int, s, 10372) = F(int, B, 15492);
            F(int, s, 20988) = F(int, s, 10368) = F(int, B, 15496);
            F(int, s, 20996) = F(int, s, 10376) = F(int, B, 15488);
            F(int, s, 21008) = F(int, s, 10380) = F(int, B, 7188);
            F(int, s, 21000) = F(int, s, 10384) = F(int, B, 15504);
            F(int, s, 21004) = F(int, s, 10388) = F(int, B, 15440);
            int x = F(int, B, 15412);
            F(int, s, 20984) = x;
            if (x)
                F(DWORD, s, 10148) |= 2;
            // the wheels on slippery ground (left / right kinds) share the steering
            int Ak[8] = {0}, Bk[8] = {0};
            float Cf[8];
            int anyL = 0, anyR = 0;
            int cnt = F(int, (BYTE*)&g_vs69809c + F(int, B, 2252) * 2416, 44);
            if (cnt > 0)
            {
                for (int k = 0; k < cnt; k++)
                {
                    BYTE* P = B + 13380 + 4 * k;
                    BYTE* Wh = B + 11220 + 248 * k;
                    int e = F(int, P, 0);
                    BYTE* gr = F(BYTE*, Wh, 0);
                    bool base = !e && F(short, Wh, -108) != 0 && gr && F(int, gr, 0) == 1;
                    int a = base && F(int, P, 40) ? 1 : 0;
                    int b = base && F(int, P, 72) ? 1 : 0;
                    Ak[k] = a;
                    Bk[k] = b;
                    anyL = anyL || a;
                    anyR = anyR || b;
                }
                if ((anyL && F(float, B, 13100) > g_vs64b6e0) || anyR)
                {
                    double sum = g_vs64c264;
                    for (int c = 0; c < cnt; c++)
                    {
                        double v = x87_fabs((double)F(float, B, 13196 + 4 * c));
                        Cf[c] = v > g_vs64f75c ? 1.0f : (float)(v * g_vs64f758);
                        sum += (double)Ak[c] * Cf[c] * g_vs64d0f0;
                    }
                    float fs = (float)sum;
                    double t = Ramp_54a330(F(float, B, 13104)) * fs;
                    double p = (double)Ak[1] * Ak[0] * g_vs64f3d8 + Ak[3] * g_vs64b840 + Ak[2] * g_vs64d0f0;
                    t = t + p * g_vs64b788 + (double)(Bk[3] + Bk[2] + Bk[1] + Bk[0]) * g_vs64b838 + fs;
                    float total = (float)t;
                    double val = g_vs64c250;
                    int sl = 0, sr = 0;
                    for (int c = 0; c < cnt; c++)
                    {
                        if (Ak[c])
                        {
                            int kind = F(int, F(BYTE*, B, 11220 + 248 * c), 4);
                            Ak[c] = kind == 1 || kind == 5 ? 1 : kind == 2 || kind == 6 ? -1 : 0;
                        }
                        sr += Bk[c];
                        sl += Ak[c];
                    }
                    if (sl || sr)
                    {
                        if (sl < 0)
                            val = g_vs64c308;
                        double xv = total * g_vs64d0f0;
                        if (xv > g_vs64c250)
                            xv = g_vs64c250;
                        F(float, s, 21020) = F(float, s, 10396) = (float)xv;
                        F(float, s, 21016) = F(float, s, 10392) = (float)val;
                    }
                }
            }
        }
    }
    if (step)
        CallThis(Vs_ContactMode, s, ok);

    // the frame's records
    Matrix Mi = g_identity;
    B = F(BYTE*, s, 21584);
    if (F(int, s, 20836) && B)
    {
        F(int, s, 804) = F(int, B, 6672);
        F(Matrix, s, 548) = F(Matrix, F(BYTE*, s, 21584), 2256);
        F(Matrix, s, 644) = F(Matrix, F(BYTE*, s, 21584), 2352);
        CallC(Vs_CheckPoint, s + 20672, &g_vs67a224, 150.0f);
        F(Vec3, s, 416) = F(Vec3, s, 20672);
        F(Vec3, s, 3148) = Vs_Scaled(F(Vec3, s, 416), dt);
        for (int k = 0; k < F(int, s, 10412); k++)
        {
            BYTE* b = F(BYTE*, s, 21584);
            if (!F(int, b, 15336 + 4 * k))
                continue;
            const float* c = (const float*)(b + 13252 + 12 * k);
            const float* A = (const float*)(s + 20624);
            Vec3 pt, v;
            pt.y = (float)((double)A[1] * c[0] + (double)A[7] * c[2] + (double)A[4] * c[1] + A[10]);
            pt.z = (float)((double)A[2] * c[0] + (double)A[8] * c[2] + (double)A[5] * c[1] + A[11]);
            pt.x = (float)((double)A[6] * c[2] + (double)A[3] * c[1] + (double)A[0] * c[0] + A[9]);
            const float* Z = &g_axisZ.x;
            double ux = (double)Z[2] * A[6] + (double)Z[1] * A[3] + (double)Z[0] * A[0];
            float uy = (float)((double)Z[0] * A[1] + (double)Z[2] * A[7] + (double)Z[1] * A[4]);
            float uz = (float)((double)Z[0] * A[2] + (double)Z[2] * A[8] + (double)Z[1] * A[5]);
            float f = (float)F(int, b, 15336 + 4 * k);
            v.x = (float)(ux * f);
            v.y = (float)((double)f * uy);
            v.z = (float)((double)f * uz);
            CallThis((void (*)())F(void**, s, 0)[41], s, &pt, &v);
        }
    }
    else if (F(int, s, 896))
    {
        if (mode != 5 && mode != 1)
            F(Vec3, s, 416) = F(Vec3, s, 20672);
        CallC(Vs_CheckPoint, s + 416, &g_vs67a208, 100.0f);
        F(Vec3, s, 3148) = Vs_Scaled(F(Vec3, s, 416), dt);
    }
    else
    {
        F(float, s, 3148) = (float)((double)W[9] - F(float, s, 584));
        F(float, s, 3152) = (float)((double)W[10] - F(float, s, 588));
        F(float, s, 3156) = (float)((double)W[11] - F(float, s, 592));
        mode = F(int, s, 10908);
        if (mode != 2 && mode != 5 && CallC<int>(Vs_OutOfBox, s + 416, 150.0f))
            CallC(FUN_5491f0, s - 16);
    }

    // the cab's angles
    Matrix mm;
    Mi = *Matrix_Multiply(&mm, &Mw, &F(Matrix, s, 644));
    Mi.m[9] = g_zeroVec.x;
    Mi.m[10] = g_zeroVec.y;
    Mi.m[11] = g_zeroVec.z;
    float a432 = 0, a428 = 0, a424 = 0;
    CallC(Vs_Decompose, &Mi, &a432, &a428, &a424, s + 3168, s + 3160, s + 3164);
    F(float, s, 3160) = -F(float, s, 3160);
    double ap = x87_fabs((double)F(float, s, 3164));
    if (ap < g_vs64b5a0)
        F(float, s, 3164) = (float)((ap + g_vs64b658) * F(float, s, 3164) * g_vs64b6a0);
    F(Vec3, s, 3300) = F(Vec3, s, 416);
    F(Vec3, s, 3312) = F(Vec3, s, 428);
    F(Vec3, s, 3324) = F(Vec3, s, 440);

    // the wheels' smoke gauges on ice and sand
    B = F(BYTE*, s, 21584);
    if (B && F(int, s, 10908) == 1 && !F(int, B, 13124) && (s[20884] & 4) && g_vs6ce388)
    {
        for (int k = 0; k < F(int, s, 10412); k++)
        {
            BYTE* G = s + 10840 + 4 * k;
            int off = 248 * k;
            BYTE* w;
            if (!F(int, G, 10008) || F(int, s, 10908) != 1 || !(w = F(BYTE*, F(BYTE*, s, 21584), off + 11220))
                || F(int, w, 0) != 1 || (F(int, w, 4) != 2 && F(int, w, 4) != 6))
            {
                F(int, G, 0) = -1;
                continue;
            }
            BYTE* b = F(BYTE*, s, 21584);
            Matrix m = F(Matrix, s, 500), t1, t2;
            Vec3 pos = F(Vec3, b, off + 11088);
            if (off < 496)
                m = *Matrix_Multiply(&t2, Matrix_RotationZ(&t1, F(float, b, 13160)), &m);
            w = F(BYTE*, F(BYTE*, s, 21584), off + 11220);
            if (F(int, w, 4) != F(int, G, 0) && F(int, G, 0) != -1)
                F(int, G, 0) = -1;
            if (F(int, G, 0) == -1)
            {
                F(float, G, -32) = (float)Game_Time();
                F(int, G, 0) = F(int, F(BYTE*, F(BYTE*, s, 21584), off + 11220), 4);
            }
            double t = Game_Time() - F(float, G, -32);
            t = t > g_vs64c25c ? (double)g_vs64c250 : x87_sqrt(t * g_vs64f754);
            float span = (float)(t + t + g_vs64b840);
            if (F(int, G, 10008) != -1)
                m = *Matrix_Multiply(&t2, Matrix_RotationZ(&t1, 3.14159274f), &m);
            const float* Zv = &g_axisZ.x;
            double zx = (double)Zv[0] * g_vs64c338;
            float zy = (float)((double)Zv[1] * g_vs64c338);
            float zz = (float)((double)Zv[2] * g_vs64c338);
            const float* Y = &g_axisY.x;
            const float* M = m.m;
            double yx = (double)M[0] * Y[0] + (double)M[6] * Y[2] + (double)M[3] * Y[1];
            float yy = (float)((double)M[7] * Y[2] + (double)M[4] * Y[1] + (double)M[1] * Y[0]);
            float yz = (float)((double)M[8] * Y[2] + (double)M[5] * Y[1] + (double)M[2] * Y[0]);
            Vec3 v;
            v.x = (float)(yx + zx);
            v.y = (float)((double)zy + yy);
            v.z = (float)((double)zz + yz);
            Vs_Gauge(pos, v, span, F(int, G, 0) != 2);
        }
    }

    // the scrape of wheels off the ground
    B = F(BYTE*, s, 21584);
    if (B && F(int, B, 9784) == 3 && !F(int, B, 13124))
    {
        const float* X = &g_axisX.x;
        const float* M = (const float*)(B + 10736);
        Vec3 vx;
        float a = (float)((double)X[1] * M[4] + (double)X[2] * M[7] + (double)X[0] * M[1]);
        vx.z = (float)((double)X[1] * M[5] + (double)X[2] * M[8] + (double)X[0] * M[2]);
        vx.x = (float)((double)X[1] * M[3] + (double)X[2] * M[6] + (double)X[0] * M[0]);
        vx.y = a;
        for (int k = 0; k < F(int, s, 10412); k++)
        {
            B = F(BYTE*, s, 21584);
            if (!F(short, B, 248 * k + 11112) || F(int, B, 13380 + 4 * k))
                continue;
            Vec3 dir = g_axisZ, tmp;
            const float* u = (const float*)(B + 10868);
            const float* r = (const float*)(B + 10976 + 12 * k);
            double c1 = (double)u[1] * r[2] - (double)u[2] * r[1];
            double c2 = (double)r[0] * u[2] - (double)u[0] * r[2];
            double c3 = (double)u[0] * r[1] - (double)r[0] * u[1];
            Vec3 p;
            p.x = (float)((float)(c3 * M[6] + c2 * M[3] + c1 * M[0]) + (double)F(float, B, 10832));
            p.y = (float)((float)(c1 * M[1] + c3 * M[7] + c2 * M[4]) + (double)F(float, B, 10836));
            p.z = (float)(c1 * M[2] + c3 * M[8] + c2 * M[5] + F(float, B, 10840));
            float len = (float)Vec3_Length(&p);
            if (F(float, B, 10976 + 12 * k) < g_vs64c264)
                CallThis(Vs_SubAssign, &dir, &tmp, &vx);
            else
                CallThis(Vs_AddAssign, &dir, &tmp, &vx);
            CallThis(Vs_NormalizeThis, &dir);
            B = F(BYTE*, s, 21584);
            if (!F(short, B, 248 * k + 11112))
                continue;
            int fl = F(int, s, 1204) ? 2 : 0;
            BYTE* g = F(BYTE*, B, 13684 + 208 * k);
            if (g && F(int, g, 0) == 1)
            {
                int kind = F(int, g, 4);
                if (kind == 4 || kind == 7 || kind == 9)
                    fl |= 1;
            }
            Vs_Sound(F(Vec3, B, 248 * k + 11088), dir, len, fl);
            B = F(BYTE*, s, 21584);
            Vs_Burst(F(Vec3, B, 248 * k + 11088), F(Vec3, s, 416), dir, fl, F(float, B, 13492));
        }
    }

    // the gauge needles
    s[10164] = (BYTE)(int)(F(float, s, 20720) / g_vs674408 * g_vs64f750);
    F(float, s, 20944) = (float)(F(float, s, 20720) / g_vs674408);
    char buf[128];
    for (int j = 0; j < F(int, s, 10408); j++)
    {
        BYTE* e = s + 10568 + 4 * j;
        s[10165 + j] = (BYTE)(int)(F(float, e, 10156) / F(float, e, 0) * g_vs64c458);
        int nn = F(int, s, 10412);
        const char* name = F(const char*, F(BYTE*, s, 320), 12);
        if (j < nn)
        {
            F(int, e, -392) = F(int, e, 10188);
            sprintf(buf, &g_vs67a1d8, name, j);
            CallC<double>(FUN_4e41d9, F(int, e, -392), buf);
        }
        else if (j - 2 >= 0 && j - 2 < nn)
        {
            F(float, e, -392) = (float)(F(float, e, 10180) + g_vs64f748);
            sprintf(buf, &g_vs67a1a4, name, j - 2);
            CallC<double>(FUN_4e41d9, F(int, e, 10180), buf);
        }
        else if (j - 4 >= 0 && j - 4 < nn)
        {
            F(float, e, -392) = (float)(F(float, e, 10172) - g_vs64f748);
            sprintf(buf, &g_vs67a170, name, j);
            CallC<double>(FUN_4e41d9, F(int, e, 10172), buf);
        }
        else
            F(int, e, -392) = 0;
    }
    for (int i = 0; i < F(int, s, 10408); i++)
    {
        BYTE* M = s + 9636 + 48 * i;
        Matrix t1, t2, t3;
        F(Matrix, M, 0) = *Matrix_RotationX(&t1, F(float, s, 10176 + 4 * i));
        int model = F(int, s, 10404);
        if (F(int, (BYTE*)&g_vs6979b4 + model * 2416, 4 * i))
        {
            float ang = (float)((signed char)s[10164] * (double)g_vs674408 * g_vs64f4c8);
            F(Matrix, M, 0) = *Matrix_Multiply(&t3, (const Matrix*)M, Matrix_RotationZ(&t2, ang));
        }
        F(Vec3, M, 36) = F(Vec3, (BYTE*)&g_vs6d219c + (F(int, s, 10404) * 31 + i) * 48, 136);
        F(float, M, 44) = (float)(F(float, M, 44) - (double)s[10165 + i] * F(float, s, 10568 + 4 * i) * g_vs64eba8);
    }
}

} // extern "C"

// === keeping the truck and trailer together at the hitch (FUN_55b6f0)

extern "C" void __cdecl Hc_ToAngles(Matrix m, double* a, double* b, double* c, double* d, double* e, double* f);
KING_ALIAS("_Hc_ToAngles", "_Matrix_ToAngles")
extern "C" void Hc_NormalizeThis(); KING_ALIAS("_Hc_NormalizeThis", "@Vec3_NormalizeThis@8")
extern "C" void Hc_GetVec9012(); KING_ALIAS("_Hc_GetVec9012", "@Body_GetVec9012@12")
extern "C" void Hc_GetVec9024(); KING_ALIAS("_Hc_GetVec9024", "@Body_GetVec9024@12")
extern "C" void Hc_GetVec9048(); KING_ALIAS("_Hc_GetVec9048", "@Body_GetVec9048@12")
extern "C" void __fastcall Hc_SetVec9012(BYTE* body, void*, Vec3 v); KING_ALIAS("@Hc_SetVec9012@20", "@Body_SetVec9012@20")
extern "C" void __fastcall Hc_SetSpin(BYTE* body, void*, Vec3 v); KING_ALIAS("@Hc_SetSpin@20", "@Body_SetSpinScaled@20")
KING_VAR(char, g_hc67a534, 67a534)
KING_VAR(char, g_hc67a51c, 67a51c)
KING_VAR(char, g_hc67a504, 67a504)
KING_VAR(char, g_hc67a4ec, 67a4ec)
KING_VAR(char, g_hc67a4d4, 67a4d4)
KING_VAR(char, g_hc67a4bc, 67a4bc)
KING_VAR(BYTE, g_hc6ef2a8, 6ef2a8)
KING_VAR(float, g_hc6ef2ac, 6ef2ac)
KING_VAR(float, g_hc6ef2b0, 6ef2b0)
KING_VAR(float, g_hc6ef2cc, 6ef2cc)
KING_VAR(int, g_hc6ef2d0, 6ef2d0)
KING_VAR(float, g_hc67a4b0, 67a4b0)
KING_VAR(float, g_hc67a4b4, 67a4b4)
KING_VAR(float, g_hc67a4b8, 67a4b8)
KING_VAR(float, g_hc64c264, 64c264)
KING_VAR(float, g_hc64c250, 64c250)
KING_VAR(float, g_hc64daf4, 64daf4)
KING_VAR(double, g_hc64b810, 64b810)
KING_VAR(double, g_hc64fa48, 64fa48)
KING_VAR(double, g_hc64b670, 64b670)
KING_VAR(double, g_hc64d378, 64d378)
KING_VAR(double, g_hc64b6e0, 64b6e0)
KING_VAR(double, g_hc64b720, 64b720)
KING_VAR(double, g_hc64b668, 64b668)
KING_VAR(double, g_hc64b788, 64b788)
KING_VAR(AsmLabel, g_hc697f9c, 697f9c)
KING_VAR(AsmLabel, g_hc697db8, 697db8)

// the relative angles of frame a seen from b, each through the debug value hook (FUN_4e41d9) with its name
static void Hc_Angles(const Matrix* a, const Matrix* b, double* r, const char* n0, const char* n1, const char* n2)
{
    Matrix ib, m;
    Matrix_Inverse(&ib, b);
    Matrix_Multiply(&m, a, &ib);
    m.m[9] = g_zeroVec.x;
    m.m[10] = g_zeroVec.y;
    m.m[11] = g_zeroVec.z;
    double x0, x1, x2;
    Hc_ToAngles(m, &x0, &x1, &x2, &r[0], &r[1], &r[2]);
    r[0] = CallC<double>(FUN_4e41d9, (float)r[0], n0);
    r[1] = CallC<double>(FUN_4e41d9, (float)r[1], n1);
    r[2] = CallC<double>(FUN_4e41d9, (float)r[2], n2);
}

// v turned by the 3x3 part of m as a column: x = m0 vx + m3 vy + m6 vz ...
static Vec3 Hc_Col(const float* m, const Vec3& v)
{
    Vec3 r;
    r.x = (float)((double)m[0] * v.x + (double)m[6] * v.z + (double)m[3] * v.y);
    r.y = (float)((double)m[1] * v.x + (double)m[7] * v.z + (double)m[4] * v.y);
    r.z = (float)((double)m[2] * v.x + (double)m[8] * v.z + (double)m[5] * v.y);
    return r;
}

static Vec3 Hc_Through(const float* M, const float* v)
{
    Vec3 r;
    r.x = (float)((double)M[6] * v[2] + (double)M[3] * v[1] + (double)M[0] * v[0] + M[9]);
    r.y = (float)((double)M[1] * v[0] + (double)M[7] * v[2] + (double)M[4] * v[1] + M[10]);
    r.z = (float)((double)M[2] * v[0] + (double)M[8] * v[2] + (double)M[5] * v[1] + M[11]);
    return r;
}

static Vec3 Hc_Lin(const Vec3& a, double k, const Vec3& b, double l)
{
    Vec3 r;
    r.x = (float)(a.x * k + b.x * l);
    r.y = (float)(a.y * k + b.y * l);
    r.z = (float)(a.z * k + b.z * l);
    return r;
}

static Vec3 Hc_Mul(const Vec3& a, double k)
{
    Vec3 r;
    r.x = (float)(a.x * k);
    r.y = (float)(a.y * k);
    r.z = (float)(a.z * k);
    return r;
}

extern "C" {

// [FUN_55b6f0] truck s and its trailer (+21880) with new frames M1 / M2: 1 when they bend too far apart; 0 when
// they are almost straight; else both frames turned about the hitch (each model's +156 / +168 points of the 697f9c
// records) by mass shares (+3284), the hitch's velocities and spins shared out (+21600 bodies) and, with 6ef2d0, the
// trailer's yaw (+3) limited; then 0
int __fastcall Hitch_Align(BYTE* s, void*, Matrix* M1, Matrix* M2)
{
    BYTE* o = F(BYTE*, s, 21880);
    double A[3], B[3];
    Hc_Angles(M2, M1, A, &g_hc67a534, &g_hc67a51c, &g_hc67a504);
    BYTE fl = g_hc6ef2a8;
    if (!(fl & 1))
    {
        fl |= 1;
        g_hc6ef2a8 = fl;
        g_hc6ef2ac = (float)x87_fabs(x87_sin(g_hc67a4b0));
    }
    if (!(fl & 2))
    {
        fl |= 2;
        g_hc6ef2a8 = fl;
        g_hc6ef2b0 = (float)x87_fabs(x87_cos(g_hc67a4b0));
    }
    double a1 = x87_fabs(A[0]);
    if (a1 > g_hc64b810 || x87_fabs(A[1]) > g_hc64fa48)
        return 1;
    double a3 = x87_fabs(A[2]);
    if (a3 > g_hc64fa48)
        return 1;
    if (g_hc67a4b0 > a1 && a3 < g_hc67a4b4)
        return 0;
    Hc_Angles(M1, M2, B, &g_hc67a4ec, &g_hc67a4d4, &g_hc67a4bc);
    double b1 = x87_fabs(B[0]);
    if (b1 > g_hc64b810 || x87_fabs(B[1]) > g_hc64fa48)
        return 1;
    double b3 = x87_fabs(B[2]);
    if (b3 > g_hc64fa48)
        return 1;
    if (b1 < g_hc67a4b0 && b3 < g_hc67a4b4)
        return 0;

    Matrix iM1, iM2, tmp;
    Matrix_Inverse(&iM1, M1);
    Matrix_Inverse(&iM2, M2);
    Matrix L1 = g_identity, L2 = g_identity, Ry = g_identity;
    BYTE* rs = (BYTE*)&g_hc697f9c + F(int, s, 10420) * 2416;
    BYTE* ro = (BYTE*)&g_hc697f9c + F(int, o, 10420) * 2416;
    Vec3 H1 = Hc_Through(&F(float, s, 708), (const float*)(rs + 156));
    float r204 = F(float, rs, 204);
    float q[3];
    q[0] = (float)((double)(float)(r204 * (double)g_axisY.x) + F(float, ro, 168));
    q[1] = (float)((double)(float)(r204 * (double)g_axisY.y) + F(float, ro, 172));
    q[2] = (float)(r204 * (double)g_axisY.z + F(float, ro, 176));
    Vec3 H2 = Hc_Through(&F(float, o, 708), q);
    float ms = F(float, s, 3284), mo = F(float, o, 3284);
    double totd = (double)mo + ms;
    float tot = (float)totd;
    float inv = (float)(g_hc64b670 / totd);
    float d = (float)(((double)ms * A[0] - (double)mo * B[0]) * inv);
    if (b1 > g_hc67a4b0 && g_hc67a4b0 < x87_fabs((double)d))
    {
        double k = d > g_hc64c264 ? (double)g_hc67a4b0 : -(double)g_hc67a4b0;
        d = (float)(d - k);
        // the mean hitch axis
        const float* P = M1->m;
        const float* Q = M2->m;
        const float* Z = &g_axisZ.x;
        double e1x = (double)Z[2] * P[6] + (double)Z[1] * P[3] + (double)Z[0] * P[0];
        float e1y = (float)((double)Z[2] * P[7] + (double)Z[1] * P[4] + (double)Z[0] * P[1]);
        float e1z = (float)((double)Z[2] * P[8] + (double)Z[1] * P[5] + (double)Z[0] * P[2]);
        double e2x = (double)Z[2] * Q[6] + (double)Z[1] * Q[3] + (double)Z[0] * Q[0];
        float e2y = (float)((double)Z[2] * Q[7] + (double)Z[1] * Q[4] + (double)Z[0] * Q[1]);
        float e2z = (float)((double)Z[2] * Q[8] + (double)Z[1] * Q[5] + (double)Z[0] * Q[2]);
        float t1 = (float)(e2x * mo), t2 = (float)(e2y * (double)mo), t3 = (float)(e2z * (double)mo);
        double sx = e1x * ms + t1;
        float sy = (float)((double)t2 + (float)(e1y * (double)ms));
        float sz = (float)((double)t3 + (float)(e1z * (double)ms));
        Vec3 N;
        N.x = (float)(sx * inv);
        N.y = (float)((double)sy * inv);
        N.z = (float)((double)sz * inv);
        CallThis(Hc_NormalizeThis, &N);
        const Vec3& Ps = F(Vec3, s, 3304);
        const Vec3& Po = F(Vec3, o, 3304);
        Vec3 V;
        V.x = (float)((double)Po.x + Ps.x);
        V.y = (float)((double)Po.y + Ps.y);
        V.z = (float)((double)Po.z + Ps.z);
        Vec3 a = Hc_Col(iM1.m, N), b = Hc_Col(iM2.m, N);
        float d2 = (float)((double)d * inv);
        Matrix_LookAt(&tmp, H1, a, (float)((double)d2 * mo * g_hc64d378));
        L1 = tmp;
        Matrix_LookAt(&tmp, H2, b, (float)(-((double)d2 * ms) * g_hc64d378));
        L2 = tmp;
        // the shared inertia about the hitch
        BYTE* is = (BYTE*)&g_hc697db8 + F(int, s, 10420) * 2416;
        BYTE* io = (BYTE*)&g_hc697db8 + F(int, o, 10420) * 2416;
        double u = ((double)F(float, is, 0) + F(float, io, 0)) * ((double)g_hc6ef2ac + g_hc64c250) * g_hc64b6e0;
        double v = ((double)F(float, is, 4) + F(float, io, 4)) * ((double)g_hc6ef2b0 + g_hc64c250) * g_hc64b6e0;
        V.z = (float)((v * v + u * u) * tot * g_hc64daf4 * g_hc67a4b8);
        // the velocities: the common one plus each one's normal part and its damped side part
        BYTE* sb = F(BYTE*, s, 21600);
        BYTE* ob = F(BYTE*, o, 21600);
        Vec3 vs, vo;
        CallThis(Hc_GetVec9012, sb, &vs);
        CallThis(Hc_GetVec9012, ob, &vo);
        Vec3 po = Hc_Mul(vo, mo), ps = Hc_Mul(vs, ms), sum, Vavg;
        sum.x = (float)((double)ps.x + po.x);
        sum.y = (float)((double)ps.y + po.y);
        sum.z = (float)((double)ps.z + po.z);
        Vavg = Hc_Mul(sum, inv);
        Vec3 dvs = Hc_Lin(vs, 1.0, Vavg, -1.0), dvo = Hc_Lin(vo, 1.0, Vavg, -1.0);
        double k1 = (double)dvs.x * N.x + (double)dvs.z * N.z + (double)dvs.y * N.y;
        Vec3 ns = Hc_Mul(N, k1), ts = Hc_Lin(dvs, 1.0, ns, -1.0);
        double k2 = (double)dvo.x * N.x + (double)dvo.z * N.z + (double)dvo.y * N.y;
        Vec3 no = Hc_Mul(N, k2), to = Hc_Lin(dvo, 1.0, no, -1.0);
        // the spins
        Vec3 ws, wo, W, Ls, Lo, c, q1, q2;
        CallThis(Hc_GetVec9024, sb, &ws);
        CallThis(Hc_GetVec9024, ob, &wo);
        W = Hc_Lin(wo, 1.0, ws, 1.0);
        CallThis(Hc_GetVec9048, sb, &Ls);
        CallThis(Hc_GetVec9048, ob, &Lo);
        c = Hc_Col(iM1.m, W);
        Vec3_DivComponents(&q1, &c, &V);
        c = Hc_Col(iM2.m, W);
        Vec3_DivComponents(&q2, &c, &V);
        Vec3 rs1 = Hc_Lin(Ls, 1.0, q1, -1.0), ro1 = Hc_Lin(Lo, 1.0, q2, -1.0);
        double k3 = (double)rs1.x * a.x + (double)a.y * rs1.y + (double)rs1.z * a.z;
        Vec3 ra = Hc_Mul(a, k3), rt = Hc_Lin(rs1, 1.0, ra, -1.0);
        Vec3 bp, bo, t, n1, n2, n3, n4, out;
        Vec3_Scaled(&bp, &b, (float)Vec3_Dot(&ro1, &b));
        Vec3_Sub(&bo, &ro1, &bp);
        float K = g_hc6ef2cc;
        Vec3_Sub(&n1, &ns, Vec3_Scaled(&t, &ts, K));
        Vec3_Sub(&n2, &no, Vec3_Scaled(&t, &to, K));
        Vec3_Sub(&n3, &rt, Vec3_Scaled(&t, &ra, K));
        Vec3_Sub(&n4, &bo, Vec3_Scaled(&t, &bp, K));
        Hc_SetVec9012(F(BYTE*, s, 21600), 0, *Vec3_Add(&out, &n1, &Vavg));
        Hc_SetVec9012(F(BYTE*, o, 21600), 0, *Vec3_Add(&out, &n2, &Vavg));
        Hc_SetSpin(F(BYTE*, s, 21600), 0, *Vec3_Add(&out, &n3, &q1));
        Hc_SetSpin(F(BYTE*, o, 21600), 0, *Vec3_Add(&out, &n4, &q2));
    }
    if (g_hc6ef2d0)
    {
        // the trailer's yaw
        float a = (float)((A[2] - B[2]) * g_hc64b6e0);
        Vec3 w;
        CallThis(Hc_GetVec9048, F(BYTE*, o, 21600), &w);
        double h = (double)g_hc67a4b4 * g_hc64b6e0;
        float hf = (float)h;
        double aa = x87_fabs((double)a);
        if (g_hc67a4b4 < aa)
        {
            float ang = (float)(-(double)a * g_hc64b6e0);
            Ry = *Matrix_RotationY(&tmp, ang);
            if ((double)w.y * ang < g_hc64c264)
            {
                w.y = (float)(w.y * g_hc64b720);
                Hc_SetSpin(F(BYTE*, o, 21600), 0, w);
            }
        }
        else if (aa > h)
        {
            float ang = (float)((aa - hf) * -(double)a / ((double)g_hc67a4b4 - hf));
            Ry = *Matrix_RotationY(&tmp, ang);
        }
        if ((double)w.y * a > g_hc64c264)
        {
            float f = (float)(g_hc64b670 - aa * g_hc64b668 / g_hc67a4b4);
            if (f < g_hc64c264)
                f = 0;
            w.y = (float)(Game_FrameTime() * f * w.y * g_hc64b788);
            Hc_SetSpin(F(BYTE*, o, 21600), 0, w);
        }
    }
    Matrix t1, r1, r2;
    Matrix I = g_identity;
    Matrix_Multiply(&t1, &I, &L1);
    Matrix_Multiply(&r1, &t1, M1);
    Matrix_Multiply(&t1, &Ry, &L2);
    Matrix_Multiply(&r2, &t1, M2);
    *M1 = r1;
    *M2 = r2;
    return 0;
}

} // extern "C"

// === the dashboard's overlays (FUN_53d9d0)

struct Dd_Style
{
    DWORD d[9];
};
extern "C" void __cdecl Dd_Text(int surf, int x, int y, int color, const char* s, int a, int b);
KING_ALIAS("_Dd_Text", "_Text_Draw")
extern "C" void __cdecl Dd_Frame(int surf, int* rect, int mode, Dd_Style st);
KING_ALIAS("_Dd_Frame", "_Ctl_DrawFrame")
extern "C" void FUN_53e7a0(); KING_ALIAS("_FUN_53e7a0", "_Cars_DumpToLog")
extern "C" void FUN_53d8c0(); KING_ALIAS("_FUN_53d8c0", "@Dash_TextPlace@20")
KING_VAR(int, g_dd696c64, 696c64)
KING_VAR(BYTE*, g_dd6d2078, 6d2078)
KING_VAR(BYTE*, g_dd696ca0, 696ca0)
KING_VAR(int, g_dd696bec, 696bec)
KING_VAR(int, g_dd696be8, 696be8)
KING_VAR(int, g_dd6cec68, 6cec68)
KING_VAR(int, g_dd696c34, 696c34)
KING_VAR(char, g_dd6792ac, 6792ac)
KING_VAR(char, g_dd67929c, 67929c)
KING_VAR(char, g_dd679288, 679288)
KING_VAR(char, g_dd679278, 679278)
KING_VAR(char, g_dd67925c, 67925c)
KING_VAR(char, g_dd679250, 679250)
KING_VAR(char, g_dd67924c, 67924c)
KING_VAR(char, g_dd679234, 679234)
KING_VAR(char, g_dd679214, 679214)

static Dd_Style Dd_Plain()
{
    Dd_Style st;
    memset(&st, 0, sizeof st);
    float h = 0.5f;
    memcpy(&st.d[6], &h, 4);
    return st;
}

static void Dd_Rect(int surf, int l, int t, int r, int b, int mode, const Dd_Style& st)
{
    int R[4] = {l, t, r, b};
    Dd_Frame(surf, R, mode, st);
}

// the five debug lines (never written here) at the panel's corner +1376 / +1380 from row y0, each with a shadow
static void Dd_Lines(BYTE* s, int surf, int color, int W, char (*txt)[52], int y0)
{
    static const int order[5] = {3, 4, 0, 1, 2};
    for (int k = 0; k < 5; k++)
    {
        int y = F(int, s, 1380) + y0 + 10 * k;
        Dd_Text(surf, F(int, s, 1376) + W - 90, y + 1, color, txt[order[k]], 0, 0);
        Dd_Text(surf, F(int, s, 1376) + W - 91, y, color, txt[order[k]], 6, 0);
    }
}

extern "C" {

// [FUN_53d9d0] dashboard s's overlays: the road sign text of the car (Car_54efd0, 696c64), the debug lines
// (696bec / 696be8), then without 6cec68 the frames around the view (+1400 cab view: above and below the window
// +32, flag 128 the gauges; else flag 8 the dash image of mode +1396 and the frames around it) and with flag 32
// the debug lines (+1404 flags)
void __fastcall Dash_DrawOverlays(BYTE* s, void*)
{
    char buf[256];
    char txt[5][52];
    if (g_dd696c64)
    {
        int k;
        float v;
        if (Car_54efd0(F(BYTE*, g_dd6d2078, 616), 0, &k, (int*)&v) && k)
        {
            int color = F(int, g_dd696ca0, 2864);
            buf[0] = 0;
            static const char* const names[6] = {&g_dd6792ac, &g_dd67929c, &g_dd679288, &g_dd679278, &g_dd67925c, &g_dd679250};
            if (k >= 1 && k <= 6)
                strcpy(buf, names[k - 1]);
            sprintf(buf + strlen(buf), &g_dd67924c, (double)v);
            Dd_Text(F(int, g_dd696ca0, 12), 120, 120, color, buf, 4, 0);
        }
    }
    if (g_dd696bec > 1)
    {
        CallC(FUN_53e7a0);
        sprintf(buf, &g_dd679234);
        Dd_Text(F(int, g_dd696ca0, 12), 120, 140, F(int, g_dd696ca0, 2864), buf, 5, 0);
    }
    if (g_dd696be8 > 1)
    {
        sprintf(buf, &g_dd679214);
        Dd_Text(F(int, g_dd696ca0, 12), 120, 160, F(int, g_dd696ca0, 2864), buf, 5, 0);
    }
    if (g_dd6cec68)
        return;
    Dd_Style st = Dd_Plain();
    int color = F(int, s, 40);
    int R[4];
    CallThis(FUN_5d85f0, F(BYTE*, s, 32), R);
    BYTE* w = F(BYTE*, s, 36);
    int W = F(int, w, 256), H = F(int, w, 260);
    int surf = F(int, F(BYTE*, w, 252), 12);
    if (W >= 640)
        color = F(int, F(BYTE*, s, 32), 2864);
    BYTE fl = s[1404];
    if (F(int, s, 1400))
    {
        if (fl & 32)
            Dd_Lines(s, surf, color, W, txt, 50);
        Dd_Rect(surf, 0, 0, W, R[1] - 1, 2, st);
        Dd_Rect(surf, 0, R[3] + 1, W, H, 2, st);
        if (s[1404] & 128)
        {
            st = Dd_Plain();
            Dd_Rect(surf, 2, H - 52, 128, H - 7, 1, st);
            Dash_Prepare(s, 0, 5, H - 45);
        }
        return;
    }
    if (fl & 8)
    {
        st = Dd_Plain();
        int x, y;
        CallThis(FUN_53d8c0, s, F(int, s, 1396), &x, &y);
        if (W == 800 && H == 600)
            y += 50;
        int mode = F(int, s, 1396);
        CSurface* sf = (CSurface*)surf;
        switch (mode)
        {
        case 0:
            Gfx_DrawImage(sf, x, y, F(void*, s, 628), 0);
            if (s[1404] & 128)
                Dash_Prepare(s, 0, x + 172, y + 356);
            Gfx_DrawImage(sf, x + 115, y + 325, F(void*, s, 904 + 4 * F(int, s, 1060)), 0);
            break;
        case 1:
        case 2:
            Gfx_DrawImage(sf, x, y, F(void*, s, 628 + 4 * mode), 0);
            break;
        case -1:
        case -2:
            Gfx_DrawImage(sf, x, y, F(void*, s, 640 - 4 * mode), 0);
            break;
        }
        if (y > 0)
            Dd_Rect(surf, 0, 0, W, y, 2, st);
        y += (&g_dd696c34)[F(int, s, 1396)];
        if (y < H)
            Dd_Rect(surf, 0, y, W, H, 2, st);
    }
    if (s[1404] & 32)
        Dd_Lines(s, surf, color, W, txt, 40);
}

} // extern "C"

// === the vehicle's sounds and force feedback (FUN_538370)

extern "C" void __cdecl Vn_Play388(void* s, float v); KING_ALIAS("_Vn_Play388", "_Sound_Play388")
extern "C" void __cdecl Vn_Play3_388(void* s, float v); KING_ALIAS("_Vn_Play3_388", "_Sound_Play3_388")
extern "C" void __cdecl Vn_Play3_388Pos(void* s, float v, float w); KING_ALIAS("_Vn_Play3_388Pos", "_Sound_Play3_388Pos")
extern "C" void __cdecl Vn_Start388(void* s, float a, float v, const void* p); KING_ALIAS("_Vn_Start388", "_Sound_Start388")
extern "C" void __cdecl Vn_Start388Pos(void* s, float a, float v, float w, const void* p);
KING_ALIAS("_Vn_Start388Pos", "_Sound_Start388Pos")
extern "C" void __cdecl Vn_Loop388(void* s, float a, float v, const void* p); KING_ALIAS("_Vn_Loop388", "_Sound_Loop388")
extern "C" void __cdecl Vn_Play390If(void* s, DWORD a, DWORD b, const void* p); KING_ALIAS("_Vn_Play390If", "_Sound_Play390If")
extern "C" void __cdecl Vn_Play3_386(void* s, float v); KING_ALIAS("_Vn_Play3_386", "_Sound_Play3_386")
extern "C" void __cdecl Vn_Start386(void* s, float a, float v, const void* p); KING_ALIAS("_Vn_Start386", "_Sound_Start386")
extern "C" void __cdecl Vn_Loop386(void* s, DWORD a, DWORD v, const void* p); KING_ALIAS("_Vn_Loop386", "_Sound_Loop386")
extern "C" void __cdecl Vn_PlayRange3(void* s, float a, float v, const void* p); KING_ALIAS("_Vn_PlayRange3", "_Sound_PlayRange3")
extern "C" void Vn_Switch(); KING_ALIAS("_Vn_Switch", "_Switch_Select")
extern "C" void Vn_Log(); KING_ALIAS("_Vn_Log", "_VehLog_Value")
extern "C" void Vn_FilterValue(); KING_ALIAS("_Vn_FilterValue", "@Filter_Value@4")
extern "C" void Vn_ActionState(); KING_ALIAS("_Vn_ActionState", "@Controls_ActionState@12")
extern "C" void Vn_SoundPlay(); KING_ALIAS("_Vn_SoundPlay", "@Sound_Play@20")
extern "C" void Vn_Decompose(); KING_ALIAS("_Vn_Decompose", "_Matrix_Decompose6F")
extern "C" void Vn_MulAssign(); KING_ALIAS("_Vn_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Vn_AddAssign(); KING_ALIAS("_Vn_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Vn_NormalizeThis(); KING_ALIAS("_Vn_NormalizeThis", "@Vec3_NormalizeThis@8")
extern "C" void __fastcall JoyEffect_Stop(BYTE* self, void*);
extern "C" int __fastcall JoyEffect_GetParameters(BYTE* self, void*, DWORD* params, int flags);
extern "C" int __fastcall JoyEffect_GetStatus(BYTE* self, void*, int* status);
extern "C" void __fastcall JoyEffect_SetGain(BYTE* self, void*, float v);
extern "C" void __fastcall JoyEffect_SetMagnitude(BYTE* self, void*, float v);
extern "C" void __fastcall FfEffect_SetPeriodic(DWORD* fx, void*, float a, float b, float c);
extern "C" void __fastcall FfEffect_SetCondition(DWORD* fx, void*, float a, float b);
extern "C" void __fastcall FfEffect_Restart(DWORD* fx, void*, float a, float b);
extern "C" int __cdecl Joy_SendCommand(int command);
extern "C" Matrix* __fastcall SceneObj_Matrix548(BYTE* self, void*, Matrix* out);
extern "C" Matrix* __fastcall SceneObj_Matrix644(BYTE* self, void*, Matrix* out);
extern "C" Matrix* __fastcall SceneObj_Matrix1(BYTE* self, void*, Matrix* out);
extern "C" void FUN_4eacce(); KING_ALIAS("_FUN_4eacce", "@History_Record@16")
KING_VAR(BYTE*, g_vn696ca0, 696ca0)
KING_VAR(int, g_vn696c74, 696c74)                 // the force feedback log
KING_VAR(int, g_vn696ca4, 696ca4)
KING_VAR(BYTE*, g_vn696cf0, 696cf0)               // the force effects
KING_VAR(BYTE*, g_vn696cf4, 696cf4)
KING_VAR(BYTE*, g_vn696cf8, 696cf8)
KING_VAR(BYTE*, g_vn696cfc, 696cfc)
KING_VAR(BYTE*, g_vn696d00, 696d00)
KING_VAR(BYTE*, g_vn696d04, 696d04)
KING_VAR(void*, g_vn6d1d44, 6d1d44)               // the sounds
KING_VAR(void*, g_vn6d1d10, 6d1d10)
KING_VAR(void*, g_vn6d1d4c, 6d1d4c)
KING_VAR(void*, g_vn6d1d50, 6d1d50)
KING_VAR(void*, g_vn6d1d28, 6d1d28)
KING_VAR(void*, g_vn6d1d1c, 6d1d1c)
KING_VAR(void*, g_vn6d1d48, 6d1d48)
KING_VAR(void*, g_vn6d1cf4, 6d1cf4)
KING_VAR(void*, g_vn6d1cf8, 6d1cf8)
KING_VAR(void*, g_vn6d1d00, 6d1d00)
KING_VAR(void*, g_vn6d1d04, 6d1d04)
KING_VAR(void*, g_vn6d1d08, 6d1d08)
KING_VAR(void*, g_vn6d1d0c, 6d1d0c)
KING_VAR(void*, g_vn6d1cd0, 6d1cd0)
KING_VAR(void*, g_vn6d1d20, 6d1d20)
KING_VAR(void*, g_vn6d1d80, 6d1d80)
KING_VAR(void*, g_vn6d1d40, 6d1d40)
KING_VAR(void*, g_vn6d1d3c, 6d1d3c)
KING_VAR(void*, g_vn6d1d54, 6d1d54)               // [3]
KING_VAR(DWORD, g_vn6d1d84, 6d1d84)               // [11]
KING_VAR(int, g_vn6d20e0, 6d20e0)
KING_VAR(float, g_vn6d1db4, 6d1db4)
KING_VAR(signed char, g_vn6d1db0, 6d1db0)
KING_VAR(float, g_vn6d20d8, 6d20d8)
KING_VAR(float, g_vn678780, 678780)
KING_VAR(float, g_vn678788, 678788)
KING_VAR(AsmLabel, g_vn6d1db8, 6d1db8)
KING_VAR(AsmLabel, g_vn697b30, 697b30)
KING_VAR(char, g_vn6791e4, 6791e4)
KING_VAR(char, g_vn6791d4, 6791d4)
KING_VAR(char, g_vn6791c0, 6791c0)
KING_VAR(char, g_vn6791ac, 6791ac)
KING_VAR(char, g_vn679198, 679198)
KING_VAR(char, g_vn679184, 679184)
KING_VAR(char, g_vn679170, 679170)
KING_VAR(char, g_vn679160, 679160)
KING_VAR(char, g_vn679138, 679138)
KING_VAR(char, g_vn679124, 679124)
KING_VAR(char, g_vn67910c, 67910c)
KING_VAR(char, g_vn6790f4, 6790f4)
KING_VAR(char, g_vn6790e0, 6790e0)
KING_VAR(char, g_vn6790c8, 6790c8)
KING_VAR(char, g_vn6790b0, 6790b0)
KING_VAR(char, g_vn679098, 679098)
KING_VAR(char, g_vn67907c, 67907c)
KING_VAR(char, g_vn679064, 679064)
KING_VAR(char, g_vn679050, 679050)
KING_VAR(char, g_vn67903c, 67903c)
KING_VAR(char, g_vn679028, 679028)
KING_VAR(char, g_vn679018, 679018)
KING_VAR(char, g_vn678ffc, 678ffc)
KING_VAR(char, g_vn678fe0, 678fe0)
KING_VAR(char, g_vn678fcc, 678fcc)
KING_VAR(char, g_vn678fb8, 678fb8)
KING_VAR(char, g_vn678fa4, 678fa4)
KING_VAR(char, g_vn678f8c, 678f8c)
KING_VAR(char, g_vn678f70, 678f70)
KING_VAR(char, g_vn678f58, 678f58)
KING_VAR(float, g_vn64c264, 64c264)
KING_VAR(float, g_vn64c250, 64c250)
KING_VAR(float, g_vn64f3fc, 64f3fc)
KING_VAR(float, g_vn64c320, 64c320)
KING_VAR(float, g_vn64c40c, 64c40c)
KING_VAR(float, g_vn64c2fc, 64c2fc)
KING_VAR(float, g_vn67440c, 67440c)
KING_VAR(float, g_vn64c25c, 64c25c)
KING_VAR(float, g_vn64f3f8, 64f3f8)
KING_VAR(float, g_vn64c450, 64c450)
KING_VAR(float, g_vn64c938, 64c938)
KING_VAR(float, g_vn64c254, 64c254)
KING_VAR(float, g_vn64d2f8, 64d2f8)
KING_VAR(float, g_vn64d4e8, 64d4e8)
KING_VAR(float, g_vn64d490, 64d490)
KING_VAR(float, g_vn64f3e0, 64f3e0)
KING_VAR(double, g_vn64f370, 64f370)
KING_VAR(double, g_vn64b8e8, 64b8e8)
KING_VAR(double, g_vn64b658, 64b658)
KING_VAR(double, g_vn64b6e0, 64b6e0)
KING_VAR(double, g_vn64ca78, 64ca78)
KING_VAR(double, g_vn64b840, 64b840)
KING_VAR(double, g_vn64b898, 64b898)
KING_VAR(double, g_vn64b5b8, 64b5b8)
KING_VAR(double, g_vn64d0f0, 64d0f0)
KING_VAR(double, g_vn64b7a8, 64b7a8)
KING_VAR(double, g_vn64f3f0, 64f3f0)
KING_VAR(double, g_vn64b670, 64b670)
KING_VAR(double, g_vn64d338, 64d338)
KING_VAR(double, g_vn64b838, 64b838)
KING_VAR(double, g_vn64d668, 64d668)
KING_VAR(double, g_vn64ca48, 64ca48)
KING_VAR(double, g_vn64b6f0, 64b6f0)
KING_VAR(double, g_vn64b780, 64b780)
KING_VAR(double, g_vn64b938, 64b938)
KING_VAR(double, g_vn64b5a0, 64b5a0)
KING_VAR(double, g_vn64b958, 64b958)
KING_VAR(double, g_vn64b738, 64b738)
KING_VAR(double, g_vn64b918, 64b918)
KING_VAR(double, g_vn64b668, 64b668)
KING_VAR(double, g_vn64d378, 64d378)
KING_VAR(double, g_vn64b6d8, 64b6d8)
KING_VAR(double, g_vn64b930, 64b930)
KING_VAR(double, g_vn64f3e8, 64f3e8)
KING_VAR(double, g_vn64d988, 64d988)

// point p (+0 / +4 / +8) in the listener's frame (s +1472)
static Vec3 Vn_Ear(BYTE* s, const float* p)
{
    const float* L = (const float*)(s + 1472);
    Vec3 r;
    r.x = (float)((double)L[3] * p[1] + (double)L[6] * p[2] + (double)p[0] * L[0] + L[9]);
    r.y = (float)((double)L[1] * p[0] + (double)L[4] * p[1] + (double)L[7] * p[2] + L[10]);
    r.z = (float)((double)L[2] * p[0] + (double)L[5] * p[1] + (double)L[8] * p[2] + L[11]);
    return r;
}

// a sound by the cab view (+1400): looped at Q with range a, else played once
static void Vn_Sound(BYTE* s, void* snd, float a, float v, const Vec3* Q)
{
    if (F(int, s, 1400))
        Vn_Loop388(snd, a, v, Q);
    else
        Vn_Play388(snd, v);
}

static void Vn_LogText(const char* t)
{
    CallC(Vn_Log, t);
}

// (v x w) x w
static Vec3 Vn_Twice(const Vec3& v, const float* w)
{
    double c1 = (double)v.z * w[1] - (double)v.y * w[2];
    double c2 = (double)v.x * w[2] - (double)v.z * w[0];
    double c3 = (double)v.y * w[0] - (double)v.x * w[1];
    Vec3 r;
    r.x = (float)(c2 * w[2] - c3 * w[1]);
    r.y = (float)(c3 * w[0] - c1 * w[2]);
    r.z = (float)(c1 * w[1] - c2 * w[0]);
    return r;
}

// the speed at a wheel at r: the spin B across r turned by M, plus the speed A
static Vec3 Vn_WheelSpeed(const Vec3& B, const float* r, const float* M, const Vec3& A)
{
    double c1 = (double)B.z * r[1] - (double)B.y * r[2];
    double c2 = (double)B.x * r[2] - (double)B.z * r[0];
    double c3 = (double)B.y * r[0] - (double)B.x * r[1];
    Vec3 v;
    v.x = (float)((double)(float)(c3 * M[6] + c2 * M[3] + M[0] * c1) + A.x);
    v.y = (float)((double)A.y + (float)(M[1] * c1 + c3 * M[7] + c2 * M[4]));
    v.z = (float)(c1 * M[2] + c3 * M[8] + c2 * M[5] + A.z);
    return v;
}

static float Vn_Clamp1(float v)
{
    if (v > g_vn64b670)
        v = 1.0f;
    if (v < g_vn64ca48)
        v = -1.0f;
    return v;
}

// an effect's status line for the log
static void Vn_LogStatus(BYTE* e, const char* fmt)
{
    char buf[256];
    int st = 0;
    JoyEffect_GetStatus(e, 0, &st);
    sprintf(buf, fmt, st, F(int, e, 0));
    Vn_LogText(buf);
}

extern "C" {

// [FUN_538370] vehicle s's sounds (+616 car, ear frame +1472, cab view +1400): the indicator click and lamps
// (+440 / +444), the horn, brakes and gear sounds (car +20960.. readings), a nearby car's engine and two others
// mixed by distance, the engine start and stop, the motor rumble by load (+21012), the wind; then the steering
// wheel's force feedback (effects 696cf0..696d04: centring, damping, the road kick from the front wheels, the
// slope, sand and ice shakes, hits) with its log, collision bangs (+21608.., +21652 points), the skid squeal
// (+15512), the air brake hiss (+116)
void __fastcall Vehicle_Sounds(BYTE* s, void*, int, int)
{
    char buf[256];
    BYTE* car = F(BYTE*, s, 616);
    if (!car)
        return;
    // the indicator
    if (F(int, car, 21504))
    {
        CallC(Vn_Switch, F(int, s, 440), 1);
        car = F(BYTE*, s, 616);
        float v = F(float, car, 20952);
        if (v > g_vn64c264)
        {
            double x = v;
            if (x < g_vn64c264)
                x = g_vn64c264;
            else if (!(x <= g_vn64c250))
                x = g_vn64c250;
            int n = 1 - (int)(x * g_vn64f3fc);
            if (n > 3)
                n = 3;
            else if (n < 1)
                n = 1;
            int now = F(int, g_vn696ca0, 2992);
            if (now >= F(int, s, 448) + F(int, s, 452))
            {
                F(int, s, 448) = now;
                float k = (float)(g_vn64c250 / (x * g_vn64c320 + g_vn64c40c));
                F(int, s, 452) = (int)(k / Game_FrameTime());
            }
            float vol = F(int, s, 1400) ? 0.5f : 1.0f;
            int p = F(int, s, 452);
            int half = p / 2 + F(int, s, 448);
            bool on = (p & 1) ? now <= half : now < half;
            if (on)
            {
                Vn_Play3_386(g_vn6d1d44, vol);
                CallC(Vn_Switch, F(int, s, 444), n);
            }
            else
                CallC(Vn_Switch, F(int, s, 444), 0);
        }
        else
            CallC(Vn_Switch, F(int, s, 444), 0);
    }
    else
        CallC(Vn_Switch, F(int, s, 440), 0);
    car = F(BYTE*, s, 616);
    F(int, car, 20952) = 0;
    DWORD L[20];
    memcpy(L, car + 20960, 80);
    // the car's sound point in the ear's frame
    const float* C = (const float*)(car + 3352);
    double dx = (double)g_axisY.x - g_axisZ.x, dy = (double)g_axisY.y - g_axisZ.y, dz = (double)g_axisY.z - g_axisZ.z;
    float P[3];
    P[0] = (float)(dz * C[6] + dy * C[3] + dx * C[0] + C[9]);
    P[1] = (float)(dz * C[7] + dy * C[4] + dx * C[1] + C[10]);
    double pz = dz * C[8] + dy * C[5] + dx * C[2] + C[11];
    const float* E = (const float*)(s + 1472);
    Vec3 Q;
    Q.x = (float)(pz * E[6] + (double)P[1] * E[3] + (double)P[0] * E[0] + E[9]);
    Q.y = (float)((double)P[0] * E[1] + pz * E[7] + (double)P[1] * E[4] + E[10]);
    Q.z = (float)((double)P[0] * E[2] + pz * E[8] + (double)P[1] * E[5] + E[11]);
    Vec3 R;
    {
        const float* t = (const float*)(car + 3388);
        R.y = (float)((double)E[4] * t[1] + (double)E[1] * t[0] + (double)E[7] * t[2] + E[10]);
        R.z = (float)((double)E[5] * t[1] + (double)E[2] * t[0] + (double)E[8] * t[2] + E[11]);
        R.x = (float)((double)E[3] * t[1] + (double)E[6] * t[2] + (double)t[0] * E[0] + E[9]);
    }
    float dt = (float)F(double, F(BYTE*, s, 32), 2960);
    float rate = (float)(g_vn64c250 / dt);
    if (L[10])
    {
        if (!g_vn6d20e0)
        {
            Vn_Sound(s, g_vn6d1d10, 5.0f, F(int, car, 21024) ? 0.3f : 0.5f, &Q);
            g_vn6d20e0 = 1;
        }
    }
    else
        g_vn6d20e0 = 0;
    if (L[12])
        Vn_Sound(s, g_vn6d1d4c, 5.0f, 1.0f, &Q);
    if (L[11])
        Vn_Sound(s, g_vn6d1d50, 5.0f, 2.0f, &Q);

    // other cars close by: one parked (state 5), two moving, mixed by distance and mass
    BYTE* A = 0;
    BYTE* B1 = 0;
    BYTE* B2 = 0;
    for (int k = 0; k < 6; k++)
    {
        BYTE* o = F(BYTE*, F(BYTE*, s, 616), 3524 + 4 * k);
        bool ok = o && F(int, o, 392) == 2 && F(int, o, 3568) == 2;
        if (!A && ok && F(int, o - 16, 9548) == 5)
            A = o - 16;
        if (!B1)
        {
            if (ok && F(int, o, 9532) != 3 && F(int, o, 9532) != 4 && A != o - 16)
                B1 = o - 16;
        }
        else if (ok && F(int, o - 16, 9548) != 3 && F(int, o - 16, 9548) != 4 && A != o - 16)
        {
            B2 = o - 16;
            break;
        }
    }
    if (A)
    {
        Vec3 p = Vn_Ear(s, (const float*)(A + 3388));
        Vn_Start386(g_vn6d1d28, 5.0f, 2.0f, &p);
    }
    if (B1)
    {
        Vec3 p1 = Vn_Ear(s, (const float*)(B1 + 3388));
        double w = g_vn64c264;
        int m = F(int, B1, 9548);
        if (m == 0 || m == 1 || m == 2)
            w = g_vn64f370 / ((double)p1.x * p1.x + (double)p1.y * p1.y + (double)p1.z * p1.z) * F(float, B1, 3284) * g_vn64b8e8;
        if (B2 && (F(int, B2, 9548) == 0 || F(int, B2, 9548) == 1))
        {
            Vec3 p2 = Vn_Ear(s, (const float*)(B2 + 3388));
            float w2 = (float)(g_vn64f370 / ((double)p2.x * p2.x + (double)p2.y * p2.y + (double)p2.z * p2.z) * F(float, B2, 3284) * g_vn64b8e8);
            double sx = (double)p1.x * w + (double)p2.x * w2;
            float sy = (float)((double)(float)((double)p2.y * w2) + (float)((double)p1.y * w));
            float sz = (float)((double)(float)((double)p2.z * w2) + (float)((double)p1.z * w));
            w = w + w2;
            p1.x = (float)(sx / w);
            p1.y = (float)(sy / w);
            p1.z = (float)(sz / w);
        }
        if (w > g_vn64b658)
            Vn_Start386(g_vn6d1d1c, 5.0f, 1.0f, &p1);
    }

    // starting and stopping the engine
    car = F(BYTE*, s, 616);
    if (F(int, car, 21012) == 1 && F(int, s, 16) > 1)
    {
        double now = F(double, g_vn696ca0, 2968);
        if (F(float, car, 20988) == g_vn64c264)
        {
            void* snd = g_vn6d1d48;
            if (snd && (double)g_vn6d1db4 + g_vn64c2fc < now)
            {
                Vn_Sound(s, snd, 6.0f, 1.0f, &Q);
                g_vn6d1db4 = (float)F(double, g_vn696ca0, 2968);
            }
        }
        else if ((double)g_vn6d1db4 + g_vn64c2fc < now)
        {
            Vn_Sound(s, g_vn6d1cf4, 6.0f, 1.0f, &Q);
            g_vn6d1db4 = (float)F(double, g_vn696ca0, 2968);
        }
        g_vn6d1db0 = 0;
    }
    // the motor by load
    int m = F(int, car, 21012);
    if (m == 2 || m == 3)
    {
        float k808 = 1.0f, k804 = 1.0f;
        if (m == 3)
        {
            k808 = 0;
            signed char c = g_vn6d1db0;
            g_vn6d1db0 = c + 1;
            float vol = 0.4f;
            k804 = (float)(c * (double)dt / g_vn67440c);
            if (k804 < g_vn64b6e0)
            {
                vol = (float)((double)k804 * g_vn64c25c);
                k808 = (float)((g_vn64b6e0 - k804) * 2);
            }
            if (F(int, s, 1400))
                Vn_Start388(g_vn6d1cf8, 6.0f, vol, &Q);
            else
                Vn_Play3_388(g_vn6d1cf8, vol);
        }
        car = F(BYTE*, s, 616);
        if (F(int, car, 21012) == 2 || k804 < g_vn64b6e0)
        {
            float ld, rpm, x788, x536, a792, a772;
            memcpy(&ld, &L[2], 4);
            float l4, l6;
            memcpy(&l4, &L[4], 4);
            memcpy(&l6, &L[6], 4);
            k804 = (float)(ld / (double)l4);
            rpm = (float)((double)l6 + k804);
            double a = ((double)l6 * g_vn64ca78 + g_vn64b840) * k808;
            x788 = (float)(k804 * g_vn64b840 + g_vn64b898);
            x536 = (float)(k804 * g_vn64b898 + g_vn64b840);
            double b = x87_sqrt(k804) * k808 * g_vn64b5b8;
            if (b > g_vn64c250)
                b = g_vn64c250;
            if (rpm > g_vn64c250)
                rpm = 1.0f;
            float rest = (float)(g_vn64c250 - rpm);
            a792 = (float)(b * rpm + rest * a);
            a772 = (float)((double)x536 * rpm + (double)rest * x788);
            bool heavy = F(float, car, 3284) > g_vn64f3f8;
            if (F(int, s, 1400))
                Vn_Start388Pos(heavy ? g_vn6d1d00 : g_vn6d1d08, 10.0f, a792, a772, &Q);
            else
                Vn_Play3_388Pos(heavy ? g_vn6d1d00 : g_vn6d1d08, a792, a772);
            double r = x87_sqrt(rpm);
            a772 = (float)(((double)l6 * g_vn64d0f0 + g_vn64b7a8) * r * k808 * g_vn64b6e0);
            a792 = (float)(r * k804 * g_vn64b898 + g_vn64b840);
            heavy = F(float, F(BYTE*, s, 616), 3284) > g_vn64f3f8;
            if (F(int, s, 1400))
                Vn_Start388Pos(heavy ? g_vn6d1d04 : g_vn6d1d0c, 10.0f, a772, a792, &Q);
            else
                Vn_Play3_388Pos(heavy ? g_vn6d1d04 : g_vn6d1d0c, a772, a792);
        }
    }
    // the wind
    car = F(BYTE*, s, 616);
    if (F(float, car, 21036) != g_vn64c264)
        Vn_Play390If(g_vn6d1cd0, L[19], L[18], &R);

    BYTE* body = F(BYTE*, car, 21600);
    if (!body)
        return;
    int ff = F(short, body, 9990) == 1;
    if (g_vn696c74)
        ff = 1;
    int log = g_vn696c74;
    // the wheel's effects on or off with the body's ground contact
    if (F(int, body, 13380) && F(int, body, 13384))
    {
        if (F(int, s, 112) == 1)
        {
            if (g_vn696cf4)
                JoyEffect_Stop(g_vn696cf4, 0);
            if (g_vn696c74)
                Vn_LogText(&g_vn6791e4);
            BYTE** fx[5] = {&g_vn696cf8, &g_vn696cfc, &g_vn696d00, &g_vn696cf0, &g_vn696d04};
            const char* names[5] = {&g_vn6791d4, &g_vn6791c0, &g_vn6791ac, &g_vn679198, &g_vn679184};
            for (int k = 0; k < 5; k++)
            {
                if (*fx[k])
                    JoyEffect_Stop(*fx[k], 0);
                if (g_vn696c74)
                {
                    sprintf(buf, names[k]);
                    Vn_LogText(buf);
                }
            }
            Joy_SendCommand(4);
            F(int, s, 112) = 0;
        }
    }
    else if (!F(int, s, 112))
    {
        Joy_SendCommand(8);
        F(int, s, 112) = 1;
    }
    if (g_vn696ca4 && F(int, s, 112))
    {
        double sum = g_vn64c264;
        for (int k = 0; k < 2; k++)
            sum += F(float, body, 15280 + 4 * k);
        float avg = (float)(sum * g_vn64b6e0);
        if (ff)
        {
            // 1: the centring spring
            if (g_vn696cf4)
            {
                float v = (float)(F(float, body, 13104) * (double)avg * g_vn64b5b8);
                if (v > avg)
                    v = avg;
                if (v > g_vn64c250)
                    v = 1.0f;
                int gr = F(int, body, 15560);
                float dir = 0;
                if (gr > 0)
                    dir = 0.2f;
                if (gr < 0)
                    dir = -0.2f;
                if (gr == F(int, body, 15564))
                    dir = 0.5f;
                if (gr == -F(int, body, 15564))
                    dir = -0.5f;
                if (v > g_vn64f3f0)
                {
                    FfEffect_SetCondition((DWORD*)g_vn696cf4, 0, v, dir);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn679170, (double)v, (double)dir);
                        Vn_LogText(buf);
                    }
                }
                else
                {
                    JoyEffect_Stop(g_vn696cf4, 0);
                    if (g_vn696c74)
                        Vn_LogText(&g_vn6791e4);
                }
            }
            // 2: the damping
            if (g_vn696cf8)
            {
                float v = (float)(((double)g_vn64c450 - F(float, body, 13104)) * avg * g_vn64b5b8);
                if (v > avg)
                    v = avg;
                if (v > g_vn64b670)
                    v = 1.0f;
                if (v > g_vn64d338)
                {
                    JoyEffect_SetGain(g_vn696cf8, 0, v);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn679160, (double)v);
                        Vn_LogText(buf);
                    }
                }
                else
                {
                    JoyEffect_Stop(g_vn696cf8, 0);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn6791d4);
                        Vn_LogText(buf);
                    }
                }
            }
            // 3: the road's kick through the front wheels
            if (g_vn696cfc)
            {
                BYTE* co = F(BYTE*, s, 616) + 16;
                Matrix m1, rz, X, M, N, MN, T;
                Matrix_Multiply(&X, Matrix_RotationZ(&rz, F(float, body, 13196)), SceneObj_Matrix1(co, 0, &m1));
                const float* Y = &g_axisY.x;
                Vec3 u;
                u.x = (float)((double)Y[1] * X.m[3] + (double)Y[2] * X.m[6] + (double)Y[0] * X.m[0]);
                u.y = (float)((double)Y[1] * X.m[4] + (double)Y[2] * X.m[7] + (double)Y[0] * X.m[1]);
                u.z = (float)((double)Y[1] * X.m[5] + (double)Y[2] * X.m[8] + (double)Y[0] * X.m[2]);
                Vec3 v = u;
                if (F(short, body, 11112))
                    u = Vn_Twice(u, (const float*)(body + 11100));
                if (F(short, body, 11360))
                    v = Vn_Twice(v, (const float*)(body + 11348));
                CallThis(Vn_NormalizeThis, &u);
                CallThis(Vn_NormalizeThis, &v);
                SceneObj_Matrix1(co, 0, &M);
                Vec3 Av = {0, 0, 0}, Bv = {0, 0, 0};
                Matrix_Multiply(&MN, &M, SceneObj_Matrix644(co, 0, &N));
                CallC(Vn_Decompose, &MN, &Av.x, &Av.y, &Av.z, &Bv.z, &Bv.x, &Bv.y);
                const Matrix* Tm = SceneObj_Matrix548(co, 0, &T);
                float ddx = (float)((double)M.m[9] - Tm->m[9]);
                float ddy = (float)((double)M.m[10] - Tm->m[10]);
                float ddz = (float)((double)M.m[11] - Tm->m[11]);
                Av.x = (float)((double)ddx * rate);
                Av.y = (float)((double)ddy * rate);
                Av.z = (float)((double)ddz * rate);
                Bv.x = -Bv.x;
                Vec3 tmp;
                CallThis(Vn_MulAssign, &Bv, &tmp, rate);
                BYTE* model = (BYTE*)&g_vn697b30 + F(int, body, 2252) * 2416;
                float t796;
                if (F(short, body, 11112) && !F(int, body, 13380))
                {
                    Vec3 V = Vn_WheelSpeed(Bv, (const float*)(body + 10976), M.m, Av);
                    Vec3 F1 = Vn_Twice(V, (const float*)(body + 11100));
                    double ls = Vec3_Length(&F1) * x87_sin(F(float, body, 13196));
                    double k1 = (double)F(float, body, 13128) * 2 / F(float, model, 36);
                    t796 = (float)(k1 * ls);
                }
                else
                    t796 = 0;
                double x;
                if (F(short, body, 11360) && !F(int, body, 13380))
                {
                    Vec3 V = Vn_WheelSpeed(Bv, (const float*)(body + 10988), M.m, Av);
                    Vec3 F2 = Vn_Twice(V, (const float*)(body + 11348));
                    double ls = Vec3_Length(&F2) * x87_sin(F(float, body, 13200));
                    t796 = (float)((double)F(float, body, 13132) * 2 / F(float, model, 40) * t796);
                    x = ls + t796;
                }
                else
                    x = (double)g_vn64c264 + t796;
                DWORD fl = F(DWORD, F(BYTE*, s, 616), 3480);
                if (!fl || (fl & 32))
                    x *= g_vn64d668;
                else
                    x = x / (x87_fabs(x) + g_vn64b838);
                float f800 = (float)(x * avg);
                if (f800 > avg)
                    f800 = avg;
                if (f800 > g_vn64b670)
                    f800 = 1.0f;
                if (f800 < -(double)avg)
                    f800 = -avg;
                if (f800 < g_vn64ca48)
                    f800 = -1.0f;
                // the steering wheel's own spring through the dash filter (+344)
                int st = F(int, body, 15368);
                float stf = st ? (float)st : 0.0f;
                DWORD stb;
                memcpy(&stb, &stf, 4);
                CallThis(FUN_4eacce, s + 344, stb, &g_vn679138);
                double r = CallThis<double>(Vn_FilterValue, s + 344);
                if (r > g_vn64c250)
                    r = g_vn64c250;
                else if (r < g_vn64c938)
                    r = g_vn64c938;
                f800 = (float)(r + f800);
                // the ice and sand shakes
                float shake = 0;
                int n = 0;
                for (int k = 0; k < 2; k++)
                {
                    BYTE* W = body + 11220 + 248 * k;
                    BYTE* gr = F(BYTE*, W, 0);
                    if (F(short, W, -108) && gr && F(int, gr, 0) == 1 && F(int, gr, 4) == 1 && !F(int, body, 13380 + 4 * k))
                        n++;
                }
                if (n)
                {
                    double tt = F(double, F(BYTE*, s, 32), 2960);
                    for (int k = 0; k < 4; k++)
                        F(float, s, 120 + 4 * k) = (float)(tt * F(float, body, 13100) + F(float, s, 120 + 4 * k));
                    double q = x87_sqrt(x87_sqrt((double)F(float, body, 13100) + g_vn64c250));
                    F(float, s, 152) = (float)(q * g_vn64d2f8);
                    F(float, s, 168) = (float)q;
                    F(float, s, 156) = (float)(q * g_vn64c320);
                    F(float, s, 172) = (float)(q * g_vn64d4e8);
                    F(float, s, 160) = (float)(q * g_vn64b6f0);
                    F(float, s, 176) = (float)(q * g_vn64b780);
                    F(float, s, 164) = (float)(q * g_vn64b938);
                    F(float, s, 180) = (float)(q * g_vn64b780);
                    for (int k = 0; k < 2; k++)
                    {
                        BYTE* e = s + 120 + 4 * k;
                        if (!(F(float, e, 0) > F(float, e, 16)))
                            continue;
                        F(float, e, 0) = 0;
                        double r1 = (rand() * (double)g_vn64c254 + g_vn64b670) * g_vn64b6e0;
                        if (F(float, body, 13100) > g_vn64b5a0)
                            r1 = r1 / (F(float, body, 13100) * g_vn64b5b8);
                        else
                            r1 = g_vn64c450;
                        F(float, e, 16) = (float)(r1 * F(float, e, 32) + F(float, e, 48));
                        double r2 = rand() * (double)g_vn64c254 - g_vn64b6e0;
                        r2 = r2 > g_vn64c264 ? r2 + g_vn64b670 : r2 - g_vn64b670;
                        shake = (float)(r2 * F(float, e, 64) + shake);
                    }
                }
                f800 = Vn_Clamp1((float)((double)shake + f800));
                if (x87_fabs((double)f800) > g_vn64b958)
                {
                    JoyEffect_SetMagnitude(g_vn696cfc, 0, f800);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn679124, (double)f800);
                        Vn_LogText(buf);
                    }
                }
                else
                {
                    JoyEffect_Stop(g_vn696cfc, 0);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn6791c0);
                        Vn_LogText(buf);
                    }
                }
            }
            // 4: the gear's pull
            if (BYTE* e4 = g_vn696d00)
            {
                int gr = F(int, body, 15560);
                float v = 0;
                if (gr)
                {
                    v = (float)(gr / (double)(F(int, body, 15564) - 1) * F(float, body, 13104) * g_vn64b5b8);
                    if (v > g_vn64b6e0)
                        v = 0.5f;
                }
                if (v > g_vn64d338)
                {
                    JoyEffect_SetMagnitude(e4, 0, v);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn67910c, (double)v);
                        Vn_LogText(buf);
                    }
                }
                else
                {
                    JoyEffect_Stop(e4, 0);
                    if (g_vn696c74)
                    {
                        sprintf(buf, &g_vn6791ac);
                        Vn_LogText(buf);
                    }
                }
            }
        }
    }
    // the rough ground rumble (effect 5 and its sound)
    int started = 0;
    if (!F(int, body, 13124) && !F(int, body, 13412))
    {
        double sum = g_vn64c264;
        int n = 0;
        for (int k = 0; k < 4; k++)
        {
            BYTE* W = body + 11220 + 248 * k;
            BYTE* gr = F(BYTE*, W, 0);
            if (F(short, W, -108) && gr && F(int, gr, 0) == 1 && (F(int, gr, 4) == 6 || F(int, gr, 4) == 2))
            {
                sum += g_vn64b738;
                n++;
            }
        }
        if (n && F(float, body, 13100) > g_vn64c250 && !F(int, body, 13380) && !F(int, body, 13384))
        {
            float vol = (float)(x87_sqrt(((double)F(float, body, 13100) - g_vn64c250) * g_vn64d490) * sum);
            if (vol > g_vn64c250)
                vol = 1.0f;
            Vn_Start386(g_vn6d1d20, 5.0f, vol, &R);
            if (!ff)
                goto bangs;
            if (g_vn696ca4 && F(int, s, 112) && g_vn696cf0)
            {
                double a = x87_fabs((double)F(float, body, 13100));
                float per = (float)(a * g_vn64b918 / (a + g_vn64b918));
                int d = F(int, body, 13380) ? -1 : (F(int, body, 13384) != 0);
                float f808 = (float)d;
                float mag = 0.2f;
                double now = F(double, F(BYTE*, s, 32), 2968);
                double x;
                if (g_vn64b6e0 / per < now - F(float, s, 48))
                {
                    F(float, s, 48) = (float)F(double, F(BYTE*, s, 32), 2968);
                    float r;
                    do
                    {
                        r = (float)((rand() * (double)g_vn64c254 * g_vn64b668 + g_vn64b6e0) * per);
                    } while (g_vn678788 * g_vn64ca78 > x87_fabs((double)r - g_vn678788));
                    x = r;
                    g_vn678788 = r;
                }
                else
                    x = g_vn678788;
                for (int k = 0; k < 2; k++)
                {
                    BYTE* W = body + 11220 + 248 * k;
                    BYTE* gr = F(BYTE*, W, 0);
                    if (!F(short, W, -108) || !gr || F(int, gr, 0) != 1)
                        continue;
                    if (F(int, gr, 4) == 6)
                    {
                        mag = (float)(vol * g_vn64ca78);
                        x *= g_vn64d378;
                    }
                    else if (F(int, gr, 4) == 2)
                        mag = (float)(vol * g_vn64b6e0);
                }
                float f800 = (float)x;
                FfEffect_SetPeriodic((DWORD*)g_vn696cf0, 0, mag, f808, f800);
                if (g_vn696c74)
                {
                    sprintf(buf, &g_vn6790f4, (double)mag, (double)f808, (double)f800);
                    Vn_LogText(buf);
                }
                started = 1;
            }
        }
    }
    if (ff && !started && g_vn696cf0 && g_vn696ca4 && F(int, s, 112))
    {
        JoyEffect_Stop(g_vn696cf0, 0);
        if (g_vn696c74)
        {
            sprintf(buf, &g_vn679198);
            Vn_LogText(buf);
        }
    }
bangs:
    // the horn
    if (CallThis<int>(Vn_ActionState, &g_vn6d1db8, 21) && !F(int, body, 13124))
    {
        void* snd = (void*)Car_543bb0(F(BYTE*, s, 616), 0);
        if (F(int, s, 1400))
        {
            if (snd)
                Vn_PlayRange3(snd, 15.0f, 1.0f, &R);
        }
        else if (snd)
            CallThis(Vn_SoundPlay, snd, 1.0f, 0.5f, 0);
    }
    // the hits: fresh ones kick the wheel (effect 6)
    int any = 0;
    started = 0;
    for (int i = 0; i < 11; i++)
    {
        BYTE* c = F(BYTE*, s, 616);
        if (F(int, c, 21608 + 4 * i) && Game_Time() - F(float, c, 21828 + 4 * i) < g_vn64b6e0)
            F(int, c, 21608 + 4 * i) = 0;
        any = F(int, F(BYTE*, s, 616), 21608 + 4 * i) || any;
    }
    if (g_vn696ca4 && F(int, s, 112) && ff && g_vn696d04 && any)
    {
        Vec3 acc = g_zeroVec;
        float mag = 0;
        for (int j = 0; j < 11; j++)
        {
            BYTE* c = F(BYTE*, s, 616);
            if (!F(int, c, 21608 + 4 * j))
                continue;
            mag = F(float, c, 21784 + 4 * j);
            const float* p = (const float*)(c + 21652 + 12 * j);
            double px = (double)F(float, c, 3412) * p[1] + (double)F(float, c, 3424) * p[2] + (double)p[0] * F(float, c, 3400) + F(float, c, 3436);
            float py = (float)((double)F(float, c, 3404) * p[0] + (double)F(float, c, 3416) * p[1] + (double)F(float, c, 3428) * p[2] + F(float, c, 3440));
            float pz = (float)((double)F(float, c, 3408) * p[0] + (double)F(float, c, 3420) * p[1] + (double)F(float, c, 3432) * p[2] + F(float, c, 3444));
            Vec3 d, tmp;
            d.x = (float)(mag * px);
            d.y = (float)((double)py * mag);
            d.z = (float)((double)pz * mag);
            CallThis(Vn_AddAssign, &acc, &tmp, &d);
        }
        if (mag > g_vn64b5b8)
        {
            if (mag > g_vn64c250)
                mag = 1.0f;
            if (acc.x > g_vn64c264)
                mag = -mag;
            float dur = (float)(x87_fabs(mag * g_vn64b840) + g_vn64b5b8);
            FfEffect_Restart((DWORD*)g_vn696d04, 0, mag, dur);
            started = 1;
            if (g_vn696c74)
            {
                sprintf(buf, &g_vn6790e0, (double)mag, (double)dur);
                Vn_LogText(buf);
            }
        }
    }
    if (F(int, s, 112) && g_vn696c74 && !started)
    {
        sprintf(buf, &g_vn679184);
        Vn_LogText(buf);
    }
    if (g_vn696c74)
    {
        // the effects' states for the log
        if (g_vn696cf4)
            Vn_LogStatus(g_vn696cf4, &g_vn6790c8);
        if (g_vn696cf8)
            Vn_LogStatus(g_vn696cf8, &g_vn6790b0);
        if (g_vn696cfc)
            Vn_LogStatus(g_vn696cfc, &g_vn679098);
        if (g_vn696d00)
            Vn_LogStatus(g_vn696d00, &g_vn67907c);
        if (BYTE* e = g_vn696d04)
        {
            Vn_LogStatus(e, &g_vn679064);
            DWORD P[14], env[5], axes[32], dirs[32], per[4];
            memset(P, 0, sizeof P);
            memset(env, 0, sizeof env);
            env[0] = 20;
            P[0] = 56;
            P[1] = 18;
            P[7] = 32;
            P[8] = (DWORD)axes;
            P[9] = (DWORD)dirs;
            P[10] = (DWORD)env;
            P[11] = 16;
            P[12] = (DWORD)per;
            JoyEffect_GetParameters(g_vn696d04, 0, P, 999);
            sprintf(buf, &g_vn679050, P[1], P[1]);
            Vn_LogText(buf);
            sprintf(buf, &g_vn67903c, (int)P[2] * g_vn64b6d8);
            Vn_LogText(buf);
            sprintf(buf, &g_vn679028, (int)P[3] * g_vn64b6d8);
            Vn_LogText(buf);
            sprintf(buf, &g_vn679018, (int)P[4] * g_vn64b930);
            Vn_LogText(buf);
            if (P[10])
                sprintf(buf, &g_vn678ffc, (int)env[1] * g_vn64b930, (int)env[2] * g_vn64b6d8, (int)env[3] * g_vn64b930,
                        (int)env[4] * g_vn64b6d8);
            else
                sprintf(buf, &g_vn678fe0, -1, -1, -1, -1);
            Vn_LogText(buf);
            sprintf(buf, &g_vn678fcc, (double)P[13] * g_vn64b6d8);
            Vn_LogText(buf);
            sprintf(buf, &g_vn678fb8, P[7]);
            Vn_LogText(buf);
            sprintf(buf, &g_vn678fa4, *(int*)P[8]);
            Vn_LogText(buf);
            sprintf(buf, &g_vn678f8c, *(int*)P[9]);
            Vn_LogText(buf);
            if (P[11] == 16 && P[12])
                sprintf(buf, &g_vn678f70, (int)per[0] * g_vn64b930, (int)per[3] * g_vn64b6d8, (int)per[2] * g_vn64f3e8,
                        (int)per[1] * g_vn64b930);
            else
                sprintf(buf, &g_vn678f58);
            Vn_LogText(buf);
        }
    }
    // the tyres' squeal
    car = F(BYTE*, s, 616);
    Vec3 Rp;
    {
        const float* t = (const float*)(car + 3388);
        Rp.z = (float)((double)E[2] * t[0] + (double)E[8] * t[2] + (double)E[5] * t[1] + E[11]);
        Rp.x = (float)((double)E[6] * t[2] + (double)E[3] * t[1] + (double)E[0] * t[0] + E[9]);
        Rp.y = (float)((double)E[1] * t[0] + (double)E[7] * t[2] + (double)E[4] * t[1] + E[10]);
    }
    if (car)
    {
        BYTE* b = F(BYTE*, car, 21600);
        if (b && F(int, b, 15512))
        {
            double a = x87_fabs((double)F(float, b, 13100));
            if (a > g_vn64b6e0)
            {
                float pitch = (float)(a * g_vn64d988 / (a + g_vn64c320) + g_vn64ca78);
                double v = a / (a + g_vn64d2f8);
                float vol = (float)v;
                int cnt = 0;
                int n = F(int, b, 13120);
                for (int c = 0; c < n && c < 2; c++)
                    if (F(int, b, 13380 + 4 * c))
                        cnt++;
                if (cnt == 2)
                {
                    v *= g_vn64b668;
                    vol = (float)v;
                }
                if (v > g_vn64c250)
                    vol = 1.0f;
                if (cnt)
                    Vn_Start388Pos(g_vn6d1d80, 3.0f, vol, pitch, &Rp);
            }
        }
    }
    // the bangs
    for (int i = 0; i < 11; i++)
    {
        BYTE* c = F(BYTE*, s, 616);
        if (!F(int, c, 21608 + 4 * i))
            continue;
        F(int, c, 21608 + 4 * i) = 0;
        c = F(BYTE*, s, 616);
        Vec3 p = Vn_Ear(s, (const float*)(c + 21652 + 12 * i));
        int v = i;
        if (i == 0)
        {
            if (rand() % 2 == 0)
                v = 2;
            else if (rand() % 2 == 0)
                v = 1;
        }
        c = F(BYTE*, s, 616);
        Vn_Loop386((&g_vn6d1d54)[v], (&g_vn6d1d84)[i], F(DWORD, c, 21784 + 4 * i), &p);
    }
    // the air brake
    car = F(BYTE*, s, 616);
    if (!(F(float, car, 3284) > g_vn64f3e0))
        return;
    int on = F(float, body, 15312) > g_vn64b5b8 || F(int, body, 15440) ? 1 : 0;
    if (F(int, s, 116) != on)
    {
        Vn_Sound(s, on ? g_vn6d1d40 : g_vn6d1d3c, 5.0f, 1.0f, &Q);
        F(int, s, 116) = on;
        return;
    }
    if (!on && F(float, body, 13096) < g_vn64b5b8)
    {
        if ((double)g_vn678780 + g_vn6d20d8 < F(double, F(BYTE*, s, 32), 2968))
            Vn_Sound(s, g_vn6d1d40, 5.0f, 1.0f, &Q);
        g_vn6d20d8 = (float)F(double, F(BYTE*, s, 32), 2968);
    }
    F(int, s, 116) = on;
}

} // extern "C"

// === hitching a trailer (FUN_55f5f0)

extern "C" void At_HitchCheck(); KING_ALIAS("_At_HitchCheck", "_Trailer_HitchCheck")
extern "C" void At_Attach1(); KING_ALIAS("_At_Attach1", "@Vehicle_Attach55ec70@12")
extern "C" void At_Attach2(); KING_ALIAS("_At_Attach2", "@Vehicle_AttachCar@12")
extern "C" void At_Get800(); KING_ALIAS("_At_Get800", "@SceneObj_Get800@8")
extern "C" void At_Set54c880(); KING_ALIAS("_At_Set54c880", "@Car_Set54c880@16")
extern "C" void At_SetMode(); KING_ALIAS("_At_SetMode", "@Body_SetMode@12")
extern "C" void At_HitObject(); KING_ALIAS("_At_HitObject", "_Query_HitObject")
extern "C" Matrix* __cdecl At_FromPose(Matrix* out, double a, double b, double c, double yaw, double pitch, int e, int f);
KING_ALIAS("_At_FromPose", "_Matrix_FromPose")
KING_VAR(double, g_at64b7a8, 64b7a8)

extern "C" {

// [FUN_55f5f0] truck s takes trailer t (unless Trailer_HitchCheck refuses: 0): linked both ways, a trailer lying
// over set upright by its angles, with put the trailer's frame moved so the hitch points (the model records'
// +156 and +168) meet (its object found again by a ray), both bodies in mode 3 sharing the controls (+9988, +9990);
// 1
int __cdecl Truck_HitchTrailer(BYTE* s, BYTE* t, int put)
{
    if (CallC<int>(At_HitchCheck, s, t))
        return 0;
    CallThis(At_Attach1, t, s);
    CallThis(At_Attach2, s, t);
    BYTE* rs = (BYTE*)&g_hc697f9c + F(int, s, 10420) * 2416;
    BYTE* rt = (BYTE*)&g_hc697f9c + F(int, t, 10420) * 2416;
    Vec3 H1 = Hc_Through(&F(float, s, 708), (const float*)(rs + 156));
    float r204 = F(float, rs, 204);
    float q[3];
    q[0] = (float)((double)(float)(g_axisY.x * (double)r204) + F(float, rt, 168));
    q[1] = (float)((double)(float)(g_axisY.y * (double)r204) + F(float, rt, 172));
    q[2] = (float)(g_axisY.z * (double)r204 + F(float, rt, 176));
    Vec3 H2 = Hc_Through(&F(float, t, 708), q);
    Matrix A, B, tmp;
    SceneObj_Matrix1(s + 16, 0, &A);
    SceneObj_Matrix1(t + 16, 0, &B);
    if (B.m[8] < g_at64b7a8)
    {
        const float* Y = &g_axisY.x;
        float ux = (float)((double)Y[1] * B.m[3] + (double)Y[2] * B.m[6] + (double)Y[0] * B.m[0]);
        float uy = (float)((double)Y[1] * B.m[4] + (double)Y[2] * B.m[7] + (double)Y[0] * B.m[1]);
        float pitch = (float)x87_asin((double)B.m[5]);
        double a[6];
        Hc_ToAngles(B, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
        B = *At_FromPose(&tmp, a[0], a[1], a[2], -x87_atan2(ux, uy), pitch, 0, 0);
        CallThis(At_Set54c880, t, CallThis<BYTE*>(At_Get800, t + 16), &B);
    }
    if (put)
    {
        Vec3 p1, p2, d;
        Vec3_MulMatrix(&p1, &H1, &A);
        Vec3_MulMatrix(&p2, &H2, &B);
        d.x = (float)((double)p1.x - p2.x);
        d.y = (float)((double)p1.y - p2.y);
        d.z = (float)((double)p1.z - p2.z);
        Matrix T, N;
        Matrix_Multiply(&N, &B, Matrix_Translation(&T, d));
        BYTE* o = t + 16;
        BYTE* hit = CallC<BYTE*>(At_HitObject, CallThis<BYTE*>(At_Get800, o), &B.m[9], &N.m[9]);
        if (!hit)
        {
            hit = CallThis<BYTE*>(At_Get800, o);
            CallThis((void (*)())F(void**, o, 0)[7], o, 1);
        }
        CallThis(At_Set54c880, t, hit, &N);
        CallThis((void (*)())F(void**, o, 0)[37], o, &g_zeroVec, 0);
        CallThis((void (*)())F(void**, o, 0)[36], o, &g_zeroVec, 0);
    }
    CallThis(At_SetMode, F(BYTE*, s, 21600), 3);
    CallThis(At_SetMode, F(BYTE*, t, 21600), 3);
    F(short, F(BYTE*, t, 21600), 9988) = F(short, F(BYTE*, s, 21600), 9988);
    F(short, F(BYTE*, t, 21600), 9990) = F(short, F(BYTE*, s, 21600), 9990);
    return 1;
}

} // extern "C"

// === the cars dumped to the warning log (FUN_53e7a0)

extern "C" void Cd_Open(); KING_ALIAS("_Cd_Open", "_WarnLog_Open")
extern "C" void Cd_Close(); KING_ALIAS("_Cd_Close", "_WarnLog_Close")
extern "C" void Cd_Log(); KING_ALIAS("_Cd_Log", "_VehLog_Value2")
extern "C" void Cd_Get800(); KING_ALIAS("_Cd_Get800", "@SceneObj_Get800@8")
extern "C" void Cd_Matrix2(); KING_ALIAS("_Cd_Matrix2", "@SceneObj_Matrix2@12")
KING_VAR(BYTE*, g_cd6ef1f8, 6ef1f8)               // the car list
KING_VAR(BYTE*, g_cd6d2078, 6d2078)
KING_VAR(char, g_cd6794e8, 6794e8)
KING_VAR(char, g_cd6794c8, 6794c8)
KING_VAR(char, g_cd6794a4, 6794a4)
KING_VAR(char, g_cd67948c, 67948c)
KING_VAR(char, g_cd67946c, 67946c)
KING_VAR(char, g_cd679464, 679464)
KING_VAR(char, g_cd679458, 679458)
KING_VAR(char, g_cd679440, 679440)
KING_VAR(char, g_cd679434, 679434)
KING_VAR(char, g_cd67941c, 67941c)
KING_VAR(char, g_cd679404, 679404)
KING_VAR(char, g_cd6793e4, 6793e4)
KING_VAR(char, g_cd6793c0, 6793c0)
KING_VAR(char, g_cd6793a0, 6793a0)
KING_VAR(char, g_cd679380, 679380)
KING_VAR(char, g_cd679360, 679360)
KING_VAR(char, g_cd679340, 679340)
KING_VAR(char, g_cd679328, 679328)
KING_VAR(char, g_cd67930c, 67930c)
KING_VAR(char, g_cd6792f8, 6792f8)

static void Cd_Text(const char* t)
{
    CallC(Cd_Log, t);
}

// the matrix's four rows, the first with its own heading
static void Cd_Matrix(const float* m, const char* first)
{
    char buf[256];
    sprintf(buf, first, (double)m[0], (double)m[1], (double)m[2]);
    Cd_Text(buf);
    for (int r = 1; r < 4; r++)
    {
        sprintf(buf, &g_cd67948c, (double)m[3 * r], (double)m[3 * r + 1], (double)m[3 * r + 2]);
        Cd_Text(buf);
    }
}

extern "C" {

// [FUN_53e7a0] the clock, the camera's frame (player +1424) and every car's name, frame, speed along its heading,
// velocity, spin, state (+10924) and position (+20676) written to the warning log
void __cdecl Cars_DumpToLog()
{
    if (!g_dd696ca0 || !g_cd6d2078)
        return;
    char buf[256];
    CallC(Cd_Open);
    Cd_Text(&g_cd6794e8);
    BYTE* g = g_dd696ca0;
    sprintf(buf, &g_cd6794c8, F(double, g, 2968), F(int, g, 2992), F(double, g, 2960));
    Cd_Text(buf);
    Matrix cam = F(Matrix, g_cd6d2078, 1424);
    Cd_Matrix(cam.m, &g_cd6794a4);
    int n = 0;
    for (BYTE* c = g_cd6ef1f8 ? g_cd6ef1f8 - 4 : 0; c; n++)
    {
        sprintf(buf, &g_cd67946c, F(int, F(BYTE*, c, 336), 12), c, F(int, c, 32));
        Cd_Text(buf);
        if (F(int, c, 1220))
            Cd_Text(&g_cd679464);
        sprintf(buf, &g_cd679458, F(int, CallThis<BYTE*>(Cd_Get800, c + 16), 12));
        Cd_Text(buf);
        Matrix m;
        SceneObj_Matrix1(c + 16, 0, &m);
        Cd_Matrix(m.m, &g_cd679440);
        Vec3 v = F(Vec3, c, 432);
        Matrix m2;
        const float* H = CallThis<const Matrix*>(Cd_Matrix2, c + 16, &m2)->m;
        sprintf(buf, &g_cd679434, (double)v.x * H[1] + (double)v.y * H[4] + (double)v.z * H[7]);
        Cd_Text(buf);
        sprintf(buf, &g_cd67941c, (double)v.x, (double)v.y, (double)v.z);
        Cd_Text(buf);
        Vec3 w = F(Vec3, c, 444);
        sprintf(buf, &g_cd679404, (double)w.x, (double)w.y, (double)w.z);
        Cd_Text(buf);
        static const char* const states[6] = {&g_cd6793e4, 0, &g_cd6793a0, &g_cd679380, &g_cd679360, &g_cd679340};
        unsigned st = F(unsigned, c, 10924);
        if (st == 1)
        {
            sprintf(buf, &g_cd6793c0, F(int, F(BYTE*, c, 21600), 9784));
            Cd_Text(buf);
        }
        else
            Cd_Text(st <= 5 ? states[st] : &g_cd679328);
        sprintf(buf, &g_cd67930c, (double)F(float, c, 20676), (double)F(float, c, 20680), (double)F(float, c, 20684));
        Cd_Text(buf);
        c = F(BYTE*, c, 8) ? F(BYTE*, c, 8) - 4 : 0;
    }
    sprintf(buf, &g_cd6792f8, n);
    Cd_Text(buf);
    CallC(Cd_Close);
}

} // extern "C"

// === the car's light shadows and lamps (FUN_552940)

extern "C" void Lt_NodeInit(); KING_ALIAS("_Lt_NodeInit", "@Node_Init@8")
extern "C" void Lt_Shadow(); KING_ALIAS("_Lt_Shadow", "_Fx_Shadow")
extern "C" void Lt_Sound(); KING_ALIAS("_Lt_Sound", "_Fx_PlaySound")
extern "C" void Lt_ListLight(); KING_ALIAS("_Lt_ListLight", "_EffectList_AddLight")
extern "C" void Lt_Decal(); KING_ALIAS("_Lt_Decal", "_Fx_AddDecal")
extern "C" void Lt_Light(); KING_ALIAS("_Lt_Light", "_Fx_AddLight")
extern "C" void Lt_HitObject(); KING_ALIAS("_Lt_HitObject", "_Query_HitObject")
extern "C" void Lt_Query(); KING_ALIAS("_Lt_Query", "_Query_4e6578")
extern "C" void Lt_Fatal(); KING_ALIAS("_Lt_Fatal", "_World_Fatal")
KING_VAR(BYTE*, g_lt6cec6c, 6cec6c)
KING_VAR(BYTE*, g_lt6d2078, 6d2078)
KING_VAR(BYTE*, g_lt6f33ac, 6f33ac)
KING_VAR(int, g_lt6f33c0, 6f33c0)
KING_VAR(Vec3, g_ltAhead, 702620)
KING_VAR(Vec3, g_ltUp, 702630)
KING_VAR(Vec3, g_ltEye, 702650)
KING_VAR(Vec3, g_ltCam, 6969f4)
KING_VAR(Vec3, g_lt696bbc, 696bbc)
KING_VAR(BYTE, g_lt697db8, 697db8)                   // the car types' tables, 2416 bytes each
KING_VAR(BYTE, g_lt697f9c, 697f9c)
KING_VAR(BYTE, g_lt69818e, 69818e)
KING_VAR(BYTE, g_lt698008, 698008)
KING_VAR(BYTE, g_lt698020, 698020)
KING_VAR(double, g_lt64b6e0, 64b6e0)
KING_VAR(float, g_lt64d7b8, 64d7b8)
KING_VAR(float, g_lt64c264, 64c264)
KING_VAR(float, g_lt64c450, 64c450)
KING_VAR(float, g_lt64c320, 64c320)
KING_VAR(float, g_lt64f798, 64f798)
KING_VAR(double, g_lt64b840, 64b840)
KING_VAR(float, g_lt6796e0, 6796e0)                   // the distances for the lamp glow, its decal and its light
KING_VAR(float, g_lt6796e4, 6796e4)
KING_VAR(float, g_lt6796e8, 6796e8)
KING_VAR(double, g_lt64b668, 64b668)
KING_VAR(double, g_lt64d300, 64d300)
KING_VAR(double, g_lt64d0f0, 64d0f0)
KING_VAR(float, g_lt6796bc, 6796bc)
KING_VAR(double, g_lt64b678, 64b678)
KING_VAR(char, g_lt67a3a8, 67a3a8)

struct LtD { double x, y, z; };

// matrix m (rows at +0, +12, +24, the position at +36) applied to v, with or without the position
static LtD Lt_Mul(const BYTE* m, double x, double y, double z, bool pos)
{
    const float* M = (const float*)m;
    LtD r;
    r.x = z * M[6] + y * M[3] + x * M[0] + (pos ? M[9] : 0);
    r.y = z * M[7] + y * M[4] + x * M[1] + (pos ? M[10] : 0);
    r.z = z * M[8] + y * M[5] + x * M[2] + (pos ? M[11] : 0);
    return r;
}

static Vec3 Lt_F(const LtD& d)
{
    Vec3 r;
    r.x = (float)d.x;
    r.y = (float)d.y;
    r.z = (float)d.z;
    return r;
}

static Vec3 Lt_MulV(const BYTE* m, const Vec3& v, bool pos)
{
    return Lt_F(Lt_Mul(m, v.x, v.y, v.z, pos));
}

// whether the camera is in front of point p (world, from the car's frame) along dir
static bool Lt_Facing(BYTE* s, const Vec3& p, const Vec3& dir)
{
    Vec3 w = Lt_MulV(s + 516, p, true);
    Vec3 v;
    v.x = (float)((double)g_ltCam.x - w.x);
    v.y = (float)((double)g_ltCam.y - w.y);
    v.z = (float)((double)g_ltCam.z - w.z);
    return (double)v.z * dir.z + (double)v.y * dir.y + (double)v.x * dir.x > g_lt64b678;
}

static void Lt_Copy(BYTE* to, const BYTE* from)
{
    memcpy(to, from, 208);
}

extern "C" {

// [FUN_552940] car s (unless +9616) casts its lamps' shadows: from the four points +3848 onto up to eight shadow
// nodes, which are the lamps' (+10284 / +10332 by mask +10273) when the car is seen from outside with them on, or
// the lamp records of its cab (+21600) or the ground under it for a player's car; then in the dark or the fog and
// near the camera each of the four lamps (+10274 on) gets its glows, decals and lights where the camera is in front
// of it, flickering by a phase per lamp (+20456) moved by the speed (+432)
void __fastcall Car_LampShadows(BYTE* s, void*)
{
    if (F(int, s, 9616))
        return;
    __declspec(align(8)) BYTE nodes[8][208];
    for (int i = 0; i < 8; i++)
        CallThis(Lt_NodeInit, nodes[i]);
    Vec3 pts[4];
    memset(pts, 0, sizeof(pts));
    for (int i = 0; i < 8; i++)
    {
        F(Vec3, nodes[i], 52) = g_ltEye;
        F(Vec3, nodes[i], 64) = g_ltUp;
        F(int, nodes[i], 108) = 0;
        F(int, nodes[i], 100) = 0;
    }
    for (int i = 0; i < 4; i++)
        pts[i] = Lt_MulV(s + 516, F(Vec3, s, 3848 + 12 * i), true);
    BYTE* tbl = &g_lt697db8 + 2416 * F(int, s, 10420);
    BYTE* g = g_lt6cec6c;
    BYTE* car = F(BYTE*, s, 21600);
    int cnt = 0;
    if (F(int, s, 388) && (F(int, g, 1300) > 1 || F(int, g, 1316) == 1 || F(int, g, 1316) == 2))
    {
        // the lamps cast from their own points
        if ((F(BYTE, s, 20900) & 8) && F(int, s, 9548) != 5 && F(float, s, 548) > g_lt64b6e0)
        {
            int mask = F(BYTE, s, 10273);
            for (int i = 0; i < 4; i++)
            {
                if (!(mask & (1 << i)))
                    continue;
                const float* p = (const float*)(s + 10284 + 12 * i);
                LtD a = Lt_Mul(s + 708, p[0], p[1], p[2], true);
                F(Vec3, nodes[i], 52) = Lt_F(Lt_Mul(s + 516, a.x, a.y, a.z, true));
                const float* q = p + 12;
                LtD b = Lt_Mul(s + 708, q[0], q[1], q[2], false);
                F(Vec3, nodes[i], 64) = Lt_F(Lt_Mul(s + 516, b.x, b.y, b.z, false));
                cnt++;
            }
            CallC(Lt_Sound, nodes, cnt, pts, F(int, tbl, 8), 0);
        }
    }
    else if (F(int, s, 10924) == 1 && car && (F(BYTE, s, 20900) & 8))
    {
        BYTE* pl = g_lt6d2078;
        Vec3 dv;
        dv.x = (float)((double)F(float, pl, 1460) - F(float, s, 552));
        dv.y = (float)((double)F(float, pl, 1464) - F(float, s, 556));
        dv.z = (float)((double)F(float, pl, 1468) - F(float, s, 560));
        if (!F(int, car, 13124) && !(F(float, s, 548) < g_lt64b6e0))
        {
            // the cab's lamp records
            if (F(int, car, 9784) == 3 &&
                !((double)dv.x * dv.x + (double)dv.y * dv.y + (double)dv.z * dv.z > g_lt64d7b8))
            {
                for (int k = 0; k < F(int, car, 13120); k++)
                {
                    BYTE* r = car + 11112 + 248 * k;
                    if (F(WORD, r, 0) && !F(WORD, r, 2))
                        Lt_Copy(nodes[cnt++], r + 8);
                }
            }
            else
            {
                int first = F(float, car, 13104) < g_lt64c264 ? 2 : 0;
                for (int k = first; k < first + 2; k++)
                {
                    BYTE* r = car + 11112 + 248 * k;
                    if (first && F(int, car, 13120) < k + 1)
                        continue;
                    if (F(WORD, r, 0) && !F(WORD, r, 2))
                        Lt_Copy(nodes[cnt++], r + 8);
                }
            }
        }
        else
        {
            // the ground under the car
            double z0 = F(float, s, 752);
            float ux = (float)(g_ltUp.x * z0), uy = (float)(g_ltUp.y * z0);
            float lx = (float)((double)F(float, s, 744) - ux), ly = (float)((double)F(float, s, 748) - uy);
            double lz = z0 - g_ltUp.z * z0;
            LtD P = Lt_Mul(s + 516, lx, ly, lz, true);
            float px = (float)P.x, py = (float)P.y;
            Vec3 top, bot;
            top.x = (float)(g_ltUp.x * (double)g_lt64c450 + px);
            top.y = (float)((double)(float)(g_ltUp.y * (double)g_lt64c450) + py);
            top.z = (float)((double)(float)(g_ltUp.z * (double)g_lt64c450) + P.z);
            bot.x = (float)((double)px - g_ltUp.x * (double)g_lt64c320);
            bot.y = (float)((double)py - (float)(g_ltUp.y * (double)g_lt64c320));
            bot.z = (float)(P.z - (float)(g_ltUp.z * (double)g_lt64c320));
            __declspec(align(8)) BYTE tmp[208];
            CallThis(Lt_NodeInit, tmp);
            void* qh = CallC<void*>(Lt_HitObject, F(BYTE*, s, 816), s + 552, &top);
            if ((CallC<int>(Lt_Query, s ? s + 16 : 0, qh, &top, &bot, tmp) & 1) && !F(int, tmp, 116))
            {
                Lt_Copy(nodes[0], tmp);
                cnt = 1;
            }
        }
        CallC(Lt_Shadow, nodes, cnt, pts, F(int, tbl, 8), 0);
    }
    // the lamps, in the dark or the fog
    BYTE* w = g_lt6f33ac;
    double tod = F(double, w, 88);
    int dark = tod > g_lt64f798 || tod < g_lt64c320;
    int fog = F(float, w, 536) > g_lt64b840;
    Vec3 dc;
    dc.x = (float)((double)g_ltCam.x - F(float, s, 552));
    dc.y = (float)((double)g_ltCam.y - F(float, s, 556));
    dc.z = (float)((double)g_ltCam.z - F(float, s, 560));
    double dist0 = Vec3_Length(&dc);
    float dist = (float)dist0;
    float mx = g_lt6796e0 > g_lt6796e4 ? g_lt6796e0 : g_lt6796e4;
    if (!(mx > g_lt6796e8))
        mx = g_lt6796e8;
    if (!(dist0 < mx) || (!fog && !dark) || g_lt6f33c0)
        return;
    __declspec(align(8)) BYTE tmp[208], def[208];
    CallThis(Lt_NodeInit, tmp);
    CallThis(Lt_NodeInit, def);
    const BYTE* R = s + 516;
    Vec3 dir = Lt_MulV(R, g_ltAhead, false);
    memcpy(tmp + 4, &g_identity, 48);
    F(int, tmp, 104) = 0;
    F(int, tmp, 108) = 0;
    F(int, tmp, 116) = 0;
    F(int, tmp, 100) = 0;
    F(Vec3, tmp, 52) = g_ltEye;
    F(Vec3, tmp, 64) = g_ltEye;
    Lt_Copy(def, tmp);
    BYTE* ed = F(BYTE*, s, 9536);
    BYTE* T1 = &g_lt697f9c + 2416 * F(int, s, 10420);
    BYTE* T2 = &g_lt69818e + 2416 * F(int, s, 10420);
    for (int j = 0; j < 4; j++)
    {
        BYTE* node;
        if (!F(BYTE, s, 10274 + j))
        {
            Lt_Copy(tmp, def);
            node = F(BYTE*, s, 816);
        }
        else
        {
            if (car)
                Lt_Copy(tmp, car + 11120 + 248 * F(int, s, 20400 + 4 * j));
            else
            {
                F(Vec3, tmp, 52) = F(Vec3, s, 10284 + 12 * j);
                F(Vec3, tmp, 64) = F(Vec3, s, 10332 + 12 * j);
            }
            if (car)
                node = F(BYTE*, car, 11328 + 248 * F(int, s, 20400 + 4 * j));
            else
                node = CallC<BYTE*>(Lt_HitObject, F(BYTE*, s, 816), s + 552, tmp + 52);
        }
        if (!node)
            CallC(Lt_Fatal, &g_lt67a3a8);
        Vec3 pos[3];
        int flag[3];
        DWORD param[3];
        memset(pos, 0, sizeof(pos));
        int n = 0;
        const BYTE* M1 = s + 708;
        switch (j)
        {
        case 0:
        case 1:
        {
            // the headlamps
            Vec3 d = F(Vec3, T2, 38);
            if (j == 1)
                d.x = -d.x;
            dir = Lt_MulV(R, d, false);
            if (F(int, ed, j == 0 ? 64 : 44) || !F(int, s, 20920))
                break;
            pos[0] = Lt_MulV(M1, F(Vec3, T1, j == 0 ? 72 : 84), true);
            if (!Lt_Facing(s, pos[0], dir))
                break;
            flag[0] = 1;
            param[0] = F(DWORD, T2, 62);
            n = 1;
            break;
        }
        case 2:
        case 3:
        {
            // the rear lamps: brake (+20860) and reversing (+20856) or tail (+20916)
            Vec3 d = F(Vec3, T2, 50);
            if (j == 2)
                d.x = -d.x;
            dir = Lt_MulV(R, d, false);
            if (F(int, ed, j == 2 ? 144 : 124))
                break;
            Vec3 a;
            if (j == 2)
                a = Lt_MulV(M1, F(Vec3, T1, 120), true);
            else
                Vec3_MulMatrix(&a, (const Vec3*)(&g_lt698020 + 2416 * F(int, s, 10420)), (const Matrix*)M1);
            pos[0] = a;
            if (!Lt_Facing(s, a, dir))
                break;
            if (F(int, s, 20860))
            {
                pos[0] = a;
                flag[0] = 2;
                param[0] = F(DWORD, T2, 70);
                n = 1;
            }
            if (F(int, s, 20856) || F(int, s, 20916))
            {
                if (j == 2)
                    pos[n] = Lt_MulV(M1, F(Vec3, T1, 96), true);
                else
                    Vec3_MulMatrix(&pos[n], (const Vec3*)(&g_lt698008 + 2416 * F(int, s, 10420)), (const Matrix*)M1);
                flag[n] = 0;
                if (F(int, s, 20856))
                {
                    float k = (float)(F(float, T2, 66) * g_lt64b668);
                    param[n] = *(DWORD*)&k;
                }
                else
                    param[n] = F(DWORD, T2, 66);
                n++;
            }
            break;
        }
        }
        for (int i = 0; i < n; i++)
        {
            Vec3 pw, q, t, u;
            pw = *Vec3_MulMatrix(&t, &pos[i], (const Matrix*)(s + 564));
            q = *Vec3_MulMatrix(&t, &pos[i], (const Matrix*)R);
            Vec3 A = *Vec3_Scaled(&u, Vec3_Add(&t, &pw, &g_lt696bbc), 0.5f);
            Vec3 B = *Vec3_Scaled(&u, Vec3_Add(&t, &q, &g_ltCam), 0.5f);
            const Vec3* c = car ? (const Vec3*)(car + 11072) : &g_ltUp;
            Vec3 e, f, h, nv;
            Vec3_Cross(&f, c, Vec3_Sub(&e, &q, &g_ltCam));
            Vec3_Cross(&h, &f, c);
            nv = *Vec3_Normalized(&u, &h);
            float dd = (float)Vec3_Dot(Vec3_Sub(&t, &A, &B), &nv);
            double* ph = &F(double, s, 20456 + 8 * j);
            if (i == 0)
            {
                double v = (g_lt64d300 / (x87_sqrt(Vec3_Dot(&F(Vec3, s, 432), &F(Vec3, s, 432))) + g_lt64c320) +
                            g_lt64d0f0) * dd + *ph;
                *ph = v;
                *ph = x87_fmod(v, g_lt6796bc);
            }
            if (fog && dist < g_lt6796e0)
                CallC(Lt_ListLight, &q, (float)*ph, param[i], &dir, flag[i], node, tmp);
            if (dist < g_lt6796e4)
                CallC(Lt_Decal, &q, &dir, param[i], tmp, flag[i]);
            if (dist < g_lt6796e8)
                CallC(Lt_Light, s, &q, &dir, i + 10 * j, flag[i]);
        }
    }
}

} // extern "C"

// === the car's events (a method at $L_54d840)

extern "C" int __cdecl Screen_5692e0(int id);
extern "C" int __cdecl Screen_56f210(int id);
extern "C" void Ce_LengthSq(); KING_ALIAS("_Ce_LengthSq", "_Vec3_LengthSq4e298b")
extern "C" Vec3* __fastcall Ce_MulAssign(Vec3* s, void*, Vec3* ret, float k); KING_ALIAS("@Ce_MulAssign@16", "@Vec3_MulAssign@16")
extern "C" void Ce_Get(); KING_ALIAS("_Ce_Get", "@Obj549bf0_Get@8")
extern "C" void Ce_Start(); KING_ALIAS("_Ce_Start", "@CarEffect_Start@12")
extern "C" void Ce_Fatal(); KING_ALIAS("_Ce_Fatal", "_World_Fatal")
KING_VAR(Vec3, g_ceDir, 702640)
KING_VAR(BYTE, g_ce697f9c, 697f9c)
KING_VAR(float, g_ce64d9d8, 64d9d8)
KING_VAR(float, g_ce64c938, 64c938)
KING_VAR(float, g_ce64c490, 64c490)
KING_VAR(float, g_ce64f768, 64f768)
KING_VAR(double, g_ce64b5b8, 64b5b8)
KING_VAR(double, g_ce64b5a0, 64b5a0)
KING_VAR(double, g_ce64b668, 64b668)
KING_VAR(float, g_ce64c2f8, 64c2f8)
KING_VAR(float, g_ce64c258, 64c258)
KING_VAR(char, g_ce67a35c, 67a35c)
KING_VAR(char, g_ce67a318, 67a318)
KING_VAR(char, g_ce67a2f0, 67a2f0)
KING_VAR(char, g_ce67a2e8, 67a2e8)

// m applied to v (rows +0 / +12 / +24, the position +36) in the float steps of Vec3_SetThis
static Vec3 Ce_Mul(const float* m, double x, double y, double z)
{
    Vec3 r;
    r.x = (float)(z * m[6] + y * m[3] + x * m[0] + m[9]);
    r.y = (float)(x * m[1] + z * m[7] + y * m[4] + m[10]);
    r.z = (float)(x * m[2] + z * m[8] + y * m[5] + m[11]);
    return r;
}

static Vec3 Ce_Half(const Vec3& v)
{
    Vec3 r;
    r.x = (float)(v.x * (double)g_ce64c258);
    r.y = (float)(v.y * (double)g_ce64c258);
    r.z = (float)(v.z * (double)g_ce64c258);
    return r;
}

extern "C" {

// [$L_54d840] car s's event ev: 4 sets +20320 and the value +5164, 9 sets +20324, 12 clears +20316 or sets +4000,
// 31 records a cab rope (two points and a width, up to four in the cab +21600 at +10496), 32 starts the effect
// +21568 when the lamp (the type's +60) is near the point the screen gives; 1 when handled
int __fastcall Car_HandleEvent(BYTE* s, void*, const int* ev)
{
    if (!ev)
        return 0;
    switch (ev[0])
    {
    case 4:
        F(int, s, 20320) = 1;
        F(float, s, 5164) = (float)(ev[1] * g_ce64b5b8);
        return 1;
    case 9:
        F(int, s, 20324) = 1;
        return 1;
    case 12:
        if (!ev[1])
            F(int, s, 20316) = 0;
        else
            F(int, s, 4000) = 1;
        return 1;
    case 31:
    {
        BYTE* car = F(BYTE*, s, 21600);
        if (!car || !Screen_5692e0(ev[1]) || ev[2] * 4 != 28)
            return 1;
        int k = F(int, car, 10704);
        if (k >= 4 || k < 0)
        {
            char buf[152];
            sprintf(buf, &g_ce67a35c, ev[0], k + 1);
            CallC(Ce_Fatal, buf);
        }
        const float* data = (const float*)ev[3];
        Vec3 a = *(const Vec3*)data, b = *(const Vec3*)(data + 3);
        Vec3 d;
        d.x = (float)((double)b.x - a.x);
        d.y = (float)((double)b.y - a.y);
        d.z = (float)((double)b.z - a.z);
        float len = (float)Vec3_Length(&d);
        float r = data[6];
        if (r < g_ce64b5a0 || r > g_ce64b668 || len > g_ce64c2f8 || len < g_ce64b5b8)
        {
            char name[100], vals[148], msg[100];
            sprintf(name, &g_ce67a318, ev[0]);
            sprintf(vals, &g_ce67a2f0, (double)a.x, (double)a.y, (double)a.z, (double)b.x, (double)b.y, (double)b.z,
                    (double)r);
            sprintf(msg, &g_ce67a2e8, name, vals);
            CallC(Ce_Fatal, msg);
        }
        car = F(BYTE*, s, 21600);
        k = F(int, car, 10704);
        F(float, car, 10688 + 4 * k) = r;
        Vec3 t;
        Ce_MulAssign(&d, 0, &t, (float)(1.0 / len));
        Matrix Rz;
        const float* R = Matrix_RotationZ(&Rz, 1.5707964f)->m;
        Vec3 e = Ce_Mul(R, d.x, d.y, d.z);
        Vec3 h = Ce_Half(e);
        BYTE* rec = car + 10496 + 48 * k;
        F(float, rec, 0) = (float)((double)a.x - h.x);
        F(float, rec, 4) = (float)((double)a.y - h.y);
        F(float, rec, 8) = (float)((double)a.z - h.z);
        Vec3 h2 = Ce_Half(e), o, sc;
        F(Vec3, rec, 12) = *Vec3_Add(&o, &a, &h2);
        F(Vec3, rec, 24) = *Vec3_Add(&o, &b, Vec3_Scaled(&sc, &e, 0.5f));
        F(Vec3, rec, 36) = *Vec3_Sub(&o, &b, Vec3_Scaled(&sc, &e, 0.5f));
        F(int, car, 10704)++;
        return 1;
    }
    case 32:
    {
        int mask = Screen_56f210(ev[1]);
        int which = (mask & 3) == 1 ? 0 : -1;
        if ((mask & 12) == 1)
            which = 1;
        if ((mask & 48) == 1)
            which = 2;
        else if (which == -1)
            return 0;
        Matrix M = g_identity;
        Vec3 P;
        P.x = P.y = P.z = 0;
        if (which >= 0)
        {
            float k = which == 0 ? g_ce64d9d8 : which == 1 ? g_ce64c938 : g_ce64c490;
            P = Ce_Mul(M.m, g_ceDir.x * (double)k, g_ceDir.y * (double)k, g_ceDir.z * (double)k);
        }
        const float* L = (const float*)(&g_ce697f9c + 2416 * F(int, s, 10420) + 60);
        Vec3 W = Ce_Mul((const float*)(s + 516), L[0], L[1], L[2]);
        Vec3 d;
        d.x = (float)((double)W.x - P.x);
        d.y = (float)((double)W.y - P.y);
        d.z = (float)((double)W.z - P.z);
        if (!(CallC<double>(Ce_LengthSq, &d) < g_ce64f768))
            return 0;
        BYTE* fx = s + 21568;
        int st = CallThis<int>(Ce_Get, fx);
        if (st == 1)
            return 0;
        st = CallThis<int>(Ce_Get, fx);
        if (st == 2)
            return 0;
        st = CallThis<int>(Ce_Get, fx);
        if (st == 3)
            return 0;
        CallThis(Ce_Start, fx, 0);
        return 0;
    }
    }
    return 0;
}

} // extern "C"

// === the player's frame (a method at $L_536360)

struct PvPos { DWORD d[8]; };                           // an AI position, passed by value
struct PvRoom
{
    DWORD flags, m_4;
    double width;
    const char* module;
    const char* room;
    DWORD m_24, m_28, m_32;
    int m_36, m_40;
    DWORD m_44;
};
struct PvRect { int l, t, r, b; };
struct PvMat { Vec3 r0, r1, r2, t; };

extern "C" void Pv_InList(); KING_ALIAS("_Pv_InList", "_Car_InList")
extern "C" void Pv_PostEvent(); KING_ALIAS("_Pv_PostEvent", "@Queue_PostEvent@12")
extern "C" void Pv_Action(); KING_ALIAS("_Pv_Action", "@Controls_ActionState@12")
extern "C" void Pv_SetCursor(); KING_ALIAS("_Pv_SetCursor", "_Input_SetCursorPos")
extern "C" void Pv_ObjReset(); KING_ALIAS("_Pv_ObjReset", "@WorldObj_Reset@8")
extern "C" void Pv_PlayVolume(); KING_ALIAS("_Pv_PlayVolume", "_Sound_PlayVolume")
extern "C" void Pv_MousePos(); KING_ALIAS("_Pv_MousePos", "@CGameApp_SetMousePos@16")
extern "C" void Pv_Chase(); KING_ALIAS("_Pv_Chase", "@Cam_ChaseStep@20")
extern "C" void Pv_ApplyChecked(); KING_ALIAS("_Pv_ApplyChecked", "_Frame_ApplyChecked")
extern "C" void Pv_Car556650(); KING_ALIAS("_Pv_Car556650", "@Car_556650@8")
extern "C" void Pv_ViewMatrix(); KING_ALIAS("_Pv_ViewMatrix", "@Car_ViewMatrix@12")
extern "C" void Pv_M324(); KING_ALIAS("_Pv_M324", "@SceneObj_Matrix324@12")
extern "C" void Pv_M1(); KING_ALIAS("_Pv_M1", "@SceneObj_Matrix1@12")
extern "C" void Pv_M2(); KING_ALIAS("_Pv_M2", "@SceneObj_Matrix2@12")
extern "C" void Pv_Get800(); KING_ALIAS("_Pv_Get800", "@SceneObj_Get800@8")
extern "C" void __cdecl Pv_ToAngles(PvMat m, double* a, double* b, double* c, double* d, double* e, double* f);
KING_ALIAS("_Pv_ToAngles", "_Matrix_ToAngles")
extern "C" void Pv_FromPose(); KING_ALIAS("_Pv_FromPose", "_Matrix_FromPose")
extern "C" void Pv_Filter(); KING_ALIAS("_Pv_Filter", "@Filter_Value@4")
extern "C" void Pv_AddAssign(); KING_ALIAS("_Pv_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Pv_MulAssign(); KING_ALIAS("_Pv_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Pv_HitObject(); KING_ALIAS("_Pv_HitObject", "_Query_HitObject")
extern "C" void Pv_Sway(); KING_ALIAS("_Pv_Sway", "@Cab_ViewSway@8")
extern "C" void Pv_SetCamera(); KING_ALIAS("_Pv_SetCamera", "@View_SetCamera@16")
extern "C" int __stdcall Pv_Light(BYTE* node, Vec3 p, void* fn, int a); KING_ALIAS("_Pv_Light@24", "_Light_Set@24")
extern "C" void Pv_PositionId(); KING_ALIAS("_Pv_PositionId", "_Ai_GetPositionId")
extern "C" void __cdecl Pv_RoomInfo(PvPos pos, PvRoom* out); KING_ALIAS("_Pv_RoomInfo", "_Road_GetRoadParameters")
extern "C" float* __cdecl Pv_Coords(float* out, PvPos pos); KING_ALIAS("_Pv_Coords", "_Ai_GetCoordinatesPos")
extern "C" void Pv_Gravity(); KING_ALIAS("_Pv_Gravity", "@Cab_FeltGravity@4")
extern "C" void Pv_Dash(); KING_ALIAS("_Pv_Dash", "@Dash_Prepare@16")
extern "C" void Pv_CabView(); KING_ALIAS("_Pv_CabView", "@CabView_Update@4")
extern "C" void Pv_Sparks(); KING_ALIAS("_Pv_Sparks", "_Car_SparkBurst")
extern "C" void Pv_Sounds(); KING_ALIAS("_Pv_Sounds", "@Vehicle_Sounds@16")
extern "C" void Pv_Log(); KING_ALIAS("_Pv_Log", "_VehLog_Value")
extern "C" void Pv_Flush(); KING_ALIAS("_Pv_Flush", "_WarnLog_Flush")
extern "C" void Pv_533590(); KING_ALIAS("_Pv_533590", "@Car_PlaceOnRoad@12")
extern "C" void Pv_535a70(); KING_ALIAS("_Pv_535a70", "@Cab_Sway@4")
extern "C" void Pv_SetText(); KING_ALIAS("_Pv_SetText", "@History_Record@16")                        // a dashboard text line (value, name)
extern "C" void Pv_4de990(); KING_ALIAS("_Pv_4de990", "_Cab_EyeFrame")
extern "C" void __cdecl Game_TruckMessage(int* m);
KING_VAR(BYTE, g_pvControls, 6d1db8)
KING_VAR(BYTE*, g_pv6cec6c, 6cec6c)
KING_VAR(BYTE*, g_pv6cec68, 6cec68)
KING_VAR(BYTE*, g_pv6cec74, 6cec74)
KING_VAR(int, g_pv696850, 696850)                     // the screen size
KING_VAR(int, g_pv696854, 696854)
KING_VAR(float, g_pv696d10, 696d10)
KING_VAR(int, g_pv696d14, 696d14)
KING_VAR(BYTE, g_pv6f6968, 6f6968)
KING_VAR(void*, g_pv6d1cfc, 6d1cfc)
KING_VAR(BYTE*, g_pv696cb0, 696cb0)
KING_VAR(int, g_pv672d7c, 672d7c)
KING_VAR(BYTE*, g_pv6d2078, 6d2078)
KING_VAR(BYTE*, g_pv696cb8, 696cb8)
KING_VAR(char, g_pv696cbc, 696cbc)
KING_VAR(Vec3, g_pv696740, 696740)
KING_VAR(Matrix, g_pv6969d0, 6969d0)
KING_VAR(Matrix, g_pv696b98, 696b98)
KING_VAR(Matrix, g_pv6969a0, 6969a0)
KING_VAR(int, g_pv68b990, 68b990)
KING_VAR(Vec3, g_pvAhead, 702620)
KING_VAR(Vec3, g_pvUp, 702630)
KING_VAR(Vec3, g_pv702640, 702640)
KING_VAR(Vec3, g_pvEye, 702650)
KING_VAR(float, g_pv6d1830, 6d1830)                   // a 3x3 rotation
KING_VAR(int, g_pv696c70, 696c70)                     // the logging switches
KING_VAR(int, g_pv696c74, 696c74)
KING_VAR(int, g_pv672d80, 672d80)
KING_VAR(DWORD, g_pv696c90, 696c90)
KING_VAR(DWORD, g_pv696c94, 696c94)
KING_VAR(int, g_pv6f661c, 6f661c)
KING_VAR(DWORD, g_pv6f65e0, 6f65e0)
KING_VAR(DWORD, g_pv6f65e4, 6f65e4)
KING_VAR(int, g_pv6ef2c0, 6ef2c0)
KING_VAR(int, g_pv6ef218, 6ef218)
KING_VAR(double, g_pv6ef2b8, 6ef2b8)
KING_VAR(double, g_pv6ef210, 6ef210)
KING_VAR(DWORD, g_pv6f65b8, 6f65b8)
KING_VAR(DWORD, g_pv6f65bc, 6f65bc)
KING_VAR(DWORD, g_pv6ef230, 6ef230)
KING_VAR(DWORD, g_pv6ef234, 6ef234)
KING_VAR(int, g_pv6ef238, 6ef238)
KING_VAR(DWORD, g_pv6ef240, 6ef240)
KING_VAR(DWORD, g_pv6ef244, 6ef244)
KING_VAR(int, g_pv6ef248, 6ef248)
KING_VAR(DWORD, g_pv697658, 697658)
KING_VAR(DWORD, g_pv69765c, 69765c)
KING_VAR(int, g_pv697660, 697660)
KING_VAR(char, g_pv66d638, 66d638)
KING_VAR(char, g_pv66d628, 66d628)
KING_VAR(double, g_pv64f3d8, 64f3d8)
KING_VAR(double, g_pv64b5a0, 64b5a0)
KING_VAR(float, g_pv64c968, 64c968)
KING_VAR(double, g_pv64d378, 64d378)
KING_VAR(float, g_pv64f3cc, 64f3cc)
KING_VAR(float, g_pv64f3c8, 64f3c8)
KING_VAR(double, g_pv64b8f8, 64b8f8)
KING_VAR(float, g_pv64c250, 64c250)
KING_VAR(double, g_pv64b668, 64b668)
KING_VAR(double, g_pv64d318, 64d318)
KING_VAR(double, g_pv64b6e0, 64b6e0)
KING_VAR(double, g_pv64d0f0, 64d0f0)
KING_VAR(float, g_pv678784, 678784)
KING_VAR(float, g_pv64d990, 64d990)
KING_VAR(double, g_pv64f3c0, 64f3c0)
KING_VAR(double, g_pv64b730, 64b730)
KING_VAR(float, g_pv64c41c, 64c41c)
KING_VAR(float, g_pv64f3bc, 64f3bc)
KING_VAR(char, g_pv678f40, 678f40)
KING_VAR(char, g_pv678f28, 678f28)
KING_VAR(char, g_pv678f10, 678f10)
KING_VAR(char, g_pv678ef8, 678ef8)
KING_VAR(char, g_pv678eec, 678eec)
KING_VAR(char, g_pv678ee0, 678ee0)
KING_VAR(char, g_pv678ed4, 678ed4)
KING_VAR(char, g_pv678ec8, 678ec8)
KING_VAR(char, g_pv678ea4, 678ea4)
KING_VAR(char, g_pv678e9c, 678e9c)
KING_VAR(char, g_pv678e5c, 678e5c)
KING_VAR(char, g_pv678e50, 678e50)
KING_VAR(char, g_pv678e3c, 678e3c)
KING_VAR(char, g_pv678e2c, 678e2c)
KING_VAR(char, g_pv678e14, 678e14)
KING_VAR(char, g_pv678e08, 678e08)
KING_VAR(char, g_pv678df4, 678df4)
KING_VAR(char, g_pv678de4, 678de4)
KING_VAR(char, g_pv678dd4, 678dd4)
KING_VAR(char, g_pv678db4, 678db4)
KING_VAR(char, g_pv678da0, 678da0)
KING_VAR(char, g_pv678d80, 678d80)
KING_VAR(char, g_pv678d68, 678d68)
KING_VAR(char, g_pv678d50, 678d50)
KING_VAR(char, g_pv678d20, 678d20)
KING_VAR(char, g_pv678cfc, 678cfc)
KING_VAR(char, g_pv678cec, 678cec)
KING_VAR(char, g_pv678cd0, 678cd0)
KING_VAR(char, g_pv678cb0, 678cb0)
KING_VAR(char, g_pv678c9c, 678c9c)
KING_VAR(char, g_pv678c84, 678c84)
KING_VAR(char, g_pv678c74, 678c74)
KING_VAR(char, g_pv678c5c, 678c5c)
KING_VAR(char, g_pv678c44, 678c44)
KING_VAR(char, g_pv678c28, 678c28)
KING_VAR(char, g_pv678c0c, 678c0c)
KING_VAR(char, g_pv678bec, 678bec)
KING_VAR(char, g_pv678bcc, 678bcc)
KING_VAR(char, g_pv678bac, 678bac)
KING_VAR(char, g_pv678b90, 678b90)

static int Pv_Act(int i)
{
    return CallThis<int>(Pv_Action, &g_pvControls, i);
}

static void Pv_Centre()
{
    CallC(Pv_SetCursor, g_pv696850 / 2, g_pv696854 / 2);
}

static void Pv_Out(const char* t)
{
    CallC(Pv_Log, t);
}

static const Matrix* Pv_Mat1(BYTE* car, Matrix* out)
{
    return CallThis<const Matrix*>(Pv_M1, car + 16, out);
}

static PvMat Pv_AsMat(const Matrix& m)
{
    return *(const PvMat*)&m;
}

extern "C" {

// [$L_536360] player s's frame: the car (+616) checked, the view zoom (+1396 by actions 26 / 27) and the free camera
// (+1400, action 20) handled, the cab's dials and the time-out of a stopped cab; then the camera: chasing in the
// free mode (with the view's frame and size), else the car's view with its sway and turning look, the light at
// the camera, the AI position of the car recorded (+204 / +216 / +228) when near its road; then the cab's
// gravity, the dashboard, the sparks, the sounds, and the debug logs
void __fastcall Player_Frame(BYTE* s, void*)
{
    BYTE* car = CallC<BYTE*>(Pv_InList, F(BYTE*, s, 616));
    F(BYTE*, s, 616) = car;
    if (!car)
    {
        CallThis(Pv_PostEvent, s, 1);
        return;
    }
    float ft = (float)F(double, F(BYTE*, s, 32), 2960);
    int lag = Game_FrameTime() < g_pv64f3d8 ? 1 : 0;
    if (!F(int, s, 1400))
    {
        // the zoom steps
        if (!F(int, s, 1396))
            F(int, s, 108) = F(int, s, 16);
        if ((Pv_Act(26) || Pv_Act(27)) && F(int, g_pv6cec6c, 1300) <= 1)
            Pv_Centre();
        if (Pv_Act(26))
        {
            int v = F(int, s, 1396);
            if (v < 0)
                F(int, s, 1396) = 0;
            else if (!v || F(int, s, 16) - F(int, s, 108) > lag)
            {
                F(int, s, 108) = F(int, s, 16);
                F(int, s, 1396) = v + 1;
            }
        }
        else if (Pv_Act(27))
        {
            int v = F(int, s, 1396);
            if (v > 0)
                F(int, s, 1396) = 0;
            else if (!v || F(int, s, 16) - F(int, s, 108) > lag)
            {
                F(int, s, 108) = F(int, s, 16);
                F(int, s, 1396) = v - 1;
            }
        }
        else
        {
            int v = F(int, s, 1396);
            F(int, s, 1396) = v == 2 ? 1 : v != -2 ? 0 : -1;
        }
        if (F(int, s, 1396) > 2)
            F(int, s, 1396) = 2;
        if (F(int, s, 1396) < -2)
            F(int, s, 1396) = -2;
    }
    else
    {
        g_pv696d10 = 0;
        g_pv696d14 = 0;
        F(int, s, 1396) = 0;
    }
    // the free camera's switch
    if ((F(BYTE, s, 1404) & 2) && Pv_Act(20) == 1)
    {
        if (!F(int, s, 1400))
        {
            F(int, s, 1400) = 1;
            if (F(int, g_pv6cec6c, 1300) <= 1)
                Pv_Centre();
            CallThis(Pv_ObjReset, &g_pv6f6968);
        }
        else
        {
            F(int, s, 1400) = 0;
            g_pv696d14 = 1;
            if (F(int, g_pv6cec6c, 1300) <= 1)
                Pv_Centre();
        }
        F(int, g_pv6cec6c, 1352) = F(int, s, 1400);
    }
    // the cab's dials and its time-out
    car = F(BYTE*, s, 616);
    if (car && F(BYTE*, car, 21600))
    {
        BYTE* c = F(BYTE*, car, 21600);
        F(float, c, 10020) = (float)(F(short, g_pv6cec74, 388) * g_pv64b5a0);
        c = F(BYTE*, F(BYTE*, s, 616), 21600);
        F(float, c, 10024) = (float)(F(short, g_pv6cec74, 390) * g_pv64b5a0);
        F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 10084) = 0.3f;
        c = F(BYTE*, F(BYTE*, s, 616), 21600);
        if (!F(int, c, 13124) && F(int, s, 1400) == 2)
        {
            F(int, s, 1400) = 0;
            F(int, g_pv6cec6c, 1352) = 0;
        }
        c = F(BYTE*, F(BYTE*, s, 616), 21600);
        if (F(int, c, 13124))
        {
            if (Game_Time() < F(float, s, 44))
            {
                F(float, s, 44) = (float)Game_Time();
                if (F(BYTE, s, 1404) & 16)
                    CallC(Pv_PlayVolume, g_pv6d1cfc, 0.3f);
            }
            int left = 1 - (int)((Game_Time() - F(float, s, 44)) * g_pv64c968);
            F(int, s, 1392) = left;
            if (left > 100000 || left < 0)
            {
                F(int, s, 1392) = 0;
                CallThis(Pv_533590, s, 1);
                BYTE* app = F(BYTE*, s, 36);
                CallThis(Pv_MousePos, app, F(int, app, 256) / 2, F(int, app, 260) / 2);
            }
        }
        else
        {
            F(float, s, 44) = 1000000.0f;
            F(int, s, 1392) = 0;
        }
    }
    if (F(BYTE*, s, 616))
    {
        g_pv696d10 = (float)Camera_ViewAngle();
        car = F(BYTE*, s, 616);
        Vec3 vel = F(Vec3, car, 3316);
        Matrix* cam = (Matrix*)(s + 1424);
        BYTE* node;
        Matrix A, B, C;
        if (F(int, s, 1400))
        {
            // the free camera
            Vec3 old = F(Vec3, s, 1460);
            node = CallThis<BYTE*>(Pv_Chase, &g_pv6f6968, cam, 0.85);
            double dx = (double)F(float, s, 1460) - old.x;
            float dy = (float)((double)F(float, s, 1464) - old.y);
            float dz = (float)((double)F(float, s, 1468) - old.z);
            vel.x = (float)(dx / ft);
            vel.y = (float)(dy / (double)ft);
            vel.z = (float)(dz / (double)ft);
            car = F(BYTE*, s, 616);
            CallVirt(car, 100);
            memcpy(s + 1520, &g_identity, 48);
            F(int, s, 1616) = 0;
            F(Vec3, s, 1620) = g_pvEye;
            memcpy(s + 1568, &g_identity, 48);
            BYTE* app = F(BYTE*, s, 36);
            int w = F(int, app, 256), h = F(int, app, 260);
            PvRect rc = {0, g_pv672d7c, w, h};
            CallVirt(F(BYTE*, s, 32), 44, rc);
            BYTE* X = F(BYTE*, g_pv696cb0, 4860);
            if (X)
            {
                double a = h * g_pv64d378 / w;
                X[40] &= ~1;
                float af = (float)a;
                float sc = w < 640 ? (float)(w * (double)g_pv64f3cc) : 1.0f;
                double ux = g_pvUp.x * (double)af;
                float uy = (float)(g_pvUp.y * (double)af), uz = (float)(g_pvUp.z * (double)af);
                float ny = -g_pv702640.y, nz = -g_pv702640.z;
                double tx = -(double)g_pv702640.x - ux;
                float ty = (float)((double)ny - uy), tz = (float)((double)nz - uz);
                Vec3 T;
                T.x = (float)(tx * g_pv64f3c8);
                T.y = (float)(ty * (double)g_pv64f3c8);
                T.z = (float)(tz * (double)g_pv64f3c8);
                if (sc < g_pv64b8f8 || sc < g_pv64c250)
                {
                    if (sc < g_pv64b8f8)
                        *(DWORD*)&sc = 1059648963;
                    Vec3 t;
                    CallThis(Pv_MulAssign, &T, &t, sc);
                    T.y = (float)((sc - (double)g_pv64c250) * g_pv64b668);
                }
                Matrix_Multiply(&B, Matrix_Translation(&A, T), cam);
                CallC(Pv_ApplyChecked, F(BYTE*, g_pv696cb0, 4872), 0, &B);
            }
            CallThis(Pv_SetText, s + 376, 0.0f, &g_pv678f40);
            CallThis(Pv_SetText, s + 408, 0.0f, &g_pv678f28);
        }
        else
        {
            // the car's view
            CallVirt(car, 104);
            CallThis<float>(Pv_Car556650, F(BYTE*, s, 616));
            CallThis(Pv_535a70, s);
            memcpy(s + 1568, CallC<const Matrix*>(Pv_4de990, &A, 0), 48);
            CallThis(Pv_ViewMatrix, s, &B);
            Matrix V;
            CallThis(Pv_M324, F(BYTE*, s, 616) + 16, &V);
            *cam = *Matrix_Multiply(&A, &B, (const Matrix*)(s + 520));
            float cosA = (float)x87_fabs(x87_cos(g_pv696d10));
            if (F(DWORD, g_pv6d2078, 1404) & 0x100)
            {
                // the turning look
                double p[6];
                Pv_ToAngles(Pv_AsMat(V), &p[0], &p[1], &p[2], &p[3], &p[4], &p[5]);
                CallThis(Pv_SetText, s + 376, (float)(p[5] * g_pv64d318), &g_pv678f10);
                CallThis(Pv_SetText, s + 408, (float)(p[4] * g_pv64d318), &g_pv678ef8);
                float b = (float)(p[4] - CallThis<float>(Pv_Filter, s + 408) * (double)cosA);
                double c = ((double)cosA + g_pv64c250) * p[5] * g_pv64b6e0;
                double c2 = c - CallThis<float>(Pv_Filter, s + 376) * (double)cosA * g_pv64b6e0;
                memcpy(&V, CallC<const Matrix*>(Pv_FromPose, &C, p[0], p[1], p[2], p[3], (double)b, c2), 48);
            }
            // the car's spin added to its speed
            const float* Q = (const float*)(s + 1460);
            const float* W = (const float*)(F(BYTE*, s, 616) + 3328);
            double v0 = (double)Q[2] * W[1] - (double)W[2] * Q[1];
            double v1 = (double)W[2] * Q[0] - (double)Q[2] * W[0];
            double v2 = (double)W[0] * Q[1] - (double)Q[0] * W[1];
            Vec3 r, t;
            r.x = (float)(v2 * V.m[6] + v1 * V.m[3] + V.m[0] * v0);
            r.y = (float)(v2 * V.m[7] + v1 * V.m[4] + v0 * V.m[1]);
            r.z = (float)(v2 * V.m[8] + v1 * V.m[5] + v0 * V.m[2]);
            CallThis(Pv_AddAssign, &vel, &t, &r);
            *cam = *Matrix_Multiply(&C, cam, &V);
            car = F(BYTE*, s, 616);
            node = CallC<BYTE*>(Pv_HitObject, CallThis<BYTE*>(Pv_Get800, car + 16), &Pv_Mat1(car, &C)->m[9], s + 1460);
            if (!node)
                node = CallThis<BYTE*>(Pv_Get800, F(BYTE*, s, 616) + 16);
            BYTE* X = F(BYTE*, g_pv696cb0, 4860);
            if (X)
                X[40] |= 1;
            CallThis(Pv_Sway, g_pv696cb0);
        }
        CallThis(Pv_SetCamera, F(BYTE*, s, 32), node, cam);
        memcpy(s + 1472, Matrix_Inverse(&A, cam), 48);
        BYTE* was = g_pv696cb8;
        g_pv696b98 = g_pv6969d0;
        g_pv6969d0 = *cam;
        g_pv6969a0 = *(Matrix*)(s + 1472);
        if (was != node)
            sprintf(&g_pv696cbc, F(char*, node, 12));
        g_pv696cb8 = node;
        g_pv696740 = vel;
        // the light at the camera
        const float* M = (const float*)(s + 520);
        double ux = -g_pvUp.x, uy = -g_pvUp.y, uz = -g_pvUp.z;
        Vec3 L;
        L.x = (float)(uz * M[6] + uy * M[3] + ux * M[0] + M[9]);
        L.y = (float)(ux * M[1] + uz * M[7] + uy * M[4] + M[10]);
        L.z = (float)(ux * M[2] + uz * M[8] + uy * M[5] + M[11]);
        car = F(BYTE*, s, 616);
        const float* Wm = CallThis<const Matrix*>(Pv_M324, car + 16, &A)->m;
        Vec3 Lw;
        Lw.x = (float)((double)L.z * Wm[6] + (double)L.y * Wm[3] + (double)L.x * Wm[0] + Wm[9]);
        Lw.y = (float)((double)L.x * Wm[1] + (double)L.z * Wm[7] + (double)L.y * Wm[4] + Wm[10]);
        Lw.z = (float)((double)L.x * Wm[2] + (double)L.z * Wm[8] + (double)L.y * Wm[5] + Wm[11]);
        CallC(Pv_HitObject, CallThis<BYTE*>(Pv_Get800, car + 16), &Pv_Mat1(car, &A)->m[9], &Lw);
        Pv_Light(CallThis<BYTE*>(Pv_Get800, F(BYTE*, s, 616) + 16), Lw, (void*)Game_TruckMessage, 0);
        Pv_Light(F(BYTE*, F(BYTE*, s, 32), 528), Lw, (void*)Game_TruckMessage, 0);
        // the car's AI position, when near its road
        car = F(BYTE*, s, 616);
        BYTE* cab = F(BYTE*, car, 21600);
        if (cab && !F(int, cab, 13124) && F(int, car, 20636) != g_pv68b990)
        {
            PvPos pid;
            CallC(Pv_PositionId, &pid, F(int, car, 20636));
            PvRoom info;
            info.flags = 0;
            info.width = 0;
            info.module = &g_pv66d638;
            info.room = &g_pv66d628;
            info.m_24 = 0;
            info.m_28 = 0;
            info.m_36 = -1;
            info.m_40 = -1;
            if (pid.d[0] != (DWORD)-1)
            {
                Pv_RoomInfo(pid, &info);
                float pos[3];
                Pv_Coords(pos, pid);
                const float* m1 = Pv_Mat1(F(BYTE*, s, 616), &A)->m;
                Vec3 d;
                d.x = (float)((double)pos[0] - m1[9]);
                d.y = (float)((double)pos[1] - m1[10]);
                d.z = (float)((double)pos[2] - m1[11]);
                double len = Vec3_Length(&d);
                car = F(BYTE*, s, 616);
                if (F(float, F(BYTE*, car, 21600), 13100) * g_pv64d0f0 > len - info.width)
                {
                    m1 = Pv_Mat1(car, &A)->m;
                    const Vec3& a = g_pvAhead;
                    F(float, s, 216) = (float)((double)a.z * m1[6] + (double)a.y * m1[3] + (double)a.x * m1[0]);
                    F(float, s, 220) = (float)((double)a.x * m1[1] + (double)a.z * m1[7] + (double)a.y * m1[4]);
                    F(float, s, 224) = (float)((double)a.x * m1[2] + (double)a.z * m1[8] + (double)a.y * m1[5]);
                    F(Vec3, s, 204) = *(const Vec3*)&Pv_Mat1(F(BYTE*, s, 616), &A)->m[9];
                    strcpy((char*)(s + 228), F(char*, CallThis<BYTE*>(Pv_Get800, F(BYTE*, s, 616) + 16), 12));
                }
            }
        }
    }
    // the cab, the dashboard, the sparks and the sounds
    CallThis(Pv_Gravity, s);
    int snd = 0;
    if (F(BYTE*, s, 616))
    {
        if (g_pv6cec68)
            CallThis(Pv_Dash, s, 0, 0);
        else
            CallThis(Pv_CabView, s);
        if (F(BYTE*, s, 616) && !(Game_Time() < F(float, s, 468)))
        {
            float until = (float)((double)g_pv678784 + F(float, s, 468));
            if (!(Game_Time() > until))
                CallC(Pv_Sparks, F(BYTE*, s, 616), s + 456);
        }
    }
    BYTE sndBuf[244];
    if (F(BYTE, s, 1404) & 16)
        CallThis(Pv_Sounds, s, sndBuf, &snd);
    // the debug logs
    car = F(BYTE*, s, 616);
    Matrix M1;
    double ang[6];
    Pv_ToAngles(Pv_AsMat(*Pv_Mat1(car, &M1)), &ang[0], &ang[1], &ang[2], &ang[3], &ang[4], &ang[5]);
    float k0 = (float)(g_pvUp.x * (double)g_pv64d990);
    float k1 = (float)(g_pvUp.y * (double)g_pv64d990);
    float k2 = (float)(g_pvUp.z * (double)g_pv64d990);
    Vec3 V;
    V.x = k0;
    V.y = k1;
    V.z = k2;
    car = F(BYTE*, s, 616);
    if (!F(int, s, 1400) && car && F(BYTE*, car, 21600))
    {
        Vec3 a = F(Vec3, car, 468), b = F(Vec3, car, 432);
        double dx = (double)b.x - a.x;
        float dy = (float)((double)b.y - a.y), dz = (float)((double)b.z - a.z);
        float vy = (float)(dy / (double)ft), vz = (float)(dz / (double)ft);
        Vec3 p;
        p.x = (float)(dx / ft + k0);
        p.y = (float)((double)k1 + vy);
        p.z = (float)((double)k2 + vz);
        const float* m2 = CallThis<const Matrix*>(Pv_M2, car + 16, &M1)->m;
        V.x = (float)((double)p.z * m2[6] + (double)p.y * m2[3] + (double)p.x * m2[0]);
        V.y = (float)((double)p.x * m2[1] + (double)p.z * m2[7] + (double)p.y * m2[4]);
        V.z = (float)((double)p.x * m2[2] + (double)p.z * m2[8] + (double)p.y * m2[5]);
        car = F(BYTE*, s, 616);
        BYTE* c = F(BYTE*, car, 21600);
        if (!F(int, c, 13124) && !F(int, car, 3480))
        {
            // relative to the cab's mounting
            const float* M5 = (const float*)(s + 520);
            const Vec3& E = g_pvEye;
            double ex = (double)E.z * M5[6] + (double)E.y * M5[3] + (double)E.x * M5[0] + M5[9];
            double ey = (double)E.x * M5[1] + (double)E.z * M5[7] + (double)E.y * M5[4] + M5[10];
            double ez = (double)E.x * M5[2] + (double)E.z * M5[8] + (double)E.y * M5[5] + M5[11];
            Vec3 k = F(Vec3, c, 10868);
            const float* N = (const float*)(car + 708);
            float fx = (float)(N[0] * ex + N[6] * ez + N[3] * ey + N[9]);
            float fy = (float)(N[7] * ez + N[4] * ey + ex * N[1] + N[10]);
            double fz = N[8] * ez + N[5] * ey + ex * N[2] + N[11];
            float gx = (float)((double)k.y * fz - (double)k.z * fy);
            double gy = (double)fx * k.z - (double)k.x * fz;
            double gz = (double)k.x * fy - (double)fx * k.y;
            float hx = (float)(gy * k.z - gz * k.y);
            float hy = (float)((double)k.x * gz - (double)gx * k.z);
            double hz = (double)gx * k.y - (double)k.x * gy;
            float ny = -F(float, c, 13232), nz = -F(float, c, 13236);
            V.x = (float)(-(double)F(float, c, 13228) - hx);
            V.y = (float)((double)ny - hy);
            V.z = (float)((double)nz - hz);
        }
    }
    double a3 = ang[3] * g_pv64f3c0, a4 = ang[4] * g_pv64f3c0, a5 = ang[5] * g_pv64f3c0;
    ang[3] = a3;
    ang[4] = a4;
    ang[5] = a5;
    double pose[6];
    pose[3] = a5;
    pose[4] = -a4;
    pose[5] = a3;
    const float* R = &g_pv6d1830;
    pose[0] = (double)V.z * R[6] + (double)V.y * R[3] + (double)R[0] * V.x;
    float ry = (float)((double)V.z * R[7] + (double)V.y * R[4] + (double)V.x * R[1]);
    float rz = (float)((double)V.z * R[8] + (double)V.y * R[5] + (double)V.x * R[2]);
    pose[1] = ry;
    pose[2] = rz;
    BYTE* g = F(BYTE*, s, 32);
    char buf[256];
    if (g_pv696c70 || g_pv696c74)
    {
        sprintf(buf, &g_pv678eec, F(double, g, 2968));
        Pv_Out(buf);
        g = F(BYTE*, s, 32);
        if (F(BYTE, g, 2992) & 1)
        {
            sprintf(buf, &g_pv678ee0, 0.0);
            Pv_Out(buf);
            sprintf(buf, &g_pv678ed4, F(double, F(BYTE*, s, 32), 2960));
        }
        else
        {
            sprintf(buf, &g_pv678ee0, F(double, g, 2960));
            Pv_Out(buf);
            sprintf(buf, &g_pv678ed4, 0.0);
        }
        Pv_Out(buf);
    }
    if (g_pv672d80)
    {
        g = F(BYTE*, s, 32);
        sprintf(buf, &g_pv678eec, F(double, g, 2968));
        Pv_Out(buf);
        sprintf(buf, &g_pv678ec8, g_pv696c90, g_pv696c94);
        Pv_Out(buf);
        g = F(BYTE*, s, 32);
        if (F(BYTE, g, 2992) & 1)
        {
            sprintf(buf, &g_pv678ee0, 0.0);
            Pv_Out(buf);
            sprintf(buf, &g_pv678ed4, F(double, F(BYTE*, s, 32), 2960) * g_pv64b730);
        }
        else
        {
            sprintf(buf, &g_pv678ee0, F(double, g, 2960) * g_pv64b730);
            Pv_Out(buf);
            sprintf(buf, &g_pv678ed4, 0.0);
        }
        Pv_Out(buf);
    }
    bool more = false;
    if (g_pv696c70)
    {
        car = F(BYTE*, s, 616);
        strcpy(buf, F(int, car, 3480) ? &g_pv678ea4 : &g_pv678e9c);
        Pv_Out(buf);
        BYTE* c = F(BYTE*, F(BYTE*, s, 616), 21600);
        DWORD any = 0;
        for (int i = 0; i < F(int, c, 13120); i++)
            any |= F(DWORD, c, 15336 + 4 * i);
        if (any)
            sprintf(buf, &g_pv678e5c, F(int, c, 15336), F(int, c, 15340), F(int, c, 15344), F(int, c, 15348),
                    F(int, c, 15352), F(int, c, 15356), F(int, c, 15360), F(int, c, 15364));
        else
            strcpy(buf, &g_pv678e50);
        Pv_Out(buf);
        sprintf(buf, &g_pv678e3c, (double)F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 10012));
        Pv_Out(buf);
        more = g_pv696c70 != 0;
    }
    if (more || g_pv696c74)
    {
        car = F(BYTE*, s, 616);
        const float* m2 = CallThis<const Matrix*>(Pv_M2, car + 16, &M1)->m;
        const float* v = (const float*)(F(BYTE*, s, 616) + 3316);
        sprintf(buf, &g_pv678e2c, (double)v[2] * m2[7] + (double)v[1] * m2[4] + (double)m2[1] * v[0]);
        Pv_Out(buf);
        BYTE* c = F(BYTE*, F(BYTE*, s, 616), 21600);
        sprintf(buf, &g_pv678e14, F(int, c, 13380), F(int, c, 13384), F(int, c, 13388), F(int, c, 13392));
        Pv_Out(buf);
        if (g_pv696c70)
        {
            car = F(BYTE*, s, 616);
            m2 = CallThis<const Matrix*>(Pv_M2, car + 16, &M1)->m;
            car = F(BYTE*, s, 616);
            const float* u = (const float*)(car + 3316);
            float lx = (float)((double)m2[3] * u[1] + (double)m2[6] * u[2] + (double)m2[0] * u[0]);
            float ly = (float)((double)m2[1] * u[0] + (double)m2[4] * u[1] + (double)m2[7] * u[2]);
            float lz = (float)((double)m2[2] * u[0] + (double)m2[5] * u[1] + (double)m2[8] * u[2]);
            Vec3 w = F(Vec3, car, 444);
            double wx = w.x * (double)g_pv64c41c;
            float wy = (float)(w.y * (double)g_pv64c41c), wz = (float)(w.z * (double)g_pv64c41c);
            float sx = (float)(wx * g_pv64f3bc), sy = (float)(wy * (double)g_pv64f3bc),
                  sz = (float)(wz * (double)g_pv64f3bc);
            c = F(BYTE*, car, 21600);
            sprintf(buf, &g_pv678e08, F(int, c, 15320));
            Pv_Out(buf);
            sprintf(buf, &g_pv678df4, (double)F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 15416));
            Pv_Out(buf);
            sprintf(buf, &g_pv678de4, (double)F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 15312));
            Pv_Out(buf);
            sprintf(buf, &g_pv678dd4, (double)F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 15420));
            Pv_Out(buf);
            c = F(BYTE*, F(BYTE*, s, 616), 21600);
            sprintf(buf, &g_pv678db4, (double)F(float, c, 13128), (double)F(float, c, 13132),
                    (double)F(float, c, 13136), (double)F(float, c, 13140));
            Pv_Out(buf);
            c = F(BYTE*, F(BYTE*, s, 616), 21600);
            sprintf(buf, &g_pv678da0, F(int, c, 13420), F(int, c, 13424), F(int, c, 13428), F(int, c, 13432));
            Pv_Out(buf);
            c = F(BYTE*, F(BYTE*, s, 616), 21600);
            sprintf(buf, &g_pv678d80, (double)F(float, c, 13228), (double)F(float, c, 13232),
                    (double)F(float, c, 13236));
            Pv_Out(buf);
            sprintf(buf, &g_pv678d68, (double)lx, (double)ly, (double)lz);
            Pv_Out(buf);
            sprintf(buf, &g_pv678d50, (double)sx, (double)sy, (double)sz);
            Pv_Out(buf);
            sprintf(buf, &g_pv678d20, pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);
            Pv_Out(buf);
            Pv_Out(F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 13124) ? &g_pv678cfc : &g_pv678cec);
            Matrix a, b, d;
            double z = Pv_Mat1(F(BYTE*, s, 616), &a)->m[11];
            double y = Pv_Mat1(F(BYTE*, s, 616), &b)->m[10];
            double x = Pv_Mat1(F(BYTE*, s, 616), &d)->m[9];
            sprintf(buf, &g_pv678cd0, x, y, z);
            Pv_Out(buf);
            sprintf(buf, &g_pv678cb0, ang[3], ang[4], ang[5]);
            Pv_Out(buf);
        }
    }
    if (g_pv672d80)
    {
        sprintf(buf, &g_pv678c9c, g_pv6f661c);
        Pv_Out(buf);
        sprintf(buf, &g_pv678c84, g_pv6f65e0, g_pv6f65e4);
        Pv_Out(buf);
        sprintf(buf, &g_pv678c74, g_pv6ef2c0 + g_pv6ef218);
        Pv_Out(buf);
        sprintf(buf, &g_pv678c5c, g_pv6ef2b8 + g_pv6ef210);
        Pv_Out(buf);
        sprintf(buf, &g_pv678c44, g_pv6f65b8, g_pv6f65bc);
        Pv_Out(buf);
        sprintf(buf, &g_pv678c28, g_pv6ef230, g_pv6ef234);
        Pv_Out(buf);
        sprintf(buf, &g_pv678c0c, g_pv6ef238);
        Pv_Out(buf);
        sprintf(buf, &g_pv678bec, g_pv6ef240, g_pv6ef244);
        Pv_Out(buf);
        sprintf(buf, &g_pv678bcc, g_pv6ef248);
        Pv_Out(buf);
        sprintf(buf, &g_pv678bac, g_pv697658, g_pv69765c);
        Pv_Out(buf);
        sprintf(buf, &g_pv678b90, g_pv697660);
        Pv_Out(buf);
    }
    CallThis(Pv_PostEvent, s, 1);
    if (g_pv696c70 || g_pv672d80 || g_pv696c74)
        CallC(Pv_Flush);
}

} // extern "C"

// === a lamp's flash (FUN_549e60)

extern "C" void Lf_Start(); KING_ALIAS("_Lf_Start", "_Effect_Start4d9460")
extern "C" void Lf_Lower(); KING_ALIAS("_Lf_Lower", "@Vehicle_LowerStep@12")
extern "C" void Lf_M1(); KING_ALIAS("_Lf_M1", "@SceneObj_Matrix1@12")
KING_VAR(BYTE, g_lf697f9c, 697f9c)
KING_VAR(BYTE*, g_lf6cec6c, 6cec6c)

extern "C" {

// [FUN_549e60] effect s at time t (newer than +0): the car's (+24) lamp point (type +60) in the world gets a flash,
// and the car steps lower in a single player game
void __fastcall Lamp_Flash(int* s, void*, int t)
{
    if (s[0] >= t)
        return;
    s[0] = t;
    BYTE* car = (BYTE*)s[6];
    const float* L = (const float*)(&g_lf697f9c + 2416 * F(int, car, 10420) + 60);
    Matrix M = F(Matrix, car, 708);
    const float* m = M.m;
    Vec3 w;
    w.x = (float)((double)m[6] * L[2] + (double)m[3] * L[1] + (double)m[0] * L[0] + m[9]);
    w.y = (float)((double)m[7] * L[2] + (double)m[4] * L[1] + (double)m[1] * L[0] + m[10]);
    w.z = (float)((double)m[8] * L[2] + (double)m[5] * L[1] + (double)m[2] * L[0] + m[11]);
    Matrix T;
    const float* n = CallThis<const Matrix*>(Lf_M1, car + 16, &T)->m;
    Vec3 v;
    v.x = (float)((double)w.z * n[6] + (double)w.y * n[3] + (double)w.x * n[0] + n[9]);
    v.y = (float)((double)w.z * n[7] + (double)w.y * n[4] + (double)w.x * n[1] + n[10]);
    v.z = (float)((double)w.z * n[8] + (double)w.y * n[5] + (double)w.x * n[2] + n[11]);
    CallC(Lf_Start, &v, 1);
    if (F(int, g_lf6cec6c, 1300) <= 1 && !F(int, g_lf6cec6c, 1316))
        CallThis(Lf_Lower, (BYTE*)s[6], -0.4f);
}

} // extern "C"

// === the player's controls and debug keys (FUN_53b4c0)

extern "C" void Pc_InList(); KING_ALIAS("_Pc_InList", "_Car_InList")
extern "C" void Pc_Grip(); KING_ALIAS("_Pc_Grip", "@Wheels_MinGrip@8")
extern "C" void Pc_Axis(); KING_ALIAS("_Pc_Axis", "@Controls_Axis@12")
extern "C" void Pc_Action(); KING_ALIAS("_Pc_Action", "@Controls_ActionState@12")
extern "C" void Pc_Act530100(); KING_ALIAS("_Pc_Act530100", "@Controls_530100@12")
extern "C" void Pc_Filter(); KING_ALIAS("_Pc_Filter", "@Filter_Value@4")
extern "C" void Pc_RampPos(); KING_ALIAS("_Pc_RampPos", "@Ramp_Position@8")
extern "C" void Pc_RampSet(); KING_ALIAS("_Pc_RampSet", "@Ramp_SetPosition@12")
extern "C" void Pc_M1(); KING_ALIAS("_Pc_M1", "@SceneObj_Matrix1@12")
extern "C" void Pc_Get800(); KING_ALIAS("_Pc_Get800", "@SceneObj_Get800@8")
extern "C" void Pc_Make(); KING_ALIAS("_Pc_Make", "_AI_Make449c40")
extern "C" void Pc_PosIdRV(); KING_ALIAS("_Pc_PosIdRV", "_Ai_GetPositionIdRV")
extern "C" float* __cdecl Pc_Coords(float* out, PvPos pos); KING_ALIAS("_Pc_Coords", "_Ai_GetCoordinatesPos")
extern "C" void Pc_53b3c0(); KING_ALIAS("_Pc_53b3c0", "@King_53b3c0@12")
extern "C" void Pc_5498b0(); KING_ALIAS("_Pc_5498b0", "@Obj5498b0_Init@12")
extern "C" void Pc_Toggle(); KING_ALIAS("_Pc_Toggle", "@Body_ToggleMode@8")
extern "C" void Pc_Probe(); KING_ALIAS("_Pc_Probe", "@Body_ProbeGround@12")
extern "C" void Pc_AddAssign(); KING_ALIAS("_Pc_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Pc_54c880(); KING_ALIAS("_Pc_54c880", "@Car_Set54c880@16")
extern "C" void Pc_Chase6(); KING_ALIAS("_Pc_Chase6", "@Copter_Chase6@12")
extern "C" void Pc_Chase10(); KING_ALIAS("_Pc_Chase10", "@Copter_Chase10@12")
extern "C" void Pc_Flasher(); KING_ALIAS("_Pc_Flasher", "@CarEffect_StartFlasher@4")
extern "C" void Pc_NewEffect(); KING_ALIAS("_Pc_NewEffect", "_Effect_New4daf40")
extern "C" void Pc_557000(); KING_ALIAS("_Pc_557000", "@Car_557000@8")
extern "C" void Pc_Working(); KING_ALIAS("_Pc_Working", "@Car_WorkingOrder@4")
extern "C" void Pc_GameTime(); KING_ALIAS("_Pc_GameTime", "_Game_Time")
extern "C" void __cdecl Cars_DumpHistory();
extern "C" void Pc_533590(); KING_ALIAS("_Pc_533590", "@Car_PlaceOnRoad@12")
extern "C" void Pc_5662f0(); KING_ALIAS("_Pc_5662f0", "@Copter_SetState@12")
extern "C" void Pc_SetText(); KING_ALIAS("_Pc_SetText", "@History_Record@16")
extern "C" void g_pcFlashFn(); KING_ALIAS("_g_pcFlashFn", "_Car_FlasherKick")
KING_VAR(BYTE, g_pcControls, 6d1db8)
KING_VAR(int, g_pc6cec68, 6cec68)
typedef int(__cdecl* PcReadInt)();
typedef float(__cdecl* PcReadFloat)();
KING_VAR(PcReadInt, g_pcIn79c, 67879c)               // the input readers
KING_VAR(PcReadInt, g_pcIn7a0, 6787a0)
KING_VAR(PcReadInt, g_pcIn7a4, 6787a4)
KING_VAR(PcReadInt, g_pcIn7a8, 6787a8)
KING_VAR(PcReadInt, g_pcIn7ac, 6787ac)
KING_VAR(PcReadInt, g_pcIn7b0, 6787b0)
KING_VAR(PcReadFloat, g_pcIn7b8, 6787b8)
KING_VAR(int, g_pc6d2068, 6d2068)
KING_VAR(int, g_pc6d206c, 6d206c)
KING_VAR(int, g_pc6d2084, 6d2084)
KING_VAR(int, g_pc6d2088, 6d2088)
KING_VAR(int, g_pc6d208c, 6d208c)
KING_VAR(int, g_pc6d2090, 6d2090)
KING_VAR(int, g_pc6d2094, 6d2094)
KING_VAR(int, g_pc6d2098, 6d2098)
KING_VAR(int, g_pc6d209c, 6d209c)
KING_VAR(BYTE, g_pcKey1958, 6d1958)                  // the debug keys
KING_VAR(BYTE, g_pcKey1968, 6d1968)
KING_VAR(BYTE, g_pcKey196a, 6d196a)
KING_VAR(BYTE, g_pcKey19a8, 6d19a8)
KING_VAR(BYTE, g_pcKey19aa, 6d19aa)
KING_VAR(BYTE, g_pcKey19ac, 6d19ac)
KING_VAR(BYTE, g_pcKey19ae, 6d19ae)
KING_VAR(BYTE, g_pcKey19b0, 6d19b0)
KING_VAR(BYTE, g_pcKey19b2, 6d19b2)
KING_VAR(BYTE, g_pcKey19b4, 6d19b4)
KING_VAR(BYTE, g_pcKey19b6, 6d19b6)
KING_VAR(BYTE, g_pcKey19b8, 6d19b8)
KING_VAR(BYTE, g_pcKey19ba, 6d19ba)
KING_VAR(BYTE, g_pcKey19d2, 6d19d2)
KING_VAR(BYTE, g_pcKey19d4, 6d19d4)
KING_VAR(BYTE, g_pcKey19d6, 6d19d6)
KING_VAR(BYTE, g_pcKey19da, 6d19da)
KING_VAR(BYTE, g_pcKey19dc, 6d19dc)
KING_VAR(BYTE, g_pcKey19de, 6d19de)
KING_VAR(BYTE, g_pcKey19e8, 6d19e8)
KING_VAR(BYTE, g_pcKey19ec, 6d19ec)
KING_VAR(BYTE, g_pcKey19f6, 6d19f6)
KING_VAR(BYTE, g_pcKey1abe, 6d1abe)
KING_VAR(BYTE, g_pcKey1ac2, 6d1ac2)
KING_VAR(int, g_pc696be4, 696be4)
KING_VAR(int, g_pc696be8, 696be8)
KING_VAR(int, g_pc696bec, 696bec)
KING_VAR(int, g_pc696c64, 696c64)
KING_VAR(int, g_pc696334, 696334)
KING_VAR(int, g_pc6796b0, 6796b0)
KING_VAR(int, g_pc68b990, 68b990)
KING_VAR(BYTE*, g_pc6d2078, 6d2078)
KING_VAR(Vec3, g_pcAhead, 702620)
KING_VAR(Vec3, g_pcUp, 702630)
KING_VAR(Vec3, g_pcDown, 702640)
KING_VAR(Vec3, g_pcEye, 702650)
KING_VAR(char, g_pc679204, 679204)
KING_VAR(char, g_pc6791f4, 6791f4)
KING_VAR(double, g_pc64b6e0, 64b6e0)
KING_VAR(float, g_pc64c250, 64c250)
KING_VAR(float, g_pc64c264, 64c264)
KING_VAR(double, g_pc64b788, 64b788)
KING_VAR(double, g_pc64b5b8, 64b5b8)
KING_VAR(float, g_pc64c258, 64c258)
KING_VAR(float, g_pc64c300, 64c300)
KING_VAR(float, g_pc64c33c, 64c33c)
KING_VAR(float, g_pc64c304, 64c304)
KING_VAR(float, g_pc64f40c, 64f40c)
KING_VAR(float, g_pc64f408, 64f408)
KING_VAR(float, g_pc64c968, 64c968)
KING_VAR(float, g_pc64c664, 64c664)
KING_VAR(float, g_pc64f31c, 64f31c)
KING_VAR(float, g_pc64c450, 64c450)
KING_VAR(float, g_pc64d4e8, 64d4e8)
KING_VAR(float, g_pc64c320, 64c320)
KING_VAR(float, g_pc64f404, 64f404)
KING_VAR(float, g_pc64f400, 64f400)
KING_VAR(float, g_pc64c448, 64c448)
KING_VAR(float, g_pc64d2f8, 64d2f8)
KING_VAR(float, g_pc64c408, 64c408)
KING_VAR(float, g_pc64c748, 64c748)
KING_VAR(double, g_pc64b678, 64b678)

static int Pc_Act(int i)
{
    return CallThis<int>(Pc_Action, &g_pcControls, i);
}

// the car's frame (rows +3352.. as columns) applied to v, plus the velocity +3316
static Vec3 Pc_Push(BYTE* car, double vx, double vy, double vz)
{
    Vec3 X;
    X.x = (float)((float)(vz * F(float, car, 3376) + vy * F(float, car, 3364) + vx * F(float, car, 3352)) +
                  (double)F(float, car, 3316));
    X.y = (float)((float)(vz * F(float, car, 3380) + vy * F(float, car, 3368) + vx * F(float, car, 3356)) +
                  (double)F(float, car, 3320));
    X.z = (float)(vz * F(float, car, 3384) + vy * F(float, car, 3372) + vx * F(float, car, 3360) + F(float, car, 3324));
    return X;
}

// a debug nudge: the ramp +16112 of the cab stepped back, the car's flip time and direction set, then pushed by v
static bool Pc_Flip(BYTE* s, BYTE* car, const Vec3& dir, double vx, double vy, double vz)
{
    BYTE* ramp = F(BYTE*, car, 21600) + 16112;
    if (!(CallThis<double>(Pc_RampPos, ramp) > g_pc64c264) && !g_pc6d2098)
        return false;
    float p = (float)(CallThis<double>(Pc_RampPos, F(BYTE*, F(BYTE*, s, 616), 21600) + 16112) - g_pc64b5b8);
    CallThis(Pc_RampSet, F(BYTE*, F(BYTE*, s, 616), 21600) + 16112, p);
    F(Vec3, s, 456) = dir;
    F(float, s, 468) = (float)CallC<double>(Pc_GameTime);
    BYTE* c = F(BYTE*, s, 616);
    Vec3 X = Pc_Push(c, vx, vy, vz);
    CallVirt(c + 16, 144, &X, 0);
    return true;
}

// whether the car may be flipped at its position: by the player's own height or the road under an AI car
static bool Pc_FlipAllowed(BYTE* car, float d, bool above, float lim)
{
    Matrix t;
    const float* m = CallThis<const Matrix*>(Pc_M1, car + 16, &t)->m;
    float px = m[9], py = m[10], pz = m[11];
    if (g_pc6d2098)
        return true;
    bool ok = above ? d > lim : d < lim;
    if (F(int, car, 20636) == g_pc68b990)
        return ok && pz < g_pc64c450;
    int room;
    CallC(Pc_Make, &room, F(int, CallThis<BYTE*>(Pc_Get800, car + 16), 12));
    if (room == -1)
        return false;
    PvPos pid;
    CallC(Pc_PosIdRV, &pid, room, px, py, pz);
    if (pid.d[0] == (DWORD)-1 && pid.d[1] == (DWORD)-1 && *(double*)&pid.d[2] == g_pc64b678 && !pid.d[4] &&
        !pid.d[5] && !pid.d[6])
        return false;
    float c[3];
    Pc_Coords(c, pid);
    if (!(above ? d > g_pc64f404 : d < g_pc64d4e8))
        return false;
    return (double)pz - c[2] < g_pc64c320;
}

// a nudge along the car's frame from its +3328 point
static void Pc_Nudge(BYTE* car, const Vec3& v, double k, bool plus)
{
    float ky = (float)(v.y * (double)k), kz = (float)(v.z * (double)k);
    double kx = v.x * (double)k;
    Vec3 X;
    if (plus)
    {
        X.x = (float)(kx + F(float, car, 3328));
        X.y = (float)((double)ky + F(float, car, 3332));
        X.z = (float)((double)kz + F(float, car, 3336));
    }
    else
    {
        X.x = (float)((double)F(float, car, 3328) - kx);
        X.y = (float)((double)F(float, car, 3332) - ky);
        X.z = (float)((double)F(float, car, 3336) - kz);
    }
    CallVirt(car + 16, 148, &X, 0);
}

static void Pc_Reset(BYTE* s, Matrix* m)
{
    BYTE* car = F(BYTE*, s, 616);
    CallVirt(car, 76, CallThis<BYTE*>(Pc_Get800, car + 16), m, 0);
    car = F(BYTE*, s, 616);
    if (F(BYTE*, car, 21600) && CallVirt<int>(car, 4) != 1)
        CallThis(Pc_54c880, F(BYTE*, s, 616), CallThis<int>(Pc_Get800, F(BYTE*, s, 616) + 16), (int)m);
    CallVirt(F(BYTE*, s, 616), 92, &g_pcEye);
    CallVirt(F(BYTE*, s, 616), 96, &g_pcEye);
}

extern "C" {

// [FUN_53b4c0] player s's controls (+616 car, its cab +21600): the pedals (+9996 throttle, +10000 brake, filtered
// +280), the steering (+10012, filtered +312, eased by a curve), the buttons, the gear choice (+10054) and the
// horn; then the debug keys: flips, nudges and resets of the car, the camera modes, the copter's orders, the
// logs, the flasher; finally the car's working order
void __fastcall Player_Controls(BYTE* s, void*)
{
    if (F(BYTE*, s, 616))
        F(BYTE*, s, 616) = CallC<BYTE*>(Pc_InList, F(BYTE*, s, 616));
    BYTE* car = F(BYTE*, s, 616);
    if (!car)
        return;
    BYTE* cab = F(BYTE*, car, 21600);
    if (!cab)
        return;
    F(int, cab, 10076) = g_pc6cec68;
    if (!F(int, s, 16))
        return;
    CallThis<double>(Pc_Grip, F(BYTE*, F(BYTE*, s, 616), 21600));
    F(int, cab, 10000) = 0;
    F(int, cab, 9996) = 0;
    int a = g_pcIn79c();
    int b = g_pcIn7a0();
    int c = g_pcIn7a4();
    F(short, cab, 10004) = (short)a;
    if (c && F(short, F(BYTE*, F(BYTE*, s, 616), 21600), 10004))
        F(short, cab, 10004) = 2;
    F(short, cab, 10006) = (short)b;
    if (c && b)
        F(short, cab, 10006) = 2;
    // the pedals
    if (g_pc6d2068)
    {
        float gas = (float)CallThis<double>(Pc_Axis, &g_pcControls, 2);
        float brk = (float)CallThis<double>(Pc_Axis, &g_pcControls, 1);
        CallThis(Pc_SetText, s + 280, gas, &g_pc679204);
        F(float, cab, 9996) = (float)((CallThis<float>(Pc_Filter, s + 280) + (double)g_pc64c250) * g_pc64b6e0);
        F(float, cab, 10000) = (float)(((double)brk + g_pc64c250) * g_pc64b6e0);
    }
    else
    {
        float gas = (float)CallThis<double>(Pc_Axis, &g_pcControls, 2);
        CallThis(Pc_SetText, s + 280, gas, &g_pc679204);
        double v = CallThis<float>(Pc_Filter, s + 280);
        if (!(v < g_pc64c264))
        {
            F(float, cab, 9996) = (float)v;
            F(int, cab, 10000) = 0;
        }
        else
        {
            F(int, cab, 9996) = 0;
            F(float, cab, 10000) = (float)-(v * v * v);
        }
    }
    if (!F(BYTE, cab, 9984))
    {
        F(short, cab, 10006) = 1;
        F(float, cab, 10000) = 1.0f;
    }
    F(BYTE, cab, 9985) = (BYTE)g_pcIn7b0();
    // the steering, eased
    float x = g_pcIn7b8();
    CallThis(Pc_SetText, s + 312, x, &g_pc6791f4);
    double fv = CallThis<float>(Pc_Filter, s + 312);
    double t = x87_sqrt(x87_fabs(x - fv)) * g_pc64b788 + g_pc64b5b8;
    if (t > g_pc64c250)
        t = g_pc64c250;
    double m = ((double)g_pc64c250 - t) * fv + t * x;
    float mf = (float)m;
    float am = (float)x87_fabs(m);
    double ca, cb, cc, add = 0;
    if (fv < g_pc64c258)
    {
        ca = g_pc64c300;
        cb = g_pc64c33c;
        cc = g_pc64c304;
    }
    else
    {
        ca = g_pc64f40c;
        cb = g_pc64f408;
        cc = g_pc64c968;
        add = 2.0f;
    }
    double r = ((ca * am + cb) * am + cc) * am + (float)add;
    if (!(mf > g_pc64c264))
        r = -r;
    F(float, cab, 10012) = (float)r;
    F(BYTE, cab, 9986) = (BYTE)g_pcIn7a8();
    F(BYTE, cab, 9987) = (BYTE)g_pcIn7ac();
    // the gear
    if (F(BYTE, cab, 9984) == 2)
    {
        BYTE* cb2 = F(BYTE*, F(BYTE*, s, 616), 21600);
        if (F(int, cb2, 7188))
        {
            int up, dn;
            if (g_pc6d206c == 1)
            {
                up = Pc_Act(10);
                dn = Pc_Act(11);
            }
            else
            {
                up = Pc_Act(5);
                dn = Pc_Act(6);
            }
            if (Pc_Act(7))
                dn = 2;
            short g;
            if (up)
                g = dn ? 21 : 100;
            else if (dn)
                g = 20;
            else if (Pc_Act(17))
                g = 21;
            else
                g = g_pc6d206c == 1 ? 21 : 0;
            F(short, cab, 10054) = g;
        }
        else
        {
            int up = Pc_Act(5);
            int dn = Pc_Act(6);
            if (Pc_Act(7))
                dn = 2;
            if (up)
            {
                if (dn)
                    F(short, cab, 10054) = 21;
                else
                    F(short, cab, 10054) = up == 2 ? 22 : 1;
            }
            else if (dn)
                F(short, cab, 10054) = dn == 2 ? 20 : -1;
            else if (Pc_Act(17))
                F(short, cab, 10054) = 21;
            else
            {
                int i = 0, n = F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 15324) - 1;
                for (; i < n; i++)
                    if (Pc_Act(i + 8))
                    {
                        F(short, cab, 10054) = (short)(i + 22);
                        break;
                    }
                if (i == F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 15324) - 1)
                {
                    if (Pc_Act(7))
                        F(short, cab, 10054) = 20;
                    else if (CallThis<int>(Pc_Act530100, &g_pcControls,
                                           F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 7188)))
                        F(short, cab, 10054) = 21;
                    else
                        F(short, cab, 10054) = 0;
                }
            }
        }
    }
    if (Pc_Act(41))
    {
        BYTE* cb2 = F(BYTE*, F(BYTE*, s, 616), 21600);
        F(int, cb2, 10108) = F(int, cb2, 15488) == 2 ? -1 : 1;
    }
    F(short, F(BYTE*, F(BYTE*, s, 616), 21600), 10010) = (short)Pc_Act(22);
    F(BYTE*, s, 624) = CallC<BYTE*>(Pc_InList, F(BYTE*, s, 624));
    // the debug flips
    if (g_pc6d2088 || g_pc6d2098 || g_pc6d2084)
    {
        if (g_pcKey19aa && g_pcKey196a)
        {
            g_pcKey19aa = 0;
            BYTE* c2 = F(BYTE*, s, 616);
            double d = (double)F(float, c2, 3428) * F(float, c2, 3324) + (double)F(float, c2, 3320) * F(float, c2, 3416) +
                       (double)F(float, c2, 3404) * F(float, c2, 3316);
            if (d < g_pc64c664)
            {
                Vec3 dir = {-g_pcAhead.x, -g_pcAhead.y, -g_pcAhead.z};
                Pc_Flip(s, c2, dir, g_pcAhead.x * (double)g_pc64c450, g_pcAhead.y * (double)g_pc64c450,
                        g_pcAhead.z * (double)g_pc64c450);
            }
        }
        if (g_pcKey19ac && g_pcKey196a)
        {
            g_pcKey19ac = 0;
            BYTE* c2 = F(BYTE*, s, 616);
            double d = (double)F(float, c2, 3324) * F(float, c2, 3428) + (double)F(float, c2, 3320) * F(float, c2, 3416) +
                       (double)F(float, c2, 3404) * F(float, c2, 3316);
            if (d > g_pc64f31c)
            {
                float ny = -g_pcAhead.y, nz = -g_pcAhead.z;
                Pc_Flip(s, c2, g_pcAhead, -(double)g_pcAhead.x * g_pc64c450, ny * (double)g_pc64c450,
                        nz * (double)g_pc64c450);
            }
        }
        for (int k = 0; k < 2; k++)
        {
            BYTE& key = k == 0 ? g_pcKey19ae : g_pcKey19b0;
            if (!key || !g_pcKey196a)
                continue;
            key = 0;
            BYTE* c2 = F(BYTE*, s, 616);
            float d = (float)((double)F(float, c2, 3324) * F(float, c2, 3432) +
                              (double)F(float, c2, 3320) * F(float, c2, 3420) +
                              (double)F(float, c2, 3408) * F(float, c2, 3316));
            if (!Pc_FlipAllowed(c2, d, k == 1, k == 0 ? g_pc64d4e8 : g_pc64f404))
                continue;
            if (k == 0)
            {
                Vec3 dir = {-g_pcUp.x, -g_pcUp.y, -g_pcUp.z};
                double ax = g_pcAhead.x * (double)g_pc64c304;
                float ay = (float)(g_pcAhead.y * (double)g_pc64c304), az = (float)(g_pcAhead.z * (double)g_pc64c304);
                double vx = ax + g_pcUp.x;
                float vy = (float)((double)g_pcUp.y + ay), vz = (float)((double)g_pcUp.z + az);
                Pc_Flip(s, c2, dir, vx * g_pc64c450, vy * (double)g_pc64c450, vz * (double)g_pc64c450);
            }
            else
                Pc_Flip(s, c2, g_pcUp, g_pcUp.x * (double)g_pc64c968, g_pcUp.y * (double)g_pc64c968,
                        g_pcUp.z * (double)g_pc64c968);
        }
    }
    if ((g_pc6d208c || g_pc6d2098 || g_pc6d2084) && g_pcKey19b2 && g_pcKey196a)
    {
        g_pcKey19b2 = 0;
        CallThis(Pc_533590, s, 1);
    }
    if ((g_pc6d2090 || g_pc6d2098 || g_pc6d2084) && g_pcKey1958)
    {
        CallThis(Pc_53b3c0, s, 1065353216);
        BYTE* c2 = F(BYTE*, s, 616);
        CallThis(Pc_5498b0, c2 + 21568, c2);
        CallThis(Pc_RampSet, F(BYTE*, s, 616) + 21360, 1.0f);
    }
    if (g_pc6d2094)
    {
        F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 10080) = 0;
        CallThis(Pc_RampSet, F(BYTE*, F(BYTE*, s, 616), 21600) + 16112, 1.0f);
    }
    // the camera modes
    if (g_pc696be8 && g_pcKey1ac2 && g_pcKey1968)
        g_pc696be8 = g_pc696be8 % 2 + 1;
    if (g_pc696bec && g_pcKey1ac2 && g_pcKey196a)
        g_pc696bec = g_pc696bec % 2 + 1;
    if (g_pc696be4 && g_pcKey1abe && g_pcKey196a)
        g_pc696be4 = g_pc696be4 % 3 + 1;
    if (g_pc6d2098 || g_pc6d2084)
    {
        if (g_pcKey19b4 && g_pcKey196a)
        {
            g_pcKey19b4 = 0;
            BYTE* c2 = F(BYTE*, s, 616);
            double d = (double)F(float, c2, 3324) * F(float, c2, 3424) + (double)F(float, c2, 3320) * F(float, c2, 3412) +
                       (double)F(float, c2, 3316) * F(float, c2, 3400);
            if (d > g_pc64f400)
            {
                float ny = -g_pcDown.y, nz = -g_pcDown.z;
                Vec3 X = Pc_Push(c2, -(double)g_pcDown.x * g_pc64c450, ny * (double)g_pc64c450, nz * (double)g_pc64c450);
                CallVirt(c2 + 16, 144, &X, 0);
            }
        }
        if (g_pcKey19b6 && g_pcKey196a)
        {
            g_pcKey19b6 = 0;
            BYTE* c2 = F(BYTE*, s, 616);
            double d = (double)F(float, c2, 3324) * F(float, c2, 3424) + (double)F(float, c2, 3320) * F(float, c2, 3412) +
                       (double)F(float, c2, 3316) * F(float, c2, 3400);
            if (d < g_pc64c448)
            {
                Vec3 X = Pc_Push(c2, g_pcDown.x * (double)g_pc64c450, g_pcDown.y * (double)g_pc64c450,
                                 g_pcDown.z * (double)g_pc64c450);
                CallVirt(c2 + 16, 144, &X, 0);
            }
        }
        if (g_pcKey19d4)
            CallThis(Pc_RampSet, F(BYTE*, F(BYTE*, s, 616), 21600) + 16112, 1.0f);
        if (g_pcKey1abe && !g_pc696be4 && g_pcKey196a)
            g_pc696be4 = 2;
        if (g_pcKey1ac2)
        {
            if (!g_pc696be8 && g_pcKey1968 && !g_pcKey196a)
                g_pc696be8 = 2;
            if (!g_pc696bec && g_pcKey196a && !g_pcKey1968)
                g_pc696bec = 2;
        }
        BYTE* c2 = F(BYTE*, s, 616);
        if (g_pcKey19d2)
        {
            if (g_pcKey196a)
                Pc_Nudge(F(BYTE*, s, 616), g_pcAhead, g_pc64d2f8, true);
            if (g_pcKey19d2 && g_pcKey1968)
                Pc_Nudge(F(BYTE*, s, 616), g_pcAhead, g_pc64d2f8, false);
        }
        if (g_pcKey19ec)
        {
            if (g_pcKey196a)
                Pc_Nudge(F(BYTE*, s, 616), g_pcUp, g_pc64c408, true);
            if (g_pcKey19ec && g_pcKey1968)
                Pc_Nudge(F(BYTE*, s, 616), g_pcUp, g_pc64c408, false);
        }
        if (g_pcKey19f6)
        {
            Vec3 d2;
            d2.x = (float)((double)g_pcDown.x + g_pcDown.x);
            d2.y = (float)((double)g_pcDown.y + g_pcDown.y);
            d2.z = (float)((double)g_pcDown.z + g_pcDown.z);
            if (g_pcKey196a)
                Pc_Nudge(F(BYTE*, s, 616), d2, 1.0, true);
            if (g_pcKey19f6 && g_pcKey1968)
                Pc_Nudge(F(BYTE*, s, 616), d2, 1.0, false);
        }
        (void)c2;
    }
    if (!g_pc6d209c)
        return;
    g_pc6d2098 = 1;
    if (g_pcKey19d6 && g_pcKey196a)
    {
        g_pc696c64 = !g_pc696c64;
        g_pc696334 = g_pc696c64;
    }
    if (g_pcKey19e8 && g_pcKey196a)
        Cars_DumpHistory();
    if (g_pcKey19dc && g_pcKey196a)
        g_pc6796b0 |= 1;
    if (g_pcKey19de && g_pcKey196a)
        g_pc6796b0 &= ~1;
    if (g_pcKey19a8 && g_pcKey196a)
        CallThis(Pc_Toggle, F(BYTE*, F(BYTE*, s, 616), 21600));
    if (g_pcKey19b8 && g_pcKey196a)
    {
        // set the car upright where it is
        g_pcKey19b8 = 0;
        F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 13124) = 1;
        CallThis(Pc_Probe, F(BYTE*, s, 616) + 16, 0);
        Matrix m;
        CallThis(Pc_M1, F(BYTE*, s, 616) + 16, &m);
        Vec3 v, tmp;
        v.x = (float)(g_pcUp.x * (double)g_pc64c748);
        v.y = (float)(g_pcUp.y * (double)g_pc64c748);
        v.z = (float)(g_pcUp.z * (double)g_pc64c748);
        CallThis(Pc_AddAssign, &m.m[9], &tmp, &v);
        Pc_Reset(s, &m);
    }
    if (g_pcKey19ba && g_pcKey196a)
    {
        // and turned over
        g_pcKey19ba = 0;
        F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 13124) = 1;
        Matrix m1, R, m;
        CallThis(Pc_M1, F(BYTE*, s, 616) + 16, &m1);
        Matrix_Multiply(&m, Matrix_RotationX(&R, -1.5707964f), &m1);
        Pc_Reset(s, &m);
    }
    BYTE* p624 = F(BYTE*, s, 624);
    if (p624 && F(BYTE*, p624, 21604))
    {
        BYTE* cp = F(BYTE*, p624, 21604);
        if (g_pcKey19b6)
            CallThis(Pc_5662f0, cp, 3);
        if (g_pcKey19b8 && F(int, F(BYTE*, F(BYTE*, s, 624), 21604), 3236) != 6)
            CallThis(Pc_Chase6, F(BYTE*, F(BYTE*, s, 624), 21604), F(BYTE*, s, 616));
        if (g_pcKey19ba && F(int, F(BYTE*, F(BYTE*, s, 624), 21604), 3236) != 10)
            CallThis(Pc_Chase10, F(BYTE*, F(BYTE*, s, 624), 21604), F(BYTE*, s, 616));
    }
    if (g_pcKey19da && g_pcKey196a)
    {
        if (CallVirt<int>(F(BYTE*, s, 616), 4) == 1)
        {
            BYTE* tr = F(BYTE*, F(BYTE*, g_pc6d2078, 616), 21880);
            CallThis(Pc_Flasher, tr ? tr + 21568 : F(BYTE*, s, 616) + 21568);
        }
        else
            CallC(Pc_NewEffect, F(BYTE*, s, 616), (void*)g_pcFlashFn);
    }
    CallThis<float>(Pc_557000, F(BYTE*, s, 616));
    CallThis<double>(Pc_Working, F(BYTE*, s, 616));
}

} // extern "C"

// === a car giving way to the one ahead (FUN_54f040)

struct GwRoom
{
    DWORD flags, m_4;
    double width;
    const char* module;
    const char* room;
    double m_24;
    int m_32, m_36, m_40, m_44;
};
extern "C" void Gw_Moving(); KING_ALIAS("_Gw_Moving", "_Moving_Of")
extern "C" void Gw_From16(); KING_ALIAS("_Gw_From16", "_Car_InListFrom16")
extern "C" void Gw_53f7b0(); KING_ALIAS("_Gw_53f7b0", "_Car_53f7b0")
extern "C" void Gw_ObjAt(); KING_ALIAS("_Gw_ObjAt", "_Room_ObjectAt")
extern "C" void Gw_IsPlace(); KING_ALIAS("_Gw_IsPlace", "_Road_IsPlace")
extern "C" PvPos* __cdecl Gw_Right(PvPos* out, PvPos pos); KING_ALIAS("_Gw_Right", "_Ai_GetRightDirectionOf")
extern "C" void __cdecl Gw_Room(PvPos pos, GwRoom* out); KING_ALIAS("_Gw_Room", "_Road_GetRoadParameters")
extern "C" float* __cdecl Gw_Coords(float* out, PvPos pos); KING_ALIAS("_Gw_Coords", "_Ai_GetCoordinatesPos")
extern "C" float* __cdecl Gw_Tangent(float* out, PvPos pos); KING_ALIAS("_Gw_Tangent", "_Ai_GetTangent")
extern "C" void Gw_NextPoint(); KING_ALIAS("_Gw_NextPoint", "@Points_NextActive@12")
extern "C" void Gw_M548(); KING_ALIAS("_Gw_M548", "@SceneObj_Matrix548@12")
extern "C" void Gw_M1(); KING_ALIAS("_Gw_M1", "@SceneObj_Matrix1@12")
extern "C" void Gw_5893d0(); KING_ALIAS("_Gw_5893d0", "_Body_5893d0")
KING_VAR(BYTE*, g_gw6ef1fc, 6ef1fc)
KING_VAR(BYTE*, g_gw6cec6c, 6cec6c)
KING_VAR(BYTE**, g_gw68cf38, 68cf38)
KING_VAR(Vec3, g_gwAhead, 702620)
KING_VAR(Vec3, g_gwUp, 702630)
KING_VAR(Vec3, g_gwEye, 702650)
KING_VAR(char, g_gw66d638, 66d638)
KING_VAR(char, g_gw66d628, 66d628)
KING_VAR(float, g_gw64c2f8, 64c2f8)
KING_VAR(float, g_gw64c40c, 64c40c)
KING_VAR(double, g_gw64b678, 64b678)
KING_VAR(double, g_gw64b6e0, 64b6e0)
KING_VAR(float, g_gw6796b8, 6796b8)
KING_VAR(float, g_gw6796b4, 6796b4)
KING_VAR(float, g_gw64c250, 64c250)
KING_VAR(float, g_gw64c264, 64c264)
KING_VAR(double, g_gw64b668, 64b668)
KING_VAR(double, g_gw64b898, 64b898)
KING_VAR(double, g_gw64b6a8, 64b6a8)
KING_VAR(double, g_gw64d0f0, 64d0f0)
KING_VAR(float, g_gw64d7b8, 64d7b8)
KING_VAR(float, g_gw64c320, 64c320)
KING_VAR(float, g_gw64d2f8, 64d2f8)
KING_VAR(double, g_gw64f770, 64f770)
KING_VAR(float, g_gw64c904, 64c904)
KING_VAR(float, g_gw64c450, 64c450)

static void Gw_RoomInit(GwRoom* r)
{
    r->flags = 0;
    r->width = 0;
    r->module = &g_gw66d638;
    r->room = &g_gw66d628;
    r->m_24 = 0;
    r->m_32 = 0;                                    // read but not always written: the original sees stack
    r->m_36 = -1;                                   // leftovers here
    r->m_40 = -1;
}

// the state of a car w.r.t. the one ahead, written to s and the result
static BYTE* Gw_Finish(BYTE* s, BYTE* mov, const Vec3& mv, BYTE* other, int st)
{
    Vec3 d;
    d.x = (float)((double)F(float, mov, 416) - mv.x);
    d.y = (float)((double)F(float, mov, 420) - mv.y);
    d.z = (float)((double)F(float, mov, 424) - mv.z);
    double L = Vec3_Length(&d);
    if (L < g_gw64d2f8)
        return other ? other + 16 : 0;
    float L2 = (float)x87_sqrt((L - g_gw64d2f8) * F(float, mov, 3268) * g_gw64f770);
    if (st)
        F(int, s, 20604) = 0;
    else
    {
        F(int, s, 20604) = 1;
        F(float, s, 20616) = (float)(Vec3_Length(&F(Vec3, s, 432)) * g_gw64c904 + g_gw64c450);
    }
    F(float, s, 20612) = (float)F(double, g_gw6ef1fc, 2968);
    F(int, s, 20596) = st;
    F(int, s, 20600) = st;
    F(float, s, 20628) = L2;
    if (other)
    {
        F(int, s, 20632) = CallC<int>(Gw_5893d0, F(int, s, 1044));
        return other + 16;
    }
    F(int, s, 20632) = 0;
    return 0;
}

extern "C" {

// [FUN_54f040] car s (its moving body from +1044) checks the car in front on the same road: with both on their
// lanes and close, whether it must stop, give way or keep its distance (1-6 into +20596 / +20600, 0 to go), the
// time to recheck (+20612, +20616) and the gap (+20628); the car ahead (its +16) or 0
BYTE* __fastcall Car_GiveWay(BYTE* s, void*)
{
    F(int, s, 20600) = 0;
    double now = F(double, g_gw6ef1fc, 2968);
    if (F(int, s, 20604) || F(float, s, 20616) < now - F(float, s, 20612))
    {
        F(int, s, 20596) = 0;
        F(int, s, 20628) = 0;
        F(int, s, 20632) = 0;
    }
    int ahead6 = 0, keep = 0;
    if (!F(int, s, 1220) || !F(int, s, 912) || F(int, g_gw6cec6c, 1300) > 1 || F(int, s, 388))
        return 0;
    BYTE* mov = CallC<BYTE*>(Gw_Moving, F(BYTE*, s, 1044));
    if (!mov)
        return 0;
    if (F(int, s, 20604) && F(float, s, 20616) > F(double, g_gw6ef1fc, 2968) - F(float, s, 20612))
        return 0;
    F(int, s, 20604) = 0;
    F(float, s, 20616) = 10.0f;
    const float* R = (const float*)(s + 564);
    const Vec3& A = g_gwAhead;
    Vec3 fwd;
    fwd.x = (float)((double)A.z * R[6] + (double)A.y * R[3] + (double)A.x * R[0]);
    fwd.y = (float)((double)A.z * R[7] + (double)A.y * R[4] + (double)A.x * R[1]);
    fwd.z = (float)((double)A.z * R[8] + (double)A.y * R[5] + (double)A.x * R[2]);
    Vec3 mv = F(Vec3, mov, 452);
    float spd = (float)Vec3_Length(&mv);
    Matrix M548;
    CallThis(Gw_M548, mov, &M548);
    Vec3 d;
    d.x = (float)((double)F(float, mov, 416) - mv.x);
    d.y = (float)((double)F(float, mov, 420) - mv.y);
    d.z = (float)((double)F(float, mov, 424) - mv.z);
    double len = Vec3_Length(&d);
    if (len > g_gw64c2f8 || len < g_gw64c40c)
        return 0;
    const float* sv = (const float*)(s + 468);
    double dd = ((double)sv[2] - mv.z) * ((double)mv.z + sv[2]) + ((double)sv[1] - mv.y) * ((double)mv.y + sv[1]) +
                ((double)sv[0] - mv.x) * ((double)mv.x + sv[0]);
    if (dd > g_gw64b678)
        ahead6 = 6;
    BYTE* other = CallC<BYTE*>(Gw_From16, F(BYTE*, s, 1044));
    int st = 0;
    for (;;)
    {
        if (!other)
            break;
        PvPos p1 = {{(DWORD)-1, (DWORD)-1, 0, 0, 0, 0, 0, (DWORD)-1}};
        PvPos p2 = {{(DWORD)-1, (DWORD)-1, 0, 0, 0, 0, 0, (DWORD)-1}};
        void* obj = 0;
        CallC(Gw_ObjAt, F(int, F(BYTE*, s, 820), 12), s + 600, &obj, &p1, 0);
        if (!obj)
            break;
        CallC(Gw_ObjAt, F(int, F(BYTE*, other, 820), 12), &M548.m[9], &obj, &p2, 0);
        if (!obj)
            break;
        PvPos t;
        p1 = *Gw_Right(&t, p1);
        p2 = *Gw_Right(&t, p2);
        GwRoom r1, r2;
        Gw_RoomInit(&r1);
        Gw_RoomInit(&r2);
        Gw_Room(p1, &r1);
        Gw_Room(p2, &r2);
        int wide = r1.m_32 > 1;
        float half0 = (float)(r1.width * g_gw64b6e0);
        Vec3 c, tg, tn;
        Gw_Coords(&c.x, p1);
        Gw_Tangent(&tg.x, p1);
        Vec3_Normalized(&tn, &tg);
        Vec3 nr, n;
        nr.x = (float)((double)tn.y * g_gwUp.z - (double)tn.z * g_gwUp.y);
        nr.y = (float)((double)tn.z * g_gwUp.x - (double)tn.x * g_gwUp.z);
        nr.z = (float)((double)tn.x * g_gwUp.y - (double)tn.y * g_gwUp.x);
        Vec3_Normalized(&n, &nr);
        Vec3 r;
        r.x = (float)((double)c.x - F(float, s, 600));
        r.y = (float)((double)c.y - F(float, s, 604));
        r.z = (float)((double)c.z - F(float, s, 608));
        float c2[3];
        Gw_Coords(c2, p2);
        Vec3 tg2, tn2;
        Gw_Tangent(&tg2.x, p2);
        Vec3_Normalized(&tn2, &tg2);
        if ((double)tn.x * fwd.x + (double)tn.z * fwd.z + (double)tn.y * fwd.y < g_gw64b678)
        {
            n.x = -n.x;
            n.y = -n.y;
            n.z = -n.z;
        }
        int place = CallC<int>(Gw_IsPlace, &p1);
        double X = (double)r.y * g_gwUp.z - (double)r.z * g_gwUp.y;
        double Y = (double)r.z * g_gwUp.x - (double)r.x * g_gwUp.z;
        double Z = (double)r.x * g_gwUp.y - (double)r.y * g_gwUp.x;
        Vec3 w = tn;
        if (Z * tn.z + Y * tn.y + X * tn.x < g_gw64b678)
        {
            w.x = -tn.x;
            w.y = -tn.y;
            w.z = -tn.z;
        }
        // the car's points against the lane
        float half = (float)((double)half0 + g_gw6796b8);
        float lo = (float)((double)half + g_gw64c250), hi = -lo;
        int found = 0;
        Vec3 pt = {0, 0, 0};
        BYTE* body = s + 16;
        int more;
        do
        {
            more = CallThis<int>(Gw_NextPoint, body, &pt);
            if (found)
                break;
            if (!more)
                pt = g_gwEye;
            const float* M = (const float*)(s + 564);
            double qx = (double)pt.z * M[6] + (double)pt.y * M[3] + (double)pt.x * M[0] + M[9];
            float qy = (float)((double)pt.z * M[7] + (double)pt.y * M[4] + (double)pt.x * M[1] + M[10]);
            double v = ((double)c.y - qy) * n.y + ((double)c.x - qx) * n.x;
            if (v < lo)
                lo = (float)v;
            if (v > hi)
                hi = (float)v;
            if (!place)
            {
                if (v > -(double)half && v < half)
                    found = 1;
            }
            else if (v < g_gw6796b8 && v > -(double)half)
                found = 1;
        } while (more);
        // both on the same lane of one road
        BYTE* L1 = g_gw68cf38[p1.d[0]];
        BYTE* L2 = g_gw68cf38[p2.d[0]];
        short a1 = F(short, F(BYTE*, L1, 112), 16), b1 = F(short, F(BYTE*, L1, 116), 16);
        short a2 = F(short, F(BYTE*, L2, 112), 16), b2 = F(short, F(BYTE*, L2, 116), 16);
        int same = (a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2);
        if ((r1.flags & 48) || (r2.flags & 48))
            same = 0;
        if (!found || !same)
            break;
        Vec3 v = fwd;
        float sp = (float)Vec3_Length(&F(Vec3, s, 468));
        float lr = (float)Vec3_Length(&r);
        if (sp > g_gw64c264)
        {
            v.x = (float)(F(float, s, 468) / (double)sp);
            v.y = (float)(F(float, s, 472) / (double)sp);
            v.z = (float)(F(float, s, 476) / (double)sp);
        }
        double dw = (double)w.x * fwd.x + (double)w.z * fwd.z + (double)w.y * fwd.y;
        int fwdOk = dw > g_gw64b678;
        int fwdStrong = dw > g_gw64b898;
        int velOk = (double)v.y * w.y + (double)v.z * w.z + (double)v.x * w.x > g_gw64b898;
        int backward = (double)v.z * fwd.z + (double)v.y * fwd.y + (double)v.x * fwd.x < g_gw64b6a8;
        if (sp > g_gw64c250)
        {
            if (!fwdOk && lr > g_gw64b668 && !wide)
                return Gw_Finish(s, mov, mv, other, 1);
            if (backward)
                return Gw_Finish(s, mov, mv, other, 3);
            if (!fwdStrong || !velOk)
                return Gw_Finish(s, mov, mv, other, 2);
            if (!wide && hi > g_gw64d0f0 && lo < g_gw64c264)
                return Gw_Finish(s, mov, mv, other, 5);
            break;
        }
        if (!fwdOk || !fwdStrong)
            return Gw_Finish(s, mov, mv, other, 4);
        int n2 = (int)(((double)half0 - lr) / g_gw6796b4) + 1;
        if (n2 <= 1)
        {
            st = 0;
            goto check6;
        }
        {
            int i = 0;
            for (; i < 6; i++)
            {
                BYTE* o = F(BYTE*, s, 3524 + 4 * i);
                if (!o)
                    break;
                BYTE* c3 = CallC<BYTE*>(Gw_53f7b0, o);
                Matrix t2;
                const float* pm = &CallThis<const Matrix*>(Gw_M1, o, &t2)->m[9];
                Vec3 pos = {pm[0], pm[1], pm[2]};
                Vec3 d3;
                d3.x = (float)((double)F(float, s, 552) - pos.x);
                d3.y = (float)((double)F(float, s, 556) - pos.y);
                d3.z = (float)((double)F(float, s, 560) - pos.z);
                if (c3 == s || !(F(float, s, 3548 + 4 * i) < g_gw64d7b8))
                    continue;
                double lx = (double)pos.y * F(float, s, 672) + (double)pos.z * F(float, s, 684) +
                            (double)pos.x * F(float, s, 660) + F(float, s, 696);
                double ly = (double)pos.y * F(float, s, 676) + (double)pos.z * F(float, s, 688) +
                            (double)pos.x * F(float, s, 664) + F(float, s, 700);
                if (!(x87_fabs(lx) < ly))
                    continue;
                const float* ov = (const float*)(o + 452);
                if ((double)d3.z * ov[2] + (double)d3.y * ov[1] + (double)ov[0] * d3.x > g_gw64b678)
                    break;
            }
            if (i < 6 && F(BYTE*, s, 3524 + 4 * i))
                keep = 0;
            else
                return Gw_Finish(s, mov, mv, other, 4);
        }
        break;
    }
    st = keep;
check6:
    if (ahead6 == 6)
    {
        if (!(spd > g_gw64c320) ||
            !((double)mv.x * fwd.x + (double)mv.z * fwd.z + (double)mv.y * fwd.y < g_gw64b678))
            st = 6;
    }
    return Gw_Finish(s, mov, mv, other, st);
}

} // extern "C"


// === placing the truck and bringing its trailer along (a method at $L_55de40)

extern "C" void Tp_Place(); KING_ALIAS("_Tp_Place", "@Car_Place@16")
extern "C" void Tp_Set(); KING_ALIAS("_Tp_Set", "@Car_Set54c880@16")
extern "C" void Tp_M1(); KING_ALIAS("_Tp_M1", "@SceneObj_Matrix1@12")
extern "C" void Tp_Get800(); KING_ALIAS("_Tp_Get800", "@SceneObj_Get800@8")
extern "C" void Tp_HitObject(); KING_ALIAS("_Tp_HitObject", "_Query_HitObject")
KING_VAR(BYTE, g_tpTypes, 697f9c)
KING_VAR(Vec3, g_tpAhead, 702620)

extern "C" {

// [$L_55de40] truck c placed at (a, b) (mode 2 by Car_Place, else Car_Set54c880); a trailer (+21880) is moved by
// the gap between the truck's hitch (type record +156, through the body matrix +708) and its own (+168 shifted
// by +204 along 702620), checked against the scene from its old place and put down there
void __fastcall Truck_PlaceWithTrailer(BYTE* c, void*, int a, int b, int mode)
{
    if (mode == 2)
        CallThis(Tp_Place, c, a, b);
    else
        CallThis(Tp_Set, c, a, b);
    BYTE* tr = F(BYTE*, c, 21880);
    if (!tr)
        return;
    Matrix M1, Mt;
    CallThis(Tp_M1, c + 16, &M1);
    CallThis(Tp_M1, tr + 16, &Mt);
    float m[12];
    memcpy(m, c + 708, 48);
    const float* h = (const float*)(&g_tpTypes + F(int, c, 10420) * 2416 + 156);
    double hy = (double)m[7] * h[2] + (double)m[4] * h[1] + (double)m[1] * h[0] + m[10];
    double hz = (double)m[8] * h[2] + (double)m[5] * h[1] + (double)m[2] * h[0] + m[11];
    Vec3 hp;
    hp.x = (float)((double)m[0] * h[0] + (double)m[6] * h[2] + (double)m[3] * h[1] + m[9]);
    hp.y = (float)hy;
    hp.z = (float)hz;
    const BYTE* tt = &g_tpTypes + F(int, tr, 10420) * 2416;
    double d = F(float, tt, 204);
    float vx = (float)(g_tpAhead.x * d), vy = (float)(g_tpAhead.y * d);
    double vz = g_tpAhead.z * d;
    memcpy(m, tr + 708, 48);
    float px = (float)((double)vx + F(float, tt, 168)), py = (float)((double)vy + F(float, tt, 172));
    double pz = vz + F(float, tt, 176);
    Vec3 tp;
    tp.x = (float)(pz * m[6] + (double)py * m[3] + (double)m[0] * px + m[9]);
    tp.y = (float)(pz * m[7] + (double)py * m[4] + (double)m[1] * px + m[10]);
    tp.z = (float)(pz * m[8] + (double)py * m[5] + (double)m[2] * px + m[11]);
    Vec3 gap;
    Vec3_Sub(&gap, &hp, &tp);
    Matrix T, W;
    Matrix_Multiply(&W, Matrix_Translation(&T, gap), &M1);
    BYTE* o = CallC<BYTE*>(Tp_HitObject, CallThis<BYTE*>(Tp_Get800, tr + 16), &Mt.m[9], &W.m[9]);
    if (!o)
    {
        o = CallThis<BYTE*>(Tp_Get800, F(BYTE*, c, 21880) + 16);
        CallVirt(c + 16, 28, 1);
    }
    CallThis(Tp_Set, F(BYTE*, c, 21880), o, &W);
}

} // extern "C"

// === a kick upwards at a point of the car while its flasher runs (a callback at $L_549580)

extern "C" void Ku_State(); KING_ALIAS("_Ku_State", "@Obj549bf0_Get@8")
extern "C" void Ku_Stop(); KING_ALIAS("_Ku_Stop", "@CarEffect_Stop@4")
extern "C" void Ku_M1(); KING_ALIAS("_Ku_M1", "@SceneObj_Matrix1@12")
extern "C" void Ku_Set(); KING_ALIAS("_Ku_Set", "@Vec3_SetThis@20")
extern "C" void Ku_MulAssign(); KING_ALIAS("_Ku_MulAssign", "@Vec3_MulAssign@16")
KING_VAR(Vec3, g_kuUp, 702630)
KING_VAR(float, g_ku64c938, 64c938)

extern "C" {

// [$L_549580] for each point p (in the body frame +708) of car c whose flasher (+21568) is in state 1 or 2: with
// mode 0 the car gets an upward kick (702630 * 64c938) on its velocity, the turn it makes about p on its spin, and
// the kick's force (times the mass +3284, times 25) at p; mode 1 stops the flasher
void __cdecl Car_FlasherKick(const float* p, int mode, BYTE* c)
{
    BYTE* fl = c + 21568;
    if (CallThis<int>(Ku_State, fl) != 2 && CallThis<int>(Ku_State, fl) != 1)
        return;
    if (mode)
    {
        if (mode == 1)
            CallThis(Ku_Stop, fl);
        return;
    }
    float m[12];
    memcpy(m, c + 708, 48);
    Vec3 P;
    P.x = (float)((double)m[0] * p[0] + (double)m[6] * p[2] + (double)m[3] * p[1] + m[9]);
    P.y = (float)((double)m[1] * p[0] + (double)m[7] * p[2] + (double)m[4] * p[1] + m[10]);
    P.z = (float)((double)m[2] * p[0] + (double)m[8] * p[2] + (double)m[5] * p[1] + m[11]);
    Vec3 vel = F(Vec3, c, 432);
    double ux = g_kuUp.x * (double)g_ku64c938;
    float uy = (float)(g_kuUp.y * (double)g_ku64c938);
    float uz = (float)(g_kuUp.z * (double)g_ku64c938);
    Vec3 nv;
    nv.x = (float)(vel.x + ux);
    nv.y = (float)((double)uy + vel.y);
    nv.z = (float)((double)uz + vel.z);
    float mass = F(float, c, 3284);
    Vec3 Fk;
    Fk.x = (float)(ux * mass);
    Fk.y = (float)((double)uy * mass);
    Fk.z = (float)((double)uz * mass);
    float tx = (float)((double)Fk.z * P.y - (double)Fk.y * P.z);
    float ty = (float)((double)P.z * Fk.x - (double)Fk.z * P.x);
    float tz = (float)((double)Fk.y * P.x - (double)P.y * Fk.x);
    Matrix M;
    const float* R = CallThis<const Matrix*>(Ku_M1, c + 16, &M)->m;
    Vec3 w;
    CallThis(Ku_Set, &w, (float)((double)tz * R[6] + (double)ty * R[3] + (double)tx * R[0]),
             (float)((double)tz * R[7] + (double)ty * R[4] + (double)tx * R[1]),
             (float)((double)tz * R[8] + (double)ty * R[5] + (double)tx * R[2]));
    Vec3 spin = F(Vec3, c, 456), ns;
    Vec3_Add(&ns, &spin, &w);
    BYTE* body = c + 16;
    CallVirt(body, 152, &ns, 0);
    CallVirt(body, 144, &nv, 0);
    Vec3 t;
    P = *Vec3_MulMatrix(&t, &P, CallThis<const Matrix*>(Ku_M1, body, &M));
    CallThis(Ku_MulAssign, &Fk, &t, 25.0f);
    CallVirt(body, 164, &P, &Fk);
}

} // extern "C"

// === the vehicle constructors (FUN_543e00 and FUN_5443e0, each with its tail)

extern "C" void Vc_Base(); KING_ALIAS("_Vc_Base", "@Obj541940_Construct@12")
extern "C" void Vc_Body(); KING_ALIAS("_Vc_Body", "@Body58b5b0_Construct@52")
extern "C" void Vc_TypeInit(); KING_ALIAS("_Vc_TypeInit", "@Obj556670_Init@12")
extern "C" void Vc_Wheels(); KING_ALIAS("_Vc_Wheels", "@Wheel541980_Construct@8")
extern "C" void Vc_542440(); KING_ALIAS("_Vc_542440", "@Obj542440_Construct@8")
extern "C" void Vc_Player(); KING_ALIAS("_Vc_Player", "@Body53fa60_Construct@8")
extern "C" void Vc_548e70(); KING_ALIAS("_Vc_548e70", "@Obj548e70_Construct@8")
extern "C" void Vc_Ramp(); KING_ALIAS("_Vc_Ramp", "@Ramp_Construct@8")
extern "C" void Vc_Flasher(); KING_ALIAS("_Vc_Flasher", "@Obj549890_Construct@8")
extern "C" void Vc_Class(); KING_ALIAS("_Vc_Class", "_CarType_Class")
extern "C" void Vc_Register(); KING_ALIAS("_Vc_Register", "@Car_Register@12")
extern "C" void Vc_556040(); KING_ALIAS("_Vc_556040", "@Car_556040@12")
extern "C" void Vc_Fatal(); KING_ALIAS("_Vc_Fatal", "_World_Fatal")
extern "C" void Vc_ForEach(); KING_ALIAS("_Vc_ForEach", "_Array_ForEach@16")
extern "C" void Vc_Zero(); KING_ALIAS("_Vc_Zero", "@Vec3_ZeroThis@8")
extern "C" void Vc_Lights(); KING_ALIAS("_Vc_Lights", "@Copter_FindLights@12")
extern "C" void Vc_CopterReset(); KING_ALIAS("_Vc_CopterReset", "@Copter5615f0_Reset@8")
KING_VAR(BYTE, g_vc6ef1f8, 6ef1f8)
KING_VAR(BYTE, g_vcTypes, 6d20f0)
KING_VAR(BYTE, g_vcFrames, 6d2258)
KING_VAR(BYTE, g_vcSpecs, 697b30)
KING_VAR(BYTE, g_vcMass, 6978c8)
KING_VAR(BYTE, g_vcIdentity, 7025f0)
KING_VAR(float, g_vc64f50c, 64f50c)
KING_VAR(AsmLabel, g_vc64f654, 64f654)
KING_VAR(AsmLabel, g_vc64f568, 64f568)
KING_VAR(AsmLabel, g_vc64f51c, 64f51c)
KING_VAR(AsmLabel, g_vc64f518, 64f518)

// n matrices from p: zeroed rows (by Vec3_ZeroThis), then the identity
static void Vc_Matrices(BYTE* p, int n)
{
    for (int i = 0; i < n; i++, p += 48)
    {
        CallStd(Vc_ForEach, p, 12, 4, Vc_Zero);
        memcpy(p, &g_vcIdentity, 48);
    }
}

// one matrix whose four rows are zeroed one by one first
static void Vc_Matrix4(BYTE* p)
{
    for (int i = 0; i < 4; i++)
        CallThis(Vc_Zero, p + 12 * i);
    memcpy(p, &g_vcIdentity, 48);
}

// the parts both constructors build after the body: the type state, wheels, the matrix and vector tables, the
// player part (+20636), eight ramps, the flasher and the vtables
static void Vc_Parts(BYTE* v, int type)
{
    CallThis(Vc_TypeInit, v + 9556, type);
    Vc_Matrices(v + 9652, 8);
    Vc_Matrix4(v + 10036);
    CallThis(Vc_Wheels, v + 10084);
    CallThis(Vc_542440, v + 10432);
    memset(v + 10616, 0, 96);
    memset(v + 10712, 0, 96);
    F(int, v, 10812) = 0;
    F(int, v, 10816) = 0;
    F(int, v, 10820) = 0;
    memset(v + 11540, 0, 600);
    memset(v + 12140, 0, 600);
    Vc_Matrices(v + 12740, 50);
    Vc_Matrices(v + 15140, 50);
    memset(v + 17540, 0, 600);
    Vc_Matrix4(v + 20212);
    memset(v + 20332, 0, 48);
    F(int, v, 20548) = 0;
    F(int, v, 20552) = 0;
    F(int, v, 20556) = 0;
    CallThis(Vc_Zero, v + 20560);
    CallThis(Vc_Zero, v + 20572);
    CallThis(Vc_Zero, v + 20584);
    CallThis(Vc_Player, v + 20636);
    CallThis(Vc_548e70, v + 20960);
    F(int, v, 21040) = 0;
    F(int, v, 21044) = 0;
    F(int, v, 21048) = 0;
    for (int i = 0; i < 8; i++)
        CallThis(Vc_Ramp, v + 21152 + 52 * i);
    CallThis(Vc_Flasher, v + 21568);
    memset(v + 21652, 0, 132);
    F(int, v, 21876) = 0;
    F(int, v, 21872) = 0;
    F(void*, v, 0) = &g_vc64f654;
    F(void*, v, 16) = &g_vc64f568;
    F(void*, v, 272) = &g_vc64f51c;
    F(void*, v, 392) = &g_vc64f518;
}

extern "C" {

// [FUN_543e00..$L_5442a4] vehicle v of world w, cab type c (+2252 its type, +244, +16 its frame), player p and a:
// the body (the type record 6d20f0, its frame 6d2258 +392 times the cab frame) and the parts; then the cab links
// (+21600), the type class, its registration in w, the cab kind (+10092: 1 for light types, 2 for bmwM5) and
// the frame copied to +20640
BYTE* __fastcall Vehicle_Construct(BYTE* v, void*, int w, BYTE* c, int p, int a)
{
    CallThis(Vc_Base, v, &g_vc6ef1f8);
    int type = F(int, c, 2252);
    BYTE* rec = F(BYTE*, &g_vcTypes + type * 1488, 0);
    const char* name = F(const char*, rec, 12);
    int r32 = F(int, rec, 32);
    BYTE M0[48], M1[48], M[48];
    memcpy(M0, c + 16, 48);
    memcpy(M1, c + 16, 48);
    const BYTE* spec = &g_vcSpecs + type * 2416;
    Matrix_Multiply((Matrix*)M, (const Matrix*)(&g_vcFrames + type * 1488 + 392), (const Matrix*)M1);
    CallThis(Vc_Body, v + 16, w, F(int, c, 244), rec, M, M0, r32, 100, F(int, spec, 152), F(int, spec, 156), name, a);
    Vc_Parts(v, F(int, c, 2252));
    if (!c)
        CallC(Vc_Fatal, "_Vehicle::_Vehicle() cv==NULL");
    F(int, v, 20636) = p;
    F(int, v, 10420) = F(int, c, 2252);
    F(BYTE*, v, 21600) = c;
    F(int, v, 21604) = 0;
    F(int, v, 9548) = CallC<int>(Vc_Class, F(int, c, 2252));
    CallThis(Vc_Register, v, w);
    BYTE* cab = F(BYTE*, v, 21600);
    F(BYTE*, cab, 10708) = v ? v + 16 : 0;
    int t = F(int, v, 10420);
    if (F(float, &g_vcMass + t * 2416, 0) < g_vc64f50c)
        F(int, F(BYTE*, v, 21600), 10092) = 1;
    else
        F(int, F(BYTE*, v, 21600), 10092) = 0;
    if (!_strnicmp(F(const char*, F(BYTE*, &g_vcTypes + F(int, v, 10420) * 1488, 0), 12), "bmwM5", 4))
        F(int, F(BYTE*, v, 21600), 10092) = 2;
    F(int, v, 10924) = 1;
    F(int, v, 20808) = F(int, c, 244);
    memcpy(v + 20640, c + 16, 48);
    CallThis(Vc_556040, v, 1);
    return v;
}

// [FUN_5443e0..$L_544834] vehicle v of world w for helicopter h (its frame at +0, +96) with record rec, type and a:
// the body (the identity times the copter frame) and the parts; then the copter lights and reset, its link (+3480),
// the frame copied to +20640, class 5, its registration in w, the flags +1212 / +20900 and the wheel slots cleared
BYTE* __fastcall Vehicle_ConstructCopter(BYTE* v, void*, int w, BYTE* h, BYTE* rec, int type, int a)
{
    CallThis(Vc_Base, v, &g_vc6ef1f8);
    const char* name = F(const char*, rec, 12);
    int r32 = F(int, rec, 32);
    BYTE M0[48], M1[48], M[48];
    memcpy(M1, h, 48);
    memcpy(M0, h, 48);
    const BYTE* spec = &g_vcSpecs + type * 2416;
    Matrix_Multiply((Matrix*)M, (const Matrix*)&g_vcIdentity, (const Matrix*)M0);
    CallThis(Vc_Body, v + 16, w, F(int, h, 96), rec, M, M1, r32, 100, F(int, spec, 152), F(int, spec, 156), name, a);
    Vc_Parts(v, type);
    F(BYTE*, v, 21604) = h;
    if (!h)
        CallC(Vc_Fatal, "_Vehicle::_Vehicle()  hc==NULL");
    CallThis(Vc_Lights, F(BYTE*, v, 21604), F(int, F(BYTE*, v, 48), 64));
    CallThis(Vc_CopterReset, F(BYTE*, v, 21604));
    F(BYTE*, F(BYTE*, v, 21604), 3480) = v ? v + 16 : 0;
    F(int, v, 20808) = F(int, h, 96);
    memcpy(v + 20640, h, 48);
    F(int, v, 21600) = 0;
    F(int, v, 10420) = type;
    CallC(Vc_Class, type);
    F(int, v, 9548) = 5;
    CallThis(Vc_Register, v, w);
    F(DWORD, v, 1212) |= 0x100;
    F(DWORD, v, 20900) &= 0xFFFFFE7F;
    F(int, v, 10924) = 5;
    for (int i = 0; i < 4; i++)
    {
        F(int, v, 20400 + 4 * i) = 0;
        F(int, v, 20456 + 8 * i) = 0;
        F(int, v, 20460 + 8 * i) = 0;
    }
    return v;
}

} // extern "C"

// === reading the controller definitions, kmjin.ini (FUN_5301a0 with its tails up to $L_530751)

#include <stdlib.h>

extern "C" void Kj_New(); KING_ALIAS("_Kj_New", "@Controls530120_Construct@4")
extern "C" void Kj_Fatal(); KING_ALIAS("_Kj_Fatal", "_World_Fatal")
extern "C" void Kj_ReadInts(); KING_ALIAS("_Kj_ReadInts", "_Carat_ReadFill2")
extern "C" void __cdecl Ini_ReadSections(const char* none, char* buf, int size, const char* file);
KING_VAR(BYTE*, g_kjHead, 6d20dc)
KING_VAR(BYTE, g_kjTemplate, 6cf4d8)
KING_VAR(BYTE, g_kjAxisA, 6d1bb0)
KING_VAR(BYTE, g_kjAxisB, 6d1bb8)
KING_VAR(int, g_kj696890, 696890)
KING_VAR(char, g_kjNames, 6ef2d4)
KING_VAR(char, g_kjNamesEnd, 6ef856)
KING_VAR(char, g_kjKeys, 6f20d0)
KING_VAR(char, g_kjKeysEnd, 6f2670)
KING_VAR(char, g_kjAxes, 6f2738)
KING_VAR(char, g_kjAxesEnd, 6f2c38)
KING_VAR(BYTE, g_kjCodes, 6d1948)

static const char s_kjFile[] = ".\\kmjin.ini";
static const char s_kjNone[] = "$$$.$$$";

// a new controller record (732 bytes) from the template, named name
static BYTE* Kj_Make(const char* name)
{
    BYTE* o = (BYTE*)operator new(732);
    if (o)
        o = CallThis<BYTE*>(Kj_New, o);
    memcpy(o + 36, &g_kjTemplate, 174 * 4);
    if (name)
        strcpy((char*)o + 36, name);
    return o;
}

// name n begins with key k and ends there or at a space
static bool Kj_Names(const char* n, const char* k, size_t kl)
{
    return !strncmp(n, k, kl) && (n[kl] == ' ' || !n[kl]);
}

// the axis groups (4 names of 40 bytes, 160 a group) matching val: for each, set(o, group's axis, the sub index)
template <class Fn> static void Kj_EachAxis(const char* val, Fn set)
{
    BYTE* axis = &g_kjAxisA;
    for (const char* g = &g_kjAxes; g < &g_kjAxesEnd; g += 160, axis += 12)
        for (int k = 0; k < 4; k++)
            if (!strcmp(g + 40 * k, val))
            {
                set(axis, k);
                break;
            }
}

extern "C" {

// [FUN_5301a0..$L_530751] the controller list (6d20dc) rebuilt: the keyboard, a one-axis and a two-axes joystick
// (French names for language 3), then one per section of kmjin.ini: its type, its four numbers (axes / buttons)
// and its keys, each either a button name (6f20d0, from code 259) or an axis name binding the first four entries
// (steering, gas, brake, clutch)
void __cdecl Controls_ReadDefFile()
{
    if (BYTE* h = g_kjHead)
    {
        BYTE* o = h - 4;
        while (o)
        {
            BYTE* cur = o;
            BYTE* nx = F(BYTE*, o, 8);
            o = nx ? nx - 4 : 0;
            if (cur)
                CallVirt(cur, 0, 1);
        }
    }
    g_kjHead = 0;
    Kj_Make(0);
    BYTE* o2 = Kj_Make(g_kj696890 == 3 ? "Manette axe unique" : "One Axis Joystick");
    F(BYTE*, o2, 676) = &g_kjAxisA;
    F(int, o2, 680) = 0;
    F(int, o2, 684) = 0;
    F(int, o2, 712) = 0;
    F(int, o2, 716) = 1;
    F(int, o2, 20) = 1;
    BYTE* o3 = Kj_Make(g_kj696890 == 3 ? "Manette deux axes" : "Two Axes Joystick");
    F(BYTE*, o3, 676) = &g_kjAxisA;
    F(BYTE*, o3, 680) = &g_kjAxisB + 4;
    F(BYTE*, o3, 684) = &g_kjAxisB + 4;
    F(int, o3, 712) = 1;
    F(int, o3, 716) = 1;
    F(int, o3, 20) = 2;
    FILE* f = fopen(s_kjFile, "rt");
    if (!f)
    {
        char cwd[200], msg[300];
        GetCurrentDirectoryA(200, cwd);
        sprintf(msg, "\"%s\" was not found in %s", s_kjFile, cwd);
        CallC(Kj_Fatal, msg);
    }
    fclose(f);
    static char sections[1300], keys[4700];
    Ini_ReadSections(s_kjNone, sections, 1300, s_kjFile);
    if (!strcmp(s_kjNone, sections))
        return;
    int soff = 0;
    for (int si = 0; si < 13; si++)
    {
        const char* sec = sections + soff;
        if (!*sec)
            return;
        char section[200], key[200], val[200];
        strcpy(section, sec);
        BYTE* o = Kj_Make(section);
        GetPrivateProfileStringA(section, "type", s_kjNone, val, 20, s_kjFile);
        if (!strcmp(val, s_kjNone))
            CallC(Kj_Fatal, "read_defFile_kmjIn(): #1");
        char* end;
        F(int, o, 16) = strtol(val, &end, 0);
        int nums[4] = {0, 0, 0, 0};
        CallC(Kj_ReadInts, section, "nums", 0, nums, s_kjFile, 4);
        if (!nums[0] && !nums[1] && !nums[2] && !nums[3])
            CallC(Kj_Fatal, "read_defFile_kmjIn(): #2");
        F(int, o, 20) = nums[0];
        F(int, o, 24) = nums[1];
        F(int, o, 28) = nums[2];
        F(int, o, 32) = nums[3];
        if (nums[0] > 1)
        {
            F(int, o, 712) = 1;
            F(BYTE*, o, 680) = &g_kjAxisB + 4;
            F(BYTE*, o, 684) = &g_kjAxisB + 4;
        }
        if (nums[0] > 0)
        {
            F(BYTE*, o, 676) = &g_kjAxisA;
            F(int, o, 716) = 1;
        }
        GetPrivateProfileStringA(section, 0, s_kjNone, keys, 4700, s_kjFile);
        if (strcmp(s_kjNone, keys))
        {
            int koff = 0;
            for (int ki = 0; ki < 47; ki++)
            {
                const char* k = keys + koff;
                if (!*k)
                    break;
                strcpy(key, k);
                GetPrivateProfileStringA(section, key, s_kjNone, val, 100, s_kjFile);
                if (!strcmp(val, s_kjNone))
                {
                    sprintf(val, "read_defFile_kmjIn: undefined [%s]%s ", section, key);
                    CallC(Kj_Fatal, val);
                }
                size_t kl = strlen(key);
                bool done = false;
                BYTE** slot = (BYTE**)(o + 488);
                for (const char* n = &g_kjNames; n < &g_kjNamesEnd && !done; n += 30, slot++)
                {
                    if (!Kj_Names(n, key, kl))
                        continue;
                    int code = 259;
                    for (const char* b = &g_kjKeys; b < &g_kjKeysEnd; b += 40, code++)
                        if (!strcmp(b, val))
                        {
                            *slot = &g_kjCodes + code * 2;
                            done = true;
                            break;
                        }
                }
                if (!done)
                {
                    const char* N = &g_kjNames;
                    if (Kj_Names(N, key, kl))
                        Kj_EachAxis(val, [o](BYTE* axis, int k) {
                            F(BYTE*, o, 684) = axis;
                            F(int, o, 708) = (k & 1) ? 0 : 1;
                            if (F(BYTE*, o, 684) != F(BYTE*, o, 680) && F(BYTE*, o, 680))
                                F(int, o, 724) = 1;
                            F(int, o, 712) = 1;
                        });
                    else if (Kj_Names(N + 30, key, kl))
                        Kj_EachAxis(val, [o](BYTE* axis, int k) {
                            F(BYTE*, o, 676) = axis;
                            F(int, o, 700) = (k & 1) ? 1 : 0;
                            F(int, o, 716) = 1;
                        });
                    else if (Kj_Names(N + 60, key, kl))
                        Kj_EachAxis(val, [o](BYTE* axis, int k) {
                            F(BYTE*, o, 676) = axis;
                            F(int, o, 700) = (k & 1) ? 0 : 1;
                            F(int, o, 716) = 1;
                        });
                    else if (Kj_Names(N + 90, key, kl))
                        Kj_EachAxis(val, [o](BYTE* axis, int k) {
                            F(BYTE*, o, 680) = axis;
                            F(int, o, 704) = (k & 1) ? 1 : 0;
                            if (F(BYTE*, o, 684) != F(BYTE*, o, 680) && F(BYTE*, o, 684))
                                F(int, o, 724) = 1;
                            F(int, o, 712) = 1;
                        });
                }
                koff += (int)strlen(key) + 1;
            }
        }
        soff += (int)strlen(section) + 1;
    }
}

} // extern "C"

// === the viewer attaching a vehicle (FUN_531cc0 with its tail $L_531ce8)

extern "C" void Av_Detach(); KING_ALIAS("_Av_Detach", "@Player_DetachCar@8")
extern "C" void Av_Fatal(); KING_ALIAS("_Av_Fatal", "_World_Fatal")
extern "C" void Av_Auto(); KING_ALIAS("_Av_Auto", "@Car_SetAuto@12")
extern "C" void Av_53b130(); KING_ALIAS("_Av_53b130", "@King_53b130@8")
extern "C" void Av_RotX(); KING_ALIAS("_Av_RotX", "_Matrix_RotationX")
extern "C" void Av_WorldReset(); KING_ALIAS("_Av_WorldReset", "@WorldObj_Reset@8")
extern "C" void Av_Dash(); KING_ALIAS("_Av_Dash", "@Dash_Setup@12")
extern "C" void Av_LogOpen(); KING_ALIAS("_Av_LogOpen", "_WarnLog_Open")
extern "C" void Av_LogsOn(); KING_ALIAS("_Av_LogsOn", "_VehLogs_Enable")
extern "C" void Av_Clock(); KING_ALIAS("_Av_Clock", "_Clock_Text")
extern "C" void Av_Log(); KING_ALIAS("_Av_Log", "_VehLog_Value")
extern "C" void Av_Joy(); KING_ALIAS("_Av_Joy", "_Joy_SendCommand")
extern "C" void Av_Cursor(); KING_ALIAS("_Av_Cursor", "_Input_SetCursorPos")
extern "C" void Av_M1(); KING_ALIAS("_Av_M1", "@SceneObj_Matrix1@12")
extern "C" void Av_Get800(); KING_ALIAS("_Av_Get800", "@SceneObj_Get800@8")
extern "C" void Av_Fill(); KING_ALIAS("_Av_Fill", "@Arrays_Fill@16")
extern "C" void Av_Time(); KING_ALIAS("_Av_Time", "_Game_Time")
extern "C" void Av_Unless(); KING_ALIAS("_Av_Unless", "_Player_DetachCarUnless")
KING_VAR(BYTE*, g_av6cec6c, 6cec6c)
KING_VAR(BYTE*, g_av6cec74, 6cec74)
KING_VAR(BYTE, g_avSpecs, 697914)
KING_VAR(BYTE, g_av6f6968, 6f6968)
KING_VAR(int, g_av69784c, 69784c)
KING_VAR(int, g_av696c70, 696c70)
KING_VAR(int, g_av672d80, 672d80)
KING_VAR(int, g_av696c6c, 696c6c)
KING_VAR(int, g_av696c74, 696c74)
KING_VAR(Vec3, g_avDown, 702640)
KING_VAR(Vec3, g_avAhead, 702620)
KING_VAR(int, g_av696850, 696850)
KING_VAR(int, g_av696854, 696854)
KING_VAR(int, g_av6d1b98, 6d1b98)
KING_VAR(int, g_av696d10, 696d10)
KING_VAR(int, g_av696d14, 696d14)
KING_VAR(float, g_av6d20d8, 6d20d8)
KING_VAR(double, g_av64b5a0, 64b5a0)
KING_VAR(double, g_av64b668, 64b668)
KING_VAR(float, g_av64d2f8, 64d2f8)
KING_VAR(double, g_av64da88, 64da88)
KING_VAR(float, g_av64c250, 64c250)
KING_VAR(float, g_av64c2f8, 64c2f8)

static double Av_Now()
{
    return CallC<double>(Av_Time);
}

// a value sunk by how far h sits above 64b668 (by 64da88 per unit, at most 1 from 64d2f8 up)
static float Av_Sink(float v, float h)
{
    if (h < g_av64d2f8)
        return (float)(v - (h - g_av64b668) * g_av64da88);
    return (float)((double)v - g_av64c250);
}

extern "C" {

// [FUN_531cc0..$L_531ce8] viewer s attaches vehicle v (+616): its detach hook and flags; with a cab, the cab's
// type (+104), its head frame (+52) and seat (+100), its view mode, steering and gas settings, the gearbox kind
// (+9984 by the game mode) and the dashboard; else the vehicle's type; then the cab frames (+472 tilt, +520 / +568
// head offset, lowered when it would poke through the roof), the world reset, the dash, an optional log line, the
// view limits (+120 .. +196), the force feedback, the start frame and name, five timers, the cursor and the clocks
void __fastcall Viewer_AttachVehicle(BYTE* s, void*, BYTE* v)
{
    CallThis(Av_Detach, s);
    if (!v)
        CallC(Av_Fatal, "_Viewer::attach_vehicle(): veh==NULL");
    F(BYTE*, s, 616) = v;
    CallVirt(v, 108, 1);
    F(void*, F(BYTE*, s, 616), 20956) = (void*)Av_Unless;
    F(DWORD, F(BYTE*, s, 616), 20900) |= 52;
    if (F(BYTE*, v, 21600))
    {
        BYTE* cab = F(BYTE*, F(BYTE*, s, 616), 21600);
        F(int, s, 104) = F(int, cab, 2252);
        memcpy(s + 52, cab + 6888, 48);
        F(int, s, 100) = F(int, cab, 6884);
        CallVirt(F(BYTE*, s, 616), 112, 3);
        cab = F(BYTE*, F(BYTE*, s, 616), 21600);
        F(int, cab, 10088) = F(int, g_av6cec6c, 1304) == 0;
        F(float, cab, 10020) = (float)(F(short, g_av6cec74, 388) * g_av64b5a0);
        F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 10024) = (float)(F(short, g_av6cec74, 390) * g_av64b5a0);
        CallThis(Av_Auto, F(BYTE*, F(BYTE*, s, 616), 21600), (int)F(short, g_av6cec74, 394));
        int mode = F(int, g_av6cec6c, 1316);
        BYTE gear;
        if (!mode)
            gear = 2;
        else if (mode == 2)
            gear = 0;
        else if ((F(BYTE, s, 1404) & 1) && F(BYTE*, s, 1632) && F(int, F(BYTE*, s, 1632), 80))
            gear = 0;
        else
            gear = 2;
        F(BYTE, F(BYTE*, F(BYTE*, s, 616), 21600), 9984) = gear;
        F(float, F(BYTE*, F(BYTE*, s, 616), 21600), 10084) = 0.5f;
        CallThis(Av_53b130, s);
        F(int, s, 116) = F(int, F(BYTE*, F(BYTE*, s, 616), 21600), 15440);
    }
    else
        F(int, s, 104) = F(int, F(BYTE*, s, 616), 10420);
    F(int, s, 628) = 0;
    const BYTE* spec = &g_avSpecs + F(int, s, 104) * 2416;
    Matrix t;
    memcpy(s + 472, CallC<const Matrix*>(Av_RotX, &t, F(int, spec, 236)), 48);
    F(int, s, 200) = F(int, spec, 236);
    const Matrix* T = Matrix_Translation(&t, F(Vec3, spec, 224));
    memcpy(s + 520, T, 48);
    memcpy(s + 568, T, 48);
    float m[12];
    memcpy(m, F(BYTE*, s, 616) + 708, 48);
    float h = (float)((double)m[8] * F(float, s, 564) + (double)m[5] * F(float, s, 560) + (double)m[2] * F(float, s, 556)
                      + m[11] - F(float, F(BYTE*, s, 616), 21048));
    if (h > g_av64b668)
        F(float, s, 612) = Av_Sink(F(float, s, 612), h);
    if (h > g_av64b668)
        F(float, s, 564) = Av_Sink(F(float, s, 564), h);
    CallThis(Av_WorldReset, &g_av6f6968);
    g_av69784c = 0;
    CallThis(Av_Dash, s, F(BYTE*, s, 616));
    if (g_av696c70 || g_av672d80 || g_av696c6c || g_av696c74)
    {
        CallC(Av_LogOpen);
        CallC(Av_LogsOn);
        char clock[100], line[160];
        CallC(Av_Clock, clock);
        sprintf(line, "\n----------Vehicle ATTACHED--------%s-------------", clock);
        CallC(Av_Log, line);
    }
    F(float, s, 48) = (float)F(double, F(BYTE*, s, 32), 2968);
    for (int k = 0; k < 4; k++)
    {
        F(int, s, 120 + 4 * k) = 0;
        F(float, s, 136 + 4 * k) = 1.0f;
    }
    F(DWORD, s, 152) = 0x40400000;
    F(DWORD, s, 168) = 0x3F800000;
    F(DWORD, s, 184) = 0x3D75C28F;
    F(DWORD, s, 156) = 0x40A00000;
    F(DWORD, s, 172) = 0x41700000;
    F(DWORD, s, 188) = 0x3DA3D70A;
    F(DWORD, s, 160) = 0x41A00000;
    F(DWORD, s, 176) = 0x41200000;
    F(DWORD, s, 192) = 0x3DF5C28F;
    F(DWORD, s, 164) = 0x42480000;
    F(DWORD, s, 180) = 0x41200000;
    F(DWORD, s, 196) = 0x3E19999A;
    F(int, s, 112) = 0;
    CallC(Av_Joy, 4);
    F(Vec3, s, 216) = g_avDown;
    F(Vec3, s, 204) = F(Vec3, CallThis<BYTE*>(Av_M1, F(BYTE*, s, 616) + 16, &t), 36);
    strcpy((char*)s + 228, F(const char*, CallThis<BYTE*>(Av_Get800, F(BYTE*, s, 616) + 16), 12));
    static const int timers[5] = {344, 280, 312, 376, 408};
    for (int k = 0; k < 5; k++)
    {
        float now = (float)Av_Now();
        CallThis(Av_Fill, s + timers[k], now, 0);
    }
    CallC(Av_Cursor, g_av696850 / 2, g_av696854 / 2);
    g_av6d1b98 = 0;
    g_av696d10 = 0;
    g_av696d14 = 0;
    F(int, s, 1408) = 0;
    F(int, s, 1412) = 0;
    F(int, s, 1416) = 0;
    F(int, s, 1420) = 0;
    memcpy(s + 1424, CallThis<const Matrix*>(Av_M1, F(BYTE*, s, 616) + 16, &t), 48);
    Vec3 back = {-g_avAhead.x, -g_avAhead.y, -g_avAhead.z};
    F(Vec3, s, 456) = back;
    F(float, s, 468) = (float)(Av_Now() - g_av64c2f8);
    g_av6d20d8 = (float)Av_Now();
}

} // extern "C"

// === the tractor moving with its semitrailer (FUN_55d140 with its tails up to $L_55d59d)

extern "C" void Tm_Mode(); KING_ALIAS("_Tm_Mode", "@Mode3976_Set@12")
extern "C" void Tm_Dt(); KING_ALIAS("_Tm_Dt", "_Game_FrameTime")
extern "C" void Tm_Fatal(); KING_ALIAS("_Tm_Fatal", "_World_Fatal")
extern "C" void Tm_Impulse(); KING_ALIAS("_Tm_Impulse", "@Car_HitchImpulse@8")
extern "C" void Tm_Of(); KING_ALIAS("_Tm_Of", "_Moving_Of")

// the hitch point of a moved before its step: its position (+3284 / +3304) offset by b's
static void Tm_Offset(BYTE* a, const BYTE* b, const BYTE* base)
{
    F(float, a, 3284) = F(float, base, 412) + F(float, b, 412);
    for (int k = 0; k < 3; k++)
        F(float, a, 3304 + 4 * k) = F(float, base, 420 + 4 * k) + F(float, b, 420 + 4 * k);
}

static void Tm_Restore(BYTE* a, const BYTE* base)
{
    F(float, a, 3284) = F(float, base, 412);
    memcpy(a + 3304, base + 420, 12);
}

extern "C" {

// [FUN_55d140..$L_55d59d] tractor s (its item +16) and semitrailer t (+21880, item +16) moved together over the
// frame time: both step their sub-steps in turn, the one free of a pending event (+1688) first, by the smaller
// step (+1472), the hitch impulse applied between; fatal when either cannot move or is in slave mode
void __fastcall Tractor_Move(BYTE* s, void*, int a1, int a2)
{
    CallThis(Tm_Mode, F(BYTE*, s, 21880) + 16, F(int, s, 3992));
    float dt = (float)CallC<double>(Tm_Dt);
    BYTE* e = s + 16;
    int r1 = CallVirt<int>(e, 84, dt, &a1);
    int r2 = CallVirt<int>(F(BYTE*, s, 21880) + 16, 84, dt, &a2);
    if (r1 && r2)
    {
        int c1 = 0, c2 = 0;
        if (!CallVirt<int>(e, 96, a1, &c1))
            CallC(Tm_Fatal, "_VEH_Tractor::Move() : !MoveItem_1()");
        if (!CallVirt<int>(F(BYTE*, s, 21880) + 16, 96, a2, &c2))
            CallC(Tm_Fatal, "_VEH_Tractor::Move() : !semitr->MoveItem_1()");
        int b = 1, x = 1;
        while (b || x)
        {
            CallVirt(e, 100, &c1);
            CallVirt(F(BYTE*, s, 21880) + 16, 100, &c2);
            CallVirt(e, 104, 1.0f);
            CallVirt(F(BYTE*, s, 21880) + 16, 104, 1.0f);
            CallVirt(e, 108, a1);
            CallVirt(F(BYTE*, s, 21880) + 16, 108, a2);
            BYTE* t = F(BYTE*, s, 21880);
            int ps = F(int, s, 1688), pt = F(int, t, 1688);
            if (!ps && !pt)
            {
                b = CallVirt<int>(e, 112, &c1);
                x = CallVirt<int>(F(BYTE*, s, 21880) + 16, 112, &c2);
                continue;
            }
            F(int, s, 21884) = 1;
            float step = F(float, t, 1472) < F(float, s, 1472) ? F(float, t, 1472) : F(float, s, 1472);
            bool selfFirst;
            if (ps && pt)
                selfFirst = F(float, s, 1472) < F(float, t, 1472);
            else
                selfFirst = ps && !pt;
            if (!selfFirst)
            {
                CallVirt(e, 104, step);
                t = F(BYTE*, s, 21880);
                if (F(int, t, 1044))
                    Tm_Offset(t, s, t);
                CallVirt(F(BYTE*, s, 21880) + 16, 120, a2, step);
                t = F(BYTE*, s, 21880);
                Tm_Restore(t, t);
                CallThis(Tm_Impulse, s);
                CallVirt(e, 116);
                CallVirt(F(BYTE*, s, 21880) + 16, 124, a2);
                b = 0;
                x = CallVirt<int>(F(BYTE*, s, 21880) + 16, 128);
            }
            else
            {
                CallVirt(t + 16, 104, step);
                if (CallC<int>(Tm_Of, F(int, s, 1044)))
                    Tm_Offset(s, s, F(BYTE*, s, 21880));
                CallVirt(e, 120, a1, step);
                Tm_Restore(s, s);
                CallThis(Tm_Impulse, s);
                CallVirt(F(BYTE*, s, 21880) + 16, 116);
                CallVirt(e, 124, a1);
                b = CallVirt<int>(e, 128);
                x = 0;
            }
        }
        CallVirt(e, 132, &c1);
        CallVirt(F(BYTE*, s, 21880) + 16, 132, &c2);
        F(int, s, 1456) = 0;
        F(int, F(BYTE*, s, 21880), 1456) = 0;
    }
    if (F(int, s, 388) || F(int, F(BYTE*, s, 21880), 388))
        CallC(Tm_Fatal, "_VEH_Tractor::Move() :this and/or semitrailer are in slavemode");
    CallVirt(e, 88, a1);
    CallVirt(F(BYTE*, s, 21880) + 16, 88, a2);
}

} // extern "C"

// === a car's frame, speed and spin kept sane (FUN_5491f0 with its tail $L_549275)

extern "C" void Sn_M1(); KING_ALIAS("_Sn_M1", "@SceneObj_Matrix1@12")
extern "C" void Sn_M548(); KING_ALIAS("_Sn_M548", "@SceneObj_Matrix548@12")
extern "C" void Sn_Vel(); KING_ALIAS("_Sn_Vel", "@SceneObj_LocalVelocity@12")
extern "C" void Sn_Out(); KING_ALIAS("_Sn_Out", "_Vec3_OutOfBox")
extern "C" void Sn_Norm(); KING_ALIAS("_Sn_Norm", "_Vec3_Normalized")
extern "C" void Sn_Fatal(); KING_ALIAS("_Sn_Fatal", "_World_Fatal")
KING_VAR(Vec3, g_snZero, 702650)
KING_VAR(float, g_sn64c448, 64c448)
KING_VAR(float, g_sn64c450, 64c450)
KING_VAR(char, g_snBadPos, 67a158)
KING_VAR(char, g_snV, 67a150)
KING_VAR(char, g_snW, 67a148)

static bool Sn_OutOf(const Vec3* v, float box)
{
    return CallC<int>(Sn_Out, v, box) != 0;
}

// v when within lim (else zero) as a direction scaled by k
static Vec3 Sn_Clamp(const Vec3& v, float lim, float k)
{
    if (Sn_OutOf(&v, lim))
        return g_snZero;
    Vec3 t;
    const float* n = &CallC<const Vec3*>(Sn_Norm, &t, &v)->x;
    Vec3 r = {(float)(n[0] * k), (float)(n[1] * k), (float)(n[2] * k)};
    return r;
}

extern "C" {

// [FUN_5491f0..$L_549275] car c (its item +16) reset: its frame (or the +548 one when far off; fatal when both
// are), its speed (+432, else +468) and spin (+444, else the local velocity) clipped, all set again
int __cdecl Car_ResetSane(BYTE* c)
{
    BYTE* it = c + 16;
    Matrix m;
    CallThis(Sn_M1, it, &m);
    if (Sn_OutOf((const Vec3*)&m.m[9], 15000.0f))
    {
        memcpy(&m, CallThis<const Matrix*>(Sn_M548, it, &m), 48);
        if (Sn_OutOf((const Vec3*)&m.m[9], 15000.0f))
        {
            char buf[100];
            sprintf(buf, &g_snBadPos);
            CallC(Sn_Fatal, buf);
        }
    }
    int e = F(int, c, 820);
    Vec3 v = F(Vec3, c, 432);
    if (Sn_OutOf(&v, 100.0f))
    {
        v = F(Vec3, c, 468);
        if (Sn_OutOf(&v, 100.0f))
            v = Sn_Clamp(v, 100000.0f, g_sn64c448);
    }
    Vec3 w = F(Vec3, c, 444);
    if (Sn_OutOf(&w, 30.0f) && Sn_OutOf(&w, 100000.0f))
    {
        Matrix t;
        w = *CallThis<const Vec3*>(Sn_Vel, it, &t);
        if (Sn_OutOf(&w, 30.0f))
            w = Sn_Clamp(w, 10000.0f, g_sn64c450);
    }
    CallVirt(it, 140, e, &m);
    CallVirt(it, 144, &v, &g_snV);
    CallVirt(it, 148, &w, &g_snW);
    return 0;
}

} // extern "C"

// === the section names of an ini file (FUN_52fa70)

extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);

// the next character of f, -1 at its end
static int Ini_Get(FILE* f)
{
    unsigned char c;
    return fread(&c, 1, 1, f) == 1 ? c : -1;
}

extern "C" {

// [FUN_52fa70] buf (size bytes) filled with the names of file's [sections], each ended by 0, the list by another 0;
// read line by line (a last line cut by the end of the file is not read); buf untouched when file does not open
void __cdecl Ini_ReadSections(const char*, char* buf, int size, const char* file)
{
    FILE* f = fopen(file, "rt");
    if (!f)
        return;
    int pos = 0;
    char line[300];
    for (;;)
    {
        int n = 0, ch = 0;
        while (n < 299 && (ch = Ini_Get(f)) != -1 && ch != '\n')
            line[n++] = (char)ch;
        line[n] = 0;
        if (ch == -1)
            break;
        int d = 0, e = 0;
        while (line[e] && line[e] != '[')
            e++;
        if (line[e] == '[')
            d = e;
        while (line[e] && line[e] != ']')
            e++;
        if (line[e] != ']' || !e)
            continue;
        if (e - d + pos + 1 >= size)
            break;
        if (d + 1 < e)
        {
            memcpy(buf + pos, line + d + 1, e - d - 1);
            pos += e - d - 1;
        }
        buf[pos++] = 0;
    }
    buf[pos] = 0;
    fclose(f);
}

} // extern "C"

// === the car's settings read (FUN_501630 with its tail $L_501702) and a three-way setting switched (FUN_4eb4a0
//     with its tail $L_4eb4dd)

extern "C" void Cg_Settings(); KING_ALIAS("_Cg_Settings", "@Car_SetSettings@12")
extern "C" void Cg_Ramp(); KING_ALIAS("_Cg_Ramp", "@Ramp_SetPosition@12")
extern "C" void Cg_Fatal(); KING_ALIAS("_Cg_Fatal", "_World_Fatal")
KING_VAR(char, g_cgBadCount, 674914)
KING_VAR(char, g_cgBadGear, 674464)
KING_VAR(float, g_cg64c264, 64c264)

extern "C" {

// [FUN_501630..$L_501702] cab s's settings from v: the car's, then +3808 and the seven ramps (+16060 .. +16372);
// fatal unless 35 were read; their count
int __fastcall Cab_ReadSettings(BYTE* s, void*, const float* v)
{
    int k = CallThis<int>(Cg_Settings, s, v);
    F(double, s, 3808) = v[k++];
    for (int off = 16060; off <= 16372; off += 52)
        CallThis(Cg_Ramp, s + off, v[k++]);
    if (k != 35)
        CallC(Cg_Fatal, &g_cgBadCount);
    return k;
}

// [FUN_4eb4a0..$L_4eb4dd] switch s (positions +0, ends +12, +36 the blend, +40 the current, +44 the highest,
// +48 the mode) set to g (0 .. 2): mode 0 jumps (1), mode 1 starts from 0, mode 2 keeps the place between the ends
void __fastcall Switch3_Set(BYTE* s, void*, int g)
{
    if (g < 0 || g >= 3 || g > F(int, s, 44))
    {
        char buf[200];
        sprintf(buf, &g_cgBadGear, g, F(int, s, 44));
        CallC(Cg_Fatal, buf);
    }
    int c = F(int, s, 40);
    if (g == c)
        return;
    const float* p = (const float*)s;
    switch (F(int, s, 48))
    {
    case 0:
        F(int, s, 40) = g;
        F(float, s, 36) = 1.0f;
        return;
    case 1:
        F(int, s, 40) = g;
        F(float, s, 36) = 0;
        return;
    case 2:
    {
        double d = (double)p[3 + g] - p[g];
        if (d == g_cg64c264)
            break;
        double t = (((double)p[3 + c] - p[c]) * p[9] + p[c] - p[g]) / d;
        F(float, s, 36) = (float)t;
        if (t > g_cg64c250)
            F(float, s, 36) = 1.0f;
        if (F(float, s, 36) < g_cg64c264)
        {
            F(int, s, 40) = g;
            F(float, s, 36) = 0;
            return;
        }
        break;
    }
    }
    F(int, s, 40) = g;
}

} // extern "C"

// === the car's moments of inertia (FUN_542580 with its tail $L_542637)

extern "C" void In_Fatal(); KING_ALIAS("_In_Fatal", "_World_Fatal")
extern "C" void In_Hold(); KING_ALIAS("_In_Hold", "@Car_HoldValue@4")
extern "C" void In_Hold2(); KING_ALIAS("_In_Hold2", "@Car_HoldValue2@4")
extern "C" void In_Load(); KING_ALIAS("_In_Load", "@Car_HoldLoad@4")
extern "C" void In_5565d0(); KING_ALIAS("_In_5565d0", "@Car_5565d0@4")
KING_VAR(BYTE, g_inSpecs, 697db8)
KING_VAR(float, g_in64c258, 64c258)
KING_VAR(float, g_in64d2f8, 64d2f8)
KING_VAR(float, g_in64f50c, 64f50c)
KING_VAR(float, g_in64c250, 64c250)
KING_VAR(float, g_in64c264, 64c264)
KING_VAR(float, g_in64daf4, 64daf4)
KING_VAR(int, g_in6ce33c, 6ce33c)
KING_VAR(int, g_in6ce340, 6ce340)
KING_VAR(char, g_inBadSize, 679e7c)

extern "C" {

// [FUN_542580..$L_542637] car s's inertia (+420 and +3304) from its type's size (length, width scaled and doubled
// for narrow, light or effect cars, height, all at least 1), its mass (+412) and the cargo's share
void __fastcall Car_SetInertia(BYTE* s, void*)
{
    int t = F(int, s, 10420);
    const float* spec = (const float*)(&g_inSpecs + t * 2416);
    float a = spec[0];
    float c = spec[2];
    float b = (float)((double)spec[1] * g_in64c258);
    if (b < g_in64d2f8 || F(float, s, 412) < g_in64f50c || (t >= g_in6ce33c && t < g_in6ce340))
        b = (float)((double)b + b);
    if (a < g_in64c250 || b < g_in64c250 || c < g_in64c250)
        CallC(In_Fatal, &g_inBadSize);
    double hv = CallThis<double>(In_Hold, s);
    float h = (float)hv;
    double f;
    if (!(hv > g_in64c264))
        f = g_in64c250;
    else
    {
        float h2 = (float)CallThis<double>(In_Hold2, s);
        float L = (float)CallThis<double>(In_Load, s);
        double x = CallThis<double>(In_5565d0, s);
        if (h2 > h)
            h2 = h;
        f = (double)h2 / h * L / x * ((double)c - g_in64c250) + g_in64c250;
    }
    double f2 = f * f;
    double b2 = (double)b * b, a2 = (double)a * a;
    float fb2 = (float)b2, fa2 = (float)a2;
    double m = F(float, s, 412);
    float I1 = (float)((b2 + f2) * m * g_in64daf4);
    float I2 = (float)((a2 + f2) * m * g_in64daf4);
    float I3 = (float)(((double)fa2 + fb2) * m * g_in64daf4);
    F(float, s, 420) = I1;
    F(float, s, 424) = I2;
    F(float, s, 428) = I3;
    F(float, s, 3304) = I1;
    F(float, s, 3308) = I2;
    F(float, s, 3312) = I3;
}

} // extern "C"

// === a semitrailer's load on the saddle (FUN_4eec80 with its tails up to $L_4ef1f7)

extern "C" void Sl_Norm(); KING_ALIAS("_Sl_Norm", "_Vec3_Normalized")
extern "C" void Sl_Fatal(); KING_ALIAS("_Sl_Fatal", "_World_Fatal")
KING_VAR(BYTE, g_slSpecs, 697f9c)
KING_VAR(float, g_sl64d990, 64d990)
KING_VAR(double, g_sl64b670, 64b670)
KING_VAR(double, g_sl64d6b8, 64d6b8)
KING_VAR(char, g_slBadWheels, 674648)

extern "C" {

// [FUN_4eec80..$L_4ef1f7] the share of semitrailer body s's weight (+9792) on the saddle along dir, by its axles
// (+3540: 3 or 5 wheel sets; +7360, +7384, +7408 their places) and the hitch point (the type's +168 through the
// body's frame +10128); any other count is fatal (the general case after it in the asm is never reached)
double __fastcall Trailer_SaddleLoad(BYTE* s, void*, const float* dir)
{
    float n[3];
    CallC(Sl_Norm, n, dir);
    const float* h = (const float*)(&g_slSpecs + 168 + F(int, s, 2252) * 2416);
    float hy = (float)((double)F(float, s, 10132) * h[0] + (double)F(float, s, 10144) * h[1]
                       + (double)F(float, s, 10156) * h[2] + F(float, s, 10168));
    int k = F(int, s, 3540);
    if (k == 3)
    {
        double r = F(float, s, 7360);
        return r * F(float, s, 9792) * g_sl64d990 / (r - hy);
    }
    if (k == 5)
    {
        double a = (double)F(float, s, 7360) + F(float, s, 7384);
        double b = (double)F(float, s, 7408) + F(float, s, 7408);
        return g_sl64b670 / (a - b) * a * F(float, s, 9792) * g_sl64d6b8;
    }
    char buf[200];
    sprintf(buf, &g_slBadWheels, k - 1, " but not 1 or 3");
    CallC(Sl_Fatal, buf);
    return 0;
}

} // extern "C"

// === the semitrailer brought to the tractor's hitch (FUN_558a30)

extern "C" void Hp_Length(); KING_ALIAS("_Hp_Length", "_Vec3_Length")
extern "C" void Hp_SetPos(); KING_ALIAS("_Hp_SetPos", "@Body_SetPosition@12")
extern "C" void Hp_SetV88(); KING_ALIAS("_Hp_SetV88", "@Body_SetVec88@12")
extern "C" void Hp_Align(); KING_ALIAS("_Hp_Align", "@Hitch_Align@16")
extern "C" void Hp_Set(); KING_ALIAS("_Hp_Set", "@Vec3_SetThis@20")
extern "C" void Hp_MulM(); KING_ALIAS("_Hp_MulM", "_Vec3_MulMatrix")
extern "C" void Hp_Norm(); KING_ALIAS("_Hp_Norm", "_Vec3_Normalized")
extern "C" void Hp_RotBy(); KING_ALIAS("_Hp_RotBy", "_Vec3_RotateBy")
extern "C" void Hp_Cross(); KING_ALIAS("_Hp_Cross", "_Vec3_Cross")
extern "C" void Hp_Div(); KING_ALIAS("_Hp_Div", "_Vec3_DivScalar")
extern "C" void Hp_LookAt(); KING_ALIAS("_Hp_LookAt", "_Matrix_LookAt")
extern "C" void Hp_Add(); KING_ALIAS("_Hp_Add", "@Vec3_AddAssign@16")
extern "C" void Hp_Get800(); KING_ALIAS("_Hp_Get800", "@SceneObj_Get800@8")
extern "C" void Hp_Hit(); KING_ALIAS("_Hp_Hit", "_Query_HitObject")
extern "C" void Hp_Place(); KING_ALIAS("_Hp_Place", "@Body_PlaceAtFrame@20")
KING_VAR(BYTE, g_hpSpecs, 697f9c)
KING_VAR(Vec3, g_hpSide, 702620)
KING_VAR(Vec3, g_hpZero, 702650)

// p through the frame m (rows of 3, the place +9), summed x87 style: m[6] z + m[3] y + m[0] x + m[9]
static Vec3 Hp_Frame(const float* m, const float* p)
{
    return {(float)((double)m[6] * p[2] + (double)m[3] * p[1] + (double)m[0] * p[0] + m[9]),
            (float)((double)m[7] * p[2] + (double)m[4] * p[1] + (double)m[1] * p[0] + m[10]),
            (float)((double)m[8] * p[2] + (double)m[5] * p[1] + (double)m[2] * p[0] + m[11])};
}

static Vec3 Hp_SetV(double x, double y, double z)
{
    Vec3 r;
    CallThis(Hp_Set, &r, (float)x, (float)y, (float)z);
    return r;
}

extern "C" {

// [FUN_558a30] tractor s and its semitrailer t (+21880): the hitch points of both (the tractor type's +156, the
// trailer type's +168 and its +204 ahead), t's body (+21600) placed at s's body's position, turned about the axis
// between the two hitch directions by their angle and moved so the hitches meet, in the object hit there; 0
int __fastcall Tractor_PlaceTrailer(BYTE* s, void*)
{
    BYTE* t = F(BYTE*, s, 21880);
    float M[12];
    memcpy(M, s + 708, 48);
    const float* sp = (const float*)(&g_hpSpecs + F(int, s, 10420) * 2416);
    const float* h = sp + 39;
    Vec3 P1;
    P1.x = (float)((double)M[0] * h[0] + (double)M[6] * h[2] + (double)M[3] * h[1] + M[9]);
    P1.y = (float)((double)M[1] * h[0] + (double)M[7] * h[2] + (double)M[4] * h[1] + M[10]);
    P1.z = (float)((double)M[2] * h[0] + (double)M[8] * h[2] + (double)M[5] * h[1] + M[11]);
    memcpy(M, t + 708, 48);
    double L = sp[51];
    float lx = (float)(L * g_hpSide.x), ly = (float)(L * g_hpSide.y);
    const float* k = (const float*)(&g_hpSpecs + 168 + F(int, t, 10420) * 2416);
    float qx = (float)((double)lx + k[0]), qy = (float)((double)ly + k[1]);
    double qz = L * g_hpSide.z + k[2];
    Vec3 P2;
    P2.x = (float)(qz * M[6] + (double)qy * M[3] + (double)M[0] * qx + M[9]);
    P2.y = (float)(qz * M[7] + (double)qy * M[4] + (double)M[1] * qx + M[10]);
    P2.z = (float)(qz * M[8] + (double)qy * M[5] + (double)M[2] * qx + M[11]);
    float len = (float)CallC<double>(Hp_Length, &P2);
    BYTE* cab = F(BYTE*, s, 21600);
    BYTE* tcab = F(BYTE*, t, 21600);
    F(DWORD, cab, 10124) = F(DWORD, t, 3284);
    CallVirt(F(BYTE*, s, 21600), 0);
    CallThis(Hp_SetPos, F(BYTE*, t, 21600), F(BYTE*, s, 21600) + 10832);
    CallThis(Hp_SetV88, F(BYTE*, t, 21600), F(BYTE*, s, 21600) + 10868);
    Matrix M3, M4;
    memcpy(&M3, F(BYTE*, s, 21600) + 10736, 48);
    memcpy(&M4, F(BYTE*, t, 21600) + 10736, 48);
    CallThis(Hp_Align, s, &M3, &M4);
    tcab = F(BYTE*, t, 21600);
    float p1[3] = {P1.x, P1.y, P1.z};
    Vec3 V = Hp_Frame(M3.m, p1);
    V = Hp_SetV(V.x, V.y, V.z);
    Vec3 R, N1, N2, E, Wn, C, axis;
    CallC(Hp_MulM, &R, &V, tcab + 10784);
    CallC(Hp_Norm, &N1, &R);
    Vec3 W = Hp_SetV((double)len * N1.x, (double)len * N1.y, (double)len * N1.z);
    Vec3 Wc = W;
    CallC(Hp_Norm, &N2, &P2);
    Vec3 D = Hp_SetV((double)R.x - W.x, (double)R.y - W.y, (double)R.z - W.z);
    CallC(Hp_RotBy, &E, &D, F(BYTE*, t, 21600) + 10736);
    CallC(Hp_Norm, &Wn, &Wc);
    CallC(Hp_Cross, &C, &N2, &Wn);
    float cl = (float)CallC<double>(Hp_Length, &C);
    CallC(Hp_Div, &axis, &C, cl);
    float ang = (float)x87_asin(cl);
    Matrix rot, N;
    CallC(Hp_LookAt, &rot, g_hpZero, axis, ang);
    memcpy(&M4, Matrix_Multiply(&N, &rot, (const Matrix*)(F(BYTE*, t, 21600) + 10736)), 48);
    Vec3 ret;
    CallThis(Hp_Add, (Vec3*)&M4.m[9], &ret, &E);
    CallThis(Hp_Get800, s + 16);
    BYTE* o = CallThis<BYTE*>(Hp_Get800, t + 16);
    int hit = CallC<int>(Hp_Hit, o, t + 3388, &M4.m[9]);
    CallThis(Hp_Place, F(BYTE*, t, 21600), hit, &M4, (int)&P2);
    return 0;
}

} // extern "C"

// === a car's flasher made (FUN_54a120)

extern "C" void Fl_NodeN(); KING_ALIAS("_Fl_NodeN", "@NodeN_Construct@20")
extern "C" void Fl_Light(); KING_ALIAS("_Fl_Light", "@Light_Init@8")
extern "C" void Fl_SetPos(); KING_ALIAS("_Fl_SetPos", "@Flasher_SetPos@12")
extern "C" void Fl_Step(); KING_ALIAS("_Fl_Step", "@Flasher_Step@12")
KING_VAR(BYTE*, g_fl6ef1fc, 6ef1fc)
KING_VAR(AsmLabel, g_fl64d444, 64d444)
KING_VAR(char, g_flName, 67a164)

extern "C" {

// [FUN_54a120] flasher s at pos with period k: the start time (+8), its light node (+0, kind 21: on, 1.0 /
// 0.94 / 0.75 colored, range 1, 0.08) added to the world's lights, placed and stepped
BYTE* __fastcall Flasher_Construct(BYTE* s, void*, const float* pos, float k)
{
    F(float, s, 8) = (float)F(double, g_fl6ef1fc, 2968);
    F(int, s, 12) = 0;
    F(float, s, 4) = k;
    BYTE* n = (BYTE*)operator new(144);
    if (n)
    {
        CallThis(Fl_NodeN, n, &g_flName, 21, 1);
        CallThis(Fl_Light, n + 56);
        F(void*, n, 0) = &g_fl64d444;
        F(int, n, 136) = 1;
        F(int, n, 140) = 0;
    }
    F(BYTE*, s, 0) = n;
    F(int, n, 136) = 1;
    BYTE* l = F(BYTE*, s, 0);
    F(int, l, 140) = 1;
    F(int, l, 56) = 2;
    F(DWORD, l, 108) = 0x3F800000;
    F(DWORD, l, 112) = 1064366313;
    F(DWORD, l, 116) = 1061076670;
    F(DWORD, l, 84) = 0x3F800000;
    F(int, l, 88) = 0;
    F(DWORD, l, 92) = 1034147594;
    if (BYTE* r = F(BYTE*, g_fl6ef1fc, 528))
        CallVirt(r, 20, 0, F(BYTE*, s, 0));
    CallThis(Fl_SetPos, s, pos);
    CallThis(Fl_Step, s, *(int*)&k);
    return s;
}

} // extern "C"

// === a tractor's frame with its trailer hitched (FUN_55d5d0 with its tails $L_55ddd4 .. $L_55ddfe)

ASM_FUNC(FUN_558f20, _Hitch_Solve558f20)
extern "C" void Th_BodyStep(); KING_ALIAS("_Th_BodyStep", "@CarBody_Step@4")
extern "C" void Th_CopyTowed(); KING_ALIAS("_Th_CopyTowed", "@Car_CopyToTowed@8")
extern "C" void Th_Mode(); KING_ALIAS("_Th_Mode", "@Body_SetMode@12")
extern "C" void Th_Hitch(); KING_ALIAS("_Th_Hitch", "@Body_UpdateHitch@12")
extern "C" void Th_Matrix(); KING_ALIAS("_Th_Matrix", "@SceneObj_Matrix1@12")
extern "C" void Th_Collide(); KING_ALIAS("_Th_Collide", "_SceneObj_Collide")
extern "C" void Th_Set(); KING_ALIAS("_Th_Set", "@Vec3_SetThis@20")
extern "C" void Th_Found(); KING_ALIAS("_Th_Found", "@Car_ApplyFound@8")
extern "C" void Th_55ed50(); KING_ALIAS("_Th_55ed50", "@Car_55ed50@8")
extern "C" void Th_558930(); KING_ALIAS("_Th_558930", "@Car_558930@8")
extern "C" void Th_Step(); KING_ALIAS("_Th_Step", "@Car_Step@12")
extern "C" void Th_Move(); KING_ALIAS("_Th_Move", "@Tractor_Move@16")
extern "C" void Th_Shadows(); KING_ALIAS("_Th_Shadows", "@Car_LampShadows@8")
KING_VAR(BYTE, g_th697f9c, 697f9c)
KING_VAR(double, g_th64b5a0, 64b5a0)
KING_VAR(float, g_th64d2f8, 64d2f8)
KING_VAR(char, g_thBad, 67a5e4)

// the hitch of tractor s (its frame +692, its type +10404) and of trailer t (frame +708, type +10420) in the
// trailer's frame: h the tractor's in its own frame (unrounded), g the trailer's (b picks the asm's two orders)
static void Th_Points(BYTE* s, BYTE* t, double* h, float* g, bool b)
{
    const float* M = &F(float, s, 692);
    const float* T = (const float*)(&g_th697f9c + F(int, s, 10404) * 2416 + 156);
    h[0] = (double)M[6] * T[2] + (double)M[3] * T[1] + (double)M[0] * T[0] + M[9];
    h[1] = (double)M[7] * T[2] + (double)M[4] * T[1] + (double)M[1] * T[0] + M[10];
    h[2] = (double)M[8] * T[2] + (double)M[5] * T[1] + (double)M[2] * T[0] + M[11];
    double a = T[12];
    const float* T2 = (const float*)(&g_th697f9c + F(int, t, 10420) * 2416 + 168);
    float px = (float)((double)(float)(a * g_axisY.x) + T2[0]);
    float py = (float)((double)(float)(a * g_axisY.y) + T2[1]);
    double pz = a * g_axisY.z + T2[2];
    const float* N = &F(float, t, 708);
    if (!b)
    {
        g[0] = (float)((double)N[0] * px + pz * N[6] + (double)py * N[3] + N[9]);
        g[1] = (float)((double)N[1] * px + pz * N[7] + (double)py * N[4] + N[10]);
        g[2] = (float)((double)N[2] * px + pz * N[8] + (double)py * N[5] + N[11]);
    }
    else
    {
        g[0] = (float)(pz * N[6] + (double)py * N[3] + (double)N[0] * px + N[9]);
        g[1] = (float)(pz * N[7] + (double)py * N[4] + (double)N[1] * px + N[10]);
        g[2] = (float)(pz * N[8] + (double)py * N[5] + (double)N[2] * px + N[11]);
    }
}

extern "C" {

// [FUN_55d5d0..$L_55ddfe] tractor (body s - 16) and its trailer t (+21864): alone just its body step; hitching
// (+21868, a free mode-1 body) until the hitches meet; both bodies in the tractor's mode, stepped (mode 1 by
// Tractor_PlaceTrailer, 3 by 558f20) and on success their speeds saved and the hitch made (+21876); while hitched
// the hitches kept together (detached when they part or the bodies collide), both cars stepped and moved together
void __fastcall Tractor_Frame(BYTE* s)
{
    BYTE* body = s - 16;
    BYTE* t = F(BYTE*, s, 21864);
    if (!t)
    {
        CallThis(Th_BodyStep, s);
        F(DWORD, s, 10148) &= 0xFFFDFFFF;
        return;
    }
    F(DWORD, s, 10148) |= 0x20000;
    CallThis(Th_CopyTowed, body);
    int ok = 1;
    if (F(int, s, 21868) && !F(int, s, 1204) && F(int, F(BYTE*, s, 21584), 9784) == 1)
    {
        double h[3];
        float g[3];
        Th_Points(s, t, h, g, false);
        const float* B = &F(float, s, 500);
        float P[3];
        P[0] = (float)(h[2] * B[6] + h[1] * B[3] + h[0] * B[0] + B[9]);
        P[1] = (float)(h[0] * B[1] + h[2] * B[7] + h[1] * B[4] + B[10]);
        P[2] = (float)(h[0] * B[2] + h[2] * B[8] + h[1] * B[5] + B[11]);
        float Mt0[12];
        const float* Mt = CallThis<float*>(Th_Matrix, t + 16, Mt0);
        double qx = (double)g[2] * Mt[6] + (double)g[1] * Mt[3] + (double)g[0] * Mt[0] + Mt[9];
        float qy = (float)((double)g[0] * Mt[1] + (double)g[2] * Mt[7] + (double)g[1] * Mt[4] + Mt[10]);
        float qz = (float)((double)g[0] * Mt[2] + (double)g[2] * Mt[8] + (double)g[1] * Mt[5] + Mt[11]);
        double dx = P[0] - qx;
        float dy = (float)((double)P[1] - qy);
        double dz = (double)P[2] - qz;
        if ((double)dy * dy + dz * dz + dx * dx < g_th64b5a0)
        {
            F(int, s, 21868) = 0;
            CallThis(Th_Mode, F(BYTE*, s, 21584), 1);
        }
        else
            CallThis(Th_Mode, F(BYTE*, s, 21584), 3);
    }
    CallThis(Th_Mode, F(BYTE*, t, 21600), F(int, F(BYTE*, s, 21584), 9784));
    int mode = F(int, F(BYTE*, s, 21584), 9784);
    if (mode == 1)
        ok = Tractor_PlaceTrailer(body, 0);
    else if (mode == 3)
        ok = CallThis<int>(FUN_558f20, body);
    if (!F(int, s, 21876) && ok)
        F(int, s, 21876) = 1;
    if (F(int, s, 21876) == 1)
    {
        F(int, s, 21876) = 2;
        memcpy(s + 21916, s + 416, 12);
        memcpy(s + 21928, s + 428, 12);
        memcpy(t + 21924, t + 432, 12);
        memcpy(t + 21936, t + 444, 12);
        CallThis(Th_Hitch, F(BYTE*, t, 21600), 1);
    }
    F(int, F(BYTE*, t, 21600), 13512) = F(int, F(BYTE*, s, 21584), 13512);
    int r1 = 0, r2 = ok;
    if (F(int, s, 21876) > 0)
    {
        r2 = 0;
        bool apart = !CallC<int>(Th_Collide, s, t ? t + 16 : 0, &r2);
        double h[3];
        float g[3];
        Th_Points(s, t, h, g, true);
        const float* B = &F(float, s, 500);
        float P[3];
        CallThis(Th_Set, P, (float)(h[2] * B[6] + h[1] * B[3] + h[0] * B[0] + B[9]),
                 (float)(h[2] * B[7] + h[1] * B[4] + h[0] * B[1] + B[10]),
                 (float)(h[2] * B[8] + h[1] * B[5] + h[0] * B[2] + B[11]));
        float Mt0[12];
        Vec3 q;
        Vec3_MulMatrix(&q, (const Vec3*)g, CallThis<Matrix*>(Th_Matrix, t + 16, Mt0));
        float d[3];
        CallThis(Th_Set, d, (float)((double)P[0] - q.x), (float)((double)P[1] - q.y), (float)((double)P[2] - q.z));
        double len = Vec3_Length((const Vec3*)d);
        if (apart && len > g_th64d2f8)
        {
            CallThis(Th_Found, body);
            CallVirt(s, 144, s + 21916, 0);
            CallVirt(s, 148, s + 21928, 0);
            CallVirt(t + 16, 144, t + 21924, 0);
            CallVirt(t + 16, 148, t + 21936, 0);
            CallThis(Th_Found, F(BYTE*, s, 21864));
            CallThis(Th_55ed50, F(BYTE*, s, 21864));
            CallVirt(F(BYTE*, s, 21864), 100);
            CallThis(Th_558930, body);
            goto lamps;
        }
    }
    {
        int a = CallThis<int>(Th_Step, body, &r1);
        int b = CallThis<int>(Th_Step, F(BYTE*, s, 21864), &r2);
        if (a != b)
            World_Fatal(&g_thBad);
        if (a)
        {
            CallThis(Th_Move, body, r1, r2);
            CallThis(FUN_54ff80, body);
            CallThis(FUN_54ff80, F(BYTE*, s, 21864));
        }
    }
lamps:
    if (F(int, s, 10564))
    {
        CallThis(Th_Shadows, body);
        F(int, s, 10564) = 0;
        if (F(BYTE*, s, 21864))
            CallThis(Th_Shadows, F(BYTE*, s, 21864));
    }
}

} // extern "C"

// === a car's frame after its physics: rocking, lights, rumble and the beacon (FUN_54ff80 with its tail $L_550358)

extern "C" void Cf_Cargo(); KING_ALIAS("_Cf_Cargo", "@Car_UpdateCargo@8")
extern "C" void Cf_Slip(); KING_ALIAS("_Cf_Slip", "_Veh_54ef90")
extern "C" void Cf_Sign(); KING_ALIAS("_Cf_Sign", "@Car_LookAtSign@8")
extern "C" void Cf_Hits(); KING_ALIAS("_Cf_Hits", "@Car_HitEvents@4")
extern "C" void Cf_Rest(); KING_ALIAS("_Cf_Rest", "@Item_Rest@4")
extern "C" void Cf_Place(); KING_ALIAS("_Cf_Place", "@Car_ApplyPlacement@8")
extern "C" void Cf_Effects(); KING_ALIAS("_Cf_Effects", "@Car_Effects@8")
extern "C" void Cf_Order(); KING_ALIAS("_Cf_Order", "@Car_WorkingOrder@4")
extern "C" void Cf_GiveWay(); KING_ALIAS("_Cf_GiveWay", "@Car_GiveWay@8")
extern "C" void Cf_Pack(); KING_ALIAS("_Cf_Pack", "@Vehicle_PackState@4")
extern "C" void Cf_Node(); KING_ALIAS("_Cf_Node", "@Node_Construct@16")
extern "C" void Cf_NodeN(); KING_ALIAS("_Cf_NodeN", "@NodeN_Construct@20")
extern "C" void Cf_Apply(); KING_ALIAS("_Cf_Apply", "@Node_ApplyMatrix@16")
extern "C" void Cf_Lamp(); KING_ALIAS("_Cf_Lamp", "@Obj551920_Construct@16")
extern "C" void Cf_LampM(); KING_ALIAS("_Cf_LampM", "@Obj5518d0_Construct@8")
extern "C" void Cf_Set(); KING_ALIAS("_Cf_Set", "@Vec3_SetThis@20")
extern "C" void Cf_Zero(); KING_ALIAS("_Cf_Zero", "@Vec3_ZeroThis@8")
extern "C" void Cf_FrameTime(); KING_ALIAS("_Cf_FrameTime", "_Game_FrameTime")
KING_VAR(int, g_cf6ef200, 6ef200)
KING_VAR(BYTE*, g_cfGame, 6d2078)
KING_VAR(BYTE*, g_cf6ef254, 6ef254)
KING_VAR(BYTE*, g_cf6ef258, 6ef258)
KING_VAR(BYTE*, g_cf6f33ac, 6f33ac)
KING_VAR(BYTE*, g_cf6ef1fc, 6ef1fc)
KING_VAR(BYTE*, g_cf69622c, 69622c)
KING_VAR(BYTE, g_cf6796b0, 6796b0)
KING_VAR(float, g_cf6796cc, 6796cc)
KING_VAR(float, g_cf6796d0, 6796d0)
KING_VAR(float, g_cf6796d4, 6796d4)
KING_VAR(float, g_cf6796d8, 6796d8)
KING_VAR(float, g_cf6796dc, 6796dc)
KING_VAR(float, g_cf6ef2a0, 6ef2a0)
KING_VAR(float, g_cf6ef2a4, 6ef2a4)
KING_VAR(Vec3, g_cf6974a8, 6974a8)
KING_VAR(BYTE, g_cf6d2510, 6d2510)
KING_VAR(BYTE, g_cf6d2524, 6d2524)
KING_VAR(char, g_cfLamp, 67334c)
KING_VAR(char, g_cfEmpty, 68d1c0)
KING_VAR(AsmLabel, g_cf64c7dc, 64c7dc)
KING_VAR(AsmLabel, g_cf64d7d0, 64d7d0)
KING_VAR(float, g_cf64c250, 64c250)
KING_VAR(float, g_cf64c264, 64c264)
KING_VAR(float, g_cf64c450, 64c450)
KING_VAR(float, g_cf64c33c, 64c33c)
KING_VAR(float, g_cf64da74, 64da74)
KING_VAR(float, g_cf64f778, 64f778)
KING_VAR(double, g_cf64d318, 64d318)
KING_VAR(double, g_cf64b6e0, 64b6e0)
KING_VAR(double, g_cf64b670, 64b670)
KING_VAR(double, g_cf64b918, 64b918)
KING_VAR(double, g_cf64b7a8, 64b7a8)
KING_VAR(double, g_cf64f790, 64f790)
KING_VAR(double, g_cf64ca78, 64ca78)
KING_VAR(double, g_cf64b930, 64b930)
KING_VAR(double, g_cf64b5b8, 64b5b8)
KING_VAR(double, g_cf64b828, 64b828)
KING_VAR(double, g_cf64b820, 64b820)
KING_VAR(double, g_cf64f788, 64f788)
KING_VAR(double, g_cf64f780, 64f780)
KING_VAR(double, g_cf64b840, 64b840)
KING_VAR(double, g_cf64f398, 64f398)
KING_VAR(double, g_cf64d668, 64d668)

static void Cf_SetV(float* o, double x, double y, double z)
{
    CallThis(Cf_Set, o, (float)x, (float)y, (float)z);
}

// the lamp matrix: scale by k times the lamp colour scale (6974a8) at p, in frame W
static void Cf_LampMatrix(Matrix* out, const float* p, const float* k, const Matrix* W)
{
    Matrix T, S, ST;
    Matrix_Translation(&T, *(const Vec3*)p);
    Matrix_Scale(&S, *(const Vec3*)k);
    Matrix_Multiply(&ST, &S, &T);
    Matrix_Multiply(out, &ST, W);
}

// a beacon lamp at angle h seen from the camera: how bright (b, the asm's two curves) and how far it leans (w)
static bool Cf_LampView(double h, float c1, float c2, float k, double* v, float* sn)
{
    double ah = x87_fabs(h);
    if (!(ah < g_cf64f788) && !(ah > g_cf64f780))
        return false;
    *sn = (float)x87_fabs(x87_sin(h));
    double v0 = ah < g_cf64f788 ? x87_cos(h) - c1 : c2 - x87_cos(h);
    *v = v0 * k;
    return true;
}

// the three rotating lamp nodes added to the scene (matrices m[0..2]); the first group builds them inline
static void Cf_AddLampsInline(const Matrix* m)
{
    for (int k = 0; k < 3; k++)
    {
        BYTE* n = (BYTE*)operator new(88);
        if (n)
        {
            BYTE* src = g_cf6ef254;
            CallThis(Cf_Node, n, &g_cfLamp, 17);
            F(int, n, 48) = F(int, n, 52) = F(int, n, 56) = F(int, n, 60) = 0;
            F(void*, n, 0) = &g_cf64c7dc;
            F(BYTE*, n, 64) = src;
            F(int, n, 68) = F(int, n, 72) = F(int, n, 76) = F(int, n, 80) = 0;
            F(int, n, 84) = -1;
            F(int, n, 36) = F(int, src, 36);
        }
        BYTE* mm = (BYTE*)operator new(112);
        if (mm)
        {
            CallThis(Cf_NodeN, mm, &g_cfEmpty, 6, 1);
            for (int i = 0; i < 4; i++)
                CallThis(Cf_Zero, mm + 64 + 12 * i);
            memcpy(mm + 64, &g_identity, 48);
            F(void*, mm, 0) = &g_cf64d7d0;
            F(int, mm, 56) = 0;
            F(int, mm, 60) = 0;
        }
        F(BYTE*, n, 68) = mm;
        CallThis(Cf_Apply, mm, m + k, 0);
        CallVirt(g_cf69622c, 20, 0, n);
    }
}

static void Cf_AddLamps(const Matrix* m)
{
    for (int k = 0; k < 3; k++)
    {
        BYTE* n = (BYTE*)operator new(88);
        BYTE* node = n ? CallThis<BYTE*>(Cf_Lamp, n, &g_cfLamp, g_cf6ef258) : 0;
        BYTE* mm = (BYTE*)operator new(112);
        BYTE* mo = mm ? CallThis<BYTE*>(Cf_LampM, mm) : 0;
        F(BYTE*, node, 68) = mo;
        CallThis(Cf_Apply, mo, m + k, 0);
        CallVirt(g_cf69622c, 20, 0, node);
    }
}

extern "C" {

// [FUN_54ff80..$L_550358] car s after its physics: its slip at a hit point (+10812), the body's rocking angles
// (+20308, +20312) from its acceleration and spin, its light flags (+10164), night lights (+20916 by the clock),
// the rumble of its wheels on rough ground (+10224), its effects, its beacon's three lamps (+20924, turning by the
// clock, shown towards the camera) added to the scene, giving way, and its packed state
void __fastcall Car_AfterPhysics(BYTE* s)
{
    F(int, s, 912) = (F(int, s, 912) || F(int, s, 908)) ? 1 : 0;
    F(int, s, 916) = F(int, s, 908);
    if (F(int, s, 908) && F(BYTE*, s, 21604))
        F(int, F(BYTE*, s, 21604), 3488) = 1;
    CallThis(Cf_Cargo, s);
    const float* M = &F(float, s, 612);
    if (F(int, s, 10808) && !g_cf6ef200)
    {
        float v[3];
        memcpy(v, s + 10812, 12);
        double px = (double)v[2] * M[6] + (double)v[1] * M[3] + (double)v[0] * M[0] + M[9];
        float py = (float)((double)v[0] * M[1] + (double)v[2] * M[7] + (double)v[1] * M[4] + M[10]);
        float pz = (float)((double)v[0] * M[2] + (double)v[2] * M[8] + (double)v[1] * M[5] + M[11]);
        float cz = (float)((double)py * F(float, s, 444) - px * F(float, s, 448));
        float cy = (float)(px * F(float, s, 452) - (double)pz * F(float, s, 444));
        float cx = (float)((double)pz * F(float, s, 448) - (double)py * F(float, s, 452));
        float u[3] = {(float)((double)cx + F(float, s, 432)), (float)((double)cy + F(float, s, 436)),
                      (float)((double)cz + F(float, s, 440))};
        Vec3 nv;
        Vec3_Normalized(&nv, (const Vec3*)u);
        if (F(int, s, 908) && !F(int, s, 1044))
        {
            double d = (double)F(float, s, 1000) * nv.z + (double)F(float, s, 996) * nv.y +
                       (double)nv.x * F(float, s, 992);
            if (x87_fabs(d) < g_cf64d318 && x87_fabs(F(float, s, 1000)) < g_cf64b6e0)
                CallC(Cf_Slip, 0, v, u);
        }
    }
    float dt = (float)CallC<double>(Cf_FrameTime);
    {
        double azd = (double)F(float, s, 440) - F(float, s, 476);
        float ayf = (float)((double)F(float, s, 436) - F(float, s, 472));
        float axf = (float)((double)F(float, s, 432) - F(float, s, 468));
        double az = azd / dt, ay = ayf / (double)dt, ax = axf / (double)dt;
        float l0 = (float)(ay * M[3] + az * M[6] + ax * M[0]);
        float l1 = (float)(ax * M[1] + ay * M[4] + az * M[7]);
        const float* g = &F(float, s, 480);
        float gp[3] = {(float)((double)M[6] * g[2] + (double)M[3] * g[1] + (double)M[0] * g[0]),
                       (float)((double)M[7] * g[2] + (double)M[4] * g[1] + (double)M[1] * g[0]),
                       (float)((double)M[8] * g[2] + (double)M[5] * g[1] + (double)M[2] * g[0])};
        Vec3 r;
        Vec3_DivComponents(&r, (const Vec3*)gp, (const Vec3*)(s + 420));
        float w0 = (float)((double)F(float, s, 444) - r.x);
        float s28 = (float)(((double)F(float, s, 448) - r.y) / dt);
        float s32 = (float)((double)w0 / dt);
        double p = x87_pow(g_cf6796cc, (double)dt * g_cf64c450);
        float e1 = (float)((double)l1 * g_cf6796d8 - (double)s32 * g_cf6ef2a0);
        double e2 = (double)s28 * g_cf6ef2a4 - (double)l0 * g_cf6796dc;
        if (!(e1 <= g_cf6796d0))
            e1 = g_cf6796d0;
        if (e1 < -g_cf6796d0)
            e1 = -g_cf6796d0;
        if (e2 > g_cf6796d4)
            e2 = g_cf6796d4;
        float lo = -g_cf6796d4;
        if (e2 < lo)
            e2 = lo;
        double q = g_cf64c250 - p;
        F(int, s, 10808) = 0;
        F(float, s, 20308) = (float)(q * e1 + p * F(float, s, 20308));
        F(float, s, 20312) = (float)(q * e2 + p * F(float, s, 20312));
    }
    memcpy(s + 10812, &g_zeroVec, 12);
    F(BYTE, s, 10168) = F(BYTE, s, 3992);
    {
        DWORD f = F(DWORD, s, 10164);
        int b = F(int, s, 20920);
        if (b == 1)
            f = (f & ~0x10u) | 4;
        else if (b == 2)
            f = (f & ~4u) | 16;
        else
            f &= ~0x14u;
        f = F(int, s, 20916) ? f | 8 : f & ~8u;
        f = F(int, s, 20860) ? f | 64 : f & ~64u;
        f = F(int, s, 20856) ? f | 32 : f & ~32u;
        f = F(int, s, 1220) ? f | 1 : f & ~1u;
        f = F(int, s, 20896) ? f | 0x400 : f & ~0x400u;
        f = F(int, s, 20928) ? f | 0x40000 : f & ~0x40000u;
        F(DWORD, s, 10164) = f;
    }
    if (F(BYTE, s, 20900) & 1)
        CallThis(Cf_Sign, s);
    CallThis(Cf_Hits, s);
    {
        int h = (int)F(double, g_cf6f33ac, 88);
        F(int, s, 20916) = (h > 22 || h < 5) ? 1 : 0;
    }
    CallThis(Cf_Rest, s + 16);
    if (g_cf6796b0 & 8)
        CallThis(Cf_Place, s);
    memset(s + 10224, 0, 32);
    BYTE* c = F(BYTE*, s, 21600);
    if (c && F(int, s, 10924) == 1 && !F(int, c, 13124))
    {
        double sp = x87_fabs(F(float, c, 13100));
        if (sp > g_cf64b670)
        {
            float rough = 1.0f;
            if (sp < g_cf64b918)
                rough = (float)(x87_fabs(sp - x87_fabs(F(float, c, 13104)) * g_cf64b7a8) * g_cf64f790);
            double k = x87_sqrt(rough) * g_cf64ca78;
            if (F(float, s, 412) < g_cf64da74)
                k = x87_sqrt(F(float, s, 412) * g_cf64b930) * k;
            for (int i = 0; i < F(int, s, 10428); i++)
            {
                BYTE* b = F(BYTE*, s, 21600);
                BYTE* e = F(BYTE*, b, 11220 + 248 * i);
                if (!F(WORD, b, 11112 + 248 * i) || F(int, b, 13380 + 4 * i) || !e || !F(int, b, 11228 + 248 * i))
                    continue;
                int t = F(int, e, 4);
                if ((t == 6 || t == 10 || t == 11) && k > g_cf64b5b8)
                    F(float, s, 10224 + 4 * i) = (float)k;
            }
        }
    }
    CallThis(Cf_Effects, s);
    const BYTE* T = &g_cf6d2524 + F(int, s, 10420) * 1488;
    if (F(int, s, 20924) && F(int, &g_cf6d2510, F(int, s, 10420) * 1488 + 8) && g_cf6ef254 && g_cf6ef258 &&
        CallThis<double>(Cf_Order, s) > g_cf64ca78)
    {
        F(int, s, 20520) = 1;
        double t = Game_Time() + F(float, s, 20508);
        float a = (float)((t - (int)t) * 2 * g_cf64b828);
        F(float, s, 20512) = a;
        F(float, s, 20516) = a;
        float k1 = (float)x87_cos(g_cf64f788), k2 = (float)x87_cos(g_cf64f780);
        float kn = (float)(g_cf64c250 / ((double)g_cf64c250 - k1));
        float am = (float)((double)a - g_cf64f778), ap = (float)((double)a + g_cf64f778);
        double cm = x87_cos(am);
        float c0 = (float)x87_cos(a), cp = (float)x87_cos(ap);
        if (a > g_cf64b828)
            F(float, s, 20516) = (float)(a - g_cf64b820);
        float cm2 = (float)(cm * cm), c02 = (float)((double)c0 * c0), cp2 = (float)((double)cp * cp);
        Matrix W, Wi;
        Matrix_Multiply(&W, (const Matrix*)(s + 708), (const Matrix*)(s + 516));
        Matrix_Inverse(&Wi, &W);
        const float* w = W.m;
        const float* iv = Wi.m;
        const float* cam = &F(float, g_cfGame, 1460);
        // the first group: the lamps around the table point +12
        {
            const float* tp = (const float*)(T + 12);
            float lx = (float)((double)w[0] * tp[0] + (double)w[6] * tp[2] + (double)w[3] * tp[1] + w[9]);
            float ly = (float)((double)w[7] * tp[2] + (double)w[4] * tp[1] + (double)w[1] * tp[0] + w[10]);
            double lz = (double)w[8] * tp[2] + (double)w[5] * tp[1] + (double)w[2] * tp[0] + w[11];
            float d[3] = {(float)((double)cam[0] - lx), (float)((double)cam[1] - ly), (float)(cam[2] - lz)};
            Vec3 dn;
            Vec3_Normalized(&dn, (const Vec3*)d);
            float vx = (float)((double)iv[6] * dn.z + (double)iv[3] * dn.y + (double)iv[0] * dn.x);
            float vy = (float)((double)iv[7] * dn.z + (double)iv[4] * dn.y + (double)dn.x * iv[1]);
            float vz = (float)((double)iv[8] * dn.z + (double)iv[5] * dn.y + (double)dn.x * iv[2]);
            double h = -x87_atan2(vx, vy);
            double v;
            float sn;
            if (Cf_LampView(h, k1, k2, kn, &v, &sn))
            {
                float b1 = (float)(v * cm2), b0 = (float)(v * c02), b2 = (float)(v * cp2);
                double sg = vy > g_cf64c264 ? g_cf64b840 : g_cf64f398;
                float oz = (float)(sg * g_axisY.z), oy = (float)(sg * g_axisY.y);
                double ox = sg * g_axisY.x;
                double wv = ((double)sn + g_cf64c250) * g_cf64c33c;
                float tz = (float)(vz * wv), ty = (float)(vy * wv), tx = (float)(vx * wv);
                const float* b = (const float*)T;
                float P[3][3];
                double r = x87_sin(am) * g_cf64d668;
                {
                    float z1 = (float)((double)b[2] - (float)(r * g_axisX.z));
                    float y1 = (float)((double)b[1] - (float)(r * g_axisX.y));
                    double x1 = b[0] - r * g_axisX.x;
                    z1 = (float)((double)z1 + oz);
                    y1 = (float)((double)y1 + oy);
                    float xf = (float)(x1 + ox);
                    P[1][0] = (float)((double)xf + tx);
                    P[1][1] = (float)((double)y1 + ty);
                    P[1][2] = (float)((double)z1 + tz);
                }
                float uz = (float)(wv * vz), uy = (float)(wv * vy), ux = (float)(wv * vx);
                r = x87_sin(a) * g_cf64d668;
                {
                    float z1 = (float)((double)b[5] - (float)(r * g_axisX.z));
                    float y1 = (float)((double)b[4] - (float)(r * g_axisX.y));
                    double x1 = b[3] - r * g_axisX.x;
                    z1 = (float)((double)z1 + oz);
                    y1 = (float)((double)y1 + oy);
                    float xf = (float)(x1 + ox);
                    P[0][0] = (float)((double)xf + ux);
                    P[0][1] = (float)((double)y1 + uy);
                    P[0][2] = (float)((double)z1 + uz);
                }
                uz = (float)(wv * vz);
                uy = (float)(wv * vy);
                ux = (float)(wv * vx);
                r = x87_sin(ap) * g_cf64d668;
                {
                    float z1 = (float)((double)b[8] - (float)(r * g_axisX.z));
                    float y1 = (float)((double)b[7] - (float)(r * g_axisX.y));
                    double x1 = b[6] - r * g_axisX.x;
                    z1 = (float)((double)z1 + oz);
                    y1 = (float)((double)y1 + oy);
                    float xf = (float)(x1 + ox);
                    P[2][0] = (float)((double)xf + ux);
                    P[2][1] = (float)((double)y1 + uy);
                    P[2][2] = (float)((double)z1 + uz);
                }
                const float* g6 = &g_cf6974a8.x;
                float K1[3] = {(float)((double)b1 * g6[0]), (float)((double)b1 * g6[1]), (float)((double)b1 * g6[2])};
                float K0[3] = {(float)((double)b0 * g6[0]), (float)((double)b0 * g6[1]), (float)((double)b0 * g6[2])};
                float K2[3] = {(float)((double)b2 * g6[0]), (float)((double)b2 * g6[1]), (float)((double)b2 * g6[2])};
                Matrix L[3];
                Cf_LampMatrix(&L[0], P[1], K1, &W);
                Cf_LampMatrix(&L[1], P[0], K0, &W);
                Cf_LampMatrix(&L[2], P[2], K2, &W);
                Cf_AddLampsInline(L);
            }
        }
        // the second group: the lamps around the table point +48
        {
            const float* tp = (const float*)(T + 48);
            float lx = (float)((double)w[6] * tp[2] + (double)w[3] * tp[1] + (double)w[0] * tp[0] + w[9]);
            float ly = (float)((double)w[7] * tp[2] + (double)w[4] * tp[1] + (double)w[1] * tp[0] + w[10]);
            double lz = (double)w[8] * tp[2] + (double)w[5] * tp[1] + (double)w[2] * tp[0] + w[11];
            float d[3] = {(float)((double)cam[0] - lx), (float)((double)cam[1] - ly), (float)(cam[2] - lz)};
            Vec3 dn;
            Vec3_Normalized(&dn, (const Vec3*)d);
            double vx = (double)dn.x * iv[0] + (double)dn.z * iv[6] + (double)dn.y * iv[3];
            float vy = (float)((double)dn.z * iv[7] + (double)dn.y * iv[4] + (double)dn.x * iv[1]);
            float vz = (float)((double)dn.z * iv[8] + (double)dn.y * iv[5] + (double)dn.x * iv[2]);
            double h = -x87_atan2(vx, vy);
            double v;
            float sn;
            if (Cf_LampView(h, k1, k2, kn, &v, &sn))
            {
                float b1 = (float)(v * cm2), b0 = (float)(v * c02), b2 = (float)(v * cp2);
                double sg = vy > g_cf64c264 ? g_cf64b840 : g_cf64f398;
                float oz = (float)(sg * g_axisY.z), oy = (float)(sg * g_axisY.y), ox = (float)(sg * g_axisY.x);
                double wv = (double)sn * g_cf64c33c;
                float tz = (float)(vz * wv), ty = (float)(vy * wv), tx = (float)(vx * wv);
                const float* b = (const float*)(T + 36);
                float PA[3], PB[3], PC[3];
                double r = x87_sin(am) * g_cf64d668;
                {
                    float z1 = (float)((double)(float)(r * g_axisX.z) + b[2]);
                    float y1 = (float)((double)(float)(r * g_axisX.y) + b[1]);
                    double x1 = r * g_axisX.x + b[0];
                    z1 = (float)((double)z1 + oz);
                    y1 = (float)((double)y1 + oy);
                    float xf = (float)(x1 + ox);
                    PA[0] = (float)((double)xf + tx);
                    PA[1] = (float)((double)y1 + ty);
                    PA[2] = (float)((double)z1 + tz);
                }
                tz = (float)(wv * vz);
                ty = (float)(wv * vy);
                tx = (float)(wv * vx);
                r = x87_sin(a) * g_cf64d668;
                {
                    float z1 = (float)((double)(float)(r * g_axisX.z) + b[5]);
                    float y1 = (float)((double)(float)(r * g_axisX.y) + b[4]);
                    double x1 = r * g_axisX.x + b[3];
                    z1 = (float)((double)oz + z1);
                    y1 = (float)((double)oy + y1);
                    float xf = (float)((double)ox + x1);
                    PB[0] = (float)((double)xf + tx);
                    PB[1] = (float)((double)y1 + ty);
                    PB[2] = (float)((double)tz + z1);
                }
                float off[3];
                Cf_SetV(off, wv * vx, wv * vy, wv * vz);
                r = x87_sin(ap) * g_cf64d668;
                {
                    float q1[3], q2[3], q3[3];
                    Cf_SetV(q1, r * g_axisX.x, r * g_axisX.y, r * g_axisX.z);
                    Cf_SetV(q2, (double)q1[0] + b[6], (double)q1[1] + b[7], (double)q1[2] + b[8]);
                    Cf_SetV(q3, (double)ox + q2[0], (double)oy + q2[1], (double)oz + q2[2]);
                    Cf_SetV(PC, (double)q3[0] + off[0], (double)q3[1] + off[1], (double)q3[2] + off[2]);
                }
                const float* g6 = &g_cf6974a8.x;
                float K1[3], K0[3];
                Cf_SetV(K1, (double)b1 * g6[0], (double)b1 * g6[1], (double)b1 * g6[2]);
                Matrix L[3];
                Cf_LampMatrix(&L[0], PA, K1, &W);
                Cf_SetV(K0, (double)b0 * g6[0], (double)b0 * g6[1], (double)b0 * g6[2]);
                Cf_LampMatrix(&L[1], PB, K0, &W);
                Vec3 k2v;
                Vec3_Scaled(&k2v, &g_cf6974a8, b2);
                Cf_LampMatrix(&L[2], PC, &k2v.x, &W);
                Cf_AddLamps(L);
            }
        }
        (void)c;
    }
    else
        F(int, s, 20520) = 0;
    CallThis(Cf_GiveWay, s);
    if ((double)F(float, s, 20624) + F(float, s, 20620) < F(double, g_cf6ef1fc, 2968))
        F(int, s, 20608) = 1;
    CallThis(Cf_Pack, s + 10084);
    F(int, s, 3572) = 0;
}

} // extern "C"

// === placing the player's vehicle back on the road (FUN_533590 with its tails $L_534443 and $L_53549a)

struct RqPos { int v[8]; };    // a road position (PositionId) passed by value

extern "C" void Rq_EffGet(); KING_ALIAS("_Rq_EffGet", "@Obj549bf0_Get@8")
extern "C" void Rq_EffEnd(); KING_ALIAS("_Rq_EffEnd", "@CarEffect_End@12")
extern "C" void Rq_Matrix1(); KING_ALIAS("_Rq_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Rq_Get800(); KING_ALIAS("_Rq_Get800", "@SceneObj_Get800@8")
extern "C" void Rq_SetThis(); KING_ALIAS("_Rq_SetThis", "@Vec3_SetThis@20")
extern "C" void Rq_AddA(); KING_ALIAS("_Rq_AddA", "@Vec3_AddAssign@16")
extern "C" void Rq_Zero(); KING_ALIAS("_Rq_Zero", "@Vec3_ZeroThis@8")
extern "C" void Rq_NodeInit(); KING_ALIAS("_Rq_NodeInit", "@Node_Init@8")
extern "C" void Rq_PtsNext(); KING_ALIAS("_Rq_PtsNext", "@Points_NextFirst@12")
extern "C" void Rq_SetCam(); KING_ALIAS("_Rq_SetCam", "@View_SetCamera@16")
extern "C" void Rq_ObjReset(); KING_ALIAS("_Rq_ObjReset", "@WorldObj_Reset@8")
extern "C" BYTE* __cdecl Rq_FromPA(BYTE* out, Vec3 p, float rx, float ry, float rz); KING_ALIAS("_Rq_FromPA", "_Matrix_FromPosAngles")
extern "C" BYTE* __cdecl Rq_Transl(BYTE* out, Vec3 t); KING_ALIAS("_Rq_Transl", "_Matrix_Translation")
extern "C" BYTE* __cdecl Rq_Mul(BYTE* out, const BYTE* a, const BYTE* b); KING_ALIAS("_Rq_Mul", "_Matrix_Multiply")
extern "C" BYTE* __cdecl Rq_Inv(BYTE* out, const BYTE* m); KING_ALIAS("_Rq_Inv", "_Matrix_Inverse")
extern "C" float* __cdecl Rq_Norm(float* out, const float* v); KING_ALIAS("_Rq_Norm", "_Vec3_Normalized")
extern "C" double __cdecl Rq_Len(const float* v); KING_ALIAS("_Rq_Len", "_Vec3_Length")
extern "C" float* __cdecl Rq_Scaled(float* out, const void* v, float k); KING_ALIAS("_Rq_Scaled", "_Vec3_Scaled")
extern "C" float* __cdecl Rq_Add(float* out, const float* a, const float* b); KING_ALIAS("_Rq_Add", "_Vec3_Add")
extern "C" float* __cdecl Rq_Sub(float* out, const float* a, const float* b); KING_ALIAS("_Rq_Sub", "_Vec3_Sub")
extern "C" BYTE* __cdecl Rq_Hit(BYTE* obj, const void* p, const void* d); KING_ALIAS("_Rq_Hit", "_Query_HitObject")
extern "C" int __cdecl Rq_ObjAt(void* id, const void* v, void* obj, void* pos, int anywhere); KING_ALIAS("_Rq_ObjAt", "_Room_ObjectAt")
extern "C" int __cdecl Rq_IsPlace(const void* pos); KING_ALIAS("_Rq_IsPlace", "_Road_IsPlace")
extern "C" BYTE* __cdecl Rq_FindNamed(BYTE* scene, const char* name); KING_ALIAS("_Rq_FindNamed", "_Scene_FindObject")
extern "C" void __cdecl Rq_Log(const char* text, int x); KING_ALIAS("_Rq_Log", "_Sys_Log")
extern "C" RqPos* __cdecl Rq_Right(RqPos* out, BYTE* player, RqPos pos); KING_ALIAS("_Rq_Right", "_Ai_GetRightDirection")
extern "C" float* __cdecl Rq_Coords(float* out, RqPos pos); KING_ALIAS("_Rq_Coords", "_Ai_GetCoordinatesPos")
extern "C" float* __cdecl Rq_Tangent(float* out, RqPos pos); KING_ALIAS("_Rq_Tangent", "_Ai_GetTangent")
extern "C" int __cdecl Rq_RoomInfo(RqPos pos, void* out); KING_ALIAS("_Rq_RoomInfo", "_Road_GetRoadParameters")
extern "C" int __cdecl Rq_Ray(BYTE* a, const void* p, const void* d, void* point, void* normal); KING_ALIAS("_Rq_Ray", "_Query_4e6749")
extern "C" int __cdecl Rq_WorldHit(BYTE* a, const void* p, const void* d, void* point, void* normal); KING_ALIAS("_Rq_WorldHit", "_World_Hit")
extern "C" int __cdecl Rq_IdSame(BYTE* x); KING_ALIAS("_Rq_IdSame", "_Id_Same")
extern "C" BYTE* __cdecl Rq_MovingOf(BYTE* o); KING_ALIAS("_Rq_MovingOf", "_Moving_Of")
extern "C" int __cdecl Rq_Blocked(int skip, BYTE* obj, const void* p, const void* d, BYTE* q); KING_ALIAS("_Rq_Blocked", "_Query_4e5415")
extern "C" int __cdecl Rq_Collide(BYTE* shape, const BYTE* m, const BYTE* inv, BYTE* object, void* out); KING_ALIAS("_Rq_Collide", "_SceneObj_CollideShape")
extern "C" void Rq_5330a0(); KING_ALIAS("_Rq_5330a0", "_Car_DropToGround")

KING_VAR(float, g_rq702620, 702620)
KING_VAR(float, g_rq702624, 702624)
KING_VAR(float, g_rq702628, 702628)
KING_VAR(float, g_rq702630, 702630)
KING_VAR(float, g_rq702634, 702634)
KING_VAR(float, g_rq702638, 702638)
KING_VAR(float, g_rq702640, 702640)
KING_VAR(float, g_rq702644, 702644)
KING_VAR(float, g_rq702648, 702648)
KING_VAR(int, g_rq702650, 702650)
KING_VAR(int, g_rq702654, 702654)
KING_VAR(int, g_rq702658, 702658)
KING_VAR(float, g_rq64d9c0, 64d9c0)
KING_VAR(float, g_rq64c350, 64c350)
KING_VAR(float, g_rq64c320, 64c320)
KING_VAR(float, g_rq64d2f8, 64d2f8)
KING_VAR(float, g_rq64c450, 64c450)
KING_VAR(double, g_rq64b678, 64b678)
KING_VAR(double, g_rq64b668, 64b668)
KING_VAR(double, g_rq64b6e0, 64b6e0)
KING_VAR(double, g_rq64b670, 64b670)
KING_VAR(double, g_rq64b660, 64b660)
KING_VAR(int, g_rq68b990, 68b990)
KING_VAR(char, g_rq6cf4b8, 6cf4b8)
KING_VAR(char, g_rq678b2c, 678b2c)
KING_VAR(char, g_rq678b64, 678b64)
KING_VAR(int, g_rq6cf428, 6cf428)
KING_VAR(int, g_rq6cf42c, 6cf42c)
KING_VAR(int, g_rq6cf430, 6cf430)
KING_VAR(int, g_rq6cf4ac, 6cf4ac)
KING_VAR(int, g_rq6cf4b0, 6cf4b0)
KING_VAR(int, g_rq6cf4b4, 6cf4b4)
KING_VAR(BYTE, g_rq697f9c, 697f9c)
KING_VAR(BYTE*, g_rq6f653c, 6f653c)
KING_VAR(BYTE, g_rq6f6968, 6f6968)
KING_VAR(BYTE, g_rq66d638, 66d638)
KING_VAR(BYTE, g_rq66d628, 66d628)

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))
#define P(n) (*(BYTE**)(fr + (n)))

static Vec3 Rq_V(BYTE* fr, int n) { return *(Vec3*)(fr + n); }
static void Rq_Copy(BYTE* fr, int to, const void* from, int bytes) { memcpy(fr + to, from, bytes); }

extern "C" {

// [FUN_533590..$L_53549a] the vehicle put back at a free place (this +616 the car, +32 the view): up to 15 tries
// along the car's line (+-5/2 steps), each a place from the room (a road position, a named start, the garage)
// with its height from a ray down, then a few slides sideways until no other car's box (list 6f653c) and none of
// the car's points hit the world; the car and the view are set there. Returns the place's code (+10 when the
// ray met the ground, +100 / -100 after the last try, +20 when no ground was found). The asm's frame is kept
// as fr (V float, I int, P pointer slots at the asm's ESP offsets)
int __fastcall Car_PlaceOnRoad(BYTE* s, void*, int special)
{
    BYTE fr[3100];
    I(36) = (int)s;
    I(40) = 0;
    I(72) = 0;
    if (!F(BYTE*, s, 616))
    {
        CallThis(Rq_SetCam, F(BYTE*, s, 32), F(int, s, 100), s + 52);
        CallThis(Rq_ObjReset, &g_rq6f6968);
        return I(72) + I(40);
    }
    if (CallThis<int>(Rq_EffGet, F(BYTE*, s, 616) + 21568) == 1
        || CallThis<int>(Rq_EffGet, F(BYTE*, s, 616) + 21568) == 2)
        CallThis(Rq_EffEnd, F(BYTE*, s, 616), 1);
    BYTE* fx = F(BYTE*, F(BYTE*, s, 616), 21600);
    if (fx)
        F(short, fx, 10056) = 1;
    CallVirt(F(BYTE*, s, 616), 92, &g_rq702650);
    CallVirt(F(BYTE*, s, 616), 96, &g_rq702650);
    I(44) = g_rq702650; I(48) = g_rq702654; I(52) = g_rq702658;
    I(24) = g_rq702650; I(28) = g_rq702654; I(32) = g_rq702658;
    I(16) = 0;
    I(240) = -1; I(244) = -1; I(248) = 0; I(252) = 0;
    I(256) = 0; I(260) = 0; I(264) = 0; I(268) = -1;
    BYTE* m = CallThis<BYTE*>(Rq_Matrix1, F(BYTE*, s, 616) + 16, fr + 1092);
    Rq_Copy(fr, 152, m + 36, 12);
    m = CallThis<BYTE*>(Rq_Matrix1, F(BYTE*, s, 616) + 16, fr + 1092);
    {
        double a = (g_rq702628 * F(float, m, 24) + g_rq702624 * F(float, m, 12)) + g_rq702620 * F(float, m, 0);
        double b = (g_rq702628 * F(float, m, 28) + g_rq702624 * F(float, m, 16)) + g_rq702620 * F(float, m, 4);
        double c = (g_rq702628 * F(float, m, 32) + g_rq702624 * F(float, m, 20)) + g_rq702620 * F(float, m, 8);
        V(88) = (float)(c * g_rq702634 - b * g_rq702638);
        V(92) = (float)(a * g_rq702638 - c * g_rq702630);
        double n2 = b * g_rq702630 - a * g_rq702634;
        V(100) = (float)(V(92) * g_rq702638 - n2 * g_rq702634);
        I(164) = I(100);
        V(104) = (float)(n2 * g_rq702630 - V(88) * g_rq702638);
        I(168) = I(104);
        V(112) = (float)(V(88) * g_rq702634 - V(92) * g_rq702630);
        I(172) = I(112);
        Rq_Norm(&V(1036), &V(164));
        double ang = x87_atan2(V(1036), V(1040));
        BYTE* r = Rq_FromPA(fr + 1324, Rq_V(fr, 152), (float)-ang, 0.0f, 0.0f);
        Rq_Copy(fr, 432, r, 48);
    }
    P(124) = CallThis<BYTE*>(Rq_Get800, F(BYTE*, s, 616) + 16);
    I(60) = 0;
    for (;;)    // $L_53386f: the tries
    {
        I(16) = 0;
        if (I(60) != 0)
        {
            V(508) = (float)(g_rq702620 * g_rq64d9c0);
            V(512) = (float)(g_rq702624 * g_rq64d9c0);
            V(516) = (float)(g_rq702628 * g_rq64d9c0);
            BYTE* t = Rq_Transl(fr + 2280, Rq_V(fr, 508));
            Rq_Mul(fr + 1144, t, fr + 432);
            s = P(36);
            if (F(int, F(BYTE*, s, 616), 20636) == g_rq68b990)
            {
                BYTE* r;
                if (I(60) & 2)
                {
                    I(56) = I(60) * 5 / 2;
                    double k = I(56);
                    V(528) = (float)(k * g_rq702620);
                    V(532) = (float)(k * g_rq702624);
                    V(536) = (float)(k * g_rq702628);
                    t = Rq_Transl(fr + 2520, Rq_V(fr, 528));
                    r = Rq_Mul(fr + 2380, t, fr + 1324);
                }
                else
                {
                    I(56) = (-1 - I(60)) * 5 / 2;
                    double k = I(56);
                    V(392) = (float)(k * g_rq702620);
                    V(396) = (float)(k * g_rq702624);
                    V(400) = (float)(k * g_rq702628);
                    t = Rq_Transl(fr + 1848, Rq_V(fr, 392));
                    r = Rq_Mul(fr + 1996, t, fr + 1324);
                }
                Rq_Copy(fr, 1144, r, 48);
            }
            BYTE* h = Rq_Hit(P(124), fr + 468, fr + 1180);
            Rq_Copy(fr, 468, fr + 1180, 12);
            P(124) = h;
        }
        else
            s = P(36);
        Rq_Copy(fr, 152, fr + 468, 12);
        int code;
        if (F(int, F(BYTE*, s, 616), 20636) != g_rq68b990)
        {
            if (special)
            {
                code = Rq_ObjAt(F(void*, P(124), 12), fr + 152, fr + 16, fr + 240, 1);
                I(40) = code;
            }
            else
            {
                code = Rq_ObjAt(s + 228, s + 204, fr + 16, fr + 240, 1);
                if (code > 0)
                {
                    code += 2;
                    I(40) = code;
                }
                else
                {
                    BYTE* o = CallThis<BYTE*>(Rq_Get800, F(BYTE*, s, 616) + 16);
                    BYTE* mm = CallThis<BYTE*>(Rq_Matrix1, F(BYTE*, s, 616) + 16, fr + 2332);
                    code = Rq_ObjAt(F(void*, o, 12), mm + 36, fr + 16, fr + 240, 1);
                    I(40) = code;
                }
            }
        }
        else
            code = I(40);

        if (I(16) == 0)
        {
            P(16) = Rq_FindNamed(0, &g_rq6cf4b8);
            if (P(16))
            {
                I(44) = g_rq6cf428; I(48) = g_rq6cf42c; I(52) = g_rq6cf430;
                I(24) = g_rq6cf4ac; I(28) = g_rq6cf4b0; I(32) = g_rq6cf4b4;
                I(40) = 5;
                V(176) = 30.0f;
            }
            else
            {
                BYTE* car = F(BYTE*, s, 616);
                if (I(60) == 0)
                {
                    P(16) = Rq_FindNamed(0, (const char*)car + 20260);
                    if (!P(16))
                    {
                        BYTE* o = CallThis<BYTE*>(Rq_Get800, F(BYTE*, s, 616) + 16);
                        P(16) = o;
                        sprintf((char*)fr + 2572, &g_rq678b2c, &g_rq678b64, F(BYTE*, s, 616) + 20260, F(int, o, 12));
                        Rq_Log((char*)fr + 2572, 0);
                    }
                    car = F(BYTE*, s, 616);
                    Rq_Copy(fr, 44, car + 20248, 12);
                    V(308) = (float)((g_rq702628 * F(float, car, 20236) + g_rq702624 * F(float, car, 20224))
                                     + g_rq702620 * F(float, car, 20212));
                    V(312) = (float)((g_rq702628 * F(float, car, 20240) + g_rq702624 * F(float, car, 20228))
                                     + g_rq702620 * F(float, car, 20216));
                    I(28) = I(312);
                    I(24) = I(308);
                    V(316) = (float)((g_rq702628 * F(float, car, 20244) + g_rq702624 * F(float, car, 20232))
                                     + g_rq702620 * F(float, car, 20220));
                    I(32) = I(316);
                }
                else
                {
                    BYTE* h = Rq_FindNamed(0, (const char*)car + 20260);
                    Rq_Copy(fr, 44, fr + 468, 12);
                    P(16) = h;
                    P(16) = Rq_Hit(h, F(BYTE*, s, 616) + 20248, fr + 44);
                }
                I(40) = 6;
                V(176) = 30.0f;
            }
        }
        else
        {
            // a road place: its right side's coordinates and tangent, the room's width
            BYTE* car = F(BYTE*, s, 616);
            RqPos rp = *(RqPos*)(fr + 240);
            RqPos* r = Rq_Right((RqPos*)(fr + 1612), F(BYTE*, car, 20636), rp);
            *(RqPos*)(fr + 240) = *r;
            float* cv = Rq_Coords((float*)(fr + 1204), *r);
            Rq_Copy(fr, 44, cv, 12);
            float* tg = Rq_Tangent((float*)(fr + 1252), *(RqPos*)(fr + 240));
            float* nv = Rq_Norm((float*)(fr + 1264), tg);
            Rq_Copy(fr, 24, nv, 12);
            I(1084) = -1;
            I(1088) = -1;
            I(1048) = 0;
            I(1056) = 0;
            I(1060) = 0;
            P(1064) = &g_rq66d638;
            P(1068) = &g_rq66d628;
            I(1072) = 0;
            I(1076) = 0;
            Rq_RoomInfo(*(RqPos*)(fr + 240), fr + 1048);
            double w = *(double*)(fr + 1056);
            V(20) = (float)(w * g_rq64b6e0 - g_rq64b668);
            V(176) = (float)w;

            bool back = false;
            if ((code == 3 || code == 4)
                && (F(float, s, 224) * V(32) + F(float, s, 220) * V(28)) + F(float, s, 216) * V(24) < g_rq64b678)
                back = true;
            if (!back && (code == 1 || code == 2))
            {
                BYTE* mm = CallThis<BYTE*>(Rq_Matrix1, F(BYTE*, s, 616) + 16, fr + 2092);
                double a = (g_rq702628 * F(float, mm, 24) + g_rq702624 * F(float, mm, 12)) + g_rq702620 * F(float, mm, 0);
                double b = (g_rq702628 * F(float, mm, 28) + g_rq702624 * F(float, mm, 16)) + g_rq702620 * F(float, mm, 4);
                double c = (g_rq702628 * F(float, mm, 32) + g_rq702624 * F(float, mm, 20)) + g_rq702620 * F(float, mm, 8);
                if ((c * V(32) + b * V(28)) + a * V(24) < g_rq64b678)
                    back = true;
            }
            if (!back)
            {
                Rq_IsPlace(fr + 240);
                double cx = V(28) * g_rq702638 - V(32) * g_rq702634;
                V(932) = (float)(V(32) * g_rq702630 - V(24) * g_rq702638);
                V(936) = (float)(V(24) * g_rq702634 - V(28) * g_rq702630);
                V(916) = (float)(cx * V(20));
                V(920) = (float)(V(932) * V(20));
                V(924) = (float)(V(936) * V(20));
                CallThis(Rq_AddA, fr + 44, fr + 1192, fr + 916);
            }
            else if (Rq_IsPlace(fr + 240))
            {
                double cx = V(28) * g_rq702638 - V(32) * g_rq702634;
                V(884) = (float)(V(32) * g_rq702630 - V(24) * g_rq702638);
                V(888) = (float)(V(24) * g_rq702634 - V(28) * g_rq702630);
                V(580) = (float)(cx * V(20));
                V(584) = (float)(V(884) * V(20));
                V(588) = (float)(V(888) * V(20));
                CallThis(Rq_AddA, fr + 44, fr + 1216, fr + 580);
                V(348) = -V(24);
                V(352) = -V(28);
                V(356) = -V(32);
                I(24) = I(348); I(28) = I(352); I(32) = I(356);
            }
            else
            {
                V(112) = -V(24);
                V(116) = -V(28);
                V(120) = -V(32);
                I(24) = I(112); I(28) = I(116); I(32) = I(120);
                double cx = V(116) * g_rq702638 - V(120) * g_rq702634;
                V(800) = (float)(V(120) * g_rq702630 - V(112) * g_rq702638);
                V(804) = (float)(V(112) * g_rq702634 - V(116) * g_rq702630);
                V(604) = (float)(cx * V(20));
                V(608) = (float)(V(800) * V(20));
                V(612) = (float)(V(804) * V(20));
                CallThis(Rq_AddA, fr + 44, fr + 1240, fr + 604);
            }
        }

        // $L_534393: the car's pose at the place, its height from a ray down
        {
            double ang = x87_atan2(V(24), V(28));
            BYTE* r = Rq_FromPA(fr + 2428, Rq_V(fr, 44), (float)-ang, 0.0f, 0.0f);
            Rq_Copy(fr, 432, r, 48);
        }
        I(20) = I(16);
        I(140) = 0; I(144) = 0; I(148) = 0;
        I(128) = 0; I(132) = 0; I(136) = 0;
        int j = 1;
        I(56) = j;
        for (;;)
        {
            double t = x87_pow(g_rq64b660, (double)I(56));
            V(76) = (float)(t * g_rq702630);
            V(80) = (float)(t * g_rq702634);
            V(84) = (float)(t * g_rq702638);
            V(808) = (float)(V(44) - V(76));
            V(812) = (float)(V(48) - V(80));
            V(816) = (float)(V(52) - V(84));
            V(628) = (float)(V(44) + V(76));
            V(632) = (float)(V(48) + V(80));
            V(636) = (float)(V(52) + V(84));
            if (Rq_Ray(P(20), fr + 628, fr + 808, fr + 140, fr + 128) & 1)
            {
                double n0 = V(128) * g_rq64c350;
                V(836) = (float)(V(132) * g_rq64c350);
                V(840) = (float)(V(136) * g_rq64c350);
                V(100) = (float)(n0 + V(140));
                V(104) = (float)(V(144) + V(836));
                V(108) = (float)(V(148) + V(840));
                P(20) = Rq_Hit(P(20), fr + 468, fr + 100);
                Rq_Copy(fr, 468, fr + 100, 12);
                break;
            }
            V(856) = (float)(V(44) - V(76));
            V(860) = (float)(V(48) - V(80));
            V(864) = (float)(V(52) - V(84));
            V(652) = (float)(V(44) + V(76));
            V(656) = (float)(V(48) + V(80));
            V(660) = (float)(V(52) + V(84));
            if (Rq_WorldHit(P(20), fr + 652, fr + 856, fr + 140, fr + 128) & 1)
            {
                double n0 = V(128) * g_rq64c350;
                V(680) = (float)(V(132) * g_rq64c350);
                V(684) = (float)(V(136) * g_rq64c350);
                V(88) = (float)(n0 + V(140));
                V(92) = (float)(V(144) + V(680));
                V(96) = (float)(V(148) + V(684));
                P(20) = Rq_Hit(P(20), fr + 468, fr + 88);
                Rq_Copy(fr, 468, fr + 88, 12);
                I(72) = 10;
                break;
            }
            if (j == 15)
            {
                // $L_535439: no ground under any try
                s = P(36);
                CallC<int>(Rq_5330a0, F(BYTE*, s, 616), F(BYTE*, s, 32));
                return I(40) + 20;
            }
            j++;
            I(56) = j;
        }

        // $L_5347ee: slides sideways
        int cnt = (int)((V(176) - g_rq64b668) * g_rq64b6e0 + g_rq64b670);
        Rq_Copy(fr, 180, fr + 432, 48);
        I(16) = I(20);
        I(64) = cnt;
        if (cnt < 1)
        {
            cnt = 1;
            I(64) = 1;
        }
        bool placed = false;
        for (I(68) = 0; I(68) < I(64); I(68)++)
        {
            {
                double nx = -g_rq702640;
                V(848) = -g_rq702644;
                V(852) = -g_rq702648;
                V(56) = (float)I(68);
                double x = nx * V(56);
                V(548) = (float)(V(56) * V(848));
                V(552) = (float)(V(56) * V(852));
                V(360) = (float)(x + x);
                V(364) = (float)(V(548) + V(548));
                V(368) = (float)(V(552) + V(552));
                BYTE* t = Rq_Transl(fr + 2188, Rq_V(fr, 360));
                BYTE* r = Rq_Mul(fr + 2476, t, fr + 432);
                double ux = -g_rq702620;
                V(896) = -g_rq702624;
                V(900) = -g_rq702628;
                Rq_Copy(fr, 180, r, 48);
                V(384) = (float)(ux * g_rq64c320);
                V(388) = (float)(V(896) * g_rq64c320);
                V(392) = (float)(V(900) * g_rq64c320);
                t = Rq_Transl(fr + 1900, Rq_V(fr, 384));
                Rq_Mul(fr + 1096, t, fr + 180);
            }
            s = P(36);
            BYTE* tow = 0;
            if (CallVirt<int>(F(BYTE*, s, 616), 4) == 1)
            {
                BYTE* car = F(BYTE*, s, 616);
                tow = F(BYTE*, car, 21880);
                if (tow)
                {
                    // the trailer: its hitch point moved with the car
                    Rq_Copy(fr, 988, car + 708, 48);
                    BYTE* T = &g_rq697f9c + 2416 * F(int, car, 10420);
                    Rq_Copy(fr, 940, tow + 708, 48);
                    V(408) = (float)(((V(1012) * F(float, T, 164) + V(1000) * F(float, T, 160)) + V(988) * F(float, T, 156)) + V(1024));
                    V(412) = (float)(((V(1016) * F(float, T, 164) + V(1004) * F(float, T, 160)) + V(992) * F(float, T, 156)) + V(1028));
                    V(416) = (float)(((V(1020) * F(float, T, 164) + V(1008) * F(float, T, 160)) + V(996) * F(float, T, 156)) + V(1032));
                    double h = F(float, T, 204);
                    BYTE* T2 = &g_rq697f9c + 2416 * F(int, tow, 10420) + 168;
                    V(724) = (float)(h * g_rq702620);
                    V(728) = (float)(h * g_rq702624);
                    double hz = h * g_rq702628;
                    V(228) = (float)(V(724) + F(float, T2, 0));
                    V(232) = (float)(V(728) + F(float, T2, 4));
                    double e2 = hz + F(float, T2, 8);
                    float wz = (float)(((V(228) * V(948) + e2 * V(972)) + V(232) * V(960)) + V(984));
                    float wy = (float)(((V(228) * V(944) + e2 * V(968)) + V(232) * V(956)) + V(980));
                    float wx = (float)(((V(940) * V(228) + e2 * V(964)) + V(232) * V(952)) + V(976));
                    CallThis(Rq_SetThis, fr + 868, wx, wy, wz);
                    float dz = (float)(V(416) - V(876));
                    float dy = (float)(V(412) - V(872));
                    float dx = (float)(V(408) - V(868));
                    CallThis(Rq_SetThis, fr + 748, dx, dy, dz);
                    BYTE* t = Rq_Transl(fr + 1948, Rq_V(fr, 748));
                    BYTE* r = Rq_Mul(fr + 2044, t, fr + 180);
                    Rq_Copy(fr, 1096, r, 48);
                }
            }

            // $L_534c8e: other cars' boxes
            BYTE* e = g_rq6f653c;
            bool hit = false;
            if (e)
                e -= 4;
            while (e)
            {
                BYTE* o = e - 376;
                BYTE* car = F(BYTE*, P(36), 616);
                if (o != (car ? car + 16 : 0) && o != (tow ? tow + 16 : 0))
                {
                    float z = (float)(F(float, o, 3380) - V(224));
                    float y = (float)(F(float, o, 3376) - V(220));
                    float x = (float)(F(float, o, 3372) - V(216));
                    CallThis(Rq_SetThis, fr + 820, x, y, z);
                    Rq_Copy(fr, 772, fr + 820, 12);
                    car = F(BYTE*, P(36), 616);
                    double len = Rq_Len(&V(772));
                    if ((F(float, car, 3288) + F(float, o, 3272)) * g_rq64b668 > len)
                    {
                        I(332) = 0;
                        BYTE* inv = Rq_Inv(fr + 2140, fr + 180);
                        if (Rq_Collide(car ? car + 16 : 0, fr + 180, inv, o, fr + 332))
                        {
                            hit = true;
                            break;
                        }
                    }
                    if (tow)
                    {
                        z = (float)(F(float, o, 3380) - V(1140));
                        y = (float)(F(float, o, 3376) - V(1136));
                        x = (float)(F(float, o, 3372) - V(1132));
                        CallThis(Rq_SetThis, fr + 556, x, y, z);
                        Rq_Copy(fr, 568, fr + 556, 12);
                        len = Rq_Len(&V(568));
                        if ((F(float, tow, 3288) + F(float, o, 3272)) * g_rq64b668 > len)
                        {
                            I(492) = 0;
                            BYTE* inv = Rq_Inv(fr + 2236, fr + 180);
                            if (Rq_Collide(tow + 16, fr + 180, inv, o, fr + 492))
                            {
                                hit = true;
                                break;
                            }
                        }
                    }
                }
                e = F(BYTE*, e, 8);
                if (!e)
                    break;
                e -= 4;
            }
            if (hit)
                continue;

            // $L_534ec8: the ground under the car and its points free
            P(16) = Rq_Hit(P(20), fr + 468, fr + 216);
            CallThis(Rq_NodeInit, fr + 1644);
            {
                float z = (float)(g_rq702638 + g_rq702638);
                float y = (float)(g_rq702634 + g_rq702634);
                float x = (float)(g_rq702630 + g_rq702630);
                CallThis(Rq_SetThis, fr + 592, x, y, z);
                z = (float)(V(600) + V(224));
                y = (float)(V(596) + V(220));
                x = (float)(V(592) + V(216));
                CallThis(Rq_SetThis, fr + 616, x, y, z);
                Rq_Copy(fr, 164, fr + 616, 12);
                z = (float)(g_rq702638 * g_rq64d2f8);
                y = (float)(g_rq702634 * g_rq64d2f8);
                x = (float)(g_rq702630 * g_rq64d2f8);
                CallThis(Rq_SetThis, fr + 640, x, y, z);
                z = (float)(V(224) - V(648));
                y = (float)(V(220) - V(644));
                x = (float)(V(216) - V(640));
                CallThis(Rq_SetThis, fr + 664, x, y, z);
                Rq_Copy(fr, 688, fr + 664, 12);
            }
            BYTE* h3 = Rq_Hit(P(16), fr + 216, fr + 164);
            BYTE* car = F(BYTE*, P(36), 616);
            if (!(Rq_Blocked(Rq_IdSame(car ? car + 16 : 0), h3, fr + 164, fr + 688, fr + 1644) & 1))
                continue;
            if (Rq_MovingOf(P(1760)))
                continue;
            for (int k = 0; k < 20; k++)
                CallThis(Rq_Zero, fr + 1372 + 12 * k);
            for (int k = 0; k < 20; k++)
                CallThis(Rq_Zero, fr + 2772 + 12 * k);
            s = P(36);
            int n = 0;
            if (CallThis<int>(Rq_PtsNext, F(BYTE*, s, 616) + 16, fr + 1372))
            {
                do
                {
                    if (n != -1)
                    {
                        int o12 = 12 * n;
                        float px = V(1372 + o12), py = V(1376 + o12), pz = V(1380 + o12);
                        float z = (float)(((V(188) * px + V(212) * pz) + V(200) * py) + V(224));
                        float y = (float)(((V(184) * px + V(208) * pz) + V(196) * py) + V(220));
                        float x = (float)(((V(204) * pz + V(192) * py) + V(180) * px) + V(216));
                        CallThis(Rq_SetThis, fr + 712, x, y, z);
                        BYTE* pt = fr + 2772 + o12;
                        z = (float)(g_rq702638 * g_rq64c450);
                        I(2772 + o12) = I(712);
                        I(2776 + o12) = I(716);
                        y = (float)(g_rq702634 * g_rq64c450);
                        I(2780 + o12) = I(720);
                        x = (float)(g_rq702630 * g_rq64c450);
                        CallThis(Rq_SetThis, fr + 736, x, y, z);
                        z = (float)(V(744) + F(float, pt, 8));
                        y = (float)(V(740) + F(float, pt, 4));
                        x = (float)(V(736) + F(float, pt, 0));
                        CallThis(Rq_SetThis, fr + 760, x, y, z);
                        Rq_Copy(fr, 784, fr + 760, 12);
                        BYTE* h4 = Rq_Hit(P(16), fr + 216, fr + 784);
                        if (!h4)
                            h4 = P(16);
                        BYTE* car2 = F(BYTE*, s, 616);
                        BYTE* sh = car2 ? car2 + 16 : 0;
                        float* lo = Rq_Sub((float*)(fr + 1312), (float*)pt,
                                           Rq_Scaled((float*)(fr + 1288), &g_rq702630, 10.0f));
                        float* hi = Rq_Add((float*)(fr + 1228), (float*)pt,
                                           Rq_Scaled((float*)(fr + 1300), &g_rq702630, 10.0f));
                        if ((Rq_Blocked(Rq_IdSame(sh), h4, hi, lo, fr + 1644) & 1) && !Rq_MovingOf(P(1760)))
                            n++;
                        else
                            n = -1;
                        s = P(36);
                    }
                }
                while (CallThis<int>(Rq_PtsNext, F(BYTE*, s, 616) + 16, fr + 1372 + 12 * n));
                if (n == -1)
                    continue;
            }
            // $L_5353af: placed
            CallVirt(F(BYTE*, s, 616), 76, P(16), fr + 180, 0);
            CallThis(Rq_SetCam, F(BYTE*, s, 32), P(16), fr + 180);
            placed = true;
            break;
        }
        if (placed)
            break;
        I(60)++;
        if (I(60) >= 15)
            break;
    }
    if (I(60) == 15)
    {
        s = P(36);
        if (CallC<int>(Rq_5330a0, F(BYTE*, s, 616), F(BYTE*, s, 32)))
            return I(72) + I(40) + 100;
        return -100 - I(72) - I(40);
    }
    CallThis(Rq_ObjReset, &g_rq6f6968);
    return I(72) + I(40);
}

} // extern "C"

#undef V
#undef I
#undef P

// === the car dropped onto the ground under its position (FUN_5330a0 with its tail $L_533153)

extern "C" void __cdecl Rd_Decompose(const BYTE* m, float* out); KING_ALIAS("_Rd_Decompose", "_Matrix_Decompose6FArray")
extern "C" int __cdecl Rd_Ray(BYTE* a, const void* p, const void* d, void* point, void* normal); KING_ALIAS("_Rd_Ray", "_Query_4e6749")
extern "C" BYTE* __cdecl Rd_FromPose(BYTE* out, double x, double y, double z, double a, double b, double c); KING_ALIAS("_Rd_FromPose", "_Matrix_FromPose")
KING_VAR(double, g_rd64b780, 64b780)

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))

extern "C" {

// [FUN_5330a0..$L_533153] car o (+16 its scene object) dropped onto what lies under it: rays down from 1, 2, 3 and
// 4 steps above its position (the steps growing by 64b780), the first hit lifted by 64c350; the car posed there
// with its heading and the view v set on it. Returns 0 when no ray hit
int __cdecl Car_DropToGround(BYTE* o, BYTE* v)
{
    BYTE fr[288];
    BYTE* m = CallThis<BYTE*>(Rq_Matrix1, o + 16, fr + 248);
    Rd_Decompose(m, &V(176));
    CallThis(Rq_Matrix1, o + 16, fr + 128);
    I(20) = I(168);
    I(16) = I(164);
    I(24) = I(172);
    I(56) = 0;
    I(60) = 0;
    I(64) = 0;
    I(116) = 0;
    I(120) = 0;
    I(124) = 0;
    BYTE* obj = CallThis<BYTE*>(Rq_Get800, o + 16);
    int i = 0;
    for (I(40) = 0; ; )
    {
        double t = x87_pow(g_rd64b780, (double)I(40)) + g_rq64b670;
        V(104) = (float)(g_rq702630 * t);
        V(108) = (float)(g_rq702634 * t);
        double tz = g_rq702638 * t;
        V(68) = V(16) - V(104);
        I(92) = I(68);
        V(72) = V(20) - V(108);
        I(96) = I(72);
        V(76) = (float)(V(24) - tz);
        I(100) = I(76);
        V(80) = (float)(g_rq702630 * t);
        V(84) = (float)(g_rq702634 * t);
        tz = g_rq702638 * t;
        V(28) = V(80) + V(16);
        I(44) = I(28);
        V(32) = V(84) + V(20);
        I(48) = I(32);
        V(36) = (float)(tz + V(24));
        I(52) = I(36);
        if (Rd_Ray(obj, fr + 44, fr + 92, fr + 56, fr + 116) & 1)
        {
            double lx = g_rq702630 * g_rq64c350;
            V(48) = (float)(g_rq702634 * g_rq64c350);
            V(52) = (float)(g_rq702638 * g_rq64c350);
            V(28) = (float)(lx + V(56));
            I(16) = I(28);
            V(32) = V(48) + V(60);
            I(20) = I(32);
            V(36) = V(52) + V(64);
            I(24) = I(36);
            break;
        }
        i++;
        I(40) = i;
        if (i >= 4)
            break;
    }
    if (i == 4)
        return 0;
    BYTE* pose = Rd_FromPose(fr + 200, V(16), V(20), V(24), V(188), 0.0, 0.0);
    (void)pose;
    BYTE* hit = Rq_Hit(obj, fr + 164, fr + 16);
    CallVirt(o, 76, hit, fr + 200, 0);
    CallThis(Rq_SetCam, v, hit, fr + 200);
    return 1;
}

} // extern "C"

#undef V
#undef I

// === the cab view's sway from the car's motion (FUN_535a70 with its tails $L_535adc, $L_535b11)

extern "C" Vec3* __fastcall Vec3_MulAssign(Vec3* self, void*, Vec3* ret, float s);
extern "C" Vec3* __fastcall Vec3_AddAssign(Vec3* self, void*, Vec3* ret, const Vec3* v);
KING_VAR(BYTE*, g_cs696cb0, 696cb0)
KING_VAR(float, g_cs64f3b8, 64f3b8)
KING_VAR(float, g_cs64f3a4, 64f3a4)
KING_VAR(float, g_cs64f3a0, 64f3a0)
KING_VAR(float, g_cs64c250, 64c250)
KING_VAR(float, g_cs64c338, 64c338)
KING_VAR(float, g_cs64da78, 64da78)
KING_VAR(float, g_cs64d2f8, 64d2f8)
KING_VAR(float, g_cs64c904, 64c904)
KING_VAR(float, g_cs64c408, 64c408)
KING_VAR(float, g_cs64d694, 64d694)
KING_VAR(float, g_cs702630, 702630)
KING_VAR(float, g_cs702634, 702634)
KING_VAR(float, g_cs702638, 702638)
KING_VAR(double, g_cs64b780, 64b780)
KING_VAR(double, g_cs64b898, 64b898)
KING_VAR(double, g_cs64b670, 64b670)
KING_VAR(double, g_cs64b5b8, 64b5b8)
KING_VAR(double, g_cs64c958, 64c958)
KING_VAR(double, g_cs64d0f0, 64d0f0)
KING_VAR(double, g_cs64f3b0, 64f3b0)
KING_VAR(double, g_cs64f3a8, 64f3a8)
KING_VAR(double, g_cs64b7a8, 64b7a8)
KING_VAR(double, g_cs64dd78, 64dd78)
KING_VAR(double, g_cs64f398, 64f398)
KING_VAR(double, g_cs64f390, 64f390)
KING_VAR(double, g_cs64f388, 64f388)

static float Cs_Bits(DWORD b) { return *(float*)&b; }

extern "C" {

// [FUN_535a70..$L_535b11] cab view s of car +616 (its body +21600): the sideways (+13232) and forward (+13228)
// pulls, eased into the camera's lean (696cb0 +5084 / +5088); the wheels' loads (+15248) turning their arms
// (+13252) into a tilt, the bounce height (+1616) and offset (+1620) eased by the speed (+13100); the cab frame
// (+1520) the tilt's two turns and the offset, identity without a car
void __fastcall Cab_Sway(BYTE* s)
{
    BYTE* car = F(BYTE*, s, 616);
    BYTE* v = car ? F(BYTE*, car, 21600) : 0;
    if (!v)
    {
        memcpy(s + 1520, &g_identity, 48);
        return;
    }
    float side = (float)(F(float, v, 13232) * g_cs64f3b8);
    float fwd = (float)(F(float, v, 13228) * g_cs64f3b8);
    double ex = Game_FrameTime() * g_cs64b780;
    float k = (float)x87_pow(g_cs64b898, ex);
    double a = x87_fabs(F(float, v, 13100));
    float kb = (float)x87_pow((g_cs64b670 - a / (a + g_cs64b6d0)) * g_cs64b5b8 + g_cs64c958, ex);
    double y;
    if (F(int, v, 10092) || F(int, v, 16204) == 1)
    {
        side = (float)(side * g_cs64d0f0);
        y = fwd * g_cs64d0f0;
    }
    else
        y = fwd;
    if (!(x87_fabs(side) <= g_cs64f3b0))
        side = side > g_cs64c264 ? Cs_Bits(1026734274) : Cs_Bits(3174217922u);
    if (!(x87_fabs(y) <= g_cs64f3a8))
        y = y > g_cs64c264 ? g_cs64f3a4 : g_cs64f3a0;
    BYTE* cam = g_cs696cb0;
    double m = g_cs64c250 - k;
    F(float, cam, 5084) = (float)(k * F(float, cam, 5084) + m * side);
    F(float, cam, 5088) = (float)(k * F(float, cam, 5088) + m * y);

    // $L_535c2f: the wheels' arms turned by their loads
    float load = 0.0f;
    Vec3 tilt = g_zeroVec;
    for (int j = 0; j < 4; j++)
    {
        const float* p = (const float*)(v + 13252 + 12 * j);
        float w = F(float, v, 15248 + 4 * j);
        load = load + w;
        Vec3 c;
        c.x = (float)(g_cs702634 * p[2] - g_cs702638 * p[1]);
        c.y = (float)(g_cs702638 * p[0] - g_cs702630 * p[2]);
        c.z = (float)(g_cs702630 * p[1] - g_cs702634 * p[0]);
        float r = (float)(g_cs64b7a8 / x87_sqrt((double)p[0] * p[0] + (double)p[1] * p[1]));
        double as = x87_asin(r * w);
        Vec3 t;
        Vec3_MulAssign(&c, 0, &t, (float)((as + as) * r));
        Vec3_AddAssign(&tilt, 0, &t, &c);
    }
    double h = load * g_cs64d0f0;
    if (!(h <= g_cs64d0f0))
        h = g_cs64c338;
    if (h < g_cs64dd78)
        h = g_cs64da78;
    double c1 = g_cs64c250 - kb;
    float kc = (float)c1;
    F(float, s, 1616) = (float)(c1 * h + kb * F(float, s, 1616));
    if (F(float, v, 13104) < g_cs64d2f8)
    {
        Vec3 t;
        Vec3_MulAssign(&tilt, 0, &t, (float)x87_fabs(F(float, v, 13104) * g_cs64c904));
    }
    if (!(tilt.x <= g_cs64b840))
        tilt.x = Cs_Bits(1045220557);
    if (tilt.x < g_cs64f398)
        tilt.x = Cs_Bits(3192704205u);
    if (!(tilt.y <= g_cs64f390))
        tilt.y = Cs_Bits(1035087118);
    if (tilt.y < g_cs64f388)
        tilt.y = Cs_Bits(3182570766u);

    // $L_535df0: the offset eased, then the frame
    float q0 = kc * tilt.x, q1 = kc * tilt.y;
    double q2 = (double)kc * tilt.z;
    double r0 = (double)kb * F(float, s, 1620);
    float r1 = kb * F(float, s, 1624), r2 = kb * F(float, s, 1628);
    F(float, s, 1620) = (float)(r0 + q0);
    F(float, s, 1624) = q1 + r1;
    F(float, s, 1628) = (float)(q2 + r2);
    float X = (float)(-(double)F(float, cam, 5084) - F(float, s, 1620));
    double Yd = (double)F(float, cam, 5088) - F(float, s, 1624);
    float Y = (float)Yd;
    Vec3 d;
    d.x = (float)(Yd * g_cs64c408);
    d.y = (float)(X * g_cs64d694);
    d.z = (float)(F(float, s, 1616) - ((double)Y * Y + (double)X * X) * g_cs64c408);
    Matrix T, R1, R2, M, out;
    Matrix* t = Matrix_Translation(&T, d);
    Matrix* r1m = Matrix_RotationY(&R1, (float)(Y * g_cs64b5b8));
    Matrix* r2m = Matrix_RotationX(&R2, X);
    Matrix* mm = Matrix_Multiply(&M, r2m, r1m);
    memcpy(s + 1520, Matrix_Multiply(&out, mm, t), 48);
}

} // extern "C"

// === the cab camera's frame in the car (FUN_4de990)

KING_VAR(BYTE*, g_cv6d2078, 6d2078)
KING_VAR(int, g_cv6cec68, 6cec68)
KING_VAR(float, g_cv696d10, 696d10)
KING_VAR(float, g_cv64d680, 64d680)
KING_VAR(float, g_cv702630, 702630)
KING_VAR(float, g_cv702634, 702634)
KING_VAR(float, g_cv702638, 702638)
KING_VAR(double, g_cv64d688, 64d688)
KING_VAR(double, g_cv64b678, 64b678)

static float Cv_Bits(DWORD b) { return *(float*)&b; }

extern "C" {

// [FUN_4de990] the cab camera's frame for the game's car (6d2078 +616) into out: in the free view (6cec68) the
// look angle (696d10) about z, the tilt (car +200 scaled by its cosine, unless keep) about x, raised by the eye
// height (Car_556650), then the cab frame (+1520); else the eye raised, turned by the look direction (+1396: -2..2
// a quarter or a twelfth turn either way), pitched by the height and put in the head frame (+472), then +1520
Matrix* __cdecl Cab_EyeFrame(Matrix* out, int keep)
{
    Matrix M = g_identity;
    BYTE* g = g_cv6d2078;
    if (g && F(BYTE*, g, 616))
    {
        float h = Car_556650(F(BYTE*, g, 616), 0);
        Matrix B;
        if (g_cv6cec68)
        {
            float ang = -g_cv696d10;
            Matrix tilt = g_identity;
            if (!keep)
            {
                double z = Obj_Float200(g_cv6d2078, 0);
                double c = !(x87_fabs(ang) <= g_cv64d688) ? g_cv64b678 : x87_cos(ang);
                Matrix r;
                tilt = *Matrix_RotationX(&r, (float)(z * c));
            }
            double h2 = Car_556650(F(BYTE*, g_cv6d2078, 616), 0);
            Vec3 d;
            d.x = (float)(h2 * g_cv702630);
            d.y = (float)(h2 * g_cv702634);
            d.z = (float)(h2 * g_cv702638);
            Matrix T, RZ, A, Bm;
            Matrix* t = Matrix_Translation(&T, d);
            Matrix* a = Matrix_Multiply(&A, &tilt, Matrix_RotationZ(&RZ, ang));
            B = *Matrix_Multiply(&Bm, a, t);
        }
        else
        {
            // $L_4deae6
            float pitch = (float)(h * g_cv64d680);
            Vec3 d;
            d.x = (float)(g_cv702630 * h);
            d.y = (float)(g_cv702634 * h);
            d.z = (float)(g_cv702638 * h);
            Matrix T, R, A, X, H, C;
            M = *Matrix_Translation(&T, d);
            int look = F(int, g_cv6d2078, 1396);
            DWORD turn = 0;
            if (look == -1)
                turn = 3208357990u;
            else if (look == -2)
                turn = 3217625051u;
            else if (look == 1)
                turn = 1057360530u;
            else if (look == 2)
                turn = 1070141403u;
            if (turn)
                M = *Matrix_Multiply(&A, Matrix_RotationZ(&R, Cv_Bits(turn)), &M);
            // $L_4dec0b
            Matrix* h1 = Matrix_Multiply(&H, (const Matrix*)(g_cv6d2078 + 472), Matrix_RotationX(&X, pitch));
            B = *Matrix_Multiply(&C, h1, &M);
        }
        // $L_4dec46
        M = B;
        Matrix E;
        M = *Matrix_Multiply(&E, (const Matrix*)(g_cv6d2078 + 1520), &M);
    }
    *out = M;
    return out;
}

} // extern "C"

// === a vehicle's settings unpacked from their packed bytes (FUN_541ad0, _NetVehIn::get)

extern "C" void __cdecl Nv_Fatal(const char* text); KING_ALIAS("_Nv_Fatal", "_World_Fatal")
KING_VAR(char, g_nvBadVersion, 679d48)
KING_VAR(char, g_nvBadVersion2, 679cbc)
KING_VAR(char, g_nvBadSize, 679c68)
KING_VAR(double, g_nv64f4e0, 64f4e0)
KING_VAR(double, g_nv64f4d8, 64f4d8)
KING_VAR(double, g_nv64f4d0, 64f4d0)
KING_VAR(double, g_nv64f4c8, 64f4c8)
KING_VAR(double, g_nv64f4c0, 64f4c0)
KING_VAR(double, g_nv64eba8, 64eba8)
KING_VAR(double, g_nv64b670, 64b670)

extern "C" {

// [FUN_541ad0] settings record s (+4 / +8 the expected sizes, +16 the channel count, packed bytes from +20: the
// version words +20 / +22, flags +24, +28, +29, then per channel an id and a 10+3 bit word, 4 wheel flags, 4 heights
// below +196, 4 signed x/y pairs with their remaining z, two nibbles, the engine and gear bytes): unpacked into
// +80.. +328; a size that doesn't match the version or the record is fatal
void __fastcall NetVehIn_Get(BYTE* s)
{
    const BYTE* b = s + 20;
    int v1 = F(WORD, s, 20), v2 = F(WORD, s, 22);
    DWORD fl = F(DWORD, s, 24);
    F(DWORD, s, 80) = fl;
    F(BYTE, s, 84) = (BYTE)(((fl & 512) ? 4 : 0) + ((fl & 256) ? 2 : 0) + ((fl >> 7) & 1));
    F(float, s, 88) = (float)(int)((F(BYTE, s, 28) >> 3) << 2);
    int kind = F(BYTE, s, 28) & 7;
    if (kind <= 4)
        F(int, s, 92) = kind;
    F(BYTE, s, 96) = F(BYTE, s, 29);
    int e = 10;
    for (int c = 0; c < F(int, s, 16); c++)
    {
        F(BYTE, s, 97 + c) = b[e];
        e++;
        int w = b[e] | (b[e + 1] << 8);
        e += 2;
        F(float, s, 108 + 4 * c) = (float)((w & 1023) * g_nv64f4e0);
        F(float, s, 140 + 4 * c) = (float)(((w >> 10) & 7) * g_nv64f4d8);
    }
    for (int c = 0; c < 4; c++)
        F(BYTE, s, 190 + c) = (b[e] & (1 << c)) != 0;
    e++;
    for (int j = 0; j < 4; j++)
    {
        F(float, s, 208 + 12 * j) = (float)(F(float, s, 196) - b[e] * g_nv64f4d0);
        e++;
    }
    for (int j = 0; j < 4; j++)
    {
        float* p = (float*)(s + 248 + 12 * j);
        p[0] = (float)((signed char)b[e] * g_nv64f4c8);
        p[1] = (float)((signed char)b[e + 1] * g_nv64f4c8);
        e += 2;
        p[2] = (float)(g_nv64b670 - x87_sqrt((double)p[1] * p[1] + (double)p[0] * p[0]));
    }
    F(BYTE, s, 188) = b[e] >> 4;
    F(BYTE, s, 189) = b[e] & 15;
    e++;
    char buf[256];
    if (!(F(BYTE, s, 80) & 1))
    {
        if (e == v1 && e == F(int, s, 4))
            return;
        sprintf(buf, &g_nvBadVersion, e, v1, F(int, s, 4), F(int, s, 0), v2);
        Nv_Fatal(buf);
    }
    F(float, s, 176) = (float)(b[e] * g_nv64f4c0);
    F(float, s, 172) = (float)(int)(*(const WORD*)(b + e + 1) * 100);
    F(float, s, 180) = (float)(b[e + 3] * g_nv64f4c0);
    F(float, s, 184) = (float)(b[e + 4] * g_nv64eba8);
    F(float, s, 296) = (float)(b[e + 5] * g_nv64eba8);
    BYTE f = b[e + 6];
    F(int, s, 300) = f & 1;
    F(int, s, 304) = (f >> 1) & 1;
    F(int, s, 308) = (f >> 2) & 7;
    F(int, s, 312) = (f >> 5) & 1;
    F(int, s, 316) = f >> 6;
    F(int, s, 320) = b[e + 7] & 1;
    F(float, s, 324) = (float)(b[e + 8] * g_nv64eba8);
    F(float, s, 328) = (float)(b[e + 9] * g_nv64eba8);
    e += 10;
    if (F(int, s, 8) > v1)
    {
        sprintf(buf, &g_nvBadVersion2, e, v1, F(int, s, 8), F(int, s, 0), v2);
        Nv_Fatal(buf);
    }
    if (e <= 58 && e == F(int, s, 8))
        return;
    sprintf(buf, &g_nvBadSize, e, 58, F(int, s, 8), v1);
    Nv_Fatal(buf);
}

} // extern "C"

// === where the dashboard's text goes (FUN_53d8c0)

KING_VAR(double, g_dp64f310, 64f310)
KING_VAR(double, g_dp64d0c0, 64d0c0)

extern "C" {

// [FUN_53d8c0] dashboard s's text place for view v (-2..2) into (*x, *y): the mirror views (0) at +1376 / +1380,
// the others at +1368 / +1372, y shifted by the window height over 480 (s+36 +260)
void __fastcall Dash_TextPlace(BYTE* s, void*, int v, int* x, int* y)
{
    if (v < -2 || v > 2)
        return;
    int ox = v == 0 ? 1376 : 1368;
    int h = F(int, F(BYTE*, s, 36), 260) - 480;
    *x = F(int, s, ox) - 80;
    *y = (int)(h * g_dp64f310 + F(int, s, ox + 4) - g_dp64d0c0);
}

} // extern "C"

// [FUN_546240] records the vehicle's state block for the replay/network
// stream: a packed 29-byte record (+1204, 11, the low bytes of the 17 dwords
// at +21052, the speed from Car_555500, also kept at +10416) written as
// message 120 with the id at 67d10a
extern "C" void Vh_MsgDestroy(); KING_ALIAS("_Vh_MsgDestroy", "@MsgBuf_Destroy@8")
KING_VAR(short, vh_67d10a, 67d10a)
KING_VAR(BYTE*, vh_6ef1fc, 6ef1fc)
#pragma pack(push, 1)
struct Vh_StateRec { int a; int kind; BYTE b[17]; float speed; };
#pragma pack(pop)
extern "C" void __fastcall Vehicle_RecordState(BYTE* self, void*)
{
    Vh_StateRec rec;
    rec.kind = 11;
    rec.a = F(int, self, 1204);
    for (int i = 0; i < 17; i++)
        rec.b[i] = F(BYTE, self, 21052 + i * 4);
    float v = (float)Car_555500(self, 0);
    F(float, self, 10416) = v;
    rec.speed = v;
    BYTE msg[428];
    CallThis(FUN_602be0, msg);
    CallThis(FUN_603110, msg, (int)vh_67d10a, &rec, 29);
    CallThis(FUN_5c3630, vh_6ef1fc, msg);
    CallThis(Vh_MsgDestroy, msg);
}

// [$L_554bb0] the vehicle hit something (virtual, the impulse): reports the
// impact (|impulse| / mass +412 times the ramp at +21204) to the AI player
// bound at +20636
extern "C" double __fastcall Ramp_Value(BYTE* self, void*);
extern "C" void __cdecl Ai_HitPlayer(int player, double impact, int);
extern "C" void __fastcall Vehicle_OnHit(BYTE* self, void*, int, const float* impulse)
{
    float k = (float)(Vec3_Length((const Vec3*)impulse) / F(float, self, 412));
    if (F(int, self, 20636) != g_veh68b990)
        Ai_HitPlayer(F(int, self, 20636), Ramp_Value(self + 21204, 0) * k, 0);
}

extern "C" BYTE* __fastcall Vh_D52ac00(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac00@12", "@Dtor514790_DeletingDestructor@12")
// [$L_52ac00] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac00(BYTE* s, void* d, int f) { return Vh_D52ac00(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac10(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac10@12", "@Dtor5117e0_DeletingDestructor@12")
// [$L_52ac10] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac10(BYTE* s, void* d, int f) { return Vh_D52ac10(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac20(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac20@12", "@Dtor520560_DeletingDestructor@12")
// [$L_52ac20] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac20(BYTE* s, void* d, int f) { return Vh_D52ac20(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac30(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac30@12", "@Dtor5213f0_DeletingDestructor@12")
// [$L_52ac30] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac30(BYTE* s, void* d, int f) { return Vh_D52ac30(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac40(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac40@12", "@Dtor517a20_DeletingDestructor@12")
// [$L_52ac40] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac40(BYTE* s, void* d, int f) { return Vh_D52ac40(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac50(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac50@12", "@Dtor5244f0_DeletingDestructor@12")
// [$L_52ac50] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac50(BYTE* s, void* d, int f) { return Vh_D52ac50(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac60(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac60@12", "@Dtor51dfe0_DeletingDestructor@12")
// [$L_52ac60] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac60(BYTE* s, void* d, int f) { return Vh_D52ac60(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac70(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac70@12", "@Dtor51e710_DeletingDestructor@12")
// [$L_52ac70] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac70(BYTE* s, void* d, int f) { return Vh_D52ac70(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac80(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac80@12", "@Dtor525e00_DeletingDestructor@12")
// [$L_52ac80] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac80(BYTE* s, void* d, int f) { return Vh_D52ac80(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52ac90(BYTE*, void*, int); KING_ALIAS("@Vh_D52ac90@12", "@Dtor51ec80_DeletingDestructor@12")
// [$L_52ac90] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52ac90(BYTE* s, void* d, int f) { return Vh_D52ac90(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52aca0(BYTE*, void*, int); KING_ALIAS("@Vh_D52aca0@12", "@Dtor526b80_DeletingDestructor@12")
// [$L_52aca0] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52aca0(BYTE* s, void* d, int f) { return Vh_D52aca0(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52acb0(BYTE*, void*, int); KING_ALIAS("@Vh_D52acb0@12", "@Dtor50ff70_DeletingDestructor@12")
// [$L_52acb0] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52acb0(BYTE* s, void* d, int f) { return Vh_D52acb0(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52acc0(BYTE*, void*, int); KING_ALIAS("@Vh_D52acc0@12", "@Dtor527d00_DeletingDestructor@12")
// [$L_52acc0] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52acc0(BYTE* s, void* d, int f) { return Vh_D52acc0(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52acd0(BYTE*, void*, int); KING_ALIAS("@Vh_D52acd0@12", "@Dtor51f340_DeletingDestructor@12")
// [$L_52acd0] the deleting destructor reached through the base 64 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52acd0(BYTE* s, void* d, int f) { return Vh_D52acd0(s - 64, d, f); }
extern "C" BYTE* __fastcall Vh_D52f4c0(BYTE*, void*, int); KING_ALIAS("@Vh_D52f4c0@12", "@Dtor52b6d0_DeletingDestructor@12")
// [$L_52f4c0] the deleting destructor reached through the base 376 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f4c0(BYTE* s, void* d, int f) { return Vh_D52f4c0(s - 376, d, f); }
extern "C" BYTE* __fastcall Vh_D52f4d0(BYTE*, void*, int); KING_ALIAS("@Vh_D52f4d0@12", "@Dtor52da10_DeletingDestructor@12")
// [$L_52f4d0] the deleting destructor reached through the base 256 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f4d0(BYTE* s, void* d, int f) { return Vh_D52f4d0(s - 256, d, f); }
extern "C" BYTE* __fastcall Vh_D52f4e0(BYTE*, void*, int); KING_ALIAS("@Vh_D52f4e0@12", "@Dtor52da10_DeletingDestructor@12")
// [$L_52f4e0] the deleting destructor reached through the base 376 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f4e0(BYTE* s, void* d, int f) { return Vh_D52f4e0(s - 376, d, f); }
extern "C" BYTE* __fastcall Vh_D52f4f0(BYTE*, void*, int); KING_ALIAS("@Vh_D52f4f0@12", "@Dtor52d3e0_DeletingDestructor@12")
// [$L_52f4f0] the deleting destructor reached through the base 256 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f4f0(BYTE* s, void* d, int f) { return Vh_D52f4f0(s - 256, d, f); }
extern "C" BYTE* __fastcall Vh_D52f500(BYTE*, void*, int); KING_ALIAS("@Vh_D52f500@12", "@Dtor52d3e0_DeletingDestructor@12")
// [$L_52f500] the deleting destructor reached through the base 376 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f500(BYTE* s, void* d, int f) { return Vh_D52f500(s - 376, d, f); }
extern "C" BYTE* __fastcall Vh_D52f510(BYTE*, void*, int); KING_ALIAS("@Vh_D52f510@12", "@Dtor52e2e0_DeletingDestructor@12")
// [$L_52f510] the deleting destructor reached through the base 256 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f510(BYTE* s, void* d, int f) { return Vh_D52f510(s - 256, d, f); }
extern "C" BYTE* __fastcall Vh_D52f520(BYTE*, void*, int); KING_ALIAS("@Vh_D52f520@12", "@Dtor52e2e0_DeletingDestructor@12")
// [$L_52f520] the deleting destructor reached through the base 376 bytes in
extern "C" BYTE* __fastcall Vh_Thunk52f520(BYTE* s, void* d, int f) { return Vh_D52f520(s - 376, d, f); }

// [$L_52ace0] static initializer: clears the 35 pair lists (44 bytes each) at 6ced98
extern "C" void Vh_Pairs5Clear(); KING_ALIAS("_Vh_Pairs5Clear", "@Pairs5_Clear@8")
KING_VAR(BYTE, vh_6ced98, 6ced98)
extern "C" void __cdecl Vh_Init52ace0()
{
    for (int i = 0; i < 35; i++)
        CallThis(Vh_Pairs5Clear, &vh_6ced98 + i * 44);
}

// [$L_52b590] [$L_52e920] message handlers of two vehicle parts: the scene
// object's handling first; then a message must be for this object (+1188),
// type 13 with subtype 1 selects a part (+9528), type 10 goes to 52e8c0
extern "C" void Vh_SceneOnMessage(); KING_ALIAS("_Vh_SceneOnMessage", "@SceneObj_OnMessage@12")
extern "C" void Vh_CarPartSelect(); KING_ALIAS("_Vh_CarPartSelect", "@CarPart_Select@12")
extern "C" void Vh_Vehicles52e8c0(); KING_ALIAS("_Vh_Vehicles52e8c0", "@Vehicles_52e8c0@12")
KING_VAR(AsmLabel, vh_s6784a0, 6784a0)
KING_VAR(AsmLabel, vh_s678550, 678550)
extern "C" void __cdecl World_Fatal(const char* msg);
extern "C" int __fastcall Vh_OnMessage52b590(BYTE* self, void*, const int* m)
{
    if (CallThis<int>(Vh_SceneOnMessage, self, m))
        return 1;
    if (m[0] != F(int, self, 1188))
        World_Fatal(vh_s6784a0);
    if (m[1] != 13)
        return 0;
    if (m[2] == 1)
        CallThis(Vh_CarPartSelect, self + 9528, m[3]);
    return 1;
}
extern "C" int __fastcall Vh_OnMessage52e920(BYTE* self, void*, const int* m)
{
    if (CallThis<int>(Vh_SceneOnMessage, self, m))
        return 1;
    if (m[0] != F(int, self, 1188))
        World_Fatal(vh_s678550);
    if (m[1] != 10)
        return 0;
    CallThis(Vh_Vehicles52e8c0, self, m + 2);
    return 1;
}

// [$L_52f530] static initializer: seeds the subtractive random generator
// (Knuth's ran3: 55 values at 6d1860, indices 0 and 31)
KING_VAR(DWORD, vh_6d1860, 6d1860)
KING_VAR(int, vh_6d193c, 6d193c)
KING_VAR(int, vh_6d1940, 6d1940)
extern "C" void __cdecl Vh_Init52f530()
{
    DWORD* ma = &vh_6d1860;
    for (int i = 0; i < 55; i++)
        ma[i] = 0;
    DWORD mj = 161803398, mk = 1;
    ma[54] = mj;
    for (unsigned s = 0; s < 1134; s += 21) {
        unsigned i = s % 55;
        mj -= mk;
        ma[i] = mk;
        mk = mj;
        mj = ma[i];
    }
    for (int k = 0; k < 4; k++)
        for (unsigned i = 0; i < 55; i++)
            ma[i] -= ma[(i + 31) % 55];
    vh_6d193c = 0;
    vh_6d1940 = 31;
}

// [$L_52f680] static initializer: constructs the object at 6d1948
extern "C" void Vh_Scy52fce0Construct(); KING_ALIAS("_Vh_Scy52fce0Construct", "@Scy52fce0_Construct@8")
KING_VAR(BYTE, vh_6d1948, 6d1948)
extern "C" void __cdecl Vh_Init52f680() { CallThis(Vh_Scy52fce0Construct, &vh_6d1948); }

// [$L_55fa70] the trailer's deleting destructor reached through the base 16 bytes in
extern "C" BYTE* __fastcall Vh_Thunk55fa70(BYTE* s, void* d, int f) { return Dtor55ebd0_DeletingDestructor(s - 16, d, f); }

// [FUN_5586f0] constructs a trailer: the vehicle (w, c, p, a), no tractor
// (+21880), cleared hitch state (+21896..+21952), its vtables, +20936 = 1
KING_VAR(AsmLabel, vh_vtbl64f96c, 64f96c)
KING_VAR(AsmLabel, vh_vtbl64f880, 64f880)
KING_VAR(AsmLabel, vh_vtbl64f834, 64f834)
KING_VAR(AsmLabel, vh_vtbl64f830, 64f830)
extern "C" BYTE* __fastcall Trailer_Construct(BYTE* self, void*, int w, BYTE* c, int p, int a)
{
    Vehicle_Construct(self, 0, w, c, p, a);
    for (int off = 21896; off <= 21952; off += 4)
        F(int, self, off) = 0;
    F(int, self, 21880) = 0;
    F(void*, self, 0) = vh_vtbl64f96c;
    F(void*, self, 16) = vh_vtbl64f880;
    F(void*, self, 272) = vh_vtbl64f834;
    F(void*, self, 392) = vh_vtbl64f830;
    F(int, self, 20936) = 1;
    return self;
}

// [$L_55ef90] the trailer hit something (virtual, the impulse): reports the
// impact (|impulse| / mass +412) to the AI player of its tractor
extern "C" void __fastcall Trailer_OnHit(BYTE* self, void*, int, const float* impulse)
{
    BYTE* tractor = F(BYTE*, self, 21880);
    if (!tractor || F(int, tractor, 20636) == g_veh68b990)
        return;
    double k = Vec3_Length((const Vec3*)impulse) / F(float, self, 412);
    Ai_HitPlayer(F(int, F(BYTE*, self, 21880), 20636), k, 1);
}

extern "C" BYTE* __fastcall Vh_D55fa80(BYTE*, void*, int); KING_ALIAS("@Vh_D55fa80@12", "@Dtor55ebd0_DeletingDestructor@12")
// [$L_55fa80] the trailer's deleting destructor reached through the base 272 bytes in
extern "C" BYTE* __fastcall Vh_Thunk55fa80(BYTE* s, void* d, int f) { return Vh_D55fa80(s - 272, d, f); }
extern "C" BYTE* __fastcall Vh_D55fa90(BYTE*, void*, int); KING_ALIAS("@Vh_D55fa90@12", "@Dtor55ebd0_DeletingDestructor@12")
// [$L_55fa90] the trailer's deleting destructor reached through the base 392 bytes in
extern "C" BYTE* __fastcall Vh_Thunk55fa90(BYTE* s, void* d, int f) { return Vh_D55fa90(s - 392, d, f); }
extern "C" BYTE* __fastcall Vh_D55faa0(BYTE*, void*, int); KING_ALIAS("@Vh_D55faa0@12", "@Dtor5587b0_DeletingDestructor@12")
// [$L_55faa0] the trailer's deleting destructor reached through the base 16 bytes in
extern "C" BYTE* __fastcall Vh_Thunk55faa0(BYTE* s, void* d, int f) { return Vh_D55faa0(s - 16, d, f); }
extern "C" BYTE* __fastcall Vh_D55fab0(BYTE*, void*, int); KING_ALIAS("@Vh_D55fab0@12", "@Dtor5587b0_DeletingDestructor@12")
// [$L_55fab0] the trailer's deleting destructor reached through the base 272 bytes in
extern "C" BYTE* __fastcall Vh_Thunk55fab0(BYTE* s, void* d, int f) { return Vh_D55fab0(s - 272, d, f); }
extern "C" BYTE* __fastcall Vh_D55fac0(BYTE*, void*, int); KING_ALIAS("@Vh_D55fac0@12", "@Dtor5587b0_DeletingDestructor@12")
// [$L_55fac0] the trailer's deleting destructor reached through the base 392 bytes in
extern "C" BYTE* __fastcall Vh_Thunk55fac0(BYTE* s, void* d, int f) { return Vh_D55fac0(s - 392, d, f); }

// [$L_55fad0] static initializer: the default names of the vehicle and
// parts tables in _BSS (numbered with the formats in _DATA), then the fixed
// names copied over them (offsets into _BSS and _DATA)
static const DWORD cv_names1[][2] = {    // _BSS offset, _DATA offset
    {413792, 61852},
    {413832, 61836},
    {413872, 61820},
    {413912, 61804},
    {414072, 61788},
    {414112, 61772},
    {414232, 61756},
    {414272, 61740},
    {414392, 61724},
    {414432, 61708},
    {414472, 61692},
    {414512, 61676},
    {414552, 61660},
    {414592, 61644},
    {414592, 61628},
    {414592, 61612},
    {414672, 61596},
    {414712, 61580},
    {414752, 61564},
    {414752, 61548},
    {414832, 61532},
    {414872, 61516},
    {414912, 61500},
    {414952, 61484},
    {414992, 61468},
    {415032, 61452},
    {415072, 61436},
    {415112, 61420},
    {415152, 61404},
    {415192, 61388},
    {415232, 61372},
    {415272, 61356},
    {415312, 61340},
    {415352, 61324},
    {415392, 61308},
    {415432, 61292},
    {415472, 61276},
    {415512, 61260},
    {415552, 61244},
    {415592, 61228},
    {415632, 61212}
};
static const DWORD cv_names5[][2] = {    // _BSS offset, _DATA offset
    {417392, 61180},
    {417432, 61164},
    {417472, 61148},
    {417592, 61132},
    {417632, 61116},
    {417672, 61100},
    {417712, 61084},
    {417752, 61068},
    {417792, 61052},
    {417832, 61036},
    {417872, 61020},
    {417912, 61004},
    {417952, 60988},
    {417992, 60972},
    {418032, 60956},
    {418072, 60940},
    {418112, 60924},
    {418152, 60908},
    {418192, 60892},
    {418232, 60876},
    {418272, 60860},
    {418312, 60844},
    {418352, 60828},
    {418392, 60812},
    {418432, 60796},
    {418472, 60780},
    {418512, 60764},
    {418552, 60748},
    {418592, 60732},
    {418632, 60716},
    {418672, 60700},
    {418712, 60684},
    {418752, 60668},
    {418792, 60652},
    {418832, 60636},
    {418872, 60620},
    {418912, 60604},
    {418952, 60588},
    {418992, 60572},
    {419032, 60556},
    {419072, 60540},
    {419112, 60524},
    {419152, 60508},
    {419512, 60492},
    {419552, 60476},
    {423992, 60464},
    {424032, 60452},
    {424072, 60440}
};
static const DWORD cv_names11[][2] = {    // _BSS offset, _DATA offset
    {412340, 60196},
    {412370, 60168},
    {412400, 60140},
    {412430, 60112},
    {412460, 60084},
    {412490, 60056},
    {412520, 60028},
    {412550, 60000},
    {412580, 59972},
    {412610, 59944},
    {412640, 59916},
    {412670, 59888},
    {412700, 59860},
    {412730, 59832},
    {412760, 59804},
    {412790, 59776},
    {412820, 59748},
    {412850, 59720},
    {412880, 59692},
    {412910, 59664},
    {412940, 59636},
    {412970, 59608},
    {413000, 59580},
    {413030, 59552},
    {413060, 59524},
    {413090, 59496},
    {413120, 59468},
    {413150, 59440},
    {413180, 59412},
    {413210, 59384},
    {413240, 59356},
    {413270, 59328},
    {413300, 59300},
    {413330, 59272},
    {413360, 59244},
    {413390, 59216},
    {413420, 59188},
    {413450, 59160},
    {413480, 59132},
    {413510, 59104},
    {413540, 59076},
    {413570, 59048},
    {413600, 59020},
    {413630, 58992},
    {413660, 58964}
};
static void Cv_CopyNames(const DWORD (*t)[2], int n)
{
    for (int i = 0; i < n; i++)
        strcpy((char*)KingBss + t[i][0], (const char*)KingData + t[i][1]);
}
#define CV_B(o) ((char*)KingBss + (o))
#define CV_D(o) ((const char*)KingData + (o))
extern "C" void __cdecl Cv_InitNames()
{
    int i;
    char* p;
    for (i = 0, p = CV_B(413752); p < CV_B(427032); p += 40, i++)
        sprintf(p, CV_D(61868), i);
    Cv_CopyNames(cv_names1, 41);
    for (i = 48, p = CV_B(415672); p <= CV_B(416032); p += 40, i++)
        sprintf(p, CV_D(61196), i);
    for (i = 65, p = CV_B(416352); p <= CV_B(417352); p += 40, i++)
        sprintf(p, CV_D(61196), i);
    Cv_CopyNames(cv_names5, 48);
    for (i = 0, p = CV_B(424112); p < CV_B(425392); p += 40, i++)
        sprintf(p, CV_D(60408), i + 1, i);
    for (i = 0, p = CV_B(425392); p < CV_B(425552); p += 40, i++)
        sprintf(p, CV_D(60372), i + 1, i);
    for (i = 0, p = CV_B(425792); p < CV_B(427072); p += 160, i++) {
        sprintf(p - 40, CV_D(60340), i + 1, i);
        sprintf(p, CV_D(60308), i + 1, i);
        sprintf(p + 40, CV_D(60276), i + 1, i);
        sprintf(p + 80, CV_D(60244), i + 1, i);
    }
    for (i = 0, p = CV_B(412340); p < CV_B(413750); p += 30, i++)
        sprintf(p, CV_D(60224), i);
    Cv_CopyNames(cv_names11, 45);
}
#undef CV_B
#undef CV_D
