// caran.cpp - engine, gearbox and driving resistance of the vehicles,
// rewritten from king.masm: torque curve, wheel force, resistance, and from
// them the top speed in each gear, the best gear and the cruising gear.
// Everything is x87 math on g_carTypes, written after the rules in x87.h.
//
// Building with  nmake ASM_CARAN=1  makes king.masm assemble the original
// procedures again.

#include "car.h"

static const double TWO_PI = 6.2831852;     // the game's constants as written
static const double HALF_PI = 1.5707963;

// s / (s + 20) * (s + 6) / (2 (s + 3)) * k
double Car_RollingFactor(float speed, float k)                          // [FUN_507330]
{
    double s = speed;
    return s / (s + 20.0f) * (s + 6.0f) / ((s + 3.0f) + (s + 3.0f)) * k;
}

double Car_EngineBrake(float rpm, float k, float c, float d)            // [FUN_507360]
{
    return -((double)rpm * k) - x87_sqrt(d) * ((double)rpm * c);
}

// Torque at an engine speed: a bell around torqueRpm, rising more slowly
// below it. Below a tenth of torqueRpm the engine counts as turning at that.
double Car_EngineTorque(float rpm, float torqueRpm, float maxTorque, float scale) // [FUN_507380]
{
    double minRpm = torqueRpm * 0.1;
    if (!(rpm >= minRpm))
        rpm = (float)minRpm;
    double d = x87_fabs((double)rpm - torqueRpm);
    float torque = (float)(x87_exp(-x87_pow(d / torqueRpm, 4.0)) * ((double)maxTorque / scale));
    if (!(rpm >= torqueRpm))
        torque = (float)(x87_sqrt(x87_sin((double)rpm / torqueRpm * HALF_PI)) * torque);
    double f = 1.0f;
    if (!(rpm >= torqueRpm))
        f = x87_pow((double)rpm / torqueRpm, 0.2);
    return f * torque;
}

// full torque at throttle 1, idle torque at 0
double Car_ThrottleTorque(int /*type*/, float full, float idle, float throttle) // [FUN_507440]
{
    double t;
    if (!(throttle >= 0.0f))
        t = 0.0f;
    else if (throttle > 1.0f)
        t = 1.0f;
    else
        t = throttle;
    return (1.0f - t) * idle + t * full;
}

// engine torque -> force at the wheels
double Car_WheelForce(int type, float torque, int gear)                 // [FUN_507490]
{
    const CarType& c = g_carTypes[type];
    return (double)c.gearRatio[gear] * c.finalDrive * torque / c.wheelRadius;
}

// rolling resistance (load-dependent) plus air drag at a speed
double Car_Resistance(int type, float speed, float load)                // [FUN_5074d0]
{
    const CarType& c = g_carTypes[type];
    double rolling = Car_RollingFactor(speed, 0.1f) * load * 10.0f;
    double v = (double)speed * c.rollSpeedFactor * 0.033;
    if (!(v <= 1.0))
        v = 1.0f;
    return (double)speed * c.airDrag * speed + (v + 1.0f) * rolling;
}

// Raises the engine speed from half to three times torqueRpm in tenths of
// it while the wheel force still beats the resistance; returns the highest
// speed reached and in *rpm the engine speed where it stopped (0 below 0.6
// torqueRpm, or for a gear without a ratio).
float Car_TopSpeedInGear(int type, int gear, float load, float* rpm)    // [FUN_507550]
{
    if (gear < 0 || gear >= 15) {
        *rpm = 0;
        return 0;
    }
    const CarType& c = g_carTypes[type];
    if (x87_eq(c.gearRatio[gear], 0) || x87_eq(c.finalDrive, 0)) {
        *rpm = 0;
        return 0;
    }
    float peak = c.torqueRpm;
    float best = 0;
    float speedPerRpm = (float)((double)c.wheelRadius * TWO_PI
                                / x87_fabs((double)c.finalDrive * c.gearRatio[gear]));
    float w = (float)(peak * 0.5);
    float wMax = (float)(peak * 3.0f);
    if (!(w >= wMax)) {
        double next;
        do {
            float speed = (float)((double)w * speedPerRpm);
            float resistance = (float)Car_Resistance(type, speed, load);
            float torque = (float)Car_EngineTorque(w, peak, c.maxTorque, 1.0f);
            if (resistance > x87_fabs(Car_WheelForce(type, torque, gear)))
                break;
            if (speed > best)
                best = speed;
            next = peak * 0.1 + w;
            w = (float)next;
        } while (!(next >= wMax));
    }
    if ((double)peak * 0.6 > w) {
        *rpm = 0;
        return 0;
    }
    *rpm = w;
    return best;
}

// The forward gear with the highest top speed.
float Car_BestGear(int type, float load, int* gear, float* rpm)         // [FUN_507710]
{
    float best = 0, bestRpm = 0;
    int bestGear = 0;
    for (int g = 2; g <= g_carTypes[type].gearCount; g++) {
        float r;
        float v = Car_TopSpeedInGear(type, g, load, &r);
        if (v > best) {
            bestGear = g;
            best = v;
            bestRpm = r;
        }
    }
    *gear = bestGear;
    *rpm = bestRpm;
    return best;
}

// From the highest gear down: the first gear and the lowest throttle (in
// steps of 0.05) at which the wheel force beats the resistance at
// torqueRpm. Returns that speed; 0 and gear 0 if no gear makes it. *gear
// selects nothing on entry, but must be a valid gear index.
float Car_CruiseGear(int type, float load, int* gear, float* rpm, float* throttle) // [FUN_5077b0]
{
    int g = *gear;
    const CarType& c = g_carTypes[type];
    if (g < 0 || g >= 15 || x87_eq(c.gearRatio[g], 0) || x87_eq(c.finalDrive, 0)) {
        *rpm = 0;
        *throttle = 0;
        return 0;
    }
    float peak = c.torqueRpm;
    for (int n = c.gearCount; n > 2; n--) {
        float speed = (float)((double)c.wheelRadius * TWO_PI
                              / x87_fabs((double)c.gearRatio[n] * c.finalDrive) * peak);
        float resistance = (float)Car_Resistance(type, speed, load);
        float idle = (float)Car_EngineBrake(peak, c.engineBrake, 0, 0);
        for (float t = 0;;) {
            float torque = (float)Car_ThrottleTorque(type, c.maxTorque, idle, t);
            if (!(resistance >= x87_fabs(Car_WheelForce(type, torque, n)))) {
                *gear = n;
                *rpm = peak;
                *throttle = t;
                return speed;
            }
            double next = (double)t + 0.05;
            t = (float)next;
            if (next >= 1.0f)
                break;
        }
    }
    *throttle = 0;
    *gear = 0;
    *rpm = 0;
    return 0;
}

// Car_BestGear expressed through the type's m_12 and m_16. The gear is
// returned one lower (counted from the first forward gear).
double Car_TopSpeedRatio(int type, float load, int* gear, float* rpm, float* speed,
                         float* inverse)                                // [FUN_507980]
{
    double v = Car_BestGear(type, load, gear, rpm);
    *speed = (float)v;
    if (!(v > 0.0f))
        return 0.0f;
    const CarType& c = g_carTypes[type];
    double q = c.m_12 / (v * c.m_16 * 0.001f / (*rpm / (double)c.torqueRpm));
    (*gear)--;
    *inverse = (float)(c.m_12 / q);
    return q;
}

// The same for Car_CruiseGear, scaled by the throttle.
double Car_CruiseRatio(int type, float load, int* gear, float* rpm, float* speed, float* inverse,
                       float* throttle)                                 // [FUN_507a10]
{
    double v = Car_CruiseGear(type, load, gear, rpm, throttle);
    *speed = (float)v;
    if (!(v > 0.0f))
        return 0.0f;
    const CarType& c = g_carTypes[type];
    double scaled = (double)*throttle * c.m_12;
    double q = scaled / (v * c.m_16 * 0.001f / (*rpm / (double)c.torqueRpm));
    *inverse = (float)(c.m_12 / q);
    (*gear)--;
    return q;
}
