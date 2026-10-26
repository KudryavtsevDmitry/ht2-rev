// vehicle_physics.cpp - small functions of the unit at 0x4eb0c7 of king.exe (the
// vehicle's controls and physics): ramps of a control between limits, the
// wheel and contact points of the body, the engine's state, steering
// geometry, and copies of the body's vectors.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" void FUN_4eb350(); KING_ALIAS("_FUN_4eb350", "@Trigger_Check@12")
extern "C" void __fastcall Body_SetLoad(BYTE* self, void*, float load);
extern "C" void __fastcall Body_UpdateHitch(BYTE* self, void*, int keep);
extern "C" void FUN_4f2670(); KING_ALIAS("_FUN_4f2670", "@Wheels_StepFrames@36")
extern "C" void FUN_4f8260(); KING_ALIAS("_FUN_4f8260", "@Body_SubStep@12")

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

extern "C" void FUN_4f0800(); KING_ALIAS("_FUN_4f0800", "@Body_FrameTerms@4")
extern "C" void FUN_4f0530(); KING_ALIAS("_FUN_4f0530", "@Body_SpinAxis@4")
extern "C" void FUN_4f09f0(); KING_ALIAS("_FUN_4f09f0", "@Body_KeepFrame@12")

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

KING_VAR(double, g_64b5a0, 64b5a0)    // 0.01

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

extern "C" void FUN_4f0fc0(); KING_ALIAS("_FUN_4f0fc0", "@Body_PlaceAt@16")
KING_VAR(float, g_quarterPi674408, 674408)
KING_VAR(double, g_64b6e0, 64b6e0)            // 0.5
KING_VAR(double, g_64b840, 64b840)            // 0.2

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

extern "C" void FUN_4ef480(); KING_ALIAS("_FUN_4ef480", "@Wheels_LoadShares@8")
typedef BYTE Bytes697f9c[1];
KING_VAR(Bytes697f9c, g_697f9c, 697f9c)     // 2416-byte records by vehicle type
KING_VAR(double, g_64c958, 64c958)            // 0.9
KING_VAR(float, g_674414, 674414)             // 0.75
KING_VAR(float, g_9_8, 64d990)                // 9.8
KING_VAR(double, g_64b668, 64b668)            // 1.5

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
KING_VAR(AsmLabel, g_carTypeNames, 69809c)
KING_VAR(AsmLabel, s_67456c, 67456c)
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
extern "C" void FUN_4ebbe0(); KING_ALIAS("_FUN_4ebbe0", "@Car_RandomStep@12")
ASM_PROC(FUN_4eb2f0)
KING_VAR(AsmLabel, g_697608, 697608)
KING_VAR(AsmLabel, g_6975b8, 6975b8)
KING_VAR(int, g_697658, 697658)
KING_VAR(int, g_69765c, 69765c)
KING_VAR(int, g_697660, 697660)

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
extern "C" void FUN_4f0530(); KING_ALIAS("_FUN_4f0530", "@Body_SpinAxis@4")
extern "C" void FUN_4f0720(); KING_ALIAS("_FUN_4f0720", "@Body_SaveState@8")
extern "C" void FUN_4fef20(); KING_ALIAS("_FUN_4fef20", "@Body_StateStep@4")
KING_VAR(AsmLabel, s_vp6746e4, 6746e4)

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
extern "C" void FUN_4f1240(); KING_ALIAS("_FUN_4f1240", "@Body_PutDown@20")
extern "C" void FUN_4f1ef0(); KING_ALIAS("_FUN_4f1ef0", "@Body_SetVec88@12")

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

extern "C" void FUN_530190(); KING_ALIAS("_FUN_530190", "@Thunk530190@8")

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

extern "C" void FUN_4f1240(); KING_ALIAS("_FUN_4f1240", "@Body_PutDown@20")
extern "C" void FUN_4f1ef0(); KING_ALIAS("_FUN_4f1ef0", "@Body_SetVec88@12")
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

// ---------------------------------------------------------------------------
// A placed object.

extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
KING_VAR(AsmLabel, g_q98_702650, 702650)
KING_VAR(AsmLabel, g_q98_702630, 702630)
// [$L_4ee4d0] constructor: the node part (+32) initialised, the position and size from the 702650 and 702630
// defaults, the rest cleared
extern "C" BYTE* __fastcall Placed4ee4d0_Construct(BYTE* s)
{
    memset(s, 0, 24);
    CallThis(FUN_5ef5c0, s + 32);
    memcpy(s, (BYTE*)g_q98_702650, 12);
    memcpy(s + 12, (BYTE*)g_q98_702630, 12);
    F(short, s, 24) = 0;
    F(short, s, 26) = 0;
    F(int, s, 132) = 0;
    F(int, s, 240) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Random tuning steps.

extern "C" void FUN_501870(); KING_ALIAS("_FUN_501870", "@Car_StepSetting@12")
extern "C" int __cdecl rand(void);
static int Vp100_Step(BYTE* s, int i, int last)
{
    if (F(int, s, 15520 + i * 20) >= F(int, s, 15524 + i * 20) - last)
        return 2;
    CallThis(FUN_501870, s, i);
    return 1;
}
// [FUN_4ebbe0] steps one setting of car s of kind k (0: setting 3 or 4 below its last step, 1: setting 0,
// 2: setting 1, 3: one of the +3540 settings from 19); 1 when stepped, 2 when it is at its end, 0 for other kinds
extern "C" int __fastcall Car_RandomStep(BYTE* s, void*, int k)
{
    switch (k) {
    case 0: return Vp100_Step(s, rand() % 2 + 3, 1);
    case 1: return Vp100_Step(s, 0, 0);
    case 2: return Vp100_Step(s, 1, 0);
    case 3: return Vp100_Step(s, rand() % F(int, s, 3540) + 19, 0);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Re-orthogonalising a body's matrix.

extern "C" void FUN_5de950(); KING_ALIAS("_FUN_5de950", "_Matrix_ToAngles")
extern "C" void FUN_5da950(); KING_ALIAS("_FUN_5da950", "_Matrix_FromPose")
extern "C" void FUN_4f0050(); KING_ALIAS("_FUN_4f0050", "@Body_CheckUpright@8")
struct Vp101_Mat { DWORD v[12]; };
// splits the matrix of body s (+16) into its position and angles
static void Vp101_Split(BYTE* s, double* d)
{
    CallC(FUN_5de950, *(Vp101_Mat*)(s + 16), d, d + 1, d + 2, d + 3, d + 4, d + 5);
}
// [FUN_4f0c20] rebuilds the matrix of body s (+16) from its position and angles, then places the body there
extern "C" void __fastcall Body_Rebuild(BYTE* s)
{
    double d[6];
    BYTE m[48];
    Vp101_Split(s, d);
    memcpy(s + 16, CallC<BYTE*>(FUN_5da950, m, d[0], d[1], d[2], d[3], d[4], d[5]), 48);
    CallThis(FUN_4f09f0, s, d);
}
// [FUN_4f1180] with +13124 set: turns body s upright (its heading kept) and checks it; else 1
extern "C" int __fastcall Body_SetUpright(BYTE* s)
{
    if (!F(int, s, 13124))
        return 1;
    double d[6];
    BYTE m[48];
    Vp101_Split(s, d);
    CallC(FUN_5da950, m, d[0], d[1], d[2], d[3], 0.0, 0.0);
    CallThis(FUN_4f1240, s, 2, F(int, s, 244), m);
    CallThis(FUN_4f0050, s);
    return 0;
}

extern "C" void FUN_4e30f4(); KING_ALIAS("_FUN_4e30f4", "_Vec3_MulComponents")
extern "C" void FUN_4f07e0(); KING_ALIAS("_FUN_4f07e0", "@Body_SaveVec88@8")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
// [FUN_4f1ef0] sets body s's local vector (+88) to v and its world form (+76: v scaled by +8008, turned by the
// body's axes +16..+48), then saves it
extern "C" void __fastcall Body_SetVec88(BYTE* s, void*, const float* v)
{
    memcpy(s + 88, v, 12);
    float t[3];
    const float* r = CallC<const float*>(FUN_4e30f4, t, s + 88, s + 8008);
    float x = (float)(F(float, s, 28) * r[1] + F(float, s, 40) * r[2] + r[0] * F(float, s, 16));
    float y = (float)(F(float, s, 32) * r[1] + F(float, s, 20) * r[0] + F(float, s, 44) * r[2]);
    float z = (float)(F(float, s, 36) * r[1] + F(float, s, 24) * r[0] + F(float, s, 48) * r[2]);
    F(float, s, 76) = x;
    F(float, s, 80) = y;
    F(float, s, 84) = z;
    CallThis(FUN_5e48f0, s);
    CallThis(FUN_4f07e0, s);
}

// === a physics state's initial values (FUN_4f6cf0)

KING_VAR(AsmLabel, g_ps7025f0, 7025f0)        // a 4x3 identity matrix
KING_VAR(AsmLabel, g_ps702650, 702650)        // a vector
KING_VAR(AsmLabel, g_ps702630, 702630)        // a vector

static void Ps_Vec(BYTE* p, const void* v)
{
    memcpy(p, v, 12);
}

extern "C" {

// [FUN_4f6cf0] initializes a physics state (thiscall): five identity matrices (+32..), zeroed accumulators,
// eight sets of vectors (+568, +664, +788) and the forces (+884..) from the default vectors, 0.1 damping
// factors (+760..+776)
BYTE* __fastcall PhysState_Init(BYTE* self, void*)
{
    for (int k = 0; k < 5; k++)
        memcpy(self + 32 + k * 48, g_ps7025f0, 48);
    memset(self + 272, 0, 228);
    memset(self + 568, 0, 96);
    memset(self + 664, 0, 96);
    memset(self + 788, 0, 96);
    memset(self + 884, 0, 72);
    memcpy(self + 960, g_ps7025f0, 48);
    memset(self + 1008, 0, 24);
    for (int k = 0; k < 8; k++) {
        F(int, self, k * 4) = 0;
        F(int, self, 536 + k * 4) = 0;
        F(int, self, 504 + k * 4) = 0;
        Ps_Vec(self + 568 + k * 12, g_ps702650);
        Ps_Vec(self + 664 + k * 12, g_ps702650);
        Ps_Vec(self + 788 + k * 12, g_ps702650);
    }
    for (int k = 0; k < 5; k++)
        memcpy(self + 32 + k * 48, g_ps7025f0, 48);
    static const short k_vecs[11] = {272, 284, 296, 308, 320, 344, 356, 368, 404, 440, 452};
    for (int i = 0; i < 11; i++)
        Ps_Vec(self + k_vecs[i], g_ps702650);
    F(DWORD, self, 760) = 1036831949;      // 0.1f
    F(DWORD, self, 776) = 1036831949;
    F(DWORD, self, 772) = 1036831949;
    F(DWORD, self, 768) = 1036831949;
    F(DWORD, self, 764) = 1036831949;
    F(int, self, 500) = 0;
    F(int, self, 780) = 1;
    F(int, self, 784) = 0;
    Ps_Vec(self + 884, g_ps702650);
    Ps_Vec(self + 896, g_ps702650);
    Ps_Vec(self + 908, g_ps702630);
    Ps_Vec(self + 920, g_ps702650);
    Ps_Vec(self + 932, g_ps702650);
    Ps_Vec(self + 944, g_ps702650);
    F(DWORD, self, 956) = 1065353216;      // 1.0f
    memcpy(self + 960, g_ps7025f0, 48);
    F(int, self, 1032) = 1;
    return self;
}

} // extern "C"

// === the engine constructor (FUN_4eb770)

KING_VAR(WORD, g_ec696bda, 696bda)
KING_VAR(float, g_ec64d970, 64d970)
extern "C" void FUN_4eba10(); KING_ALIAS("_FUN_4eba10", "@Engine_Reset@8")

extern "C" {

// [FUN_4eb770] constructor of engine e: an identity matrix at +144, the five wheels' vectors (+200 the scaled
// 702630, +260 .. +440 zero vectors), the gear settings (0.4, 0.6, 0.3 at +36, +40, +100), 16 zero vectors at
// +512 with four counters at +704, then reset
BYTE* __fastcall Engine_Construct(BYTE* e)
{
    memset(e + 144, 0, 48);
    memcpy(e + 144, g_ps7025f0, 48);
    memset(e + 200, 0, 312);
    memset(e + 512, 0, 192);
    F(WORD, e, 6) = 0;
    F(WORD, e, 4) = 0;
    F(WORD, e, 64) = 0;
    F(WORD, e, 66) = 0;
    F(int, e, 724) = 0;
    F(WORD, e, 8) = g_ec696bda;
    F(float, e, 36) = 0.4f;
    F(float, e, 40) = 0.6f;
    F(WORD, e, 74) = 1;
    F(int, e, 92) = 0;
    F(int, e, 80) = 1;
    F(int, e, 84) = 1;
    F(int, e, 76) = 0;
    F(float, e, 100) = 0.3f;
    F(int, e, 88) = 1;
    F(int, e, 104) = 1;
    F(int, e, 108) = 0;
    F(int, e, 112) = 0;
    const float* d = (const float*)g_ps702630;
    for (int i = 0; i < 5; i++) {
        float* v = (float*)(e + 200 + i * 12);
        v[0] = (float)((double)d[0] * g_ec64d970);
        v[1] = (float)((double)d[1] * g_ec64d970);
        v[2] = (float)((double)d[2] * g_ec64d970);
        for (int k = 1; k <= 4; k++)
            memcpy(e + 200 + k * 60 + i * 12, g_ps702650, 12);
    }
    F(int, e, 116) = 1;
    F(int, e, 192) = 0;
    F(int, e, 196) = 0;
    F(int, e, 120) = 0;
    F(int, e, 136) = 0;
    F(int, e, 124) = 0;
    F(int, e, 96) = 0;
    F(int, e, 128) = 0;
    F(BYTE, e, 0) = 2;
    F(int, e, 140) = 0;
    F(int, e, 132) = 0;
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++)
            memcpy(e + 512 + (j * 4 + k) * 12, g_ps702650, 12);
        F(int, e, 704 + j * 4) = 0;
    }
    F(int, e, 720) = 0;
    CallThis(FUN_4eba10, e);
    return e;
}

} // extern "C"

// === a body's physics frame (FUN_4f6860)

extern "C" void FUN_4e5b22(); KING_ALIAS("_FUN_4e5b22", "_Query_4e5b22")
extern "C" void FUN_4eb6e0(); KING_ALIAS("_FUN_4eb6e0", "@Ramp_Down@12")
extern "C" void FUN_4f0000(); KING_ALIAS("_FUN_4f0000", "@Body_IsTipped@8")
extern "C" void FUN_4f0ce0(); KING_ALIAS("_FUN_4f0ce0", "@Body_UpdateContacts@8")
extern "C" void FUN_4f6b70(); KING_ALIAS("_FUN_4f6b70", "@Body_Step@12")
extern "C" void Bf_Length(); KING_ALIAS("_Bf_Length", "_Vec3_Length")

extern "C" {

// [FUN_4f6860] a physics frame of body s: its matrix (+16) rebuilt from its position and angles, the ground
// under it (+244, from the query of +2292 to +52), the sideways travel since then counted (+3808, ramping +16164
// down) unless +6580, then contacts and the step; tipped over it keeps its wheel state (+3544 from +8092) and asks
// to be set upright (+13124), else the contact lists (+10176/+10180) are cleared
void __fastcall Body_PhysicsFrame(BYTE* s)
{
    double d[6];
    BYTE m[48];
    Vp101_Split(s, d);
    memcpy(s + 16, CallC<BYTE*>(FUN_5da950, m, d[0], d[1], d[2], d[3], d[4], d[5]), 48);
    BYTE node[192];
    CallThis(FUN_5ef5c0, node);
    const float* p = (const float*)(s + 52);
    const float* q = (const float*)(s + 2292);
    if (CallC<int>(FUN_4e5b22, F(int, s, 6672), q, p, node))
        F(int, s, 244) = F(int, node, 104);
    if (!F(BYTE, s, 6580)) {
        double dx = (double)p[0] - q[0], dy = (double)p[1] - q[1], dz = (double)p[2] - q[2];
        const float* n = (const float*)(s + 208);
        float cx = (float)(dz * n[1] - dy * n[2]);
        float cy = (float)(dx * n[2] - dz * n[0]);
        double cz = dy * n[0] - dx * n[1];
        float e[3];
        e[0] = (float)((double)cy * n[2] - cz * n[1]);
        e[1] = (float)(cz * n[0] - (double)cx * n[2]);
        e[2] = (float)((double)cx * n[1] - (double)cy * n[0]);
        double len = CallC<double>(Bf_Length, e);
        float lf = (float)len;
        F(double, s, 3808) = len + F(double, s, 3808);
        CallThis(FUN_4eb6e0, s + 16164, lf);
    }
    CallThis(FUN_4f0ce0, s);
    CallThis(FUN_4f6b70, s, d);
    if (CallThis<int>(FUN_4f0000, s)) {
        for (int i = 0; i < F(int, s, 3540); i++)
            F(int, s, 3544 + i * 4) = F(int, s, 8092 + i * 4);
        F(int, s, 13124) = 1;
        return;
    }
    CallThis(FUN_5e48f0, s);
    for (int i = 0; i < F(int, s, 10176); i++) {
        memcpy(s + 10244 + i * 12, (BYTE*)g_q98_702650, 12);
        memcpy(s + 10304 + i * 12, (BYTE*)g_q98_702650, 12);
    }
    for (int i = 0; i < F(int, s, 10180); i++) {
        memcpy(s + 10364 + i * 12, (BYTE*)g_q98_702650, 12);
        memcpy(s + 10424 + i * 12, (BYTE*)g_q98_702650, 12);
    }
    F(int, s, 10176) = 0;
    F(int, s, 10180) = 0;
}

} // extern "C"

// === integrating a body's momentum (FUN_4fab20)

extern "C" void Bi_RotateBy(); KING_ALIAS("_Bi_RotateBy", "_Vec3_RotateBy")
extern "C" void Bi_DivComponents(); KING_ALIAS("_Bi_DivComponents", "_Vec3_DivComponents")
extern "C" void Bi_Length(); KING_ALIAS("_Bi_Length", "_Vec3_Length")
extern "C" void Bi_AddAssign(); KING_ALIAS("_Bi_AddAssign", "@Vec3_AddAssign@16")
KING_VAR(double, g_bi64b6d0, 64b6d0)

extern "C" {

// [FUN_4fab20] body s takes force f and torque t for its steps (+9512, +9508): its momentum (+9012; the part across
// the axis +208 kept at +3524, dropped while small on the ground unless +6586), its angular momentum (+9036), the
// angular speed (+9048, its change +9060, by the inertia +8008) and the momentum turned into the body (+9024, +8868)
void __fastcall Body_Integrate(BYTE* s, void*, const float* f, const float* t)
{
    double dt = F(float, s, 9512);
    float a[3] = {(float)(dt * f[0]), (float)(dt * f[1]), (float)(dt * f[2])};
    float* v = (float*)(s + 9012);
    float tmp[3];
    CallThis(Bi_AddAssign, v, tmp, a);
    const float* n = (const float*)(s + 208);
    double c0 = (double)n[1] * v[2] - (double)n[2] * v[1];
    double c1 = (double)n[2] * v[0] - (double)v[2] * n[0];
    double c2 = (double)v[1] * n[0] - (double)n[1] * v[0];
    float w[3] = {(float)(c1 * n[2] - c2 * n[1]), (float)(c2 * n[0] - c0 * n[2]), (float)(c0 * n[1] - c1 * n[0])};
    double l = CallC<double>(Bi_Length, w);
    F(float, s, 3524) = (float)l;
    if (F(float, s, 9500) * g_bi64b6d0 > l && F(int, s, 6844) && !F(BYTE, s, 6586)) {
        double d = (double)v[2] * n[2] + (double)v[1] * n[1] + (double)n[0] * v[0];
        float x = (float)(d * n[0]), y = (float)(d * n[1]);
        v[0] = x;
        v[1] = y;
        v[2] = (float)(d * n[2]);
    }
    double dr = F(float, s, 9508);
    float b[3] = {(float)(dr * t[0]), (float)(dr * t[1]), (float)(dr * t[2])};
    float m[3], tmp2[3], om[3], r[3];
    CallC(FUN_4e30f4, m, b, s + 8008);
    float* lm = (float*)(s + 9036);
    CallThis(Bi_AddAssign, lm, tmp2, m);
    CallC(Bi_DivComponents, om, lm, s + 8008);
    F(float, s, 9060) = (float)((double)om[0] - F(float, s, 9048));
    F(float, s, 9064) = (float)((double)om[1] - F(float, s, 9052));
    F(float, s, 9068) = (float)((double)om[2] - F(float, s, 9056));
    memcpy(s + 9048, om, 12);
    memcpy(s + 9024, CallC<float*>(Bi_RotateBy, r, lm, s + 8868), 12);
}

} // extern "C"

// === a body's saved state (FUN_4f09f0)

KING_VAR(AsmLabel, g_bs697f9c, 697f9c)       // 2416 bytes per car model

extern "C" {

// [FUN_4f09f0] body s keeps its state for the next frame: its frames (+16, +2304 to +10736, +10784), ground
// (+11084), position d (+24.. doubles to +13084), its spin turned by its frame (+13228), its points (+100, +7356 to
// +10880, +10976), the engine values (+15312..), the throttle capped at the model's (+220) and the wheel mask
void __fastcall Body_KeepFrame(BYTE* s, void*, const BYTE* d)
{
    memcpy(s + 10736, s + 16, 48);
    memcpy(s + 10784, s + 2304, 48);
    const float* m = (const float*)(s + 2304);
    F(int, s, 11084) = F(int, s, 244);
    F(float, s, 13084) = (float)F(double, d, 24);
    F(float, s, 13088) = (float)F(double, d, 32);
    F(float, s, 13092) = (float)F(double, d, 40);
    const float* v = (const float*)(s + 3416);
    float x = (float)((double)m[3] * v[1] + (double)m[6] * v[2] + (double)v[0] * m[0]);
    float y = (float)((double)m[4] * v[1] + (double)m[1] * v[0] + (double)m[7] * v[2]);
    float z = (float)((double)m[5] * v[1] + (double)m[2] * v[0] + (double)m[8] * v[2]);
    F(float, s, 13228) = x;
    F(float, s, 13232) = y;
    F(float, s, 13236) = z;
    for (int i = 0; i < F(int, s, 3540); i++) {
        memcpy(s + 10880 + i * 12, s + 100 + i * 12, 12);
        memcpy(s + 10976 + i * 12, s + 7356 + i * 12, 12);
    }
    F(int, s, 15416) = F(int, s, 10712);
    F(int, s, 15312) = F(int, s, 10716);
    F(int, s, 15320) = F(int, s, 7196);
    F(int, s, 15420) = F(int, s, 7300);
    F(float, s, 15424) = F(float, s, 7300);
    const float* lim = (const float*)((BYTE*)g_bs697f9c + F(int, s, 2252) * 2416 + 220);
    if (F(float, s, 7300) > *lim)
        F(float, s, 15424) = *lim;
    F(int, s, 15428) = F(int, s, 7304);
    F(int, s, 15488) = F(int, s, 6948);
    F(int, s, 15504) = F(int, s, 15540);
    F(int, s, 15508) = 0;
    for (int i = 0; i < F(int, s, 3540); i++)
        F(int, s, 15508) |= (F(int, s, 15580 + i * 20) != 0) << i;
    F(int, s, 15512) = F(int, s, 15560);
}

} // extern "C"

// [FUN_4f0fc0] places body s at matrix m (+16, its inverse +2304) with owner a (+244): the velocity +76 in body
// axes over the inertia +8008 into +88; each contact point (+6212, count +3540) placed (+100), its velocity
// (+3848) and flags (+3816, +3944) cleared, +9872 = 0.01 and +9840 = 0.5
extern "C" void __fastcall Body_PlaceAt(BYTE* s, void*, int a, const Matrix* m)
{
    F(Matrix, s, 16) = *m;
    Matrix t;
    F(Matrix, s, 2304) = *Matrix_Inverse(&t, (const Matrix*)(s + 16));
    const float* iv = (const float*)(s + 2304);
    const float* v = (const float*)(s + 76);
    Vec3 l, r;
    l.x = (float)((double)iv[6] * v[2] + (double)iv[3] * v[1] + (double)v[0] * iv[0]);
    l.y = (float)((double)iv[7] * v[2] + (double)iv[1] * v[0] + (double)iv[4] * v[1]);
    l.z = (float)((double)iv[8] * v[2] + (double)iv[2] * v[0] + (double)iv[5] * v[1]);
    F(Vec3, s, 88) = *Vec3_DivComponents(&r, &l, (const Vec3*)(s + 8008));
    F(int, s, 244) = a;
    const float* M = (const float*)(s + 16);
    for (int i = 0; i < F(int, s, 3540); i++) {
        const float* q = (const float*)(s + 6212 + i * 12);
        float* w = (float*)(s + 100 + i * 12);
        float x = (float)((double)q[1] * M[3] + (double)q[2] * M[6] + (double)q[0] * M[0] + M[9]);
        float y = (float)((double)q[1] * M[4] + (double)M[1] * q[0] + (double)q[2] * M[7] + M[10]);
        float z = (float)((double)q[2] * M[8] + (double)q[1] * M[5] + (double)q[0] * M[2] + M[11]);
        w[0] = x, w[1] = y, w[2] = z;
        memcpy(s + 3848 + i * 12, (BYTE*)g_q98_702650, 12);
        F(int, s, 3816 + i * 4) = 0;
        F(int, s, 3944 + i * 4) = 0;
        F(DWORD, s, 9872 + i * 4) = 0x3C23D70A;
        F(DWORD, s, 9840 + i * 4) = 0x3F000000;
    }
}

// === a body's tilt from its slope (FUN_4f01b0)

extern "C" void Bt_RotZ(); KING_ALIAS("_Bt_RotZ", "_Matrix_RotationZ")
KING_VAR(double, g_bt64b658, 64b658)
KING_VAR(double, g_bt64b730, 64b730)
KING_VAR(float, g_bt64c250, 64c250)

extern "C" {

// [FUN_4f01b0] body s's lean from its two slope angles (+6692, +6688): the gravity components along its axes
// (+6308 by +2724, +6332 by +2748) over cos(+6692) (or by 64b730 when that is tiny) into +3532 and +3536, the
// smaller one k scaling the offset +2616 turned by +6692 into +220 and, with sin(+6688) / +8032, the vector +208
// taken into the body's axes (+2304) into +232
void __fastcall Body_SlopeLean(BYTE* s)
{
    float ang = F(float, s, 6692);
    float c1 = (float)x87_cos(ang);
    double c2 = x87_cos(F(float, s, 6688));
    double a = (double)F(float, s, 6316) * F(float, s, 2732) + (double)F(float, s, 6312) * F(float, s, 2728)
        + (double)F(float, s, 6308) * F(float, s, 2724);
    double p = a * c2;
    float b = (float)((double)F(float, s, 6340) * F(float, s, 2756) + (double)F(float, s, 6336) * F(float, s, 2752)
        + (double)F(float, s, 6332) * F(float, s, 2748));
    float q;
    if (c1 > g_bt64b658) {
        double inv = g_bt64c250 / c1;
        c1 = (float)inv;
        p *= inv;
        q = (float)(c1 * (double)b);
    } else {
        p *= g_bt64b730;
        q = (float)(b * g_bt64b730);
    }
    float k = x87_fabs((double)q) > x87_fabs(p) ? (float)p : q;
    F(float, s, 3532) = (float)p;
    F(float, s, 3536) = q;
    float m[12];
    const float* R = CallC<const float*>(Bt_RotZ, m, *(DWORD*)&ang);
    const float* v = (const float*)(s + 2616);
    double wx = (double)R[6] * v[2] + (double)R[3] * v[1] + (double)v[0] * R[0] + R[9];
    float wy = (float)((double)R[1] * v[0] + (double)R[7] * v[2] + (double)R[4] * v[1] + R[10]);
    float wz = (float)((double)R[2] * v[0] + (double)R[8] * v[2] + (double)R[5] * v[1] + R[11]);
    F(float, s, 220) = (float)(wx * k);
    F(float, s, 224) = wy * k;
    F(float, s, 228) = wz * k;
    double f = x87_sin(F(float, s, 6688)) * (k / F(float, s, 8032));
    float ux = (float)(f * F(float, s, 208)), uy = (float)(f * F(float, s, 212));
    double uz = f * F(float, s, 216);
    const float* M = (const float*)(s + 2304);
    float rx = (float)(uz * M[6] + (double)uy * M[3] + (double)ux * M[0]);
    float ry = (float)(uz * M[7] + (double)uy * M[4] + (double)ux * M[1]);
    float rz = (float)(uz * M[8] + (double)uy * M[5] + (double)ux * M[2]);
    F(float, s, 232) = rx;
    F(float, s, 236) = ry;
    F(float, s, 240) = rz;
}

} // extern "C"

// === a body's spin axis terms (FUN_4f0530)

extern "C" void Sa_Length(); KING_ALIAS("_Sa_Length", "_Vec3_Length")
KING_VAR(double, g_sa64b6d0, 64b6d0)

extern "C" {

// [FUN_4f0530] body s's spin (+64): a spin rate (+3520) jumping by more than 64b6d0 or below 1 clears +3216 and
// +3288; d = a x (w x a) for the axis a (+208) into +2544 with its length +3524, its part along +2616 (+3528,
// scaled +2592) and its direction +2556 (d itself when tiny)
void __fastcall Body_SpinAxis(BYTE* s)
{
    const float* w = (const float*)(s + 64);
    double l = CallC<double>(Sa_Length, w);
    if (x87_fabs(F(float, s, 3520) - l) > g_sa64b6d0 || l < g_bt64c250) {
        memcpy(s + 3216, (BYTE*)g_q98_702650, 12);
        memcpy(s + 3288, (BYTE*)g_q98_702650, 12);
    }
    F(float, s, 3520) = (float)l;
    const float* a = (const float*)(s + 208);
    double c1 = (double)w[1] * a[2] - (double)w[2] * a[1];
    double c2 = (double)a[0] * w[2] - (double)a[2] * w[0];
    double c3 = (double)a[1] * w[0] - (double)a[0] * w[1];
    float* d = (float*)(s + 2544);
    d[0] = (float)(c3 * a[1] - c2 * a[2]);
    d[1] = (float)(c1 * a[2] - c3 * a[0]);
    d[2] = (float)(c2 * a[0] - c1 * a[1]);
    double l2 = CallC<double>(Sa_Length, d);
    F(float, s, 3524) = (float)l2;
    const float* r = (const float*)(s + 2616);
    double p = (double)d[2] * r[2] + (double)d[1] * r[1] + (double)d[0] * r[0];
    F(float, s, 3528) = (float)p;
    float* o = (float*)(s + 2592);
    o[0] = (float)(p * r[0]);
    o[1] = (float)(p * r[1]);
    o[2] = (float)(p * r[2]);
    float* n = (float*)(s + 2556);
    if (l2 > g_bt64b658) {
        n[0] = (float)(d[0] / l2);
        n[1] = (float)(d[1] / l2);
        n[2] = (float)(d[2] / l2);
    } else
        memcpy(n, d, 12);
}

} // extern "C"

// === a body's frame terms (FUN_4f0800)

extern "C" void Bf_NormalizeThis(); KING_ALIAS("_Bf_NormalizeThis", "@Vec3_NormalizeThis@8")
KING_VAR(float, g_bf702620, 702620)
KING_VAR(float, g_bf702624, 702624)
KING_VAR(float, g_bf702628, 702628)

extern "C" {

// [FUN_4f0800] body s's frame-dependent terms: the inverse frame (+2304), the velocity in body axes over the inertia
// (+88), the forward axis (+2604, 702620 turned by the frame) and the unit side axis (axis +208 x forward) x axis
// (+2616)
void __fastcall Body_FrameTerms(BYTE* s)
{
    Matrix t;
    F(Matrix, s, 2304) = *Matrix_Inverse(&t, (const Matrix*)(s + 16));
    const float* iv = (const float*)(s + 2304);
    const float* v = (const float*)(s + 76);
    Vec3 l, r;
    l.x = (float)((double)iv[3] * v[1] + (double)iv[6] * v[2] + (double)v[0] * iv[0]);
    l.y = (float)((double)iv[4] * v[1] + (double)iv[1] * v[0] + (double)iv[7] * v[2]);
    l.z = (float)((double)iv[5] * v[1] + (double)iv[2] * v[0] + (double)iv[8] * v[2]);
    F(Vec3, s, 88) = *Vec3_DivComponents(&r, &l, (const Vec3*)(s + 8008));
    const float* R = (const float*)(s + 16);
    float* f = (float*)(s + 2604);
    f[0] = (float)((double)g_bf702628 * R[6] + (double)g_bf702624 * R[3] + (double)g_bf702620 * R[0]);
    f[1] = (float)((double)g_bf702628 * R[7] + (double)g_bf702624 * R[4] + (double)g_bf702620 * R[1]);
    f[2] = (float)((double)g_bf702628 * R[8] + (double)g_bf702624 * R[5] + (double)g_bf702620 * R[2]);
    const float* a = (const float*)(s + 208);
    double c1 = (double)a[1] * f[2] - (double)a[2] * f[1];
    double c2 = (double)f[0] * a[2] - (double)f[2] * a[0];
    double c3 = (double)f[1] * a[0] - (double)f[0] * a[1];
    float* o = (float*)(s + 2616);
    o[0] = (float)(c2 * a[2] - c3 * a[1]);
    o[1] = (float)(c3 * a[0] - c1 * a[2]);
    o[2] = (float)(c1 * a[1] - c2 * a[0]);
    CallThis(Bf_NormalizeThis, o);
}

} // extern "C"

// === placing a body and resetting its contacts (FUN_4f0df0)

extern "C" void FUN_4ef480(); KING_ALIAS("_FUN_4ef480", "@Wheels_LoadShares@8")
KING_VAR(float, g_bp64c450, 64c450)

extern "C" {

// [FUN_4f0df0] Body_PlaceAt plus FUN_4ef480 before the contacts, whose loads +3360 restart from their rest values
// (+9808 by +9792 and 64c450)
void __fastcall Body_PlaceReset(BYTE* s, void*, int a, const Matrix* m)
{
    F(Matrix, s, 16) = *m;
    Matrix t;
    F(Matrix, s, 2304) = *Matrix_Inverse(&t, (const Matrix*)(s + 16));
    const float* iv = (const float*)(s + 2304);
    const float* v = (const float*)(s + 76);
    Vec3 l, r;
    l.x = (float)((double)iv[6] * v[2] + (double)iv[3] * v[1] + (double)v[0] * iv[0]);
    l.y = (float)((double)iv[7] * v[2] + (double)iv[1] * v[0] + (double)iv[4] * v[1]);
    l.z = (float)((double)iv[8] * v[2] + (double)iv[2] * v[0] + (double)iv[5] * v[1]);
    F(Vec3, s, 88) = *Vec3_DivComponents(&r, &l, (const Vec3*)(s + 8008));
    F(int, s, 244) = a;
    CallThis(FUN_4ef480, s);
    const float* M = (const float*)(s + 16);
    for (int i = 0; i < F(int, s, 3540); i++) {
        const float* q = (const float*)(s + 6212 + i * 12);
        float* w = (float*)(s + 100 + i * 12);
        float x = (float)((double)M[6] * q[2] + (double)M[0] * q[0] + (double)q[1] * M[3] + M[9]);
        float y = (float)((double)M[1] * q[0] + (double)M[7] * q[2] + (double)q[1] * M[4] + M[10]);
        float z = (float)((double)q[1] * M[5] + (double)M[2] * q[0] + (double)q[2] * M[8] + M[11]);
        w[0] = x, w[1] = y, w[2] = z;
        memcpy(s + 3848 + i * 12, (BYTE*)g_q98_702650, 12);
        F(int, s, 3816 + i * 4) = 0;
        F(DWORD, s, 9872 + i * 4) = 0x3C23D70A;
        F(DWORD, s, 9840 + i * 4) = 0x3F000000;
        F(int, s, 3944 + i * 4) = 0;
        F(float, s, 3360 + i * 4) = (float)((double)F(float, s, 9808 + i * 4) * F(float, s, 9792) * g_bp64c450);
    }
}

} // extern "C"

// === restarting a body's axis motion (FUN_4efc10)

extern "C" Vec3* __fastcall Vec3_SetThis(Vec3* self, void*, float x, float y, float z);
extern "C" void Ba_Length(); KING_ALIAS("_Ba_Length", "_Vec3_Length")
extern "C" void Ba_UpdateGrip(); KING_ALIAS("_Ba_UpdateGrip", "@Body_UpdateGrip@8")
extern "C" void Ba_Cross(); KING_ALIAS("_Ba_Cross", "_Vec3_Cross")
extern "C" void Ba_DivScalar(); KING_ALIAS("_Ba_DivScalar", "_Vec3_DivScalar")
extern "C" void Ba_Dot(); KING_ALIAS("_Ba_Dot", "_Vec3_Dot")
extern "C" void Ba_Scaled(); KING_ALIAS("_Ba_Scaled", "_Vec3_Scaled")
KING_VAR(AsmLabel, g_ba7025f0, 7025f0)
KING_VAR(double, g_ba64b5a0, 64b5a0)

extern "C" {

// [FUN_4efc10] body s restricted to its axis a (+208): the spin (+64) kept along it, the velocity (+76) across
// it, +3108 cleared, the contacts' flags (+6596, +6612) cleared, grip updated, +2400 identity, +3452 = 10; the
// forward axis (+2604), the side axis (+2616), the spin rate (+3520) and the spin's cross term (+2544 with its
// length +3524, direction +2556, its part along the side +3528 / +2592); +3416 = +3392
void __fastcall Body_AxisRestart(BYTE* s)
{
    const float* a = (const float*)(s + 208);
    float* w = (float*)(s + 64);
    double p = (double)w[2] * a[2] + (double)w[1] * a[1] + (double)w[0] * a[0];
    float wx = (float)(p * a[0]), wy = (float)(p * a[1]), wz = (float)(p * a[2]);
    w[0] = wx, w[1] = wy, w[2] = wz;
    float* v = (float*)(s + 76);
    double c1 = (double)v[2] * a[1] - (double)a[2] * v[1];
    double c2 = (double)a[2] * v[0] - (double)v[2] * a[0];
    double c3 = (double)v[1] * a[0] - (double)v[0] * a[1];
    float vx = (float)(c2 * a[2] - c3 * a[1]);
    float vy = (float)(c3 * a[0] - c1 * a[2]);
    float vz = (float)(c1 * a[1] - c2 * a[0]);
    v[0] = vx, v[1] = vy, v[2] = vz;
    memcpy(s + 3108, (BYTE*)g_q98_702650, 12);
    F(float, s, 3520) = CallC<float>(Ba_Length, w);
    F(int, s, 3524) = 0;
    for (int i = 0; i < F(int, s, 3540); i++) {
        F(BYTE, s, 6596 + i) = 0;
        F(int, s, 6612 + i * 4) = 0;
    }
    CallThis(Ba_UpdateGrip, s);
    F(BYTE, s, 6580) = 0;
    F(BYTE, s, 6581) = 0;
    F(BYTE, s, 6583) = 0;
    memcpy(s + 2400, g_ba7025f0, 48);
    F(DWORD, s, 3452) = 0x41200000;
    const float* m = (const float*)(s + 16);
    Vec3 f, e, g, h, t;
    Vec3_SetThis(&f, 0, (float)((double)g_bf702628 * m[6] + (double)g_bf702624 * m[3] + (double)g_bf702620 * m[0]),
        (float)((double)g_bf702628 * m[7] + (double)g_bf702624 * m[4] + (double)g_bf702620 * m[1]),
        (float)((double)g_bf702628 * m[8] + (double)g_bf702624 * m[5] + (double)g_bf702620 * m[2]));
    F(Vec3, s, 2604) = f;
    Vec3_SetThis(&e, 0, (float)((double)a[1] * f.z - (double)f.y * a[2]), (float)((double)f.x * a[2] - (double)a[0] * f.z),
        (float)((double)f.y * a[0] - (double)a[1] * f.x));
    Vec3_SetThis(&g, 0, (float)((double)e.y * a[2] - (double)e.z * a[1]), (float)((double)e.z * a[0] - (double)e.x * a[2]),
        (float)((double)e.x * a[1] - (double)e.y * a[0]));
    F(Vec3, s, 2616) = g;
    CallThis(Bf_NormalizeThis, s + 2616);
    F(float, s, 3520) = CallC<float>(Ba_Length, w);
    Vec3_SetThis(&h, 0, (float)((double)w[1] * a[2] - (double)w[2] * a[1]), (float)((double)w[2] * a[0] - (double)a[2] * w[0]),
        (float)((double)w[0] * a[1] - (double)w[1] * a[0]));
    F(Vec3, s, 2544) = *CallC<Vec3*>(Ba_Cross, &t, a, &h);
    float l = CallC<float>(Ba_Length, s + 2544);
    F(float, s, 3524) = l;
    if (l > g_ba64b5a0)
        F(Vec3, s, 2556) = *CallC<Vec3*>(Ba_DivScalar, &t, s + 2544, *(DWORD*)&l);
    else
        F(Vec3, s, 2556) = F(Vec3, s, 2616);
    float d = CallC<float>(Ba_Dot, s + 2544, s + 2616);
    F(float, s, 3528) = d;
    F(Vec3, s, 2592) = *CallC<Vec3*>(Ba_Scaled, &t, s + 2616, *(DWORD*)&d);
    F(Vec3, s, 3416) = F(Vec3, s, 3392);
}

} // extern "C"

// === drive and brake forces of the wheels (FUN_4f3000)

extern "C" void Wf_MulAssign(); KING_ALIAS("_Wf_MulAssign", "@Vec3_MulAssign@16")
KING_VAR(double, g_wf64b5b8, 64b5b8)
KING_VAR(double, g_wf64b678, 64b678)
KING_VAR(double, g_wf64b670, 64b670)
KING_VAR(float, g_wf64c264, 64c264)
KING_VAR(float, g_wf64c250, 64c250)

static void Wf_Zero(BYTE* d)
{
    memcpy(d, g_q98_702650, 12);
}

static void Wf_Scale(BYTE* d, const float* v, float k)
{
    F(float, d, 0) = k * v[0], F(float, d, 4) = k * v[1], F(float, d, 8) = k * v[2];
}

extern "C" {

// [FUN_4f3000] for body s's wheels (+3540) on the ground (+6564 zero): the load share (+9840 * +3360) of the
// driven ones (+7092) gets the engine force +7316, the brake (+7320 * +10716, braking when +10716 above 64b5b8,
// +6844) works against the rolling direction; grip (+6780) limits the sum: a wheel that slips gets its forces
// (+8340, +8628) cut back and +6596 / +6612 set; the total force used; then the grip is updated
float __fastcall Wheels_DriveForces(BYTE* s)
{
    Game_FrameTime();
    F(int, s, 6844) = F(float, s, 10716) > g_wf64b5b8 ? 1 : 0;
    int n = F(int, s, 3540);
    float total = 0;
    float load[8], drive[8], brake[8], grip[8], net[8];
    for (int i = 0; i < n; i++) {
        F(int, s, 6612 + i * 4) = 0;
        load[i] = 0, drive[i] = 0, brake[i] = 0, grip[i] = 0;
        Wf_Zero(s + 8628 + i * 12);
        Wf_Zero(s + 8340 + i * 12);
    }
    float sum = 0.01f;
    for (int i = 0; i < n; i++) {
        if (F(WORD, s, 6564 + i * 2))
            continue;
        load[i] = F(float, s, 9840 + i * 4) * F(float, s, 3360 + i * 4);
        if (F(int, s, 7092 + i * 4))
            sum = load[i] + sum;
    }
    for (int i = 0; i < n; i++) {
        if (F(WORD, s, 6564 + i * 2))
            continue;
        const float* dir = (const float*)(s + 6404 + i * 12);
        if (F(int, s, 7092 + i * 4))
            drive[i] = load[i] * F(float, s, 7316) / sum;
        float g = F(float, s, 6780 + i * 4) * load[i];
        grip[i] = g;
        if (F(int, s, 6844))
            brake[i] = (F(float, s, 6780 + i * 4) < g_wf64c264 ? -1 : 1) * F(float, s, 7320 + i * 4) *
                F(float, s, 10716);
        int sg = 1;
        if ((double)F(float, s, 36) * dir[2] + (double)F(float, s, 32) * dir[1] + (double)F(float, s, 28) * dir[0] <
            g_wf64b678) {
            brake[i] = -brake[i];
            sg = -1;
        }
        float d = drive[i] - brake[i];
        net[i] = d;
        if (x87_fabs(g) < x87_fabs(d)) {
            if (x87_fabs(drive[i]) < x87_fabs(brake[i])) {
                if (x87_fabs(F(float, s, 3528)) > g_wf64b670) {
                    F(BYTE, s, 6596 + i) = 1;
                    F(int, s, 6612 + i * 4) = -1;
                }
                float k = F(float, s, 6500 + i * 4) < g_wf64c250 ? -(F(float, s, 6500 + i * 4) * load[i]) : -load[i];
                Wf_Scale(s + 8628 + i * 12, dir, k);
                Wf_Zero(s + 8436 + i * 12);
                Wf_Zero(s + 8532 + i * 12);
                F(int, s, 3772 + i * 4) = 0;
                total = (float)(x87_fabs(grip[i]) + total);
            } else {
                float t[3];
                F(BYTE, s, 6596 + i) = 1;
                CallThis(Wf_MulAssign, s + 8436 + i * 12, t, 0x3DCCCCCDu);
                CallThis(Wf_MulAssign, s + 8532 + i * 12, t, 0x3DCCCCCDu);
                const float* fw = (const float*)(s + 2724 + i * 12);
                if (drive[i] > g_wf64c264) {
                    Wf_Scale(s + 8340 + i * 12, fw, load[i]);
                    F(int, s, 6612 + i * 4) = 1;
                } else {
                    Wf_Scale(s + 8340 + i * 12, fw, -load[i]);
                    F(int, s, 6612 + i * 4) = -1;
                }
                total = total + brake[i];
            }
        } else {
            if (x87_fabs(drive[i]) < x87_fabs(brake[i])) {
                brake[i] = -(sg * d);
                drive[i] = 0;
            } else {
                drive[i] = d;
                brake[i] = 0;
            }
            const float* fw = (const float*)(s + 2628 + i * 12);
            float m[3] = {-fw[0], -fw[1], -fw[2]};
            Wf_Scale(s + 8628 + i * 12, m, brake[i]);
            if ((double)F(float, s, 2624) * fw[2] + (double)F(float, s, 2620) * fw[1] +
                    (double)F(float, s, 2616) * fw[0] > g_wf64b678)
                Wf_Scale(s + 8340 + i * 12, fw, drive[i]);
            else
                Wf_Scale(s + 8340 + i * 12, fw, -drive[i]);
            total = (float)(x87_fabs(brake[i]) + total);
        }
    }
    Body_UpdateGrip(s, 0);
    return total;
}

} // extern "C"

// === tyre grip curves (FUN_4f4d60)

KING_VAR(double, g_tg64b6d0, 64b6d0)
KING_VAR(double, g_tg64b840, 64b840)
KING_VAR(float, g_tg64d350, 64d350)
KING_VAR(float, g_tg64c2fc, 64c2fc)
KING_VAR(double, g_tg64b5b8, 64b5b8)
KING_VAR(double, g_tg64d9e8, 64d9e8)
KING_VAR(double, g_tg64d9e0, 64d9e0)
KING_VAR(float, g_tg64c40c, 64c40c)
KING_VAR(float, g_tg64d2f8, 64d2f8)
KING_VAR(float, g_tg64c250, 64c250)
KING_VAR(float, g_tg64c264, 64c264)
KING_VAR(float, g_tg67442c, 67442c)
KING_VAR(float, g_tg674430, 674430)
KING_VAR(float, g_tg674434, 674434)
KING_VAR(float, g_tg674438, 674438)
KING_VAR(float, g_tg67443c, 67443c)
KING_VAR(float, g_tg674440, 674440)

extern "C" {

// [FUN_4f4d60] the grip factor of curve kind for slip a (and b for kind 7): 7 and 5 are slip curves, 0..4 shapes
// of |sin a|, 6 a step at 64b5b8; 1 otherwise
double __cdecl Tyre_Grip(int kind, float a, float b)
{
    if (kind == 7) {
        double x = x87_fabs(a);
        double q = g_tg64b6d0 / (x87_fabs(b) + g_tg64b6d0);
        double d = q * q * g_tg64b840 + x;
        return x * x / (d * d) * g_tg64d350 / (x + g_tg64c2fc);
    }
    if (kind == 5) {
        double x = x87_fabs(a);
        double u = (x - g_tg64b5b8) * g_tg64d9e8;
        double v = x + g_tg64b5b8;
        double g = x87_exp(-(u * u)) * x * g_tg64d9e0 / v;
        return (g + x * x / ((x + g_tg64c40c) * v * v)) * g_tg64d2f8;
    }
    double s = x87_fabs(x87_sin(a));
    switch (kind) {
    case 0:
        return (g_tg64c250 - s) * g_tg67442c * s + s;
    case 1:
        return (g_tg674434 + s) * (g_tg64c250 - s) * g_tg674430 + s;
    case 2:
        return (g_tg64c250 - g_tg674434) * s + g_tg674434;
    case 3:
        if (s > g_tg674438)
            return g_tg64c250;
        return s / g_tg674438 * (g_tg64c250 - g_tg67443c) + g_tg67443c;
    case 4: {
        double v = s > g_tg674438 ? g_tg64c250 : s / g_tg674438 * (g_tg64c250 - g_tg67443c) + g_tg67443c;
        return s * g_tg674440 + v;
    }
    case 6:
        return s > g_tg64b5b8 ? g_tg64c250 : g_tg64c264;
    }
    return g_tg64c250;
}

} // extern "C"

// === the wheels' tyre forces (FUN_4f4f60)

extern "C" void Tf_AddAssign(); KING_ALIAS("_Tf_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Tf_CheckPoint(); KING_ALIAS("_Tf_CheckPoint", "_World_CheckPoint")
KING_VAR(float, g_tf674404, 674404)
KING_VAR(float, g_tf64c250, 64c250)
KING_VAR(float, g_tf64c258, 64c258)
KING_VAR(float, g_tf64c264, 64c264)
KING_VAR(float, g_tf64d7b8, 64d7b8)
KING_VAR(double, g_tf64d9f0, 64d9f0)
KING_VAR(double, g_tf64b6e0, 64b6e0)
KING_VAR(double, g_tf64b5a0, 64b5a0)
KING_VAR(double, g_tf64b678, 64b678)
KING_VAR(AsmLabel, g_tf67479c, 67479c)

static void Tf_Zero(BYTE* d)
{
    memcpy(d, g_q98_702650, 12);
}

static void Tf_Scale(BYTE* d, const BYTE* v, float k)
{
    F(float, d, 0) = k * F(float, v, 0), F(float, d, 4) = k * F(float, v, 4), F(float, d, 8) = k * F(float, v, 8);
}

static double Tf_Dot(const BYTE* a, const BYTE* b)
{
    return (double)F(float, a, 8) * F(float, b, 8) + (double)F(float, a, 4) * F(float, b, 4) +
        (double)F(float, a, 0) * F(float, b, 0);
}

// out = body s's rotation (+2304) applied to v
static void Tf_Rotate(BYTE* s, const BYTE* v, float* out)
{
    float x = F(float, v, 0), y = F(float, v, 4), z = F(float, v, 8);
    out[0] = (float)((double)F(float, s, 2328) * z + (double)F(float, s, 2316) * y + (double)F(float, s, 2304) * x);
    out[1] = (float)((double)F(float, s, 2308) * x + (double)F(float, s, 2332) * z + (double)F(float, s, 2320) * y);
    out[2] = (float)((double)F(float, s, 2312) * x + (double)F(float, s, 2336) * z + (double)F(float, s, 2324) * y);
}

extern "C" {

// [FUN_4f4f60] the tyre forces of body s's wheels (+3540) under loads p: each wheel's grip limit (+3328 = |p| *
// +9840) and its side force (+8436 along +2820) from the slip curve; the drive / brake share of the body's
// acceleration (+3532, +3536 against the masses) as the wheels' forward force (+8532 along +2916) limited by
// that grip; the sums go to the force +3192 and torque +3276 (the cross product with the arms +6212)
void __fastcall Tyre_Forces(BYTE* s, void*, const float* p)
{
    int n = F(int, s, 3540);
    F(int, s, 6644) = 1;
    F(int, s, 6648) = 1;
    float f[8] = {0};
    for (int i = 0; i < n; i++)
        F(float, s, 3328 + i * 4) = (float)(F(float, s, 9840 + i * 4) * x87_fabs(p[i]));
    float t[3];
    for (int i = 0; i < n; i++) {
        if (!F(WORD, s, 6564 + i * 2)) {
            float g = (float)Tyre_Grip(7, (float)x87_fabs(F(float, s, 6704 + i * 4)), (float)x87_fabs(F(float, s, 3532)));
            f[i] = g * F(float, s, 3328 + i * 4);
            if (!F(int, s, 10088))
                f[i] = f[i] * g_tf674404;
            if (F(float, s, 6500 + i * 4) < g_tf64c250)
                f[i] = f[i] * F(float, s, 6500 + i * 4);
        } else
            f[i] = 0;
        Tf_Scale(s + 8436 + i * 12, s + 2820 + i * 12, f[i]);
        F(BYTE, s, 6596 + i) =
            x87_fabs(F(float, s, 6748 + i * 4)) > g_tf64d9f0 && F(float, s, 6500 + i * 4) > g_tf64b6e0 ? 1 : 0;
    }
    Body_UpdateGrip(s, 0);
    if (F(BYTE, s, 6594))
        F(int, s, 6644) = 0;
    float a = (float)((double)F(float, s, 3532) * F(float, s, 3532) * F(float, s, 6740) * F(float, s, 9792) /
        F(float, s, 8056));
    Tf_Zero(s + 3192);
    Tf_Zero(s + 3108);
    Tf_Zero(s + 3276);
    float b = (float)((double)F(float, s, 6744) * F(float, s, 3536) * F(float, s, 3536) * F(float, s, 9792) /
        F(float, s, 8056));
    float r;
    if (x87_fabs(F(float, s, 6736)) > g_tf64b5a0)
        r = F(float, s, 6740) / F(float, s, 6736);
    else
        r = F(float, s, 8056) / F(float, s, 8032);
    double drv = (double)a * r, brk = (double)r * b;
    if (n == 6)
        brk = brk * g_tf64c258;
    if (n == 8)
        drv = drv * g_tf64c258;
    for (int i = 0; i < n; i++) {
        if (F(WORD, s, 6564 + i * 2))
            continue;
        f[i] = F(int, s, 7124 + i * 4) ? (float)drv : (float)brk;
        float lim = F(float, s, 3328 + i * 4);
        if (lim < x87_fabs(f[i]))
            f[i] = f[i] > g_tf64c264 ? lim : -lim;
    }
    for (int i = 0; i < n; i++) {
        BYTE* fw = s + 8532 + i * 12;
        BYTE* sd = s + 8436 + i * 12;
        BYTE* dir = s + 2916 + i * 12;
        Tf_Zero(fw);
        if (!F(WORD, s, 6564 + i * 2) && !F(int, s, 6612 + i * 4)) {
            Tf_Scale(fw, dir, f[i]);
            if (Tf_Dot(fw, sd) > g_tf64b678) {
                float d1 = (float)Tf_Dot(dir, sd);
                double g = Tyre_Grip(7, (float)x87_fabs(F(float, s, 6704 + i * 4)), (float)x87_fabs(F(float, s, 3532)));
                float d2 = (float)(Tf_Dot(dir, fw) * g);
                double x = d2 - d1;
                if (F(BYTE, s, 6596 + i) || x87_fabs(d2) < x87_fabs(d1))
                    x = g_tf64c264;
                Tf_Scale(fw, dir, (float)x);
            } else
                Tf_Zero(fw);
        }
        CallThis(Tf_AddAssign, s + 3192, t, fw);
        float rv[3];
        Tf_Rotate(s, fw, rv);
        const float* arm = (const float*)(s + 6212 + i * 12);
        float q[3] = {(float)((double)rv[2] * arm[1] - (double)rv[1] * arm[2]),
            (float)((double)rv[0] * arm[2] - (double)rv[2] * arm[0]),
            (float)((double)rv[1] * arm[0] - (double)rv[0] * arm[1])};
        CallThis(Tf_AddAssign, s + 3276, t, q);
        float k = F(float, s, 9792) * g_tf64d7b8;
        CallC(Tf_CheckPoint, sd, g_tf67479c, *(DWORD*)&k);
    }
    Body_UpdateGrip(s, 0);
}

} // extern "C"

// === a wheel against a ground segment (FUN_4fbdd0)

struct Wc_V { float x, y, z; };
extern "C" void Wc_SetThis(); KING_ALIAS("_Wc_SetThis", "@Vec3_SetThis@20")
extern "C" void Wc_Normalized(); KING_ALIAS("_Wc_Normalized", "_Vec3_Normalized")
extern "C" void Wc_Length(); KING_ALIAS("_Wc_Length", "_Vec3_Length")
extern "C" void Wc_DivAssign(); KING_ALIAS("_Wc_DivAssign", "@Vec3_DivAssign@16")
extern "C" void Wc_DivScalar(); KING_ALIAS("_Wc_DivScalar", "_Vec3_DivScalar")
extern "C" void Wc_Dot(); KING_ALIAS("_Wc_Dot", "_Vec3_Dot")
extern "C" void Wc_Sub(); KING_ALIAS("_Wc_Sub", "_Vec3_Sub")
extern "C" void Wc_Add(); KING_ALIAS("_Wc_Add", "_Vec3_Add")
extern "C" void Wc_Scaled(); KING_ALIAS("_Wc_Scaled", "_Vec3_Scaled")
KING_VAR(float, g_wc702630, 702630)
KING_VAR(float, g_wc702634, 702634)
KING_VAR(float, g_wc702638, 702638)
KING_VAR(float, g_wc64d7b8, 64d7b8)
KING_VAR(float, g_wc64c258, 64c258)
KING_VAR(double, g_wc64b658, 64b658)
KING_VAR(double, g_wc64b678, 64b678)

static void Wc_Set(Wc_V* v, float x, float y, float z)
{
    CallThis(Wc_SetThis, v, *(DWORD*)&x, *(DWORD*)&y, *(DWORD*)&z);
}

static double Wc_DotOf(const Wc_V* a, const Wc_V* b)
{
    return CallC<double>(Wc_Dot, a, b);
}

// h + dir * t
static void Wc_Along(Wc_V* out, const Wc_V* h, const Wc_V* dir, float t)
{
    Wc_V a, b;
    CallC(Wc_Scaled, &a, dir, *(DWORD*)&t);
    *out = *CallC<Wc_V*>(Wc_Add, &b, h, &a);
}

extern "C" {

// [FUN_4fbdd0] where body s's wheel k (hub +7356, its axis through frame m) meets the ground segment p0..p1 with
// end normals +4132 / +260 (by the wheel's 248-byte record): the contact to *at and the normal to *n, 1; 0 when the
// contact lies beyond the segment while the wheel faces away
int __fastcall Wheel_SegmentContact(BYTE* s, void*, Wc_V* at, Wc_V* n, const float* m, const Wc_V* p0,
    const Wc_V* p1, int k)
{
    const float* hub = (const float*)(s + 7356 + k * 12);
    Wc_V h, down, q, g, ax, t, dirA, c;
    Wc_Set(&h, (float)((double)m[0] * hub[0] + (double)hub[2] * m[6] + (double)hub[1] * m[3] + m[9]),
        (float)((double)hub[2] * m[7] + (double)hub[1] * m[4] + (double)m[1] * hub[0] + m[10]),
        (float)((double)hub[2] * m[8] + (double)m[2] * hub[0] + (double)hub[1] * m[5] + m[11]));
    Wc_Set(&down, g_wc702630 * g_wc64d7b8, g_wc702634 * g_wc64d7b8, g_wc702638 * g_wc64d7b8);
    Wc_Set(&q, hub[0] - down.x, hub[1] - down.y, hub[2] - down.z);
    Wc_Set(&g, (float)((double)q.y * m[3] + (double)q.z * m[6] + (double)q.x * m[0] + m[9]),
        (float)((double)q.x * m[1] + (double)q.y * m[4] + (double)q.z * m[7] + m[10]),
        (float)((double)q.y * m[5] + (double)q.x * m[2] + (double)q.z * m[8] + m[11]));
    Wc_Set(&ax, g.x - h.x, g.y - h.y, g.z - h.z);
    Wc_V axc = ax;
    dirA = *CallC<Wc_V*>(Wc_Normalized, &t, &axc);
    BYTE* w = s + k * 248;
    Wc_V nA = F(Wc_V, w, 4132), nB = F(Wc_V, w, 260);
    Wc_V d;
    Wc_Set(&d, p1->x - p0->x, p1->y - p0->y, p1->z - p0->z);
    double L = CallC<double>(Wc_Length, &d);
    Wc_V tmp, tmp2;
    if (L < g_wc64b658) {
        BYTE* w2 = s + (k + 1) * 248;
        Wc_Set(&tmp, F(float, w2, 0) - F(float, w, 4120), F(float, w2, 4) - F(float, w, 4124),
            F(float, w2, 8) - F(float, w, 4128));
        Wc_V e = tmp;
        d = *CallC<Wc_V*>(Wc_Normalized, &tmp2, &e);
        Wc_Set(&tmp, nB.x + nA.x, nB.y + nA.y, nB.z + nA.z);
        Wc_Set(&c, tmp.x * g_wc64c258, tmp.y * g_wc64c258, tmp.z * g_wc64c258);
    } else {
        float fl = (float)L;
        CallThis(Wc_DivAssign, &d, &tmp2, *(DWORD*)&fl);
        Wc_V cr;
        Wc_Set(&cr, d.y * nA.z - d.z * nA.y, d.z * nA.x - nA.z * d.x, nA.y * d.x - d.y * nA.x);
        Wc_Set(&tmp, cr.y * d.z - cr.z * d.y, cr.z * d.x - cr.x * d.z, cr.x * d.y - cr.y * d.x);
        Wc_V e = tmp;
        c = *CallC<Wc_V*>(Wc_Normalized, &tmp2, &e);
    }
    Wc_V v;
    if (!(x87_fabs((double)dirA.x * c.x + (double)dirA.z * c.z + (double)dirA.y * c.y) < g_wc64b658)) {
        *n = c;
        double tn = Wc_DotOf(n, CallC<Wc_V*>(Wc_Sub, &tmp, p1, &h));
        float tt = (float)(tn / Wc_DotOf(n, &dirA));
        Wc_Along(at, &h, &dirA, tt);
        Wc_V r = *CallC<Wc_V*>(Wc_Sub, &tmp, p1, at);
        double dd = Wc_DotOf(&d, &r);
        if (!(dd < g_wc64b678) && !(dd > L))
            return Wc_DotOf(&dirA, n) < g_wc64b678 ? 1 : 0;
        const Wc_V* from;
        if (dd < g_wc64b678 && !(dd > L)) {
            *n = nA;
            from = p1;
        } else {
            *n = nB;
            from = p0;
        }
        if (x87_fabs(Wc_DotOf(n, &dirA)) < g_wc64b658) {
            *at = h;
            return 1;
        }
        tn = Wc_DotOf(n, CallC<Wc_V*>(Wc_Sub, &tmp, from, &h));
        tt = (float)(tn / Wc_DotOf(n, &dirA));
        Wc_Along(at, &h, &dirA, tt);
        return Wc_DotOf(&dirA, n) < g_wc64b678 ? 1 : 0;
    }
    if (!((double)c.x * c.x + (double)c.y * c.y + (double)c.z * c.z < g_wc64b658)) {
        *at = *p0;
        *n = c;
        return 1;
    }
    if (L < g_wc64b658) {
        *at = *p0;
    } else {
        Wc_Set(&v, h.x - p0->x, h.y - p0->y, h.z - p0->z);
        int behind0 = (double)v.z * d.z + (double)v.y * d.y + (double)v.x * d.x < g_wc64b678;
        Wc_Set(&v, h.x - p1->x, h.y - p1->y, h.z - p1->z);
        int past1 = (double)v.z * d.z + (double)v.y * d.y + (double)v.x * d.x > g_wc64b678;
        int ahead = Wc_DotOf(&d, CallC<Wc_V*>(Wc_Sub, &tmp, &h, p1)) > g_wc64b678;
        if (!past1 && !behind0)
            *at = h;
        else if (behind0 && ahead)
            *at = *p0;
        else
            *at = *p1;
    }
    float two = 2.0f;
    *n = *CallC<Wc_V*>(Wc_DivScalar, &tmp2, CallC<Wc_V*>(Wc_Add, &tmp, &nA, &nB), *(DWORD*)&two);
    return 1;
}

} // extern "C"

// === limiting a body's velocity and spin changes (FUN_4f4110)

extern "C" void Vl_ResetMotion(); KING_ALIAS("_Vl_ResetMotion", "@Body_ResetMotion@8")
extern "C" void Vl_MulAssign(); KING_ALIAS("_Vl_MulAssign", "@Vec3_MulAssign@16")
KING_VAR(double, g_vl64b678, 64b678)
KING_VAR(double, g_vl64b5b8, 64b5b8)
KING_VAR(float, g_vl64c250, 64c250)
KING_VAR(double, g_vl64b658, 64b658)
KING_VAR(double, g_vl64b738, 64b738)
KING_VAR(float, g_vl64d9d8, 64d9d8)
KING_VAR(double, g_vl64d9d0, 64d9d0)
KING_VAR(double, g_vl64ca48, 64ca48)
KING_VAR(double, g_vl64b670, 64b670)

static Vec3 Vl_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

static Vec3 Vl_Mul(const Vec3& a, double k) { return Vl_Set(a.x * k, a.y * k, a.z * k); }

static Vec3 Vl_Sum(const Vec3& a, const Vec3& b) { return Vl_Set((double)a.x + b.x, (double)a.y + b.y, (double)a.z + b.z); }

// v *= a / (a - b)
static void Vl_Cut(Vec3* v, double a, double b)
{
    Vec3 t;
    CallThis(Vl_MulAssign, v, &t, (float)(a / (a - b)));
}

extern "C" {

// [FUN_4f4110] limits the velocity change *dv and spin change *dw of body s (mass +9792) for impulse e, spin
// q and step k: split along the contact normal frames, the parts that would turn the motion back are cut
void __fastcall Body_LimitChange(BYTE* s, void*, Vec3* dv, Vec3* dw, Vec3* e, const Vec3* q, float k)
{
    if (!s[6580])
    {
        bool done = false;
        if ((double)e->x * e->x + (double)e->y * e->y + (double)e->z * e->z == g_vl64b678)
        {
            double m = F(float, s, 9792);
            double v2 = (double)F(float, s, 3236) * F(float, s, 3236) + (double)F(float, s, 3232) * F(float, s, 3232)
                        + (double)F(float, s, 3228) * F(float, s, 3228);
            if (m * m * g_vl64b5b8 > v2)
            {
                *dv = g_zeroVec;
                CallThis(Vl_ResetMotion, s);
                *e = g_zeroVec;
                done = true;
            }
        }
        if (!done)
        {
            float inv = (float)(g_vl64c250 / F(float, s, 9792));
            Vec3 n, u, t, c, c2;
            Vec3_Normalized(&n, (const Vec3*)(s + 220));
            Vec3_Cross(&c, (const Vec3*)(s + 208), &n);
            Vec3_Normalized(&u, &c);
            Vec3_Cross(&t, &u, &n);
            const float* V = (const float*)(s + 64);
            double d = (double)V[1] * n.y + (double)V[2] * n.z + (double)V[0] * n.x;
            Vec3 v72 = Vl_Set(d * n.x, d * n.y, d * n.z);
            d = (double)V[2] * u.z + (double)V[1] * u.y + (double)u.x * V[0];
            Vec3 v108 = Vl_Set(d * u.x, d * u.y, d * u.z);
            d = (double)V[1] * t.y + (double)V[2] * t.z + (double)V[0] * t.x;
            Vec3 v96 = Vl_Set(d * t.x, d * t.y, d * t.z);
            d = (double)n.z * dv->z + (double)n.y * dv->y + (double)dv->x * n.x;
            Vec3 v144 = Vl_Set(d * n.x, d * n.y, d * n.z);
            d = (double)u.z * dv->z + (double)u.y * dv->y + (double)dv->x * u.x;
            Vec3 v84 = Vl_Set(d * u.x, d * u.y, d * u.z);
            d = (double)dv->x * t.x + (double)t.y * dv->y + (double)t.z * dv->z;
            Vec3 v132 = Vl_Set(d * t.x, d * t.y, d * t.z);
            double kk = (double)inv * k;
            float kf = (float)kk;
            Vec3 v60 = Vl_Sum(v72, Vl_Mul(v144, kk));
            Vec3 v120 = Vl_Sum(v108, Vl_Mul(v84, kf));
            Vec3 v208 = Vl_Sum(v96, Vl_Mul(v132, kf));
            if ((double)v60.y * v72.y + (double)v60.z * v72.z + (double)v60.x * v72.x < g_vl64b678)
                Vl_Cut(&v144, (double)v72.y * n.y + (double)v72.z * n.z + (double)v72.x * n.x,
                       (double)v60.y * n.y + (double)v60.z * n.z + (double)v60.x * n.x);
            if ((double)v120.x * v108.x + (double)v120.y * v108.y + (double)v120.z * v108.z < g_vl64b678)
                Vl_Cut(&v84, (double)v108.z * u.z + (double)v108.y * u.y + (double)v108.x * u.x,
                       (double)v120.z * u.z + (double)v120.y * u.y + (double)v120.x * u.x);
            if ((double)v208.x * v96.x + (double)v208.y * v96.y + (double)v208.z * v96.z < g_vl64b678)
                Vl_Cut(&v132, (double)v96.x * t.x + (double)v96.y * t.y + (double)v96.z * t.z,
                       (double)v208.x * t.x + (double)v208.y * t.y + (double)v208.z * t.z);
            *dv = Vl_Sum(Vl_Sum(v84, v144), v132);
        }
    }
    if (s[6580])
        return;
    const float* M = (const float*)(s + 2304);
    Vec3 v84 = Vl_Set((double)M[3] * q->y + (double)M[6] * q->z + (double)M[0] * q->x,
                      (double)M[4] * q->y + (double)M[7] * q->z + (double)M[1] * q->x,
                      (double)M[5] * q->y + (double)M[8] * q->z + (double)M[2] * q->x);
    Vec3 a = F(Vec3, s, 232);
    if (Vec3_Length((const Vec3*)(s + 232)) < g_vl64b658)
        a = g_axisZ;
    Vec3 w, n, u, t, c;
    Vec3_MulComponents(&w, &a, (const Vec3*)(s + 8008));
    Vec3_Normalized(&n, &w);
    Vec3_Cross(&c, &g_axisY, &n);
    Vec3_Normalized(&u, &c);
    Vec3_Cross(&t, &n, &u);
    Vec3 v120, v96, v108, v144, v168, v72, v156, v48, v24, v36;
    Vec3_Scaled(&v120, &n, (float)Vec3_Dot(&n, &v84));
    Vec3_Scaled(&v96, &u, (float)Vec3_Dot(&u, &v84));
    Vec3_Scaled(&v108, &t, (float)Vec3_Dot(&t, &v84));
    Vec3_Scaled(&v144, &n, (float)Vec3_Dot(dw, &n));
    Vec3_Scaled(&v168, &u, (float)Vec3_Dot(dw, &u));
    Vec3_Scaled(&v72, &t, (float)Vec3_Dot(dw, &t));
    Vec3_Add(&v48, &v120, Vec3_Scaled(&v156, &v144, k));
    Vec3_Add(&v24, &v108, Vec3_Scaled(&v156, &v72, k));
    Vec3_Add(&v36, &v96, Vec3_Scaled(&v156, &v168, k));
    float d1 = (float)Vec3_Dot(&v120, &v120);
    double d2 = Vec3_Dot(&v120, &v48);
    if (-(double)d1 * g_vl64b738 > d2 && d2 > (double)d1 * g_vl64d9d8)
    {
        double r = x87_fabs(F(float, s, 6736)) * F(float, s, 3524) > g_vl64b5b8 ? g_vl64d9d0 / k : g_vl64ca48 / k;
        Vec3_Scaled(&v144, &v120, (float)r);
    }
    if (Vec3_Dot(e, e) < g_vl64b670)
    {
        if (Vec3_Dot(&v36, &v96) < g_vl64b678)
        {
            float a1 = (float)Vec3_Dot(&u, &v96);
            Vl_Cut(&v168, a1, Vec3_Dot(&u, &v36));
        }
        if (Vec3_Dot(&v24, &v108) < g_vl64b678)
        {
            float a1 = (float)Vec3_Dot(&t, &v108);
            Vl_Cut(&v72, a1, Vec3_Dot(&t, &v24));
        }
    }
    Vec3 r1, r2;
    *dw = *Vec3_Add(&r2, Vec3_Add(&r1, &v144, &v168), &v72);
}

} // extern "C"

// === a body's setup before its steps of the frame (FUN_4f7380)

extern "C" void Vp_floor(); KING_ALIAS("_Vp_floor", "_floor")
extern "C" void Vp_FrameTime(); KING_ALIAS("_Vp_FrameTime", "_Game_FrameTime")
extern "C" void Vp_DivAssign(); KING_ALIAS("_Vp_DivAssign", "@Vec3_DivAssign@16")
KING_VAR(double, g_vp64d9f8, 64d9f8)
KING_VAR(double, g_vp64b670, 64b670)
KING_VAR(float, g_vp64da10, 64da10)
KING_VAR(double, g_vp64b6a0, 64b6a0)
KING_VAR(double, g_vp64b5b8, 64b5b8)
KING_VAR(double, g_vp64b930, 64b930)
KING_VAR(double, g_vp64b678, 64b678)
KING_VAR(double, g_vp64da08, 64da08)
KING_VAR(float, g_vp64c264, 64c264)
KING_VAR(double, g_vp64b780, 64b780)

static Vec3 Vp_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// (a x b) x a, with the products in the original's order
static Vec3 Vp_DoubleCross(const float* a, const float* b)
{
    double c1 = (double)a[1] * b[2] - (double)b[1] * a[2];
    double c2 = (double)b[0] * a[2] - (double)a[0] * b[2];
    double c3 = (double)b[1] * a[0] - (double)a[1] * b[0];
    return Vp_Set(c2 * a[2] - c3 * a[1], c3 * a[0] - c1 * a[2], c1 * a[1] - c2 * a[0]);
}

static int Vp_Steps(double t, double rate) { return (int)(CallC<double>(Vp_floor, t * rate) + g_vp64b670); }

extern "C" {

// [FUN_4f7380] prepares body s for the frame's steps: its frames (+8772 .. +8964), contact points of the
// wheels (+9304, +9404), the number and length of the steps (+9520, +9508), the spin limits, and the drag
// force (+9748) and torque (+9760) toward the target +10484; a single step unless full
void __fastcall Body_PrepareSteps(BYTE* s, void*, int full)
{
    Matrix m;
    memcpy(s + 8772, Matrix_Translation(&m, F(Vec3, s, 6116)), 48);
    const float* E = (const float*)(s + 8820);
    memcpy(s + 8820, Matrix_Inverse(&m, (const Matrix*)(s + 8772)), 48);
    memcpy(s + 8868, Matrix_Multiply(&m, (const Matrix*)(s + 2256), (const Matrix*)E), 48);
    memcpy(s + 8916, Matrix_Multiply(&m, (const Matrix*)(s + 2256), (const Matrix*)E), 48);
    memcpy(s + 8964, Matrix_Inverse(&m, (const Matrix*)(s + 8868)), 48);
    Vec3* V = (Vec3*)(s + 9012);
    *V = F(Vec3, s, 2568);
    F(Vec3, s, 9024) = F(Vec3, s, 3132);
    Vec3 t;
    Vec3_RotateBy((Vec3*)(s + 9036), (const Vec3*)(s + 3132), (const Matrix*)(s + 8964));
    F(Vec3, s, 9048) = *Vec3_DivComponents(&t, (const Vec3*)(s + 9036), (const Vec3*)(s + 8008));
    F(Vec3, s, 9096) = g_zeroVec;
    F(Vec3, s, 9120) = g_zeroVec;
    F(Vec3, s, 9132) = g_zeroVec;
    F(Vec3, s, 9108) = F(Vec3, s, 3180);
    F(Vec3, s, 9168) = F(Vec3, s, 3264);
    F(Vec3, s, 9180) = g_zeroVec;
    F(Vec3, s, 9192) = g_zeroVec;
    for (int i = 0; i < F(int, s, 3540); i++)
        F(int, s, 9244 + i * 4) = F(int, s, 3576 + i * 4);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const float* p = (const float*)(s + 100 + i * 12);
        F(Vec3, s, 9404 + i * 12) = Vp_Set((double)E[6] * p[2] + (double)E[3] * p[1] + (double)E[0] * p[0] + E[9],
                                           (double)E[7] * p[2] + (double)E[4] * p[1] + (double)E[1] * p[0] + E[10],
                                           (double)E[8] * p[2] + (double)E[5] * p[1] + (double)E[2] * p[0] + E[11]);
        const float* q = (const float*)(s + 6116 + i * 12);
        F(Vec3, s, 9308 + i * 12) = Vp_Set((double)E[0] * q[0] + (double)q[2] * E[6] + (double)q[1] * E[3] + E[9],
                                           (double)q[2] * E[7] + (double)q[1] * E[4] + (double)E[1] * q[0] + E[10],
                                           (double)E[2] * q[0] + (double)q[2] * E[8] + (double)q[1] * E[5] + E[11]);
    }
    double dt = CallC<double>(Vp_FrameTime);
    F(float, s, 9500) = (float)dt;
    F(int, s, 9520) = Vp_Steps(dt, g_vp64d9f8);
    if ((F(WORD, s, 10048) && F(float, s, 9792) < g_vp64da10) || F(int, s, 10092))
        F(int, s, 9520) = Vp_Steps(F(float, s, 9500), g_vp64b6a0);
    double n = F(int, s, 9520);
    F(float, s, 9516) = (float)n;
    F(float, s, 9508) = F(float, s, 9512) = (float)(F(float, s, 9500) / n);
    if (!full)
    {
        float t0 = F(float, s, 9500);
        int w = F(int, s, 3540);
        F(int, s, 9520) = 1;
        F(float, s, 9512) = t0;
        if (w == 4 && ((F(WORD, s, 6564) && F(WORD, s, 6568)) || (F(WORD, s, 6566) && F(WORD, s, 6570))))
            F(float, s, 9508) = t0;
        if (w == 2 && (F(WORD, s, 6564) || F(WORD, s, 6566)))
            F(float, s, 9508) = t0;
    }
    F(int, s, 9504) = 0;
    if (F(float, s, 3524) < g_vp64b5b8 && F(int, s, 6844))
    {
        Vec3 c = Vp_DoubleCross((const float*)(s + 208), &V->x);
        F(float, s, 3524) = (float)Vec3_Length(&c);
        F(float, s, 9240) = (float)Vec3_Length(V);
    }
    Vec3_RotateBy((Vec3*)(s + 9624), (const Vec3*)(s + 208), (const Matrix*)(s + 8964));
    {
        const float* A = (const float*)(s + 196);
        const float* Z = &g_axisZ.x;
        double c1 = (double)Z[2] * A[1] - (double)Z[1] * A[2];
        double c2 = (double)Z[0] * A[2] - (double)Z[2] * A[0];
        double c3 = (double)Z[1] * A[0] - (double)Z[0] * A[1];
        F(Vec3, s, 9636) = Vp_Set(c2 * A[2] - c3 * A[1], c3 * A[0] - c1 * A[2], c1 * A[1] - c2 * A[0]);
    }
    Vec3* dir = (Vec3*)(s + 9648);
    *dir = g_zeroVec;
    F(Vec3, s, 9684) = F(Vec3, s, 3156);
    double L = Vec3_Length((const Vec3*)(s + 9636));
    F(float, s, 9696) = (float)L;
    if (L > g_vp64b930)
    {
        const float* a = (const float*)(s + 9636);
        *dir = Vp_Set(a[0] / L, a[1] / L, a[2] / L);
    }
    memcpy(s + 9700, &g_identity, 48);
    int off = 0;
    for (int i = 0; i < F(int, s, 3540); i++)
        if (off || !F(WORD, s, 6564 + i * 2))
            off = 1;
    double f0 = off ? (double)F(float, s, 9796) * F(float, s, 3524) * g_vp64da08 : g_vp64b678;
    float f = (float)f0;
    if (f0 > g_vp64b670)
        f = 1.0f;
    Vec3 d = Vp_Set((double)F(float, s, 10484) - F(float, s, 64), (double)F(float, s, 10488) - F(float, s, 68),
                    (double)F(float, s, 10492) - F(float, s, 72));
    Vec3 dn = d;
    float len = (float)Vec3_Length(&d);
    if (len != g_vp64c264)
        CallThis(Vp_DivAssign, &dn, &t, len);
    float side = (float)x87_fabs((double)F(float, s, 2624) * dn.z + (double)F(float, s, 2620) * dn.y + (double)F(float, s, 2616) * dn.x);
    const float* M = (const float*)(s + 8868);
    double gx = -(double)g_axisZ.x, gy = -(double)g_axisZ.y, gz = -(double)g_axisZ.z;
    float wx = (float)(gz * M[6] + gy * M[3] + gx * M[0]);
    float wy = (float)(gz * M[7] + gy * M[4] + gx * M[1]);
    double wz = gz * M[8] + gy * M[5] + gx * M[2];
    double k = (double)f * F(float, s, 9792) * g_vp64b780;
    Vec3 grav = Vp_Set(wx * k, wy * k, wz * k);
    Vec3 v100 = Vp_Set((double)d.x * len, (double)d.y * len, (double)d.z * len);
    double g = ((double)g_vp64b670 - side) * F(float, s, 9804) + (double)side * F(float, s, 9800);
    Vec3 drag = Vp_Set(v100.x * g, v100.y * g, v100.z * g);
    F(Vec3, s, 9748) = Vp_Set((double)drag.x + grav.x, (double)drag.y + grav.y, (double)drag.z + grav.z);
    Vec3 r;
    Vec3_Cross((Vec3*)(s + 9760), (const Vec3*)(s + 9908), Vec3_RotateBy(&r, &drag, (const Matrix*)(s + 8964)));
    F(int, s, 9524) = 1;
}

} // extern "C"

// === a body's integration step (FUN_4fadb0)

extern "C" void Bi_AddAssign(); KING_ALIAS("_Bi_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Bi_SetPosition(); KING_ALIAS("_Bi_SetPosition", "@Body_SetPosition@12")
extern "C" void Bi_SetVelocity(); KING_ALIAS("_Bi_SetVelocity", "@Body_SetVelocity@12")
extern "C" void Bi_NodeInit(); KING_ALIAS("_Bi_NodeInit", "@Node_Init@8")
extern "C" void Bi_Query(); KING_ALIAS("_Bi_Query", "_Query_4e5b22")
extern "C" void Bi_Rebuild(); KING_ALIAS("_Bi_Rebuild", "@Body_Rebuild@4")
extern "C" void Bi_SaveState(); KING_ALIAS("_Bi_SaveState", "@Body_SaveState@8")
extern "C" void Bi_SaveVec88(); KING_ALIAS("_Bi_SaveVec88", "@Body_SaveVec88@8")
KING_VAR(float, g_bi64c258, 64c258)

static void Bi_Add(void* to, const Vec3& v)
{
    Vec3 t;
    CallThis(Bi_AddAssign, to, &t, &v);
}

extern "C" {

// [FUN_4fadb0] integrates body s over dt: the forces (+10244.., +10304..) and their torques about the
// points +10184.., the extra forces +10364/+10424, then the momentum (+9012), spin (+9036), the rolling frame
// +9700 and the frames +8868 / +8964, and the ground under the body
void __fastcall Body_FrameIntegrate(BYTE* s, void*, float dt)
{
    memcpy(s + 8772, &g_identity, 48);
    memcpy(s + 8820, &g_identity, 48);
    F(Vec3, s, 9012) = F(Vec3, s, 2568);
    const float* M = (const float*)(s + 8964);
    memcpy(s + 8964, s + 2352, 48);
    memcpy(s + 8868, s + 2256, 48);
    Vec3 acc = F(Vec3, s, 3156), sum = g_zeroVec;
    for (int i = 0; i < F(int, s, 10176); i++)
    {
        const float* f = (const float*)(s + 10244 + i * 12);
        const float* g = (const float*)(s + 10304 + i * 12);
        Vec3 t;
        t.x = (float)((double)g[0] + f[0]);
        t.y = (float)((double)g[1] + f[1]);
        t.z = (float)((double)f[2] + g[2]);
        Bi_Add(&acc, t);
        double a = (double)M[6] * f[2] + (double)f[0] * M[0] + (double)f[1] * M[3];
        double b = (double)M[1] * f[0] + (double)f[2] * M[7] + (double)f[1] * M[4];
        double c = (double)M[2] * f[0] + (double)f[2] * M[8] + (double)f[1] * M[5];
        const float* p = (const float*)(s + 10184 + i * 12);
        float tx = (float)(c * p[1] - b * p[2]);
        float ty = (float)(a * p[2] - c * p[0]);
        double tz = b * p[0] - a * p[1];
        sum.x = (float)((double)sum.x + tx);
        sum.y = (float)((double)ty + sum.y);
        sum.z = (float)(tz + sum.z);
    }
    for (int i = 0; i < F(int, s, 10180); i++)
    {
        const float* h = (const float*)(s + 10364 + i * 12);
        const float* k = (const float*)(s + 10424 + i * 12);
        Vec3 t;
        t.x = (float)((double)h[0] + k[0]);
        t.y = (float)((double)h[1] + k[1]);
        t.z = (float)((double)h[2] + k[2]);
        Bi_Add(&sum, t);
    }
    double k = (double)dt / F(float, s, 9792);
    Vec3 t;
    t.x = (float)(acc.x * k);
    t.y = (float)(acc.y * k);
    t.z = (float)(acc.z * k);
    Bi_Add(s + 9012, t);
    {
        const float* v = (const float*)(s + 76);
        float* w = (float*)(s + 9036);
        w[0] = (float)((double)M[6] * v[2] + (double)M[3] * v[1] + (double)v[0] * M[0]);
        w[1] = (float)((double)M[7] * v[2] + (double)M[1] * v[0] + (double)M[4] * v[1]);
        w[2] = (float)((double)M[8] * v[2] + (double)M[2] * v[0] + (double)M[5] * v[1]);
    }
    t.x = (float)((double)sum.x * dt);
    t.y = (float)((double)sum.y * dt);
    t.z = (float)((double)sum.z * dt);
    Bi_Add(s + 9036, t);
    Vec3 r;
    F(Vec3, s, 9048) = *Vec3_DivComponents(&r, (const Vec3*)(s + 9036), (const Vec3*)(s + 8008));
    {
        const float* N = (const float*)(s + 8868);
        const float* w = (const float*)(s + 9036);
        float* o = (float*)(s + 9024);
        o[0] = (float)((double)N[6] * w[2] + (double)N[3] * w[1] + (double)w[0] * N[0]);
        o[1] = (float)((double)N[7] * w[2] + (double)N[1] * w[0] + (double)N[4] * w[1]);
        o[2] = (float)((double)N[8] * w[2] + (double)N[2] * w[0] + (double)N[5] * w[1]);
    }
    Vec3 dd;
    dd.x = (float)((double)F(float, s, 9060) * g_bi64c258);
    dd.y = (float)((double)F(float, s, 9064) * g_bi64c258);
    dd.z = (float)((double)F(float, s, 9068) * g_bi64c258);
    Vec3 spin;
    spin.x = (float)((double)F(float, s, 9048) - dd.x);
    spin.y = (float)((double)F(float, s, 9052) - dd.y);
    spin.z = (float)((double)F(float, s, 9056) - dd.z);
    float roll = (float)Vec3_Length(Vec3_Scaled(&r, &spin, dt));
    Matrix look, tr, m1, m2;
    memcpy(s + 9700, Matrix_LookAt(&look, g_zeroVec, spin, roll), 48);
    Matrix_Translation(&tr, *Vec3_Scaled(&r, (const Vec3*)(s + 9012), dt));
    memcpy(s + 8916, s + 8868, 48);
    Matrix_Multiply(&m2, Matrix_Multiply(&m1, (const Matrix*)(s + 9700), (const Matrix*)(s + 8916)), &tr);
    memcpy(s + 8868, &m2, 48);
    memcpy(s + 8964, Matrix_Inverse(&m2, (const Matrix*)(s + 8868)), 48);
    F(Vec3, s, 9624) = *Vec3_RotateBy(&r, (const Vec3*)(s + 208), (const Matrix*)(s + 8964));
    F(Vec3, s, 9036) = *Vec3_RotateBy(&r, (const Vec3*)(s + 9024), (const Matrix*)(s + 8964));
    F(Vec3, s, 9048) = *Vec3_DivComponents(&r, (const Vec3*)(s + 9036), (const Vec3*)(s + 8008));
    F(Vec3, s, 9060) = g_zeroVec;
    CallThis(Bi_SetPosition, s, s + 9012);
    CallThis(Bi_SetVelocity, s, s + 9024);
    memcpy(s + 16, s + 8868, 48);
    BYTE node[208];
    CallThis(Bi_NodeInit, node);
    if (CallC<int>(Bi_Query, F(int, s, 6672), s + 2292, s + 52, node))
        F(int, s, 244) = F(int, node, 104);
    CallThis(Bi_Rebuild, s);
    CallThis(Bi_SaveState, s);
    CallThis(Bi_SaveVec88, s);
}

} // extern "C"

// === the wheels' total force and torque (FUN_4f38f0)

extern "C" float* __cdecl Vec3_DivScalar(float* out, const float* v, float k);
KING_VAR(float, g_wf674404, 674404)

static Vec3 Wf_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_4f38f0] the total force *f and torque *t of body s's wheels (+3540) that touch (word +6564 clear):
// their three forces (+3848, +8436, +8628 per wheel) at the contact points (+6212, lowered by +3944 when
// +10088 is set), else in the body frame +2304 with the spring forces of +8436 scaled down by 674404 squared
void __fastcall Wheels_ForceTorque(BYTE* s, void*, Vec3* f, Vec3* t)
{
    *f = g_zeroVec;
    *t = g_zeroVec;
    const float* R = (const float*)(s + 2304);
    Vec3 tmp;
    if (F(int, s, 10088))
    {
        for (int i = 0; i < F(int, s, 3540); i++)
        {
            Vec3 a = g_zeroVec, b = g_zeroVec, c = g_zeroVec;
            if (!F(WORD, s, 6564 + i * 2))
            {
                c = F(Vec3, s, 8628 + i * 12);
                a = F(Vec3, s, 3848 + i * 12);
                b = F(Vec3, s, 8436 + i * 12);
            }
            double sx = (double)a.x + c.x;
            float sy = (float)((double)a.y + c.y), sz = (float)((double)a.z + c.z);
            Vec3 sum = Wf_Set(b.x + sx, (double)sy + b.y, (double)sz + b.z);
            CallThis(Bi_AddAssign, f, &tmp, &sum);
            Vec3 cr, br;
            Vec3_RotateBy(&cr, &c, (const Matrix*)R);
            Vec3_RotateBy(&br, &b, (const Matrix*)R);
            Vec3 ar = Wf_Set((double)a.x * R[0] + (double)a.y * R[3] + (double)a.z * R[6],
                             (double)a.y * R[4] + (double)a.z * R[7] + (double)a.x * R[1],
                             (double)a.y * R[5] + (double)a.z * R[8] + (double)a.x * R[2]);
            double d = F(float, s, 3944 + i * 4);
            Vec3 dz = Wf_Set(d * g_axisZ.x, d * g_axisZ.y, d * g_axisZ.z);
            const float* q = (const float*)(s + 6212 + i * 12);
            Vec3 p = Wf_Set((double)dz.x + q[0], (double)dz.y + q[1], (double)dz.z + q[2]);
            Vec3 v84 = Wf_Set((double)p.y * cr.z - (double)p.z * cr.y, (double)p.z * cr.x - (double)p.x * cr.z,
                              (double)p.x * cr.y - (double)p.y * cr.x);
            Vec3 v120 = Wf_Set((double)p.y * br.z - (double)p.z * br.y, (double)p.z * br.x - (double)p.x * br.z,
                               (double)p.x * br.y - (double)p.y * br.x);
            Vec3 v96 = Wf_Set((double)ar.z * p.y - (double)ar.y * p.z, (double)ar.x * p.z - (double)ar.z * p.x,
                              (double)ar.y * p.x - (double)ar.x * p.y);
            Vec3 v108 = Wf_Set((double)v84.x + v120.x, (double)v84.y + v120.y, (double)v84.z + v120.z);
            Vec3 v72 = Wf_Set((double)v108.x + v96.x, (double)v108.y + v96.y, (double)v108.z + v96.z);
            CallThis(Bi_AddAssign, t, &tmp, &v72);
        }
        return;
    }
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        Vec3 a = g_zeroVec, b = g_zeroVec, c = g_zeroVec;
        if (!F(WORD, s, 6564 + i * 2))
        {
            a = F(Vec3, s, 8628 + i * 12);
            c = F(Vec3, s, 3848 + i * 12);
            b = F(Vec3, s, 8436 + i * 12);
        }
        Vec3 t1, t2, r1, r2, r3, cr;
        CallThis(Bi_AddAssign, f, &tmp, Vec3_Add(&t2, Vec3_Add(&t1, &a, &c), &b));
        a = *Vec3_RotateBy(&r1, &a, (const Matrix*)R);
        b = *Vec3_RotateBy(&r2, &b, (const Matrix*)R);
        c = *Vec3_RotateBy(&r3, &c, (const Matrix*)R);
        CallThis(Bi_AddAssign, t, &tmp, Vec3_Cross(&cr, (const Vec3*)(s + 6212 + i * 12), &a));
    }
    Vec3 acc = g_zeroVec;
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        Vec3 v = g_zeroVec, r, cr;
        if (!F(WORD, s, 6564 + i * 2))
            v = F(Vec3, s, 8436 + i * 12);
        v = *Vec3_RotateBy(&r, &v, (const Matrix*)R);
        CallThis(Bi_AddAssign, &acc, &tmp, Vec3_Cross(&cr, (const Vec3*)(s + 6212 + i * 12), &v));
    }
    Vec3 q;
    Vec3_DivScalar(&q.x, &acc.x, (float)((double)g_wf674404 * g_wf674404));
    CallThis(Bi_AddAssign, t, &tmp, &q);
}

} // extern "C"

// === the wheels on a flat ground (FUN_4f1fa0)

extern "C" void Wg_LineProject(); KING_ALIAS("_Wg_LineProject", "_Line_Project")
KING_VAR(float, g_wg64c2f8, 64c2f8)
KING_VAR(double, g_wg64d9b8, 64d9b8)
KING_VAR(float, g_wg64c264, 64c264)

// p through body s's frame (+16, position +52)
static void Wg_Frame(BYTE* s, double x, double y, double z, float* o, bool second)
{
    const float* M = (const float*)(s + 16);
    o[0] = (float)(z * M[6] + y * M[3] + x * M[0] + M[9]);
    o[1] = (float)(z * M[7] + y * M[4] + x * M[1] + M[10]);
    o[2] = (float)(second ? x * M[2] + z * M[8] + y * M[5] + M[11] : z * M[8] + y * M[5] + x * M[2] + M[11]);
}

extern "C" {

// [FUN_4f1fa0] puts body s's wheels (+3540) on the ground plane through point p with normal n (reset first
// when reset is set): each wheel's travel line (+7356.., up and down by 64c2f8) meets the plane at its
// contact (wheel +248 records at +248: point, normal, flags) and its spring length (+3544..) is kept in range
void __fastcall Wheels_OnPlane(BYTE* s, void*, int reset, const Vec3* p, const Vec3* n)
{
    F(Vec3, s, 196) = *n;
    F(Vec3, s, 208) = *n;
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        BYTE* W = s + 272 + i * 248;
        WORD* touch = &F(WORD, s, 6564 + i * 2);
        if (reset)
        {
            F(Vec3, s, 3848 + i * 12) = g_zeroVec;
            F(int, s, 3816 + i * 4) = 0;
            F(int, s, 3944 + i * 4) = 0;
            F(DWORD, s, 9872 + i * 4) = 0x3c23d70a;
            F(float, s, 9840 + i * 4) = 0.5f;
        }
        const float* P = (const float*)(s + 7356 + i * 12);
        double z0 = (double)g_axisZ.x * g_wg64c2f8;
        float z1 = (float)((double)g_axisZ.y * g_wg64c2f8), z2 = (float)((double)g_axisZ.z * g_wg64c2f8);
        float t1[3], t2[3];
        Wg_Frame(s, z0 + P[0], (double)z1 + P[1], (double)z2 + P[2], t1, false);
        Wg_Frame(s, P[0] - z0, (double)P[1] - z1, (double)P[2] - z2, t2, true);
        Vec3 dir, u;
        dir.x = (float)((double)t2[0] - t1[0]);
        dir.y = (float)((double)t2[1] - t1[1]);
        dir.z = (float)((double)t2[2] - t1[2]);
        Vec3_Normalized(&u, &dir);
        double c = x87_fabs((double)u.y * n->y + (double)n->x * u.x + (double)n->z * u.z);
        float* spring = &F(float, s, 3544 + i * 4);
        float limit = F(float, s, 8092 + i * 4);
        if (c < g_wg64d9b8)
        {
            F(WORD, W, 2) = 0;
            F(WORD, W, 0) = 0;
            F(Vec3, W, -24) = g_zeroVec;
            F(Vec3, W, -12) = g_zeroVec;
            F(int, W, 108) = 0;
            F(int, W, 216) = 0;
            memcpy(W + 3848, W - 24, 248);
            *touch = 1;
            *spring = limit;
            F(Vec3, s, 100 + i * 12) = F(Vec3, W, -24);
            F(Vec3, s, 6116 + i * 12) = F(Vec3, W, -24);
            continue;
        }
        Vec3 hit;
        F(Vec3, W, -24) = *CallC<const Vec3*>(Wg_LineProject, &hit, p, n, t1, t2);
        F(WORD, W, 0) = 1;
        F(Vec3, W, -12) = *n;
        F(WORD, W, 2) = 0;
        F(int, W, 108) = 0;
        F(int, W, 216) = 0;
        const float* h = (const float*)(W - 24);
        F(Vec3, s, 100 + i * 12) = *(const Vec3*)h;
        const float* G = (const float*)(s + 2304);
        float* g = (float*)(s + 6212 + i * 12);
        g[0] = (float)((double)G[6] * h[2] + (double)G[3] * h[1] + (double)G[0] * h[0] + G[9]);
        g[1] = (float)((double)G[7] * h[2] + (double)G[4] * h[1] + (double)G[1] * h[0] + G[10]);
        g[2] = (float)((double)G[8] * h[2] + (double)G[5] * h[1] + (double)G[2] * h[0] + G[11]);
        *spring = (float)((double)P[2] - F(float, s, 7028 + i * 4) - g[2]);
        *touch = 0;
        if (*spring < g_wg64c264)
            *spring = 0;
        if (*spring > F(float, s, 8092 + i * 4))
        {
            *spring = F(float, s, 8092 + i * 4);
            *touch = 1;
        }
    }
}

} // extern "C"

// === the body's frame vectors for a step (FUN_4ef7c0)

extern "C" void Bv_DivScalar(); KING_ALIAS("_Bv_DivScalar", "_Vec3_DivScalar")
extern "C" void Bv_NormalizeThis(); KING_ALIAS("_Bv_NormalizeThis", "@Vec3_NormalizeThis@8")

static Vec3 Bv_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

extern "C" {

// [FUN_4ef7c0] body s's per-step vectors: the turned push +76 (from +88 by +8008 when +9784 is set), the
// sums cleared, the forward +2604 and its part across the ground normal +208 (+2616), the speed +3520 and
// the velocity's part across it (+2544, its length +3524, its direction +2556) and its forward part +2592
void __fastcall Body_StepVectors(BYTE* s, void*)
{
    const float* M = (const float*)(s + 16);
    if (!F(int, s, 9784))
    {
        F(Vec3, s, 88) = g_zeroVec;
        F(Vec3, s, 76) = g_zeroVec;
    }
    else
    {
        F(float, s, 96) = 0;
        Vec3 t;
        const float* p = &Vec3_MulComponents(&t, &F(Vec3, s, 88), &F(Vec3, s, 8008))->x;
        F(Vec3, s, 76) = Bv_Set((double)M[3] * p[1] + (double)M[6] * p[2] + (double)p[0] * M[0],
                                (double)M[4] * p[1] + (double)M[7] * p[2] + (double)M[1] * p[0],
                                (double)M[5] * p[1] + (double)M[8] * p[2] + (double)M[2] * p[0]);
    }
    F(Vec3, s, 3132) = F(Vec3, s, 76);
    F(Vec3, s, 3108) = g_zeroVec;
    F(Vec3, s, 232) = g_zeroVec;
    F(Vec3, s, 3428) = g_zeroVec;
    F(Vec3, s, 3288) = g_zeroVec;
    F(Vec3, s, 3264) = g_zeroVec;
    memcpy(s + 2400, &g_identity, 48);
    const float* Y = &g_axisY.x;
    Vec3 A = Bv_Set((double)Y[2] * M[6] + (double)Y[1] * M[3] + (double)Y[0] * M[0],
                    (double)Y[2] * M[7] + (double)Y[1] * M[4] + (double)Y[0] * M[1],
                    (double)Y[2] * M[8] + (double)Y[1] * M[5] + (double)Y[0] * M[2]);
    F(Vec3, s, 2604) = A;
    const float* N = (const float*)(s + 208);
    double cx = (double)A.z * N[1] - (double)N[2] * A.y;
    float cy = (float)((double)N[2] * A.x - (double)A.z * N[0]);
    float cz = (float)((double)A.y * N[0] - (double)A.x * N[1]);
    float dz = (float)(cx * N[1] - (double)cy * N[0]);
    float dy = (float)((double)cz * N[0] - cx * N[2]);
    float dx = (float)((double)cy * N[2] - (double)cz * N[1]);
    F(Vec3, s, 2616) = Bv_Set(dx, dy, dz);
    CallThis(Bv_NormalizeThis, s + 2616);
    const float* V = (const float*)(s + 64);
    F(float, s, 3520) = (float)Vec3_Length((const Vec3*)V);
    float ez = (float)((double)N[1] * V[0] - (double)N[0] * V[1]);
    float ey = (float)((double)N[0] * V[2] - (double)N[2] * V[0]);
    float ex = (float)((double)V[1] * N[2] - (double)N[1] * V[2]);
    float fz = (float)((double)ey * N[0] - (double)ex * N[1]);
    float fy = (float)((double)ex * N[2] - (double)ez * N[0]);
    float fx = (float)((double)ez * N[1] - (double)ey * N[2]);
    F(Vec3, s, 2544) = Bv_Set(fx, fy, fz);
    float L = (float)Vec3_Length(&F(Vec3, s, 2544));
    F(float, s, 3524) = L;
    if (L > g_64b5a0)
    {
        Vec3 t;
        F(Vec3, s, 2556) = *CallC<const Vec3*>(Bv_DivScalar, &t, &F(Vec3, s, 2544), L);
    }
    else
        F(Vec3, s, 2556) = F(Vec3, s, 2616);
    float d = (float)Vec3_Dot(&F(Vec3, s, 2544), &F(Vec3, s, 2616));
    F(float, s, 3528) = d;
    Vec3 t;
    F(Vec3, s, 2592) = *Vec3_Scaled(&t, &F(Vec3, s, 2616), d);
}

} // extern "C"

// === putting a body down on the ground (FUN_4f1240)

extern "C" void Pd_Query(); KING_ALIAS("_Pd_Query", "_Query_4e6621")
extern "C" int __fastcall Wheels_FindGround(BYTE* s);

extern "C" {

// [FUN_4f1240] places body s at frame m: the ground under it (looked for up to 2, 9 and 28 above and below)
// takes its wheels, with mode 2 the body turned onto the ground's slope and raised by its springs, with modes
// 1 and 2 the wheels' contacts found again; without ground the wheels sit on a level plane at its springs'
// depth. The frame terms and the body are rebuilt
void __fastcall Body_PutDown(BYTE* s, void*, int mode, int a, const Matrix* m)
{
    F(Vec3, s, 3216) = g_zeroVec;
    F(Vec3, s, 3288) = g_zeroVec;
    F(Vec3, s, 3228) = g_zeroVec;
    F(Vec3, s, 3300) = g_zeroVec;
    const float* M = m->m;
    const float* Z = &g_axisZ.x;
    Vec3 hit = g_zeroVec, n = g_zeroVec;
    BYTE nd[256];
    int r = 0;
    for (int i = 1; i <= 3 && !r; i++)
    {
        double k = i * i * i + 1;
        float ax = (float)(k * Z[0]), ay = (float)(k * Z[1]);
        Vec3Bits up, dn;
        float u[3] = {(float)((double)ax + M[9]), (float)((double)ay + M[10]), (float)(k * Z[2] + M[11])};
        float d[3] = {(float)((double)M[9] - ax), (float)((double)M[10] - ay), (float)((double)M[11] - k * Z[2])};
        memcpy(&up, u, 12);
        memcpy(&dn, d, 12);
        CallThis(FUN_5ef5c0, nd);
        r = CallC<int>(Pd_Query, F(int, s, 10708), a, &up, &dn, nd);
    }
    if (r)
    {
        hit = F(Vec3, nd, 52);
        n = F(Vec3, nd, 64);
        Matrix mc = *m, inv;
        Matrix_Inverse(&inv, m);
        if (mode == 2)
        {
            double h = (double)F(float, s, 7024) - F(float, s, 7364) + (double)g_674414 * F(float, s, 8092);
            float ox = (float)(n.x * h), oy = (float)(n.y * h);
            float px = (float)((double)ox + hit.x), py = (float)((double)oy + hit.y), pz = (float)(n.z * h + hit.z);
            const float* I = inv.m;
            double lx = (double)I[3] * n.y + (double)I[6] * n.z + (double)I[0] * n.x;
            float ly = (float)((double)I[4] * n.y + (double)I[7] * n.z + (double)I[1] * n.x);
            float lz = (float)((double)I[5] * n.y + (double)I[8] * n.z + (double)I[2] * n.x);
            Vec3 c;
            c.z = (float)((double)ly * Z[0] - lx * Z[1]);
            c.y = (float)(lx * Z[2] - (double)lz * Z[0]);
            c.x = (float)((double)lz * Z[1] - (double)ly * Z[2]);
            float ang = (float)Vec3_Length(&c);
            Matrix look, prod;
            Matrix_Multiply(&prod, Matrix_LookAt(&look, g_zeroVec, c, ang), m);
            mc = prod;
            mc.m[9] = px;
            mc.m[10] = py;
            mc.m[11] = pz;
        }
        Body_PlaceReset(s, 0, a, &mc);
        memcpy(s + 2256, s + 16, 48);
        F(int, s, 6672) = F(int, s, 244);
        memcpy(s + 2352, s + 2304, 48);
        Wheels_OnPlane(s, 0, 1, &hit, &n);
        if (mode == 1 || mode == 2)
            Wheels_FindGround(s);
    }
    else
    {
        Body_PlaceReset(s, 0, a, m);
        memcpy(s + 2256, s + 16, 48);
        F(int, s, 6672) = F(int, s, 244);
        memcpy(s + 2352, s + 2304, 48);
        double h = (double)g_674414 * F(float, s, 8092) + F(float, s, 7024) - F(float, s, 7364);
        Vec3 v, d;
        v.z = (float)(h * Z[2]);
        v.y = (float)(h * Z[1]);
        v.x = (float)(h * Z[0]);
        Vec3_Sub(&d, &F(Vec3, s, 52), &v);
        Wheels_OnPlane(s, 0, 1, &d, &g_axisZ);
    }
    Body_FrameTerms(s);
    Body_Rebuild(s);
}

} // extern "C"

// === the wheels' contact frames for a step (FUN_4f2670)

extern "C" void Wk_Negated(); KING_ALIAS("_Wk_Negated", "_Vec3_Negated")
KING_VAR(double, g_wk64b658, 64b658)
KING_VAR(double, g_wk64b670, 64b670)
KING_VAR(double, g_wk64ca48, 64ca48)
KING_VAR(double, g_wk64b678, 64b678)
KING_VAR(float, g_wk64c264, 64c264)

static Vec3 Wk_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// n x v as the code orders it
static Vec3 Wk_NCross(const float* n, const Vec3& v)
{
    return Wk_Set((double)n[1] * v.z - (double)n[2] * v.y, (double)n[2] * v.x - (double)v.z * n[0],
                  (double)v.y * n[0] - (double)n[1] * v.x);
}

extern "C" {

// [FUN_4f2670] body s's wheels' frames for a step: with one body or less (+9784) all along the ground direction d
// (side by the sign of turn), else each wheel's point turned with the body's spin (frames a1 .. a3, velocity v,
// base a4) gives its contact direction (+6404), speed (+6500) and load, its rolling direction (+2628) and the
// angles to the ground normal +208 (+6704, +6748, +6780); the drive terms +3328 from +3360 and +9840
void __fastcall Wheels_StepFrames(BYTE* s, void*, const float* a1, const float* a2, const float* a3, const Vec3* a4,
                                  const Vec3* v, float turn, const Vec3* d)
{
    const float* N = (const float*)(s + 208);
    if (F(int, s, 9784) <= 1)
    {
        double g = turn > g_wk64c264 ? 1 : -1;
        double gt = g * turn;
        for (int i = 0; i < F(int, s, 3540); i++)
        {
            F(Vec3, s, 6308 + i * 12) = *a4;
            F(float, s, 6500 + i * 4) = (float)gt;
            F(float, s, 6532 + i * 4) = (float)(gt * F(float, s, 268 + i * 248));
            Vec3 gd = Wk_Set(g * d->x, g * d->y, g * d->z);
            F(Vec3, s, 6404 + i * 12) = gd;
            F(Vec3, s, 2628 + i * 12) = gd;
            F(Vec3, s, 2724 + i * 12) = *d;
            F(Vec3, s, 2916 + i * 12) = Wk_NCross(N, *d);
            F(Vec3, s, 2820 + i * 12) = Wk_NCross(N, gd);
            F(int, s, 6748 + i * 4) = 0;
            F(float, s, 6780 + i * 4) = 1.0f;
            F(int, s, 6704 + i * 4) = 0;
            F(float, s, 3328 + i * 4) = (float)((double)F(float, s, 9840 + i * 4) * F(float, s, 3360 + i * 4));
        }
        return;
    }
    Vec3 q, k;
    q.x = (float)((double)a3[0] * v->x + (double)a3[3] * v->y + (double)a3[6] * v->z);
    q.y = (float)((double)v->z * a3[7] + (double)a3[4] * v->y + (double)v->x * a3[1]);
    q.z = (float)((double)a3[5] * v->y + (double)v->x * a3[2] + (double)a3[8] * v->z);
    Vec3_MulComponents(&k, &q, &F(Vec3, s, 8020));
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const float* p = (const float*)(s + 260 + i * 248);
        double lx = (double)a3[3] * p[1] + (double)a3[0] * p[0] + (double)a3[6] * p[2];
        float ly = (float)((double)a3[1] * p[0] + (double)p[2] * a3[7] + (double)p[1] * a3[4]);
        float lz = (float)((double)a3[5] * p[1] + (double)a3[8] * p[2] + (double)p[0] * a3[2]);
        float dd = (float)(lx * k.x + (double)lz * k.z + (double)ly * k.y);
        double A = lx * dd, B = (double)ly * dd, C = (double)lz * dd;
        const float* Q = (const float*)(s + 6212 + i * 12);
        float ex = (float)(B * Q[2] - C * Q[1]);
        float ey = (float)(C * Q[0] - A * Q[2]);
        double ez = A * Q[1] - B * Q[0];
        Vec3 w = Wk_Set(ez * a2[6] + (double)ey * a2[3] + (double)ex * a2[0], ez * a2[7] + (double)ey * a2[4] + (double)ex * a2[1],
                        (double)ex * a2[2] + ez * a2[8] + (double)ey * a2[5]);
        Vec3 f = Wk_Set((double)w.z * p[1] - (double)w.y * p[2], (double)w.x * p[2] - (double)w.z * p[0],
                        (double)w.y * p[0] - (double)w.x * p[1]);
        Vec3 gg = Wk_Set((double)f.y * p[2] - (double)f.z * p[1], (double)f.z * p[0] - (double)f.x * p[2],
                         (double)f.x * p[1] - (double)f.y * p[0]);
        Vec3* h = &F(Vec3, s, 6308 + i * 12);
        *h = Wk_Set((double)gg.x + a4->x, (double)gg.y + a4->y, (double)gg.z + a4->z);
        double L = Vec3_Length(h);
        float Lf = (float)L;
        F(float, s, 6500 + i * 4) = Lf;
        F(float, s, 6532 + i * 4) = (float)(L * p[2]);
        Vec3* c6404 = &F(Vec3, s, 6404 + i * 12);
        if (Lf > g_wk64b658)
            *c6404 = Wk_Set(h->x / (double)Lf, h->y / (double)Lf, h->z / (double)Lf);
        else
            *c6404 = *d;
        Vec3* T = &F(Vec3, s, 2628 + i * 12);
        if (F(int, s, 7124 + i * 4))
        {
            if (F(float, s, 7360 + i * 12) > g_wk64c264)
                *T = Wk_Set((double)a1[6] * d->z + (double)d->y * a1[3] + (double)d->x * a1[0],
                            (double)a1[7] * d->z + (double)a1[1] * d->x + (double)a1[4] * d->y,
                            (double)a1[8] * d->z + (double)a1[2] * d->x + (double)a1[5] * d->y);
            else
            {
                Matrix look;
                Vec3 t;
                Matrix_LookAt(&look, g_zeroVec, F(Vec3, s, 208), (float)-(double)F(float, s, 6688));
                *T = *Vec3_RotateBy(&t, d, &look);
            }
        }
        else
            *T = *d;
        F(Vec3, s, 2724 + i * 12) = *T;
        F(Vec3, s, 2916 + i * 12) = Wk_Set((double)N[1] * T->z - (double)T->y * N[2], (double)N[2] * T->x - (double)T->z * N[0],
                                           (double)T->y * N[0] - (double)N[1] * T->x);
        Vec3 tmp, m;
        if (Vec3_Dot(T, c6404) < g_wk64b678)
            *T = *CallC<const Vec3*>(Wk_Negated, &tmp, T);
        Vec3_Cross(&F(Vec3, s, 2820 + i * 12), &F(Vec3, s, 208), T);
        Vec3_Cross(&m, c6404, T);
        float sn = (float)Vec3_Dot(&m, &F(Vec3, s, 208));
        F(float, s, 6748 + i * 4) = sn;
        F(float, s, 6780 + i * 4) = (float)Vec3_Dot(c6404, T);
        if (sn > g_wk64b670)
            F(float, s, 6748 + i * 4) = 1.0f;
        if (F(float, s, 6748 + i * 4) < g_wk64ca48)
            F(float, s, 6748 + i * 4) = -1.0f;
        if (F(float, s, 6748 + i * 4) < g_wk64c264)
            F(Vec3, s, 2820 + i * 12) = *CallC<const Vec3*>(Wk_Negated, &tmp, &F(Vec3, s, 2820 + i * 12));
        F(float, s, 6704 + i * 4) = (float)x87_asin(F(float, s, 6748 + i * 4));
        F(float, s, 3328 + i * 4) = (float)((double)F(float, s, 3360 + i * 4) * F(float, s, 9840 + i * 4));
    }
}

} // extern "C"

// === the wheels' load shares (FUN_4ef480)

KING_VAR(char, s_wl67468c, 67468c)

extern "C" {

// [FUN_4ef480] body s's wheel load shares (+9808) by its wheel count (+3540): the fixed ratios for 3 .. 6 wheels
// (from the hubs +7356), by moments about the contacts (+6212) under gravity in body axes for more; fatal for
// fewer than 3
void __fastcall Wheels_LoadShares(BYTE* s, void*)
{
    double gx = -(double)g_axisZ.x, gy = -(double)g_axisZ.y, gz = -(double)g_axisZ.z;
    const float* M = (const float*)(s + 2304);
    float v[3];
    v[0] = (float)(gz * M[6] + gy * M[3] + gx * M[0]);
    v[1] = (float)(gz * M[7] + gy * M[4] + gx * M[1]);
    v[2] = (float)(gz * M[8] + gy * M[5] + gx * M[2]);
    int n = F(int, s, 3540);
    const BYTE* hubs = s + 7356;
    float* out = (float*)(s + 9808);
    switch (n)
    {
    case 0:
    case 1:
    case 2:
    {
        int type = F(int, s, 2252);
        BYTE* r = (BYTE*)g_carTypeNames + type * 2416;
        char buf[200];
        sprintf(buf, &s_wl67468c, n, type, r + 112, r + 142, r + 172, r + 242);
        World_Fatal(buf);
        return;
    }
    case 3:
        Wheels_Ratios2(hubs, out);
        return;
    case 4:
        Wheels_Ratios4(hubs, out);
        return;
    case 5:
        Wheels_Ratios5(hubs, out);
        return;
    case 6:
        Wheels_Ratios6(hubs, out);
        return;
    default:
        Weights_ByMoments(v, (const float*)(s + 6212), out, n);
        return;
    }
}

} // extern "C"

// === setting a body's mass, frame and wheels (FUN_4ee540)

KING_VAR(Vec3, g_bm6974a8, 6974a8)               // the unit inertia
KING_VAR(AsmLabel, g_bm697a10, 697914)           // car type records (hubs at +252 + 12 i)
KING_VAR(float, g_bm64c300, 64c300)

extern "C" {

// [FUN_4ee540] body s set up with mass m (+9792), the inverse of frame f (+10128) and inertia in (+8008, its
// inverse +8020): the wheel hubs of its type in body axes (+7356, their lengths +7452, copies +13252 / +13348,
// lowered by 64c300 into +6212), the track widths (+8052, +8056), the spring rest +8736, the load shares, the
// weight +3156 and each wheel's load terms (+7484, +8124, +7320); a trailer hitch and a fixed load as set
void __fastcall Body_SetMass(BYTE* s, void*, float m, const Matrix* f, const Vec3* in)
{
    F(float, s, 9792) = m;
    F(Vec3, s, 8008) = *in;
    Vec3 t;
    F(Vec3, s, 8020) = *Vec3_DivComponents(&t, &g_bm6974a8, &F(Vec3, s, 8008));
    Matrix inv;
    Matrix_Inverse(&inv, f);
    memcpy(s + 10128, &inv, 48);
    const float* I = inv.m;
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const float* h = (const float*)((BYTE*)&g_bm697a10 + 252 + F(int, s, 2252) * 2416 + i * 12);
        float* hub = (float*)(s + 7356 + i * 12);
        hub[0] = (float)((double)I[6] * h[2] + (double)I[3] * h[1] + (double)I[0] * h[0] + I[9]);
        hub[1] = (float)((double)I[1] * h[0] + (double)I[7] * h[2] + (double)I[4] * h[1] + I[10]);
        hub[2] = (float)((double)I[2] * h[0] + (double)I[8] * h[2] + (double)I[5] * h[1] + I[11]);
        if (i == F(int, s, 3540) - 1 && F(int, s, 10104) == 2)
            Body_UpdateHitch(s, 0, 0);
        F(float, s, 7452 + i * 4) = (float)Vec3_Length((const Vec3*)hub);
        F(Vec3, s, 13252 + i * 12) = *(const Vec3*)hub;
        F(float, s, 13348 + i * 4) = F(float, s, 7452 + i * 4);
        double zx = g_axisZ.x * (double)g_bm64c300;
        float zy = (float)(g_axisZ.y * (double)g_bm64c300), zz = (float)(g_axisZ.z * (double)g_bm64c300);
        float* low = (float*)(s + 6212 + i * 12);
        low[0] = (float)(hub[0] - zx);
        low[1] = (float)((double)hub[1] - zy);
        low[2] = (float)((double)hub[2] - zz);
    }
    F(float, s, 8052) = (float)x87_fabs(F(float, s, 7360));
    F(float, s, 8056) = (float)x87_fabs(F(float, s, 7384));
    if (F(int, s, 3540) == 6)
        F(float, s, 8056) = (float)x87_fabs(F(float, s, 7408));
    F(float, s, 13488) = F(float, s, 9792);
    F(Vec3, s, 13496) = F(Vec3, s, 8008);
    F(float, s, 8736) = (float)((double)F(float, s, 7024) - F(float, s, 7364) + (double)g_674414 * F(float, s, 8092));
    Wheels_LoadShares(s, 0);
    float gy = (float)-(double)g_axisZ.y, gz = (float)-(double)g_axisZ.z;
    float mm = F(float, s, 9792);
    double wx = -(double)g_axisZ.x * mm;
    float wy = (float)((double)gy * mm), wz = (float)((double)gz * mm);
    F(float, s, 3156) = (float)(wx * g_9_8);
    F(float, s, 3160) = (float)((double)wy * g_9_8);
    F(float, s, 3164) = (float)((double)wz * g_9_8);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        float load = (float)x87_fabs((double)F(float, s, 9808 + i * 4) * F(float, s, 3164));
        double L = F(float, s, 8060 + i * 4);
        double d = L - (double)g_674414 * F(float, s, 8092 + i * 4);
        F(float, s, 8124 + i * 4) = (float)(load / ((d / L + g_bt64c250) * d));
        F(float, s, 7484 + i * 4) = load;
        F(float, s, 7320 + i * 4) = (float)(load * g_64b668);
    }
    if (F(int, s, 10104) == 1)
        Body_SetLoad(s, 0, F(float, s, 10120));
}

} // extern "C"

// === placing a car on the ground under a frame (FUN_4f17a0)

extern "C" void Bp_SaveAll(); KING_ALIAS("_Bp_SaveAll", "@Body_SaveAll@8")
extern "C" void Bp_Terms(); KING_ALIAS("_Bp_Terms", "@Body_FrameTerms@4")
extern "C" void Bp_Rebuild(); KING_ALIAS("_Bp_Rebuild", "@Body_Rebuild@4")
extern "C" void Bp_Place(); KING_ALIAS("_Bp_Place", "@Body_PlaceReset@16")
extern "C" void Bp_OnPlane(); KING_ALIAS("_Bp_OnPlane", "@Wheels_OnPlane@20")
extern "C" void Bp_Step(); KING_ALIAS("_Bp_Step", "@Body_Step@12")
extern "C" void Bp_Ground(); KING_ALIAS("_Bp_Ground", "@Car_PlaceOnGround@12")
extern "C" void Bp_Query(); KING_ALIAS("_Bp_Query", "_Query_4e6621")
extern "C" void Bp_HitObject(); KING_ALIAS("_Bp_HitObject", "_Query_HitObject")
extern "C" void Bp_NodeInit(); KING_ALIAS("_Bp_NodeInit", "@Node_Init@8")
extern "C" void Bp_ToAngles(); KING_ALIAS("_Bp_ToAngles", "_Matrix_ToAngles")
extern "C" void Bp_FromPose(); KING_ALIAS("_Bp_FromPose", "_Matrix_FromPose")
extern "C" void Bp_4fca10(); KING_ALIAS("_Bp_4fca10", "@Body_NextFrame@12")
KING_VAR(Vec3, g_bpUp, 702630)
KING_VAR(Vec3, g_bpEye, 702650)
KING_VAR(float, g_bp674414, 674414)

struct BpMat { Vec3 r0, r1, r2, t; };

extern "C" {

// [FUN_4f17a0] car body b put at frame m (on object o): the ground probed under m's position from heights 2, 9 and
// 28 above and below; where found the body is set on it facing m's heading with its wheels on the plane and its
// frame re-placed on the object found there, else at m with the wheels on a level plane; then rebuilt and stepped
void __fastcall Body_PlaceAtFrame(BYTE* b, void*, int o, const Matrix* m, int)
{
    CallThis(Bp_SaveAll, b);
    F(Vec3, b, 3216) = g_bpEye;
    F(Vec3, b, 3288) = g_bpEye;
    F(Vec3, b, 3228) = g_bpEye;
    F(Vec3, b, 3300) = g_bpEye;
    const Vec3& U = g_bpUp;
    int hit = 0;
    __declspec(align(8)) BYTE node[208];
    for (int k = 1; k <= 3 && !hit; k++)
    {
        double h = k * k * k + 1;
        Vec3 top, bot;
        float ux = (float)(h * U.x), uy = (float)(h * U.y);
        top.x = (float)((double)ux + m->m[9]);
        top.y = (float)((double)uy + m->m[10]);
        top.z = (float)(h * U.z + m->m[11]);
        float dx = (float)(h * U.x), dy = (float)(h * U.y);
        bot.x = (float)((double)m->m[9] - dx);
        bot.y = (float)((double)m->m[10] - dy);
        bot.z = (float)((double)m->m[11] - h * U.z);
        CallThis(Bp_NodeInit, node);
        hit = CallC<int>(Bp_Query, F(int, b, 10708), o, &top, &bot, node);
    }
    if (hit)
    {
        Vec3 P = F(Vec3, node, 52), N = F(Vec3, node, 64);
        Matrix M = *m, I, L, X;
        Matrix_Inverse(&I, m);
        double lift = (double)F(float, b, 7024) - F(float, b, 7364) + (double)g_bp674414 * F(float, b, 8092);
        float nx = (float)(N.x * lift), ny = (float)(N.y * lift);
        Vec3 Q;
        Q.x = (float)((double)nx + P.x);
        Q.y = (float)((double)ny + P.y);
        Q.z = (float)(N.z * lift + P.z);
        const float* R = I.m;
        double ax = (double)R[3] * N.y + (double)R[6] * N.z + (double)R[0] * N.x;
        double ay = (double)R[4] * N.y + (double)R[7] * N.z + (double)R[1] * N.x;
        double az = (double)R[5] * N.y + (double)R[8] * N.z + (double)R[2] * N.x;
        Vec3 d;
        d.x = (float)(az * U.y - ay * U.z);
        d.y = (float)(ax * U.z - az * U.x);
        d.z = (float)(ay * U.x - ax * U.y);
        float len = (float)Vec3_Length(&d);
        Matrix_LookAt(&L, g_bpEye, d, len);
        M = *Matrix_Multiply(&X, &L, m);
        M.m[9] = Q.x;
        M.m[10] = Q.y;
        M.m[11] = Q.z;
        CallThis(Bp_Place, b, o, &M);
        CallThis(Bp_Terms, b);
        CallThis(Bp_OnPlane, b, 1, &P, &N);
        CallThis(Bp_Ground, b, b + 16);
        int cnt = F(int, b, 3540);
        F(BYTE, b, 6586) = 0;
        if (F(int, b, 10104) == 2)
            cnt--;
        for (int i = 0; i < cnt; i++)
            F(BYTE, b, 6586) = (F(BYTE, b, 6586) || F(short, b, 6564 + 2 * i)) ? 1 : 0;
        CallThis(Bp_4fca10, b, &L);
        int obj = CallC<int>(Bp_HitObject, F(int, b, 244), b + 52, &L.m[9]);
        CallThis(Bp_Place, b, obj, &L);
    }
    else
    {
        CallThis(Bp_Place, b, o, m);
        double lift = (double)g_bp674414 * F(float, b, 8092) + F(float, b, 7024) - F(float, b, 7364);
        Vec3 v, d;
        v.x = (float)(lift * U.x);
        v.y = (float)(lift * U.y);
        v.z = (float)(lift * U.z);
        Vec3_Sub(&d, &F(Vec3, b, 52), &v);
        CallThis(Bp_OnPlane, b, 1, &d, &g_bpUp);
    }
    CallThis(Bp_Terms, b);
    CallThis(Bp_Rebuild, b);
    double a[6];
    CallC(Bp_ToAngles, *(BpMat*)(b + 16), &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
    Matrix Y;
    memcpy(b + 16, CallC<const Matrix*>(Bp_FromPose, &Y, a[0], a[1], a[2], a[3], a[4], a[5]), 48);
    CallThis(Bp_Step, b, a);
}

} // extern "C"

#include "x87.h"
// === a vehicle's physical body made from its type's table (FUN_4ebd00, running on through $L_4ed6c8)

extern "C" void Pb_List(); KING_ALIAS("_Pb_List", "@List4eb3b0_Construct@8")
extern "C" void Pb_Engine(); KING_ALIAS("_Pb_Engine", "@Engine_Construct@4")
extern "C" void Pb_History(); KING_ALIAS("_Pb_History", "@History_Construct@24")
extern "C" void Pb_State(); KING_ALIAS("_Pb_State", "@PhysState_Init@8")
extern "C" void Pb_ForEach(); KING_ALIAS("_Pb_ForEach", "_Array_ForEach@16")
extern "C" void Pb_Ramp(); KING_ALIAS("_Pb_Ramp", "@Ramp_Construct@8")
extern "C" void Pb_Range(); KING_ALIAS("_Pb_Range", "@Ramp_SetRange@24")
extern "C" void Pb_RampValue(); KING_ALIAS("_Pb_RampValue", "@Ramp_Value@8")
extern "C" void Pb_NewPart(); KING_ALIAS("_Pb_NewPart", "@Phys4eb3c0_NewPart@16")
extern "C" void Pb_Mass(); KING_ALIAS("_Pb_Mass", "@Body_SetMass@20")
extern "C" void Pb_Shares(); KING_ALIAS("_Pb_Shares", "@Wheels_LoadShares@8")
extern "C" void Pb_SetPos(); KING_ALIAS("_Pb_SetPos", "@Body_SetPosition@12")
extern "C" void Pb_Set88(); KING_ALIAS("_Pb_Set88", "@Body_SetVec88@12")
extern "C" void Pb_EngReset(); KING_ALIAS("_Pb_EngReset", "@Engine_Reset@8")
extern "C" void Pb_Restart(); KING_ALIAS("_Pb_Restart", "@Body_AxisRestart@4")
extern "C" void Pb_PutDown(); KING_ALIAS("_Pb_PutDown", "@Body_PutDown@20")
extern "C" void Pb_Prepare(); KING_ALIAS("_Pb_Prepare", "@Body_PrepareSteps@12")
extern "C" void Pb_Step(); KING_ALIAS("_Pb_Step", "@Body_Step@12")
extern "C" void Pb_AddAssign(); KING_ALIAS("_Pb_AddAssign", "@Vec3_AddAssign@16")
extern "C" void Pb_MulAssign(); KING_ALIAS("_Pb_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Pb_Normalize(); KING_ALIAS("_Pb_Normalize", "@Vec3_NormalizeThis@8")
extern "C" void Pb_Decompose(); KING_ALIAS("_Pb_Decompose", "_Matrix_Decompose6")
extern "C" void Pb_Compose(); KING_ALIAS("_Pb_Compose", "_World_4e3021")
extern "C" void Pb_Parts(); KING_ALIAS("_Pb_Parts", "_Query_Parts2")
extern "C" void Pb_Zero(); KING_ALIAS("_Pb_Zero", "@Vec3_ZeroThis@8")
extern "C" void Pb_Node(); KING_ALIAS("_Pb_Node", "@Node_Init@8")
extern "C" void Pb_Elem(); KING_ALIAS("_Pb_Elem", "@Placed4ee4d0_Construct@4")
KING_VAR(BYTE, g_pbTypes, 697868)
KING_VAR(BYTE, g_pbTypes2144, 69809c)
KING_VAR(BYTE, g_pbFrames, 6d2258)
KING_VAR(int, g_pb6ce33c, 6ce33c)
KING_VAR(int, g_pb6ce340, 6ce340)
KING_VAR(float, g_pb6969f4, 6969f4)
KING_VAR(float, g_pb6969f8, 6969f8)
KING_VAR(float, g_pb6969fc, 6969fc)
KING_VAR(char, g_pbNames, 698166)
KING_VAR(char, g_pbNoOwner, 674510)
KING_VAR(char, g_pbWheels, 6744e8)
KING_VAR(char, g_pbBadWheels, 6744c8)
KING_VAR(char, g_pbNoGround, 6744ac)
KING_VAR(Vec3, g_pb6974a8, 6974a8)
KING_VAR(AsmLabel, g_pbVtbl, 64d994)
extern "C" void g_pbHist1(); KING_ALIAS("_g_pbHist1", "_Window_Parabola")
extern "C" void g_pbHist2(); KING_ALIAS("_g_pbHist2", "_Window_Cosine")
KING_VAR(float, g_pb64c408, 64c408)
KING_VAR(double, g_pb64b7d8, 64b7d8)
KING_VAR(float, g_pb674414, 674414)
KING_VAR(float, g_pb64c264, 64c264)
KING_VAR(double, g_pb64b840, 64b840)
KING_VAR(float, g_pb64d990, 64d990)
KING_VAR(double, g_pb64b838, 64b838)
KING_VAR(double, g_pb64d988, 64d988)
KING_VAR(double, g_pb64b738, 64b738)
KING_VAR(float, g_pb64d980, 64d980)
KING_VAR(double, g_pb64b6e0, 64b6e0)
KING_VAR(double, g_pb64b7b8, 64b7b8)
KING_VAR(float, g_pb64c970, 64c970)
KING_VAR(float, g_pb64c904, 64c904)
KING_VAR(float, g_pb64c258, 64c258)
KING_VAR(float, g_pb64d2f8, 64d2f8)
KING_VAR(double, g_pb64b668, 64b668)
KING_VAR(double, g_pb64d978, 64d978)

// a 248-byte contact record: zero point, normal up (+12), flags (+24, +26), its node (+32)
static void Pb_Contact(BYTE* e)
{
    CallThis(Pb_Zero, e);
    CallThis(Pb_Zero, e + 12);
    CallThis(Pb_Node, e + 32);
    memcpy(e, &g_zeroVec, 12);
    memcpy(e + 12, &g_axisZ, 12);
    F(WORD, e, 24) = 0;
    F(WORD, e, 26) = 0;
    F(int, e, 132) = 0;
    F(int, e, 240) = 0;
}

static void Pb_Zeros(BYTE* s, int from, int to)
{
    memset(s + from, 0, to - from);
}

static void Pb_SetRange(BYTE* r, int i, float a, float b, float c)
{
    CallThis(Pb_Range, r, i, a, b, c);
}

static float Pb_Len(const float* a, const float* b)
{
    float d[3] = {(float)((double)a[0] - b[0]), (float)((double)a[1] - b[1]), (float)((double)a[2] - b[2])};
    return (float)Vec3_Length((const Vec3*)d);
}

static float Pb_Abs(float f)
{
    return (float)x87_fabs((double)f);
}

extern "C" {

// [FUN_4ebd00..$L_4ed6c8] body s of owner o at frame m, of vehicle type `type` (the 2416-byte record at 697868,
// its frame at 6d2258 +344): every part cleared, its wheels (up to 8, the count +3540; 6 checked for a sane
// layout), springs, ramps, mass, extent and inertia set from the record, put down on the ground below m (o's
// parts, fatal without one), then its first step; fatal without an owner or with too many wheels
BYTE* __fastcall PhysBody_Construct(BYTE* s, void*, BYTE* o, Matrix m, int type)
{
    memcpy(s + 16, &g_identity, 48);
    Pb_Zeros(s, 64, 100);
    Pb_Zeros(s, 100, 196);
    Pb_Zeros(s, 196, 244);
    for (int i = 0; i < 8; i++)
        Pb_Contact(s + 248 + 248 * i);
    for (int k = 0; k < 6; k++)
        memcpy(s + 2256 + 48 * k, &g_identity, 48);
    Pb_Zeros(s, 2544, 2628);
    Pb_Zeros(s, 2628, 3312);
    Pb_Zeros(s, 3392, 3452);
    Pb_Zeros(s, 3456, 3516);
    Pb_Zeros(s, 3848, 3944);
    Pb_Zeros(s, 4104, 4116);
    for (int i = 0; i < 8; i++)
    {
        BYTE* e = s + 4120 + 248 * i;
        Pb_Zeros(e, 0, 12);
        CallThis(Pb_Zero, e + 12);
        CallThis(Pb_Node, e + 32);
        memcpy(e, &g_zeroVec, 12);
        memcpy(e + 12, &g_axisZ, 12);
        F(WORD, e, 24) = 0;
        F(WORD, e, 26) = 0;
        F(int, e, 132) = 0;
        F(int, e, 240) = 0;
    }
    Pb_Zeros(s, 6116, 6500);
    Pb_Zeros(s, 6652, 6664);
    Pb_Zeros(s, 6848, 6884);
    memcpy(s + 6888, &g_identity, 48);
    CallThis(Pb_History, s + 6960, g_pbHist1, 0x3F000000u, 0, 0);
    CallThis(Pb_History, s + 6992, g_pbHist2, 0x3F333333u, 0, 0);
    Pb_Zeros(s, 7356, 7452);
    Pb_Zeros(s, 7848, 7944);
    Pb_Zeros(s, 8008, 8032);
    CallThis(Pb_List, s + 8188);
    memcpy(s + 8196, &g_identity, 48);
    Pb_Zeros(s, 8244, 8736);
    CallThis(Pb_State, s + 8740);
    Pb_Zeros(s, 9908, 9920);
    CallThis(Pb_Engine, s + 9984);
    CallStd(Pb_ForEach, s + 10736, 12, 4, Pb_Zero);
    memcpy(s + 10736, &g_identity, 48);
    CallStd(Pb_ForEach, s + 10784, 12, 4, Pb_Zero);
    memcpy(s + 10784, &g_identity, 48);
    Pb_Zeros(s, 10832, 10856);
    CallThis(Pb_Zero, s + 10856);
    CallThis(Pb_Zero, s + 10868);
    CallStd(Pb_ForEach, s + 10880, 12, 8, Pb_Zero);
    CallStd(Pb_ForEach, s + 10976, 12, 8, Pb_Zero);
    CallThis(Pb_Zero, s + 11072);
    CallStd(Pb_ForEach, s + 11088, 248, 8, Pb_Elem);
    CallThis(Pb_Zero, s + 13072);
    CallThis(Pb_Zero, s + 13228);
    CallThis(Pb_Zero, s + 13240);
    CallStd(Pb_ForEach, s + 13252, 12, 8, Pb_Zero);
    CallThis(Pb_Zero, s + 13496);
    CallStd(Pb_ForEach, s + 13584, 208, 8, Pb_Node);
    CallThis(Pb_Zero, s + 15388);
    CallThis(Pb_Zero, s + 15400);
    CallThis(Pb_Zero, s + 15452);
    CallThis(Pb_Zero, s + 15464);
    for (int k = 0; k < 7; k++)
        CallThis(Pb_Ramp, s + 16060 + 52 * k);
    F(void*, s, 0) = &g_pbVtbl;
    if (!o)
    {
        char buf[1024];
        sprintf(buf, &g_pbNoOwner, &g_pbNames + type * 2416, (double)m.m[9], (double)m.m[10], (double)m.m[11],
                (double)g_pb6969f4, (double)g_pb6969f8, (double)g_pb6969fc);
        World_Fatal(buf);
    }

    F(float, s, 8) = (float)Game_Time();
    F(int, s, 12) = 0;
    BYTE L[2416];
    memcpy(L, &g_pbTypes + type * 2416, 2416);
    F(int, s, 2252) = type;
    F(int, s, 9792) = F(int, L, 96);
    memcpy(s + 8008, L + 412, 12);
    Vec3 q;
    memcpy(s + 8020, Vec3_DivComponents(&q, &g_pb6974a8, (const Vec3*)(s + 8008)), 12);
    memcpy(s + 16, &g_identity, 48);
    memcpy(s + 2304, &g_identity, 48);
    Matrix Mt;
    Matrix_Inverse(&Mt, (const Matrix*)(&g_pbFrames + 344 + F(int, s, 2252) * 1488));
    F(int, s, 3540) = F(int, &g_pbTypes2144, F(int, s, 2252) * 2416 + 44);
    if (F(int, s, 2252) >= g_pb6ce33c && F(int, s, 2252) < g_pb6ce340)
    {
        int n = ++F(int, s, 3540);
        if (n >= 8)
        {
            char buf[612];
            wsprintfA(buf, &g_pbWheels, n, 8);
            World_Fatal(buf);
        }
    }
    F(int, s, 13120) = F(int, s, 3540);
    const float* mt = Mt.m;
    for (int i = 0; i < 8; i++)
    {
        const float* p = (const float*)(L + 748) + i;
        const float* v = (const float*)(L + 424 + 12 * i);
        float* w = (float*)(s + 7356 + 12 * i);
        F(float, s, 8060 + 4 * i) = (float)((double)p[0] * g_pb64c408 * g_pb64b7d8);
        F(float, s, 8092 + 4 * i) = p[0];
        F(float, s, 8124 + 4 * i) = p[8];
        w[0] = (float)((double)mt[6] * v[2] + (double)mt[3] * v[1] + (double)mt[0] * v[0] + mt[9]);
        w[1] = (float)((double)mt[7] * v[2] + (double)mt[4] * v[1] + (double)mt[1] * v[0] + mt[10]);
        w[2] = (float)((double)mt[8] * v[2] + (double)mt[5] * v[1] + (double)mt[2] * v[0] + mt[11]);
        double t = (double)g_pb674414 * p[0] + F(float, L, 100);
        float ax = (float)(t * g_axisZ.x), ay = (float)(t * g_axisZ.y);
        float* u = (float*)(s + 6212 + 12 * i);
        u[0] = (float)((double)w[0] - ax);
        u[1] = (float)((double)w[1] - ay);
        u[2] = (float)((double)w[2] - t * g_axisZ.z);
        F(int, s, 7484 + 4 * i) = ((const int*)p)[16];
    }
    if (F(int, s, 3540) == 6)
    {
        const float* w = (const float*)(s + 7356);
        float k = g_pb64c264;
        if (w[1] < k || w[4] < k || w[7] > k || w[10] > k || w[13] > k || w[16] > k || w[0] > k || w[3] < k ||
            w[6] > k || w[9] < k || w[12] > k || w[15] < k ||
            x87_fabs((double)w[1]) * g_pb64b840 < x87_fabs((double)w[4] - w[1]) ||
            x87_fabs((double)w[7]) * g_pb64b840 < x87_fabs((double)w[10] - w[7]) ||
            x87_fabs((double)w[13]) * g_pb64b840 < x87_fabs((double)w[16] - w[13]))
            World_Fatal(&g_pbBadWheels);
    }

    double nx = -(double)g_axisZ.x;
    float ny = -g_axisZ.y, nz = -g_axisZ.z;
    float* d3392 = (float*)(s + 3392);
    d3392[0] = (float)(nx * g_pb64d990);
    d3392[1] = (float)((double)ny * g_pb64d990);
    d3392[2] = (float)((double)nz * g_pb64d990);
    {
        double f = F(float, s, 9792);
        F(float, s, 3156) = (float)(f * d3392[0]);
        F(float, s, 3160) = (float)(f * d3392[1]);
        F(float, s, 3164) = (float)(f * d3392[2]);
    }
    float l192 = F(float, L, 192);
    F(float, s, 9788) = F(float, s, 3452) = Pb_Abs(F(float, s, 3400));
    F(int, s, 16108) = 3;
    Pb_SetRange(s + 16060, 0, (float)(l192 * g_pb64b838), l192, 1.0f);
    Pb_SetRange(s + 16060, 1, (float)(l192 * g_pb64b738), (float)(l192 * g_pb64d988), 1.0f);
    float l108 = F(float, L, 108), l112 = F(float, L, 112);
    F(int, s, 16160) = 2;
    Pb_SetRange(s + 16112, 0, 0, l108, l112);
    Pb_SetRange(s + 16112, 1, 0, (float)((double)l108 + l108), (float)((double)l112 + l112));
    F(float, s, 15480) = (float)CallThis<double>(Pb_RampValue, s + 16112);
    F(int, s, 15484) = 0;
    F(BYTE*, s, 8192) = s;
    for (int k = 0; k < 4; k++)
        CallThis(Pb_NewPart, s + 8188, 300.0f, k);
    CallThis(Pb_Mass, s, F(float, L, 96), &g_pbFrames + 344 + F(int, s, 2252) * 1488, L + 412);
    memcpy(s + 9796, L + 172, 12);
    memcpy(s + 9908, &g_axisZ, 12);

    float minx = 100.0f, maxx = -100.0f, miny = 100.0f;
    double maxy = g_pb64d980;
    for (int i = 0; i < 8; i++)
    {
        const float* w = (const float*)(s + 7356 + 12 * i);
        if (w[0] < minx)
            minx = w[0];
        if (!(w[0] <= maxx))
            maxx = w[0];
        if (w[1] < miny)
            miny = w[1];
        if (maxy < w[1])
            maxy = w[1];
    }
    {
        double wd = (double)maxx - minx;
        float wf = (float)wd;
        double qd = wd * wd * g_pb64b6e0 * g_pb64b7b8;
        float qf = (float)qd;
        F(float, s, 9800) = (float)(qd * F(float, s, 9800));
        double h = maxy - miny;
        F(float, s, 9804) = (float)(h * qf / wf * F(float, s, 9804));
        double c = h * g_pb64c970 + (double)wf * g_pb64c904;
        F(float, s, 9908) = (float)(c * g_axisZ.x);
        F(float, s, 9912) = (float)(c * g_axisZ.y);
        F(float, s, 9916) = (float)(c * g_axisZ.z);
    }
    F(int, s, 6668) = 0x3FCCCCCD;
    F(int, s, 7024) = F(int, L, 100);
    for (int i = 0; i < 8; i++)
    {
        F(int, s, 7028 + 4 * i) = F(int, s, 7024);
        F(int, s, 7060 + 4 * i) = F(int, L, 104);
        float len = (float)Vec3_Length((const Vec3*)(s + 7356 + 12 * i));
        F(float, s, 7452 + 4 * i) = len;
        if (!(len <= F(float, s, 6668)))
            F(float, s, 6668) = len;
    }
    {
        const float* w0 = (const float*)(s + 7356);
        const float* w1 = w0 + 3;
        const float* w2 = w0 + 6;
        const float* w3 = w0 + 9;
        if (F(int, s, 3540) >= 4)
        {
            F(float, s, 8036) = Pb_Len(w2, w0);
            F(float, s, 8040) = Pb_Len(w0, w1);
            F(float, s, 8044) = Pb_Len(w1, w3);
            F(float, s, 8048) = Pb_Len(w3, w2);
            float b = Pb_Len(w1, w3);
            F(float, s, 8032) = (float)(((double)Pb_Len(w0, w2) + b) * g_pb64c258);
            F(float, s, 8052) = Pb_Abs(w0[1]);
            F(float, s, 8056) = Pb_Abs(w2[1]);
            if (F(int, s, 3540) == 6)
            {
                const float* w4 = w0 + 12;
                const float* w5 = w0 + 15;
                float b6 = Pb_Len(w1, w5);
                F(float, s, 8032) = (float)(((double)Pb_Len(w0, w4) + b6) * g_pb64c258);
                F(float, s, 8056) = Pb_Abs(w4[1]);
            }
        }
        else
        {
            float d = Pb_Len(w0, w1);
            for (int k = 0; k < 5; k++)
                F(float, s, 8032 + 4 * k) = d;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        F(int, s, 7092 + 4 * i) = F(int, L, 300 + 4 * i);
        F(int, s, 7156 + 4 * i) = F(int, L, 364 + 4 * i);
        F(int, s, 7124 + 4 * i) = F(int, L, 332 + 4 * i);
    }
    F(int, s, 7196) = 1;
    F(int, s, 7200) = -1;
    F(int, s, 7188) = 1;
    F(int, s, 7192) = F(int, L, 144);
    F(int, s, 15324) = F(int, L, 144);
    F(int, s, 7296) = F(int, L, 204);
    for (short i = 0; i <= F(int, s, 7192); i++)
        F(float, s, 7236 + 4 * i) = (float)((double)F(float, L, 208 + 4 * i) * F(float, s, 7296));
    F(int, s, 7300) = 0;
    F(int, s, 7304) = F(int, L, 268);
    F(int, s, 7308) = F(int, L, 196);
    Pb_SetRange(s + 16372, 0, 0, 0, 0);
    Pb_SetRange(s + 16372, 1, (float)(F(float, s, 7308) * g_pb64b738),
                (float)((double)F(float, s, 7308) + F(float, s, 7308)), 0);
    Pb_SetRange(s + 16320, 0, 1.0f, 1.0f, 1.0f);
    Pb_SetRange(s + 16320, 1, 0.8f, 0.8f, 1.0f);
    F(int, s, 7316) = 0;
    F(float, s, 7352) = 1.0f;
    F(int, s, 7224) = 0x3DCCCCCD;
    F(float, s, 7216) = 0.5f;
    F(int, s, 7220) = 0x3BA3D70A;
    F(float, s, 7228) = (float)Game_Time();
    F(float, s, 7232) = 0.5f;
    F(int, s, 7212) = 0;
    F(int, s, 7204) = 0;
    Pb_SetRange(s + 16216, 0, 0.1f, 1.0f, 60.0f);
    Pb_SetRange(s + 16216, 1, 0.5f, 1.0f, 120.0f);
    Pb_SetRange(s + 16268, 0, 0, 1.0f, 10.0f);
    Pb_SetRange(s + 16268, 1, 0, 1.0f, 20.0f);
    Pb_SetRange(s + 16164, 0, 0.6f, 1.0f, 30000.0f);
    Pb_SetRange(s + 16164, 1, 0.8f, 1.0f, 60000.0f);
    Pb_SetRange(s + 16164, 2, 0.8f, 1.0f, 60000.0f);
    F(int, s, 15444) = 0;
    F(BYTE*, s, 6884) = o;
    F(BYTE*, s, 244) = o;
    F(BYTE*, s, 6672) = o;
    memcpy(s + 16, &m, 48);

    // the ground below: a ray from twice up to K down from the place, against o's parts
    __declspec(align(8)) BYTE node[208];
    CallThis(Pb_Node, node);
    memcpy(node + 4, &g_identity, 48);
    {
        double ux = (double)g_axisZ.x + g_axisZ.x;
        float uy = (float)((double)g_axisZ.y + g_axisZ.y), uz = (float)((double)g_axisZ.z + g_axisZ.z);
        F(float, node, 76) = (float)(ux + F(float, s, 52));
        F(float, node, 80) = (float)((double)uy + F(float, s, 56));
        F(float, node, 84) = (float)((double)uz + F(float, s, 60));
        double dx = (double)g_axisZ.x * g_pb64d2f8;
        float dy = (float)((double)g_axisZ.y * g_pb64d2f8), dz = (float)((double)g_axisZ.z * g_pb64d2f8);
        F(float, node, 88) = (float)(F(float, s, 52) - dx);
        F(float, node, 92) = (float)((double)F(float, s, 56) - dy);
        F(float, node, 96) = (float)((double)F(float, s, 60) - dz);
    }
    F(int, node, 116) = 0;
    F(int, node, 120) = 0;
    F(int, node, 0) = 0;
    F(int, node, 104) = 0;
    int r;
    if (F(WORD, s, 9992))
        r = CallC<int>(Pb_Parts, o, 4, node);
    else
    {
        r = CallVirt<int>(o, 52, 4, node);
        if ((r & 2) && !F(int, node, 104))
        {
            F(int, s, 15444) = 1;
            World_Fatal(&g_pbNoGround);
        }
    }
    if (r & 1)
    {
        double t = ((double)F(float, s, 7024) - F(float, s, 7364)) + (double)g_pb674414 * F(float, s, 8092);
        float ax = (float)(F(float, node, 64) * t), ay = (float)(F(float, node, 68) * t);
        double az = F(float, node, 72) * t;
        F(float, s, 52) = (float)((double)F(float, node, 52) + ax);
        F(float, s, 56) = (float)((double)ay + F(float, node, 56));
        F(float, s, 60) = (float)(az + F(float, node, 60));
    }

    memcpy(s + 6888, s + 16, 48);
    memcpy(s + 2256, s + 16, 48);
    Matrix Mi;
    Matrix* inv = Matrix_Inverse(&Mi, (const Matrix*)(s + 16));
    memcpy(s + 2304, inv, 48);
    memcpy(s + 2352, inv, 48);
    memcpy(s + 196, &g_zeroVec, 12);
    const float* M = (const float*)(s + 16);
    for (int i = 0; i < 8; i++)
    {
        BYTE* e = s + 248 + 248 * i;
        float up[3] = {0, 0, 1.0f};
        memcpy(e + 12, up, 12);
        float tmp[3];
        CallThis(Pb_AddAssign, s + 196, tmp, e + 12);
        float* c = &F(float, s, 3544 + 4 * i);
        c[0] = (float)((double)g_pb674414 * F(float, s, 8092 + 4 * i));
        c[8] = c[0];
        c[16] = c[0];
        const float* w = (const float*)(s + 7356 + 12 * i);
        double t = (double)c[0] + F(float, s, 7024);
        float ax = (float)(t * g_axisZ.x), ay = (float)(t * g_axisZ.y);
        double az = t * g_axisZ.z;
        double x = (double)M[3] * w[1] + (double)M[6] * w[2] + (double)w[0] * M[0] + M[9];
        float y = (float)((double)M[4] * w[1] + (double)M[1] * w[0] + (double)M[7] * w[2] + M[10]);
        float z = (float)((double)M[5] * w[1] + (double)M[2] * w[0] + (double)M[8] * w[2] + M[11]);
        F(float, e, 0) = (float)(x - ax);
        F(float, e, 4) = (float)((double)y - ay);
        F(float, e, 8) = (float)((double)z - az);
        F(WORD, e, 24) = 1;
        F(WORD, e, 26) = 0;
        memcpy(s + 4120 + 248 * i, e, 248);
        double u = (double)F(float, s, 7028 + 4 * i) + c[0];
        float cx = (float)(u * g_axisZ.x), cy = (float)(u * g_axisZ.y);
        float r0 = (float)((double)w[0] - cx), r1 = (float)((double)w[1] - cy);
        double r2 = (double)w[2] - u * g_axisZ.z;
        float* g = (float*)(s + 100 + 12 * i);
        g[0] = (float)(r2 * M[6] + (double)r1 * M[3] + (double)r0 * M[0] + M[9]);
        g[1] = (float)((double)r0 * M[1] + r2 * M[7] + (double)r1 * M[4] + M[10]);
        g[2] = (float)((double)r0 * M[2] + r2 * M[8] + (double)r1 * M[5] + M[11]);
        memcpy(s + 6116 + 12 * i, g, 12);
        F(WORD, s, 6564 + 2 * i) = 0;
        F(int, s, 3708 + 4 * i) = 0;
        F(int, s, 3676 + 4 * i) = 0;
        F(int, s, 3644 + 4 * i) = 0;
        F(int, s, 15248 + 4 * i) = 0;
        F(int, s, 4040 + 4 * i) = 0;
        F(int, s, 3976 + 4 * i) = 0;
        F(int, s, 13684 + 208 * i) = 0;
    }
    {
        float* a = F(float, s, 3616) < F(float, s, 3620) ? &F(float, s, 3620) : &F(float, s, 3616);
        float* b = F(float, s, 3608) < F(float, s, 3612) ? &F(float, s, 3612) : &F(float, s, 3608);
        F(float, s, 3640) = *b < *a ? *a : *b;
    }
    CallThis(Pb_Normalize, s + 196);
    memcpy(s + 208, s + 196, 12);
    memcpy(s + 7516, s + 7024, 492);
    CallThis(Pb_Shares, s);
    for (short i = 0; i < F(int, s, 3540); i++)
    {
        double v = -((double)F(float, s, 3400) * F(float, s, 9808 + 4 * i) * F(float, s, 9792));
        F(float, s, 3360 + 4 * i) = (float)v;
        float w = (float)(v * g_pb64b668);
        F(float, s, 7320 + 4 * i) = w;
        F(float, s, 7812 + 4 * i) = w;
    }
    for (short i = 0; i < F(int, s, 3540); i++)
    {
        double a = F(float, s, 8092 + 4 * i) - x87_fabs((double)F(float, s, 3360 + 4 * i) / F(float, s, 8124 + 4 * i));
        F(float, s, 3544 + 4 * i) = (float)a;
        if (a < g_pb64c264)
            F(int, s, 3544 + 4 * i) = 0;
        else if (!(a <= F(float, s, 8092 + 4 * i)))
            F(int, s, 3544 + 4 * i) = F(int, s, 8092 + 4 * i);
    }
    static const int zeroVecs[] = {64, 220, 88, 232, 76, 3120, 3108, 3192, 3468};
    for (int k = 0; k < 9; k++)
        Pb_Zeros(s, zeroVecs[k], zeroVecs[k] + 12);
    F(int, s, 3516) = 0x5014F9F9;
    Pb_Zeros(s, 3404, 3416);
    F(int, s, 3536) = 0;
    F(int, s, 3532) = 0;
    F(int, s, 3520) = 0;
    F(int, s, 3524) = 0;
    {
        const float* y = &g_axisY.x;
        F(float, s, 2604) = (float)((double)y[2] * M[6] + (double)y[1] * M[3] + (double)y[0] * M[0]);
        F(float, s, 2608) = (float)((double)y[2] * M[7] + (double)y[1] * M[4] + (double)y[0] * M[1]);
        F(float, s, 2612) = (float)((double)y[2] * M[8] + (double)y[1] * M[5] + (double)y[0] * M[2]);
    }
    CallThis(Pb_SetPos, s, &g_zeroVec);
    CallThis(Pb_Set88, s, &g_zeroVec);
    F(int, s, 3808) = 0;
    F(int, s, 3812) = 0;
    CallThis(Pb_EngReset, s + 9984);
    F(int, s, 6940) = 0;
    F(int, s, 6936) = 0;
    F(int, s, 6944) = 0;
    F(float, s, 6104) = (float)Game_Time();
    F(float, s, 6108) = (float)Game_Time();
    F(float, s, 6112) = (float)Game_Time();
    F(int, s, 10720) = 0;
    F(int, s, 10716) = 0;
    F(int, s, 10712) = 0x3D4CCCCD;
    F(int, s, 10728) = 0;
    F(int, s, 13492) = F(int, s, 7024);
    F(int, s, 13124) = 0;
    F(int, s, 13516) = 0;
    F(int, s, 15384) = 0;
    F(int, s, 13112) = 0;
    F(int, s, 13116) = 0;
    for (int i = 0; i < 8; i++)
    {
        F(int, s, 13520 + 4 * i) = 0;
        F(int, s, 13552 + 4 * i) = F(int, &g_pbTypes, F(int, s, 2252) * 2416 + (i / 2 ? 188 : 184));
    }
    F(int, s, 15332) = 1;
    F(int, s, 15376) = 0;
    F(int, s, 15380) = 0;
    Pb_Zeros(s, 15412, 15428);
    F(int, s, 15320) = 1;
    F(int, s, 15316) = 0;
    F(int, s, 15312) = 0;
    F(int, s, 15440) = 0;
    F(int, s, 15488) = 0;
    F(int, s, 6948) = 0;
    Pb_Zeros(s, 15504, 15516);
    F(float, s, 6952) = 0.5f;
    F(int, s, 6956) = 0;
    for (int i = 0; i < 8; i++)
    {
        F(float, s, 9840 + 4 * i) = 0.5f;
        F(float, s, 15280 + 4 * i) = 0.5f;
    }
    F(float, s, 9904) = 0.5f;
    F(int, s, 6676) = 0;
    F(int, s, 6680) = 0;
    Pb_Zeros(s, 6688, 6700);
    F(int, s, 6744) = 0;
    F(int, s, 6740) = 0;
    F(int, s, 6736) = 0;
    for (int i = 0; i < 8; i++)
    {
        int rr = rand();
        float f = (float)(rr * g_pb64d978);
        F(float, s, 3740 + 4 * i) = f;
        F(int, s, 3772 + 4 * i) = 0;
        F(float, s, 13164 + 4 * i) = f;
        F(int, s, 6704 + 4 * i) = 0;
        F(int, s, 13196 + 4 * i) = 0;
        F(float, s, 6780 + 4 * i) = 1.0f;
        F(int, s, 6748 + 4 * i) = 0;
    }
    const float* n = (const float*)(s + 208);
    for (int i = 0; i < 8; i++)
    {
        memcpy(s + 6308 + 12 * i, s + 2544, 12);
        F(float, s, 6500 + 4 * i) = F(float, s, 3528);
        F(float, s, 6532 + 4 * i) = F(float, s, 3528);
        memcpy(s + 6404 + 12 * i, s + 2616, 12);
        memcpy(s + 2628 + 12 * i, s + 2616, 12);
        memcpy(s + 2724 + 12 * i, s + 2616, 12);
        const float* v = (const float*)(s + 2628 + 12 * i);
        float c[3] = {(float)((double)n[1] * v[2] - (double)n[2] * v[1]),
                      (float)((double)v[0] * n[2] - (double)v[2] * n[0]),
                      (float)((double)n[0] * v[1] - (double)n[1] * v[0])};
        memcpy(s + 2820 + 12 * i, c, 12);
        memcpy(s + 2916 + 12 * i, c, 12);
        F(int, s, 6748 + 4 * i) = 0;
        F(float, s, 6780 + 4 * i) = 1.0f;
        F(int, s, 6704 + 4 * i) = 0;
        F(float, s, 3328 + 4 * i) = (float)((double)F(float, s, 9840 + 4 * i) * F(float, s, 3360 + 4 * i));
    }
    float dec[12];
    float mc[12];
    CallC(Pb_Decompose, s + 16, dec);
    memcpy(s + 16, CallC<void*>(Pb_Compose, mc, dec), 48);
    memcpy(s + 8196, &g_identity, 48);
    for (int i = 0; i < 27; i++)
    {
        BYTE* q2 = s + 15520 + 20 * i;
        F(int, q2, 0) = 0;
        F(int, q2, 12) = 0;
        F(int, q2, 16) = 0x3C8EFA89;
        F(int, q2, 8) = 0;
        F(int, q2, 4) = 1;
    }
    for (int i = 0; i < 8; i++)
    {
        F(int, s, 15744 + 20 * i) = 2;
        F(int, s, 15748 + 20 * i) = -2;
        F(int, s, 15584 + 20 * i) = 3;
        F(int, s, 15596 + 20 * i) = 0x3D4CCCCD;
    }
    F(int, s, 15556) = 1010174817;
    F(int, s, 15544) = 2;
    F(int, s, 15536) = 0x3C8EFA89;
    F(int, s, 15524) = 3;
    F(int, s, 15564) = 3;
    F(int, s, 15568) = -3;
    for (int i = 0; i < 8; i++)
        F(int, s, 15904 + 20 * i) = 1;
    CallThis(Pb_Restart, s);
    CallThis(Pb_SetPos, s, &g_zeroVec);
    CallThis(Pb_Set88, s, &g_zeroVec);
    CallThis(Pb_PutDown, s, 2, F(BYTE*, s, 6884), s + 6888);
    if (F(WORD, s, 272))
    {
        double a = (double)F(float, s, 52) - F(float, s, 248);
        double b = (double)F(float, s, 56) - F(float, s, 252);
        double c = (double)F(float, s, 60) - F(float, s, 256);
        F(float, s, 8736) = (float)(a * F(float, s, 260) + (c * F(float, s, 268) + b * F(float, s, 264)));
    }
    memcpy(s + 8724, &g_zeroVec, 12);
    float tmp[3];
    for (short i = 0; i < F(int, s, 3540); i++)
        CallThis(Pb_AddAssign, s + 8724, tmp, s + (i + 1) * 248);
    CallThis(Pb_MulAssign, s + 8724, tmp, 0.25f);
    F(int, s, 3312) = 0;
    F(int, s, 3316) = 0;
    F(float, s, 3320) = 1000.0f;
    F(float, s, 3324) = 100.0f;
    memcpy(s + 3216, &g_zeroVec, 12);
    memcpy(s + 3288, &g_zeroVec, 12);
    memcpy(s + 3228, &g_zeroVec, 12);
    memcpy(s + 3300, &g_zeroVec, 12);
    F(int, s, 9784) = 1;
    F(int, s, 9780) = -1;
    F(int, s, 9776) = 0;
    CallThis(Pb_Prepare, s, 1);
    CallThis(Pb_Step, s, dec);
    F(int, s, 15448) = 0;
    memcpy(s + 15452, &g_zeroVec, 12);
    F(int, s, 15492) = 0;
    F(int, s, 15496) = 0;
    for (int i = 0; i < 8; i++)
    {
        F(int, s, 9920 + 4 * i) = 0;
        F(int, s, 9952 + 4 * i) = 0;
    }
    F(int, s, 15500) = 0;
    return s;
}

} // extern "C"

// === one step of a body's physics state machine (FUN_4fef20 with its tail $L_4ff139)

extern "C" void FUN_4f5820(); KING_ALIAS("_FUN_4f5820", "@Body_StartSteps@4")
extern "C" void FUN_4fb470(); KING_ALIAS("_FUN_4fb470", "@Body_EndSteps@12")
extern "C" void FUN_4fdc80(); KING_ALIAS("_FUN_4fdc80", "@Body_PlainStep@4")
extern "C" void Sm_Check(); KING_ALIAS("_Sm_Check", "@Body_4fc8b0@8")
extern "C" void Sm_Frame(); KING_ALIAS("_Sm_Frame", "@Body_PhysicsFrame@4")
KING_VAR(int, g_sm674460, 674460)
KING_VAR(float, g_sm64c450, 64c450)

extern "C" {

// [FUN_4fef20..$L_4ff139] body s: a pending skip (+10112) taken; without stepping (674460) or with one step its
// plain step (4fdc80); else its state +9776: 0 starts (4f5820 decides +9772, -1 stays), > 0 steps on (4fc8b0)
// or, when not stepping, sets its frame (4fca10) and wheel loads, < 0 ends (4fb470); the new state or 0
int __fastcall Body_StateStep(BYTE* s)
{
    if (F(int, s, 10112))
    {
        F(int, s, 10112) = 0;
        return 0;
    }
    if (!g_sm674460 || F(int, s, 9784) <= 1)
    {
        CallThis(FUN_4fdc80, s);
        F(int, s, 9776) = 0;
        return 0;
    }
    int st = F(int, s, 9776);
    if (!st)
    {
        int r = CallThis<int>(FUN_4f5820, s);
        F(int, s, 9772) = r;
        if (r == -1)
            return 0;
        if (r)
            CallThis(Pb_Prepare, s, 1);
        return ++F(int, s, 9776);
    }
    if (st < 0)
    {
        CallThis(FUN_4fb470, s, 1);
        CallThis(Sm_Frame, s);
        F(int, s, 9776) = 0;
        return 0;
    }
    if (F(int, s, 9772))
    {
        if (CallThis<int>(Sm_Check, s))
            return ++F(int, s, 9776);
        F(int, s, 9776) = -1;
        return -1;
    }
    memcpy(s + 13240, &g_zeroVec, 12);
    float L[12];
    CallThis(Bp_4fca10, s, L);
    memcpy(s + 16, L, 48);
    Matrix t;
    const float* inv = (const float*)memcpy(s + 2304, Matrix_Inverse(&t, (const Matrix*)(s + 16)), 48);
    double ax = F(float, s, 76), ay = F(float, s, 80), az = F(float, s, 84);
    float v[3] = {(float)((double)inv[3] * ay + (double)inv[6] * az + (double)inv[0] * ax),
                  (float)((double)inv[4] * ay + (double)inv[7] * az + (double)inv[1] * ax),
                  (float)((double)inv[5] * ay + (double)inv[8] * az + (double)inv[2] * ax)};
    Vec3 q;
    memcpy(s + 88, Vec3_DivComponents(&q, (const Vec3*)v, (const Vec3*)(s + 8008)), 12);
    CallThis(Pb_Shares, s);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        if (F(WORD, s, 6564 + 2 * i))
            F(int, s, 3360 + 4 * i) = 0;
        else
            F(float, s, 3360 + 4 * i) =
                (float)((double)F(float, s, 9808 + 4 * i) * F(float, s, 9792) * g_sm64c450);
    }
    F(int, s, 9776) = 0;
    CallThis(Sm_Frame, s);
    F(int, s, 9776) = 0;
    return 0;
}

} // extern "C"

// === a body's frame and speeds rebuilt at the end of its steps (FUN_4fb470 with its tail $L_4fbb7e)

extern "C" void FUN_4e41d9(); KING_ALIAS("_FUN_4e41d9", "_Angle_F2Pi")
extern "C" void Fe_Set(); KING_ALIAS("_Fe_Set", "@Vec3_SetThis@20")
KING_VAR(float, g_fe64c258, 64c258)
KING_VAR(float, g_fe64c304, 64c304)
KING_VAR(float, g_fe64c264, 64c264)
KING_VAR(float, g_fe64c250, 64c250)
KING_VAR(double, g_fe64b5b8, 64b5b8)
KING_VAR(double, g_fe64d318, 64d318)
KING_VAR(double, g_fe64b6e0, 64b6e0)
KING_VAR(double, g_fe64b780, 64b780)
KING_VAR(char, g_feName, 6747a8)

// a x b through the asm's SetThis
static void Fe_Cross(float* out, const float* a, const float* b)
{
    CallThis(Fe_Set, out, (float)((double)a[1] * b[2] - (double)a[2] * b[1]),
             (float)((double)a[2] * b[0] - (double)a[0] * b[2]), (float)((double)a[0] * b[1] - (double)a[1] * b[0]));
}

extern "C" {

// [FUN_4fb470..$L_4fbb7e] body s after its steps (while its step count +15372 is below +9520 its frame +8868
// rebuilt from the steps' mean motion +9048 / +9012 first): unless `keep`, with two or more wheels free its spin
// turned towards upright about the ground normal (+208) and its roll and pitch eased (+9500) towards level; then
// its world frame (+16), inverse (+2304), speeds (+64, +76, +88), acceleration (+13240) and wheel points set
void __fastcall Body_EndSteps(BYTE* s, void*, int keep)
{
    if (F(int, s, 15372) < F(int, s, 9520))
    {
        double hx = (double)F(float, s, 9060) * g_fe64c258;
        float hy = (float)((double)F(float, s, 9064) * g_fe64c258), hz = (float)((double)F(float, s, 9068) * g_fe64c258);
        float e[3] = {(float)(F(float, s, 9048) - hx), (float)((double)F(float, s, 9052) - hy),
                      (float)((double)F(float, s, 9056) - hz)};
        float k = F(float, s, 9512);
        float v[3] = {(float)((double)e[0] * k), (float)((double)e[1] * k), (float)((double)e[2] * k)};
        float roll = (float)Vec3_Length((const Vec3*)v);
        Matrix A, T, AB, R;
        memcpy(s + 9700, Matrix_LookAt(&A, g_zeroVec, *(const Vec3*)e, roll), 48);
        double t0 = (double)F(float, s, 9012) + F(float, s, 2580);
        float t1 = (float)((double)F(float, s, 9016) + F(float, s, 2584));
        float t2 = (float)((double)F(float, s, 9020) + F(float, s, 2588));
        Vec3 tv = {(float)(t0 * k), (float)((double)t1 * k), (float)((double)t2 * k)};
        Matrix_Translation(&T, tv);
        Matrix_Multiply(&AB, (const Matrix*)(s + 9700), (const Matrix*)(s + 8916));
        memcpy(s + 8868, Matrix_Multiply(&R, &AB, &T), 48);
        const float* inv = (const float*)memcpy(s + 8964, Matrix_Inverse(&R, (const Matrix*)(s + 8868)), 48);
        const float* n = &F(float, s, 208);
        float* a = &F(float, s, 9624);
        a[0] = (float)((double)inv[3] * n[1] + (double)inv[6] * n[2] + (double)n[0] * inv[0]);
        a[1] = (float)((double)inv[4] * n[1] + (double)inv[1] * n[0] + (double)inv[7] * n[2]);
        a[2] = (float)((double)inv[5] * n[1] + (double)inv[2] * n[0] + (double)inv[8] * n[2]);
        const float* w = &F(float, s, 9024);
        float* b = &F(float, s, 9036);
        b[0] = (float)((double)inv[3] * w[1] + (double)inv[6] * w[2] + (double)w[0] * inv[0]);
        b[1] = (float)((double)inv[4] * w[1] + (double)inv[1] * w[0] + (double)inv[7] * w[2]);
        b[2] = (float)((double)inv[5] * w[1] + (double)inv[2] * w[0] + (double)inv[8] * w[2]);
        Vec3 q;
        memcpy(s + 9048, Vec3_DivComponents(&q, (const Vec3*)b, (const Vec3*)(s + 8008)), 12);
    }
    if (!keep && F(int, s, 3540) > 0)
    {
        int nfree = 0;
        for (int i = 0; i < F(int, s, 3540); i++)
            nfree += !F(WORD, s, 6564 + 2 * i);
        if (nfree > 1)
        {
            const float* n = &F(float, s, 208);
            float* w = &F(float, s, 9012);
            double dot = (double)w[2] * n[2] + (double)w[1] * n[1] + (double)n[0] * w[0];
            if (x87_fabs(dot) > g_fe64b5b8)
            {
                float sg = dot > g_fe64c264 ? 1.0f : -1.0f;
                double c0 = (double)w[2] * n[1] - (double)n[2] * w[1];
                double c1 = (double)n[2] * w[0] - (double)w[2] * n[0];
                double c2 = (double)w[1] * n[0] - (double)w[0] * n[1];
                float d0 = (float)(c1 * n[2] - c2 * n[1]);
                float d1 = (float)(c2 * n[0] - c0 * n[2]);
                double d2 = c0 * n[1] - c1 * n[0];
                double m0 = (double)n[0] * g_fe64c304;
                float m1 = (float)((double)n[1] * g_fe64c304), m2 = (float)((double)n[2] * g_fe64c304);
                float m1s = (float)((double)m1 * sg), m2s = (float)((double)m2 * sg);
                w[0] = (float)(m0 * sg + d0);
                w[1] = (float)((double)d1 + m1s);
                w[2] = (float)(d2 + m2s);
            }
            const float* M = &F(float, s, 8868);
            const float* y = &g_axisY.x;
            float u[3], a1[3], b[3];
            CallThis(Fe_Set, u, (float)((double)y[2] * M[6] + (double)y[1] * M[3] + (double)y[0] * M[0]),
                     (float)((double)y[2] * M[7] + (double)y[1] * M[4] + (double)y[0] * M[1]),
                     (float)((double)y[2] * M[8] + (double)y[1] * M[5] + (double)y[0] * M[2]));
            Fe_Cross(a1, n, u);
            Fe_Cross(b, a1, n);
            Vec3 zz;
            Vec3_RotateBy(&zz, &g_axisZ, (const Matrix*)M);
            if (zz.z > g_fe64d318)
            {
                double ang6[6];
                CallC(Bp_ToAngles, *(BpMat*)M, &ang6[0], &ang6[1], &ang6[2], &ang6[3], &ang6[4], &ang6[5]);
                ang6[3] = CallC<double>(FUN_4e41d9, (float)ang6[3], &g_feName);
                float yaw = (float)x87_atan2(-(double)b[0], (double)b[1]);
                int sgn = -1;
                if (!((double)b[2] * g_axisZ.z + (double)b[1] * g_axisZ.y + (double)b[0] * g_axisZ.x > g_fe64c264))
                    sgn = 1;
                float p = (float)x87_pow(g_fe64b6e0, F(float, s, 9500) * g_fe64b780);
                double qq = (double)g_fe64c250 - p;
                float qf = (float)qq;
                ang6[3] = qq * yaw + p * ang6[3];
                ang6[4] = x87_acos(F(float, s, 216)) * sgn * qf + p * ang6[4];
                ang6[5] = p * ang6[5];
                float mc[12];
                memcpy(s + 8868, CallC<void*>(Pb_Compose, mc, ang6), 48);
            }
        }
    }
    memcpy(s + 64, s + 9012, 12);
    Matrix W, Wi;
    memcpy(s + 16, Matrix_Multiply(&W, (const Matrix*)(s + 8868), (const Matrix*)(s + 8772)), 48);
    memcpy(s + 2304, Matrix_Inverse(&Wi, (const Matrix*)(s + 16)), 48);
    float dv[3];
    CallThis(Fe_Set, dv, (float)((double)F(float, s, 88) - F(float, s, 9048)),
             (float)((double)F(float, s, 92) - F(float, s, 9052)), (float)((double)F(float, s, 96) - F(float, s, 9056)));
    float acc[3];
    memcpy(s + 13240, Vec3_DivScalar(acc, dv, (float)Game_FrameTime()), 12);
    memcpy(s + 88, s + 9048, 12);
    memcpy(s + 76, s + 9024, 12);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        F(int, s, 3544 + 4 * i) = F(int, s, 9244 + 4 * i);
        Vec3 p;
        memcpy(s + 100 + 12 * i, Vec3_MulMatrix(&p, (const Vec3*)(s + 9528 + 12 * i), (const Matrix*)(s + 8772)), 12);
        if (F(WORD, s, 6564 + 2 * i))
            F(int, s, 3360 + 4 * i) = 0;
        else
            F(float, s, 3360 + 4 * i) = (float)x87_fabs(F(float, s, 8740 + 4 * i));
        F(float, s, 9808 + 4 * i) =
            (float)(x87_fabs(F(float, s, 8740 + 4 * i)) / ((double)F(float, s, 9788) * F(float, s, 9792)));
    }
}

} // extern "C"

// === a body's new frame from its supports or its free motion (FUN_4fca10 with its tail $L_4fdb50)

extern "C" void Bf_Set(); KING_ALIAS("_Bf_Set", "@Vec3_SetThis@20")
extern "C" void Bf_SubAssign(); KING_ALIAS("_Bf_SubAssign", "@Vec3_SubAssign@16")
extern "C" void Bf_Pose(); KING_ALIAS("_Bf_Pose", "_Matrix_FromPose")
extern "C" void Bf_Reject(); KING_ALIAS("_Bf_Reject", "_Vec3_RejectFrom")
KING_VAR(float, g_bf674458, 674458)
KING_VAR(float, g_bf67445c, 67445c)
KING_VAR(float, g_bf64c250, 64c250)
KING_VAR(float, g_bf64c258, 64c258)
KING_VAR(float, g_bf64c264, 64c264)
KING_VAR(float, g_bf64c450, 64c450)
KING_VAR(float, g_bf64da28, 64da28)
KING_VAR(float, g_bf64d2f8, 64d2f8)
KING_VAR(double, g_bf64b738, 64b738)
KING_VAR(double, g_bf64d318, 64d318)
KING_VAR(double, g_bf64d900, 64d900)
KING_VAR(double, g_bf64da20, 64da20)
KING_VAR(double, g_bf64da18, 64da18)
KING_VAR(double, g_bf64b5a0, 64b5a0)
KING_VAR(double, g_bf64b840, 64b840)

static void Bf_SetV(float* o, double x, double y, double z)
{
    CallThis(Bf_Set, o, (float)x, (float)y, (float)z);
}

// a + b of two float triples as SetThis stores it
static void Bf_AddV(float* o, const float* a, const float* b)
{
    Bf_SetV(o, (double)a[0] + b[0], (double)a[1] + b[1], (double)a[2] + b[2]);
}

// the x axis of frame M
static void Bf_XAxis(float* o, const float* M)
{
    const float* a = &g_axisX.x;
    Bf_SetV(o, (double)a[2] * M[6] + (double)a[1] * M[3] + (double)a[0] * M[0],
            (double)a[2] * M[7] + (double)a[1] * M[4] + (double)a[0] * M[1],
            (double)a[2] * M[8] + (double)a[1] * M[5] + (double)a[0] * M[2]);
}

// the smaller and the larger of two support loads
static void Bf_Pair(BYTE* s, int a, int b, float* lo, float* hi)
{
    float x = F(float, s, a), y = F(float, s, b);
    *lo = y < x ? y : x;
    *hi = x < y ? y : x;
}

// the midpoint of the contact points at +p and +q (x unrounded before the half, b keeps all three unrounded)
static void Bf_Mid(BYTE* s, int p, int q, float* m)
{
    double x = (double)F(float, s, p) + F(float, s, q);
    float y = (float)((double)F(float, s, p + 4) + F(float, s, q + 4));
    float z = (float)((double)F(float, s, p + 8) + F(float, s, q + 8));
    m[0] = (float)(x * g_bf64c258);
    m[1] = (float)((double)y * g_bf64c258);
    m[2] = (float)((double)z * g_bf64c258);
}

// the centroid of three contact points (+a + +b, then + +c, times 1/3)
static void Bf_Centroid(BYTE* s, int a, int b, int c, float* m)
{
    float A[3], B[3];
    Bf_SetV(A, (double)F(float, s, a) + F(float, s, b), (double)F(float, s, a + 4) + F(float, s, b + 4),
            (double)F(float, s, a + 8) + F(float, s, b + 8));
    Bf_AddV(B, A, &F(float, s, c));
    Bf_SetV(m, (double)B[0] * g_bf64da28, (double)B[1] * g_bf64da28, (double)B[2] * g_bf64da28);
}

extern "C" {

// [FUN_4fca10..$L_4fdb50] body s's next frame into out: unchanged when held (+6580); standing on its wheels
// (+6586 with load +3640 >= 1) turned about the wheels' mean point by the support case (pairs +6581 .. +6584,
// single wheels +6564 ..) and lifted by the axle, then moved by its speed; otherwise free, its yaw from +2604 /
// +2608 and its roll and pitch eased towards the motion (+3180 .. +3400), its center kept on the ground plane
void __fastcall Body_NextFrame(BYTE* s, void*, float* out)
{
    float res[12];
    memcpy(res, s + 16, 48);
    const float* M = &F(float, s, 16);
    float n[3], c[3];
    memcpy(n, s + 208, 12);
    memcpy(c, s + 8724, 12);
    float dt = (float)Game_FrameTime();
    if (F(BYTE, s, 6580))
    {
        memcpy(out, res, 48);
        return;
    }
    float tmp[3];
    if (F(BYTE, s, 6586) && !(F(float, s, 3640) < g_bf64c250))
    {
        float m[3], v[3];
        memcpy(m, n, 12);
        memcpy(v, &g_zeroVec, 12);
        memcpy(c, &g_zeroVec, 12);
        float k1 = 0, k2 = 0, sum = 0;
        for (int i = 0; i < F(int, s, 3540); i++)
        {
            double t = F(float, s, 7028 + 4 * i);
            float ax = (float)(t * g_axisZ.x), ay = (float)(t * g_axisZ.y);
            const float* w = &F(float, s, 7356 + 12 * i);
            float p[3] = {(float)((double)w[0] - ax), (float)((double)w[1] - ay), (float)(w[2] - t * g_axisZ.z)};
            CallThis(Pb_AddAssign, c, tmp, p);
            sum = (float)((double)sum + F(float, s, 8060 + 4 * i));
        }
        sum = (float)(sum * g_bf64b738);
        CallThis(Pb_MulAssign, c, tmp, 0.25f);
        const float* g = &F(float, s, 2616);
        if (F(BYTE, s, 6581))
        {
            Bf_Pair(s, 3608, 3612, &k1, &k2);
            Bf_Mid(s, 756, 1004, m);
            memcpy(v, g, 12);
        }
        else if (F(BYTE, s, 6582))
        {
            Bf_Pair(s, 3608, 3616, &k1, &k2);
            Bf_Mid(s, 508, 1004, m);
            v[0] = (float)((double)m[1] * g[2] - (double)m[2] * g[1]);
            v[1] = (float)((double)m[2] * g[0] - (double)m[0] * g[2]);
            v[2] = (float)((double)m[0] * g[1] - (double)m[1] * g[0]);
        }
        else if (F(BYTE, s, 6584))
        {
            Bf_Pair(s, 3612, 3620, &k1, &k2);
            Bf_Mid(s, 260, 756, m);
            v[0] = (float)((double)m[2] * g[1] - (double)m[1] * g[2]);
            v[1] = (float)((double)m[0] * g[2] - (double)m[2] * g[0]);
            v[2] = (float)((double)m[1] * g[0] - (double)m[0] * g[1]);
        }
        else if (F(BYTE, s, 6583))
        {
            Bf_Pair(s, 3616, 3620, &k1, &k2);
            v[0] = -g[0];
            v[1] = -g[1];
            v[2] = -g[2];
            double x = (double)F(float, s, 260) + F(float, s, 508);
            double y = (double)F(float, s, 264) + F(float, s, 512);
            double z = (double)F(float, s, 268) + F(float, s, 516);
            Bf_SetV(m, x * g_bf64c258, y * g_bf64c258, z * g_bf64c258);
        }
        else if (F(WORD, s, 6564))
        {
            k1 = k2 = F(float, s, 3608);
            Bf_Centroid(s, 508, 756, 1004, m);
            float X[3];
            Bf_XAxis(X, M);
            Bf_SetV(v, (double)g[0] - X[0], (double)g[1] - X[1], (double)g[2] - X[2]);
        }
        else if (F(WORD, s, 6566))
        {
            k1 = k2 = F(float, s, 3612);
            float X[3];
            Bf_XAxis(X, M);
            Bf_AddV(v, X, g);
            Bf_Centroid(s, 260, 756, 1004, m);
        }
        else if (F(WORD, s, 6568) || F(WORD, s, 6570))
        {
            bool h = !F(WORD, s, 6568);
            k1 = k2 = F(float, s, h ? 3620 : 3616);
            float X[3], N[3];
            Bf_XAxis(X, M);
            Bf_SetV(N, -(double)g[0], -(double)g[1], -(double)g[2]);
            Bf_SetV(v, (double)N[0] - X[0], (double)N[1] - X[1], (double)N[2] - X[2]);
            float A[3], B[3];
            if (!h)
            {
                Bf_SetV(A, (double)F(float, s, 508) + F(float, s, 260), (double)F(float, s, 512) + F(float, s, 264),
                        (double)F(float, s, 516) + F(float, s, 268));
                Bf_AddV(B, A, &F(float, s, 1004));
            }
            else
            {
                Bf_SetV(A, (double)F(float, s, 756) + F(float, s, 508), (double)F(float, s, 512) + F(float, s, 760),
                        (double)F(float, s, 516) + F(float, s, 764));
                Bf_AddV(B, A, &F(float, s, 260));
            }
            Bf_SetV(m, (double)B[0] * g_bf64da28, (double)B[1] * g_bf64da28, (double)B[2] * g_bf64da28);
        }
        k1 = (float)((double)k1 - sum);
        if (k1 < g_bf64c264)
            k1 = 0;
        else if (k1 > g_bf64c250)
            k1 = 1.0f;
        if (k2 < g_bf64c264)
            k2 = 0;
        else if (k2 > g_bf64c250)
            k2 = 1.0f;
        Vec3 C, R, t1;
        Vec3_Cross(&C, (const Vec3*)m, (const Vec3*)v);
        Vec3_RotateBy(&R, &C, (const Matrix*)(s + 2304));
        CallThis(Pb_MulAssign, v, &t1, (float)((double)k1 * dt * g_bf64c450));
        float k3 = (float)(dt * g_bf64d318);
        CallThis(Bf_SubAssign, v, tmp, Vec3_Scaled(&t1, (const Vec3*)m, k3));
        if (!(v[2] <= g_bf64c264))
            v[2] = 0;
        if (F(float, s, 3520) < g_bf64c450)
            memcpy(s + 64, Vec3_Add(&t1, (const Vec3*)(s + 2568), (const Vec3*)v), 12);
        CallThis(Bf_SubAssign, v, tmp, Vec3_Scaled(&t1, (const Vec3*)m, k3));
        float lift = F(float, s, 3640) < g_bf64d2f8 ? (float)((double)g_bf674458 * g_bf64d2f8) : g_bf674458;
        Matrix T, L, LM, W;
        Matrix_Translation(&T, *Vec3_Scaled(&t1, (const Vec3*)v, dt));
        float roll = (float)((double)lift * k2 * dt * g_bf64d900);
        Matrix_LookAt(&L, *(const Vec3*)c, R, roll);
        Matrix_Multiply(&LM, &L, (const Matrix*)M);
        memcpy(res, Matrix_Multiply(&W, &LM, &T), 48);
    }
    else
    {
        Vec3 a, u, q1, q, f, nq;
        Vec3_Add(&a, (const Vec3*)(s + 3180), (const Vec3*)(s + 3192));
        Vec3_DivScalar(&u.x, &a.x, F(float, s, 9792));
        Vec3_Scaled(&a, (const Vec3*)n, F(float, s, 3400));
        Vec3_Sub(&q1, (const Vec3*)(s + 3392), &a);
        Vec3_Sub(&q, &q1, &u);
        Vec3_Cross(&nq, (const Vec3*)n, &q);
        Vec3_Cross(&f, &nq, (const Vec3*)n);
        const float* g = &F(float, s, 2616);
        float w[3];
        memcpy(w, g, 12);
        double lf = Vec3_Length(&f);
        float lff = (float)lf;
        double kd = lf * g_bf64da20;
        float k = (float)kd;
        if (kd > g_bf64da18)
            *(int*)&k = 1026698510;
        if (lf > g_bf64b5a0)
        {
            Vec3 d;
            memcpy(w, Vec3_DivScalar(&d.x, &f.x, lff), 12);
        }
        float tilt = (float)(Vec3_Dot((const Vec3*)g, (const Vec3*)w) * k * g_bf64b840);
        Vec3 X, P, Q;
        Vec3_RotateBy(&X, &g_axisX, (const Matrix*)M);
        Vec3_Cross(&P, (const Vec3*)n, &X);
        Vec3_Cross(&Q, &P, (const Vec3*)n);
        float roll = (float)(Vec3_Dot(&Q, (const Vec3*)w) * k);
        float pitch0 = (float)-x87_asin(g[2]);
        Vec3 cz;
        float roll0 = (float)-x87_asin(Vec3_Cross(&cz, (const Vec3*)g, (const Vec3*)n)->z);
        float yaw = (float)-x87_atan2(F(float, s, 2604), F(float, s, 2608));
        double an[6];
        CallC(Bp_ToAngles, *(const BpMat*)M, &an[0], &an[1], &an[2], &an[3], &an[4], &an[5]);
        double p = x87_pow(g_bf67445c, (double)dt * g_bf64c450);
        double qq = g_bf64c250 - p;
        an[4] = ((double)pitch0 + tilt) * qq + p * an[4];
        an[5] = ((double)roll0 + roll) * qq + p * an[5];
        Vec3 r, sn;
        memcpy(c, CallC<float*>(Bf_Reject, &r, s + 8724, n, s + 52), 12);
        CallThis(Pb_AddAssign, c, tmp, Vec3_Scaled(&sn, (const Vec3*)n, F(float, s, 8736)));
        float W[12];
        memcpy(res, CallC<float*>(Bf_Pose, W, (double)c[0], (double)c[1], (double)c[2], (double)yaw, an[4], an[5]),
               48);
    }
    memcpy(out, res, 48);
}

} // extern "C"

// === a body's plain frame without sub-steps (FUN_4fdc80 with its tail $L_4fe18e)

extern "C" void FUN_500850(); KING_ALIAS("_FUN_500850", "@Car_PedalStep@4")
extern "C" void FUN_4f35f0(); KING_ALIAS("_FUN_4f35f0", "@Wheels_FrameSpin@4")
extern "C" void Ps_Check(); KING_ALIAS("_Ps_Check", "_World_CheckPoint")
extern "C" void Ps_Damage(); KING_ALIAS("_Ps_Damage", "@Car_CheckDamage@8")
extern "C" void Ps_Ground(); KING_ALIAS("_Ps_Ground", "@Car_PlaceOnGround@12")
extern "C" void Pls_Query(); KING_ALIAS("_Pls_Query", "_Query_4e5b22")
extern "C" void Ps_Nop(); KING_ALIAS("_Ps_Nop", "_Fw_Nop")
KING_VAR(int, g_ps697670, 697670)
KING_VAR(char, g_psCheck1, 6747f0)
KING_VAR(char, g_psCheck2, 6747e0)
KING_VAR(char, g_psCheck3, 6747d0)
KING_VAR(char, g_psCheck4, 6747c0)
KING_VAR(float, g_ps64c264, 64c264)
KING_VAR(float, g_ps64c258, 64c258)
KING_VAR(float, g_ps64c408, 64c408)
KING_VAR(double, g_ps64b678, 64b678)
KING_VAR(double, g_ps64b898, 64b898)
KING_VAR(double, g_ps64b5b8, 64b5b8)

// p through the rotation and place of frame m (+0 .. +44): the asm's per-row order for the wheel points
static void Ps_Inv(const float* m, const float* p, float* o)
{
    o[0] = (float)((double)m[6] * p[2] + (double)p[0] * m[0] + (double)p[1] * m[3] + m[9]);
    o[1] = (float)((double)p[0] * m[1] + (double)m[7] * p[2] + (double)p[1] * m[4] + m[10]);
    o[2] = (float)((double)p[1] * m[5] + (double)p[0] * m[2] + (double)p[2] * m[8] + m[11]);
}

// v minus its part along n, as (n x v) x n with the asm's rounding (n x v unrounded)
static void Ps_Tangent(const float* v, const float* n, float* o)
{
    double c0 = (double)v[2] * n[1] - (double)n[2] * v[1];
    double c1 = (double)n[2] * v[0] - (double)n[0] * v[2];
    double c2 = (double)n[0] * v[1] - (double)v[0] * n[1];
    o[0] = (float)(c1 * n[2] - c2 * n[1]);
    o[1] = (float)(c2 * n[0] - c0 * n[2]);
    double z = c0 * n[1] - c1 * n[0];
    o[2] = (float)z;
}

extern "C" {

// [FUN_4fdc80..$L_4fe18e] body s for one frame without sub-steps: checks and damage, wheel points in its own
// frame, the steering turn and its sideways push, the forces of its contacts (+10176 points) and wheels summed
// (+3180) into its speed (+64), sliding along the ground or snapping to the steering direction, its new frame
// from the turn and the speed then from its supports (Body_NextFrame), its angles stepped; when tipped its loads
// reset, else its contacts cleared
void __fastcall Body_PlainStep(BYTE* s)
{
    float* vel = &F(float, s, 64);
    CallC(Ps_Check, vel, &g_psCheck1, 150.0f);
    float dt = (float)Game_FrameTime();
    F(float, s, 15316) = (float)(Game_Time() - F(float, s, 6112));
    CallThis(Ps_Damage, s);
    CallThis(FUN_500850, s);
    F(int, s, 15492) = 0;
    F(int, s, 15448) = 0;
    memcpy(s + 15452, &g_zeroVec, 12);
    F(int, s, 15496) = 0;
    {
        double f = F(float, s, 9792);
        F(float, s, 3156) = (float)(f * F(float, s, 3392));
        F(float, s, 3160) = (float)(f * F(float, s, 3396));
        F(float, s, 3164) = (float)(f * F(float, s, 3400));
    }
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        F(int, s, 3644 + 4 * i) = 0;
        F(int, s, 15248 + 4 * i) = 0;
        F(int, s, 3708 + 4 * i) = 0;
        F(int, s, 3676 + 4 * i) = 0;
    }
    if (F(WORD, s, 10056))
    {
        F(int, s, 13124) = 0;
        F(WORD, s, 10056) = 0;
        F(int, s, 10012) = 0;
        F(int, s, 10720) = 0;
        F(int, s, 10016) = 0;
        F(int, s, 10716) = 0;
        F(int, s, 10712) = 0x3D4CCCCD;
        F(int, s, 10728) = 0;
        CallThis(Pb_EngReset, s + 9984);
    }
    Body_SaveAll(s, 0);
    Body_UpdateContacts(s, 0);
    const float* inv = &F(float, s, 2304);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        F(int, s, 3328 + 4 * i) = 0;
        memcpy(s + 8436 + 12 * i, &g_zeroVec, 12);
        Ps_Inv(inv, &F(float, s, 100 + 12 * i), &F(float, s, 6212 + 12 * i));
        F(BYTE, s, 6595 + i) = 0;
    }
    const float* n = &F(float, s, 208);
    {
        float* o = &F(float, s, 4104);
        o[0] = (float)((double)inv[3] * n[1] + (double)inv[6] * n[2] + (double)n[0] * inv[0]);
        o[1] = (float)((double)inv[4] * n[1] + (double)inv[1] * n[0] + (double)inv[7] * n[2]);
        o[2] = (float)((double)inv[5] * n[1] + (double)inv[2] * n[0] + (double)inv[8] * n[2]);
    }
    float steer = (float)Body_SteerAngle(s, 0);
    F(float, s, 6688) = steer;
    Matrix Rz;
    Matrix_RotationZ(&Rz, steer);
    Body_4f2630(s, 0, (int)&Rz);
    Body_UpdateGrip(s, 0);
    double sn = x87_sin(F(float, s, 6688));
    float sinf_ = (float)sn;
    double v = F(BYTE, s, 6581) ? (double)g_ps64c264 : x87_fabs(sn) / F(float, s, 8032);
    double sp = x87_fabs(F(float, s, 3528));
    double vs = v * sp;
    float vsf = (float)vs;
    float vs2 = (float)(vs * sp);
    float sgn = 1.0f;
    {
        int a = sinf_ > g_ps64c264;
        const float* g = &F(float, s, 2556);
        double d = (double)g[2] * F(float, s, 2612) + (double)g[1] * F(float, s, 2608) +
                   (double)F(float, s, 2604) * g[0];
        int b = d > g_ps64b678;
        if (a != b)
            sgn = -1.0f;
        double w = v * ((double)sgn * F(float, s, 3524));
        F(float, s, 3108) = (float)(w * n[0]);
        F(float, s, 3112) = (float)(w * n[1]);
        F(float, s, 3116) = (float)(w * n[2]);
        double c0 = (double)n[1] * g[2] - (double)g[1] * n[2];
        float c1 = (float)((double)g[0] * n[2] - (double)n[0] * g[2]);
        float c2 = (float)((double)g[1] * n[0] - (double)g[0] * n[1]);
        float k = (float)((double)sgn * F(float, s, 9792) * vs2);
        float e[3] = {(float)(c0 * k), (float)((double)c1 * k), (float)((double)c2 * k)};
        const float* R = Rz.m;
        float* o = &F(float, s, 3192);
        o[0] = (float)((double)R[0] * e[0] + (double)R[6] * e[2] + (double)R[3] * e[1]);
        o[1] = (float)((double)R[1] * e[0] + (double)R[7] * e[2] + (double)R[4] * e[1]);
        o[2] = (float)((double)R[2] * e[0] + (double)R[8] * e[2] + (double)R[5] * e[1]);
    }
    CallThis(FUN_4f35f0, s);

    float A[3];
    memcpy(A, &g_zeroVec, 12);
    float tmp[3];
    for (int i = 0; i < F(int, s, 10176); i++)
        CallThis(Pb_AddAssign, A, tmp, s + 10244 + 12 * i);
    memcpy(s + 3180, &g_zeroVec, 12);
    memcpy(s + 3264, &g_zeroVec, 12);
    int touch = 0;
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        bool free = !F(WORD, s, 6564 + 2 * i);
        if (touch || !free)
            touch = 1;
        float fa[3], fb[3];
        memcpy(fa, &g_zeroVec, 12);
        memcpy(fb, &g_zeroVec, 12);
        if (free)
        {
            memcpy(fa, s + 8628 + 12 * i, 12);
            memcpy(fb, s + 3848 + 12 * i, 12);
        }
        float t[3] = {(float)((double)fb[0] + fa[0]), (float)((double)fb[1] + fa[1]), (float)((double)fb[2] + fa[2])};
        CallThis(Pb_AddAssign, s + 3180, tmp, t);
        if (free)
        {
            const float* a = &F(float, s, 8340 + 12 * i);
            const float* r = &F(float, s, 260 + 248 * i);
            double c0 = (double)a[2] * r[1] - (double)a[1] * r[2];
            double c1 = (double)a[0] * r[2] - (double)a[2] * r[0];
            double c2 = (double)a[1] * r[0] - (double)r[1] * a[0];
            float* d = &F(float, s, 8244 + 12 * i);
            d[0] = (float)(c1 * r[2] - c2 * r[1]);
            d[1] = (float)(c2 * r[0] - c0 * r[2]);
            d[2] = (float)(c0 * r[1] - c1 * r[0]);
            CallThis(Pb_AddAssign, s + 3180, tmp, d);
        }
    }
    {
        const float* e = &F(float, s, 3192);
        double m = F(float, s, 9792);
        double x = (double)e[0] + F(float, s, 3180);
        float y = (float)((double)F(float, s, 3184) + e[1]);
        float z = (float)((double)F(float, s, 3188) + e[2]);
        double ax = x / m;
        float ay = (float)(y / m), az = (float)(z / m);
        const float* w = &F(float, s, 3392);
        F(float, s, 3416) = (float)(w[0] - ax);
        F(float, s, 3420) = (float)((double)w[1] - ay);
        F(float, s, 3424) = (float)((double)w[2] - az);
    }
    float u[3];
    memcpy(u, s + 3392, 12);
    if (!touch)
    {
        const float* w = &F(float, s, 3392);
        double vn = (double)w[2] * n[2] + (double)w[1] * n[1] + (double)w[0] * n[0];
        float nx = (float)(vn * n[0]), ny = (float)(vn * n[1]);
        double nz = vn * n[2];
        double c0 = (double)w[2] * n[1] - (double)n[2] * w[1];
        double c1 = (double)n[2] * w[0] - (double)n[0] * w[2];
        double c2 = (double)n[0] * w[1] - (double)w[0] * n[1];
        float d0 = (float)(c1 * n[2] - c2 * n[1]);
        float d1 = (float)(c2 * n[0] - c0 * n[2]);
        double d2 = c0 * n[1] - c1 * n[0];
        float h0 = (float)((double)d0 * g_ps64c258), h1 = (float)((double)d1 * g_ps64c258);
        double h2 = d2 * g_ps64c258;
        u[0] = (float)((double)h0 + nx);
        u[1] = (float)((double)ny + h1);
        u[2] = (float)(nz + h2);
    }
    {
        double m2 = (double)F(float, s, 10124) + F(float, s, 9792);
        double m = F(float, s, 9792);
        float a0 = (float)(A[0] / m), a1 = (float)(A[1] / m), a2 = (float)(A[2] / m);
        float f0 = (float)(F(float, s, 3180) / m2), f1 = (float)(F(float, s, 3184) / m2);
        double f2 = F(float, s, 3188) / m2;
        float b0 = (float)((double)u[0] + f0), b1 = (float)((double)f1 + u[1]);
        double b2 = f2 + u[2];
        float c0 = (float)((double)b0 + a0), c1 = (float)((double)a1 + b1);
        double c2 = b2 + a2;
        float dv[3] = {(float)((double)c0 * dt), (float)((double)c1 * dt), (float)(c2 * dt)};
        CallThis(Pb_AddAssign, vel, tmp, dv);
    }
    CallC(Ps_Check, vel, &g_psCheck2, 150.0f);
    if (!(touch && F(BYTE, s, 6581) && F(BYTE, s, 6583)))
    {
        float t[3];
        Ps_Tangent(vel, n, t);
        memcpy(vel, t, 12);
        double len = Vec3_Length((const Vec3*)vel);
        F(float, s, 3520) = (float)len;
        double mx;
        if (!F(BYTE, s, 6582) && !F(BYTE, s, 6584))
            mx = (double)F(float, s, 8060) * g_ps64c258;
        else
        {
            mx = g_ps64c264;
            for (int i = 0; i < F(int, s, 3540); i++)
                if (mx < F(float, s, 3608 + 4 * i))
                    mx = F(float, s, 3608 + 4 * i);
        }
        if (!(F(float, s, 216) < g_ps64b898) && !(mx > (double)F(float, s, 8060) * g_ps64c408))
        {
            const float* g = &F(float, s, 2616);
            double sg = 1.0;
            if (!((double)vel[2] * g[2] + (double)vel[1] * g[1] + (double)g[0] * vel[0] > g_ps64b678))
                sg = -1.0;
            float q[3], r[3];
            CallThis(Bf_Set, q, (float)(len * g[0]), (float)(len * g[1]), (float)(len * g[2]));
            CallThis(Bf_Set, r, (float)(q[0] * sg), (float)(q[1] * sg), (float)(q[2] * sg));
            memcpy(vel, r, 12);
        }
        else
            F(float, s, 3520) = (float)Vec3_Length((const Vec3*)vel);
        CallC(Ps_Check, vel, &g_psCheck3, 150.0f);
        if (F(int, s, 6844) &&
            (F(float, s, 3524) < g_ps64b5b8 || (double)vel[2] * F(float, s, 2576) + (double)vel[1] * F(float, s, 2572) +
                                                       (double)vel[0] * F(float, s, 2568) < g_ps64b678))
        {
            CallThis(Pb_Restart, s);
            memcpy(s + 3180, &g_zeroVec, 12);
            memcpy(s + 2580, &g_zeroVec, 12);
        }
    }

    Matrix Rm, a, b, c, T;
    CallStd(Pb_ForEach, &Rm, 12, 4, Pb_Zero);
    memcpy(&Rm, &g_identity, 48);
    const Matrix* R;
    if (F(BYTE, s, 6581))
    {
        Matrix_RotationY(&a, (float)((double)dt * F(float, s, 92)));
        Matrix_RotationX(&b, (float)((double)dt * F(float, s, 88)));
        Matrix_RotationZ(&c, (float)((double)dt * F(float, s, 96)));
        Matrix ab;
        Matrix_Multiply(&ab, &c, &b);
        R = Matrix_Multiply(&T, &ab, &a);
    }
    else
    {
        const float* w = &F(float, s, 3108);
        float q[3];
        CallThis(Bf_Set, q, (float)((double)inv[6] * w[2] + (double)inv[3] * w[1] + (double)w[0] * inv[0]),
                 (float)((double)inv[1] * w[0] + (double)inv[7] * w[2] + (double)inv[4] * w[1]),
                 (float)((double)inv[2] * w[0] + (double)inv[8] * w[2] + (double)inv[5] * w[1]));
        memcpy(s + 88, q, 12);
        Vec3 mc, rr;
        Vec3_MulComponents(&mc, (const Vec3*)(s + 88), (const Vec3*)(s + 8008));
        memcpy(s + 76, Vec3_RotateBy(&rr, &mc, (const Matrix*)(s + 16)), 12);
        R = Matrix_RotationZ(&T, (float)((double)sgn * vsf * dt));
    }
    memcpy(&Rm, R, 48);
    if (g_ps697670)
    {
        Matrix x, y;
        int kb = 1043511490;
        Matrix_RotationX(&x, *(float*)&kb);
        memcpy(&Rm, Matrix_Multiply(&y, &Rm, &x), 48);
    }
    {
        float d[3];
        CallThis(Bf_Set, d, (float)((double)dt * vel[0]), (float)((double)dt * vel[1]), (float)((double)dt * vel[2]));
        Matrix_Translation(&T, *(const Vec3*)d);
        Matrix rm, w;
        Matrix_Multiply(&rm, &Rm, (const Matrix*)(s + 16));
        memcpy(s + 16, Matrix_Multiply(&w, &rm, &T), 48);
    }
    {
        Vec3 y, p, q, nn;
        memcpy(s + 2604, Vec3_RotateBy(&y, &g_axisY, (const Matrix*)(s + 16)), 12);
        Vec3_Cross(&p, (const Vec3*)n, (const Vec3*)(s + 2604));
        Vec3_Cross(&q, &p, (const Vec3*)n);
        memcpy(s + 2616, Vec3_Normalized(&nn, &q), 12);
    }
    CallThis(Ps_Ground, s, s + 16);
    float nf[12];
    Body_NextFrame(s, 0, nf);
    memcpy(s + 16, nf, 48);
    {
        Matrix iv;
        Matrix_Inverse(&iv, (const Matrix*)(s + 16));
        memcpy(s + 2304, &iv, 48);
        Vec3 r, d;
        Vec3_RotateBy(&r, (const Vec3*)(s + 76), &iv);
        memcpy(s + 88, Vec3_DivComponents(&d, &r, (const Vec3*)(s + 8008)), 12);
    }
    memset(s + 15336, 0, 32);
    F(int, s, 15368) = 0;
    F(int, s, 15372) = 0;
    double an[6];
    CallC(Bp_ToAngles, *(const BpMat*)(s + 16), &an[0], &an[1], &an[2], &an[3], &an[4], &an[5]);
    __declspec(align(8)) BYTE node[208];
    CallThis(Pb_Node, node);
    if (CallC<int>(Pls_Query, F(BYTE*, s, 6672), s + 2292, s + 52, node))
        F(int, s, 244) = F(int, node, 104);
    Body_UpdateContacts(s, 0);
    CallThis(Pb_Step, s, an);
    if (Body_IsTipped(s, 0))
    {
        for (int i = 0; i < F(int, s, 3540); i++)
            F(int, s, 3544 + 4 * i) = F(int, s, 8092 + 4 * i);
        F(int, s, 13124) = 1;
        return;
    }
    CallThis(Ps_Nop, s);
    for (int i = 0; i < F(int, s, 10176); i++)
        memcpy(s + 10244 + 12 * i, &g_zeroVec, 12);
    F(int, s, 10176) = 0;
    CallC(Ps_Check, vel, &g_psCheck4, 150.0f);
}

} // extern "C"

// === the first of a body's sub-steps (FUN_4f5820 with its tail $L_4f5c77)

extern "C" void Ss_Items(); KING_ALIAS("_Ss_Items", "@Items_StepAll@8")
extern "C" void Ss_Setting(); KING_ALIAS("_Ss_Setting", "@Car_SetSetting@16")
extern "C" void Ss_Steering(); KING_ALIAS("_Ss_Steering", "@Steering_Update@8")
extern "C" void Ss_Lean(); KING_ALIAS("_Ss_Lean", "@Body_SlopeLean@4")
extern "C" void Ss_Tyres(); KING_ALIAS("_Ss_Tyres", "@Tyre_Forces@12")
extern "C" void Ss_Mean30(); KING_ALIAS("_Ss_Mean30", "@Body_Mean1a30@8")
extern "C" void Ss_Mean38(); KING_ALIAS("_Ss_Mean38", "@Body_Mean1a38@8")
extern "C" void Ss_Forces(); KING_ALIAS("_Ss_Forces", "@Wheels_ForceTorque@16")
extern "C" void Ss_Limit(); KING_ALIAS("_Ss_Limit", "@Body_LimitChange@28")
extern "C" void Ss_ResetMotion(); KING_ALIAS("_Ss_ResetMotion", "@Body_ResetMotion@8")
extern "C" void Ss_StepVectors(); KING_ALIAS("_Ss_StepVectors", "@Body_StepVectors@8")
extern "C" void Ss_Slide(); KING_ALIAS("_Ss_Slide", "@Body_SlideMove@8")
extern "C" void Ss_Ground(); KING_ALIAS("_Ss_Ground", "@Wheels_FindGround@4")
extern "C" void Ss_floor(); KING_ALIAS("_Ss_floor", "_floor")
KING_VAR(float, g_ss674444, 674444)
KING_VAR(double, g_ss64da00, 64da00)
KING_VAR(double, g_ss64b5a0, 64b5a0)
KING_VAR(double, g_ss64d9f8, 64d9f8)
KING_VAR(double, g_ss64b670, 64b670)
KING_VAR(double, g_ss64b6e0, 64b6e0)
KING_VAR(float, g_ss64c258, 64c258)

extern "C" {

// [FUN_4f5820..$L_4f5c77] body s before its sub-steps: timers, settings fading (+9920), damage; -1 when settled
// (+13124); wheel points and the ground normal in its own frame, the steering frame (+8196), tyre forces, the
// wheels' force and torque limited per sub-step, sums of contact forces (+3180) and torques (+3264), the speed
// and spin stepped, its frame turned about the ground contact point and moved; -1 when tipped, else 1
int __fastcall Body_StartSteps(BYTE* s)
{
    float dt = (float)Game_FrameTime();
    double gt = Game_Time();
    float now = (float)gt;
    F(float, s, 12) = (float)(gt - F(float, s, 8));
    F(int, s, 15496) = 0;
    if (F(int, s, 10116))
        CallThis(Ss_Items, s + 8188);
    F(float, s, 15316) = (float)(Game_Time() - F(float, s, 6112));
    F(int, s, 15492) = 0;
    F(int, s, 15448) = 0;
    memcpy(s + 15452, &g_zeroVec, 12);
    for (int i = 0; i < 8; i++)
    {
        if (!F(int, s, 9920 + 4 * i))
            continue;
        float t = F(float, s, 9952 + 4 * i);
        if (!((double)g_ss674444 + t <= now))
            CallThis(Ss_Setting, s, 3 + i, (float)(((double)now - t) / g_ss674444));
        else
        {
            F(int, s, 9920 + 4 * i) = 0;
            CallThis(Ss_Setting, s, 3 + i, 1.0f);
        }
    }
    CallThis(Ps_Damage, s);
    CallThis(FUN_500850, s);
    if (F(int, s, 13124) && !F(WORD, s, 10056))
        return -1;
    {
        double f = F(float, s, 9792);
        F(float, s, 3156) = (float)(f * F(float, s, 3392));
        F(float, s, 3160) = (float)(f * F(float, s, 3396));
        F(float, s, 3164) = (float)(f * F(float, s, 3400));
    }
    if (F(WORD, s, 10056))
    {
        F(int, s, 13124) = 0;
        F(WORD, s, 10056) = 0;
        F(int, s, 10012) = 0;
        F(int, s, 10720) = 0;
        F(int, s, 10016) = 0;
        F(int, s, 10716) = 0;
        F(int, s, 10712) = 0x3D4CCCCD;
        F(int, s, 10728) = 0;
        CallThis(Pb_EngReset, s + 9984);
    }
    Body_SaveAll(s, 0);
    Body_UpdateContacts(s, 0);
    const float* m = &F(float, s, 2304);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        F(int, s, 3328 + 4 * i) = 0;
        memcpy(s + 8436 + 12 * i, &g_zeroVec, 12);
        const float* p = &F(float, s, 100 + 12 * i);
        float* o = &F(float, s, 6212 + 12 * i);
        o[0] = (float)((double)m[6] * p[2] + (double)m[0] * p[0] + (double)m[3] * p[1] + m[9]);
        o[1] = (float)((double)p[2] * m[7] + (double)m[1] * p[0] + (double)m[4] * p[1] + m[10]);
        o[2] = (float)((double)p[1] * m[5] + (double)p[2] * m[8] + (double)m[2] * p[0] + m[11]);
        F(BYTE, s, 6595 + i) = 0;
    }
    const float* n = &F(float, s, 208);
    {
        float* o = &F(float, s, 4104);
        o[0] = (float)((double)m[3] * n[1] + (double)m[6] * n[2] + (double)m[0] * n[0]);
        o[1] = (float)((double)m[4] * n[1] + (double)m[7] * n[2] + (double)m[1] * n[0]);
        o[2] = (float)((double)m[5] * n[1] + (double)m[8] * n[2] + (double)m[2] * n[0]);
    }
    F(float, s, 6688) = (float)Body_SteerAngle(s, 0);
    CallThis(Ss_Steering, s);
    {
        Matrix L;
        memcpy(s + 8196, Matrix_LookAt(&L, g_zeroVec, *(const Vec3*)n, F(float, s, 6688)), 48);
    }
    Body_4f2630(s, 0, (int)(s + 8196));
    Body_UpdateGrip(s, 0);
    CallThis(Ss_Lean, s);
    F(int, s, 6676) = 0x47C35000;
    if (x87_fabs(F(float, s, 6688) - g_ss64da00) > g_ss64b5a0)
        F(float, s, 6676) = (float)(x87_sin(F(float, s, 6688)) / x87_cos(F(float, s, 6688)));
    CallThis(Ss_Tyres, s, s + 3360);
    F(float, s, 6696) = (float)CallThis<double>(Ss_Mean30, s);
    F(float, s, 6700) = (float)CallThis<double>(Ss_Mean38, s);
    CallThis(FUN_4f35f0, s);

    float Fv[3], T[3], tmp[3];
    memcpy(Fv, &g_zeroVec, 12);
    memcpy(T, &g_zeroVec, 12);
    CallThis(Ss_Forces, s, Fv, T);
    {
        double h = (double)dt / (CallC<double>(Ss_floor, F(float, s, 9500) * g_ss64d9f8) + g_ss64b670);
        CallThis(Ss_Limit, s, Fv, T, s + 2544, s + 76, (float)h);
    }
    if (!F(BYTE, s, 6580))
    {
        float sum3[3];
        memcpy(sum3, &g_zeroVec, 12);
        float load = 0;
        for (int i = 0; i < F(int, s, 3540); i++)
        {
            load = (float)((double)load + F(float, s, 6500 + 4 * i));
            CallThis(Pb_AddAssign, sum3, tmp, s + 6308 + 12 * i);
        }
        float len = (float)Vec3_Length((const Vec3*)sum3);
        if (F(int, s, 6844) && F(float, s, 3524) < g_ss64b6e0)
        {
            memcpy(Fv, &g_zeroVec, 12);
            CallThis(Ss_ResetMotion, s);
        }
        if ((double)len + len < load && x87_fabs(F(float, s, 96)) < g_ss64b6e0)
        {
            memcpy(T, &g_zeroVec, 12);
            CallThis(Ss_StepVectors, s);
        }
    }
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        float* a = &F(float, s, 8244 + 12 * i);
        memcpy(a, s + 8340 + 12 * i, 12);
        const float* r = &F(float, s, 260 + 248 * i);
        double c0 = (double)r[1] * a[2] - (double)r[2] * a[1];
        double c1 = (double)a[0] * r[2] - (double)r[0] * a[2];
        double c2 = (double)a[1] * r[0] - (double)a[0] * r[1];
        a[0] = (float)(c1 * r[2] - c2 * r[1]);
        a[1] = (float)(c2 * r[0] - c0 * r[2]);
        a[2] = (float)(c0 * r[1] - c1 * r[0]);
    }
    memcpy(s + 3228, &g_zeroVec, 12);
    memcpy(s + 3300, &g_zeroVec, 12);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const float* v = &F(float, s, 8244 + 12 * i);
        CallThis(Pb_AddAssign, s + 3228, tmp, v);
        double x = (double)m[6] * v[2] + (double)m[0] * v[0] + (double)m[3] * v[1];
        double y = (double)m[1] * v[0] + (double)m[7] * v[2] + (double)m[4] * v[1];
        double z = (double)m[2] * v[0] + (double)v[2] * m[8] + (double)v[1] * m[5];
        const float* w = &F(float, s, 6212 + 12 * i);
        float t[3] = {(float)(z * w[1] - y * w[2]), (float)(x * w[2] - z * w[0]), (float)(y * w[0] - x * w[1])};
        CallThis(Pb_AddAssign, s + 3300, tmp, t);
    }
    {
        double ax = (double)Fv[0] + F(float, s, 3228);
        float ay = (float)((double)Fv[1] + F(float, s, 3232)), az = (float)((double)Fv[2] + F(float, s, 3236));
        double bx = ax + F(float, s, 3216);
        float by = (float)((double)ay + F(float, s, 3220)), bz = (float)((double)az + F(float, s, 3224));
        F(float, s, 3180) = (float)(bx + F(float, s, 3192));
        F(float, s, 3184) = (float)((double)by + F(float, s, 3196));
        F(float, s, 3188) = (float)((double)bz + F(float, s, 3200));
        double cx = (double)T[0] + F(float, s, 3300);
        float cy = (float)((double)T[1] + F(float, s, 3304)), cz = (float)((double)T[2] + F(float, s, 3308));
        double dx = cx + F(float, s, 3288);
        float dy = (float)((double)cy + F(float, s, 3292)), dz = (float)((double)cz + F(float, s, 3296));
        F(float, s, 3264) = (float)(dx + F(float, s, 3276));
        F(float, s, 3268) = (float)((double)dy + F(float, s, 3280));
        F(float, s, 3272) = (float)((double)dz + F(float, s, 3284));
    }
    {
        double ms = F(float, s, 9792);
        float a0 = (float)(F(float, s, 3180) / ms), a1 = (float)(F(float, s, 3184) / ms);
        double a2 = F(float, s, 3188) / ms;
        float* d = &F(float, s, 3416);
        d[0] = (float)((double)F(float, s, 3392) - a0);
        d[1] = (float)((double)F(float, s, 3396) - a1);
        d[2] = (float)(F(float, s, 3400) - a2);
        F(float, s, 3452) = (float)Vec3_Length((const Vec3*)d);
        double x = -(double)d[0], y = -(double)d[1], z = -(double)d[2];
        double nd = n[0] * x + (z * n[2] + n[1] * y);
        F(float, s, 9788) = (float)nd;
        F(float, s, 3440) = (float)(nd * n[0]);
        F(float, s, 3444) = (float)(nd * n[1]);
        F(float, s, 3448) = (float)(nd * n[2]);
    }
    memcpy(s + 6652, &g_zeroVec, 12);
    if (F(BYTE, s, 6595) && !F(BYTE, s, 6594))
    {
        double x = (double)F(float, s, 7368) + F(float, s, 7356);
        float y = (float)((double)F(float, s, 7360) + F(float, s, 7372));
        float z = (float)((double)F(float, s, 7364) + F(float, s, 7376));
        F(float, s, 6652) = (float)(x * g_ss64c258);
        F(float, s, 6656) = (float)((double)y * g_ss64c258);
        F(float, s, 6660) = (float)((double)z * g_ss64c258);
    }
    F(int, s, 6660) = 0;
    F(float, s, 3144) = (float)((double)F(float, s, 3156) + F(float, s, 3180));
    F(float, s, 3148) = (float)((double)F(float, s, 3184) + F(float, s, 3160));
    F(float, s, 3152) = (float)((double)F(float, s, 3188) + F(float, s, 3164));
    memcpy(s + 2580, &g_zeroVec, 12);
    {
        double ms = F(float, s, 9792);
        float a0 = (float)(F(float, s, 3180) / ms), a1 = (float)(F(float, s, 3184) / ms);
        double a2 = F(float, s, 3188) / ms;
        float dv[3] = {(float)((double)a0 * dt), (float)((double)a1 * dt), (float)(a2 * dt)};
        CallThis(Pb_AddAssign, s + 64, tmp, dv);
    }
    memcpy(s + 3240, s + 3264, 12);
    memcpy(s + 3120, &g_zeroVec, 12);
    float t[3] = {(float)((double)dt * F(float, s, 3240)), (float)((double)dt * F(float, s, 3244)),
                  (float)((double)dt * F(float, s, 3248))};
    {
        const float* w = &F(float, s, 76);
        float r[3], q[3], o[3];
        CallThis(Bf_Set, r, (float)((double)m[6] * w[2] + (double)m[3] * w[1] + (double)w[0] * m[0]),
                 (float)((double)m[7] * w[2] + (double)m[4] * w[1] + (double)m[1] * w[0]),
                 (float)((double)m[8] * w[2] + (double)m[5] * w[1] + (double)m[2] * w[0]));
        CallThis(Bf_Set, q, (float)((double)r[0] + t[0]), (float)((double)t[1] + r[1]), (float)((double)t[2] + r[2]));
        const float* M = &F(float, s, 16);
        CallThis(Bf_Set, o, (float)((double)q[1] * M[3] + (double)q[2] * M[6] + (double)q[0] * M[0]),
                 (float)((double)q[1] * M[4] + (double)q[2] * M[7] + (double)q[0] * M[1]),
                 (float)((double)q[1] * M[5] + (double)q[2] * M[8] + (double)q[0] * M[2]));
        memcpy(s + 76, o, 12);
        Vec3 d;
        memcpy(s + 88, Vec3_DivComponents(&d, (const Vec3*)q, (const Vec3*)(s + 8008)), 12);
    }
    if (Body_IsTipped(s, 0))
    {
        F(int, s, 13124) = 1;
        CallThis(Ss_Slide, s);
        return -1;
    }
    {
        float nb[3];
        CallThis(Bf_Set, nb, (float)((double)m[6] * n[2] + (double)m[3] * n[1] + (double)n[0] * m[0]),
                 (float)((double)m[1] * n[0] + (double)m[7] * n[2] + (double)m[4] * n[1]),
                 (float)((double)m[2] * n[0] + (double)m[8] * n[2] + (double)m[5] * n[1]));
        Vec3 c1, c2, a, b;
        Vec3_Cross(&c1, (const Vec3*)nb, (const Vec3*)(s + 88));
        Vec3_Cross(&c2, &c1, (const Vec3*)nb);
        float d = (float)Vec3_Dot((const Vec3*)(s + 88), (const Vec3*)nb);
        Vec3 dir = *Vec3_Scaled(&a, (const Vec3*)nb, d);
        float roll = (float)Vec3_Length(Vec3_Scaled(&b, &dir, dt));
        Matrix L1, L2, M1, Tm, W, Wi;
        memcpy(s + 2448, Matrix_LookAt(&L1, *(const Vec3*)(s + 6652), dir, roll), 48);
        float roll2 = (float)Vec3_Length(Vec3_Scaled(&b, &c2, dt));
        memcpy(s + 2400, Matrix_LookAt(&L2, g_zeroVec, c2, roll2), 48);
        memcpy(s + 16, Matrix_Multiply(&M1, (const Matrix*)(s + 2448), (const Matrix*)(s + 16)), 48);
        Vec3 vs, mv;
        Vec3_Add(&vs, (const Vec3*)(s + 64), (const Vec3*)(s + 2580));
        Matrix_Translation(&Tm, *Vec3_Scaled(&mv, &vs, dt));
        memcpy(s + 16, Matrix_Multiply(&W, (const Matrix*)(s + 16), &Tm), 48);
        memcpy(s + 2304, Matrix_Inverse(&Wi, (const Matrix*)(s + 16)), 48);
        Vec3 r, q;
        Vec3_RotateBy(&r, (const Vec3*)(s + 76), (const Matrix*)(s + 2304));
        memcpy(s + 88, Vec3_MulComponents(&q, &r, (const Vec3*)(s + 8020)), 12);
    }
    CallThis(Ss_Ground, s);
    memset(s + 15336, 0, 32);
    F(int, s, 15368) = 0;
    F(int, s, 15372) = 0;
    return 1;
}

} // extern "C"

// === a body's contact with a moving point (FUN_4e7b4d with its pieces up to $L_4e8644)

extern "C" Vec3* __cdecl Ct4_DivScalar(Vec3* out, const Vec3* v, float k); KING_ALIAS("_Ct4_DivScalar", "_Vec3_DivScalar")
extern "C" void __cdecl Ct4_Soften(Vec3* v); KING_ALIAS("_Ct4_Soften", "_Vec3_Soften")
extern "C" int __cdecl Ct4_Report(char* out, int n, const char* name, const Matrix* m, const Vec3* a, const Vec3* b); KING_ALIAS("_Ct4_Report", "_Frame_Report")
extern "C" void __cdecl Ct4_Fatal(const char* msg); KING_ALIAS("_Ct4_Fatal", "_World_Fatal")
extern "C" void Ct4_MulA(); KING_ALIAS("_Ct4_MulA", "@Vec3_MulAssign@16")
extern "C" void Ct4_SubA(); KING_ALIAS("_Ct4_SubA", "@Vec3_SubAssign@16")
KING_VAR(int, g_ct4zero702650, 702650)
KING_VAR(int, g_ct4zero702654, 702654)
KING_VAR(int, g_ct4zero702658, 702658)
KING_VAR(float, g_ct4_64c264, 64c264)
KING_VAR(double, g_ct4_64b958, 64b958)
KING_VAR(double, g_ct4_64b5b8, 64b5b8)
KING_VAR(double, g_ct4_64b6e0, 64b6e0)
KING_VAR(double, g_ct4_64d918, 64d918)
KING_VAR(double, g_ct4_64b5a0, 64b5a0)
KING_VAR(char, g_ct4_673ddc, 673ddc)
KING_VAR(char, g_ct4_673dfc, 673dfc)
KING_VAR(char, g_ct4_673e28, 673e28)
KING_VAR(char, g_ct4_673e60, 673e60)
KING_VAR(char, g_ct4_673e9c, 673e9c)
KING_VAR(char, g_ct4_673ed8, 673ed8)
KING_VAR(char, g_ct4_673f14, 673f14)
KING_VAR(char, g_ct4_673f34, 673f34)
KING_VAR(char, g_ct4_673f58, 673f58)
KING_VAR(char, g_ct4_673f88, 673f88)
KING_VAR(char, g_ct4_673fac, 673fac)

#define L(n) (*(float*)(fp - (n)))
#define LI(n) (*(int*)(fp - (n)))
#define LD(n) (*(double*)(fp - (n)))
#define P(n) ((Vec3*)(fp - (n)))
#define FI(p, o) (*(int*)((BYTE*)(p) + (o)))
#define FF(p, o) (*(float*)((BYTE*)(p) + (o)))

extern "C" {

// [FUN_4e7b4d..$L_4e8644] a body (pose ma: rotation and position +36, mb its world rotation, inertia ic) touching
// point pa with normal nb, the point moving with pc (position) and vc (speed): the impulse from the masses' ratio
// k24, restitution k28 and friction k32 (a quadratic for the speed after), the new position and speed into
// outP / outV; fatal reports when the ratio is out of range. The asm's EBP frame is kept as fr (L / LI / LD)
void __cdecl Body_MovingPointImpulse(const Vec3* pa, const Vec3* nb, const Matrix* ma, const Matrix* mb, float k24,
                               float k28, float k32, const Vec3* pc, const Vec3* vc, const Vec3* ic, Vec3* outP,
                               Vec3* outV)
{
    BYTE fr[4724];
    BYTE* fp = fr + 4724;
    Vec3_RotateBy(P(68), vc, ma);
    Vec3_Sub(P(288), pa, (const Vec3*)((const BYTE*)ma + 36));
    double d = Vec3_Dot(P(288), P(288));
    L(304) = (float)d;
    L(300) = (float)x87_sqrt(d);
    Ct4_DivScalar(P(212), P(288), L(300));
    Vec3_RotateBy(P(392), P(212), mb);
    Vec3_RotateBy(P(348), nb, mb);
    Vec3_Cross(P(276), nb, P(212));
    Vec3_RotateBy(P(28), pc, mb);
    LI(200) = g_ct4zero702650;
    LI(196) = g_ct4zero702654;
    LI(192) = g_ct4zero702658;
    Vec3_Cross(P(4448), P(348), P(28));
    Vec3_Cross(P(260), P(4448), P(348));
    Vec3_Cross(P(4460), nb, P(212));
    Vec3_Normalized(P(156), P(4460));
    Vec3_Cross(P(4472), P(348), P(392));
    Vec3_Normalized(P(336), P(4472));
    Vec3_RotateBy(P(380), P(156), mb);
    Vec3_MulComponents(P(16), vc, ic);
    double r19 = Vec3_Dot(P(16), P(380));
    L(4704) = (float)r19;
    Vec3_Scaled(P(124), P(380), (float)r19);
    Vec3_Sub(P(324), P(16), P(124));
    LI(92) = LI(324);
    LI(88) = LI(320);
    LI(84) = LI(316);
    LI(188) = g_ct4zero702650;
    LI(184) = g_ct4zero702654;
    LI(180) = g_ct4zero702658;
    FI(outP, 0) = FI(pc, 0);
    FI(outP, 4) = FI(pc, 4);
    FI(outP, 8) = FI(pc, 8);
    FI(outV, 0) = FI(vc, 0);
    FI(outV, 4) = FI(vc, 4);
    FI(outV, 8) = FI(vc, 8);
    Vec3_Cross(P(4484), vc, P(392));
    Vec3_Scaled(P(404), P(4484), L(300));
    Vec3_RotateBy(P(4496), P(404), ma);
    LI(404) = LI(4496);
    LI(400) = LI(4492);
    LI(396) = LI(4488);
    Vec3_Add(P(4508), pc, P(404));
    LI(404) = LI(4508);
    LI(400) = LI(4504);
    LI(396) = LI(4500);
    L(408) = (float)Vec3_Dot(P(404), nb);
    if (L(408) > g_ct4_64c264)
        return;
    L(236) = (float)Vec3_Length(P(276));
    if (L(300) * g_ct4_64b958 > L(236))
    {
        // a hit at the rim: slide along the surface
        L(468) = (float)Vec3_Dot(pc, nb);
        L(424) = (float)(-k28 * L(468));
        Vec3_Cross(P(4520), nb, pc);
        Vec3_Cross(P(420), P(4520), nb);
        Vec3_Scaled(P(464), nb, L(424));
        Vec3_Add(P(4532), P(464), P(420));
        FI(outP, 0) = LI(4532);
        FI(outP, 4) = LI(4528);
        FI(outP, 8) = LI(4524);
        double r33 = Vec3_Dot(vc, P(392));
        L(4708) = (float)r33;
        Vec3_Scaled(P(440), P(392), (float)r33);
        Vec3_Sub(P(452), vc, P(440));
        L(428) = (float)((L(424) - L(468)) * k24);
        if (L(428) > g_ct4_64c264)
        {
            L(484) = (float)Vec3_Length(P(92));
            L(488) = (float)(L(428) * k32);
            if (L(488) > L(484))
                LI(484) = 0;
            else
                L(484) = (float)((L(484) - L(488)) / L(484));
            CallThis(Ct4_MulA, P(92), P(4544), L(484));
            LI(188) = LI(124);
            LI(184) = LI(120);
            LI(180) = LI(116);
            Vec3_Add(P(480), P(188), P(92));
            Vec3_DivComponents(P(4556), P(480), ic);
            FI(outV, 0) = LI(4556);
            FI(outV, 4) = LI(4552);
            FI(outV, 8) = LI(4548);
        }
        else
            L(428) = (float)(L(428) - g_ct4_64b5b8);
        return;
    }
    L(368) = (float)Vec3_Dot(P(28), P(348));
    L(112) = (float)Vec3_Dot(P(124), P(380));
    Vec3_Cross(P(4568), P(392), P(348));
    L(308) = (float)Vec3_Length(P(4568));
    L(56) = (float)((k24 * L(300)) * L(308));
    L(108) = (float)(k24 * g_ct4_64b6e0);
    L(80) = (float)((L(368) * L(56)) + L(112));
    Vec3_DivComponents(P(4580), P(380), ic);
    L(136) = (float)(Vec3_Dot(P(380), P(4580)) * g_ct4_64b6e0);
    if (!(L(136) > g_ct4_64c264))
    {
        Ct4_Report((char*)P(788), 300, &g_ct4_673ddc, ma, pc, vc);
        FI(outP, 0) = FI(pc, 0);
        FI(outP, 4) = FI(pc, 4);
        FI(outP, 8) = FI(pc, 8);
        FI(outV, 0) = FI(vc, 0);
        FI(outV, 4) = FI(vc, 4);
        FI(outV, 8) = FI(vc, 8);
        return;
    }
    L(312) = (float)((L(368) * L(368)) * L(108));
    L(76) = (float)((L(112) * L(112)) * L(136));
    L(232) = (float)(L(312) + L(76));
    L(296) = k28;
    L(104) = (float)((L(56) * L(80)) * L(136));
    L(132) = (float)(((L(136) * L(56)) * L(56)) + L(108));
    L(168) = (float)(((L(108) * L(136)) * L(80)) * L(80));
    if (L(132) * L(232) != g_ct4_64c264)
        L(296) = (float)(L(168) / (L(132) * L(232)));
    if (L(296) > g_ct4_64d918 || L(296) < g_ct4_64c264)
    {
        char* s1 = (char*)P(2988);
        char* s2 = (char*)P(3988);
        char* s3 = (char*)P(3188);
        char* s4 = (char*)P(3488);
        char* s5 = (char*)P(3788);
        char* s6 = (char*)P(4188);
        char* s7 = (char*)P(4388);
        sprintf(s1, &g_ct4_673dfc, (double)L(296), (double)L(168), (double)L(132), (double)L(232));
        sprintf(s2, &g_ct4_673e28, (double)L(56), (double)L(80), (double)L(108), (double)L(136), (double)L(368),
                (double)L(112), (double)L(312), (double)L(76));
        sprintf(s3, &g_ct4_673e60, (double)k24, (double)k28, (double)k32, (double)FF(ic, 0), (double)FF(ic, 4),
                (double)FF(ic, 8));
        sprintf(s4, &g_ct4_673e9c, (double)FF(ma, 0), (double)FF(ma, 4), (double)FF(ma, 8), (double)FF(ma, 12),
                (double)FF(ma, 16), (double)FF(ma, 20), (double)FF(ma, 24), (double)FF(ma, 28), (double)FF(ma, 32),
                (double)FF(ma, 36), (double)FF(ma, 40), (double)FF(ma, 44));
        sprintf(s5, &g_ct4_673ed8, (double)FF(mb, 0), (double)FF(mb, 4), (double)FF(mb, 8), (double)FF(mb, 12),
                (double)FF(mb, 16), (double)FF(mb, 20), (double)FF(mb, 24), (double)FF(mb, 28), (double)FF(mb, 32),
                (double)FF(mb, 36), (double)FF(mb, 40), (double)FF(mb, 44));
        sprintf(s6, &g_ct4_673f14, (double)FF(pc, 0), (double)FF(pc, 4), (double)FF(pc, 8), (double)FF(vc, 0),
                (double)FF(vc, 4), (double)FF(vc, 8));
        sprintf(s7, &g_ct4_673f34, (double)FF(pa, 0), (double)FF(pa, 4), (double)FF(pa, 8), (double)FF(nb, 0),
                (double)FF(nb, 4), (double)FF(nb, 8));
        sprintf((char*)P(2788), &g_ct4_673f58, s1, s2, s3, s4, s5, s6, s7);
        Ct4_Fatal((char*)P(2788));
        L(296) = 1.0f;
        Ct4_Fatal(&g_ct4_673f88);
    }
    LI(292) = 0;
    if (L(292) < g_ct4_64c264)
        return;
    L(144) = (float)x87_sqrt(L(292));
    L(72) = (float)((L(104) + L(144)) / L(132));
    L(100) = (float)((L(104) - L(144)) / L(132));
    L(228) = (float)(L(80) - L(72) * L(56));
    L(248) = (float)(L(80) - L(100) * L(56));
    LD(4716) = L(368);
    Vec3_DivComponents(P(4592), P(380), ic);
    Vec3_Scaled(P(4604), P(4592), L(112));
    Vec3_Cross(P(4616), P(4604), P(392));
    Vec3_Scaled(P(4628), P(4616), L(300));
    L(140) = (float)(Vec3_Dot(P(4628), P(348)) + LD(4716));
    L(164) = (float)(((L(228) * L(136)) + (L(228) * L(136))) * L(300) + L(72));
    L(176) = (float)(L(100) - ((L(248) * L(136)) + (L(248) * L(136))) * L(300));
    L(244) = (float)(((L(72) * L(72)) * L(108)) + ((L(228) * L(228)) * L(136)));
    L(264) = (float)(((L(100) * L(100)) * L(108)) + ((L(248) * L(248)) * L(136)));
    LI(4) = LI(72);
    L(172) = (float)(L(80) - L(4) * L(56));
    L(160) = (float)((L(4) - L(368)) * k24);
    if (L(160) > g_ct4_64c264)
    {
        L(4392) = (float)Vec3_Length(P(92));
        L(4396) = (float)(L(160) * k32);
        if (!(L(4396) < L(4392)))
            LI(4392) = 0;
        else if (L(4392) != g_ct4_64c264)
            L(4392) = (float)((L(4392) - L(4396)) / L(4392));
        CallThis(Ct4_MulA, P(92), P(4640), L(4392));
    }
    Vec3_Scaled(P(4652), P(348), L(4));
    LI(200) = LI(4652);
    LI(196) = LI(4648);
    LI(192) = LI(4644);
    Vec3_Scaled(P(4664), P(380), L(172));
    LI(188) = LI(4664);
    LI(184) = LI(4660);
    LI(180) = LI(4656);
    Vec3_Add(P(52), P(200), P(260));
    Vec3_Add(P(40), P(188), P(92));
    Vec3_RotateBy(P(364), P(52), ma);
    Vec3_DivComponents(P(224), P(40), ic);
    L(352) = (float)(Vec3_Dot(P(200), P(200)) * L(108));
    L(96) = (float)(Vec3_Dot(P(188), P(188)) * L(136));
    L(240) = (float)(L(352) + L(96));
    L(128) = 1.0f;
    if (L(232) > g_ct4_64c264)
    {
        L(128) = (float)(L(240) / L(232));
        if (L(128) > g_ct4_64d918)
            Ct4_Fatal(&g_ct4_673fac);
    }
    Vec3_Cross(P(4676), P(224), P(392));
    Vec3_Scaled(P(4408), P(4676), L(300));
    Vec3_RotateBy(P(4432), P(4408), ma);
    Vec3_Add(P(4420), P(364), P(4432));
    L(4436) = (float)Vec3_Dot(P(4420), nb);
    if (L(4436) < g_ct4_64c264)
    {
        float k = (float)(L(4436) - g_ct4_64b5a0);
        L(4720) = k;
        Vec3_Scaled(P(4688), nb, k);
        CallThis(Ct4_SubA, P(364), P(4700), P(4688));
        L(4436) = (float)(L(4436) * g_ct4_64d918);
    }
    Ct4_Soften(P(224));
    FI(outP, 0) = LI(364);
    FI(outP, 4) = LI(360);
    FI(outP, 8) = LI(356);
    FI(outV, 0) = LI(224);
    FI(outV, 4) = LI(220);
    FI(outV, 8) = LI(216);
}

} // extern "C"

#undef L
#undef LI
#undef LD
#undef P
#undef FI
#undef FF

// === a body's contact with a fixed point, solved as a 4x4 system (FUN_4e8ae1 with its tail $L_4e964c)

struct Ct5Row { double v[4]; };
extern "C" Ct5Row* __fastcall Ct5_RowSet(Ct5Row* s, void*, Ct5Row v); KING_ALIAS("@Ct5_RowSet@40", "@DRow_Set@40")
extern "C" BYTE* __fastcall Ct5_FromRows(BYTE* s, void*, const Ct5Row* a, const Ct5Row* b, const Ct5Row* c, const Ct5Row* d); KING_ALIAS("@Ct5_FromRows@24", "@DMat_FromRows@24")
extern "C" BYTE* __cdecl Ct5_DInverse(BYTE* out, const BYTE* m); KING_ALIAS("_Ct5_DInverse", "_DMat_Inverse")
extern "C" BYTE* __cdecl Ct5_Combine(BYTE* out, BYTE* a, BYTE* b); KING_ALIAS("_Ct5_Combine", "_Rec128_Combine")
extern "C" Ct5Row* __cdecl Ct5_MulRow(Ct5Row* out, const BYTE* m, const Ct5Row* v); KING_ALIAS("_Ct5_MulRow", "_DMat_MulRow")
extern "C" int __cdecl Ct5_OutOfBox(const void* v, float lim); KING_ALIAS("_Ct5_OutOfBox", "_Vec3_OutOfBox")
extern "C" void Ct5_SetThis(); KING_ALIAS("_Ct5_SetThis", "@Vec3_SetThis@20")
KING_VAR(BYTE, g_ct5Identity7025f0, 7025f0)
KING_VAR(BYTE, g_ct5Axis702640, 702640)
KING_VAR(BYTE, g_ct5Axis702620, 702620)
KING_VAR(double, g_ct5_64b660, 64b660)
KING_VAR(double, g_ct5_64b678, 64b678)
KING_VAR(float, g_ct5_64c250, 64c250)
KING_VAR(char, g_ct5_673fcc, 673fcc)
KING_VAR(char, g_ct5_674004, 674004)
KING_VAR(char, g_ct5_67402c, 67402c)
KING_VAR(char, g_ct5_674068, 674068)
KING_VAR(char, g_ct5_6740a4, 6740a4)
KING_VAR(char, g_ct5_6740c4, 6740c4)
KING_VAR(char, g_ct5_6740e8, 6740e8)

#define L(n) (*(float*)(fp - (n)))
#define LI(n) (*(int*)(fp - (n)))
#define LD(n) (*(double*)(fp - (n)))
#define P(n) ((Vec3*)(fp - (n)))
#define M(n) ((Matrix*)(fp - (n)))
#define FI(p, o) (*(int*)((BYTE*)(p) + (o)))
#define FF(p, o) (*(float*)((BYTE*)(p) + (o)))

static Ct5Row Ct5_Row(double a, double b, double c, double d)
{
    Ct5Row r;
    r.v[0] = a;
    r.v[1] = b;
    r.v[2] = c;
    r.v[3] = d;
    return r;
}

extern "C" {

// [FUN_4e8ae1..$L_4e964c] a body (pose ma, world rotation mb, speed vel, spin spin, inverse inertia ic) touching
// point pt with normal nrm: the contact frame (normal, tangent, binormal), the impulse from a 4x4 system of the
// masses' ratio k36, restitution k40 and friction k44, the new speed and spin into outV / outW; fatal when either
// leaves its range. The asm's EBP frame is kept as fr (L / LI / LD)
void __cdecl Body_FixedPointImpulse(const Matrix* ma, const Matrix* mb, const Vec3* vel, const Vec3* spin,
                                    const Vec3* ic, const Vec3* pt, const Vec3* nrm, float k36, float k40,
                                    float k44, Vec3* outV, Vec3* outW)
{
    BYTE fr[4952];
    BYTE* fp = fr + 4952;
    Vec3_Sub(P(536), pt, (const Vec3*)((const BYTE*)ma + 36));
    Vec3_Normalized(P(268), P(536));
    Vec3_Cross(P(4632), nrm, P(268));
    Vec3_Normalized(P(680), P(4632));
    Vec3_Cross(P(76), P(268), P(680));
    Vec3_RotateBy(P(4644), spin, ma);
    Vec3_Cross(P(4656), P(4644), P(536));
    Vec3_Add(P(1216), vel, P(4656));
    L(804) = (float)Vec3_Length(P(536));
    if (L(804) == g_ct4_64c264)
    {
        FI(outV, 0) = FI(vel, 0);
        FI(outV, 4) = FI(vel, 4);
        FI(outV, 8) = FI(vel, 8);
        FI(outW, 0) = FI(spin, 0);
        FI(outW, 4) = FI(spin, 4);
        FI(outW, 8) = FI(spin, 8);
    }
    if (Vec3_Dot(P(680), P(680)) == g_ct5_64b678)
    {
        Vec3_Cross(P(4668), P(268), (const Vec3*)&g_ct5Axis702640);
        Vec3_Cross(P(4680), P(4668), P(268));
        Vec3_Normalized(P(4692), P(4680));
        LI(680) = LI(4692);
        LI(676) = LI(4688);
        LI(672) = LI(4684);
        if (Vec3_Dot(P(680), P(680)) == g_ct5_64b678)
        {
            Vec3_Cross(P(4704), P(268), (const Vec3*)&g_ct5Axis702620);
            Vec3_Cross(P(4716), P(4704), P(268));
            Vec3_Normalized(P(4728), P(4716));
            LI(680) = LI(4728);
            LI(676) = LI(4724);
            LI(672) = LI(4720);
        }
        Vec3_Cross(P(4740), P(268), P(680));
        LI(76) = LI(4740);
        LI(72) = LI(4736);
        LI(68) = LI(4732);
    }
    L(916) = (float)Vec3_Dot(vel, P(268));
    L(1204) = (float)(-k40 * L(916));
    if (L(916) < g_ct4_64c264)
        LI(1204) = LI(916);
    memcpy(fp - 424, ma, 48);
    memcpy(fp - 524, mb, 48);
    memcpy(fp - 1200, &g_ct5Identity7025f0, 48);
    LI(388) = g_ct4zero702650;
    LI(384) = g_ct4zero702654;
    LI(380) = g_ct4zero702658;
    LI(488) = g_ct4zero702650;
    LI(484) = g_ct4zero702654;
    LI(480) = g_ct4zero702658;
    LI(1200) = LI(680);
    LI(1196) = LI(676);
    LI(1192) = LI(672);
    LI(1188) = LI(76);
    LI(1184) = LI(72);
    LI(1180) = LI(68);
    LI(1176) = LI(268);
    LI(1172) = LI(264);
    LI(1168) = LI(260);
    Matrix_Inverse(M(472), M(1200));
    Matrix_Multiply(M(1152), M(424), M(472));
    Matrix_Inverse(M(376), M(1152));
    Matrix_ScaleColumns(M(4788), (const Matrix*)&g_ct5Identity7025f0, ic);
    Matrix_Multiply(M(4836), M(524), M(4788));
    Matrix_Multiply(M(1104), M(4836), M(424));
    Matrix_ScaleColumns(M(4884), (const Matrix*)&g_ct5Identity7025f0, ic);
    Matrix_Multiply(M(4932), M(376), M(4884));
    Matrix_Multiply(M(64), M(4932), M(1152));
    Matrix_Inverse(M(328), M(64));
    Vec3_MulComponents(P(16), ic, spin);
    Vec3_RotateBy(P(864), P(16), M(1152));
    Vec3_RotateBy(P(724), vel, M(472));
    CallThis(Ct5_SetThis, P(800), 0.0f, 0.0f, L(1204));
    L(256) = (float)(L(856) - (k44 * k36) * (L(716) - L(792)));
    if (L(256) * L(856) <= g_ct4_64c264)
        LI(256) = 0;
    CallThis(Ct5_SetThis, P(912), 0.0f, 0.0f, L(256));
    double r0 = L(804) * k36;
    Ct5_RowSet((Ct5Row*)(fp - 712), 0, Ct5_Row(1.0, 0.0, 0.0, r0));
    double r1 = -L(804) * k36;
    Ct5_RowSet((Ct5Row*)(fp - 788), 0, Ct5_Row(0.0, 1.0, r1, 0.0));
    double r2a = L(804) * L(312);
    double r2b = L(804) * L(324);
    Ct5_RowSet((Ct5Row*)(fp - 852), 0, Ct5_Row(r2b, r2a, 1.0, 0.0));
    double r3a = -L(804) * L(316);
    double r3b = -L(804) * L(328);
    Ct5_RowSet((Ct5Row*)(fp - 900), 0, Ct5_Row(r3b, r3a, 0.0, 1.0));
    double e = (L(804) * L(304)) * L(256);
    double f = (-L(804) * L(300)) * L(256);
    double g = L(860) - (L(804) * k36) * L(724);
    double h = (L(804) * k36) * L(720) + L(864);
    Ct5_RowSet((Ct5Row*)(fp - 252), 0, Ct5_Row(h, g, f, e));
    Ct5_FromRows(fp - 220, 0, (Ct5Row*)(fp - 712), (Ct5Row*)(fp - 788), (Ct5Row*)(fp - 852), (Ct5Row*)(fp - 900));
    Ct5_DInverse(fp - 1056, fp - 220);
    Ct5_Combine(fp - 668, fp - 220, fp - 1056);
    Ct5_MulRow((Ct5Row*)(fp - 756), fp - 1056, (Ct5Row*)(fp - 252));
    L(912) = (float)LD(756);
    L(908) = (float)LD(748);
    L(800) = (float)LD(740);
    L(796) = (float)LD(732);
    Vec3_RotateBy(P(92), P(912), M(376));
    Vec3_DivComponents(P(820), P(92), ic);
    Vec3_RotateBy(P(280), P(800), M(1200));
    Vec3_RotateBy(P(928), spin, ma);
    LD(4940) = k36;
    L(4) = (float)((Vec3_Dot(vel, vel) * LD(4940)) / g_ct5_64b660);
    LD(4948) = k36;
    L(80) = (float)((Vec3_Dot(P(280), P(280)) * LD(4948)) / g_ct5_64b660);
    L(476) = (float)(Vec3_Dot(P(16), spin) / g_ct5_64b660);
    L(540) = (float)(Vec3_Dot(P(92), P(820)) / g_ct5_64b660);
    L(808) = (float)(L(4) + L(476));
    L(868) = (float)(L(80) + L(540));
    if (L(808) != g_ct4_64c264)
    {
        L(1220) = (float)(L(868) / L(808));
        if (L(1220) > g_ct5_64c250)
            L(1220) = (float)(L(1220) + L(1220));
    }
    if (Ct5_OutOfBox(outV, 400.0f) || Ct5_OutOfBox(P(820), 200.0f))
    {
        char* s1 = (char*)(fp - 3420);
        char* s2 = (char*)(fp - 3620);
        char* s3 = (char*)(fp - 3920);
        char* s4 = (char*)(fp - 4220);
        char* s5 = (char*)(fp - 4420);
        char* s6 = (char*)(fp - 4620);
        sprintf(s1, &g_ct5_673fcc, (double)FF(outV, 0), (double)FF(outV, 4), (double)FF(outV, 8), (double)L(820),
                (double)L(816), (double)L(812));
        sprintf(s2, &g_ct5_674004, (double)k36, (double)k40, (double)k44, (double)FF(ic, 0), (double)FF(ic, 4),
                (double)FF(ic, 8));
        sprintf(s3, &g_ct5_67402c, (double)FF(ma, 0), (double)FF(ma, 4), (double)FF(ma, 8), (double)FF(ma, 12),
                (double)FF(ma, 16), (double)FF(ma, 20), (double)FF(ma, 24), (double)FF(ma, 28), (double)FF(ma, 32),
                (double)FF(ma, 36), (double)FF(ma, 40), (double)FF(ma, 44));
        sprintf(s4, &g_ct5_674068, (double)FF(mb, 0), (double)FF(mb, 4), (double)FF(mb, 8), (double)FF(mb, 12),
                (double)FF(mb, 16), (double)FF(mb, 20), (double)FF(mb, 24), (double)FF(mb, 28), (double)FF(mb, 32),
                (double)FF(mb, 36), (double)FF(mb, 40), (double)FF(mb, 44));
        sprintf(s5, &g_ct5_6740a4, (double)FF(vel, 0), (double)FF(vel, 4), (double)FF(vel, 8), (double)FF(spin, 0),
                (double)FF(spin, 4), (double)FF(spin, 8));
        sprintf(s6, &g_ct5_6740c4, (double)FF(pt, 0), (double)FF(pt, 4), (double)FF(pt, 8), (double)FF(nrm, 0),
                (double)FF(nrm, 4), (double)FF(nrm, 8));
        sprintf((char*)(fp - 3220), &g_ct5_6740e8, s1, s2, s3, s4, s5, s6);
        Ct4_Fatal((char*)(fp - 3220));
    }
    Ct4_Soften(P(820));
    FI(outW, 0) = LI(820);
    FI(outW, 4) = LI(816);
    FI(outW, 8) = LI(812);
    FI(outV, 0) = LI(280);
    FI(outV, 4) = LI(276);
    FI(outV, 8) = LI(272);
}

} // extern "C"

#undef L
#undef LI
#undef LD
#undef P
#undef M
#undef FI
#undef FF

// === a body's contact impulse, simple form ($L_4e969e with its tail $L_4e96e8; nothing calls it in this build)

extern "C" Vec3* __cdecl Ct6_Negated(Vec3* out, const Vec3* v); KING_ALIAS("_Ct6_Negated", "_Vec3_Negated")
extern "C" void Ct6_DivA(); KING_ALIAS("_Ct6_DivA", "@Vec3_DivAssign@16")
extern "C" void __fastcall Ct6_Zero(Vec3* v, void*); KING_ALIAS("@Ct6_Zero@8", "@Vec3_ZeroThis@8")
KING_VAR(double, g_ct6_64d950, 64d950)
KING_VAR(double, g_ct6_64c958, 64c958)
KING_VAR(double, g_ct6_64b748, 64b748)
KING_VAR(double, g_ct6_64d948, 64d948)
KING_VAR(BYTE, g_ct6_6974a8, 6974a8)

#define L(n) (*(float*)(fp - (n)))
#define LI(n) (*(int*)(fp - (n)))
#define LD(n) (*(double*)(fp - (n)))
#define P(n) ((Vec3*)(fp - (n)))
#define FI(p, o) (*(int*)((BYTE*)(p) + (o)))
#define FF(p, o) (*(float*)((BYTE*)(p) + (o)))

static float Ct6_Bits(DWORD b) { return *(float*)&b; }

extern "C" {

// [$L_4e969e..$L_4e96e8] body (pose ma, rotation mb, speed vel, spin spin, inertia ic) touching point pt with
// normal nrm: the contact direction (bent towards the normal when nearly flat), the speed's normal part reversed
// by k36 and its sliding part by k40, the spin from the lever arm, both scaled down when the energy would grow;
// into outW / outV. The asm's EBP frame is kept as fr
void __cdecl Body_ContactImpulseB(const Matrix* ma, const Matrix* mb, const Vec3* vel, const Vec3* spin,
                                  const Vec3* ic, const Vec3* pt, const Vec3* nrm, float k36, float k40,
                                  Vec3* outW, Vec3* outV)
{
    BYTE fr[576];
    BYTE* fp = fr + 576;
    Vec3_Sub(P(188), (const Vec3*)((const BYTE*)ma + 36), pt);
    double d = Vec3_Dot(P(188), P(188));
    L(220) = (float)d;
    L(204) = (float)x87_sqrt(d);
    L(104) = 1.0f;
    Ct4_DivScalar(P(132), P(188), L(204));
    Vec3_RotateBy(P(72), spin, ma);
    Ct6_Zero(P(24), 0);
    Ct6_Zero(P(60), 0);
    Ct6_Zero(P(96), 0);
    LI(100) = 0;
    LI(108) = 0;
    if (!(FF(nrm, 8) <= g_ct6_64d950) && !(Vec3_Dot(P(132), nrm) <= g_ct6_64c958))
    {
        Vec3_Cross(P(284), nrm, P(132));
        Vec3_Cross(P(296), P(284), nrm);
        LI(24) = LI(296);
        LI(20) = LI(292);
        LI(16) = LI(288);
        double r12 = Vec3_Dot(P(132), nrm);
        L(516) = (float)r12;
        Vec3_Scaled(P(308), nrm, (float)r12);
        LI(60) = LI(308);
        LI(56) = LI(304);
        LI(52) = LI(300);
        L(100) = (float)Vec3_Length(P(24));
        if (!(L(100) <= g_ct6_64b748))
        {
            L(108) = (float)Vec3_Length(P(60));
            float ka = (float)(g_ct6_64d948 / L(108));
            L(520) = ka;
            Vec3_Scaled(P(320), P(60), ka);
            float kb = (float)(g_ct6_64c958 / L(100));
            L(524) = kb;
            Vec3_Scaled(P(332), P(24), kb);
            Vec3_Add(P(344), P(332), P(320));
            LI(96) = LI(344);
            LI(92) = LI(340);
            LI(88) = LI(336);
            LI(132) = LI(96);
            LI(128) = LI(92);
            LI(124) = LI(88);
        }
    }
    double r19 = Vec3_Dot(vel, P(132));
    L(528) = (float)r19;
    Vec3_Scaled(P(176), P(132), (float)r19);
    Vec3_Cross(P(356), P(132), vel);
    Vec3_Cross(P(216), P(356), P(132));
    double r23 = Vec3_Dot(P(72), P(132));
    L(532) = (float)r23;
    Vec3_Scaled(P(200), P(132), (float)r23);
    Vec3_Cross(P(368), P(132), P(72));
    Vec3_Cross(P(232), P(368), P(132));
    Ct6_Negated(P(380), P(176));
    Vec3_Scaled(P(244), P(380), k40);
    Vec3_Cross(P(392), P(188), P(216));
    Vec3_Scaled(P(404), P(392), k36);
    Vec3_RotateBy(P(416), P(232), mb);
    Vec3_MulComponents(P(428), ic, P(416));
    Vec3_RotateBy(P(440), P(428), ma);
    Vec3_Add(P(160), P(440), P(404));
    Vec3_Scaled(P(452), (const Vec3*)&g_ct6_6974a8, k36);
    Vec3_Scaled(P(464), P(452), L(220));
    Vec3_Add(P(120), ic, P(464));
    Vec3_DivComponents(P(48), P(160), P(120));
    LI(104) = 1058642330;
    if (Vec3_Dot(P(48), P(232)) < g_ct5_64b678)
        LI(104) = 1050253722;
    Vec3_Scaled(P(36), P(48), L(104));
    Vec3_Cross(P(12), P(36), P(188));
    Vec3_Scaled(P(268), P(200), Ct6_Bits(1064849900));
    Vec3_Add(P(84), P(36), P(268));
    Vec3_RotateBy(P(148), P(84), mb);
    Vec3_Add(P(256), P(12), P(244));
    LD(540) = k36;
    LD(548) = (Vec3_Dot(vel, vel) * LD(540)) * g_ct4_64b6e0;
    Vec3_MulComponents(P(476), ic, spin);
    L(136) = (float)(Vec3_Dot(P(476), spin) * g_ct4_64b6e0 + LD(548));
    LD(556) = k36;
    LD(564) = (Vec3_Dot(P(256), P(256)) * LD(556)) * g_ct4_64b6e0;
    Vec3_MulComponents(P(488), ic, P(148));
    L(164) = (float)(Vec3_Dot(P(488), P(148)) * g_ct4_64b6e0 + LD(564));
    if (L(136) * g_ct4_64d918 < L(164) && !(L(136) <= g_ct4_64c264))
    {
        L(272) = L(164) / L(136);
        float k = (float)(L(272) + g_ct4_64b5b8);
        L(568) = k;
        CallThis(Ct6_DivA, P(256), P(500), k);
        L(572) = k;
        CallThis(Ct6_DivA, P(148), P(512), k);
    }
    FI(outV, 0) = LI(148);
    FI(outV, 4) = LI(144);
    FI(outV, 8) = LI(140);
    FI(outW, 0) = LI(256);
    FI(outW, 4) = LI(252);
    FI(outW, 8) = LI(248);
}

} // extern "C"

#undef L
#undef LI
#undef LD
#undef P
#undef FI
#undef FF

// === a body's sub-step with its wheels on springs (FUN_4f8260 with its tail $L_4f9489)

extern "C" void __fastcall Wheel_RollDrag(BYTE* s, void*, BYTE* nd, int i, int kind);
extern "C" void __fastcall Vec3_NormalizeThis(Vec3* self, void*);
extern "C" Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*);
extern "C" Vec3* __fastcall Vec3_AddAssign(Vec3* self, void*, Vec3* ret, const Vec3* v);
KING_VAR(int, g_sv702650, 702650)
KING_VAR(int, g_sv702654, 702654)
KING_VAR(int, g_sv702658, 702658)
KING_VAR(float, g_sv702620, 702620)
KING_VAR(float, g_sv702624, 702624)
KING_VAR(float, g_sv702628, 702628)
KING_VAR(float, g_sv702630, 702630)
KING_VAR(float, g_sv702634, 702634)
KING_VAR(float, g_sv702638, 702638)
KING_VAR(float, g_sv64c258, 64c258)
KING_VAR(float, g_sv64c450, 64c450)
KING_VAR(float, g_sv64c264, 64c264)
KING_VAR(float, g_sv64c938, 64c938)
KING_VAR(float, g_sv674454, 674454)
KING_VAR(float, g_sv674450, 674450)
KING_VAR(double, g_sv64b678, 64b678)
KING_VAR(double, g_sv64b6a0, 64b6a0)
KING_VAR(double, g_sv64b670, 64b670)
KING_VAR(double, g_sv64b5b8, 64b5b8)
KING_VAR(double, g_sv64b6e0, 64b6e0)
KING_VAR(double, g_sv64b6b8, 64b6b8)
KING_VAR(double, g_sv64d950, 64d950)
KING_VAR(double, g_sv64d0f0, 64d0f0)
KING_VAR(double, g_sv64b6d8, 64b6d8)

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))
#define SF(n) F(float, s, n)

extern "C" {

// [FUN_4f8260..$L_4f9489] one sub-step of body s (modes 2 and 3 of +9784): from the second on, the wheel frames,
// tyre and drive forces, the wheels' force and torque limited and the contact sums (+3180, +3264); then the frame
// turned by the spin and moved by the speed (+9012), its inverse (+8964); each wheel's contact with the ground:
// travel (+9244), spring and damper forces (+8740, pushed out by an impulse when pressed in), slip flags (+6596);
// the wheel and body contact forces summed into the force (+9072) and torque (+9144) and integrated unless
// frozen (+10104); 1 while sub-steps (+9524 counted up to +9520) remain
int __fastcall Body_SubStep(BYTE* s, void*, int mode)
{
    BYTE fr[1760];
    if (F(int, s, 9524) > 1)
    {
        I(128) = g_sv702650;
        I(132) = g_sv702654;
        I(136) = g_sv702658;
        I(48) = g_sv702650;
        I(52) = g_sv702654;
        I(56) = g_sv702658;
        V(88) = (float)(((SF(9012) * SF(212) - SF(9016) * SF(208)) * SF(212)) - ((SF(208) * SF(9020) - SF(9012) * SF(216)) * SF(216)));
        I(60) = I(88);
        V(92) = (float)(((SF(9016) * SF(216) - SF(9020) * SF(212)) * SF(216)) - ((SF(9012) * SF(212) - SF(9016) * SF(208)) * SF(208)));
        I(64) = I(92);
        V(96) = (float)(((SF(208) * SF(9020) - SF(9012) * SF(216)) * SF(208)) - ((SF(9016) * SF(216) - SF(9020) * SF(212)) * SF(212)));
        I(68) = I(96);
        {
            const BYTE* m = s + 8868;
            double ux = (g_sv702628 * F(float, m, 24) + g_sv702624 * F(float, m, 12)) + g_sv702620 * F(float, m, 0);
            double uy = (g_sv702628 * F(float, m, 28) + g_sv702624 * F(float, m, 16)) + g_sv702620 * F(float, m, 4);
            double uz = (g_sv702628 * F(float, m, 32) + g_sv702624 * F(float, m, 20)) + g_sv702620 * F(float, m, 8);
            V(88) = (float)(uy * SF(216) - uz * SF(212));
            V(92) = (float)(uz * SF(208) - ux * SF(216));
            double cz = ux * SF(212) - uy * SF(208);
            V(140) = (float)(cz * SF(212) - V(92) * SF(216));
            I(20) = I(140);
            V(144) = (float)(V(88) * SF(216) - cz * SF(208));
            I(24) = I(144);
            V(148) = (float)(V(92) * SF(208) - V(88) * SF(212));
            I(28) = I(148);
        }
        Vec3_NormalizeThis((Vec3*)(fr + 20), 0);
        Wheels_StepFrames(s, 0, (const float*)(s + 8196), (const float*)(s + 8868), (const float*)(s + 8964), (const Vec3*)(fr + 60), (const Vec3*)(s + 9024),
                      (float)((V(28) * V(68) + V(24) * V(64)) + V(20) * V(60)), (const Vec3*)(fr + 20));
        Body_SlopeLean(s);
        const float* grip;
        if (F(int, s, 10092) == 0 && F(int, s, 16204) != 1)
            grip = (const float*)(s + 8740);
        else
        {
            for (int k = 0; k < F(int, s, 3540); k++)
                V(516 + 4 * k) = (float)x87_fabs(F(float, s, 8740 + 4 * k));
            grip = &V(516);
        }
        Tyre_Forces(s, 0, grip);
        Wheels_DriveForces(s);
        for (int i = 0; i < F(int, s, 3540); i++)
        {
            BYTE* w = s + 3848 + 12 * i;
            F(int, w, 0) = g_sv702650;
            F(int, w, 4) = g_sv702654;
            F(int, w, 8) = g_sv702658;
            F(int, s, 3816 + 4 * i) = 0;
            if (F(WORD, s, 272 + 248 * i) != 0 && F(WORD, s, 6564 + 2 * i) == 0)
                Wheel_RollDrag(s, 0, s + 13584 + 208 * i, i, 0);
        }
        Wheels_ForceTorque(s, 0, (Vec3*)(fr + 128), (Vec3*)(fr + 48));
        Body_LimitChange(s, 0, (Vec3*)(fr + 128), (Vec3*)(fr + 48), (Vec3*)(fr + 60), (const Vec3*)(s + 9024), SF(9508));
        F(int, s, 3228) = g_sv702650;
        F(int, s, 3232) = g_sv702654;
        F(int, s, 3236) = g_sv702658;
        F(int, s, 3300) = g_sv702650;
        F(int, s, 3304) = g_sv702654;
        F(int, s, 3308) = g_sv702658;
        memset(fr + 896, 0, 96);
        I(72) = 0;
        for (int i = 0; i < F(int, s, 3540); i++)
        {
            BYTE* q = fr + 900 + 12 * i;
            const BYTE* r = s + 264 + 248 * i;
            const BYTE* a = s + 8344 + 12 * i;
            V(88) = (float)(((F(float, r, 4) * F(float, a, -4) - F(float, a, 4) * F(float, r, -4)) * F(float, r, 4))
                            - ((F(float, r, -4) * F(float, a, 0) - F(float, a, -4) * F(float, r, 0)) * F(float, r, 0)));
            F(int, q, -4) = I(88);
            V(92) = (float)(((F(float, r, -4) * F(float, a, 0) - F(float, a, -4) * F(float, r, 0)) * F(float, r, -4))
                            - ((F(float, a, 4) * F(float, r, 0) - F(float, r, 4) * F(float, a, 0)) * F(float, r, 4)));
            F(int, q, 0) = I(92);
            V(96) = (float)(((F(float, a, 4) * F(float, r, 0) - F(float, r, 4) * F(float, a, 0)) * F(float, r, 0))
                            - ((F(float, r, 4) * F(float, a, -4) - F(float, a, 4) * F(float, r, -4)) * F(float, r, -4)));
            F(int, q, 4) = I(96);
            Vec3_AddAssign((Vec3*)(s + 3228), 0, (Vec3*)(fr + 168), (const Vec3*)(q - 4));
            V(140) = (float)((((F(float, q, 4) * SF(2336) + F(float, q, -4) * SF(2312)) + F(float, q, 0) * SF(2324)) * F(float, a, -2128))
                             - (((F(float, q, -4) * SF(2308) + F(float, q, 4) * SF(2332)) + SF(2320) * F(float, q, 0)) * F(float, a, -2124)));
            I(32) = I(140);
            V(144) = (float)((((F(float, q, 4) * SF(2328) + SF(2304) * F(float, q, -4)) + SF(2316) * F(float, q, 0)) * F(float, a, -2124))
                             - (((F(float, q, 4) * SF(2336) + F(float, q, -4) * SF(2312)) + F(float, q, 0) * SF(2324)) * F(float, a, -2132)));
            I(36) = I(144);
            V(148) = (float)((((F(float, q, -4) * SF(2308) + F(float, q, 4) * SF(2332)) + SF(2320) * F(float, q, 0)) * F(float, a, -2132))
                             - (((F(float, q, 4) * SF(2328) + SF(2304) * F(float, q, -4)) + SF(2316) * F(float, q, 0)) * F(float, a, -2128)));
            I(40) = I(148);
            Vec3_AddAssign((Vec3*)(s + 3300), 0, (Vec3*)(fr + 76), (const Vec3*)(fr + 32));
            I(72) = i + 1;
        }
        V(36) = V(132) + SF(3232);
        V(40) = V(136) + SF(3236);
        V(116) = V(36) + SF(3220);
        V(120) = V(40) + SF(3224);
        V(88) = (float)(((V(128) + SF(3228)) + SF(3216)) + SF(3192));
        V(92) = V(116) + SF(3196);
        F(int, s, 3180) = I(88);
        V(96) = V(120) + SF(3200);
        F(int, s, 3184) = I(92);
        F(int, s, 3188) = I(96);
        V(36) = V(52) + SF(3304);
        V(40) = V(56) + SF(3308);
        V(116) = V(36) + SF(3292);
        V(120) = V(40) + SF(3296);
        V(88) = (float)(((V(48) + SF(3300)) + SF(3288)) + SF(3276));
        V(92) = V(116) + SF(3280);
        F(int, s, 3264) = I(88);
        V(96) = V(120) + SF(3284);
        F(int, s, 3268) = I(92);
        F(int, s, 3272) = I(96);
        memcpy(s + 9108, s + 3180, 12);
        memcpy(s + 9168, s + 3264, 12);
    }

    // $L_4f8961: the frame's pose
    I(156) = 0;
    I(160) = 0;
    I(164) = 0;
    memset(fr + 324, 0, 96);
    memset(fr + 800, 0, 96);
    memset(fr + 992, 0, 96);
    memset(fr + 1232, 0, 96);
    memset(fr + 692, 0, 96);
    memset(fr + 1520, 0, 96);
    memset(fr + 1520, 0, 96);
    SF(9504) = (float)((double)SF(9512) + SF(9504));
    {
        double vx = SF(9060) * g_sv64c258;
        V(36) = (float)(SF(9064) * g_sv64c258);
        V(40) = (float)(SF(9068) * g_sv64c258);
        V(20) = (float)(SF(9048) - vx);
        V(24) = SF(9052) - V(36);
        V(28) = SF(9056) - V(40);
        double dt = SF(9512);
        V(60) = (float)(V(20) * dt);
        I(32) = I(60);
        V(64) = (float)(dt * V(24));
        I(36) = I(64);
        V(68) = (float)(dt * V(28));
        I(40) = I(68);
        float ang = (float)Vec3_Length((const Vec3*)(fr + 32));
        Matrix* r = Matrix_LookAt((Matrix*)(fr + 692), g_zeroVec, *(const Vec3*)(fr + 20), ang);
        memcpy(s + 9700, r, 48);
        double px = SF(9012) + SF(2580);
        V(36) = SF(9016) + SF(2584);
        V(40) = SF(9020) + SF(2588);
        I(44) = F(int, s, 9512);
        V(20) = (float)(px * V(44));
        V(24) = V(44) * V(36);
        V(28) = V(44) * V(40);
        Matrix* t = Matrix_Translation((Matrix*)(fr + 1184), *(const Vec3*)(fr + 20));
        memcpy(s + 8916, s + 8868, 48);
        Matrix* a = Matrix_Multiply((Matrix*)(fr + 692), (const Matrix*)(s + 9700), (const Matrix*)(s + 8916));
        memcpy(s + 8868, Matrix_Multiply((Matrix*)(fr + 516), a, t), 48);
        memcpy(s + 8964, Matrix_Inverse((Matrix*)(fr + 692), (const Matrix*)(s + 8868)), 48);
    }
    {
        const BYTE* M = s + 8964;
        double x = (F(float, M, 12) * SF(212) + F(float, M, 24) * SF(216)) + SF(208) * F(float, M, 0);
        double y = (F(float, M, 4) * SF(208) + F(float, M, 16) * SF(212)) + F(float, M, 28) * SF(216);
        double z = (F(float, M, 8) * SF(208) + F(float, M, 20) * SF(212)) + F(float, M, 32) * SF(216);
        SF(9624) = (float)x;
        SF(9628) = (float)y;
        SF(9632) = (float)z;
        x = (F(float, M, 24) * SF(9032) + F(float, M, 12) * SF(9028)) + SF(9024) * F(float, M, 0);
        y = (F(float, M, 28) * SF(9032) + F(float, M, 16) * SF(9028)) + F(float, M, 4) * SF(9024);
        z = (F(float, M, 32) * SF(9032) + F(float, M, 20) * SF(9028)) + F(float, M, 8) * SF(9024);
        SF(9036) = (float)x;
        SF(9040) = (float)y;
        SF(9044) = (float)z;
        memcpy(s + 9048, Vec3_DivComponents((Vec3*)(fr + 168), (const Vec3*)(s + 9036), (const Vec3*)(s + 8008)), 12);
        V(48) = (float)((g_sv702638 * SF(8892) + g_sv702634 * SF(8880)) + g_sv702630 * SF(8868));
        V(52) = (float)((g_sv702638 * SF(8896) + g_sv702634 * SF(8884)) + g_sv702630 * SF(8872));
        V(56) = (float)((g_sv702638 * SF(8900) + g_sv702634 * SF(8888)) + g_sv702630 * SF(8876));
    }

    // $L_4f8df7: each wheel's contact
    const BYTE* m = s + 8868;
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const int e = 12 * i;
        F(int, s, 9276 + 4 * i) = F(int, s, 9244 + 4 * i);
        F(int, s, 8740 + 4 * i) = 0;
        I(292 + 4 * i) = 0;
        double f = -SF(7028 + 4 * i);
        memcpy(fr + 692 + e, &g_zeroVec, 12);
        memcpy(fr + 1520 + e, &g_zeroVec, 12);
        I(452 + 4 * i) = 0;
        I(484 + 4 * i) = 0;
        const BYTE* p = s + 7356 + e;
        V(100) = (float)(f * g_sv702630);
        V(104) = (float)(f * g_sv702634);
        double z = f * g_sv702638 + F(float, p, 8);
        V(60) = V(100) + F(float, p, 0);
        V(64) = V(104) + F(float, p, 4);
        V(20) = (float)(((z * F(float, m, 24) + V(64) * F(float, m, 12)) + V(60) * F(float, m, 0)) + F(float, m, 36));
        V(24) = (float)(((V(60) * F(float, m, 4) + z * F(float, m, 28)) + V(64) * F(float, m, 16)) + F(float, m, 40));
        V(28) = (float)(((V(60) * F(float, m, 8) + z * F(float, m, 32)) + V(64) * F(float, m, 20)) + F(float, m, 44));
        const BYTE* r = s + 248 * i;
        BYTE* c = s + 9528 + e;
        if (F(WORD, r, 272) && F(WORD, r, 4144)
            && Wheel_SegmentContact(s, 0, (Wc_V*)c, (Wc_V*)(fr + 324 + e), (const float*)(s + 8868),
                                    (const Wc_V*)(s + 9308 + e), (const Wc_V*)(s + 9404 + e), i))
        {
            V(280) = V(20) - F(float, c, 0);
            I(156) = I(280);
            V(284) = V(24) - F(float, c, 4);
            I(160) = I(284);
            V(288) = V(28) - F(float, c, 8);
            I(164) = I(288);
            V(16) = (float)Vec3_Length((const Vec3*)(fr + 156));
            if ((V(164) * V(56) + V(160) * V(52)) + V(48) * V(156) < g_sv64b678)
                V(16) = -V(16);
        }
        else
        {
            // $L_4f9028: no contact, the wheel hangs at its full travel
            double t = SF(8092 + 4 * i) * g_sv64c450;
            V(16) = (float)t;
            double a = t * g_sv702630, b = V(16) * g_sv702634, cz = V(16) * g_sv702638;
            V(76) = (float)(((cz * F(float, m, 24) + b * F(float, m, 12)) + a * F(float, m, 0)) + F(float, m, 36));
            V(80) = (float)(((cz * F(float, m, 28) + b * F(float, m, 16)) + a * F(float, m, 4)) + F(float, m, 40));
            double w = ((cz * F(float, m, 32) + b * F(float, m, 20)) + a * F(float, m, 8)) + F(float, m, 44);
            V(268) = V(20) - V(76);
            F(int, c, 0) = I(268);
            V(272) = V(24) - V(80);
            F(int, c, 4) = I(272);
            V(276) = (float)(V(28) - w);
            F(int, c, 8) = I(276);
            memcpy(fr + 156, &g_zeroVec, 12);
        }

        // $L_4f9119
        const BYTE* n = fr + 324 + e;
        V(232) = (float)((SF(8976) * F(float, n, 4) + SF(8988) * F(float, n, 8)) + SF(8964) * F(float, n, 0));
        I(800 + e) = I(232);
        V(236) = (float)((SF(8980) * F(float, n, 4) + SF(8968) * F(float, n, 0)) + SF(8992) * F(float, n, 8));
        I(804 + e) = I(236);
        V(240) = (float)((SF(8984) * F(float, n, 4) + SF(8972) * F(float, n, 0)) + SF(8996) * F(float, n, 8));
        I(808 + e) = I(240);
        V(152) = (float)((F(float, n, 8) * V(56) + F(float, n, 4) * V(52)) + F(float, n, 0) * V(48));
        I(564 + 4 * i) = I(152);
        double ac = x87_acos(V(152));
        V(692 + 4 * i) = (float)ac;
        V(660 + 4 * i) = (float)x87_sin(ac);

        float lim8092 = SF(8092 + 4 * i);
        bool hang;
        if (V(16) > lim8092)
            hang = true;
        else if (!F(WORD, r, 272) || !F(WORD, r, 4144))
            hang = true;
        else
        {
            hang = false;
            bool spring;
            if (V(16) < g_sv64c264 && F(int, s, 10092) == 0)
                spring = true;
            else if (F(int, s, 10092) == 0)
                spring = false;
            else
                spring = !(-lim8092 <= V(16));
            if (spring)
            {
                // $L_4f9295: the wheel pressed in, pushed back out by an impulse
                if (i == 0)
                    F(int, s, 15368) = (int)(V(16) * g_sv64b6a0 + F(int, s, 15368));
                else if (i == 1)
                    F(int, s, 15368) = (int)(F(int, s, 15368) - V(16) * g_sv64b6a0);
                double g = -V(16);
                F(int, s, 9244 + 4 * i) = 0;
                V(244) = (float)(g * F(float, n, 0));
                V(248) = (float)(g * F(float, n, 4));
                V(252) = (float)(g * F(float, n, 8));
                memcpy(fr + 156, fr + 244, 12);
                V(692 + 4 * i) = (float)g;
                V(628 + 4 * i) = (float)-(g / SF(9508));
                double x = SF(8060 + 4 * i);
                double k = (x87_fabs(x) / SF(8060 + 4 * i) + g_sv64b670) * SF(8124 + 4 * i) * x;
                V(420 + 4 * i) = (float)k;
                SF(8740 + 4 * i) = (float)k;
                Vec3 d;
                d.x = (float)(g * g_sv702630);
                d.y = (float)(g * g_sv702634);
                d.z = (float)(g * g_sv702638);
                Matrix* T = Matrix_Translation((Matrix*)(fr + 1664), d);
                memcpy(s + 8868, Matrix_Multiply((Matrix*)(fr + 1712), T, (const Matrix*)(s + 8868)), 48);
                memcpy(s + 8964, Matrix_Inverse((Matrix*)(fr + 1616), (const Matrix*)(s + 8868)), 48);
                F(int, s, 15372) = F(int, s, 9524);
                memcpy(fr + 196, s + 9012, 12);
                I(152) = (int)(fr + 324 + e);
                Body_MovingPointImpulse((const Vec3*)c, (const Vec3*)n, (const Matrix*)(s + 8868),
                                        (const Matrix*)(s + 8964), SF(9792), 0.5f, 0.0f, (const Vec3*)(s + 9012),
                                        (const Vec3*)(s + 9048), (const Vec3*)(s + 8008), (Vec3*)(s + 9012),
                                        (Vec3*)(s + 9048));
                // $L_4f9489
                memcpy(s + 9036, Vec3_MulComponents((Vec3*)(fr + 180), (const Vec3*)(s + 9048), (const Vec3*)(s + 8008)), 12);
                {
                    const BYTE* q = s + 9036;
                    V(208) = (float)((F(float, m, 12) * F(float, q, 4) + F(float, m, 24) * F(float, q, 8)) + F(float, m, 0) * F(float, q, 0));
                    V(212) = (float)((F(float, m, 16) * F(float, q, 4) + F(float, m, 4) * F(float, q, 0)) + F(float, m, 28) * F(float, q, 8));
                    V(216) = (float)((F(float, m, 20) * F(float, q, 4) + F(float, m, 8) * F(float, q, 0)) + F(float, m, 32) * F(float, q, 8));
                    memcpy(s + 9024, fr + 208, 12);
                }
                double da = SF(9012) - V(196), db = SF(9016) - V(200), dc = SF(9020) - V(204);
                double dd = (dc * F(float, n, 8) + db * F(float, n, 4)) + da * F(float, n, 0);
                F(int, s, 15336 + 4 * i) = (int)(dd * SF(9792) + F(int, s, 15336 + 4 * i));
                F(WORD, s, 6564 + 2 * i) = 0;
            }
            else
            {
                // $L_4f95bf: the spring and damper at this travel
                F(WORD, s, 6564 + 2 * i) = 0;
                SF(9244 + 4 * i) = V(16);
                double a = SF(8060 + 4 * i) - V(16);
                double b = (V(16) - lim8092) / (lim8092 * g_sv64b5b8);
                double h = g_sv64b670 - x87_exp(-(b * b));
                double q = (x87_fabs(a) / SF(8060 + 4 * i) + g_sv64b670) * SF(8124 + 4 * i);
                double v = h * (a * q);
                V(420 + 4 * i) = (float)v;
                SF(8740 + 4 * i) = (float)v;
                double dd = (double)SF(9244 + 4 * i) - SF(9276 + 4 * i);
                V(692 + 4 * i) = (float)dd;
                V(44) = (float)(dd / SF(9512));
                V(628 + 4 * i) = V(44);
                V(16) = (float)(V(44) / g_sv674454);
                if (!(dd <= g_sv64c264))
                {
                    double pp;
                    if (!(V(16) <= g_sv64b6e0))
                        pp = -(V(16) * SF(7484 + 4 * i) * V(16));
                    else
                        pp = SF(7484 + 4 * i) * V(16) * g_sv64b6b8;
                    V(292 + 4 * i) = (float)pp;
                    if (!(-v <= V(292 + 4 * i)))
                        V(292 + 4 * i) = (float)-v;
                }
                else
                {
                    // $L_4f96e8
                    double pp = -(V(44) / g_sv674450 * SF(7484 + 4 * i));
                    V(152) = (float)pp;
                    V(292 + 4 * i) = (float)pp;
                    double lim = V(420 + 4 * i) * g_sv64d950;
                    if (!(V(152) <= lim))
                        V(292 + 4 * i) = (float)lim;
                }
            }
        }
        if (hang)
        {
            // $L_4f973e: the wheel off the ground or beyond its travel
            F(WORD, s, 6564 + 2 * i) = 1;
            double two = lim8092 + lim8092;
            if ((F(int, s, 10092) != 0 || F(int, s, 16204) == 1) && V(16) < two
                && F(WORD, r, 272) && F(WORD, r, 4144))
            {
                SF(9244 + 4 * i) = V(16);
                double b = (V(16) - lim8092) / (lim8092 * g_sv64b5b8);
                double h = g_sv64b670 - x87_exp(-(b * b));
                double t;
                if (F(int, s, 10092) != 0)
                {
                    double w = -V(16);
                    t = x87_fabs(w) * (w + w) / lim8092 * SF(8124 + 4 * i);
                }
                else
                    t = ((lim8092 - V(16)) - lim8092 * g_sv64d0f0) * SF(8124 + 4 * i) * 2.0;
                V(420 + 4 * i) = (float)t;
                double v2 = h * V(420 + 4 * i);
                V(420 + 4 * i) = (float)v2;
                SF(8740 + 4 * i) = (float)v2;
                double u = (F(float, p, 4) * SF(9048) - SF(9052) * F(float, p, 0)) + SF(9020);
                V(176) = (float)u;
                V(628 + 4 * i) = (float)u;
                V(16) = (float)(u / g_sv674454);
                if (!(u <= g_sv64c264))
                {
                    double pp;
                    if (!(V(16) <= g_sv64b6e0))
                        pp = -(V(16) * SF(7484 + 4 * i) * V(16));
                    else
                        pp = SF(7484 + 4 * i) * V(16) * g_sv64b6b8;
                    V(292 + 4 * i) = (float)pp;
                    double nv = -x87_fabs(v2);
                    if (V(292 + 4 * i) < nv)
                        V(292 + 4 * i) = (float)nv;
                }
                else
                {
                    double pp = -(V(176) / g_sv674450 * SF(7484 + 4 * i));
                    V(192) = (float)pp;
                    V(292 + 4 * i) = (float)pp;
                    double lim = x87_fabs(V(420 + 4 * i)) * g_sv64d950;
                    if (!(V(192) <= lim))
                        V(292 + 4 * i) = (float)lim;
                }
                F(WORD, s, 6564 + 2 * i) = 0;
            }
            // $L_4f9955
            I(692 + 4 * i) = (int)0xC1A00000;
            F(int, s, 9244 + 4 * i) = F(int, s, 8092 + 4 * i);
        }

        // $L_4f9976: the forces along the normal and sideways
        double x = (double)SF(8740 + 4 * i) + V(292 + 4 * i);
        V(596 + 4 * i) = (float)x;
        if (F(int, s, 10092) == 0 && F(int, s, 16204) == 0 && x < g_sv64c264)
            I(596 + 4 * i) = 0;
        double g = (double)SF(8740 + 4 * i) * V(564 + 4 * i);
        I(516 + 4 * i) = 0;
        V(452 + 4 * i) = (float)g;
        double h = (double)V(564 + 4 * i) * V(292 + 4 * i);
        V(484 + 4 * i) = (float)h;
        if (!(x87_fabs(V(660 + 4 * i)) <= g_sv64b6d8))
        {
            if ((h + g) * SF(9840 + 4 * i) < (double)V(660 + 4 * i) * V(596 + 4 * i))
                F(BYTE, s, 6596 + i) = 1;
        }

        // $L_4f9a2c
        double A = V(56) * F(float, n, 4) - V(52) * F(float, n, 8);
        double B = V(48) * F(float, n, 8) - V(56) * F(float, n, 0);
        double C = V(52) * F(float, n, 0) - V(48) * F(float, n, 4);
        BYTE* tt = fr + 992 + e;
        V(112) = (float)(B * F(float, n, 8) - C * F(float, n, 4));
        F(int, tt, 0) = I(112);
        V(116) = (float)(C * F(float, n, 0) - A * F(float, n, 8));
        F(int, tt, 4) = I(116);
        V(120) = (float)(A * F(float, n, 4) - B * F(float, n, 0));
        F(int, tt, 8) = I(120);
        V(32) = (float)((SF(8976) * F(float, tt, 4) + SF(8988) * F(float, tt, 8)) + F(float, tt, 0) * SF(8964));
        V(36) = (float)((SF(8968) * F(float, tt, 0) + SF(8980) * F(float, tt, 4)) + SF(8992) * F(float, tt, 8));
        V(40) = (float)((SF(8972) * F(float, tt, 0) + SF(8984) * F(float, tt, 4)) + SF(8996) * F(float, tt, 8));
        memcpy(fr + 1232 + e, fr + 32, 12);
    }

    // $L_4f9b67: the wheels' forces summed
    memcpy(s + 9096, &g_zeroVec, 12);
    memcpy(s + 9120, &g_zeroVec, 12);
    memcpy(s + 9132, &g_zeroVec, 12);
    F(BYTE, s, 6586) = 0;
    F(BYTE, s, 6580) = 1;
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const int e = 12 * i;
        WORD up = F(WORD, s, 6564 + 2 * i);
        F(BYTE, s, 6586) = (F(BYTE, s, 6586) || up) ? 1 : 0;
        F(BYTE, s, 6580) = (F(BYTE, s, 6580) && up) ? 1 : 0;
        double g = (double)V(484 + 4 * i) + V(452 + 4 * i);
        V(76) = (float)(g * V(324 + e));
        V(80) = (float)(g * V(328 + e));
        V(84) = (float)(g * V(332 + e));
        Vec3_AddAssign((Vec3*)(s + 9120), 0, (Vec3*)(fr + 180), (const Vec3*)(fr + 76));
        double k = V(516 + 4 * i);
        V(100) = (float)(k * V(992 + e));
        V(104) = (float)(k * V(996 + e));
        V(108) = (float)(k * V(1000 + e));
        Vec3_AddAssign((Vec3*)(s + 9132), 0, (Vec3*)(fr + 168), (const Vec3*)(fr + 100));
    }

    // $L_4f9d1f
    {
        const BYTE* v = s + 9096;
        V(32) = (float)((SF(8880) * F(float, v, 4) + SF(8892) * F(float, v, 8)) + SF(8868) * F(float, v, 0));
        V(36) = (float)((SF(8872) * F(float, v, 0) + SF(8884) * F(float, v, 4)) + SF(8896) * F(float, v, 8));
        V(40) = (float)((SF(8876) * F(float, v, 0) + SF(8888) * F(float, v, 4)) + SF(8900) * F(float, v, 8));
        memcpy(s + 9072, fr + 32, 12);
        BYTE* u = s + 9108;
        double x = (SF(9724) * F(float, u, 8) + SF(9712) * F(float, u, 4)) + SF(9700) * F(float, u, 0);
        double y = (SF(9728) * F(float, u, 8) + SF(9704) * F(float, u, 0)) + SF(9716) * F(float, u, 4);
        double z = (SF(9732) * F(float, u, 8) + SF(9708) * F(float, u, 0)) + SF(9720) * F(float, u, 4);
        V(32) = (float)x;
        V(36) = (float)y;
        V(40) = (float)z;
        memcpy(u, fr + 32, 12);
        Vec3_SetThis((Vec3*)(fr + 76), 0, (float)((SF(9132) + SF(9120)) + SF(9684)),
                     (float)((SF(9124) + SF(9136)) + SF(9688)), (float)((SF(9128) + SF(9140)) + SF(9692)));
        Vec3_SetThis((Vec3*)(fr + 100), 0, V(76) + F(float, u, 0), V(80) + F(float, u, 4), V(84) + F(float, u, 8));
        Vec3_SetThis((Vec3*)(fr + 32), 0, V(100) + SF(9748), V(104) + SF(9752), V(108) + SF(9756));
        memcpy(s + 9072, fr + 32, 12);
    }
    I(72) = 0;
    memcpy(fr + 140, &g_zeroVec, 12);
    memcpy(fr + 220, &g_zeroVec, 12);
    memcpy(fr + 88, &g_zeroVec, 12);
    memcpy(fr + 128, &g_zeroVec, 12);
    for (int j = 0; j < F(int, s, 10176); j++)
    {
        // the body's own contacts: forces at points q (+10184), w their moments
        const BYTE* q = s + 10184 + 12 * j;
        const BYTE* d = s + 10244 + 12 * j;
        const BYTE* w = s + 10304 + 12 * j;
        Vec3_AddAssign((Vec3*)(fr + 140), 0, (Vec3*)(fr + 180), (const Vec3*)d);
        Vec3_AddAssign((Vec3*)(fr + 220), 0, (Vec3*)(fr + 168), (const Vec3*)w);
        Vec3_SetThis((Vec3*)(fr + 20), 0,
                     (float)((SF(8988) * F(float, d, 8) + SF(8976) * F(float, d, 4)) + SF(8964) * F(float, d, 0)),
                     (float)((SF(8992) * F(float, d, 8) + SF(8980) * F(float, d, 4)) + SF(8968) * F(float, d, 0)),
                     (float)((SF(8996) * F(float, d, 8) + SF(8984) * F(float, d, 4)) + SF(8972) * F(float, d, 0)));
        Vec3_SetThis((Vec3*)(fr + 76), 0, (float)(V(28) * F(float, q, 4) - V(24) * F(float, q, 8)),
                     (float)(V(20) * F(float, q, 8) - V(28) * F(float, q, 0)),
                     (float)(V(24) * F(float, q, 0) - V(20) * F(float, q, 4)));
        memcpy(fr + 100, fr + 76, 12);
        Vec3_AddAssign((Vec3*)(fr + 88), 0, (Vec3*)(fr + 196), (const Vec3*)(fr + 100));
        Vec3_SetThis((Vec3*)(fr + 60), 0,
                     (float)((F(float, w, 8) * SF(8996) + SF(8972) * F(float, w, 0)) + F(float, w, 4) * SF(8984)),
                     (float)((SF(8968) * F(float, w, 0) + F(float, w, 8) * SF(8992)) + F(float, w, 4) * SF(8980)),
                     (float)((F(float, w, 0) * SF(8964) + F(float, w, 8) * SF(8988)) + F(float, w, 4) * SF(8976)));
        Vec3_SetThis((Vec3*)(fr + 32), 0, (float)(V(68) * F(float, q, 4) - V(64) * F(float, q, 8)),
                     (float)(V(60) * F(float, q, 8) - V(68) * F(float, q, 0)),
                     (float)(V(64) * F(float, q, 0) - V(60) * F(float, q, 4)));
        memcpy(fr + 112, fr + 32, 12);
        Vec3_AddAssign((Vec3*)(fr + 128), 0, (Vec3*)(fr + 208), (const Vec3*)(fr + 112));
        I(72) = j + 1;
    }
    for (int j = 0; j < F(int, s, 10180); j++)
    {
        Vec3_AddAssign((Vec3*)(fr + 88), 0, (Vec3*)(fr + 180), (const Vec3*)(s + 10364 + 12 * j));
        Vec3_AddAssign((Vec3*)(fr + 128), 0, (Vec3*)(fr + 168), (const Vec3*)(s + 10424 + 12 * j));
    }
    if (F(int, s, 10176) || F(int, s, 10180))
    {
        // $L_4fa1c9: the contacts' push kept from pulling the body back in
        memcpy(fr + 20, fr + 128, 12);
        Vec3_Normalized((Vec3*)(fr + 60), (const Vec3*)(fr + 20));
        double d = (SF(9044) * V(68) + SF(9040) * V(64)) + V(60) * SF(9036);
        Vec3_SetThis((Vec3*)(fr + 48), 0, (float)(V(60) * d), (float)(V(64) * d), (float)(V(68) * d));
        const float* res;
        if (!((V(60) * V(48) + V(64) * V(52)) + V(68) * V(56) <= g_sv64b678))
            res = (const float*)&g_zeroVec;
        else
        {
            double dt = SF(9508);
            Vec3_SetThis((Vec3*)(fr + 76), 0, (float)(V(20) * dt), (float)(dt * V(24)), (float)(dt * V(28)));
            Vec3_SetThis((Vec3*)(fr + 100), 0, V(76) + V(48), V(80) + V(52), V(84) + V(56));
            if ((V(108) * V(56) + V(100) * V(48)) + V(104) * V(52) < g_sv64b678)
            {
                double k = g_sv64c938 / SF(9508);
                Vec3_SetThis((Vec3*)(fr + 76), 0, (float)(k * V(48)), (float)(k * V(52)), (float)(k * V(56)));
                res = &V(76);
            }
            else
                res = &V(20);
        }
        float r0 = res[0], r1 = res[1], r2 = res[2];
        V(128) = r0;
        V(132) = r1;
        V(136) = r2;
    }

    // $L_4fa36a
    Vec3_SetThis((Vec3*)(fr + 76), 0, V(220) + V(140), V(224) + V(144), V(228) + V(148));
    memcpy(fr + 100, fr + 76, 12);
    Vec3_AddAssign((Vec3*)(s + 9072), 0, (Vec3*)(fr + 180), (const Vec3*)(fr + 100));
    memcpy(s + 9084, s + 9072, 12);
    memcpy(s + 9180, &g_zeroVec, 12);
    memcpy(s + 9192, &g_zeroVec, 12);
    memcpy(s + 9204, &g_zeroVec, 12);
    memcpy(s + 9216, &g_zeroVec, 12);
    memcpy(s + 9228, &g_zeroVec, 12);
    Vec3_SetThis((Vec3*)(fr + 60), 0, (float)(g_sv702638 * SF(212) - g_sv702634 * SF(216)),
                 (float)(g_sv702630 * SF(216) - g_sv702638 * SF(208)),
                 (float)(g_sv702634 * SF(208) - g_sv702630 * SF(212)));
    Vec3_SetThis((Vec3*)(fr + 20), 0, (float)(V(64) * SF(216) - V(68) * SF(212)),
                 (float)(V(68) * SF(208) - V(60) * SF(216)), (float)(V(60) * SF(212) - V(64) * SF(208)));
    Vec3_SetThis((Vec3*)(fr + 788), 0, (float)((V(24) * SF(8976) + V(28) * SF(8988)) + V(20) * SF(8964)),
                 (float)((V(20) * SF(8968) + V(24) * SF(8980)) + V(28) * SF(8992)),
                 (float)((V(20) * SF(8972) + V(24) * SF(8984)) + V(28) * SF(8996)));
    for (int k = 0; k < 8; k++)
    {
        Vec3_ZeroThis((Vec3*)(fr + 1088 + 12 * k), 0);
        Vec3_ZeroThis((Vec3*)(fr + 1328 + 12 * k), 0);
        Vec3_ZeroThis((Vec3*)(fr + 1424 + 12 * k), 0);
        Vec3_ZeroThis((Vec3*)(fr + 896 + 12 * k), 0);
    }
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        // $L_4fa64e: each wheel's push and torque about the centre
        const int e = 12 * i;
        const BYTE* c = s + 9528 + e;
        Vec3_SetThis((Vec3*)(fr + 76), 0,
                     (float)(((F(float, c, 0) * SF(8964) + F(float, c, 8) * SF(8988)) + F(float, c, 4) * SF(8976)) + SF(9000)),
                     (float)(((F(float, c, 4) * SF(8980) + F(float, c, 0) * SF(8968)) + F(float, c, 8) * SF(8992)) + SF(9004)),
                     (float)(((F(float, c, 8) * SF(8996) + F(float, c, 4) * SF(8984)) + F(float, c, 0) * SF(8972)) + SF(9008)));
        memcpy(fr + 1088 + e, fr + 76, 12);
        double g = V(452 + 4 * i);
        Vec3_SetThis((Vec3*)(fr + 100), 0, (float)(g * V(800 + e)), (float)(g * V(804 + e)), (float)(g * V(808 + e)));
        memcpy(fr + 32, fr + 100, 12);
        memcpy(fr + 1328 + e, Vec3_Cross((Vec3*)(fr + 180), (const Vec3*)(fr + 1088 + e), (const Vec3*)(fr + 32)), 12);
        Vec3* sc = Vec3_Scaled((Vec3*)(fr + 168), (const Vec3*)&g_sv702630, V(484 + 4 * i));
        memcpy(fr + 1424 + e, Vec3_Cross((Vec3*)(fr + 112), (const Vec3*)(s + 7356 + e), sc), 12);
        sc = Vec3_Scaled((Vec3*)(fr + 196), (const Vec3*)(fr + 1232 + e), V(516 + 4 * i));
        memcpy(fr + 896 + e, Vec3_Cross((Vec3*)(fr + 208), (const Vec3*)(fr + 1088 + e), sc), 12);
        Vec3_AddAssign((Vec3*)(s + 9204), 0, (Vec3*)(fr + 256), (const Vec3*)(fr + 1328 + e));
        Vec3_AddAssign((Vec3*)(s + 9216), 0, (Vec3*)(fr + 244), (const Vec3*)(fr + 1424 + e));
        Vec3_AddAssign((Vec3*)(s + 9228), 0, (Vec3*)(fr + 232), (const Vec3*)(fr + 896 + e));
    }

    // $L_4fa86c: the turning, kept from pulling back against the contacts
    Vec3* a = Vec3_Add((Vec3*)(fr + 180), (const Vec3*)(s + 9760), (const Vec3*)(s + 9216));
    a = Vec3_Add((Vec3*)(fr + 168), a, (const Vec3*)(fr + 128));
    memcpy(s + 9156, a, 12);
    memcpy(fr + 48, a, 12);
    Vec3_Normalized((Vec3*)(fr + 76), (const Vec3*)(fr + 48));
    a = Vec3_Add((Vec3*)(fr + 180), (const Vec3*)(s + 9204), (const Vec3*)(s + 9228));
    a = Vec3_Add((Vec3*)(fr + 168), a, (const Vec3*)(fr + 88));
    a = Vec3_Scaled((Vec3*)(fr + 100), a, SF(9508));
    a = Vec3_Add((Vec3*)(fr + 112), (const Vec3*)(s + 9036), a);
    float dw = (float)Vec3_Dot(a, (const Vec3*)(fr + 76));
    Vec3_Scaled((Vec3*)(fr + 32), (const Vec3*)(fr + 76), dw);
    int r0, r1, r2;
    if (!(Vec3_Dot((const Vec3*)(fr + 48), (const Vec3*)(fr + 32)) <= g_sv64b678))
    {
        r0 = F(int, &g_zeroVec, 0);
        r1 = F(int, &g_zeroVec, 4);
        r2 = F(int, &g_zeroVec, 8);
    }
    else
    {
        Vec3* t = Vec3_Scaled((Vec3*)(fr + 180), (const Vec3*)(fr + 48), SF(9508));
        Vec3_Add((Vec3*)(fr + 168), (const Vec3*)(fr + 32), t);
        if (Vec3_Dot((const Vec3*)(fr + 168), (const Vec3*)(fr + 32)) < g_sv64b678)
        {
            t = Vec3_Scaled((Vec3*)(fr + 180), (const Vec3*)(fr + 32), (float)(g_sv64c938 / SF(9508)));
            I(48) = F(int, t, 0);
            I(52) = F(int, t, 4);
            r0 = I(48);
            r1 = I(52);
            r2 = F(int, t, 8);
        }
        else
        {
            r0 = I(48);
            r1 = I(52);
            r2 = I(56);
        }
    }
    F(int, s, 9156) = r0;
    F(int, s, 9160) = r1;
    F(int, s, 9164) = r2;
    Vec3_AddAssign((Vec3*)(s + 9156), 0, (Vec3*)(fr + 180), (const Vec3*)(s + 9168));
    a = Vec3_Add((Vec3*)(fr + 180), (const Vec3*)(s + 9204), (const Vec3*)(s + 9228));
    a = Vec3_Add((Vec3*)(fr + 168), a, (const Vec3*)(fr + 88));
    memcpy(s + 9144, Vec3_Add((Vec3*)(fr + 76), a, (const Vec3*)(s + 9156)), 12);
    if (!F(int, s, 10104))
    {
        Ct4_DivScalar((Vec3*)(fr + 76), (const Vec3*)(s + 9072), SF(9792));
        Vec3_DivComponents((Vec3*)(fr + 168), (const Vec3*)(s + 9144), (const Vec3*)(s + 8008));
        Body_Integrate(s, 0, (const float*)(fr + 76), (const float*)(fr + 168));
    }
    F(int, s, 9524)++;
    return F(int, s, 9524) <= F(int, s, 9520);
}

} // extern "C"

#undef V
#undef I
#undef SF

// === the car's pedals eased by the keys (FUN_500850 with its tail $L_500939)

extern "C" void __fastcall Car_DriveStep(BYTE* s);
KING_VAR(float, g_pd67484c, 67484c)
KING_VAR(float, g_pd64c250, 64c250)
KING_VAR(float, g_pd64c938, 64c938)
KING_VAR(float, g_pd64c264, 64c264)
KING_VAR(float, g_pd64da50, 64da50)
KING_VAR(double, g_pd64d338, 64d338)
KING_VAR(double, g_pd64d950, 64d950)
KING_VAR(double, g_pd64da58, 64da58)
KING_VAR(double, g_pd64b5a0, 64b5a0)
KING_VAR(double, g_pd64b780, 64b780)
KING_VAR(double, g_pd64b840, 64b840)
KING_VAR(double, g_pd64b6e0, 64b6e0)
KING_VAR(double, g_pd64b6f0, 64b6f0)
KING_VAR(double, g_pd64b5b8, 64b5b8)
KING_VAR(double, g_pd64b668, 64b668)
KING_VAR(double, g_pd64b670, 64b670)
KING_VAR(double, g_pd64b738, 64b738)

#define PF(n) F(float, s, n)

// the push of a held pedal key since time t0 (s+10032 / +10040), less at speed
static double Pd_Push(BYTE* s, double t0, float dt)
{
    double t = Game_Time() - t0 + g_pd64b840;
    double q = g_pd64b670 / (x87_fabs(PF(3528)) * g_pd64b738 + g_pd64b670);
    return t * q * PF(10020) * dt * g_pd64da50 * g_pd67484c;
}

extern "C" {

// [FUN_500850..$L_500939] car s's pedal (+10720, -1 brake .. 1 throttle) from the keys (+9986 up, +9987 down,
// +9985 the handbrake-like hold, +9990 the cruise flag with the target +10012): eased back by k (from the grip
// +10024) and pushed by Pd_Push up to the limit c (lower at speed, or by the slide angle with +10048); the key
// times (+10032, +10040), the reverse word (+10008, +10006, +15440 by +10010), then Car_DriveStep
void __fastcall Car_PedalStep(BYTE* s)
{
    float dt = (float)Game_FrameTime();
    BYTE up = F(BYTE, s, 9986), down = F(BYTE, s, 9987);
    bool cruise = !up && !down && F(WORD, s, 9990);
    int dir = (int)up - (int)down;
    if ((int)F(BYTE, s, 9985) != F(int, s, 6944) && (F(WORD, s, 10048) || F(int, s, 10060)))
    {
        F(double, s, 10040) = Game_Time() - dt;
        F(double, s, 10032) = Game_Time() - dt;
    }
    float k;
    if (PF(10024) < g_pd64d338)
        k = 1.0f;
    else if (!(PF(10024) <= g_pd64d950))
        k = 0.1f;
    else
        k = (float)x87_pow((g_pd64c250 - PF(10024)) * g_pd64da58 + g_pd64b5a0, dt * g_pd64b780);

    // $L_50093d
    if (cruise)
    {
        PF(10012) = (float)((PF(10020) + PF(10020) + g_pd64b840) * PF(10012));
        for (int r = 0; r < 2; r++)
        {
            if (!(PF(10012) <= g_pd64c250))
                PF(10012) = 1.0f;
            if (PF(10012) < g_pd64c938)
                PF(10012) = -1.0f;
        }
        if (F(int, s, 10728))
        {
            PF(10720) = (float)((PF(10012) - PF(10720)) * (g_pd64c250 - k) + PF(10720));
            if (!(dt * g_pd64b6e0 <= x87_fabs(PF(10720) - PF(10012))))
                F(int, s, 10728) = 0;
        }
        else
            F(int, s, 10720) = F(int, s, 10012);
    }
    else
    {
        // $L_500a2e: the limit c
        double a = x87_fabs(PF(3528));
        float c = (float)(g_pd64b6f0 / (a + g_pd64b6f0));
        F(int, s, 10728) = 1;
        if (F(WORD, s, 10048) && !F(BYTE, s, 9985))
        {
            double v;
            if (a < g_pd64b5b8)
                v = g_pd64c250;
            else
            {
                double x = PF(3520);
                double y = PF(9904) * PF(9788) * PF(8032);
                v = x87_atan2(y / (x * x), 1.0) * g_pd64b668;
            }
            c = (float)v;
            if (!(v <= g_pd64b670))
                c = 1.0f;
        }
        if (F(BYTE, s, 9985))
        {
            c = 1.0f;
            g_pd67484c = 2.0f;
        }
        else
            *(DWORD*)&g_pd67484c = 1059648963;

        // $L_500ae5
        if (!(up == 1 && down == 1))
        {
            if (!up && !down)
                PF(10720) = k * PF(10720);
            else if (up)
            {
                // $L_500b24: throttle
                double r = Pd_Push(s, F(double, s, 10032), dt);
                if (PF(10720) < g_pd64c264)
                {
                    double w = k * PF(10720);
                    PF(10720) = (float)(x87_fabs(w) * dt + w);
                    if (!(PF(10720) <= g_pd64c264))
                        F(int, s, 10720) = 0;
                    PF(10720) = (float)(r + PF(10720));
                    if (!(PF(10720) <= c))
                        PF(10720) = c;
                }
                else if (!F(BYTE, s, 9985) && dt + c < PF(10720))
                {
                    if (!(PF(10720) <= g_pd64c250))
                        PF(10720) = 1.0f;
                    PF(10720) = PF(10720) - dt;
                    float v = PF(10720);
                    if (!(v <= c - dt) && v < c)
                        PF(10720) = c;
                }
                else
                {
                    PF(10720) = (float)(r + PF(10720));
                    if (!(PF(10720) <= c))
                        PF(10720) = c;
                }
            }
            else
            {
                // $L_500c80: brake
                double r = Pd_Push(s, F(double, s, 10040), dt);
                float nc = -c;
                if (!(PF(10720) <= g_pd64c264))
                {
                    double w = k * PF(10720);
                    PF(10720) = (float)(w - x87_fabs(w) * dt);
                    if (PF(10720) < g_pd64c264)
                        F(int, s, 10720) = 0;
                    PF(10720) = (float)(PF(10720) - r);
                    if (PF(10720) < nc)
                        PF(10720) = nc;
                }
                else if (!F(BYTE, s, 9985) && !(nc - dt <= PF(10720)))
                {
                    if (PF(10720) < g_pd64c938)
                        PF(10720) = -1.0f;
                    PF(10720) = dt + PF(10720);
                    float v = PF(10720);
                    if (v < dt - c && !(v <= nc))
                        PF(10720) = nc;
                }
                else
                {
                    PF(10720) = (float)(PF(10720) - r);
                    if (PF(10720) < nc)
                        PF(10720) = nc;
                }
            }
        }
    }

    // $L_500df0: the key times and the pedal kept per direction
    if (dir <= 0)
    {
        F(double, s, 10032) = Game_Time();
        F(int, s, 6936) = F(int, s, 10720);
    }
    if (dir >= 0)
    {
        F(double, s, 10040) = Game_Time();
        F(int, s, 6940) = F(int, s, 10720);
    }
    F(int, s, 6944) = F(BYTE, s, 9985);
    WORD g = F(WORD, s, 10008);
    if (F(WORD, s, 10010))
    {
        if (g == 0)
            F(WORD, s, 10008) = 3;
        else if (g == 2)
            F(WORD, s, 10008) = 1;
    }
    else
    {
        if (g == 1)
            F(WORD, s, 10008) = 0;
        else if (g == 3)
            F(WORD, s, 10008) = 2;
    }
    if (F(short, s, 10008) >= 2)
    {
        F(WORD, s, 10006) = 1;
        F(int, s, 15440) = 1;
    }
    else
        F(int, s, 15440) = 0;
    Car_DriveStep(s);
    F(WORD, s, 10054) = 0;
    F(int, s, 15448) = 0;
    memcpy(s + 15452, &g_zeroVec, 12);
    F(int, s, 15492) = 0;
}

} // extern "C"

#undef PF

// === the wheels' spin before the sub-steps (FUN_4f35f0 with its tail $L_4f3620)

extern "C" void Wv_Check(); KING_ALIAS("_Wv_Check", "_Angle_F2Pi")    // a value through the debug hook (value, name)
KING_VAR(char, g_wvRun1, 67478c)
KING_VAR(char, g_wvRun2, 67477c)
KING_VAR(char, g_wvRun3, 674734)
KING_VAR(char, g_wvRun4, 674724)
KING_VAR(char, g_wvRun5, 674714)
KING_VAR(char, g_wvRun6, 674704)
KING_VAR(char, g_wvRun7, 6746f4)
KING_VAR(double, g_wv64c958, 64c958)
KING_VAR(double, g_wv64b780, 64b780)
KING_VAR(double, g_wv64d9c8, 64d9c8)
KING_VAR(double, g_wv64b6d0, 64b6d0)
KING_VAR(float, g_wv64c2fc, 64c2fc)
KING_VAR(float, g_wv64c450, 64c450)
KING_VAR(float, g_wv64c264, 64c264)
KING_VAR(float, g_wv64d9c0, 64d9c0)

#define WF(n) F(float, s, n)

static float Wv_Hook(float v, const char* name)
{
    return (float)CallC<double>(Wv_Check, v, name);
}

extern "C" {

// [FUN_4f35f0..$L_4f3620] car s's wheel spins (+3772) for this frame: a free wheel's spin from its speed along the
// ground (+2724 · +6308 over the radius +7028); a driven one eased by e (or set from the engine +7300 through the
// gear ratio +7236 when the clutch +7092 holds); the drive forces, the spin of locked wheels (+6612), the wheel
// angles (+3740); the engine's ramps (+16216, +16268) eased by the load or let go
void __fastcall Wheels_FrameSpin(BYTE* s)
{
    float dt = (float)Game_FrameTime();
    float e = (float)x87_pow(g_wv64c958, dt * g_wv64b780);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        const int q = 12 * i;
        float* w = (float*)(s + 3772 + 4 * i);
        double v = -((((double)WF(2728 + q) * WF(6312 + q) + (double)WF(6316 + q) * WF(2732 + q))
                      + (double)WF(6308 + q) * WF(2724 + q)) / WF(7028 + 4 * i) * dt);
        if (F(WORD, s, 6564 + 2 * i))
        {
            if (F(int, s, 7092 + 4 * i) && F(int, s, 7196) != 1)
            {
                *w = (float)((double)dt * WF(7300) / WF(7236 + 4 * F(int, s, 7196)));
                *w = Wv_Hook(*w, &g_wvRun1);
            }
            else
            {
                *w = e * *w;
                *w = Wv_Hook(*w, &g_wvRun2);
            }
        }
        else
        {
            *w = (float)v;
            char buf[256];
            double lb = Vec3_Length((const Vec3*)(s + 6308 + q));
            double la = Vec3_Length((const Vec3*)(s + 2724 + q));
            sprintf(buf, &g_wvRun3, F(int, s, 2252), i, (double)WF(3528), lb, la, (double)WF(7028 + 4 * i));
            if (!(x87_fabs(*w) <= g_wv64d9c8))
                *(DWORD*)w = 1103695835;
            else
                *w = Wv_Hook(*w, buf);
        }
    }
    float drive = (float)Wheels_DriveForces(s);
    for (int i = 0; i < F(int, s, 3540); i++)
    {
        float* w = (float*)(s + 3772 + 4 * i);
        if (F(int, s, 6612 + 4 * i))
        {
            int sign = F(int, s, 7196) ? 1 : -1;
            double x = (double)WF(7300) * g_wv64c2fc / WF(7304) / WF(7028 + 4 * i) * dt;
            *w = (float)((-x87_fabs(*w) - x) * sign);
            *w = Wv_Hook(*w, &g_wvRun4);
        }
        *w = Wv_Hook(*w, &g_wvRun5);
        float* a = (float*)(s + 3740 + 4 * i);
        double r = CallC<double>(Wv_Check, *a, &g_wvRun6);
        *a = (float)r;
        *a = Wv_Hook((float)(r + *w), &g_wvRun7);
    }
    if (F(int, s, 6844))
    {
        double a = x87_fabs(WF(3528));
        double v = drive / (WF(9792) * g_wv64c450) * a / (a + g_wv64b6d0) * dt;
        float k = (float)v;
        Ramp_Down(s + 16216, 0, k);
        Ramp_Down(s + 16268, 0, k);
        if (!(Ramp_Value(s + 16268, 0) > g_wv64c264))
            Ramp_SetPosition(s + 16216, 0, 0.0f);
    }
    else
        Ramp_Down(s + 16268, 0, (float)(dt * g_wv64d9c0));
}

} // extern "C"

#undef WF
