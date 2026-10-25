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

extern "C" void FUN_4f0fc0(); KING_ALIAS("_FUN_4f0fc0", "@Body_PlaceAt@16")
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
extern "C" void FUN_4ebbe0(); KING_ALIAS("_FUN_4ebbe0", "@Car_RandomStep@12")
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
extern "C" void FUN_4f0530(); KING_ALIAS("_FUN_4f0530", "@Body_SpinAxis@4")
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

ASM_PROC(FUN_4f1240)
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
ASM_VAR(AsmLabel, g_q98_702650, $L_702650)
ASM_VAR(AsmLabel, g_q98_702630, $L_702630)
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

ASM_VAR(AsmLabel, g_ps7025f0, $L_7025f0)        // a 4x3 identity matrix
ASM_VAR(AsmLabel, g_ps702650, $L_702650)        // a vector
ASM_VAR(AsmLabel, g_ps702630, $L_702630)        // a vector

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

ASM_VAR(WORD, g_ec696bda, $L_696bda)
ASM_VAR(float, g_ec64d970, $L_64d970)
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
ASM_VAR(double, g_bi64b6d0, $L_64b6d0)

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

ASM_VAR(AsmLabel, g_bs697f9c, $L_697f9c)       // 2416 bytes per car model

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
ASM_VAR(double, g_bt64b658, $L_64b658)
ASM_VAR(double, g_bt64b730, $L_64b730)
ASM_VAR(float, g_bt64c250, $L_64c250)

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
ASM_VAR(double, g_sa64b6d0, $L_64b6d0)

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
ASM_VAR(float, g_bf702620, $L_702620)
ASM_VAR(float, g_bf702624, $L_702624)
ASM_VAR(float, g_bf702628, $L_702628)

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

ASM_PROC(FUN_4ef480)
ASM_VAR(float, g_bp64c450, $L_64c450)

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
ASM_VAR(AsmLabel, g_ba7025f0, $L_7025f0)
ASM_VAR(double, g_ba64b5a0, $L_64b5a0)

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
ASM_VAR(double, g_wf64b5b8, $L_64b5b8)
ASM_VAR(double, g_wf64b678, $L_64b678)
ASM_VAR(double, g_wf64b670, $L_64b670)
ASM_VAR(float, g_wf64c264, $L_64c264)
ASM_VAR(float, g_wf64c250, $L_64c250)

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

ASM_VAR(double, g_tg64b6d0, $L_64b6d0)
ASM_VAR(double, g_tg64b840, $L_64b840)
ASM_VAR(float, g_tg64d350, $L_64d350)
ASM_VAR(float, g_tg64c2fc, $L_64c2fc)
ASM_VAR(double, g_tg64b5b8, $L_64b5b8)
ASM_VAR(double, g_tg64d9e8, $L_64d9e8)
ASM_VAR(double, g_tg64d9e0, $L_64d9e0)
ASM_VAR(float, g_tg64c40c, $L_64c40c)
ASM_VAR(float, g_tg64d2f8, $L_64d2f8)
ASM_VAR(float, g_tg64c250, $L_64c250)
ASM_VAR(float, g_tg64c264, $L_64c264)
ASM_VAR(float, g_tg67442c, $L_67442c)
ASM_VAR(float, g_tg674430, $L_674430)
ASM_VAR(float, g_tg674434, $L_674434)
ASM_VAR(float, g_tg674438, $L_674438)
ASM_VAR(float, g_tg67443c, $L_67443c)
ASM_VAR(float, g_tg674440, $L_674440)

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
ASM_VAR(float, g_tf674404, $L_674404)
ASM_VAR(float, g_tf64c250, $L_64c250)
ASM_VAR(float, g_tf64c258, $L_64c258)
ASM_VAR(float, g_tf64c264, $L_64c264)
ASM_VAR(float, g_tf64d7b8, $L_64d7b8)
ASM_VAR(double, g_tf64d9f0, $L_64d9f0)
ASM_VAR(double, g_tf64b6e0, $L_64b6e0)
ASM_VAR(double, g_tf64b5a0, $L_64b5a0)
ASM_VAR(double, g_tf64b678, $L_64b678)
ASM_VAR(AsmLabel, g_tf67479c, $L_67479c)

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
ASM_VAR(float, g_wc702630, $L_702630)
ASM_VAR(float, g_wc702634, $L_702634)
ASM_VAR(float, g_wc702638, $L_702638)
ASM_VAR(float, g_wc64d7b8, $L_64d7b8)
ASM_VAR(float, g_wc64c258, $L_64c258)
ASM_VAR(double, g_wc64b658, $L_64b658)
ASM_VAR(double, g_wc64b678, $L_64b678)

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
