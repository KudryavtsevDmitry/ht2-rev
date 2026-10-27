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

// noinline: a function whose double result must come back in ST(0)
#define X87 __declspec(noinline) inline

// The helpers are in x87.cpp, compiled with /fp:fast so that each is the FPU
// instruction or chain itself. They stay real calls: the result comes back
// unrounded in ST(0), the return register for double.
double x87_sqrt(double x);
double x87_sin(double x);
double x87_cos(double x);
double x87_tan(double x);                          // fptan
double x87_atan2(double y, double x);              // fpatan
double x87_ln(double x);                           // fldln2, fyl2x
double x87_log10(double x);                        // fldlg2, fyl2x
double x87_fabs(double x);
double x87_exp(double x);                          // e^x as VC6 inlines exp()
double x87_exp_neg_sq(double y);                   // e^-(y*y)
double x87_pow_mul(double k, double a, double b);  // k^(a*b) through ln and exp
double x87_hypot(double x, double y);              // sqrt(x*x + y*y)
double x87_inv_dist(float x, float y, double unit);    // unit / (sqrt(x*x + y*y) + unit)
void x87_normalize(const float* v, float* out);    // v / |v|, the length unrounded
// the C runtime's register-based intrinsics __CIpow, __CIacos, __CIasin, __CIfmod
double x87_pow(double x, double y);
double x87_acos(double x);
double x87_asin(double x);
double x87_fmod(double x, double y);

// fcomp with test AH,64 (C3): equal, or unordered
inline bool x87_eq(double a, double b) { return !(a < b || a > b); }
