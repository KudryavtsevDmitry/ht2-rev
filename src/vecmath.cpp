// vecmath.cpp - the engine's vector, matrix and plane math, rewritten from
// king.masm (unit at 0x5ab230; Vec3_MulMatrix comes from 0x43f5b0). Every
// function keeps the original's order of additions and the places where it
// rounds to float, after the rules in x87.h: the results are the same.
// The unit's static initializers (g_identity, g_zeroVec, the axes) are
// compiler-generated and stay assembly.
//
// Building with  nmake ASM_VECMATH=1  makes king.masm assemble the original
// procedures again.

#include "vecmath.h"

ASM_PROC(FUN_5ab190)    // ReadFloat

static float ReadFloat(FILE* file) { return CallC<float>(FUN_5ab190, file); }

static double SumSquares(double x0, double y0, double z0)
{
    return x0 * x0 + y0 * y0 + z0 * z0;
}

// ---------------------------------------------------------------------------
// Vec3

double Vec3_Length(const Vec3* v)
{
    return x87_sqrt(SumSquares(v->x, v->y, v->z));
}

Vec3* Vec3_Normalized(Vec3* out, const Vec3* v)
{
    float len = (float)x87_sqrt(SumSquares(v->x, v->y, v->z));
    if (!(len >= 1e-5)) {
        *out = g_zeroVec;
        return out;
    }
    double z = v->z / (double)len;
    double y = v->y / (double)len;
    double x = v->x / (double)len;
    out->x = (float)x;
    out->y = (float)y;
    out->z = (float)z;
    return out;
}

void Vec3::Normalize()
{
    double len = x87_sqrt(SumSquares(x, y, z));
    if (!(len >= 1e-5))
        return;
    x = (float)(x / len);
    y = (float)(y / len);
    z = (float)(z / len);
}

BOOL Vec3_NearlyEqual(const Vec3* a, const Vec3* b)
{
    Vec3 d;
    d.x = (float)((double)a->x - b->x);
    d.y = (float)((double)a->y - b->y);
    d.z = (float)((double)a->z - b->z);
    return !(x87_fabs(Vec3_Length(&d)) >= 1e-5);
}

// The direction as angles: polar from +z, azimuth around z from +x (0..2pi).
void Vec3_ToAngles(Vec3 v, float* azimuth, float* polar)
{
    Vec3 n;
    v = *Vec3_Normalized(&n, &v);
    if (v.z > 1.0)
        v.z = 1.0f;
    if (!(v.z >= -1.0))
        v.z = -1.0f;
    float p = (float)x87_acos(v.z);
    *polar = p;
    if (!(p >= 0.001)) {
        *azimuth = 0;
    } else if (!(3.1415926 - p >= 0.001)) {
        *azimuth = 0;
    } else {
        double c = v.x / x87_sin(p);
        float q = (float)c;
        if (c > 1.0)
            q = 1.0f;
        if (!(q >= -1.0))
            q = -1.0f;
        *azimuth = (float)x87_acos(q);
    }
    if (!(v.y >= 0.0))
        *azimuth = (float)(6.2831852 - *azimuth);
}

void Vec3::TransformPoint(const Matrix* by)
{
    const float* a = by->m;
    double ny = (((double)a[7] * z + (double)a[1] * x) + (double)a[4] * y) + a[10];
    float nz = (float)((((double)a[8] * z + (double)a[2] * x) + (double)a[5] * y) + a[11]);
    double nx = (((double)a[6] * z + (double)a[3] * y) + (double)a[0] * x) + a[9];
    z = nz;
    x = (float)nx;
    y = (float)ny;
}

void Vec3::TransformDir(const Matrix* by)
{
    const float* a = by->m;
    double ny = ((double)a[7] * z + (double)a[1] * x) + (double)a[4] * y;
    float nz = (float)(((double)a[8] * z + (double)a[2] * x) + (double)a[5] * y);
    double nx = ((double)a[6] * z + (double)a[3] * y) + (double)a[0] * x;
    z = nz;
    x = (float)nx;
    y = (float)ny;
}

// p * M with translation; the same as TransformPoint, summed in another order
Vec3* Vec3_MulMatrix(Vec3* out, const Vec3* p, const Matrix* m)
{
    const float* a = m->m;
    double ny = (((double)a[7] * p->z + (double)a[4] * p->y) + (double)a[1] * p->x) + a[10];
    float nz = (float)((((double)a[8] * p->z + (double)a[5] * p->y) + (double)a[2] * p->x) + a[11]);
    double nx = (((double)a[6] * p->z + (double)a[3] * p->y) + (double)p->x * a[0]) + a[9];
    out->x = (float)nx;
    out->z = nz;
    out->y = (float)ny;
    return out;
}

void Vec3::Read(FILE* file)
{
    x = ReadFloat(file);
    y = ReadFloat(file);
    z = ReadFloat(file);
}

// ---------------------------------------------------------------------------
// Matrix

void Matrix::Read(FILE* file)
{
    for (int i = 0; i < 4; i++)
        ((Vec3*)m)[i].Read(file);
}

Matrix* Matrix_Multiply(Matrix* out, const Matrix* ma, const Matrix* mb)
{
    const float* a = ma->m;
    const float* b = mb->m;
    Matrix r;
    r.m[0] = (float)(((double)b[3] * a[1] + (double)a[0] * b[0]) + (double)a[2] * b[6]);
    r.m[1] = (float)(((double)b[4] * a[1] + (double)b[7] * a[2]) + (double)a[0] * b[1]);
    r.m[2] = (float)(((double)b[5] * a[1] + (double)b[2] * a[0]) + (double)b[8] * a[2]);
    r.m[3] = (float)(((double)a[5] * b[6] + (double)a[4] * b[3]) + (double)a[3] * b[0]);
    r.m[4] = (float)(((double)a[3] * b[1] + (double)a[5] * b[7]) + (double)a[4] * b[4]);
    r.m[5] = (float)(((double)a[5] * b[8] + (double)a[4] * b[5]) + (double)a[3] * b[2]);
    r.m[6] = (float)(((double)a[8] * b[6] + (double)a[6] * b[0]) + (double)a[7] * b[3]);
    r.m[7] = (float)(((double)a[8] * b[7] + (double)a[7] * b[4]) + (double)a[6] * b[1]);
    r.m[8] = (float)(((double)a[8] * b[8] + (double)a[7] * b[5]) + (double)b[2] * a[6]);
    Vec3 t;
    Vec3_MulMatrix(&t, (const Vec3*)&a[9], mb);
    r.m[9] = t.x;
    r.m[10] = t.y;
    r.m[11] = t.z;
    *out = r;
    return out;
}

void Matrix::Transform(const Matrix* by)
{
    ((Vec3*)m)[0].TransformDir(by);
    ((Vec3*)m)[1].TransformDir(by);
    ((Vec3*)m)[2].TransformDir(by);
    ((Vec3*)m)[3].TransformPoint(by);
}

void Matrix::PreMultiply(const Matrix* by)
{
    Matrix r;
    *this = *Matrix_Multiply(&r, by, this);
}

Matrix* Matrix_Translation(Matrix* out, Vec3 t)
{
    Matrix r = g_identity;
    r.m[9] = t.x;
    r.m[10] = t.y;
    r.m[11] = t.z;
    *out = r;
    return out;
}

Matrix* Matrix_Scale(Matrix* out, Vec3 s)
{
    Matrix r = g_identity;
    r.m[0] = s.x;
    r.m[4] = s.y;
    r.m[8] = s.z;
    *out = r;
    return out;
}

static Matrix* SetRows(Matrix* out, float m0, float m1, float m2, float m3, float m4, float m5,
                       float m6, float m7, float m8)
{
    float* r = out->m;
    r[0] = m0; r[1] = m1; r[2] = m2;
    r[3] = m3; r[4] = m4; r[5] = m5;
    r[6] = m6; r[7] = m7; r[8] = m8;
    r[9] = r[10] = r[11] = 0;
    return out;
}

Matrix* Matrix_RotationX(Matrix* out, float angle)
{
    double s = x87_sin(angle), c = x87_cos(angle);
    return SetRows(out, 1, 0, 0, 0, (float)c, (float)s, 0, (float)-s, (float)c);
}

Matrix* Matrix_RotationY(Matrix* out, float angle)
{
    double s = x87_sin(angle), c = x87_cos(angle);
    return SetRows(out, (float)c, 0, (float)-s, 0, 1, 0, (float)s, 0, (float)c);
}

Matrix* Matrix_RotationZ(Matrix* out, float angle)
{
    double s = x87_sin(angle), c = x87_cos(angle);
    return SetRows(out, (float)c, (float)s, 0, (float)-s, (float)c, 0, 0, 0, 1);
}

// A frame at pos whose rotation turns +x towards dir (spherical angles), rolled
// around that direction; the identity for a zero direction.
Matrix* Matrix_LookAt(Matrix* out, Vec3 pos, Vec3 dir, float roll)
{
    Matrix result = g_identity, frame = g_identity, rollRot = g_identity;
    Vec3 n;
    dir = *Vec3_Normalized(&n, &dir);
    if (!Vec3_NearlyEqual(&dir, &g_zeroVec)) {
        float azimuth, polar;
        Vec3 nd = *Vec3_Normalized(&n, &dir);
        Vec3_ToAngles(nd, &azimuth, &polar);
        Matrix_RotationZ(&rollRot, roll);
        Matrix move, rz, ry, rot, inv;
        Matrix_Translation(&move, pos);
        Matrix_RotationZ(&rz, azimuth);
        Matrix_RotationY(&ry, polar);
        Matrix_Multiply(&rot, &ry, &rz);
        Matrix_Multiply(&frame, &rot, &move);
        result = *Matrix_Inverse(&inv, &frame);
        result.Transform(&rollRot);
        result.Transform(&frame);
    }
    *out = result;
    return out;
}

double Matrix::Cofactor(int row, int col) const
{
    switch (row * 3 + col) {
    case 0: return (double)m[8] * m[4] - (double)m[7] * m[5];
    case 1: return (double)m[6] * m[5] - (double)m[8] * m[3];
    case 2: return (double)m[7] * m[3] - (double)m[6] * m[4];
    case 3: return -((double)m[8] * m[1] - (double)m[7] * m[2]);
    case 4: return -((double)m[6] * m[2] - (double)m[8] * m[0]);
    case 5: return -((double)m[7] * m[0] - (double)m[6] * m[1]);
    case 6: return (double)m[5] * m[1] - (double)m[4] * m[2];
    case 7: return (double)m[3] * m[2] - (double)m[5] * m[0];
    case 8: return (double)m[4] * m[0] - (double)m[3] * m[1];
    }
    return 0.0f;
}

double Matrix::Determinant() const
{
    float sum = (float)(Cofactor(0, 2) * m[2]);
    sum = (float)(Cofactor(0, 1) * m[1] + sum);
    return Cofactor(0, 0) * m[0] + sum;
}

// The inverse of a rotation (with scale) and translation, through a copy.
Matrix* Matrix_Inverse(Matrix* out, const Matrix* src)
{
    const float* m = src->m;
    Matrix r;
    double c0 = (double)m[1] * m[5] - (double)m[2] * m[4];
    float c1 = (float)((double)m[3] * m[2] - (double)m[5] * m[0]);
    float c2 = (float)((double)m[0] * m[4] - (double)m[1] * m[3]);
    float det = (float)((c0 * m[6] + (double)c2 * m[8]) + (double)c1 * m[7]);
    r.m[0] = (float)(((double)m[8] * m[4] - (double)m[7] * m[5]) / det);
    r.m[1] = (float)(((double)m[7] * m[2] - (double)m[8] * m[1]) / det);
    r.m[2] = (float)(c0 / det);
    r.m[3] = (float)(((double)m[6] * m[5] - (double)m[8] * m[3]) / det);
    r.m[4] = (float)(((double)m[8] * m[0] - (double)m[6] * m[2]) / det);
    r.m[5] = (float)((double)c1 / det);
    r.m[6] = (float)(((double)m[7] * m[3] - (double)m[6] * m[4]) / det);
    r.m[7] = (float)(((double)m[6] * m[1] - (double)m[7] * m[0]) / det);
    r.m[8] = (float)((double)c2 / det);
    double tx = -m[9], ty = -m[10], tz = -m[11];
    r.m[9] = (float)((tz * r.m[6] + ty * r.m[3]) + tx * r.m[0]);
    r.m[10] = (float)((tz * r.m[7] + ty * r.m[4]) + tx * r.m[1]);
    r.m[11] = (float)((tz * r.m[8] + ty * r.m[5]) + tx * r.m[2]);
    *out = r;
    return out;
}

// The same, written straight into out; the translation uses three of the
// elements before they are rounded to float.
void Matrix_InverseTo(Matrix* out, const Matrix* src)
{
    const float* m = src->m;
    float* r = out->m;
    double c0 = (double)m[1] * m[5] - (double)m[4] * m[2];
    float c1 = (float)((double)m[3] * m[2] - (double)m[0] * m[5]);
    float c2 = (float)((double)m[4] * m[0] - (double)m[1] * m[3]);
    float det = (float)((c0 * m[6] + (double)c2 * m[8]) + (double)c1 * m[7]);
    r[0] = (float)(((double)m[4] * m[8] - (double)m[7] * m[5]) / det);
    r[1] = (float)(((double)m[7] * m[2] - (double)m[8] * m[1]) / det);
    double r2 = c0 / det;
    r[2] = (float)r2;
    r[3] = (float)(((double)m[6] * m[5] - (double)m[8] * m[3]) / det);
    r[4] = (float)(((double)m[8] * m[0] - (double)m[6] * m[2]) / det);
    double r5 = (double)c1 / det;
    r[5] = (float)r5;
    r[6] = (float)(((double)m[7] * m[3] - (double)m[4] * m[6]) / det);
    r[7] = (float)(((double)m[6] * m[1] - (double)m[7] * m[0]) / det);
    double r8 = (double)c2 / det;
    r[8] = (float)r8;
    double tx = -m[9], ty = -m[10], tz = -m[11];
    r[9] = (float)((tz * r[6] + tx * r[0]) + ty * r[3]);
    r[10] = (float)((ty * r[4] + tx * r[1]) + tz * r[7]);
    r[11] = (float)((r8 * tz + r5 * ty) + r2 * tx);
}

void Matrix_TransposeTo(Matrix* out, const Matrix* src)
{
    const float* m = src->m;
    float* r = out->m;
    r[0] = m[0]; r[1] = m[3]; r[2] = m[6];
    r[3] = m[1]; r[4] = m[4]; r[5] = m[7];
    r[6] = m[2]; r[7] = m[5]; r[8] = m[8];
    r[9] = m[9]; r[10] = m[10]; r[11] = m[11];
}

Matrix* Matrix_Transposed(Matrix* out, const Matrix* src)
{
    Matrix r;
    Matrix_TransposeTo(&r, src);
    *out = r;
    return out;
}

// ---------------------------------------------------------------------------
// Plane

Plane* Plane::FromPoints(Vec3 a, Vec3 b, Vec3 c)
{
    double ux = (double)c.x - a.x, uy = (double)c.y - a.y, uz = (double)c.z - a.z;
    float vx = (float)((double)b.x - a.x);
    float vy = (float)((double)b.y - a.y);
    double vz = (double)b.z - a.z;
    n.x = (float)(uz * vy - uy * vz);
    n.y = (float)(vz * ux - uz * vx);
    n.z = (float)(uy * vx - vy * ux);
    if (!(Vec3_Length(&n) >= 0.0001)) {
        n.x = 0;
        n.y = -1.0f;
        n.z = 0;
    } else {
        Vec3 t;
        n = *Vec3_Normalized(&t, &n);
    }
    d = (float)(((double)-n.y * a.y + (double)-n.z * a.z) + (double)-n.x * a.x);
    return this;
}

void Plane::Read(FILE* file)
{
    n.Read(file);
    d = ReadFloat(file);
}

// Normalizes the plane, then moves it by m: the normal rotated, the point
// -d n moved.
Plane* Plane_Transformed(Plane* out, const Plane* p, const Matrix* m)
{
    Plane q = *p;
    q.d = (float)(q.d / Vec3_Length(&q.n));
    q.n.Normalize();
    float px = (float)(-(double)q.n.x * q.d);
    float py = (float)((double)(float)-q.n.y * q.d);
    float pz = (float)((double)(float)-q.n.z * q.d);
    q.n.TransformDir(m);
    const float* a = m->m;
    double tx = (((double)pz * a[6] + (double)py * a[3]) + (double)px * a[0]) + a[9];
    double ty = (((double)pz * a[7] + (double)py * a[4]) + (double)px * a[1]) + a[10];
    double tz = (((double)pz * a[8] + (double)py * a[5]) + (double)px * a[2]) + a[11];
    double dot = (ty * q.n.y + tz * q.n.z) + tx * q.n.x;
    q.d = (float)-dot;
    *out = q;
    return out;
}

void Plane::Transform(const Matrix* m)
{
    Plane t;
    *this = *Plane_Transformed(&t, this, m);
}

// Where the segment p0..p1 crosses the plane: 1 between the ends, 2 outside
// the segment or at an end (hit is still set), 0 if parallel.
int Plane::Intersect(Vec3* hit, Vec3 p0, Vec3 p1) const
{
    Vec3 dv;
    dv.x = (float)((double)p1.x - p0.x);
    dv.y = (float)((double)p1.y - p0.y);
    dv.z = (float)((double)p1.z - p0.z);
    Vec3 dir;
    Vec3_Normalized(&dir, &dv);
    double denom = ((double)n.z * dir.z + (double)n.x * dir.x) + (double)dir.y * n.y;
    if (!(x87_fabs(denom) >= 1e-5))
        return 0;
    double num = (((double)n.z * p0.z + (double)n.x * p0.x) + (double)p0.y * n.y) + d;
    double t = -(num / denom);
    float sx = (float)(dir.x * t);
    float sy = (float)(dir.y * t);
    double sz = t * dir.z;
    hit->x = (float)((double)sx + p0.x);
    hit->y = (float)((double)sy + p0.y);
    hit->z = (float)(sz + p0.z);
    if (hit->x > p0.x && hit->x > p1.x)
        return 2;
    if (!(hit->x >= p0.x) && !(hit->x >= p1.x))
        return 2;
    if (hit->y > p0.y && hit->y > p1.y)
        return 2;
    if (!(hit->y >= p0.y) && !(hit->y >= p1.y))
        return 2;
    if (hit->z > p0.z && hit->z > p1.z)
        return 2;
    if (!(hit->z >= p0.z) && !(hit->z >= p1.z))
        return 2;
    if (x87_eq(hit->x, p0.x) && x87_eq(hit->y, p0.y) && x87_eq(hit->z, p0.z))
        return 2;
    if (x87_eq(hit->x, p1.x) && x87_eq(hit->y, p1.y) && x87_eq(hit->z, p1.z))
        return 2;
    return 1;
}

// ---------------------------------------------------------------------------
// Inline operators the compiler put out of line in other units: the vector
// operators at 0x4bd4d0..0x4bdf10, Vec3_RotateBy at 0x4cb2b0, Vec3_Dot at
// 0x610f00, and an unoptimized 3x3 unit at 0x4e30f4..0x4e3460.

Vec3* Vec3::Zero()
{
    x = y = z = 0;
    return this;
}

Vec3* Vec3::Set(float nx, float ny, float nz)
{
    x = nx;
    y = ny;
    z = nz;
    return this;
}

Matrix* Matrix::Identity()
{
    for (int i = 0; i < 12; i++)
        m[i] = 0;
    *this = g_identity;
    return this;
}

Vec3* Vec3_Scaled(Vec3* out, const Vec3* v, float s)
{
    Vec3 r;
    r.x = (float)((double)s * v->x);
    r.y = (float)((double)s * v->y);
    r.z = (float)((double)s * v->z);
    *out = r;
    return out;
}

Vec3* Vec3_Add(Vec3* out, const Vec3* a, const Vec3* b)
{
    Vec3 r;
    r.x = (float)((double)a->x + b->x);
    r.y = (float)((double)a->y + b->y);
    r.z = (float)((double)a->z + b->z);
    *out = r;
    return out;
}

Vec3* Vec3_Sub(Vec3* out, const Vec3* a, const Vec3* b)
{
    Vec3 r;
    r.x = (float)((double)a->x - b->x);
    r.y = (float)((double)a->y - b->y);
    r.z = (float)((double)a->z - b->z);
    *out = r;
    return out;
}

Vec3* Vec3_Cross(Vec3* out, const Vec3* a, const Vec3* b)
{
    Vec3 r;
    r.x = (float)((double)b->z * a->y - (double)a->z * b->y);
    r.y = (float)((double)a->z * b->x - (double)b->z * a->x);
    r.z = (float)((double)b->y * a->x - (double)b->x * a->y);
    *out = r;
    return out;
}

double Vec3_Dot(const Vec3* a, const Vec3* b)
{
    return ((double)a->z * b->z + (double)a->y * b->y) + (double)a->x * b->x;
}

Vec3* Vec3_MulComponents(Vec3* out, const Vec3* a, const Vec3* b)
{
    float z = (float)((double)a->z * b->z);
    float y = (float)((double)a->y * b->y);
    float x = (float)((double)a->x * b->x);
    out->Set(x, y, z);
    return out;
}

Vec3* Vec3_DivComponents(Vec3* out, const Vec3* a, const Vec3* b)
{
    float z = (float)((double)a->z / b->z);
    float y = (float)((double)a->y / b->y);
    float x = (float)((double)a->x / b->x);
    out->Set(x, y, z);
    return out;
}

Vec3* Vec3_RotateBy(Vec3* out, const Vec3* v, const Matrix* m)
{
    const float* a = m->m;
    double ny = ((double)a[7] * v->z + (double)a[4] * v->y) + (double)a[1] * v->x;
    float nz = (float)(((double)a[8] * v->z + (double)a[5] * v->y) + (double)a[2] * v->x);
    double nx = ((double)a[6] * v->z + (double)a[3] * v->y) + (double)v->x * a[0];
    out->z = nz;
    out->x = (float)nx;
    out->y = (float)ny;
    return out;
}

// each row multiplied component-wise by s: M diag(s)
Matrix* Matrix_ScaleColumns(Matrix* out, const Matrix* m, const Vec3* s)
{
    Matrix r = *m;
    Vec3 t;
    *(Vec3*)&r.m[0] = *Vec3_MulComponents(&t, (const Vec3*)&m->m[0], s);
    *(Vec3*)&r.m[3] = *Vec3_MulComponents(&t, (const Vec3*)&m->m[3], s);
    *(Vec3*)&r.m[6] = *Vec3_MulComponents(&t, (const Vec3*)&m->m[6], s);
    *(Vec3*)&r.m[9] = g_zeroVec;
    *out = r;
    return out;
}

Matrix* Matrix_SubRows(Matrix* out, const Matrix* a, const Matrix* b)
{
    Matrix r;
    r.Identity();
    Vec3 t;
    for (int i = 0; i < 9; i += 3)
        *(Vec3*)&r.m[i] = *Vec3_Sub(&t, (const Vec3*)&a->m[i], (const Vec3*)&b->m[i]);
    *(Vec3*)&r.m[9] = g_zeroVec;
    *out = r;
    return out;
}

Matrix* Matrix_ScaleRows(Matrix* out, const Matrix* m, float s)
{
    Matrix r;
    r.Identity();
    Vec3 t;
    for (int i = 0; i < 9; i += 3)
        *(Vec3*)&r.m[i] = *Vec3_Scaled(&t, (const Vec3*)&m->m[i], s);
    *(Vec3*)&r.m[9] = g_zeroVec;
    *out = r;
    return out;
}

// ---------------------------------------------------------------------------
// Entry points for king.masm. The cdecl functions declared in vecmath.h
// replace their labels themselves; these replace thiscall methods. The
// operators returned a copy of the result: *this op= x; return *this.

extern "C" {

Vec3* __fastcall Vec3_ZeroThis(Vec3* self, void*) { return self->Zero(); }                       // [FUN_4bd4d0]
Vec3* __fastcall Vec3_SetThis(Vec3* self, void*, float x, float y, float z) { return self->Set(x, y, z); } // [FUN_4bdef0]
Matrix* __fastcall Matrix_IdentityThis(Matrix* self, void*) { return self->Identity(); }        // [FUN_4bd4e0]
void __fastcall Vec3_NormalizeThis(Vec3* self, void*) { self->Normalize(); }                         // [FUN_5ab480]
void __fastcall Vec3_TransformPoint(Vec3* self, void*, const Matrix* m) { self->TransformPoint(m); } // [FUN_5ab950]
void __fastcall Vec3_TransformDir(Vec3* self, void*, const Matrix* m) { self->TransformDir(m); }     // [FUN_5ab9b0]
void __fastcall Vec3_Read(Vec3* self, void*, FILE* file) { self->Read(file); }                      // [FUN_5ab790]

Vec3* __fastcall Vec3_MulAssign(Vec3* self, void*, Vec3* ret, float s)                          // [FUN_5ab620]
{
    self->x = (float)((double)s * self->x);
    self->y = (float)((double)s * self->y);
    self->z = (float)((double)s * self->z);
    *ret = *self;
    return ret;
}

Vec3* __fastcall Vec3_DivAssign(Vec3* self, void*, Vec3* ret, float s)                          // [FUN_5ab660]
{
    self->x = (float)(self->x / (double)s);
    self->y = (float)(self->y / (double)s);
    self->z = (float)(self->z / (double)s);
    *ret = *self;
    return ret;
}

Vec3* __fastcall Vec3_AddAssign(Vec3* self, void*, Vec3* ret, const Vec3* v)                    // [FUN_5ab710]
{
    self->x = (float)((double)v->x + self->x);
    self->y = (float)((double)v->y + self->y);
    self->z = (float)((double)v->z + self->z);
    *ret = *self;
    return ret;
}

Vec3* __fastcall Vec3_SubAssign(Vec3* self, void*, Vec3* ret, const Vec3* v)                    // [FUN_5ab750]
{
    self->x = (float)((double)self->x - v->x);
    self->y = (float)((double)self->y - v->y);
    self->z = (float)((double)self->z - v->z);
    *ret = *self;
    return ret;
}

void __fastcall Matrix_Read(Matrix* self, void*, FILE* file) { self->Read(file); }               // [FUN_5aba10]
void __fastcall Matrix_Transform(Matrix* self, void*, const Matrix* by) { self->Transform(by); } // [FUN_5abbd0]
void __fastcall Matrix_Transform2(Matrix* self, void*, const Matrix* by) { self->Transform(by); }// [FUN_5abc00]
void __fastcall Matrix_PreMultiply(Matrix* self, void*, const Matrix* by) { self->PreMultiply(by); } // [FUN_5abc10]
double __fastcall Matrix_Cofactor(const Matrix* self, void*, int row, int col) { return self->Cofactor(row, col); } // [FUN_5ac1b0]
double __fastcall Matrix_Determinant(const Matrix* self, void*) { return self->Determinant(); }  // [FUN_5ac2a0]

Plane* __fastcall Plane_FromPoints(Plane* self, void*, Vec3 a, Vec3 b, Vec3 c) { return self->FromPoints(a, b, c); } // [FUN_5ac6d0]
void __fastcall Plane_Read(Plane* self, void*, FILE* file) { self->Read(file); }                 // [FUN_5ac7f0]
void __fastcall Plane_Transform(Plane* self, void*, const Matrix* m) { self->Transform(m); }     // [FUN_5ab8f0]
int __fastcall Plane_Intersect(const Plane* self, void*, Vec3* hit, Vec3 p0, Vec3 p1)          // [FUN_5ac810]
{
    return self->Intersect(hit, p0, p1);
}

}
