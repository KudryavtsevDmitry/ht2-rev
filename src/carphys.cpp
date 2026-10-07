// carphys.cpp - small functions of the unit at 0x4eb0c7 of king.masm (the
// vehicle's controls and physics): ramps of a control between limits, the
// wheel and contact points of the body, the engine's state, steering
// geometry, and copies of the body's vectors.
//
// Building with  nmake ASM_CARPHYS=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

ASM_PROC(FUN_4eb350)
ASM_PROC(FUN_4eeaf0)
ASM_PROC(FUN_4ee980)
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
    CallThis(FUN_4eeaf0, self, a);
}

extern "C" void __fastcall Engine_4eec20(BYTE* self, void*)
{
    int mode = F(int, self, 10104);
    F(int, self, 10120) = 0;
    if (mode == 1)
        CallThis(FUN_4eeaf0, self, 0);
    else if (mode == 2 || mode == 4) {
        if (mode == 2)
            CallThis(FUN_4ee980, self, 1);
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
