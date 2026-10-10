// car.h - vehicles: declarations shared by the C++ code of the car units.
#pragma once

#include "king.h"
#include "x87.h"

// Engine and gearbox data of a vehicle type, one record per type
// [$L_6978c8]. The names are read from the formulas that use the fields.
struct CarType {
    float m_0;
    float wheelRadius;          // +4
    float m_8;
    float m_12;
    float m_16;
    BYTE  m_20[28];
    int   gearCount;            // +48  gearRatio[2..gearCount] are forward gears
    BYTE  m_52[24];
    float rollSpeedFactor;      // +76  rolling resistance grows with speed * this
    float airDrag;              // +80  resistance per speed squared
    BYTE  m_84[12];
    float maxTorque;            // +96
    float engineBrake;          // +100 torque without throttle (per rpm)
    float m_104;
    float finalDrive;           // +108
    float gearRatio[15];        // +112
    float torqueRpm;            // +172 engine speed of the highest torque
    BYTE  m_176[2416 - 176];
};
CHECK_OFFSET(CarType, gearCount, 48);
CHECK_OFFSET(CarType, rollSpeedFactor, 76);
CHECK_OFFSET(CarType, maxTorque, 96);
CHECK_OFFSET(CarType, finalDrive, 108);
CHECK_OFFSET(CarType, gearRatio, 112);
CHECK_OFFSET(CarType, torqueRpm, 172);
static_assert(sizeof(CarType) == 2416, "CarType size");

typedef CarType CarTypes[];
ASM_VAR(CarTypes, g_carTypes, $L_6978c8)

// caran.cpp: engine, gearbox and driving resistance
extern "C" {
double __cdecl Car_RollingFactor(float speed, float k);
double __cdecl Car_EngineBrake(float rpm, float k, float c, float d);
double __cdecl Car_EngineTorque(float rpm, float torqueRpm, float maxTorque, float scale);
double __cdecl Car_ThrottleTorque(int type, float full, float idle, float throttle);
double __cdecl Car_WheelForce(int type, float torque, int gear);
double __cdecl Car_Resistance(int type, float speed, float load);
float __cdecl Car_TopSpeedInGear(int type, int gear, float load, float* rpm);
float __cdecl Car_BestGear(int type, float load, int* gear, float* rpm);
float __cdecl Car_CruiseGear(int type, float load, int* gear, float* rpm, float* throttle);
double __cdecl Car_TopSpeedRatio(int type, float load, int* gear, float* rpm, float* speed, float* inverse);
double __cdecl Car_CruiseRatio(int type, float load, int* gear, float* rpm, float* speed, float* inverse,
                               float* throttle);
}
