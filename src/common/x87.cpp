// x87.cpp - the x87 helpers of x87.h, in C++. This file alone is compiled
// with /fp:fast, under which the compiler turns sqrt, sin, cos, atan2, tan,
// log, log10, exp and fabs into the FPU instructions themselves (fsqrt,
// fsin, fcos, fpatan, fptan, fyl2x, f2xm1/fscale) and keeps a whole
// expression on the FPU stack: what the game's compiler (VC6) generated
// and what the inline __asm of these helpers did. Each function's code was
// compared with that __asm instruction by instruction. The helpers are
// separate calls (x87.h): their unrounded result comes back in ST(0).

#include <math.h>
#include "x87.h"

#pragma intrinsic(sqrt, sin, cos, atan2, tan, log, log10, exp, fabs)

double x87_sqrt(double x) { return sqrt(x); }
double x87_sin(double x) { return sin(x); }
double x87_cos(double x) { return cos(x); }
double x87_tan(double x) { return tan(x); }
double x87_atan2(double y, double x) { return atan2(y, x); }
double x87_ln(double x) { return log(x); }
double x87_log10(double x) { return log10(x); }
double x87_fabs(double x) { return fabs(x); }

// e^x as VC6 inlines exp()
double x87_exp(double x) { return exp(x); }

// e^-(y*y), one FPU chain
double x87_exp_neg_sq(double y) { return exp(-(y * y)); }

// k^(a*b) as e^(ln k * a * b), one FPU chain
double x87_pow_mul(double k, double a, double b) { return exp(log(k) * a * b); }

// sqrt(x*x + y*y) computed on the FPU in one helper
double x87_hypot(double x, double y) { return sqrt(x * x + y * y); }

// unit / (sqrt(x*x + y*y) + unit) from floats, one FPU chain
double x87_inv_dist(float x, float y, double unit) { return unit / (sqrt(x * x + y * y) + unit); }

// v / |v| per component, the length kept unrounded on the FPU stack
void x87_normalize(const float* v, float* out)
{
    double len = sqrt((v[0] * v[0] + v[1] * v[1]) + v[2] * v[2]);
    out[0] = (float)(v[0] / len);
    out[1] = (float)(v[1] / len);
    out[2] = (float)(v[2] / len);
}

// The C runtime's register-based intrinsics (__CIpow, __CIacos, ...)
double x87_pow(double x, double y) { return pow(x, y); }
double x87_acos(double x) { return acos(x); }
double x87_asin(double x) { return asin(x); }
double x87_fmod(double x, double y) { return fmod(x, y); }
