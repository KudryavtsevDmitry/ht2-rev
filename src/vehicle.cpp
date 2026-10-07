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
ASM_PROC(FUN_5d33d0)
extern "C" void __cdecl Switch_SetChecked(BYTE* obj, int v);     // world.cpp
ASM_PROC(FUN_545cc0)
extern "C" BYTE* __fastcall ListItem592bf0_Construct(BYTE* self, void*, BYTE** list);   // movingb.cpp
extern "C" void __fastcall Mass_Shares(float* self, void*, int wheels);
extern "C" void __fastcall Car_SetView(BYTE* self, void*, int n);
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
extern "C" Vec3* __fastcall Vec3_DivAssign(Vec3* self, void*, Vec3* ret, float s);
ASM_PROC(FUN_5e1a10)    // set a switch
extern "C" float __fastcall Ramp_Position(BYTE* self, void*);   // carphys.cpp
ASM_PROC(FUN_5014b0)
ASM_PROC(FUN_501550)
ASM_PROC(FUN_556110)
ASM_PROC(FUN_5561b0)
ASM_PROC(FUN_5f19b0)    // named object: constructor
ASM_PROC(FUN_5ef6d0)
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
KING_ALIAS("_Road_RoomInfo", "FUN_438d70")

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

ASM_PROC(FUN_5ef2b0)    // named object: find (parent, name)
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
ASM_PROC(FUN_4e2821)
ASM_PROC(FUN_4fc930)
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
