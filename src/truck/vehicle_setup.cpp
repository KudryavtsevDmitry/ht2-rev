// vehicle_setup.cpp - car2e.cpp of the game, rewritten from king.masm: small methods
// of the vehicle: drive wheel speeds, the forwarding to the gearbox functions
// of caran.cpp, and the 27 adjustable settings of a vehicle.
//
// Building with  nmake ASM_VEHICLE_SETUP=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS     // strcpy as in the game
#include <string.h>
#include "car.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

ASM_PROC(FUN_506750)    // vehicle: part by index

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
ASM_PROC(FUN_4eaf98)    // filter: output
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
ASM_PROC(FUN_4eaf98)    // filter: output
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

