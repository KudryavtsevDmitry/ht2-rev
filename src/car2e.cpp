// car2e.cpp - car2e.cpp of the game, rewritten from king.masm: small methods
// of the vehicle: drive wheel speeds, the forwarding to the gearbox functions
// of caran.cpp, and the 27 adjustable settings of a vehicle.
//
// Building with  nmake ASM_CAR2E=1  makes king.masm assemble the original
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
