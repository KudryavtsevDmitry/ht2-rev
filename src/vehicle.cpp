// vehicle.cpp - small functions of the unit at 0x52fa20 of king.masm (the
// code before copter.cpp): the control bindings, sounds at the volume of the
// options, the list of vehicles and the classes of vehicle types, getters,
// setters and saved states of the vehicle, a flashing light, and some
// constructors.
//
// Building with  nmake ASM_VEHICLE=1  makes king.masm assemble the original
// procedures again.

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
ASM_PROC(FUN_542450)
ASM_PROC(FUN_543c00)
ASM_PROC(FUN_5e1a10)    // set a switch
extern "C" float __fastcall Ramp_Position(BYTE* self, void*);   // carphys.cpp
ASM_PROC(FUN_5014b0)
ASM_PROC(FUN_501550)
ASM_PROC(FUN_556110)
ASM_PROC(FUN_5561b0)
ASM_PROC(FUN_5f19b0)    // named object: constructor
ASM_PROC(FUN_5ef6d0)
ASM_PROC(FUN_4ba130)
ASM_PROC(FUN_4ba210)

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
    CallThis(FUN_542450, self, 8);
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
        CallThis(FUN_543c00, self, cmd[1]);
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
    return CallC<int>(FUN_4ba130, a, b, 1);
}

extern "C" int __cdecl Veh_54efb0(int a, int b)
{
    return CallC<int>(FUN_4ba210, a, b, 1);
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
