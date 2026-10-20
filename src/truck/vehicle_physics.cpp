// vehicle_physics.cpp - small functions of the unit at 0x4eb0c7 of king.masm (the
// vehicle's controls and physics): ramps of a control between limits, the
// wheel and contact points of the body, the engine's state, steering
// geometry, and copies of the body's vectors.
//
// Building with  nmake ASM_VEHICLE_PHYSICS=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void FUN_4eb350(); KING_ALIAS("_FUN_4eb350", "@Trigger_Check@12")
extern "C" void __fastcall Body_SetLoad(BYTE* self, void*, float load);
extern "C" void __fastcall Body_UpdateHitch(BYTE* self, void*, int keep);
ASM_PROC(FUN_4f2670)
ASM_PROC(FUN_4f8260)

struct Vec3Bits { DWORD x, y, z; };

// fptan as the original uses it (the 1 it pushes is dropped)
X87 double x87_tan(double x) { __asm { fld x } __asm { fptan } __asm { fstp st(0) } }

extern "C" int __fastcall Obj4eb150_Kind(BYTE* self, void*)
{
    return F(int, self, 16);
}

extern "C" int __fastcall Obj4eb170_Get8(BYTE* self, void*)
{
    return F(int, self, 8) ? F(int, self, 8) : 0;
}

// point i (0..3) of those at +60, the zero vector for others
extern "C" Vec3Bits* __fastcall Body_Point(BYTE* self, void*, Vec3Bits* out, int i)
{
    if (i < 0 || i >= 4)
        *out = *(const Vec3Bits*)&g_zeroVec;
    else
        *out = F(Vec3Bits, self, 60 + i * 12);
    return out;
}

extern "C" BYTE* __fastcall List4eb3b0_Construct(BYTE* self, void*)
{
    F(int, self, 4) = 0;
    F(int, self, 0) = 0;
    return self;
}

// steps every item of the list (linked through +4 of the items) to now
extern "C" void __fastcall Items_StepAll(BYTE** self, void*)
{
    float now = (float)Game_Time();
    DWORD n = (DWORD)*self;
    if (!n || !(n - 4))
        return;
    for (BYTE* item = (BYTE*)(n - 4);;) {
        CallThis(FUN_4eb350, item, now);
        n = F(DWORD, item, 8);
        if (!n || !(n - 4))
            return;
        item = (BYTE*)(n - 4);
    }
}

// ---------------------------------------------------------------------------
// A ramp: three ranges (+0 the low ends, +12 the high ends, +24 the rates),
// +36 the position (0..1) in the range +40; +44 the ranges set

extern "C" BYTE* __fastcall Ramp_Construct(BYTE* self, void*)
{
    F(int, self, 40) = 0;
    for (int i = 0; i < 3; i++) {
        F(int, self, i * 4) = 0;
        F(float, self, 12 + i * 4) = 1.0f;
        F(float, self, 24 + i * 4) = 1.0f;
    }
    F(int, self, 48) = 0;
    F(int, self, 44) = 0;
    F(float, self, 36) = 1.0f;
    return self;
}

extern "C" void __fastcall Ramp_SetRange(BYTE* self, void*, int i, float lo, float hi, float time)
{
    if (i < 0 || i >= 3)
        return;
    int n = F(int, self, 44);
    if (i > n)
        return;
    if (i == n)
        F(int, self, 44) = n + 1;
    F(float, self, i * 4) = lo;
    F(float, self, 12 + i * 4) = hi;
    if (x87_eq(time, 0.0f))
        F(int, self, 24 + i * 4) = 0;
    else
        F(float, self, 24 + i * 4) = (float)(1.0f / (double)time);
}

extern "C" double __fastcall Ramp_Value(BYTE* self, void*)
{
    int i = F(int, self, 40);
    return ((double)F(float, self, 12 + i * 4) - F(float, self, i * 4)) * F(float, self, 36) + F(float, self, i * 4);
}

// the position of value v clamped into the range
extern "C" void __fastcall Ramp_SetValue(BYTE* self, void*, float v)
{
    int i = F(int, self, 40);
    float lo = F(float, self, i * 4);
    float hi = F(float, self, 12 + i * 4);
    double x = v;
    if (!(lo >= hi)) {                  // rising range
        if (!(x >= lo))
            x = lo;
        if (x > hi)
            x = hi;
    } else {
        if (x > lo)
            x = lo;
        if (!(x >= hi))
            x = hi;
    }
    double d = (double)hi - lo;
    if (x87_eq(d, 0.0f))
        F(float, self, 36) = 1.0f;
    else
        F(float, self, 36) = (float)((x - lo) / d);
}

extern "C" float __fastcall Ramp_High(BYTE* self, void*)
{
    return F(float, self, 12 + F(int, self, 40) * 4);
}

extern "C" float __fastcall Ramp_Position(BYTE* self, void*)
{
    return F(float, self, 36);
}

extern "C" void __fastcall Ramp_SetPosition(BYTE* self, void*, float p)
{
    if (!(p >= 0.0f))
        F(int, self, 36) = 0;
    else if (p > 1.0f)
        F(float, self, 36) = 1.0f;
    else
        F(float, self, 36) = p;
}

static void ClampPosition(BYTE* self, double x)
{
    F(float, self, 36) = (float)x;
    if (x > 1.0f)
        F(float, self, 36) = 1.0f;
    if (!(F(float, self, 36) >= 0.0f))
        F(int, self, 36) = 0;
}

extern "C" void __fastcall Ramp_Down(BYTE* self, void*, float dt)
{
    ClampPosition(self, F(float, self, 36) - (double)dt * F(float, self, 24 + F(int, self, 40) * 4));
}

extern "C" void __fastcall Ramp_FromTop(BYTE* self, void*, float dt)
{
    ClampPosition(self, 1.0f - (double)dt * F(float, self, 24 + F(int, self, 40) * 4));
}

// ---------------------------------------------------------------------------
// The engine's state (+9984 of the vehicle's +21600)

extern "C" void __fastcall Engine_Reset(BYTE* self, void*)
{
    F(int, self, 12) = 0;
    F(float, self, 16) = 1.0f;
    F(short, self, 24) = 0;
    F(short, self, 26) = 0;
    F(short, self, 20) = 0;
    F(short, self, 22) = 0;
    F(int, self, 32) = 0;
    F(int, self, 28) = 0;
    F(short, self, 72) = 0;
    F(short, self, 70) = 0;
    F(float, self, 44) = 1.0f;
    double t = Game_Time();
    F(double, self, 56) = t;
    F(double, self, 48) = t;
    self[3] = 0;
    self[2] = 0;
    self[1] = 0;
    F(short, self, 68) = 0;
    for (int i = 0; i < 5; i++)
        F(Vec3, self, 260 + i * 12) = g_zeroVec;
    F(int, self, 192) = 0;
    F(int, self, 196) = 0;
    F(int, self, 124) = 0;
}

extern "C" void __fastcall Engine_SetKeys(BYTE* self, void*, int a, int b)
{
    F(short, self, 4) = a ? 1 : 0;
    F(short, self, 6) = b ? 1 : 0;
}

// up to 5 contacts: point (+260), normal (+320), force (+200); the index
extern "C" int __fastcall Engine_AddContact(BYTE* self, void*, const Vec3Bits* force, const Vec3Bits* point,
                                            const Vec3Bits* normal)
{
    int n = F(int, self, 192);
    if (n >= 5)
        return 0;
    F(Vec3Bits, self, 260 + n * 12) = *point;
    F(Vec3Bits, self, 320 + F(int, self, 192) * 12) = *normal;
    F(Vec3Bits, self, 200 + F(int, self, 192) * 12) = *force;
    return F(int, self, 192)++;
}

extern "C" int __fastcall Engine_AddPush(BYTE* self, void*, const Vec3Bits* a, const Vec3Bits* b)
{
    int n = F(int, self, 196);
    if (n >= 5)
        return 0;
    F(Vec3Bits, self, 380 + n * 12) = *a;
    F(Vec3Bits, self, 440 + F(int, self, 196) * 12) = *b;
    return F(int, self, 196)++;
}

extern "C" void __fastcall Engine_4eead0(BYTE* self, void*, int a)
{
    F(int, self, 10104) = 1;
    Body_SetLoad(self, 0, *(float*)&a);
}

extern "C" void __fastcall Engine_4eec20(BYTE* self, void*)
{
    int mode = F(int, self, 10104);
    F(int, self, 10120) = 0;
    if (mode == 1)
        Body_SetLoad(self, 0, 0.0f);
    else if (mode == 2 || mode == 4) {
        if (mode == 2)
            Body_UpdateHitch(self, 0, 1);
        F(short, self, 10006) = 1;
        F(float, self, 10000) = 1.0f;
    }
    F(int, self, 10104) = 0;
    F(Matrix, self, 10128) = g_identity;
}

// ---------------------------------------------------------------------------
// Suspension ratios from the wheel positions (+4, +28, +52)

extern "C" void __cdecl Wheels_Ratios2(const BYTE* w, float* out)
{
    double a = F(float, w, 4), b = F(float, w, 28);
    double x = b / (b - a) * 0.5;
    out[0] = (float)x;
    out[1] = (float)x;
    out[2] = (float)(a / (a - b));
}

extern "C" void __cdecl Wheels_Ratios4(const BYTE* w, float* out)
{
    double a = F(float, w, 4), b = F(float, w, 28);
    double x = b / (b - a) * 0.5;
    out[0] = (float)x;
    out[1] = (float)x;
    double y = a / (a - b) * 0.5;
    out[3] = (float)y;
    out[2] = (float)y;
}

extern "C" void __cdecl Wheels_Ratios5(const BYTE* w, float* out)
{
    double c = F(float, w, 52);
    float s = (float)(F(float, w, 28) + (double)F(float, w, 4));
    double k = 1.0 / (s - (c + c));
    float kf = (float)k;
    double v = -(k * c) * 0.5;
    out[0] = (float)v;
    out[1] = (float)v;
    out[2] = (float)v;
    out[3] = (float)v;
    out[4] = (float)((double)s * kf);
}

extern "C" void __cdecl Wheels_Ratios6(const BYTE* w, float* out)
{
    double a = F(float, w, 4);
    float s = (float)(F(float, w, 52) + (double)F(float, w, 28));
    double k = 1.0 / (s - (a + a));
    float kf = (float)k;
    double m = -(k * a);
    double h = (double)s * kf * 0.5;
    out[0] = (float)h;
    out[1] = (float)h;
    double v = m * 0.5;
    out[2] = (float)v;
    out[3] = (float)v;
    out[4] = (float)v;
    out[5] = (float)v;
}

// whether the body leans over: its up axis (+24, +36, +48 rows) against
// the world's z, or the roll at +36
extern "C" int __fastcall Body_IsTipped(BYTE* self, void*)
{
    const float* z = &g_axisZ.x;
    double up = ((double)z[2] * F(float, self, 48) + (double)z[1] * F(float, self, 36)) + (double)z[0] * F(float, self, 24);
    if (!(up >= 0.17))
        return 1;
    if (!(x87_fabs(F(float, self, 36)) > 0.94))
        return 0;
    return 1;
}

extern "C" int __fastcall Body_CheckUpright(BYTE* self, void*)
{
    if (F(int, self, 13124) && Body_IsTipped(self, 0))
        return 0;
    F(int, self, 13124) = 0;
    F(short, self, 10056) = 1;
    return 1;
}

// the steering geometry of the wheel angle at +6688
extern "C" void __fastcall Steering_Update(BYTE* self, void*)
{
    double angle = F(float, self, 6688);
    double inner = x87_atan2(x87_tan(angle) * ((double)F(float, self, 8056) / F(float, self, 8032)), 1.0);
    F(float, self, 6692) = (float)inner;
    F(float, self, 6736) = (float)x87_sin(F(float, self, 6688));
    F(float, self, 6740) = (float)x87_sin(inner);
    F(float, self, 6744) = (float)x87_sin(F(float, self, 6688) - inner);
}

extern "C" double __fastcall Body_Mean1a30(BYTE* self, void*)
{
    return ((double)F(float, self, 6708) + F(float, self, 6704)) * 0.5f;
}

extern "C" double __fastcall Body_Mean1a38(BYTE* self, void*)
{
    return ((double)F(float, self, 6716) + F(float, self, 6712)) * 0.5f;
}

// keeps the state of this step
extern "C" void __fastcall Body_SaveState(BYTE* self, void*)
{
    F(Vec3Bits, self, 10832) = F(Vec3Bits, self, 64);
    F(Vec3Bits, self, 10844) = F(Vec3Bits, self, 2544);
    F(Vec3Bits, self, 10856) = F(Vec3Bits, self, 2592);
    F(int, self, 13100) = F(int, self, 3524);
    F(int, self, 13096) = F(int, self, 3520);
    F(int, self, 13104) = F(int, self, 3528);
    F(int, self, 13112) = F(int, self, 3808);
    F(int, self, 13116) = F(int, self, 3812);
    for (int i = 0; i < F(int, self, 3540); i++)
        F(int, self, 15280 + i * 4) = F(int, self, 9840 + i * 4);
}

extern "C" void __fastcall Body_SaveVec88(BYTE* self, void*)
{
    F(Vec3Bits, self, 10868) = F(Vec3Bits, self, 88);
}

extern "C" void __fastcall Body_4f2630(BYTE* self, void*, int a)
{
    CallThis(FUN_4f2670, self, a, self + 16, self + 2304, self + 2544, self + 76, F(int, self, 3528), self + 2616);
}

// the smallest load ratio of the wheels on the ground, 1 in mode 1
extern "C" float __fastcall Wheels_MinGrip(BYTE* self, void*)
{
    if (F(int, self, 9784) == 1)
        return 1.0f;
    float best = 1.0f;
    for (int i = 0; i < F(int, self, 3540); i++) {
        if (F(short, self, 6564 + i * 2))
            continue;
        float load = (float)x87_fabs((double)F(float, self, 3360 + i * 4) * F(float, self, 9840 + i * 4) * F(float, self, 6780 + i * 4));
        double r = 1.0f;
        if (F(float, self, 7320 + i * 4) > load)
            r = load / (double)F(float, self, 7320 + i * 4);
        if (best > r)
            best = (float)r;
    }
    return best;
}

extern "C" void __fastcall Body_SetVec9012(BYTE* self, void*, int a, int b, int c)
{
    F(int, self, 9012) = a;
    F(int, self, 9016) = b;
    F(int, self, 9020) = c;
}

extern "C" Vec3Bits* __fastcall Body_GetVec9012(BYTE* self, void*, Vec3Bits* out) { *out = F(Vec3Bits, self, 9012); return out; }
extern "C" Vec3Bits* __fastcall Body_GetVec9048(BYTE* self, void*, Vec3Bits* out) { *out = F(Vec3Bits, self, 9048); return out; }
extern "C" Vec3Bits* __fastcall Body_GetVec9024(BYTE* self, void*, Vec3Bits* out) { *out = F(Vec3Bits, self, 9024); return out; }
extern "C" Vec3Bits* __fastcall Body_GetVec9084(BYTE* self, void*, Vec3Bits* out) { *out = F(Vec3Bits, self, 9084); return out; }
extern "C" Vec3Bits* __fastcall Body_GetVec9144(BYTE* self, void*, Vec3Bits* out) { *out = F(Vec3Bits, self, 9144); return out; }

extern "C" Matrix* __fastcall Body_Matrix22a4(BYTE* self, void*, Matrix* out)
{
    Matrix_Multiply(out, (const Matrix*)(self + 8868), (const Matrix*)(self + 8772));
    return out;
}

extern "C" Matrix* __fastcall Body_Matrix2274(BYTE* self, void*, Matrix* out)
{
    Matrix_Multiply(out, (const Matrix*)(self + 8820), (const Matrix*)(self + 8964));
    return out;
}

extern "C" float __fastcall Body_Get9508(BYTE* self, void*)
{
    return F(float, self, 9508);
}

extern "C" int __fastcall Body_4fc8b0(BYTE* self, void*)
{
    if (F(int, self, 9784) == 2)
        return CallThis<int>(FUN_4f8260, self, 0);
    if (F(int, self, 9784) == 3)
        return CallThis<int>(FUN_4f8260, self, 1);
    return 0;
}

extern "C" void __fastcall Body_ToggleMode(BYTE* self, void*)
{
    F(int, self, 9784) = F(int, self, 9784) == 1 ? 3 : 1;
}

extern "C" void __fastcall Body_ClearMode(BYTE* self, void*)
{
    F(int, self, 9784) = 0;
}

extern "C" void __fastcall Body_SetMode(BYTE* self, void*, int mode)
{
    F(int, self, 9784) = mode;
}

// ---------------------------------------------------------------------------
// medium-sized functions of the unit

ASM_PROC(FUN_4f0800)
ASM_PROC(FUN_4f0530)
ASM_PROC(FUN_4f09f0)

static inline void Copy3i(BYTE* to, const BYTE* from)
{
    F(int, to, 0) = F(int, from, 0);
    F(int, to, 4) = F(int, from, 4);
    F(int, to, 8) = F(int, from, 8);
}

// keeps the body's state and that of its wheels for the next step
extern "C" void __fastcall Body_SaveAll(BYTE* self, void*)
{
    memcpy(self + 2256, self + 16, 48);
    memcpy(self + 2352, self + 2304, 48);
    F(int, self, 6672) = F(int, self, 244);
    Copy3i(self + 2568, self + 64);
    Copy3i(self + 3132, self + 76);
    for (int i = 0; i < F(int, self, 3540); i++) {
        memcpy(self + 4120 + i * 248, self + 248 + i * 248, 248);
        Copy3i(self + 6116 + i * 12, self + 100 + i * 12);
        F(int, self, 3576 + i * 4) = F(int, self, 3544 + i * 4);
    }
}

// the wheels' contacts after a step: all on the ground, any on it
extern "C" void __fastcall Body_UpdateContacts(BYTE* self, void*)
{
    CallThis(FUN_4f0800, self);
    CallThis(FUN_4f0530, self);
    F(BYTE, self, 6580) = 1;
    F(BYTE, self, 6586) = 0;
    for (int i = 0; i < F(int, self, 3540); i++) {
        short* on = &F(short, self, 6564 + i * 2);
        if (!F(short, self, 272 + i * 248)) {
            F(int, self, 3544 + i * 4) = F(int, self, 8092 + i * 4);
            *on = 1;
        }
        F(BYTE, self, 6580) = F(BYTE, self, 6580) && *on;
        F(BYTE, self, 6586) = F(BYTE, self, 6586) || *on;
    }
    if (F(int, self, 3540) >= 4)
        F(BYTE, self, 6583) = F(short, self, 6568) && F(short, self, 6570);
    F(BYTE, self, 6581) = F(short, self, 6564) && F(short, self, 6566);
}

// the flags of the wheels' grip by axle and side
extern "C" void __fastcall Body_UpdateGrip(BYTE* self, void*)
{
    int n = F(int, self, 3540);
    F(char, self, 6587) = 0;
    F(char, self, 6588) = 1;
    for (int i = 0; i < n; i++) {
        F(char, self, 6588) = F(char, self, 6588) && F(char, self, 6596 + i);
        F(char, self, 6587) = F(char, self, 6587) || F(char, self, 6596 + i);
    }
    if (n >= 4) {
        F(char, self, 6592) = F(char, self, 6596) && F(char, self, 6598);
        F(char, self, 6593) = F(char, self, 6597) && F(char, self, 6599);
    }
    F(char, self, 6590) = F(char, self, 6596) && F(char, self, 6597);
    if (n >= 4)
        F(char, self, 6591) = F(char, self, 6598) && F(char, self, 6599);
    F(char, self, 6589) = F(char, self, 6588);
    char front = (F(char, self, 6596) || F(short, self, 6564)) && (F(char, self, 6597) || F(short, self, 6566));
    F(char, self, 6594) = front;
    if (n >= 4)
        F(char, self, 6595) = (F(char, self, 6598) || F(short, self, 6568))
                           && (F(char, self, 6599) || F(short, self, 6570));
    if (front)
        F(int, self, 6644) = 0;
    if (F(char, self, 6595))
        F(int, self, 6648) = 0;
}

ASM_VAR(double, g_64b5a0, $L_64b5a0)    // 0.01

// a step of the body: the inputs, the forces and the outputs of the wheels
extern "C" void __fastcall Body_Step(BYTE* self, void*, short* in)
{
    F(int, self, 13488) = F(int, self, 9792);
    Copy3i(self + 13496, self + 8008);
    F(int, self, 13484) = F(short, self, 10048);
    CallThis(FUN_4f09f0, self, in);
    Body_SaveState(self, 0);
    Body_SaveVec88(self, 0);
    F(int, self, 13160) = F(int, self, 6688);
    Copy3i(self + 11072, self + 208);
    Game_FrameTime();
    for (int i = 0; i < F(int, self, 3540); i++) {
        F(int, self, 13164 + i * 4) = F(int, self, 3740 + i * 4);
        F(int, self, 13196 + i * 4) = F(int, self, 6704 + i * 4);
        memcpy(self + 11088 + i * 248, self + 248 + i * 248, 248);
        F(int, self, 13128 + i * 4) = F(int, self, 3544 + i * 4);
        F(int, self, 13380 + i * 4) = F(short, self, 6564 + i * 2);
        F(int, self, 13420 + i * 4) = F(signed char, self, 6596 + i);
        F(int, self, 13196 + i * 4) = F(int, self, 6704 + i * 4);
        F(int, self, 13452 + i * 4) = F(int, self, 6612 + i * 4);
    }
    F(int, self, 13416) = F(signed char, self, 6586);
    F(int, self, 13412) = F(signed char, self, 6580);
    F(int, self, 13508) = F(float, self, 10716) > g_64b5a0;
    F(int, self, 13512) = F(int, self, 7196) == 0;
}

ASM_PROC(FUN_4f0fc0)
ASM_VAR(float, g_quarterPi674408, $L_674408)
ASM_VAR(double, g_64b6e0, $L_64b6e0)            // 0.5
ASM_VAR(double, g_64b840, $L_64b840)            // 0.2

#define FL(off) F(float, self, off)

// the steering angle: of the wheel's position, or of the held keys
extern "C" double __fastcall Body_SteerAngle(BYTE* self, void*)
{
    int s = F(int, self, 15560);
    if (!s)
        return (double)g_quarterPi674408 * FL(10720);
    int a = s < 0 ? -s : s;
    if (a >= F(int, self, 15564) - 1)
        return s > 0 ? g_quarterPi674408 * g_64b6e0 : -g_quarterPi674408 * g_64b6e0;
    double add = 0.0;
    if (a >= 1)
        add = (s > 0 ? (double)g_quarterPi674408 : -(double)g_quarterPi674408) * g_64b840;
    double v = add + (double)g_quarterPi674408 * FL(10720);
    if (v > g_quarterPi674408)
        v = g_quarterPi674408;
    float lo = -g_quarterPi674408;
    if (!(v >= lo))
        return lo;
    return v;
}

// the velocity: in the world (+76), in the body (+88)
extern "C" void __fastcall Body_SetVelocity(BYTE* self, void*, const float* v)
{
    FL(76) = v[0];
    FL(80) = v[1];
    FL(84) = v[2];
    Vec3 t, r;
    double ax = FL(76), ay = FL(80), az = FL(84);
    t.x = (float)(FL(2328) * az + FL(2316) * ay + ax * FL(2304));
    t.y = (float)(FL(2332) * az + FL(2308) * ax + FL(2320) * ay);
    t.z = (float)(FL(2336) * az + FL(2312) * ax + FL(2324) * ay);
    Vec3* p = Vec3_DivComponents(&r, &t, (const Vec3*)(self + 8008));
    F(Vec3, self, 88) = *p;
    Body_SaveVec88(self, 0);
}

// the angular velocity in the world (+9024), in the body (+9036, +9048 scaled)
extern "C" void __fastcall Body_SetSpin(BYTE* self, void*, float x, float y, float z)
{
    FL(9024) = x;
    FL(9028) = y;
    FL(9032) = z;
    double ax = FL(9024), ay = FL(9028), az = FL(9032);
    float tx = (float)(FL(8988) * az + FL(8976) * ay + ax * FL(8964));
    float ty = (float)(FL(8992) * az + FL(8968) * ax + FL(8980) * ay);
    float tz = (float)(FL(8996) * az + FL(8972) * ax + FL(8984) * ay);
    FL(9036) = tx;
    FL(9040) = ty;
    FL(9044) = tz;
    Vec3 r;
    F(Vec3, self, 9048) = *Vec3_DivComponents(&r, (const Vec3*)(self + 9036), (const Vec3*)(self + 8008));
}

extern "C" void __fastcall Body_SetSpinScaled(BYTE* self, void*, float x, float y, float z)
{
    FL(9048) = x;
    FL(9052) = y;
    FL(9056) = z;
    Vec3 r;
    F(Vec3, self, 9036) = *Vec3_MulComponents(&r, (const Vec3*)(self + 9048), (const Vec3*)(self + 8008));
    double bx = FL(9036), by = FL(9040), bz = FL(9044);
    float tx = (float)(FL(8880) * by + FL(8892) * bz + bx * FL(8868));
    float ty = (float)(FL(8884) * by + FL(8872) * bx + FL(8896) * bz);
    float tz = (float)(FL(8888) * by + FL(8876) * bx + FL(8900) * bz);
    FL(9024) = tx;
    FL(9028) = ty;
    FL(9032) = tz;
}

// the body's orientation from a matrix, and the spin in the body again
extern "C" void __fastcall Body_SetOrientation(BYTE* self, void*, int a, const Matrix* m)
{
    if (F(int, self, 9524) > F(int, self, 9520))
        CallThis(FUN_4f0fc0, self, a, m);
    Matrix t1, t2;
    F(Matrix, self, 8868) = *Matrix_Multiply(&t1, m, (const Matrix*)(self + 8820));
    F(Matrix, self, 8964) = *Matrix_Inverse(&t2, (const Matrix*)(self + 16));
    double ax = FL(9024), ay = FL(9028), az = FL(9032);
    float tx = (float)(FL(8976) * ay + FL(8988) * az + ax * FL(8964));
    float ty = (float)(FL(8980) * ay + FL(8968) * ax + FL(8992) * az);
    float tz = (float)(FL(8984) * ay + FL(8972) * ax + FL(8996) * az);
    FL(9036) = tx;
    FL(9040) = ty;
    FL(9044) = tz;
    Vec3 r;
    F(Vec3, self, 9048) = *Vec3_DivComponents(&r, (const Vec3*)(self + 9036), (const Vec3*)(self + 8008));
}
#undef FL

ASM_PROC(FUN_4ef480)
typedef BYTE Bytes697f9c[1];
ASM_VAR(Bytes697f9c, g_697f9c, $L_697f9c)     // 2416-byte records by vehicle type
ASM_VAR(double, g_64c958, $L_64c958)            // 0.9
ASM_VAR(float, g_674414, $L_674414)             // 0.75
ASM_VAR(float, g_9_8, $L_64d990)                // 9.8
ASM_VAR(double, g_64b668, $L_64b668)            // 1.5

#define FL(off) F(float, self, off)

// the trailer's hitch: the point of the vehicle type in the world
extern "C" void __fastcall Body_UpdateHitch(BYTE* self, void*, int keep)
{
    if (F(int, self, 10104) != 2)
        return;
    int n = F(int, self, 3540);
    if (keep) {
        F(int, self, 7844 + n * 12) = F(int, self, 7364);
        F(int, self, 7352 + F(int, self, 3540) * 12) = F(int, self, 7364);
        return;
    }
    const float* p = (const float*)(g_697f9c + F(int, self, 2252) * 2416 + 168);
    double p0 = p[0], p1 = p[1], p2 = p[2];
    float x = (float)(FL(10152) * p2 + FL(10140) * p1 + p0 * FL(10128) + FL(10164));
    float y = (float)(FL(10156) * p2 + FL(10132) * p0 + FL(10144) * p1 + FL(10168));
    float z = (float)(FL(10160) * p2 + FL(10136) * p0 + FL(10148) * p1 + FL(10172));
    BYTE* d = self + (n + 653) * 12;
    F(float, d, 0) = x;
    F(float, d, 4) = y;
    F(float, d, 8) = z;
    FL(7844 + F(int, self, 3540) * 12) = (float)(FL(7364) + g_64c958);
    n = F(int, self, 3540);
    Copy3i(self + 7344 + n * 12, self + 7836 + n * 12);
}

// the weight on the wheels (load: of the trailer, on the rear wheels)
extern "C" void __fastcall Body_SetLoad(BYTE* self, void*, float load)
{
    CallThis(FUN_4ef480, self);
    FL(10120) = load;
    float m = FL(9792);
    float gy = -g_axisZ.y, gz = -g_axisZ.z;
    double wx = -(double)g_axisZ.x * m;
    float wy = gy * m, wz = gz * m;
    FL(3156) = (float)(wx * g_9_8);
    FL(3160) = (float)(wy * (double)g_9_8);
    FL(3164) = (float)(wz * (double)g_9_8);
    int n = F(int, self, 3540);
    float w[16];
    for (int i = 0; i < n; i++) {
        double t = (double)FL(9808 + i * 4) * FL(3164);
        if (t < 0)
            t = -t;
        w[i] = (float)t;
        if (i > 1)
            w[i] = (float)(FL(10120) / (double)(n - 2) + t);
        double d = FL(8060 + i * 4) - g_674414 * (double)FL(8092 + i * 4);
        FL(8124 + i * 4) = (float)(w[i] / ((d / FL(8060 + i * 4) + 1.0f) * d));
        FL(7484 + i * 4) = w[i];
        FL(7320 + i * 4) = (float)(w[i] * g_64b668);
        n = F(int, self, 3540);
    }
}

// the shares of the points q[] in a force at p: by the lengths of p x q
extern "C" void __cdecl Weights_ByMoments(const float* p, const float* q, float* out, int n)
{
    float len[8], w[8];
    for (int i = 0; i < n; i++, q += 3) {
        Vec3 c;
        c.x = (float)((double)p[1] * q[2] - (double)p[2] * q[1]);
        c.y = (float)((double)p[2] * q[0] - (double)p[0] * q[2]);
        c.z = (float)((double)p[0] * q[1] - (double)p[1] * q[0]);
        len[i] = (float)Vec3_Length(&c);
    }
    double s = 0.0;
    for (int i = 0; i < n; i++) {
        w[i] = 1.0f;
        for (int j = 0; j < n; j++)
            w[i] = (float)((j == i ? 1.0 : (double)len[j]) * w[i]);
        s += w[i];
    }
    float inv = (float)(1.0 / s);
    for (int i = 0; i < n; i++)
        out[i] = (float)((double)inv * w[i]);
}

extern "C" void __fastcall Body_ResetMotion(BYTE* self, void*)
{
    F(Vec3, self, 64) = g_zeroVec;
    F(Vec3, self, 2568) = g_zeroVec;
    F(Vec3, self, 220) = g_zeroVec;
    F(Vec3, self, 2544) = g_zeroVec;
    F(int, self, 3520) = 0;
    F(int, self, 3524) = 0;
    F(Vec3, self, 3216) = g_zeroVec;
    F(Vec3, self, 3180) = g_zeroVec;
    F(Vec3, self, 3228) = g_zeroVec;
    Copy3i(self + 2556, self + 2616);
    double d = (double)FL(2552) * FL(2624) + (double)FL(2548) * FL(2620) + (double)FL(2544) * FL(2616);
    float x = (float)(d * FL(2616)), y = (float)(d * FL(2620));
    F(int, self, 3528) = 0;
    FL(2592) = x;
    FL(2596) = y;
    FL(2600) = (float)(d * FL(2624));
    FL(3452) = 10.0f;
    Copy3i(self + 3416, self + 3392);
}
#undef FL

// ---------------------------------------------------------------------------
typedef char AsmLabel[1];
extern "C" void FUN_4ee980(); KING_ALIAS("_FUN_4ee980", "@Body_UpdateHitch@12")
ASM_VAR(AsmLabel, g_carTypeNames, $L_69809c)
ASM_VAR(AsmLabel, s_67456c, $L_67456c)
extern "C" void __cdecl World_Fatal(const char* msg);

// [FUN_4ee900..$L_4ee965] counts one more use of the car type's (2416-byte record) counter at +44 into +3540/+13120 (fatal from 8 on), state 2
extern "C" void __fastcall Car_CountType(BYTE* self, void*)
{
    BYTE* type = (BYTE*)g_carTypeNames + F(int, self, 2252) * 2416;
    F(int, self, 10104) = 2;
    int n = F(int, type, 44) + 1;
    F(int, self, 3540) = n;
    F(int, self, 13120) = n;
    if (n >= 8)
    {
        char text[300];
        wsprintfA(text, s_67456c, F(int, type, 44), 8);
        World_Fatal(text);
    }
    CallThis(FUN_4ee980, self, 0);
}


// ---------------------------------------------------------------------------
ASM_PROC(FUN_4ebbe0)
ASM_PROC(FUN_4eb2f0)
ASM_VAR(AsmLabel, g_697608, $L_697608)
ASM_VAR(AsmLabel, g_6975b8, $L_6975b8)
ASM_VAR(int, g_697658, $L_697658)
ASM_VAR(int, g_69765c, $L_69765c)
ASM_VAR(int, g_697660, $L_697660)

// [FUN_4eb350..$L_4eb399] at or past +20: once, asks the car (+36) for 4ebbe0(+28); fails mark +32
extern "C" void __fastcall Trigger_Check(BYTE* self, void*, float x)
{
    if (!(x >= F(float, self, 20)) || F(int, self, 32))
        return;
    if (F(BYTE*, self, 36))
    {
        int r = CallThis<int>(FUN_4ebbe0, F(BYTE*, self, 36), F(int, self, 28));
        if (r)
        {
            CallThis(FUN_4eb2f0, self);
            if (r == 1)
                F(int, F(BYTE*, self, 36), 15496) = r;
            return;
        }
    }
    F(int, self, 32) = 1;
}

// [FUN_4eb190] +128
extern "C" int __fastcall Obj_Get128(BYTE* self, void*)
{
    return F(int, self, 128);
}

// [FUN_4eb210] flag 1 of +20
extern "C" int __fastcall Obj_Flag20(BYTE* self, void*)
{
    return (F(DWORD, self, 20) & 1) != 0;
}

// [FUN_4eb230] clears the two 20-dword tables and three counters
extern "C" void __cdecl Tables_Clear697608()
{
    memset(g_697608, 0, 80);
    memset(g_6975b8, 0, 80);
    g_697658 = g_69765c = g_697660 = 0;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_4e2821(); KING_ALIAS("_FUN_4e2821", "_World_CheckPoint")    // checks a vector (v, name, limit)
ASM_PROC(FUN_4f0530)
extern "C" void FUN_4f0720(); KING_ALIAS("_FUN_4f0720", "@Body_SaveState@8")
ASM_PROC(FUN_4fef20)
ASM_VAR(AsmLabel, s_vp6746e4, $L_6746e4)

// [FUN_4f1dd0] sets the position (+64) after checking it, then updates the body
extern "C" void __fastcall Body_SetPosition(BYTE* self, void*, Vec3* p)
{
    CallC(FUN_4e2821, p, (char*)s_vp6746e4, 100.0f);
    F(Vec3, self, 64) = *p;
    CallThis(FUN_4f0530, self);
    CallThis(FUN_4f0720, self);
}

// [FUN_4feee0] copies the pending mode (+10112) to +15500 and clears it; without one runs 4fef20 until it returns 0
extern "C" void __fastcall Body_TakeMode(BYTE* self, void*)
{
    int m = F(int, self, 10112);
    F(int, self, 15500) = m;
    if (m)
    {
        F(int, self, 10112) = 0;
        return;
    }
    while (CallThis<int>(FUN_4fef20, self))
        ;
}


// ---------------------------------------------------------------------------
ASM_PROC(FUN_4f1240)
ASM_PROC(FUN_4f1ef0)

// [FUN_4fc9b0] switches the control mode (+9784) to 1..3 with (a, b) for 4f1240, a position and 4f1ef0's value; mode 0 just clears it
extern "C" void __fastcall Body_SetControlMode(BYTE* self, void*, int mode, int a, int b, Vec3* pos, int v)
{
    if (!mode)
    {
        F(int, self, 9784) = 0;
        return;
    }
    if (mode < 0 || mode > 3 || F(int, self, 9784) == mode)
        return;
    F(int, self, 9784) = mode;
    CallThis(FUN_4f1240, self, 2, a, b);
    Body_SetPosition(self, 0, pos);
    CallThis(FUN_4f1ef0, self, v);
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

ASM_PROC(FUN_530190)

static BYTE* Vehphy_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_4eb2d0]
extern "C" BYTE* __fastcall Vehphy4eb2d0_DeletingDestructor(BYTE* s, void*, int flags) { return Vehphy_DeleteAfter(s, flags, FUN_530190); }

ASM_PROC(FUN_4eb260)

// [FUN_4eb3c0] a new 40-byte part (FUN_4eb260 (self, a, +4, b))
extern "C" int __fastcall Phys4eb3c0_NewPart(BYTE* s, void*, int a, int b)
{
    BYTE* o = (BYTE*)operator new(40);
    if (!o)
        return 0;
    return CallThis<int>(FUN_4eb260, o, s, a, F(int, s, 4), b);
}

// ---------------------------------------------------------------------------
// Body mode.

ASM_PROC(FUN_4f1240)
ASM_PROC(FUN_4f1ef0)
extern "C" void FUN_4f1dd0(); KING_ALIAS("_FUN_4f1dd0", "@Body_SetPosition@12")
// [FUN_4fc930] sets the body mode (+9784): 0, 1, or 2/3 with the frame (a, b), the position c and d
extern "C" void __fastcall Body4fc930_SetMode(BYTE* s, void*, unsigned mode, int a, int b, int c, int d)
{
    switch (mode) {
    case 0: F(int, s, 9784) = 0; break;
    case 1: F(int, s, 9784) = 1; break;
    case 2:
    case 3:
        CallThis(FUN_4f1240, s, 2, a, b);
        CallThis(FUN_4f1dd0, s, c);
        CallThis(FUN_4f1ef0, s, d);
        F(int, s, 9784) = mode;
        break;
    }
}
