// vecmath.h - the engine's vector, matrix and plane math (unit at 0x5ab230
// of king.masm), used by the vehicles, the physics and the effects.
//
// Matrices are 3x4: three rows of rotation and a row of translation.
// Vectors are rows: v' = v * M, so x' = x m[0] + y m[3] + z m[6] + m[9].
#pragma once

#include "king.h"
#include "x87.h"

struct Matrix;

struct Vec3 {
    float x, y, z;

    Vec3* Zero();                               // the constructor: 0, 0, 0
    Vec3* Set(float x, float y, float z);
    void Normalize();                           // in place; unchanged if shorter than 1e-5
    void TransformPoint(const Matrix* m);       // v = v * M with translation
    void TransformDir(const Matrix* m);         // rotation only
    void Read(FILE* file);                      // three floats
};

struct Matrix {
    float m[12];

    Matrix* Identity();                         // the constructor
    void Transform(const Matrix* by);           // this = this * by
    void PreMultiply(const Matrix* by);         // this = by * this
    double Cofactor(int row, int col) const;    // of the 3x3 part, signed
    double Determinant() const;                 // of the 3x3 part
    void Read(FILE* file);                      // 12 floats
};

struct Plane {
    Vec3  n;                                    // normal
    float d;                                    // n . p + d = 0 on the plane

    Plane* FromPoints(Vec3 a, Vec3 b, Vec3 c);  // normal (c - a) x (b - a), normalized
    void Transform(const Matrix* m);
    int  Intersect(Vec3* hit, Vec3 p0, Vec3 p1) const;
    void Read(FILE* file);
};

ASM_VAR(Matrix, g_identity, $L_7025f0)
ASM_VAR(Vec3, g_zeroVec, $L_702650)
ASM_VAR(Vec3, g_axisX, $L_702640)              // (1, 0, 0)
ASM_VAR(Vec3, g_axisY, $L_702620)              // (0, 1, 0)
ASM_VAR(Vec3, g_axisZ, $L_702630)              // (0, 0, 1)

extern "C" {
double __cdecl Vec3_Length(const Vec3* v);
Vec3* __cdecl Vec3_Normalized(Vec3* out, const Vec3* v);       // zero vector if shorter than 1e-5
BOOL __cdecl Vec3_NearlyEqual(const Vec3* a, const Vec3* b);   // distance below 1e-5
void __cdecl Vec3_ToAngles(Vec3 v, float* azimuth, float* polar);
Vec3* __cdecl Vec3_MulMatrix(Vec3* out, const Vec3* p, const Matrix* m);
Matrix* __cdecl Matrix_Multiply(Matrix* out, const Matrix* a, const Matrix* b);    // a * b
Matrix* __cdecl Matrix_Translation(Matrix* out, Vec3 t);
Matrix* __cdecl Matrix_Scale(Matrix* out, Vec3 s);
Matrix* __cdecl Matrix_RotationX(Matrix* out, float angle);
Matrix* __cdecl Matrix_RotationY(Matrix* out, float angle);
Matrix* __cdecl Matrix_RotationZ(Matrix* out, float angle);
Matrix* __cdecl Matrix_LookAt(Matrix* out, Vec3 pos, Vec3 dir, float roll);
Matrix* __cdecl Matrix_Inverse(Matrix* out, const Matrix* m);
void __cdecl Matrix_InverseTo(Matrix* out, const Matrix* m);
void __cdecl Matrix_TransposeTo(Matrix* out, const Matrix* m);
Matrix* __cdecl Matrix_Transposed(Matrix* out, const Matrix* m);
Plane* __cdecl Plane_Transformed(Plane* out, const Plane* p, const Matrix* m);

// out-of-line copies of inline operators (in the address ranges of other units)
Vec3* __cdecl Vec3_Scaled(Vec3* out, const Vec3* v, float s);
Vec3* __cdecl Vec3_Add(Vec3* out, const Vec3* a, const Vec3* b);
Vec3* __cdecl Vec3_Sub(Vec3* out, const Vec3* a, const Vec3* b);
Vec3* __cdecl Vec3_Cross(Vec3* out, const Vec3* a, const Vec3* b);
double __cdecl Vec3_Dot(const Vec3* a, const Vec3* b);
Vec3* __cdecl Vec3_MulComponents(Vec3* out, const Vec3* a, const Vec3* b);
Vec3* __cdecl Vec3_DivComponents(Vec3* out, const Vec3* a, const Vec3* b);
Vec3* __cdecl Vec3_RotateBy(Vec3* out, const Vec3* v, const Matrix* m);  // rotation only
// 3x3 helpers: the translation of the result is zero
Matrix* __cdecl Matrix_ScaleColumns(Matrix* out, const Matrix* m, const Vec3* s);
Matrix* __cdecl Matrix_SubRows(Matrix* out, const Matrix* a, const Matrix* b);
Matrix* __cdecl Matrix_ScaleRows(Matrix* out, const Matrix* m, float s);
}
