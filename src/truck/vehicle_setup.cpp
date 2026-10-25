// vehicle_setup.cpp - car2e.cpp of the game, rewritten from king.masm: small methods
// of the vehicle: drive wheel speeds, the forwarding to the gearbox functions
// of caran.cpp, and the 27 adjustable settings of a vehicle.
//
// Building with  nmake ASM_VEHICLE_SETUP=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS     // strcpy as in the game
#include <string.h>
#include "car.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void FUN_506750(); KING_ALIAS("_FUN_506750", "@Car_AxleFlat@12")    // vehicle: part by index

ASM_VAR(int, g_697678, $L_697678)
ASM_VAR(int, g_69767c, $L_69767c)
ASM_VAR(int, g_697680, $L_697680)
ASM_VAR(int, g_697684, $L_697684)
ASM_VAR(int, g_carTypeCount, $L_6ce32c)
typedef char CarTypeNames[1];
ASM_VAR(CarTypeNames, g_carTypeNames, $L_69809c)

struct Car {
    // wheel i drives: dword +7092 + 4i set and word +6564 + 2i zero
    bool Driving(int i) { return F(int, this, 7092 + i * 4) != 0 && F(short, this, 6564 + i * 2) == 0; }

    float WheelSpeed(int i);
    float TopSpeed(int gear);
    float BestGear();
    double DriveForce();
    int SetSettings(const float* v);
    int GetSettings(int* out);
    void SetSetting(int i, float v);
    void SetSettingRaw(int i, int x);
    void StepSetting(int i);
    int FindParts();
};

// the speed of a wheel from the drive wheels' mean radius, at least 5% of
// the speed at +7304; 0 for wheel 1
float Car::WheelSpeed(int i)
{
    if (i == 1)
        return 0;
    float rate = F(float, this, 7236 + i * 4);
    int n = F(int, this, 3540);
    double sum = 0.0f;
    int count = 0;
    for (int k = 0; k < n; k++)
        if (Driving(k)) {
            sum = sum + F(float, this, 7024);
            count++;
        }
    if (count == 0)
        return 0;
    float v = (float)(F(float, this, 3528) / (sum / count) * rate * 0.15915494309188485);
    double min = F(float, this, 7304) * 0.05;
    if (!(v >= min)) {
        float m = (float)min;
        return m;
    }
    return v;
}

// +2252 the vehicle type, +9792 the load, +7196 the gear
float Car::TopSpeed(int gear)
{
    return Car_TopSpeedInGear(F(int, this, 2252), gear, F(float, this, 9792), (float*)&gear);
}

float Car::BestGear()
{
    int gear;
    float rpm;
    return Car_BestGear(F(int, this, 2252), F(float, this, 9792), &gear, &rpm);
}

double Car::DriveForce()
{
    double sum = 0.0f;
    int n = F(int, this, 3540);
    for (int k = 0; k < n; k++)
        if (Driving(k))
            sum = sum + (double)F(float, this, 3360 + k * 4) * F(float, this, 9840 + k * 4);
    return sum;
}

// setting i: value +15520 + 20i, maximum +15524, minimum +15528, fraction +15532
#define SETTING(i, off) F(int, this, 15520 + (i) * 20 + (off))
#define SETTINGF(i) F(float, this, 15532 + (i) * 20)

int Car::SetSettings(const float* v)
{
    for (int i = 0; i < 27; i++)
        SetSetting(i, v[i]);
    return 27;
}

int Car::GetSettings(int* out)
{
    for (int i = 0; i < 27; i++)
        out[i] = SETTING(i, 12);
    return 27;
}

void Car::SetSetting(int i, float v)
{
    if (F(short, this, 10058) == 0 || i < 0 || i >= 27)
        return;
    if (!(v >= 0.0f) || v > 1.0f)
        return;
    SETTING(i, 0) = (int)(SETTING(i, 4) * (double)v);
    SETTINGF(i) = v;
}

void Car::SetSettingRaw(int i, int x)
{
    if (F(short, this, 10058) == 0 || i < 0 || i >= 27)
        return;
    if (SETTING(i, 4) < x || SETTING(i, 8) > x)
        return;
    SETTING(i, 0) = x;
    SETTINGF(i) = (float)((double)x / SETTING(i, 4));
}

// one step toward the maximum, or toward the minimum from a negative value
void Car::StepSetting(int i)
{
    if (F(short, this, 10058) == 0 || i < 0 || i >= 27)
        return;
    int v = SETTING(i, 0);
    if (v >= 0) {
        v++;
        if (v > SETTING(i, 4))
            v = SETTING(i, 4);
        SETTING(i, 0) = v;
        SETTINGF(i) = (float)((double)v / SETTING(i, 4));
        return;
    }
    v--;
    if (v < SETTING(i, 8))
        v = SETTING(i, 8);
    SETTING(i, 0) = v;
    int min = SETTING(i, 8);
    if (min == 0) {
        F(int, this, 15532 + i * 20) = 0;
        return;
    }
    SETTINGF(i) = (float)((double)v / min);
}

int Car::FindParts()
{
    for (int i = 0; i < 4; i++)
        F(void*, this, 2236 + i * 4) = CallThis<void*>(FUN_506750, this, i);
    if (F(void*, this, 2236) && F(void*, this, 2240) && F(void*, this, 2244) && F(void*, this, 2248)) {
        F(int, this, 2232) = 1;
        return 1;
    }
    F(int, this, 2232) = 0;
    return 0;
}

extern "C" int __cdecl Car_ClearSettings(int* out)
{
    memset(out, 0, 27 * 4);
    return 27;
}

extern "C" void __cdecl Car_InitDefaults()
{
    g_697678 = 1;
    g_69767c = 12;
    g_697680 = 0;
    g_697684 = 0;
}

// copies the names of the vehicle types; the copies are not used
extern "C" void __cdecl Car_CopyTypeNames()
{
    char a[32], b[32];
    const char* names = g_carTypeNames + 112;
    for (int n = g_carTypeCount; n > 0; n--, names += 2416) {
        strcpy(a, names + 30);
        strcpy(b, names);
    }
}

extern "C" {
float __fastcall Car_WheelSpeed(Car* s, void*, int i) { return s->WheelSpeed(i); }                  // [FUN_4ff150]
double __fastcall Car_WheelForceIn(Car* s, void*, float torque, int gear) { return Car_WheelForce(F(int, s, 2252), torque, gear); } // [FUN_4ff570]
double __fastcall Car_WheelForceNow2(Car* s, void*, float torque, int) { return Car_WheelForceIn(s, 0, torque, F(int, s, 7196)); } // [FUN_4ff590]
double __fastcall Car_WheelForceNowB(Car* s, void*, float torque) { return Car_WheelForceNow2(s, 0, torque, F(int, s, 7196)); }   // [FUN_4ff530]
double __fastcall Car_WheelForceNow(Car* s, void*, float torque) { return Car_WheelForceIn(s, 0, torque, F(int, s, 7196)); }      // [FUN_4ff550]
float __fastcall Car_TopSpeed(Car* s, void*, int gear) { return s->TopSpeed(gear); }                 // [FUN_4ff5b0]
float __fastcall Car_BestGearNow(Car* s, void*) { return s->BestGear(); }                            // [FUN_4ff5e0]
double __fastcall Car_DriveForce(Car* s, void*) { return s->DriveForce(); }                          // [FUN_4ff610]
void __fastcall Car_SetAuto(Car* s, void*, int a) { F(int, s, 7188) = a == 1; }                      // [FUN_4ff770]
int __fastcall Car_SetSettings(Car* s, void*, const float* v) { return s->SetSettings(v); }          // [FUN_501450]
int __fastcall Car_GetSettings(Car* s, void*, int* out) { return s->GetSettings(out); }              // [FUN_501480]
void __fastcall Car_SetSetting(Car* s, void*, int i, float v) { s->SetSetting(i, v); }               // [FUN_5017a0]
void __fastcall Car_SetSettingRaw(Car* s, void*, int i, int x) { s->SetSettingRaw(i, x); }           // [FUN_501810]
void __fastcall Car_StepSetting(Car* s, void*, int i) { s->StepSetting(i); }                         // [FUN_501870]
int __fastcall Car_FindParts(Car* s, void*) { return s->FindParts(); }                               // [FUN_506a60]
}

extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
typedef DWORD Dwords12[12];
ASM_VAR(Dwords12, g_rayDefaults, $L_7025f0)

extern "C" {

// [FUN_5039a0] casts a ray from the vehicle's point at +2292 to p through the
// scene object at +6672; *hit gets the object hit, p the hit point
int __fastcall Vehicle_RayTest(BYTE* self, void*, DWORD* p, BYTE** hit)
{
    BYTE q[208];
    CallThis(FUN_5ef5c0, q);
    memcpy(q + 4, g_rayDefaults, 48);
    DWORD* from = (DWORD*)(self + 2292);
    F(DWORD, q, 76) = from[0];
    F(DWORD, q, 80) = from[1];
    BYTE* scene = F(BYTE*, self, 6672);
    F(DWORD, q, 84) = from[2];
    F(DWORD, q, 88) = p[0];
    F(DWORD, q, 92) = p[1];
    F(DWORD, q, 96) = p[2];
    F(int, q, 116) = 0;
    F(int, q, 0) = 0;
    if (!scene) {
        *hit = 0;
        return 1;
    }
    unsigned r = CallVirt<unsigned>(scene, 52, 0, q);
    BYTE* other = F(BYTE*, q, 104);
    unsigned two = r & 2;
    if (two && !other) {
        F(int, self, 15444) = 1;
        other = scene;
    }
    if ((r & 1) == 1) {
        *hit = two == 2 ? other : scene;
        p[0] = F(DWORD, q, 52);
        p[1] = F(DWORD, q, 56);
        p[2] = F(DWORD, q, 60);
        return 0;
    }
    *hit = two == 2 ? other : scene;
    return 1;
}

}

extern "C" double __fastcall Ramp_Value(BYTE* self, void*);       // vehicle_physics.cpp
extern "C" double __cdecl Car_EngineBrake(float rpm, float k, float c, float d);   // drivetrain.cpp

// [FUN_4ff3e0] the engine brake torque at a speed: none in neutral, less in higher gears
extern "C" double __fastcall Car_EngineBrakeTorque(BYTE* car, void*, float speed)
{
    if (F(int, car, 7208) == 1)
        return 0.0f;
    double r = Car_EngineBrake(speed, F(float, car, 7308), (float)Ramp_Value(car + 16372, 0), F(float, car, 10716));
    int gear = F(int, car, 7208);
    if (gear > 1)
        r *= 1.0f - (double)((gear - 2) / 1000);
    return r;
}

// [FUN_500ef0] advances wheel i's three ramps by dt and sets their integer outputs
extern "C" void __cdecl Wheel_AdvanceRamps(BYTE* base, int i, float dt)
{
    BYTE* w = base + i * 20;
    double v = (double)dt * F(float, w, 76) + F(float, w, 72);
    F(float, w, 72) = (float)v;
    if (v > 1.0f)
        F(float, w, 72) = 1.0f;
    F(int, w, 60) = (int)((double)F(int, w, 64) * F(float, w, 72));
    double sign = F(int, w, 220) ? (double)(F(int, w, 220) > 0 ? 1 : -1) : -1.0f;
    v = sign * F(float, w, 236) * dt + F(float, w, 232);
    F(float, w, 232) = (float)v;
    if (v > 1.0f)
        F(float, w, 232) = 1.0f;
    else if (!(v >= -1.0f))
        F(float, w, 232) = -1.0f;
    F(int, w, 220) = (int)((double)F(int, w, 224) * F(float, w, 232));
    v = (double)dt * F(float, w, 396) + F(float, w, 392);
    F(float, w, 392) = (float)v;
    if (v > 1.0f)
        F(float, w, 392) = 1.0f;
    F(int, w, 380) = (int)((double)F(int, w, 384) * F(float, w, 392));
}

// [FUN_501550] the smallest wear factor of the wheels and the two axles
extern "C" double __fastcall Car_MinWearFactor(BYTE* car, void*)
{
    float m = 1.0f;
    int n = F(int, car, 3540);
    for (int k = 0; k < n; k++) {
        double r;
        int a = F(int, car, 15580 + k * 20), b = F(int, car, 15584 + k * 20);
        if (F(int, car, 7124 + k * 4) == 0 && n >= 4)
            r = 1.0 - (double)a * (n == 4 ? 0.6 : 0.3) / b;
        else
            r = 1.0 - (double)a * 0.9 / b;
        if (m > r)
            m = (float)r;
    }
    double r = 1.0 - (double)F(int, car, 15540) * 0.3 / F(int, car, 15544);
    if (m > r)
        m = (float)r;
    r = 1.0 - (double)F(int, car, 15520) * 0.7 / F(int, car, 15524);
    if (m > r)
        return r;
    return m;
}

ASM_PROC(FUN_4eacce)    // filter: feed (value, name)
extern "C" void FUN_4eaf98(); KING_ALIAS("_FUN_4eaf98", "@Filter_Value@4")    // filter: output
ASM_VAR(char, g_strFilterEngineTach, $L_674850)

// [FUN_4ff210] the tachometer target from the driven wheels' slip, filtered into +7300
extern "C" void __fastcall Car_UpdateTach(BYTE* s, void*, const float* rpm)
{
    double acc = 0.0f;
    int slipping = 0, gripping = 0, driven = 0;
    int n = F(int, s, 3540);
    for (int k = 0; k < n; k++)
        if (F(int, s, 6612 + 480 + k * 4)) {
            driven++;
            if (F(WORD, s, 6564 + k * 2))
                slipping++;
            if (F(int, s, 6612 + k * 4)) {
                acc += F(float, s, 6612 + 3228 + k * 4);
                gripping++;
            }
        }
    float out = *rpm;
    F(float, s, 7352) = 1.0f;
    if (F(int, s, 7196) == 1 || slipping == driven)
        out = (float)((double)F(float, s, 10712) * F(float, s, 7304) * 3.0f);
    else if (gripping == driven && gripping != 0) {
        double a = acc / gripping;
        if (a > 1.0f) a = 1.0f;
        else if (!(a >= 0.0f)) a = 0.0f;
        out = (float)(((1.0f - a) + a) * 3.0f * F(float, s, 10712) * F(float, s, 7304));
    } else if (!(*rpm >= 0.0f)) {
        double t = (double)F(float, s, 10712) * F(float, s, 7304);
        F(float, s, 7352) = 0.3f;
        out = (float)(t * 3.0f);
    } else if ((double)F(float, s, 7304) * 0.5 > *rpm) {
        double r = (double)*rpm / F(float, s, 7304);
        r = r + r;
        double q = 1.0f - r;
        double w = (double)F(float, s, 10712) * F(float, s, 7304) * q;
        out = (float)((w + w) + r * *rpm);
        F(float, s, 7352) = (float)(q * 0.3 + r);
    }
    double lim = (double)F(float, s, 7304) * 0.05;
    if (!(out >= lim))
        out = (float)lim;
    CallThis(FUN_4eacce, s + 6992, out, &g_strFilterEngineTach);
    F(float, s, 7300) = (float)CallThis<double>(FUN_4eaf98, s + 6992);
}

// [FUN_501e30] moves wheel i's level (+15532) towards its notch by *amount at rate; 1 when the notch was reached
extern "C" int __fastcall Wheel_StepLevel(BYTE* s, void*, int i, float* amount, float rate)
{
    if (!(*amount < 0 || *amount > 0))
        return 0;
    BYTE* w = s + i * 20;
    if (!(rate < 0 || rate > 0)) {
        F(float, w, 15532) = 0;
        F(int, w, 15520) = 0;
        return 1;
    }
    if (!(F(float, w, 15532) >= 0.0f))
        return 0;
    int cnt = F(int, w, 15520);
    int max = F(int, w, 15524);
    if (cnt == max)
        cnt--;
    double x = F(float, w, 15532) - (double)cnt / max;
    double y = (x < 0 ? -x : x) * rate;
    if (y > *amount) {
        double t = (double)*amount / rate;
        double v;
        if (x > 0.0f) {
            v = F(float, w, 15532) - t;
            F(float, w, 15532) = (float)v;
            if (!(v >= 0.0f))
                F(float, w, 15532) = 0;
        } else {
            v = t + F(float, w, 15532);
            F(float, w, 15532) = (float)v;
            if (v > 0.0f)
                F(float, w, 15532) = 0;
        }
        F(int, w, 15520) = (int)((double)F(int, w, 15524) * F(float, w, 15532));
        *amount = 0;
        return 0;
    }
    *amount = (float)((double)*amount - y);
    double v = F(float, w, 15532) - x;
    F(float, w, 15532) = (float)v;
    if (!(v >= 0.0f))
        F(float, w, 15532) = 0;
    F(int, w, 15520) = (int)((double)F(int, w, 15524) * F(float, w, 15532));
    return 1;
}

ASM_PROC(FUN_4e41d9)    // a random value (angle, name)
extern "C" double __cdecl Table360_At(float angle);   // world.cpp
ASM_VAR(char, g_strCheckDamage, $L_67498c)    // "_Car_V::check_damage"

// [FUN_501930] the damaged wheels' wobble (+7028), the drive shares (+7320) and the gear range of the drive mode
extern "C" void __fastcall Car_CheckDamage(BYTE* s, void*)
{
    int n = F(int, s, 3540);
    for (int k = 0; k < n; k++) {
        int a = F(int, s, 15580 + k * 20), b = F(int, s, 15584 + k * 20);
        float A = F(float, s, 7520 + k * 4), B = F(float, s, 7552 + k * 4);
        if (a == b)
            F(DWORD, s, 7028 + k * 4) = F(DWORD, s, 7552 + k * 4);
        else if (a == 2 || a == 1) {
            float t = (float)(F(float, s, 3740 + k * 4) + 1.570796326795);
            float u = (float)(CallC<double>(FUN_4e41d9, t, &g_strCheckDamage) + 3.14159265359);
            double c = Table360_At(u);
            if (a == 2)
                F(float, s, 7028 + k * 4) = (float)((c * ((double)A - B) + ((double)A + B)) * 0.5);
            else
                F(float, s, 7028 + k * 4) = (float)((c * ((double)A - B) + B) * 0.25 + (double)A * 0.75);
        } else
            F(DWORD, s, 7028 + k * 4) = F(DWORD, s, 7520 + k * 4);
        n = F(int, s, 3540);
    }
    n = F(int, s, 3540);
    for (int k = 0; k < n; k++) {
        double r = Ramp_Value(s + 16216, 0) * F(float, s, 7812 + k * 4);
        if (F(int, s, 15900 + k * 20))
            r *= 0.2;
        F(float, s, 7320 + k * 4) = (float)r;
        n = F(int, s, 3540);
    }
    int m = F(int, s, 15540);
    if (m == 2) {
        int g = F(int, s, 7684) - 3;
        F(int, s, 7192) = g < 1 ? 1 : g;
        F(float, s, 7216) = (float)((double)F(float, s, 7708) * 5.0f);
        F(float, s, 7220) = (float)((double)F(float, s, 7712) * 5.0f);
        F(float, s, 7224) = (float)((double)F(float, s, 7716) * 5.0f);
    } else if (m == 1) {
        F(int, s, 7192) = F(int, s, 7684) - 1;
        F(float, s, 7216) = (float)((double)F(float, s, 7708) + F(float, s, 7708));
        F(float, s, 7220) = (float)((double)F(float, s, 7712) + F(float, s, 7712));
        F(float, s, 7224) = (float)((double)F(float, s, 7716) + F(float, s, 7716));
    } else {
        F(int, s, 7192) = F(int, s, 7684);
        F(DWORD, s, 7216) = F(DWORD, s, 7708);
        F(DWORD, s, 7220) = F(DWORD, s, 7712);
        F(DWORD, s, 7224) = F(DWORD, s, 7716);
    }
}

// ---------------------------------------------------------------------------
typedef char AsmLabel[1];
extern "C" void FUN_501710(); KING_ALIAS("_FUN_501710", "_Car_ClearSettings")
ASM_VAR(AsmLabel, s_674950, $L_674950)
extern "C" void __cdecl World_Fatal(const char* msg);

// [FUN_501730..$L_501790] clears the 27 first settings, then one zero and seven ones; returns 35
extern "C" int __cdecl Car_ResetSettings35(float* p)
{
    int i = CallC<int>(FUN_501710, p);
    p[i++] = 0;
    for (int k = 0; k < 7; k++)
        p[i++] = 1.0f;
    if (i != 35)
        World_Fatal(s_674950);
    return i;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_4eb5e0(); KING_ALIAS("_FUN_4eb5e0", "@Ramp_Value@8")
extern "C" void FUN_4eb690(); KING_ALIAS("_FUN_4eb690", "@Ramp_Position@8")
extern "C" void FUN_507380(); KING_ALIAS("_FUN_507380", "_Car_EngineTorque")
extern "C" void FUN_507440(); KING_ALIAS("_FUN_507440", "_Car_ThrottleTorque")
extern "C" void FUN_4ff3e0(); KING_ALIAS("_FUN_4ff3e0", "@Car_EngineBrakeTorque@12")
extern "C" void FUN_501480(); KING_ALIAS("_FUN_501480", "@Car_GetSettings@12")
extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")
ASM_PROC(FUN_4eacce)    // filter: feed (value, name)
extern "C" void FUN_4eaf98(); KING_ALIAS("_FUN_4eaf98", "@Filter_Value@4")    // filter: output
ASM_VAR(AsmLabel, s_674914, $L_674914)
ASM_VAR(AsmLabel, s_674870, $L_674870)

// [FUN_4ff460..$L_4ff492] the engine torque at rpm from the torque-rpm (+7304) and the two ramps
extern "C" double __fastcall Car_TorqueAt(BYTE* self, void*, float rpm)
{
    float scale = (float)CallThis<double>(FUN_4eb5e0, self + 16320);
    float maxTorque = (float)CallThis<double>(FUN_4eb5e0, self + 16060);
    return CallC<double>(FUN_507380, rpm, F(float, self, 7304), maxTorque, scale);
}

// [FUN_4ff4a0..$L_4ff4ac] the engine torque at the current rpm (+7300)
extern "C" double __fastcall Car_TorqueNow(BYTE* self, void*)
{
    return Car_TorqueAt(self, 0, F(float, self, 7300));
}

// [FUN_4ff500..$L_4ff50e] -brake / (torque - brake) at rpm
extern "C" double __fastcall Car_TorqueRatio(BYTE* self, void*, float rpm)
{
    float t = (float)Car_TorqueAt(self, 0, rpm);
    double u = CallThis<double>(FUN_4ff3e0, self, rpm);
    return -(u / ((double)t - u));
}

// [FUN_4ff4b0..$L_4ff4bf] the throttle torque of the car type between torque and brake at rpm
extern "C" double __fastcall Car_TorqueThrottle(BYTE* self, void*, float rpm, float throttle)
{
    float t = (float)Car_TorqueAt(self, 0, rpm);
    float u = (float)CallThis<double>(FUN_4ff3e0, self, rpm);
    return CallC<double>(FUN_507440, F(int, self, 2252), t, u, throttle);
}

// [FUN_5014b0..$L_501543] the settings, then +3808 and the 7 ramp positions; returns 35
extern "C" int __fastcall Car_ReadSettings35(BYTE* self, void*, float* p)
{
    int i = CallThis<int>(FUN_501480, self, p);
    p[i++] = (float)F(double, self, 3808);
    for (int off = 16060; off <= 16372; off += 52)
        p[i++] = (float)CallThis<double>(FUN_4eb690, self + off);
    if (i != 35)
        World_Fatal(s_674914);
    return i;
}

// [FUN_4ff790] starts the engine: state 1, start time, rpm 2*idle*ratio through the rpm filter
extern "C" void __fastcall Car_StartEngine(BYTE* self, void*)
{
    F(int, self, 7196) = 1;
    F(int, self, 7204) = 0;
    F(float, self, 7228) = (float)CallC<double>(FUN_4e27a5);
    double d = (double)F(float, self, 10712) * F(float, self, 7304);
    float v = (float)(d + d);
    F(float, self, 7300) = v;
    CallThis(FUN_4eacce, self + 6992, v, (char*)s_674870);
    F(float, self, 7300) = (float)CallThis<double>(FUN_4eaf98, self + 6992);
}


// ---------------------------------------------------------------------------
// Small setup procs: static vectors, a light array, the race start, a stream read.

extern "C" void Is_ExtractText(); KING_ALIAS("_Is_ExtractText", "??5istream@@QAEAAV0@PAD@Z")
// [FUN_507ab0] reads a word from the stream into buf: 1 unless the stream failed (state bits 2, 4)
extern "C" int __cdecl Stream_ReadWord(BYTE* is, char* buf)
{
    BYTE* r = CallThis<BYTE*>(Is_ExtractText, is, buf);
    return (F(int, r + F(int, F(BYTE*, r, 0), 4), 8) & 6) == 0;
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

ASM_VAR(void*, g_q49_697684, $L_697684)
// [$L_503e10] frees the 697684 block
extern "C" void __cdecl Block697684_Free(void)
{
    if (g_q49_697684)
        operator delete(g_q49_697684);
}

// ---------------------------------------------------------------------------
// Small methods: pool slots, list teardown, loader thread, frame shutdown, message dispatch.

extern "C" int __cdecl printf(const char* f, ...);
extern "C" int __cdecl fprintf(FILE* f, const char* t, ...);
extern "C" void __cdecl exit(int code);
ASM_VAR(void*, g_q53_6ce36c, $L_6ce36c)
ASM_VAR(AsmLabel, s_q53_674b20, $L_674b20)
ASM_VAR(AsmLabel, s_q53_66c4f4, $L_66c4f4)
// [FUN_507c80] a fatal setup error: to the 6ce36c handler when set, else printed, appended to the 674b20 log and
// the game exits
extern "C" int __cdecl Setup_Fatal(const char* msg)
{
    if (g_q53_6ce36c)
        return ((int(__cdecl*)(const char*))g_q53_6ce36c)(msg);
    printf(msg);
    FILE* f = fopen((const char*)s_q53_674b20, (const char*)s_q53_66c4f4);
    if (f) {
        fprintf(f, msg);
        fclose(f);
    }
    exit(0);
    return 0;
}

// ---------------------------------------------------------------------------
// Default tuning values.

// [$L_5021c0] fills the floats of s from +4 to +108 (all but +56) with the default values
extern "C" BYTE* __fastcall Tuning_SetDefaults(BYTE* s)
{
    static const int off[26] = {4, 20, 40, 8, 24, 28, 44, 12, 16, 32, 60, 76, 96,
                                36, 48, 52, 64, 68, 72, 80, 84, 88, 92, 100, 104, 108};
    static const DWORD val[26] = {
        1036831949, 1036831949, 1036831949, 1045220557, 1045220557, 1045220557, 1045220557, 1050253722,
        1050253722, 1050253722, 1028443341, 1028443341, 1028443341, 1053609165, 1084227584, 1084227584,
        1069547520, 1077936128, 1058642330, 1065353216, 1058642330, 1058642330, 1065353216, 1069547520,
        1084227584, 1084227584};
    for (int k = 0; k < 26; k++)
        F(DWORD, s, off[k]) = val[k];
    return s;
}

// ---------------------------------------------------------------------------
// The static vehicle records.

extern "C" void __stdcall Array_ForEach(BYTE* p, int size, int count, void (*fn)());    // ai_world.cpp
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
ASM_VAR(AsmLabel, g_q98_7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_q98_6c6b68, $L_6c6b68)
// [$L_507230] static init: the 35 876-byte records at 6c6b68 + 72 get zeroed vectors, two identity matrices
// and cleared fields
extern "C" void __cdecl VehSet_InitRecords(void)
{
    BYTE* b = (BYTE*)g_q98_6c6b68 + 176;
    for (int k = 35; k; k--, b += 876) {
        Array_ForEach(b - 104, 12, 4, FUN_4bd4d0);
        memcpy(b - 104, (BYTE*)g_q98_7025f0, 48);
        Array_ForEach(b - 56, 12, 4, FUN_4bd4d0);
        memcpy(b - 56, (BYTE*)g_q98_7025f0, 48);
        memset(b - 8, 0, 24);
        Array_ForEach(b + 28, 12, 20, FUN_4bd4d0);
        memset(b + 268, 0, 36);
        Array_ForEach(b + 312, 12, 4, FUN_4bd4d0);
        Array_ForEach(b + 360, 16, 20, FUN_4bd4d0);
    }
}

// [$L_501fb0] fills the floats of s from +4 to +332 (all but +56, +112, +168, +224, +280) with the default values
extern "C" BYTE* __fastcall Tuning501fb0_SetDefaults(BYTE* s)
{
    static const short off[78] = {
        4, 8, 12, 16, 20, 24, 28, 32, 40, 44, 48, 52, 36, 60, 64, 68, 72, 76, 80, 84, 88, 96, 100, 104, 108, 92,
        116, 120, 124, 128, 132, 136, 140, 144, 152, 156, 160, 164, 148, 172, 176, 180, 184, 188, 192, 196, 200, 208,
        212, 216, 220, 204, 228, 232, 236, 240, 244, 248, 252, 256, 264, 268, 272, 276, 260, 284, 288, 292, 316, 304,
        312, 320, 324, 300, 296, 308, 328, 332};
    static const DWORD val[78] = {
        1058642330, 1057803469, 1050253722, 1050253722, 1055286886, 1055286886, 1050253722, 1055286886, 1058642330,
        1055286886, 0, 0, 1045220557, 1056964608, 1051931443, 1050253722, 1050253722, 1053609165, 1051931443,
        1050253722, 1051931443, 1056964608, 1051931443, 0, 0, 1045220557, 1061997773, 1058642330, 1045220557,
        1050253722, 1061997773, 1055286886, 1050253722, 1055286886, 1060320051, 1060320051, 0, 0, 1045220557,
        1060320051, 1051931443, 1045220557, 1050253722, 1060320051, 1051931443, 1050253722, 1051931443, 1056964608,
        1048576000, 0, 0, 1045220557, 1058642330, 1061997773, 1053609165, 1050253722, 1056964608, 1056964608,
        1050253722, 1055286886, 1056964608, 1061997773, 0, 0, 1045220557, 1056964608, 1061997773, 1053609165,
        1045220557, 1056964608, 1056964608, 1056964608, 1061997773, 1055286886, 1053609165, 1050253722, 0, 0};
    for (int k = 0; k < 78; k++)
        F(DWORD, s, off[k]) = val[k];
    return s;
}

ASM_VAR(AsmLabel, g_q156_6976f8, $L_6976f8)
ASM_VAR(AsmLabel, g_q156_697688, $L_697688)
// [$L_5021a0] static init: the 6976f8 tuning defaults
extern "C" BYTE* __cdecl VehSet_Init6976f8(void) { return Tuning501fb0_SetDefaults((BYTE*)g_q156_6976f8); }
// [$L_502250] static init: the 697688 tuning defaults
extern "C" BYTE* __cdecl VehSet_Init697688(void) { return Tuning_SetDefaults((BYTE*)g_q156_697688); }

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q197_FUN_503dd0(); KING_ALIAS("_q197_FUN_503dd0", "_Car_InitDefaults")
extern "C" void q197_L_503e10(); KING_ALIAS("_q197_L_503e10", "_Block697684_Free")
// [$L_503e00] registers the teardown at exit
extern "C" int __cdecl Static503dc0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_503e10); }
// [$L_503dc0] the static initializer: FUN_503dd0, then the teardown at exit
extern "C" int __cdecl Static503dc0_Init(void)
{
    CallC(q197_FUN_503dd0);
    return Static503dc0_AtExit();
}

// === the axle line of the touching wheels (FUN_503ac0)

ASM_VAR(Vec3, g_vs702650, $L_702650)
extern "C" Vec3* __fastcall Vec3_DivAssign(Vec3* self, void*, Vec3* ret, float s);
extern "C" Vec3* __cdecl Line_Project(Vec3* out, const Vec3* p2, const Vec3* n, const Vec3* p4, const Vec3* p5);

extern "C" {

// [FUN_503ac0] the contact record out (208 bytes) of car c's wheels (248 bytes from +248, count +3540) that are
// on (+7156 flags) and touch (+24): a copy of the first one's record (+280, when +380 is set) or an empty one, with
// the mean contact point (+0) at +52, projected on the line +76..+88, and the mean normal (+12) at +64; 0 for none
int __fastcall Car_WheelContact(BYTE* c, void*, BYTE* out)
{
    Vec3 p[8], q[8], spare[8];
    int idx[8];
    memset(p, 0, sizeof p);
    memset(q, 0, sizeof q);
    memset(spare, 0, sizeof spare);
    int n = 0;
    for (int i = 0; i < F(int, c, 3540); i++) {
        BYTE* w = c + 248 + i * 248;
        if (!F(int, c, 7156 + i * 4) || !F(WORD, w, 24))
            continue;
        idx[n] = i;
        p[n] = F(Vec3, w, 0);
        q[n] = F(Vec3, w, 12);
        n++;
    }
    if (!n)
        return 0;
    Vec3 a = g_vs702650, b = g_vs702650;
    if (n == 1) {
        a = p[0];
        b = q[0];
    } else {
        for (int i = 0; i < n; i++) {
            a.x = a.x + p[i].x;
            a.y = a.y + p[i].y;
            a.z = a.z + p[i].z;
            b.x = b.x + q[i].x;
            b.y = b.y + q[i].y;
            b.z = b.z + q[i].z;
        }
        Vec3 t;
        Vec3_DivAssign(&a, 0, &t, (float)n);
        Vec3_DivAssign(&b, 0, &t, (float)n);
    }
    BYTE* w = c + idx[0] * 248;
    if (F(int, w, 380))
        memcpy(out, w + 280, 208);
    else {
        F(int, out, 0) = 0;
        memcpy(out + 4, (BYTE*)g_q98_7025f0, 48);
        F(int, out, 100) = 0;
        F(int, out, 108) = 0;
        F(int, out, 112) = 0;
        F(int, out, 116) = 0;
        F(int, out, 120) = 0;
        F(BYTE, out, 124) = 0;
    }
    Vec3 r;
    F(Vec3, out, 52) = *Line_Project(&r, &a, &b, (Vec3*)(out + 76), (Vec3*)(out + 88));
    F(Vec3, out, 64) = b;
    return 1;
}

} // extern "C"

// === constructing the car model table (the static initializer $L_506fc0)

extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
ASM_VAR(AsmLabel, g_cm697868, $L_697868)    // 80 models of 2416 bytes

// count empty vectors of size bytes at p
static void Cm_Vectors(BYTE* p, int size, int count) { CallStd(FUN_4011e0, p, size, count, (void*)FUN_4bd4d0); }

static void Cm_Zero(BYTE* m, int off, int n)
{
    for (int i = 0; i < n; i++)
        F(int, m, off + i * 4) = 0;
}

extern "C" {

// [$L_506fc0] constructs the 80 car models (697868): two identity matrices, the empty vectors of the wheels,
// lights, shapes and points, the zeroed values
void __cdecl CarModels_Construct()
{
    for (int k = 0; k < 80; k++) {
        BYTE* m = (BYTE*)g_cm697868 + k * 2416;
        Cm_Vectors(m, 12, 4);
        memcpy(m, (BYTE*)g_q98_7025f0, 48);
        Cm_Vectors(m + 48, 12, 4);
        memcpy(m + 48, (BYTE*)g_q98_7025f0, 48);
        Cm_Zero(m, 288, 3);
        Cm_Zero(m, 396, 3);
        Cm_Zero(m, 412, 3);
        Cm_Vectors(m + 424, 12, 8);
        Cm_Vectors(m + 520, 12, 8);
        Cm_Vectors(m + 616, 12, 8);
        Cm_Zero(m, 848, 3);
        Cm_Vectors(m + 880, 12, 20);
        Cm_Vectors(m + 1120, 12, 20);
        Cm_Zero(m, 1360, 9);
        Cm_Vectors(m + 1400, 12, 4);
        Cm_Vectors(m + 1448, 16, 20);
        Cm_Vectors(m + 1780, 12, 5);
        Cm_Zero(m, 1844, 9);
        for (int i = 0; i < 9; i++)
            CallThis(FUN_4bd4d0, m + 1880 + i * 12);
        for (int i = 0; i < 4; i++)
            CallThis(FUN_4bd4d0, m + 2000 + i * 12);
        CallThis(FUN_4bd4d0, m + 2380);
        CallThis(FUN_4bd4d0, m + 2392);
    }
}

} // extern "C"

// === whether an axle stands flat (FUN_506750)

ASM_VAR(double, g_ax64dac0, $L_64dac0)
ASM_VAR(float, g_ax6749f0, $L_6749f0)

extern "C" {

// [FUN_506750] whether wheel pair k (0: wheels 0-2, 1: 0-1, 2: 1-3, 3: 2-3; wheel i at +248 + 248i, its contact
// +24, ground normal +12) of car s touches ground of nearly the same slope and height (within 6749f0 times
// +8036 + 4k); 1 for cars of fewer than four wheels
int __fastcall Car_AxleFlat(BYTE* s, void*, unsigned k)
{
    if (F(int, s, 3540) < 4)
        return 1;
    if (k > 3)
        return 0;
    static const int pa[4] = {0, 0, 1, 2}, pb[4] = {2, 1, 3, 3};
    const BYTE* a = s + 248 + pa[k] * 248;
    const BYTE* b = s + 248 + pb[k] * 248;
    if (!F(WORD, a, 24) || !F(WORD, b, 24))
        return 0;
    const float* na = (const float*)(a + 12);
    const float* nb = (const float*)(b + 12);
    double dot = k == 3 ? (double)na[2] * nb[2] + (double)na[1] * nb[1] + (double)nb[0] * na[0]
                        : (double)na[2] * nb[2] + (double)na[1] * nb[1] + (double)na[0] * nb[0];
    if (!(dot > g_ax64dac0))
        return 0;
    const float* pa3 = (const float*)a;
    const float* pb3 = (const float*)b;
    const float* n = k == 0 ? nb : na;
    double dx = (double)pa3[0] - pb3[0], dy = (double)pa3[1] - pb3[1], dz = (double)pa3[2] - pb3[2];
    double e = dz * n[2] + dy * n[1] + dx * n[0];
    if (e < 0)
        e = -e;
    return g_ax6749f0 * (double)F(float, s, 8036 + k * 4) > e;
}

} // extern "C"

// === moving a control along its range (FUN_501b90)

ASM_VAR(float, g_cm64c264, $L_64c264)
ASM_VAR(float, g_cm64c250, $L_64c250)
ASM_VAR(float, g_cm64c938, $L_64c938)
ASM_VAR(double, g_cm64b670, $L_64b670)

extern "C" {

// [FUN_501b90] moves control k (of 27: at +15520 + 20k its step, +4 the steps, +12 the value -1..1) of car s by
// *amount at speed: a positive amount brings it back towards 0, a negative one out towards the end; what it
// uses is taken from *amount; 1 when it is all used (for the gears 3..10 the last step but one is the limit)
int __fastcall Control_Move(BYTE* s, void*, int k, float* amount, float speed)
{
    if (*amount == g_cm64c264 || k < 0 || k >= 27)
        return 0;
    BYTE* e = s + 15520 + k * 20;
    if (speed == g_cm64c264) {
        F(int, e, 12) = 0;
        F(int, e, 0) = 0;
        return 1;
    }
    double lim = speed * (double)F(float, e, 12);
    float limf = (float)lim;
    if (*amount > g_cm64c264) {
        double al = lim < 0 ? -lim : lim;
        if (!(al > *amount)) {
            *amount = (float)(*amount - al);
            F(int, e, 12) = 0;
            F(int, e, 0) = 0;
            return 0;
        }
        double q = *amount / (double)speed;
        if (limf > g_cm64c264) {
            F(float, e, 12) = (float)(F(float, e, 12) - q);
            if (F(float, e, 12) < g_cm64c264)
                F(int, e, 12) = 0;
        } else {
            F(float, e, 12) = (float)(q + F(float, e, 12));
            if (F(float, e, 12) > g_cm64c264)
                F(int, e, 12) = 0;
        }
        F(int, e, 0) = (int)(F(int, e, 4) * (double)F(float, e, 12));
        *amount = 0;
        return 1;
    }
    double v = F(float, e, 12);
    double room = (g_cm64b670 - (v < 0 ? -v : v)) * speed;
    double aa = *amount < 0 ? -(double)*amount : *amount;
    if (aa < room) {
        double q = *amount / (double)speed;
        if (!(lim < g_cm64c264)) {
            F(float, e, 12) = (float)(F(float, e, 12) - q);
            if (F(float, e, 12) > g_cm64c250)
                F(float, e, 12) = 1.0f;
        } else {
            F(float, e, 12) = (float)(q + F(float, e, 12));
            if (F(float, e, 12) < g_cm64c938)
                F(float, e, 12) = -1.0f;
        }
        int p = (int)(F(int, e, 4) * (double)F(float, e, 12));
        F(int, e, 0) = p;
        if (k >= 3 && k < 11 && p >= F(int, e, 4) - 2) {
            int last = F(int, e, 4) - 2;
            F(int, e, 0) = last;
            F(float, e, 12) = (float)(last / (double)F(int, e, 4));
            return 1;
        }
        *amount = 0;
        return 1;
    }
    *amount = (float)((room < 0 ? -room : room) + *amount);
    F(int, e, 0) = F(int, e, 4);
    F(float, e, 12) = lim > g_cm64c264 ? 1.0f : -1.0f;
    if (k >= 3 && k < 11 && F(int, e, 0) == F(int, e, 4) - 2) {
        int last = F(int, e, 4) - 2;
        F(int, e, 0) = last;
        F(float, e, 12) = (float)(last / (double)F(int, e, 4));
    }
    return 0;
}

} // extern "C"

// === a body in water (FUN_504930)

extern "C" Vec3* __fastcall Vec3_SetThis(Vec3* self, void*, float x, float y, float z);
ASM_VAR(float, g_bw674414, $L_674414)
ASM_VAR(float, g_bw702630, $L_702630)
ASM_VAR(float, g_bw702634, $L_702634)
ASM_VAR(float, g_bw702638, $L_702638)
ASM_VAR(float, g_bw64c258, $L_64c258)
ASM_VAR(float, g_bw64c264, $L_64c264)
ASM_VAR(float, g_bw64c250, $L_64c250)
ASM_VAR(float, g_bw64d990, $L_64d990)
ASM_VAR(double, g_bw64b898, $L_64b898)
ASM_VAR(double, g_bw64b670, $L_64b670)
ASM_VAR(double, g_bw64b788, $L_64b788)

extern "C" {

// [FUN_504930] body s in water: with a contact on water (kinds 4 and 7 among the +380 contacts) and its centre
// (+7356/+7380 mid, lowered along gravity by +7024 + 674414 * +8092) under the surface (frame row +24, +60):
// buoyancy against gravity (by 64d990, less by the depth half +15432) into +3392 and +13072 (+15436 set; deep
// cabs +13124), and the spin and velocity damped by it into +3216 and +3288; else the plain lift and no damping
void __fastcall Body_InWater(BYTE* s)
{
    int n = F(int, s, 3540), i = 0;
    for (; i < n; i++) {
        const int* c = F(const int*, s, 380 + i * 248);
        if (c && c[0] == 1 && (c[1] == 7 || c[1] == 4))
            break;
    }
    double h = g_bw674414 * (double)F(float, s, 8092) + F(float, s, 7024);
    float gx = (float)(g_bw702630 * h), gy = (float)(g_bw702634 * h);
    double gz = h * g_bw702638;
    double cx = (double)F(float, s, 7380) + F(float, s, 7356);
    float cy = F(float, s, 7360) + F(float, s, 7384), cz = F(float, s, 7364) + F(float, s, 7388);
    float hy = cy * g_bw64c258, hz = cz * g_bw64c258;
    float dx = (float)(cx * g_bw64c258 - gx), dy = hy - gy;
    double dz = hz - gz;
    float depth = (float)(dz * F(float, s, 48) + (double)dy * F(float, s, 36) + (double)dx * F(float, s, 24)
        + F(float, s, 60));
    Vec3 u, v, w;
    Vec3_SetThis(&u, 0, -g_bw702630, -g_bw702634, -g_bw702638);
    Vec3_SetThis(&v, 0, u.x * g_bw64d990, u.y * g_bw64d990, u.z * g_bw64d990);
    if (i < n && depth < g_bw64c264) {
        float a = (float)x87_fabs(depth);
        float b = a * g_bw64c258;
        if (b < g_bw64c264)
            b = 0;
        else if (b > g_bw64c250)
            b = 1.0f;
        double f = g_bw64b670 - b * g_bw64b898;
        Vec3_SetThis(&w, 0, (float)(v.x * f), (float)(v.y * f), (float)(v.z * f));
        F(Vec3, s, 3392) = w;
        F(int, s, 15436) = 1;
        F(Vec3, s, 13072) = w;
        F(float, s, 15432) = a;
        if (F(WORD, s, 10048) == 1 && a > g_bw64b788)
            F(int, s, 13124) = 1;
        Vec3 t;
        F(Vec3, s, 3216) = *Vec3_Scaled(&t, (const Vec3*)(s + 64), (float)-(F(float, s, 3520) * (double)F(float, s, 3320) * b));
        F(Vec3, s, 3288) = *Vec3_Scaled(&t, (const Vec3*)(s + 88), (float)-(b * (double)F(float, s, 3324)));
        return;
    }
    F(int, s, 15432) = 0;
    F(Vec3, s, 3392) = v;
    F(int, s, 15436) = 0;
    F(Vec3, s, 13072) = v;
    float z = g_bw64c264;
    Vec3_SetThis(&w, 0, z * F(float, s, 64), z * F(float, s, 68), z * F(float, s, 72));
    F(Vec3, s, 3216) = w;
    Vec3_SetThis(&w, 0, z * F(float, s, 88), z * F(float, s, 92), z * F(float, s, 96));
    F(Vec3, s, 3288) = w;
}

} // extern "C"

// === a hit shaking the cab (FUN_501010)

extern "C" int __cdecl rand(void);
extern "C" void Ch_Normalized(); KING_ALIAS("_Ch_Normalized", "_Vec3_Normalized")
extern "C" void Ch_RampDown(); KING_ALIAS("_Ch_RampDown", "@Ramp_Down@12")
ASM_VAR(float, g_ch64c40c, $L_64c40c)
ASM_VAR(float, g_ch64c250, $L_64c250)
ASM_VAR(float, g_ch64c264, $L_64c264)
ASM_VAR(float, g_ch64c408, $L_64c408)
ASM_VAR(float, g_ch64c320, $L_64c320)
ASM_VAR(float, g_ch64da74, $L_64da74)
ASM_VAR(float, g_ch64da70, $L_64da70)
ASM_VAR(float, g_ch702630, $L_702630)
ASM_VAR(float, g_ch702634, $L_702634)
ASM_VAR(float, g_ch702638, $L_702638)
ASM_VAR(double, g_ch64b6d0, $L_64b6d0)
ASM_VAR(double, g_ch64b6e0, $L_64b6e0)
ASM_VAR(double, g_ch64da68, $L_64da68)
ASM_VAR(double, g_ch64da60, $L_64da60)
ASM_VAR(double, g_ch64d310, $L_64d310)

extern "C" {

// [FUN_501010] cab s (with +10058) hit at p (normal n) with force f above 64c40c, unless p lies below both its
// sides: the wheels near the hit get their ramps advanced (by the force over the distance and the upward part of
// n) and, on a car (+10048) when hard enough at random, a setting changed; a mostly sideways hit raises the
// shake level (+15532, steps +15520 of +15524 with the 16060 ramp at 2 and 3), a mostly upward one less so; the
// cab sway +15552 and its count +15540 of +15544 follow
void __fastcall Cab_Hit(BYTE* s, void*, const float* p, const float* n, float f)
{
    if (!F(WORD, s, 10058) || f < g_ch64c40c)
        return;
    if (x87_fabs(p[1]) < p[2] && x87_fabs(p[0]) < p[2])
        return;
    f = f - g_ch64c250;
    float ax = (float)x87_fabs(p[0]);
    const float* M = (const float*)(s + 16);
    float u[3];
    u[0] = (float)((double)g_ch702638 * M[6] + (double)g_ch702634 * M[3] + (double)g_ch702630 * M[0]);
    u[1] = (float)((double)g_ch702638 * M[7] + (double)g_ch702634 * M[4] + (double)g_ch702630 * M[1]);
    u[2] = (float)((double)g_ch702638 * M[8] + (double)g_ch702634 * M[5] + (double)g_ch702630 * M[2]);
    float t[3];
    const float* nn = CallC<const float*>(Ch_Normalized, t, n);
    float c = (float)((double)nn[2] * u[2] + (double)nn[1] * u[1] + (double)nn[0] * u[0]);
    if (c < g_ch64c264)
        c = 0;
    const BYTE* ty = (const BYTE*)&g_carTypeNames + F(int, s, 2252) * 2416;
    for (int i = 0; i < F(int, ty, 44); i++, ty = (const BYTE*)&g_carTypeNames + F(int, s, 2252) * 2416) {
        const float* w = (const float*)(s + 7356 + i * 12);
        double dx = (double)w[0] - p[0];
        float fdx = (float)dx, dy = w[1] - p[1], dz = w[2] - p[2];
        float d2 = (float)((double)dz * dz + (double)dy * dy + dx * fdx);
        if (!(d2 < g_ch64b6d0))
            continue;
        float a = (float)(f * g_ch64b6e0 / (d2 + g_ch64b6e0) * c);
        Wheel_AdvanceRamps(s + 15520, i, a);
        if (!F(WORD, s, 10048) || !(d2 < g_ch64c408) || !(p[2] < g_ch64c264) || !(f > g_ch64c320))
            continue;
        int r = rand();
        if (!(g_ch64da70 - f * (double)g_ch64da74 < r))
            continue;
        if (!F(int, s, 15560))
            Car_SetSettingRaw((Car*)s, 0, 2, rand() <= 16000 ? -1 : 1);
        else
            Car_StepSetting((Car*)s, 0, 2);
    }
    if (ax < p[1] * g_ch64da68) {
        float t2 = (float)(f * (double)F(float, s, 15536) + F(float, s, 15532));
        int cur = F(int, s, 15520), max = F(int, s, 15524);
        int top = cur < max ? cur + 1 : max;
        double frac = (double)top / max;
        double v = t2 > frac ? frac * g_ch64da60 : t2;
        if (v > g_ch64c250)
            v = g_ch64c250;
        F(float, s, 15532) = (float)v;
        int g = (int)(max * v);
        if (cur != g) {
            if (g == 2)
                CallThis(Ch_RampDown, s + 16060, 0x3F000000u);
            else if (g == 3)
                CallThis(Ch_RampDown, s + 16060, 0x3F4CCCCDu);
        }
        F(int, s, 15520) = g;
        F(float, s, 15552) = (float)(t2 * (double)F(float, s, 15556) + F(float, s, 15552));
        if (F(float, s, 15552) > g_ch64c250)
            F(DWORD, s, 15552) = 0x3F800000;
    } else if (ax < p[1] * g_ch64d310) {
        double h = f * g_ch64b6e0;
        F(float, s, 15532) = (float)(h * F(float, s, 15536) + F(float, s, 15532));
        if (F(float, s, 15532) > g_ch64c250)
            F(DWORD, s, 15532) = 0x3F800000;
        F(int, s, 15520) = (int)(F(int, s, 15524) * (double)F(float, s, 15532));
        F(float, s, 15552) = (float)(h * (h * F(float, s, 15556)) + F(float, s, 15552));
        if (F(float, s, 15552) > g_ch64c250)
            F(DWORD, s, 15552) = 0x3F800000;
    } else
        return;
    F(int, s, 15540) = (int)(F(int, s, 15544) * (double)F(float, s, 15552));
}

} // extern "C"

// === the wheels' ground contacts (FUN_504d30)

extern "C" int __cdecl sprintf(char*, const char*, ...);
extern "C" void Gc_DumpText(); KING_ALIAS("_Gc_DumpText", "_Dump_Text")
extern "C" void Gc_NormalizeThis(); KING_ALIAS("_Gc_NormalizeThis", "@Vec3_NormalizeThis@8")
extern "C" void Gc_AddAssign(); KING_ALIAS("_Gc_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Gc_Contact(); KING_ALIAS("_Gc_Contact", "@Wheel_FindGround@24")
extern "C" void Gc_State(); KING_ALIAS("_Gc_State", "@Body_GroundNormal@12")
ASM_VAR(int, g_gc696be8, $L_696be8)
ASM_VAR(AsmLabel, g_gc674a20, $L_674a20)
ASM_VAR(BYTE, g_gc698166, $L_698166)
ASM_VAR(BYTE, g_gc702650, $L_702650)
ASM_VAR(float, g_gc702630, $L_702630)
ASM_VAR(float, g_gc702634, $L_702634)
ASM_VAR(float, g_gc702638, $L_702638)
ASM_VAR(float, g_gc64d7b8, $L_64d7b8)
ASM_VAR(float, g_gc64c264, $L_64c264)
ASM_VAR(double, g_gc64b658, $L_64b658)
ASM_VAR(double, g_gc64dab8, $L_64dab8)

extern "C" {

// [FUN_504d30] finds each wheel's ground contact (FUN_503e30 at its hub +7356 lowered by the suspension length
// +7028, in body space): on the ground (+6564 zero) its contact point +100, world contact +6212 and the
// compression +3608 / +3544 (clamped to +8092, the wheel lifted when beyond), the mean ground normal +196 and
// grip +9904; in the air the wheel hangs at full length; then the axle flags +6580.. and the body's state; 0 when
// the body is replaced (+15444)
int __fastcall Wheels_FindGround(BYTE* s)
{
    memcpy(s + 196, &g_gc702650, 12);
    F(float, s, 9904) = 0;
    int count = 0;
    if (g_gc696be8 > 1) {
        char buf[100];
        sprintf(buf, (const char*)g_gc674a20, &g_gc698166 + F(int, s, 2252) * 2416, (double)F(float, s, 52),
            (double)F(float, s, 56), (double)F(float, s, 60), F(int, s, 3540));
        CallC(Gc_DumpText, buf);
    }
    for (int i = 0; i < F(int, s, 3540); i++) {
        memcpy(s + 3848 + i * 12, &g_gc702650, 12);
        float h = F(float, s, 7028 + i * 4);
        float rx = F(float, s, 7356 + i * 12) - g_gc702630 * h, ry = F(float, s, 7360 + i * 12) - g_gc702634 * h;
        double rz = F(float, s, 7364 + i * 12) - (double)h * g_gc702638;
        float pos[3];
        pos[0] = (float)(rz * F(float, s, 40) + (double)ry * F(float, s, 28) + (double)rx * F(float, s, 16) + F(float, s, 52));
        pos[1] = (float)(rz * F(float, s, 44) + (double)ry * F(float, s, 32) + (double)rx * F(float, s, 20) + F(float, s, 56));
        pos[2] = (float)(rz * F(float, s, 48) + (double)ry * F(float, s, 36) + (double)rx * F(float, s, 24) + F(float, s, 60));
        int r = CallThis<int>(Gc_Contact, s, 0, pos, 0, i);
        if (F(int, s, 15444))
            return 0;
        WORD* w = (WORD*)(s + 6564 + i * 2);
        float* c = (float*)(s + 100 + i * 12);
        float* wc = (float*)(s + 6212 + i * 12);
        if (r == 1) {
            float k = F(float, s, 3644 + i * 4);
            const float* nb = (const float*)(s + 248 + i * 248);
            c[0] = k * nb[3] + nb[0], c[1] = k * nb[4] + nb[1], c[2] = k * nb[5] + nb[2];
            float t[3];
            CallThis(Gc_AddAssign, s + 196, t, nb + 3);
            F(float, s, 9904) = F(float, s, 9904) + F(float, s, 9840 + i * 4);
            count++;
            float x = c[0], y = c[1], z = c[2];
            wc[0] = (float)((double)F(float, s, 2328) * z + (double)F(float, s, 2316) * y + (double)F(float, s, 2304) * x +
                F(float, s, 2340));
            wc[1] = (float)((double)F(float, s, 2332) * z + (double)F(float, s, 2320) * y + (double)F(float, s, 2308) * x +
                F(float, s, 2344));
            wc[2] = (float)((double)F(float, s, 2336) * z + (double)F(float, s, 2324) * y + (double)F(float, s, 2312) * x +
                F(float, s, 2348));
            float d = F(float, s, 7364 + i * 12) - h - wc[2];
            F(float, s, 3608 + i * 4) = d;
            F(float, s, 3544 + i * 4) = d;
            if (d > F(float, s, 8092 + i * 4)) {
                F(float, s, 3544 + i * 4) = F(float, s, 8092 + i * 4);
                *w = 1;
                continue;
            }
            if (d < g_gc64c264)
                F(float, s, 3544 + i * 4) = 0;
            if (F(float, s, 3360 + i * 4) < g_gc64b658 && !*w)
                F(float, s, 3360 + i * 4) = (float)(x87_fabs(nb[5]) * F(float, s, 9792) * g_gc64dab8);
            *w = 0;
        } else {
            float ox = g_gc702630 * g_gc64d7b8, oy = g_gc702634 * g_gc64d7b8, oz = g_gc702638 * g_gc64d7b8;
            double ax = (double)oz * F(float, s, 40) + (double)oy * F(float, s, 28) + (double)ox * F(float, s, 16);
            double ay = (double)ox * F(float, s, 20) + (double)oz * F(float, s, 44) + (double)oy * F(float, s, 32);
            double az = (double)ox * F(float, s, 24) + (double)oz * F(float, s, 48) + (double)oy * F(float, s, 36);
            c[0] = (float)(pos[0] - (float)ax), c[1] = (float)(pos[1] - (float)ay), c[2] = (float)(pos[2] - az);
            wc[0] = F(float, s, 7356 + i * 12) - ox, wc[1] = F(float, s, 7360 + i * 12) - oy;
            wc[2] = F(float, s, 7364 + i * 12) - oz;
            F(DWORD, s, 3608 + i * 4) = 0x447A0000;
            F(DWORD, s, 3544 + i * 4) = F(DWORD, s, 8092 + i * 4);
            *w = 1;
            F(float, s, 3360 + i * 4) = 0;
        }
    }
    float m1 = F(float, s, 3616) < F(float, s, 3620) ? F(float, s, 3620) : F(float, s, 3616);
    float m2 = F(float, s, 3608) < F(float, s, 3612) ? F(float, s, 3612) : F(float, s, 3608);
    F(float, s, 3640) = m2 < m1 ? m1 : m2;
    if (count) {
        CallThis(Gc_NormalizeThis, s + 196);
        F(float, s, 9904) = F(float, s, 9904) / count;
    } else {
        F(float, s, 196) = g_gc702630, F(float, s, 200) = g_gc702634, F(float, s, 204) = g_gc702638;
    }
    Body_InWater(s);
    Car_FindParts((Car*)s, 0);
    int n = F(int, s, 3540);
    const WORD* w = (const WORD*)(s + 6564);
    if (n == 6) {
        F(BYTE, s, 6583) = w[2] && w[3] && w[4] && w[5];
        F(BYTE, s, 6581) = w[0] && w[1];
        F(BYTE, s, 6582) = w[0] && w[2] && w[4];
        F(BYTE, s, 6584) = w[1] && w[3] && w[5];
    } else {
        F(BYTE, s, 6583) = w[2] && w[3];
        F(BYTE, s, 6581) = w[0] && w[1];
        F(BYTE, s, 6582) = w[0] && w[2];
        F(BYTE, s, 6584) = w[1] && w[3];
    }
    F(BYTE, s, 6580) = 1;
    F(BYTE, s, 6585) = F(BYTE, s, 6581) || F(BYTE, s, 6582) || F(BYTE, s, 6583) || F(BYTE, s, 6584);
    F(BYTE, s, 6586) = 0;
    for (int i = 0; i < n; i++) {
        F(BYTE, s, 6586) = F(BYTE, s, 6586) || w[i];
        F(BYTE, s, 6580) = F(BYTE, s, 6580) && w[i];
    }
    CallThis(Gc_State, s, 1);
    return (int)(s + 6564 + n * 2);
}

} // extern "C"

// === a wheel on its surface (FUN_502780)

extern "C" void Ws_InPatch(); KING_ALIAS("_Ws_InPatch", "_Patch_SegmentHits")
extern "C" void Ws_RampValue(); KING_ALIAS("_Ws_RampValue", "@Ramp_Value@8")
extern "C" void Ws_AddContact(); KING_ALIAS("_Ws_AddContact", "@Engine_AddContact@20")
extern "C" void Ws_CameraGet(); KING_ALIAS("_Ws_CameraGet", "@Camera_Get@12")
extern "C" void Ws_Length(); KING_ALIAS("_Ws_Length", "_Vec3_Length")
extern "C" void Ws_MulAssign(); KING_ALIAS("_Ws_MulAssign", "@Vec3_MulAssign@16")
ASM_VAR(int, g_ws69784c, $L_69784c)
ASM_VAR(BYTE*, g_ws6ef1fc, $L_6ef1fc)
ASM_VAR(float, g_ws6976c0, $L_6976c0)
ASM_VAR(float, g_ws697688, $L_697688)
ASM_VAR(float, g_ws6976f8, $L_6976f8)
ASM_VAR(float, g_ws696bdc, $L_696bdc)
ASM_VAR(BYTE, g_ws702650, $L_702650)
ASM_VAR(float, g_ws64c450, $L_64c450)
ASM_VAR(float, g_ws64c254, $L_64c254)
ASM_VAR(float, g_ws64c250, $L_64c250)
ASM_VAR(float, g_ws64c40c, $L_64c40c)
ASM_VAR(float, g_ws64c304, $L_64c304)
ASM_VAR(float, g_ws64d690, $L_64d690)
ASM_VAR(float, g_ws64c260, $L_64c260)
ASM_VAR(float, g_ws64c968, $L_64c968)
ASM_VAR(float, g_ws64c338, $L_64c338)
ASM_VAR(float, g_ws64c320, $L_64c320)
ASM_VAR(float, g_ws64daac, $L_64daac)
ASM_VAR(float, g_ws64daa8, $L_64daa8)
ASM_VAR(float, g_ws64da94, $L_64da94)
ASM_VAR(float, g_ws64da90, $L_64da90)
ASM_VAR(float, g_ws64da7c, $L_64da7c)
ASM_VAR(float, g_ws64da78, $L_64da78)
ASM_VAR(double, g_ws64b5b8, $L_64b5b8)
ASM_VAR(double, g_ws64b918, $L_64b918)
ASM_VAR(double, g_ws64d318, $L_64d318)
ASM_VAR(double, g_ws64daa0, $L_64daa0)
ASM_VAR(double, g_ws64da98, $L_64da98)
ASM_VAR(double, g_ws64d338, $L_64d338)
ASM_VAR(double, g_ws64d668, $L_64d668)
ASM_VAR(double, g_ws64b840, $L_64b840)
ASM_VAR(double, g_ws64c958, $L_64c958)
ASM_VAR(double, g_ws64b5a0, $L_64b5a0)
ASM_VAR(double, g_ws64da88, $L_64da88)
ASM_VAR(double, g_ws64b6e0, $L_64b6e0)
ASM_VAR(double, g_ws64da80, $L_64da80)
ASM_VAR(double, g_ws64b668, $L_64b668)
ASM_VAR(double, g_ws64d0f0, $L_64d0f0)
ASM_VAR(double, g_ws64d378, $L_64d378)

#define WS(off) F(float, s, (off) + w * 4)

// whether wheel w slips (spinning over lim on slip +slip, or locked by +6612) while on the ground
static bool Ws_Slips(BYTE* s, int w, int slip, double lim)
{
    bool spin = (F(BYTE, s, 6596 + w) && x87_fabs(WS(slip)) > lim) || F(int, s, 6612 + w * 4);
    return spin && !F(WORD, s, 6564 + w * 2);
}

// the surface's grip and roughness (+9840 / +9872) from its parameters: the first two of mat+12 with 8 bytes
static void Ws_Params(BYTE* s, int w, const int* mat)
{
    if (mat[2] * 4 == 8) {
        F(int, s, 9840 + w * 4) = ((const int*)mat[3])[0];
        F(int, s, 9872 + w * 4) = ((const int*)mat[3])[1];
    }
}

// the wheel's shake speed (+3676) pulled back by the travel speed v over m and kicked at random by a (times
// the travel's root when root), limited to lim; the shake (+3644) follows within the given bounds
static void Ws_Shake(BYTE* s, int w, float v, float m, float lim, double a, bool root, float dt, DWORD hi,
    float hiLim, DWORD lo, float loLim)
{
    float q = (float)(x87_fabs(v) / m);
    WS(3676) = (g_ws64c250 - q) * WS(3676);
    double r = rand() * (double)g_ws64c254;
    double t = r - ((double)v + m) / ((double)m + m);
    t = t * a * ((root ? x87_sqrt(q) : q) * g_ws64c958 + g_ws64b5b8);
    WS(3676) = (float)(t + WS(3676));
    if (WS(3676) > lim)
        WS(3676) = lim;
    if (-lim > WS(3676))
        WS(3676) = -lim;
    WS(3644) = dt * WS(3676) + WS(3644);
    if (WS(3644) > hiLim)
        F(DWORD, s, 3644 + w * 4) = hi;
    if (WS(3644) < loLim)
        F(DWORD, s, 3644 + w * 4) = lo;
}

extern "C" {

// [FUN_502780] wheel w of body s on contact c (its material c+100: kind +4, parameters +8 / +12): the
// surface's grip and roughness, gravel thrown at the camera (kind 8), the slip marks (+13520) and the wheel's
// shake by the surface kind; the kind (13 inside a patch +10496), 0 without a material
int __fastcall Wheel_Surface(BYTE* s, void*, BYTE* c, int w)
{
    float lim = 10.0f;
    float v = WS(3644);
    memcpy(s + 3848 + w * 12, &g_ws702650, 12);
    F(int, s, 3816 + w * 4) = 0;
    F(int, s, 3944 + w * 4) = 0;
    F(int, s, 13520 + w * 4) = 0;
    if (!F(int, c, 100))
        return 0;
    float k = WS(6500);
    if (k > g_ws64c450)
        k = 10.0f;
    float dt = (float)Game_FrameTime();
    float a = (float)x87_fabs(WS(6780));
    F(DWORD, s, 9872 + w * 4) = 0x3C23D70A;
    F(DWORD, s, 9840 + w * 4) = 0x3F000000;
    float b = (float)x87_fabs(WS(6748));
    if (g_ws69784c == 1)
        F(DWORD, s, 9840 + w * 4) = 0x3E99999A;
    const int* mat = F(const int*, c, 100);
    int kind = mat[1];
    if (F(WORD, s, 4144 + w * 248)) {
        float P[3], Q[3];
        memcpy(P, s + 4120 + w * 248, 12);
        memcpy(Q, c + 52, 12);
        for (int j = 0; j < F(int, s, 10704); j++)
            if (CallC<int>(Ws_InPatch, s + 10496 + j * 48, P, Q))
                kind = 13;
    }
    mat = F(const int*, c, 100);
    if (mat[0] == 1) {
        if (kind > 0 && kind <= 13) {
            int m4 = mat[1];
            WS(9872) = b * (&g_ws6976c0)[m4] + a * (&g_ws697688)[m4];
            WS(9840) = (&g_ws6976f8)[m4 + 14 * (g_ws69784c + F(int, s, 16204) * 2)];
        }
        int sk = F(const int*, c, 100)[1];
        bool shared = false;
        switch (sk) {
        case 8:
            shared = true;
            if (F(WORD, s, 6564 + w * 2))
                break;
            if (!F(WORD, s, 10048)) {
                BYTE tmp[48];
                const float* cam = CallThis<const float*>(Ws_CameraGet, g_ws6ef1fc, tmp);
                float d[3] = {cam[9] - F(float, s, 52), cam[10] - F(float, s, 56), cam[11] - F(float, s, 60)};
                double p = g_ws64b918 / (CallC<double>(Ws_Length, d) + g_ws64b5b8);
                if (!(rand() * (double)g_ws64c254 > p))
                    break;
            }
            {
                F(int, s, 15448) = 1;
                float* e = (float*)(s + 15452);
                float* at = (float*)(s + 15464);
                e[0] = F(float, c, 64) * g_ws64daac, e[1] = F(float, c, 68) * g_ws64daac,
                e[2] = F(float, c, 72) * g_ws64daac;
                memcpy(at, c + 52, 12);
                const int* m2 = F(const int*, c, 100);
                if (m2[2] * 4 >= 4)
                    F(int, s, 15448) = ((const int*)m2[3])[0];
                if (m2[2] * 4 >= 8) {
                    float g = ((const float*)m2[3])[1];
                    e[0] = g * F(float, c, 64), e[1] = g * F(float, c, 68), e[2] = g * F(float, c, 72);
                }
                float t[3];
                float f = F(float, s, 9792) * g_ws64daa8;
                CallThis(Ws_MulAssign, e, t, *(DWORD*)&f);
                float ed[3] = {e[0] / dt, e[1] / dt, e[2] / dt};
                float q[3];
                q[0] = (float)((double)F(float, s, 2316) * at[1] + (double)F(float, s, 2328) * at[2] +
                    (double)at[0] * F(float, s, 2304) + F(float, s, 2340));
                q[1] = (float)((double)F(float, s, 2320) * at[1] + (double)F(float, s, 2308) * at[0] +
                    (double)F(float, s, 2332) * at[2] + F(float, s, 2344));
                q[2] = (float)((double)F(float, s, 2324) * at[1] + (double)F(float, s, 2312) * at[0] +
                    (double)F(float, s, 2336) * at[2] + F(float, s, 2348));
                CallThis(Ws_AddContact, s + 9984, q, ed, &g_ws702650);
            }
            break;
        case 1:
        case 5:
            shared = true;
            break;
        case 2: {
            if (CallThis<float>(Ws_RampValue, s + 16164) == g_ws64c40c)
                WS(9872) = (float)(WS(9872) * g_ws64da88);
            Ws_Params(s, w, F(const int*, c, 100));
            if (F(WORD, s, 6564 + w * 2))
                break;
            if (F(WORD, s, 10048) && Ws_Slips(s, w, 6704, g_ws64daa0))
                F(int, s, 13520 + w * 4) = 1;
            if (F(WORD, s, 10048) && Ws_Slips(s, w, 6704, g_ws64daa0))
                F(int, s, 13520 + w * 4) = 2;
            float m;
            int gm = F(int, s, 10092);
            if (gm == 2)
                lim = (float)(k * g_ws64d338), m = (float)(WS(8092) * g_ws64d338);
            else if (gm == 1)
                lim = (float)(k * g_ws64b5b8), m = (float)(WS(8092) * g_ws64b840);
            else
                lim = (float)(k * g_ws64b6e0), m = (float)(WS(8092) * g_ws64b6e0);
            double kick = g_ws64b5b8 / (dt + g_ws64b5a0) * k * 2 / (k + g_ws64c450);
            Ws_Shake(s, w, v, m, lim, kick, true, dt, 0x3DCCCCCD, g_ws64c304, 0xBDCCCCCD, g_ws64d690);
            break;
        }
        case 6:
        case 10:
        case 11: {
            Ws_Params(s, w, F(const int*, c, 100));
            if (F(WORD, s, 6564 + w * 2))
                break;
            float q = (float)(x87_fabs(v) * g_ws64da80);
            WS(3676) = (g_ws64c250 - q) * WS(3676);
            double r = rand() * (double)g_ws64c254;
            double t = (r - ((double)v + g_ws64c260) * g_ws64da7c) * (k / ((double)k + g_ws64c450));
            t = t * (x87_sqrt(q) * g_ws64c958 + g_ws64b5b8);
            WS(3676) = (float)(t + WS(3676));
            if (WS(3676) > g_ws64c450)
                F(DWORD, s, 3676 + w * 4) = 0x41200000;
            if (WS(3676) < g_ws64c968)
                F(DWORD, s, 3676 + w * 4) = 0xC1200000;
            WS(3644) = dt * WS(3676) + WS(3644);
            if (WS(3644) > g_ws64c338)
                F(DWORD, s, 3644 + w * 4) = 0x3E99999A;
            if (WS(3644) < g_ws64da78)
                F(DWORD, s, 3644 + w * 4) = 0xBE99999A;
            break;
        }
        case 3:
            Ws_Params(s, w, F(const int*, c, 100));
            if (F(WORD, s, 6564 + w * 2))
                break;
            if (F(WORD, s, 10048) && Ws_Slips(s, w, 6748, g_ws64b840))
                F(int, s, 13520 + w * 4) = 1;
            if (F(WORD, s, 10048) && Ws_Slips(s, w, 6748, g_ws64b840))
                F(int, s, 13520 + w * 4) = 2;
            break;
        case 4:
        case 7:
        case 9: {
            if (F(WORD, s, 6564 + w * 2))
                break;
            Ws_Params(s, w, F(const int*, c, 100));
            float m;
            if (F(const int*, c, 100)[1] == 9)
                m = 0.05f, lim = 2.0f;
            else
                *(DWORD*)&m = 0x3E99999A;
            double kick = (double)k * g_ws64c320 / ((double)k + g_ws64c450);
            float q = (float)(x87_fabs(v) / m);
            WS(3676) = (g_ws64c250 - q) * WS(3676);
            double r = rand() * (double)g_ws64c254;
            double t = (r - ((double)v + m) / ((double)m + m)) * kick * (q * g_ws64c958 + g_ws64b5b8);
            WS(3676) = (float)(t + WS(3676));
            if (WS(3676) > lim)
                WS(3676) = lim;
            if (-lim > WS(3676))
                WS(3676) = -lim;
            WS(3644) = dt * WS(3676) + WS(3644);
            if (WS(3644) > m)
                WS(3644) = m;
            if (-m > WS(3644))
                WS(3644) = -m;
            break;
        }
        }
        if (shared) {
            const int* m2 = F(const int*, c, 100);
            int n = m2[2] * 4;
            if (n >= 8) {
                const int* p = 0;
                if (m2[1] == 8) {
                    if (n == 16)
                        p = (const int*)m2[3] + 2;
                } else
                    p = (const int*)m2[3];
                if (p) {
                    F(int, s, 9840 + w * 4) = p[0];
                    F(int, s, 9872 + w * 4) = p[1];
                    if (g_ws69784c == 1)
                        WS(9840) = (float)(WS(9840) * g_ws64d318);
                }
            }
            if (F(WORD, s, 10048) && Ws_Slips(s, w, 6704, g_ws64daa0))
                F(int, s, 13520 + w * 4) = 2;
            float m;
            int gm = F(int, s, 10092);
            if (gm == 2)
                lim = (float)(k * g_ws64da98), m = (float)(WS(8092) * g_ws64d338);
            else if (gm == 1)
                lim = (float)(k * g_ws64d668), m = (float)(WS(8092) * g_ws64b5b8);
            else
                lim = k, m = (float)(WS(8092) * g_ws64b840);
            double kick = g_ws64b5b8 / (dt + g_ws64b5a0) * k / (k + g_ws64c450);
            Ws_Shake(s, w, v, m, lim, kick, true, dt, 0x3CA3D70A, g_ws64da94, 0xBCA3D70A, g_ws64da90);
        }
    }
    double g = CallThis<float>(Ws_RampValue, s + 16164) * (double)WS(9840) * g_ws696bdc;
    WS(9840) = (float)g;
    int gm = F(int, s, 10092);
    int mk = F(const int*, c, 100)[1];
    if (gm == 2 && (mk == 1 || mk == 5))
        WS(9840) = (float)(g * g_ws64b668 + g_ws64d0f0);
    else if (gm)
        WS(9840) = (float)(g * g_ws64d378 + g_ws64b5b8);
    F(float, s, 15248 + w * 4) = WS(3644);
    return kind;
}

} // extern "C"

// === a segment against a four-cornered patch (FUN_502270)

extern "C" void Ph_PlaneHitT(); KING_ALIAS("_Ph_PlaneHitT", "_Vec3_PlaneHitT")
ASM_VAR(double, g_ph64b678, $L_64b678)
ASM_VAR(float, g_ph64c250, $L_64c250)
ASM_VAR(double, g_ph64b670, $L_64b670)

extern "C" {

// [FUN_502270] 1 when a or b lies inside the patch p[4] (inside all four edges, whose outward normals are the
// edges turned a quarter about z) or the segment a-b crosses an edge's plane within that edge
int __cdecl Patch_SegmentHits(const Vec3* p, const Vec3* a, const Vec3* b)
{
    Matrix R;
    Matrix_RotationZ(&R, 1.5707964f);
    Vec3 edge[4], nrm[4], hit[4], tmp;
    float len[4];
    double t[4];
    int flag[4];
    memset(edge, 0, sizeof edge);
    memset(nrm, 0, sizeof nrm);
    for (int i = 0; i < 4; i++)
    {
        if (i < 2)
        {
            edge[i].x = (float)((double)p[i + 1].x - p[i].x);
            edge[i].y = (float)((double)p[i + 1].y - p[i].y);
            edge[i].z = (float)((double)p[i + 1].z - p[i].z);
            double L = Vec3_Length(&edge[i]);
            len[i] = (float)L;
            if (L != g_ph64b678)
                CallThis(Ws_MulAssign, &edge[i], &tmp, (float)(g_ph64c250 / L));
        }
        else
        {
            len[i] = len[i - 2];
            edge[i].x = (float)-(double)edge[i - 2].x;
            edge[i].y = (float)-(double)edge[i - 2].y;
            edge[i].z = (float)-(double)edge[i - 2].z;
        }
    }
    const float* m = R.m;
    for (int i = 0; i < 4; i++)
    {
        if (i < 2)
        {
            const Vec3& e = edge[i];
            nrm[i].x = (float)((double)m[6] * e.z + (double)m[0] * e.x + (double)m[3] * e.y + m[9]);
            nrm[i].y = (float)((double)m[7] * e.z + (double)m[1] * e.x + (double)m[4] * e.y + m[10]);
            nrm[i].z = (float)((double)m[8] * e.z + (double)m[2] * e.x + (double)m[5] * e.y + m[11]);
        }
        else
        {
            nrm[i].x = (float)-(double)nrm[i - 2].x;
            nrm[i].y = (float)-(double)nrm[i - 2].y;
            nrm[i].z = (float)-(double)nrm[i - 2].z;
        }
    }
    for (int k = 0; k < 2; k++)
    {
        const Vec3* q = k ? b : a;
        int in = 1;
        for (int i = 0; i < 4; i++)
            in = in && ((double)q->z - p[i].z) * nrm[i].z + ((double)q->y - p[i].y) * nrm[i].y
                               + ((double)q->x - p[i].x) * nrm[i].x < g_ph64b678;
        if (in)
            return 1;
    }
    Vec3 d;
    d.x = (float)((double)b->x - a->x);
    d.y = (float)((double)b->y - a->y);
    d.z = (float)((double)b->z - a->z);
    double L = Vec3_Length(&d);
    if (L == g_ph64b678)
        return 0;
    CallThis(Ws_MulAssign, &d, &tmp, (float)(g_ph64b670 / L));
    memset(hit, 0, sizeof hit);
    for (int i = 0; i < 4; i++)
        hit[i] = *CallC<const Vec3*>(Ph_PlaneHitT, &tmp, &p[i], &nrm[i], a, &d, &t[i]);
    for (int i = 0; i < 4; i++)
    {
        flag[i] = t[i] >= g_ph64b678 && t[i] <= L;
        if (!flag[i])
            continue;
        double pr = ((double)hit[i].x - p[i].x) * edge[i].x + ((double)hit[i].y - p[i].y) * edge[i].y
                    + ((double)hit[i].z - p[i].z) * edge[i].z;
        if (pr < g_ph64b678 || pr > len[i])
            flag[i] = 0;
    }
    return flag[0] || flag[1] || flag[2] || flag[3];
}

} // extern "C"

// === the ground normal under four wheels (FUN_506210)

extern "C" void Gn_AddAssign(); KING_ALIAS("_Gn_AddAssign", "@Vec3_AddAssign@16")

// (v x u) for u = b - a kept unrounded and v = c - a with its x, y rounded, as the code keeps them
static Vec3 Gn_Normal(const float* a, const float* b, const float* c)
{
    double ux = (double)b[0] - a[0], uy = (double)b[1] - a[1], uz = (double)b[2] - a[2];
    float vx = (float)((double)c[0] - a[0]), vy = (float)((double)c[1] - a[1]);
    double vz = (double)c[2] - a[2];
    Vec3 n;
    n.x = (float)(uz * vy - uy * vz);
    n.y = (float)(vz * ux - uz * vx);
    n.z = (float)(uy * vx - (double)vy * ux);
    return n;
}

extern "C" {

// [FUN_506210] body s's ground normal +208: the mean of the unit normals of the triangles its four wheels'
// contacts (+248 + 248 i, touching when the word +272 + 248 i is set) make; +196 when it has not four wheels,
// sits on +2232, is held (+6585 with need set and +3640 below 1 needed) or, when need is set, no triangle counts
void __fastcall Body_GroundNormal(BYTE* s, void*, int need)
{
    if (F(int, s, 3540) != 4)
    {
        F(Vec3, s, 208) = F(Vec3, s, 196);
        return;
    }
    Vec3 n[8], u[8], sum = g_zeroVec, tmp;
    memset(n, 0, sizeof n);
    memset(u, 0, sizeof u);
    int count = 0;
    if ((s[6585] && (!need || !(F(float, s, 3640) < g_ph64c250))) || F(int, s, 2232))
    {
        F(Vec3, s, 208) = F(Vec3, s, 196);
        return;
    }
    const float* P[4];
    bool t[4];
    for (int i = 0; i < 4; i++)
    {
        P[i] = (const float*)(s + 248 + i * 248);
        t[i] = F(WORD, s, 272 + i * 248) != 0;
    }
    int valid[4];
    valid[0] = t[0] && t[1] && t[2];
    if (valid[0])
        n[0] = Gn_Normal(P[0], P[1], P[2]);
    valid[1] = t[0] && t[1] && t[3];
    if (valid[1])
        n[1] = Gn_Normal(P[1], P[3], P[0]);
    valid[2] = t[0] && t[2] && t[3];
    if (valid[2])
    {
        float ax = (float)((double)P[0][0] - P[2][0]), ay = (float)((double)P[0][1] - P[2][1]), az = (float)((double)P[0][2] - P[2][2]);
        float bx = (float)((double)P[3][0] - P[2][0]), by = (float)((double)P[3][1] - P[2][1]), bz = (float)((double)P[3][2] - P[2][2]);
        n[2].x = (float)((double)by * az - (double)bz * ay);
        n[2].y = (float)((double)bz * ax - (double)bx * az);
        n[2].z = (float)((double)bx * ay - (double)by * ax);
    }
    valid[3] = t[1] && t[2] && t[3];
    if (valid[3])
    {
        Vec3 c, d;
        c.x = (float)((double)P[2][0] - P[3][0]);
        c.y = (float)((double)P[2][1] - P[3][1]);
        c.z = (float)((double)P[2][2] - P[3][2]);
        Vec3_Cross(&n[3], Vec3_Sub(&d, (const Vec3*)P[1], (const Vec3*)P[3]), &c);
    }
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        if (!valid[i])
            continue;
        u[i] = *Vec3_Normalized(&tmp, &n[i]);
        CallThis(Gn_AddAssign, &sum, &tmp, &u[i]);
        count++;
    }
    if (need && !count)
        F(Vec3, s, 208) = F(Vec3, s, 196);
    else
        F(Vec3, s, 208) = *Vec3_Normalized(&tmp, &sum);
}

} // extern "C"

// === sliding a body's move along what it hits (FUN_506ae0)

extern "C" void Sl_SetThis(); KING_ALIAS("_Sl_SetThis", "@Vec3_SetThis@20")
ASM_VAR(float, g_sl64c334, $L_64c334)

static Vec3 Sl_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_506ae0] body s's move from +2292 to +52 cast against its world (+6672, kind 6) up to three times, each
// hit turning the rest of the move along the surface (pushed off it a little more each time): when every cast
// hits the body stays at +2292 (0); else it ends where the last free move does, +2292 shifted alike, the
// inverse frames renewed and the hit object (+244) kept (1)
int __fastcall Body_SlideMove(BYTE* s, void*)
{
    BYTE nd[256];
    CallThis(FUN_5ef5c0, nd);
    memcpy(nd + 4, &g_identity, 48);
    Vec3* P = &F(Vec3, s, 52);
    Vec3* Q = &F(Vec3, s, 2292);
    Vec3 d = Sl_Set((double)P->x - Q->x, (double)P->y - Q->y, (double)P->z - Q->z);
    Vec3 p0 = *P, w = d, tmp;
    F(Vec3, nd, 76) = *P;
    F(Vec3, nd, 88) = Sl_Set((double)d.x + p0.x, (double)d.y + p0.y, (double)d.z + p0.z);
    F(int, nd, 116) = 0;
    F(int, nd, 0) = F(int, s, 6668);
    int res = 0;
    for (int i = 0; i < 3; i++)
    {
        BYTE* o = F(BYTE*, s, 6672);
        res = CallVirt<int>(o, 52, 6, nd);
        if ((res & 2) && !F(BYTE*, nd, 104))
        {
            F(int, s, 15444) = 1;
            F(BYTE*, nd, 104) = o;
        }
        if (!(res & 1))
        {
            *P = Sl_Set((double)p0.x + w.x, (double)w.y + p0.y, (double)w.z + p0.z);
            Vec3 e = Sl_Set((double)w.x - d.x, (double)w.y - d.y, (double)w.z - d.z);
            CallThis(Gn_AddAssign, Q, &tmp, &e);
            Matrix inv;
            memcpy(s + 2304, Matrix_Inverse(&inv, (const Matrix*)(s + 16)), 48);
            memcpy(s + 2352, Matrix_Inverse(&inv, (const Matrix*)(s + 2256)), 48);
            if (res & 2)
                F(BYTE*, s, 244) = F(BYTE*, nd, 104);
            return 1;
        }
        const Vec3& n = F(Vec3, nd, 64);
        double L = Vec3_Length(&w);
        Vec3 nn = Sl_Set(n.x * L, n.y * L, n.z * L);
        double k = i;
        Vec3 m = Sl_Set(nn.x * k, nn.y * k, nn.z * k);
        Vec3 cc = Sl_Set(m.x * (double)g_sl64c334, m.y * (double)g_sl64c334, m.z * (double)g_sl64c334);
        Vec3 cr = Sl_Set((double)n.y * w.z - (double)n.z * w.y, (double)n.z * w.x - (double)w.z * n.x,
                         (double)w.y * n.x - (double)n.y * w.x);
        Vec3 t, t2;
        Vec3_Cross(&t, &cr, &n);
        w = *Vec3_Add(&t2, &t, &cc);
        F(Vec3, nd, 76) = p0;
        Vec3_Add(&F(Vec3, nd, 88), &p0, &w);
    }
    *P = *Q;
    Matrix inv;
    memcpy(s + 2304, Matrix_Inverse(&inv, (const Matrix*)(s + 16)), 48);
    return 0;
}

} // extern "C"

// === placing the wheels on the ground for a new frame (FUN_505530)

extern "C" void Gp_PlaneHit(); KING_ALIAS("_Gp_PlaneHit", "_Vec3_PlaneHit")
extern "C" void Gp_MulAssign(); KING_ALIAS("_Gp_MulAssign", "@Vec3_MulAssign@16")
ASM_VAR(float, g_gp64d980, $L_64d980)
ASM_VAR(double, g_gp64b678, $L_64b678)
ASM_VAR(double, g_gp64d6b8, $L_64d6b8)
ASM_VAR(double, g_gp64b670, $L_64b670)
ASM_VAR(int, g_gp6ce33c, $L_6ce33c)
ASM_VAR(int, g_gp6ce340, $L_6ce340)

// wheel i's hub +100 taken from its contact record (+248 + 248 i) and turned into the body (+6212 by the inverse I),
// its spring length (+3608 and +3544) from the hub height +7364
static void Gp_TakeHub(BYTE* s, int i, const float* I)
{
    Vec3 v = F(Vec3, s, 248 + 248 * i);
    F(Vec3, s, 100 + 12 * i) = v;
    Vec3& b = F(Vec3, s, 6212 + 12 * i);
    b.x = (float)((double)I[0] * v.x + (double)I[6] * v.z + (double)I[3] * v.y + I[9]);
    b.y = (float)((double)I[7] * v.z + (double)I[4] * v.y + (double)I[1] * v.x + I[10]);
    b.z = (float)((double)I[8] * v.z + (double)I[5] * v.y + (double)I[2] * v.x + I[11]);
    float h = (float)((double)F(float, s, 7364 + 12 * i) - F(float, s, 7028 + 4 * i) - F(float, s, 6220 + 12 * i));
    F(float, s, 3608 + 4 * i) = h;
    F(float, s, 3544 + 4 * i) = h;
}

extern "C" {

// [FUN_505530] car s set on the ground in frame M: each wheel's hub (+7356) turned by M, the two lowest (or with
// +3528 >= 0 the two highest) asked for contacts (FUN_503e30) and the others copied from the nearest touching wheel
// and its plane, then the springs, the mean ground normal +196 and point +8724, the axle flags and the ground state;
// returns M
const Matrix* __fastcall Car_PlaceOnGround(BYTE* s, void*, const Matrix* Mx)
{
    Matrix Iv;
    Matrix_Inverse(&Iv, Mx);
    const float* M = Mx->m;
    const float* I = Iv.m;
    F(Vec3, s, 196) = g_zeroVec;
    F(Vec3, s, 8724) = g_zeroVec;
    F(int, s, 9904) = 0;
    int cnt = 0;
    Vec3 P[8];
    for (int k = 0; k < 8; k++)
        P[k].x = P[k].y = P[k].z = 0;
    int n = F(int, s, 3540);
    for (int i = 0; i < n; i++)
    {
        F(Vec3, s, 3848 + 12 * i) = g_zeroVec;
        F(int, s, 3816 + 4 * i) = 0;
        F(float, s, 9872 + 4 * i) = 0.01f;
        F(float, s, 9840 + 4 * i) = 0.5f;
        const Vec3& v = F(Vec3, s, 7356 + 12 * i);
        P[i].x = (float)((double)v.x * M[0] + (double)v.y * M[3] + (double)v.z * M[6] + M[9]);
        P[i].y = (float)((double)M[1] * v.x + (double)v.z * M[7] + (double)v.y * M[4] + M[10]);
        P[i].z = (float)((double)M[8] * v.z + (double)M[5] * v.y + (double)v.x * M[2] + M[11]);
    }
    // the two extreme hubs by height
    int Fl[8] = {0}, R[8] = {0};
    int lo1 = 0, lo2 = 0, hi1 = 0, hi2 = 0;
    float min1 = 100.0f, min2 = 100.0f, max1 = -100.0f;
    double max2 = g_gp64d980;
    n = F(int, s, 3540);
    for (int i = 0; i < n; i++)
    {
        float y = F(float, s, 7360 + 12 * i);
        if (!(y > min2))
        {
            if (!(y > min1))
            {
                min2 = min1;
                lo2 = lo1;
                min1 = y;
                lo1 = i;
            }
            else
            {
                min2 = y;
                lo2 = i;
            }
        }
        if (!(max2 > y))
        {
            if (y < max1)
            {
                max2 = y;
                hi2 = i;
            }
            else
            {
                max2 = max1;
                hi2 = hi1;
                max1 = y;
                hi1 = i;
            }
        }
    }
    if (n > 0)
    {
        if (F(float, s, 3528) < g_gc64c264)
            Fl[lo1] = Fl[lo2] = 1;
        else
            Fl[hi1] = Fl[hi2] = 1;
    }
    for (int i = 0; i < F(int, s, 3540); i++)
        if (Fl[i])
            R[i] = CallThis<int>(Gc_Contact, s, 0, &P[i], 0, i);
    const float* Z = &g_axisZ.x;
    Vec3 up;
    up.x = (float)((double)Z[2] * M[6] + (double)Z[0] * M[0] + (double)Z[1] * M[3]);
    up.y = (float)((double)Z[0] * M[1] + (double)Z[2] * M[7] + (double)Z[1] * M[4]);
    up.z = (float)((double)Z[2] * M[8] + (double)Z[1] * M[5] + (double)Z[0] * M[2]);
    // the others from the nearest touching wheel
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        if (Fl[i])
            continue;
        float best = 1000.0f;
        int bi = -1;
        n = F(int, s, 3540);
        for (int j = 0; j < n; j++)
        {
            if (!Fl[j] || !F(short, s, 272 + 248 * j))
                continue;
            float dx = (float)((double)F(float, s, 7356 + 12 * j) - F(float, s, 7356 + 12 * i));
            float dy = (float)((double)F(float, s, 7360 + 12 * j) - F(float, s, 7360 + 12 * i));
            double dz = (double)F(float, s, 7364 + 12 * j) - F(float, s, 7364 + 12 * i);
            double dd = (double)dx * dx + (double)dy * dy + dz * dz;
            if (dd < best)
            {
                best = (float)dd;
                bi = j;
            }
        }
        if (bi < 0)
            continue;
        memcpy(s + 13584 + 208 * i, s + 13584 + 208 * bi, 208);
        memcpy(s + 248 + 248 * i, s + 248 + 248 * bi, 248);
        Vec3 h;
        F(Vec3, s, 248 + 248 * i) = *CallC<Vec3*>(Gp_PlaneHit, &h, s + 248 + 248 * bi, s + 260 + 248 * bi, &P[i], &up);
        R[i] = R[bi];
    }
    // the springs
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        short* touch = (short*)(s + 6564) + i;
        BYTE* rec = s + 248 + 248 * i;
        int c = (!Fl[i] && R[i]) ? 0 : 1;
        bool fresh = false;
        if ((Fl[i] && R[i] == 1) || (!c && R[i] == 1))
        {
            Gp_TakeHub(s, i, I);
            float h = F(float, s, 3608 + 4 * i);
            if (!(h >= g_gp64b678 && h <= F(float, s, 8092 + 4 * i)) && !c)
                fresh = true;
        }
        else if (!c)
            fresh = true;
        if (fresh)
        {
            R[i] = CallThis<int>(Gc_Contact, s, 0, &P[i], 0, i);
            if (R[i])
                Gp_TakeHub(s, i, I);
        }
        if (R[i])
        {
            Vec3 t;
            CallThis(Gc_AddAssign, s + 196, &t, rec + 12);
            F(float, s, 9904) = (float)((double)F(float, s, 9840 + 4 * i) + F(float, s, 9904));
            CallThis(Gc_AddAssign, s + 8724, &t, s + 100 + 12 * i);
            cnt++;
            if (!(F(float, s, 3544 + 4 * i) <= F(float, s, 8092 + 4 * i)))
            {
                F(float, s, 3544 + 4 * i) = F(float, s, 8092 + 4 * i);
                *touch = 1;
            }
            else
            {
                if (F(float, s, 3544 + 4 * i) < g_gc64c264)
                    F(float, s, 3544 + 4 * i) = 0;
                F(float, s, 3360 + 4 * i) = (float)(x87_fabs((double)F(float, rec, 20))
                                                    * (F(float, s, 9792) / (double)F(int, s, 3540)) * g_gp64d6b8);
                *touch = 0;
            }
        }
        else
        {
            double k = g_gc64d7b8;
            double zx = Z[0] * k, zy = Z[1] * k, zz = Z[2] * k;
            float wx = (float)(zx * M[0] + zy * M[3] + zz * M[6]);
            float wy = (float)(zx * M[1] + zz * M[7] + zy * M[4]);
            double wz = zz * M[8] + zy * M[5] + zx * M[2];
            Vec3& hub = F(Vec3, s, 100 + 12 * i);
            hub.x = (float)((double)P[i].x - wx);
            hub.y = (float)((double)P[i].y - wy);
            hub.z = (float)(P[i].z - wz);
            float ky = (float)(Z[1] * k), kz = (float)(Z[2] * k);
            Vec3& b = F(Vec3, s, 6212 + 12 * i);
            b.x = (float)(F(float, s, 7356 + 12 * i) - Z[0] * k);
            b.y = (float)((double)F(float, s, 7360 + 12 * i) - ky);
            b.z = (float)((double)F(float, s, 7364 + 12 * i) - kz);
            F(float, s, 3608 + 4 * i) = 1000.0f;
            F(float, s, 3544 + 4 * i) = F(float, s, 8092 + 4 * i);
            F(float, s, 3360 + 4 * i) = 0;
            *touch = 1;
        }
    }
    // the longest of the first four springs
    float* a = F(float, s, 3616) < F(float, s, 3620) ? &F(float, s, 3620) : &F(float, s, 3616);
    float* b = F(float, s, 3608) < F(float, s, 3612) ? &F(float, s, 3612) : &F(float, s, 3608);
    F(float, s, 3640) = *b < *a ? *a : *b;
    if (cnt)
    {
        CallThis(Gc_NormalizeThis, s + 196);
        double q = g_gp64b670 / cnt;
        float f = (float)q;
        F(float, s, 9904) = (float)(q * F(float, s, 9904));
        Vec3 t;
        CallThis(Gp_MulAssign, s + 8724, &t, f);
    }
    else
    {
        F(Vec3, s, 196) = g_axisZ;
        F(Vec3, s, 8724) = g_zeroVec;
    }
    F(int, s, 2240) = Car_AxleFlat(s, 0, 1);
    F(int, s, 2248) = Car_AxleFlat(s, 0, 3);
    F(int, s, 2232) = F(int, s, 2236) && F(int, s, 2240) && F(int, s, 2244) && F(int, s, 2248) ? 1 : 0;
    n = F(int, s, 3540);
    short* W = (short*)(s + 6564);
    if (n == 3 && F(int, s, 2252) >= g_gp6ce33c && F(int, s, 2252) < g_gp6ce340)
    {
        s[6583] = W[0] && W[1];
        s[6581] = s[6568];
        s[6582] = W[0] && W[2];
        s[6584] = W[1] && W[2];
    }
    else
    {
        s[6583] = W[2] && W[3];
        s[6581] = W[0] && W[1];
        s[6582] = W[0] && W[2];
        s[6584] = W[1] && W[3];
    }
    s[6580] = 1;
    s[6586] = 0;
    for (int k = 0; k < n; k++)
    {
        s[6580] = s[6580] && W[k];
        s[6586] = s[6586] || W[k];
    }
    s[6585] = s[6581] || s[6582] || s[6583] || s[6584];
    CallThis(Gc_State, s, 0);
    return Mx;
}

} // extern "C"

// === a wheel's ground contact (FUN_503e30)

extern "C" void Wg_RayTest(); KING_ALIAS("_Wg_RayTest", "@Vehicle_RayTest@16")
extern "C" void Wg_WheelContact(); KING_ALIAS("_Wg_WheelContact", "@Car_WheelContact@12")
extern "C" void Wg_Parts1(); KING_ALIAS("_Wg_Parts1", "_Query_Parts1")
extern "C" void Wg_Parts2(); KING_ALIAS("_Wg_Parts2", "_Query_Parts2")
extern "C" void Wg_LineProject(); KING_ALIAS("_Wg_LineProject", "_Line_Project")
extern "C" void Wg_Surface(); KING_ALIAS("_Wg_Surface", "@Wheel_Surface@16")
extern "C" void Wg_SubAssign(); KING_ALIAS("_Wg_SubAssign", "@Vec3_SubAssign@16")
ASM_PROC(FUN_503500)
extern "C" void Wg_NodeInit(); KING_ALIAS("_Wg_NodeInit", "@Node_Init@8")
ASM_VAR(int, g_wg697660, $L_697660)                 // the contact count
ASM_VAR(double, g_wg697658, $L_697658)              // and its time
ASM_VAR(AsmLabel, g_wg697678, $L_697678)
ASM_VAR(AsmLabel, g_wg697914, $L_697914)
ASM_VAR(char, g_wg674a14, $L_674a14)
ASM_VAR(char, g_wg674a08, $L_674a08)
ASM_VAR(char, g_wg674a00, $L_674a00)
ASM_VAR(char, g_wg6749f4, $L_6749f4)
ASM_VAR(float, g_wg64c258, $L_64c258)
ASM_VAR(double, g_wg64b670, $L_64b670)
ASM_VAR(double, g_wg64b678, $L_64b678)

// k times the car's side axis (+16 rows) for the wheel offsets
static Vec3 Wg_Side(BYTE* s, double k)
{
    float kx = (float)(k * g_axisX.x), ky = (float)(k * g_axisX.y);
    double kz = k * g_axisX.z;
    Vec3 v;
    v.x = (float)(kz * F(float, s, 40) + (double)ky * F(float, s, 28) + (double)kx * F(float, s, 16));
    v.y = (float)((double)kx * F(float, s, 20) + kz * F(float, s, 44) + (double)ky * F(float, s, 32));
    v.z = (float)((double)kx * F(float, s, 24) + kz * F(float, s, 48) + (double)ky * F(float, s, 36));
    return v;
}

static int Wg_Done(double t0, int count, int r)
{
    g_wg697660 += count;
    g_wg697658 = Timer_Now() - t0 + g_wg697658;
    return r;
}

extern "C" {

// [FUN_503e30] car s's wheel i at hub p on object obj (none: found by Vehicle_RayTest): up to three rays along the
// car's down axis, longer each time (+3520 speed, +7028 + 4 i spring), through the object's parts or the car's own
// test, into the wheel's record (+248 + 248 i: point, normal +12, object +240, touching +24 / +26, node +32) and
// with keep 0 its offset point, surface and node copy (+13584 + 208 i); without a hit the old contact projected on
// the line; 1 touching, 0 not, 2 no object
int __fastcall Wheel_FindGround(BYTE* s, void*, int keep, const Vec3* p, BYTE* obj, int i)
{
    Vec3 P = *p;
    BYTE nd[208];
    CallThis(Wg_NodeInit, nd);
    BYTE* rec = s + 248 * (i + 1);
    double t0 = Timer_Now();
    const float* Z = &g_axisZ.x;
    Vec3 D;
    D.x = (float)((double)Z[2] * F(float, s, 40) + (double)Z[1] * F(float, s, 28) + (double)Z[0] * F(float, s, 16));
    D.y = (float)((double)Z[2] * F(float, s, 44) + (double)Z[1] * F(float, s, 32) + (double)Z[0] * F(float, s, 20));
    D.z = (float)((double)Z[2] * F(float, s, 48) + (double)Z[1] * F(float, s, 36) + (double)Z[0] * F(float, s, 24));
    Vec3 U;
    U.x = (float)((double)P.x + Z[0]);
    U.y = (float)((double)P.y + Z[1]);
    U.z = (float)((double)P.z + Z[2]);
    if (!obj && !CallThis<int>(Wg_RayTest, s, &U, &obj))
    {
        F(int, rec, 240) = 0;
        F(short, rec, 24) = 0;
        F(Vec3, rec, 0) = P;
        F(int, rec, 132) = 0;
        F(short, rec, 26) = 0;
        F(Vec3, rec, 12) = P;
        return Wg_Done(t0, 1, 2);
    }
    if (F(int, s, 15444))
        return Wg_Done(t0, 1, 0);
    int it = 1, r;
    for (;;)
    {
        F(Matrix, nd, 4) = g_identity;
        double dt = Game_FrameTime();
        float L = (float)((dt * x87_sqrt(F(float, s, 3520)) + F(float, s, 7028 + 4 * i)) * (it * it) + g_wg64b670);
        F(int, nd, 116) = 0;
        F(int, nd, 0) = 0;
        double dx = (double)D.x * L;
        float dy = (float)((double)D.y * L), dz = (float)((double)D.z * L);
        F(float, nd, 76) = (float)(dx + P.x);
        F(float, nd, 80) = (float)((double)dy + P.y);
        F(float, nd, 84) = (float)((double)dz + P.z);
        F(float, nd, 88) = (float)(P.x - dx);
        F(float, nd, 92) = (float)((double)P.y - dy);
        F(float, nd, 96) = (float)((double)P.z - dz);
        F(int, nd, 120) = F(int, s, 10708);
        F(BYTE*, nd, 104) = F(int, s, 10076) ? obj : 0;
        if (F(int, s, 7156 + 4 * i))
        {
            if (F(short, s, 9992))
            {
                char buf[256];
                r = CallC<int>(Wg_Parts2, obj, 4, nd);
                int hit = r & 1;
                if (hit)
                {
                    if (g_gc696be8 > 1)
                    {
                        sprintf(buf, &g_wg674a14, i);
                        CallC(Gc_DumpText, buf);
                    }
                }
                else
                {
                    r = CallC<int>(Wg_Parts1, obj, 4, nd);
                    hit = r & 1;
                    if (hit)
                    {
                        if (g_gc696be8 > 1)
                        {
                            sprintf(buf, &g_wg674a08, i);
                            CallC(Gc_DumpText, buf);
                        }
                        if (!F(void*, nd, 100))
                            F(void*, nd, 100) = &g_wg697678;
                    }
                }
                if (hit && g_gc696be8 > 1)
                {
                    BYTE* m = F(BYTE*, nd, 100);
                    if (m)
                    {
                        sprintf(buf, &g_wg674a00, F(int, m, 0), F(int, m, 4));
                        CallC(Gc_DumpText, buf);
                    }
                    sprintf(buf, &g_wg6749f4, (double)F(float, nd, 52), (double)F(float, nd, 56), (double)F(float, nd, 60));
                    CallC(Gc_DumpText, buf);
                }
            }
            else
                r = CallThis<int>((void (*)())F(void**, obj, 0)[13], obj, 4, nd);
        }
        else
            r = CallThis<int>(Wg_WheelContact, s, nd);
        if ((r & 1) == 1)
            break;
        if (++it > 3)
        {
            // no hit: keep the old contact when it still lies under the wheel
            if (F(short, rec, 24) && !F(short, rec, 26))
            {
                Vec3 up, dn, q;
                up.x = (float)((double)D.x + P.x);
                up.y = (float)((double)P.y + D.y);
                up.z = (float)((double)P.z + D.z);
                dn.x = (float)((double)P.x - D.x);
                dn.y = (float)((double)P.y - D.y);
                dn.z = (float)((double)P.z - D.z);
                Vec3 Q = *CallC<Vec3*>(Wg_LineProject, &q, rec, rec + 12, &up, &dn);
                Vec3 e;
                e.x = (float)((double)P.x - Q.x);
                e.y = (float)((double)P.y - Q.y);
                e.z = (float)((double)P.z - Q.z);
                double len = Vec3_Length(&e);
                const float* n = (const float*)(rec + 12);
                if ((double)n[2] * e.z + (double)n[1] * e.y + (double)n[0] * e.x > g_wg64b678
                    && (double)n[2] * D.z + (double)n[1] * D.y + (double)n[0] * D.x > g_wg64b678
                    && len < (double)F(float, s, 8092 + 4 * i) + F(float, s, 7028 + 4 * i))
                {
                    F(BYTE*, rec, 240) = obj;
                    F(Vec3, rec, 0) = Q;
                    F(short, rec, 24) = 1;
                    F(short, rec, 26) = 1;
                    return Wg_Done(t0, it - 1, 1);
                }
                F(int, rec, 240) = 0;
            }
            else
            {
                F(int, rec, 240) = 0;
                F(Vec3, rec, 0).x = F(Vec3, rec, 0).y = F(Vec3, rec, 0).z = 0;
                F(Vec3, rec, 12) = g_zeroVec;
            }
            F(short, rec, 24) = 0;
            F(short, rec, 26) = 0;
            F(int, rec, 132) = 0;
            return Wg_Done(t0, it - 1, 0);
        }
    }
    // a hit
    if (!F(short, rec, 24) && !F(short, rec, 26))
    {
        F(short, rec, 26) = 1;
        return Wg_Done(t0, 0, 0);
    }
    F(BYTE*, rec, 240) = obj;
    memcpy(rec + 32, nd, 208);
    F(Vec3, rec, 0) = F(Vec3, nd, 52);
    F(short, rec, 24) = 1;
    F(short, rec, 26) = 0;
    F(Vec3, rec, 12) = F(Vec3, nd, 64);
    if (!keep)
    {
        BYTE* model = (BYTE*)&g_wg697914 + F(int, s, 2252) * 2416;
        Vec3 tmp, v;
        Vec3* hp = &F(Vec3, nd, 52);
        switch (i)
        {
        case 0:
            v = Wg_Side(s, (double)F(float, model, 12) * g_wg64c258);
            CallThis(Gc_AddAssign, hp, &tmp, &v);
            break;
        case 1:
            v = Wg_Side(s, (double)F(float, model, 12) * g_wg64c258);
            CallThis(Wg_SubAssign, hp, &tmp, &v);
            break;
        case 2:
        case 4:
            v = Wg_Side(s, (double)F(float, model, 16) * g_wg64c258);
            CallThis(Gc_AddAssign, hp, &tmp, &v);
            break;
        case 3:
        case 5:
            v = Wg_Side(s, (double)F(float, model, 16) * g_wg64c258);
            CallThis(Wg_SubAssign, hp, &tmp, &v);
            break;
        }
        int surf = CallThis<int>(Wg_Surface, s, nd, i);
        CallThis(FUN_503500, s, nd, i, surf);
    }
    memcpy(s + 13584 + 208 * i, nd, 208);
    return Wg_Done(t0, it, 1);
}

} // extern "C"
