// x87.h - the floating point math the game's compiler (VC6) generated inline
// or through its x87 intrinsics, so the C++ code computes what the assembly
// computed. The C++ code is compiled for the x87 FPU too (/arch:IA32), so
// it runs under the same precision control as the assembly around it.
//
// Rules for rewriting x87 code: a value the assembly keeps on the FPU stack
// is a double in C++ (the FPU computes to 53 bits), a value it stores with
// fstp DWORD is a float. A function that returns an unrounded result in
// ST(0) returns double; one that loads a float before returning returns
// float. A comparison that jumps on C0 or C3 is also true when the operands
// are unordered (NaN): write it as !(a >= b) or !(a > b).
#pragma once

#pragma warning(push)
#pragma warning(disable: 4035)  // no return value: the result is in ST(0)

inline double x87_sqrt(double x) { __asm { fld x } __asm { fsqrt } }
inline double x87_sin(double x) { __asm { fld x } __asm { fsin } }
inline double x87_cos(double x) { __asm { fld x } __asm { fcos } }
inline double x87_atan2(double y, double x) { __asm { fld y } __asm { fld x } __asm { fpatan } }

// e^x as VC6 inlines exp()
inline double x87_exp(double x)
{
    __asm {
        fld x
        fldl2e
        fmulp st(1), st
        fld st(0)
        frndint
        fxch st(1)
        fsub st, st(1)
        f2xm1
        fld1
        faddp st(1), st
        fscale
        fstp st(1)
    }
}

// The C runtime's register-based intrinsics [__CIpow], [__CIacos], ...
extern "C" void _CIpow();
extern "C" void _CIacos();
extern "C" void _CIasin();
extern "C" void _CIfmod();

inline double x87_pow(double x, double y) { __asm { fld x } __asm { fld y } __asm { call _CIpow } }
inline double x87_acos(double x) { __asm { fld x } __asm { call _CIacos } }
inline double x87_asin(double x) { __asm { fld x } __asm { call _CIasin } }
inline double x87_fmod(double x, double y) { __asm { fld x } __asm { fld y } __asm { call _CIfmod } }

inline double x87_fabs(double x) { __asm { fld x } __asm { fabs } }

#pragma warning(pop)

// fcomp with test AH,64 (C3): equal, or unordered
inline bool x87_eq(double a, double b) { return !(a < b || a > b); }
