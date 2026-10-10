// engine.cpp - small functions of several engine units of king.masm
// (sch_embd.cpp and the units at 0x4dd9f0, 0x52abf0, 0x5586f0, 0x57d580 and
// 0x5a3e00): script values, sounds at the volumes of the options, the
// vehicle type classes, transforms of scene objects, the force feedback
// switch, a tokenizer, color blends and little-endian file helpers.
//
// Building with  nmake ASM_ENGINE=1  makes king.masm assemble the original
// procedures again.

#include "car.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl atoi(const char* s);
extern "C" int __cdecl fputc(int c, FILE* f);
extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);
extern "C" void __cdecl ConstructStrRef(DWORD* d, const DWORD* s);     // aistl.cpp
extern "C" int __cdecl Joy_SendCommand(int command);                     // mmt.cpp
extern "C" void __fastcall Engine_4eec20(BYTE* self, void*);            // carphys.cpp
extern "C" void __cdecl Matrix_Decompose6F(const Matrix* m, float* a, float* b, float* c, float* d, float* e,
                                           float* f);                    // world.cpp

ASM_PROC(FUN_44c710)    // vector<12 bytes>::insert_aux
ASM_PROC(FUN_4a1f10)
ASM_PROC(FUN_5c53e0)
ASM_PROC(FUN_5c5310)
ASM_PROC(FUN_5dbbb0)
ASM_PROC(FUN_5c3800)
ASM_PROC(FUN_52e740)
ASM_PROC(FUN_5fd5c0)
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
ASM_PROC(FUN_4e5140)
ASM_PROC(FUN_4e51b1)
ASM_PROC(FUN_4e4d82)
ASM_PROC(FUN_4e480c)
ASM_PROC(FUN_5f25a0)
extern "C" int __fastcall Vehicle_MakeOut(BYTE* self, void*);
ASM_PROC(FUN_581ea0)
ASM_PROC(FUN_5f25c0)
ASM_PROC(FUN_5f2190)

ASM_VAR(int, g_6cec68, $L_6cec68)
ASM_VAR(int, g_6968ac, $L_6968ac)
ASM_VAR(BYTE*, g_options, $L_6cec78)            // +386, +388, +390: volumes in percent
typedef BYTE* Ptrs[1];
typedef int Ints[1];
ASM_VAR(Ptrs, g_6ced98, $L_6ced98)              // 11 per row
ASM_VAR(Ints, g_6cedac, $L_6cedac)
ASM_VAR(int, g_class1, $L_6ce350)               // the first type of each class
ASM_VAR(int, g_class2, $L_6ce354)
ASM_VAR(int, g_class3, $L_6ce358)
ASM_VAR(int, g_class4, $L_6ce35c)
ASM_VAR(int, g_class5, $L_6ce360)
ASM_VAR(int, g_class6, $L_6ce364)
ASM_VAR(int, g_classEnd, $L_6ce368)
ASM_VAR(BYTE*, g_vehicleList2, $L_6ef1f8)
ASM_VAR(AsmLabel, g_6d219c, $L_6d219c)          // 1488-byte records by vehicle type
ASM_VAR(int, g_7025b0, $L_7025b0)
ASM_VAR(double, g_6fb3e8, $L_6fb3e8)
ASM_VAR(int, g_6fb3d8, $L_6fb3d8)
ASM_VAR(int, g_6fb3d0, $L_6fb3d0)
ASM_VAR(int, g_7025c8, $L_7025c8)
ASM_VAR(DWORD, g_67e8c8, $L_67e8c8)
ASM_VAR(int, g_7025cc, $L_7025cc)
ASM_VAR(AsmLabel, g_strCabCl, $L_672e40)        // "%s_cab_cl"
ASM_VAR(AsmLabel, g_vtbl64d578, $L_64d578)
ASM_VAR(AsmLabel, g_vtbl64d6c8, $L_64d6c8)
ASM_VAR(AsmLabel, g_vtbl64d830, $L_64d830)

struct Vec3Bits { DWORD x, y, z; };
struct Rect16 { DWORD m[4]; };

// ---------------------------------------------------------------------------
// Script values (sch_embd.cpp): +0 the object (reference count at +4), +8
// flags (0x10000: immediate)

extern "C" DWORD* __cdecl SchValue_Make10007(DWORD* out, int unused, DWORD v)
{
    out[0] = v;
    out[1] = 0;
    out[2] = 0x10007;
    return out;
}

extern "C" DWORD* __cdecl SchValue_Make10008(DWORD* out, int unused, DWORD v)
{
    out[0] = v;
    out[1] = 0;
    out[2] = 0x10008;
    return out;
}

extern "C" BYTE* __fastcall SchEmbd_Copy(BYTE* self, void*, const BYTE* o)
{
    F(int, self, 4) = F(int, o, 4);
    F(int, self, 8) = F(int, o, 8);
    F(int, self, 16) = F(int, o, 16);
    F(int, self, 20) = F(int, o, 20);
    F(int, self, 24) = F(int, o, 24);
    F(int, self, 28) = F(int, o, 28);
    return self;
}

// the value's assignment operator
extern "C" BYTE* __fastcall SchValue_Assign(BYTE* self, void*, const BYTE* v)
{
    BYTE* o = F(BYTE*, self, 0);
    if (o && !(F(DWORD, self, 8) & 0x10000)) {
        F(int, o, 4)--;
        o = F(BYTE*, self, 0);
        if (F(int, o, 4) == 0
            && !(o == F(BYTE*, v, 0) && F(DWORD, self, 8) == F(DWORD, v, 8) && F(int, self, 4) == F(int, v, 4))
            && o)
            CallVirt(o, 12, 1);
    }
    if (F(DWORD, v, 8) & 0x10000) {
        F(DWORD, self, 8) = F(DWORD, v, 8);
        F(BYTE*, self, 0) = F(BYTE*, v, 0);
        F(int, self, 4) = F(int, v, 4);
        return self;
    }
    F(int, self, 4) = 0;
    if (F(BYTE*, v, 0)) {
        F(BYTE*, self, 0) = F(BYTE*, v, 0);
        F(DWORD, self, 8) = F(DWORD, v, 8);
        F(int, F(BYTE*, self, 0), 4)++;
    } else {
        F(BYTE*, self, 0) = 0;
        F(DWORD, self, 8) = 0;
    }
    return self;
}

extern "C" BYTE* __fastcall Vec12_At(BYTE** self, void*, int i)
{
    return *self + i * 12;
}

extern "C" void __fastcall Vec12_PushBackRef(BYTE* self, void*, const DWORD* x)
{
    BYTE* end = F(BYTE*, self, 4);
    if (end != F(BYTE*, self, 8)) {
        ConstructStrRef((DWORD*)end, x);
        F(BYTE*, self, 4) += 12;
    } else
        CallThis(FUN_44c710, self, end, x);
}

extern "C" BYTE* __fastcall SchEmbd4a1900_Construct(BYTE* self, void*, int unused, int a)
{
    for (int i = 0; i < 40; i += 4)
        F(int, self, i) = 0;
    CallThis(FUN_4a1f10, self, a);
    return self;
}

// ---------------------------------------------------------------------------
// The unit at 0x4dd9f0

extern "C" void __fastcall Obj4dd9f0_Destruct(BYTE* self, void*)
{
    void* p = F(void*, self, 4);
    F(void*, self, 0) = g_vtbl64d578;
    operator delete(p);
}

extern "C" void __fastcall Cab_Release(BYTE* self, void*)
{
    if (!g_6cec68)
        return;
    BYTE* o = F(BYTE*, self, 548);
    if (o) {
        if (F(void*, o, 68))
            CallVirt(F(void*, o, 68), 0, 1);
        F(int, F(BYTE*, self, 548), 68) = 0;
        if (F(BYTE*, self, 548))
            CallVirt(F(BYTE*, self, 548), 0, 1);
    }
    F(int, self, 548) = 0;
    CallVirt(self, 32);
}

// plays the "<type>_cab_cl" sound when the state changes
extern "C" void __fastcall Cab_SetState(BYTE* self, void*, int state)
{
    if (F(int, self, 4824) == state)
        return;
    F(int, self, 4824) = state;
    char name[100];
    sprintf(name, g_strCabCl, (const char*)&g_carTypes[F(int, self, 4720)] + 2226);
    Sound* s = Sound_Find(name);
    if (s)
        CallVirt(s, 40);
}

extern "C" void __cdecl Obj6968ac_Clear()
{
    g_6968ac = 0;
}

extern "C" void __fastcall Wnd4e0140_Destruct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl64d6c8;
    CallThis(FUN_5c53e0, self);
}

extern "C" BYTE* __fastcall Wnd4e01d0_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_5c5310, self, a);
    F(void*, self, 0) = g_vtbl64d6c8;
    g_6968ac = 0;
    return self;
}

// [FUN_5dbbb0] with the rectangle by value; the object in ECX is passed on
extern "C" void __fastcall Obj_4e0630(void* self, void*, const Rect16* r)
{
    CallThis(FUN_5dbbb0, self, *r);
}

extern "C" int __cdecl Joy_ForceFeedback(int on)
{
    return Joy_SendCommand(on ? 8 : 4);
}

// the first part of b (up to a backslash), then a from its first backslash
extern "C" void __cdecl Path_Join(const char* a, const char* b, char* out)
{
    for (char c = *b; c && c != '\\'; c = *++b)
        *out++ = c;
    while (*a && *a != '\\')
        a++;
    for (char c = *a; c; c = *++a)
        *out++ = c;
    *out = 0;
}

extern "C" BYTE* __fastcall Ini4e1e10_Construct(BYTE* self, void*, const char* name)
{
    CallThis(FUN_5c3800, self, name, 0);
    F(void*, self, 0) = g_vtbl64d830;
    return self;
}

// ---------------------------------------------------------------------------
// The unit at 0x52abf0

extern "C" BYTE* __fastcall Pairs5_Clear(BYTE* self, void*)
{
    for (int i = 0; i < 5; i++) {
        F(int, self, i * 4) = 0;
        F(int, self, 20 + i * 4) = 0;
    }
    return self;
}

extern "C" BYTE* __fastcall Slot_Construct(BYTE* self, void*, int row, int b)
{
    F(int, self, 4) = row;
    F(int, self, 8) = b;
    F(int, self, 12) = -1;
    F(int, self, 0) = 0;
    int base = row * 11;
    BYTE* a = g_6ced98[base];
    for (int i = 0; i < 5; i++) {         // tests the same slot each time, as the original
        if (a && g_6cedac[base]) {
            F(int, self, 0) = i;
            break;
        }
    }
    int e = base + F(int, self, 0);
    if (!g_6cedac[e] && g_6ced98[e])
        CallVirt(g_6ced98[e], 40);
    g_6cedac[F(int, self, 4) * 11 + F(int, self, 0)]++;
    return self;
}

extern "C" void __fastcall Obj52e540_Set3576(BYTE* self, void*, int v)
{
    F(int, self, 3576) = v;
}

// [FUN_52e740] for the vehicle with id p[0] (NULL if there is none); the
// object in ECX is passed on
extern "C" int __fastcall Vehicles_52e8c0(void* self, void*, const int* p)
{
    DWORD n = (DWORD)g_vehicleList2;
    BYTE* car = 0;
    while (n && n - 4) {
        if (p[0] == F(int, n - 4, 9544)) {
            car = (BYTE*)(n - 4);
            break;
        }
        n = F(DWORD, n - 4, 8);
    }
    return CallThis<int>(FUN_52e740, self, car, p[1], p + 3);
}

// the list number of a vehicle type (class ranges), 0 for none
extern "C" int __cdecl CarType_Number(int t)
{
    if (t >= g_class1 && t < g_class2)
        return t - g_class1 + 1;
    if (t >= g_class2 && t < g_class3)
        return t - g_class2 + 31;
    if (t >= g_class3 && t < g_class4)
        return t - g_class3 + 41;
    if (t >= g_class4 && t < g_class5)
        return t - g_class4 + 51;
    if (t >= g_class5 && t < g_class6)
        return t - g_class5 + 61;
    if (t >= g_class6 && t < g_classEnd)
        return t - g_class6 + 81;
    return 0;
}

extern "C" int __cdecl CarType_FromNumber(int n)
{
    if (n >= 1 && n <= 30)
        return g_class1 + n - 1;
    if (n >= 31 && n <= 40)
        return g_class2 + n - 31;
    if (n >= 41 && n <= 50)
        return g_class3 + n - 41;
    if (n >= 51 && n <= 60)
        return g_class4 + n - 51;
    if (n >= 61 && n <= 80)
        return g_class5 + n - 61;
    if (n >= 81 && n <= 90)
        return g_class6 + n - 81;
    return 0;
}

// the volume of the options at off times v
static float Volume(int off, float v)
{
    double k = g_options ? F(short, g_options, off) * 0.01 : 1.0;
    return (float)(v * k);
}

static float VolumeMul(int off, float v)
{
    double k = g_options ? F(short, g_options, off) * 0.01 : 1.0;
    return (float)(k * v);
}

extern "C" void __cdecl Sound_Play388(Sound* s, float v)
{
    if (!s)
        return;
    float x = Volume(388, v);
    s->Play(x, x);
}

extern "C" void __cdecl Sound_Play3_388(void* s, float v)
{
    if (!s)
        return;
    float x = Volume(388, v);
    CallThis(FUN_5fd430, s, x, x, 0);
}

extern "C" void __cdecl Sound_Play3_388Pos(void* s, float v, int pos)
{
    if (!s)
        return;
    Sound_Play3_388(s, v);
    CallThis(FUN_5fd5c0, s, pos);
}

extern "C" void __cdecl Sound_Start388(void* s, int a, float v, int b)
{
    if (s)
        CallC(FUN_4e5140, s, a, VolumeMul(388, v), b);
}

extern "C" void __cdecl Sound_Start388Pos(void* s, int a, float v, int pos, int b)
{
    if (!s)
        return;
    Sound_Start388(s, a, v, b);
    CallThis(FUN_5fd5c0, s, pos);
}

extern "C" void __cdecl Sound_Loop388(void* s, int a, float v, int b)
{
    if (s)
        CallC(FUN_4e51b1, s, a, VolumeMul(388, v), b);
}

extern "C" void __cdecl Sound_Play390(void* s, float v, int a)
{
    if (!s)
        return;
    float x = Volume(390, v);
    CallC(FUN_4e4d82, s, x, a, 0);
    CallThis(FUN_5fd430, s, x, x, 0);
}

extern "C" void __cdecl Sound_Play390If(void* s, float v, int unused, int a)
{
    if (s)
        Sound_Play390(s, v, a);
}

extern "C" void __cdecl Sound_Play3_386(void* s, float v)
{
    if (!s)
        return;
    float x = Volume(386, v);
    CallThis(FUN_5fd430, s, x, x, 0);
}

extern "C" void __cdecl Sound_Start386(void* s, int a, float v, int b)
{
    if (s)
        CallC(FUN_4e5140, s, a, VolumeMul(386, v), b);
}

extern "C" void __cdecl Sound_Loop386(void* s, int a, float v, int b)
{
    if (s)
        CallC(FUN_4e51b1, s, a, VolumeMul(386, v), b);
}

// ---------------------------------------------------------------------------
// The unit at 0x5586f0 (the vehicle; +21600 its engine)

extern "C" void __fastcall Car_558930(BYTE* self, void*)
{
    BYTE* e = F(BYTE*, self, 21600);
    F(int, self, 21880) = 0;
    F(int, self, 20936) = 1;
    F(Vec3, e, 10244) = g_zeroVec;
    F(int, self, 9528) = 0;
    F(int, F(BYTE*, self, 21600), 10096) = 0;
    Engine_4eec20(F(BYTE*, self, 21600), 0);
}

// copies the engine's state to the towed vehicle at +21880
extern "C" void __fastcall Car_CopyToTowed(BYTE* self, void*)
{
    if (!F(BYTE*, self, 21880))
        return;
    BYTE* from = F(BYTE*, self, 21600) + 9984;
    BYTE* to = F(BYTE*, F(BYTE*, self, 21880), 21600) + 9984;
    F(int, to, 92) = F(int, from, 92);
    F(int, to, 16) = F(int, from, 16);
    F(int, to, 12) = F(int, from, 12);
    F(short, to, 20) = F(short, from, 20);
    F(short, to, 22) = F(short, from, 22);
    to[0] = from[0];
    F(short, to, 26) = F(short, from, 26);
    F(short, to, 24) = F(short, from, 24);
    F(int, F(BYTE*, self, 21880), 20860) = F(int, self, 20860);
    F(int, F(BYTE*, self, 21880), 20916) = F(int, self, 20916);
    if (F(int, self, 21892)) {
        F(short, to, 22) = 0;
        F(int, to, 16) = 0;
    }
}

extern "C" void __fastcall Car_55ed50(BYTE* self, void*)
{
    F(int, self, 20936) = 1;
    F(int, self, 9528) = 0;
    F(Vec3, F(BYTE*, self, 21600), 10244) = g_zeroVec;
    F(float, F(BYTE*, self, 21600), 10000) = 1.0f;
    F(short, F(BYTE*, self, 21600), 10006) = 1;
    Engine_4eec20(F(BYTE*, self, 21600), 0);
    F(int, self, 21880) = 0;
    int p = F(int, g_6d219c, F(int, self, 10420) * 1488 + 92);
    if (p)
        CallC(FUN_4e480c, p, 0);
}

// ---------------------------------------------------------------------------
// Scene objects (the unit at 0x57d580): +1440 selects the second set

extern "C" Matrix* __fastcall SceneObj_Matrix1(BYTE* self, void*, Matrix* out)
{
    *out = F(Matrix, self, F(int, self, 1440) ? 1792 : 500);
    return out;
}

extern "C" Matrix* __fastcall SceneObj_Matrix2(BYTE* self, void*, Matrix* out)
{
    *out = F(Matrix, self, F(int, self, 1440) ? 1840 : 596);
    return out;
}

extern "C" int __fastcall SceneObj_Get800(BYTE* self, void*)
{
    return F(int, self, 1440) ? F(int, self, 2032) : F(int, self, 800);
}

extern "C" Matrix* __fastcall SceneObj_Matrix324(BYTE* self, void*, Matrix* out)
{
    Matrix m;
    for (int i = 0; i < 4; i++)
        F(Vec3, &m, i * 12) = Vec3();
    m = g_identity;
    CallThis(FUN_5f25a0, F(void*, self, 324), &m);
    *out = m;
    return out;
}

extern "C" Matrix* __fastcall SceneObj_Matrix548(BYTE* self, void*, Matrix* out) { *out = F(Matrix, self, 548); return out; }
extern "C" Matrix* __fastcall SceneObj_Matrix644(BYTE* self, void*, Matrix* out) { *out = F(Matrix, self, 644); return out; }

extern "C" int __fastcall SceneObj_57e030(BYTE* self, void*, int* out)
{
    out[0] = F(int, self, 396);
    out[1] = F(int, self, 400);
    out[3] = F(int, self, 400);
    out[2] = F(int, self, 400);
    out[4] = F(int, self, 400);
    return Vehicle_MakeOut(self, 0);
}

// the velocity at +464 in the frame of the matrix at +644, divided by +404
extern "C" Vec3* __fastcall SceneObj_LocalVelocity(BYTE* self, void*, Vec3* out)
{
    float v[3];
    v[0] = (float)(((double)F(float, self, 656) * F(float, self, 468) + (double)F(float, self, 668) * F(float, self, 472))
                   + (double)F(float, self, 464) * F(float, self, 644));
    v[1] = (float)(((double)F(float, self, 660) * F(float, self, 468) + (double)F(float, self, 648) * F(float, self, 464))
                   + (double)F(float, self, 672) * F(float, self, 472));
    v[2] = (float)(((double)F(float, self, 664) * F(float, self, 468) + (double)F(float, self, 652) * F(float, self, 464))
                   + (double)F(float, self, 676) * F(float, self, 472));
    Vec3_DivComponents(out, (const Vec3*)v, (const Vec3*)(self + 404));
    return out;
}

extern "C" void __fastcall SceneObj_SetFlags(BYTE* self, void*, unsigned flags)
{
    F(unsigned, self, 892) |= flags;
    F(float, self, 904) = (float)Game_Time();
}

// the offset of a from b and the angles of a's rotation relative to m
extern "C" void __cdecl SceneObj_Relative(const BYTE* a, const BYTE* b, const Matrix* m, float* offset, float* angles)
{
    float d[3];
    d[0] = (float)((double)F(float, a, 36) - F(float, b, 36));
    d[1] = (float)((double)F(float, a, 40) - F(float, b, 40));
    d[2] = (float)((double)F(float, a, 44) - F(float, b, 44));
    offset[0] = d[0];
    offset[1] = d[1];
    offset[2] = d[2];
    Matrix r;
    Matrix_Multiply(&r, (const Matrix*)a, m);
    F(Vec3, &r, 36) = g_zeroVec;
    float x = 0, y = 0, z = 0;
    Matrix_Decompose6F(&r, &x, &y, &z, angles + 2, angles, angles + 1);
    angles[0] = (float)-(double)angles[0];
}

extern "C" int __cdecl SceneObj_5820c0(const BYTE* o, int a, int b, int c)
{
    Vec3Bits p = F(Vec3Bits, o, 964);
    Vec3Bits q = F(Vec3Bits, o, 976);
    if (F(int, o, 392) == 2) {
        int r = CallC<int>(FUN_581ea0, o, a, &p, &q, b, 0, c);
        if (r)
            return r;
    }
    return 0;
}

extern "C" void __fastcall Slots6_Remove(BYTE* self, void*, int v)
{
    for (int i = 0; i < 6; i++)
        if (F(int, self, 3508 + i * 4) == v)
            F(int, self, 3508 + i * 4) = 0;
}

// ---------------------------------------------------------------------------
// The unit at 0x5a3e00: nodes with a frame (+48) or a position (+52)

extern "C" void __fastcall Node_Matrix(BYTE* self, void*, Matrix* out)
{
    if (F(void*, self, 48)) {
        CallThis(FUN_5f25c0, F(void*, self, 48), out);
        return;
    }
    Matrix m;
    *out = *Matrix_Translation(&m, *F(Vec3*, self, 52));
}

extern "C" void __fastcall Node_SetMatrix(BYTE* self, void*, const Matrix* m)
{
    if (F(void*, self, 48)) {
        CallThis(FUN_5f2190, F(void*, self, 48), m, 0);
        return;
    }
    *F(Vec3Bits*, self, 52) = F(Vec3Bits, m, 36);
    BYTE* q = F(BYTE*, self, 32) + 20;
    int w = F(int, q, 12);
    *(Vec3Bits*)q = *F(Vec3Bits*, self, 52);
    F(int, q, 12) = w;
}

extern "C" void __fastcall Node_Position(BYTE* self, void*, Vec3Bits* out)
{
    Matrix m;
    for (int i = 0; i < 4; i++)
        F(Vec3, &m, i * 12) = Vec3();
    if (F(void*, self, 48)) {
        CallThis(FUN_5f25c0, F(void*, self, 48), &m);
        *out = F(Vec3Bits, &m, 36);
        return;
    }
    *out = *F(Vec3Bits*, self, 52);
}

extern "C" void __cdecl Set7025b0(int v)
{
    g_7025b0 = v;
}

// marks pixel (x, y) of the surface's current buffer
extern "C" void __cdecl Surface_Mark(const WORD* s, int x, int y, const BYTE* flags)
{
    if (!flags || !(flags[28] & 1))
        return;
    BYTE* buf = *(BYTE**)((BYTE*)s + 24 + F(int, s, 16) * 4);
    if (buf && y < s[1] && x < s[0] && y >= 0 && x >= 0)
        *(WORD*)(buf + s[2] * x + s[3] * y) = 0x8888;
}

extern "C" void __fastcall Obj5a58c0_Start(BYTE* self, void*)
{
    if (F(DWORD, self, 164) & 0x60)
        return;
    F(int, self, 160) = 100;
    F(DWORD, self, 164) |= 0x20;
}

extern "C" void __fastcall Obj5a6860_Set(BYTE* self, void*, int a, int b, int c, int d, int flags)
{
    if (!(flags & 16)) {
        F(int, self, 40) = a;
        F(int, self, 44) = b;
    }
    if (!(flags & 32)) {
        F(int, self, 48) = c;
        F(int, self, 52) = d;
    }
}

extern "C" void __fastcall Objects_Release(BYTE* self, void*)
{
    for (int i = 0; i < F(int, self, 24); i++) {
        void* o = F(void**, self, 32)[i];
        if (o)
            CallVirt(o, 0, 1);
        void* p = F(void**, self, 36)[i];
        if (p) {
            CallVirt(p, 36);
            p = F(void**, self, 36)[i];
            if (p)
                CallVirt(p, 0, 1);
        }
    }
    if (F(void*, self, 32))
        operator delete(F(void*, self, 32));
    if (F(void*, self, 36))
        operator delete(F(void*, self, 36));
}

// "r g b" (0..255) into a color of floats
extern "C" void __cdecl Color_Parse(float* out, char* text)
{
    out[0] = (float)(atoi(NextToken(&text)) * 0.00392156862745098);
    out[1] = (float)(atoi(NextToken(&text)) * 0.00392156862745098);
    out[2] = (float)(atoi(NextToken(&text)) * 0.00392156862745098);
}

extern "C" void __fastcall Obj5a8960_Set(BYTE* self, void*, int a, int b, double freq, int d, int e, int f, int g)
{
    F(int, self, 88) = a;
    F(int, self, 92) = b;
    F(int, self, 96) = d;
    F(int, self, 100) = e;
    F(double, self, 104) = freq * 6.2831852;
    F(int, self, 112) = f;
    F(int, self, 116) = g;
}

extern "C" void __cdecl Color_Lerp(const float* a, const float* b, float t, float* out)
{
    double x = t;
    if (x > 1.0f)
        x = 1.0f;
    else if (!(x >= 0.0f))
        x = 0.0f;
    double u = 1.0f - x;
    out[0] = (float)(x * a[0] + u * b[0]);
    out[1] = (float)(u * b[1] + x * a[1]);
    out[2] = (float)(u * b[2] + x * a[2]);
}

extern "C" void __cdecl Set6fb3e8(int a, float f)
{
    g_6fb3e8 = f;
    g_6fb3d8 = a;
    g_6fb3d0 = a;
}

// bilinear blend of the four values at p by (a, b), a folded at 0.5
extern "C" double __cdecl Blend4(float a, float b, const float* p)
{
    double x = (double)a + a;
    if (x > 1.0f)
        x = 2.0f - x;
    double nb = 1.0f - (double)b;
    double l = nb * p[2] + (double)b * p[0];
    double r = nb * p[3] + (double)b * p[1];
    return l * (1.0f - x) + r * x;
}

extern "C" double __fastcall Wave_Value(BYTE* self, void*, double phase)
{
    double w = 1.0f - ((double)F(float, self, 544) + F(float, self, 544));
    double s = x87_sin(F(double, self, 120)) * 0.3 + 1.0;
    double m = s * (1.0 - x87_fabs(w));
    double v = (m * x87_sin(F(double, self, 104) - 1.5707963 + phase) + w + 1.0) * 0.5;
    if (v > 1.0)
        return 1.0;
    if (!(v >= 0.0))
        return 0.0;
    return v;
}

extern "C" void __cdecl Fog_Set(double level, int a, int b)
{
    int i = (int)(level * 255.0);
    g_7025c8 = a;
    g_67e8c8 = ((((DWORD)i | 0xffffff00) << 8 | i) << 8) | i;
    g_7025cc = b;
}

extern "C" void __cdecl File_WriteShort(int v, FILE* f)
{
    fputc(v, f);
    fputc(v >> 8, f);
}

extern "C" int __cdecl File_ReadInt(FILE* f)
{
    int v;
    fread(&v, 4, 1, f);
    return v;
}

extern "C" void __cdecl File_WriteInt(int v, FILE* f)
{
    File_WriteShort(v, f);
    File_WriteShort(v >> 16, f);
}

extern "C" float __cdecl File_ReadFloat(FILE* f)
{
    float v;
    fread(&v, 4, 1, f);
    return v;
}

// the next word of *p (spaces and tabs separate them); *p moves past it,
// NULL at the end of the line
extern "C" char* __cdecl NextToken(char** pp)
{
    BYTE* p = (BYTE*)*pp;
    if (!p)
        return 0;
    while (*p == ' ' || *p == '\t')
        p++;
    if (*p < 32) {
        *pp = 0;
        return 0;
    }
    BYTE* start = p;
    if (*p > 32) {
        do
            p++;
        while (*p > 32);
    }
    if (*p != ' ' && *p != '\t') {
        *pp = 0;
        *p = 0;
        return (char*)start;
    }
    *p = 0;
    p++;
    if (*p < 32 && *p != '\t') {
        *pp = 0;
        return (char*)start;
    }
    *pp = (char*)p;
    return (char*)start;
}

// ---------------------------------------------------------------------------
// medium-sized functions

ASM_PROC(FUN_4e2821)    // (vector, name, scale as float)
ASM_PROC(FUN_4ee900)
ASM_PROC(FUN_4e480c)
ASM_PROC(FUN_55f000)    // whether a vehicle can take another (vehicle, other)
ASM_PROC(FUN_4f1180)
ASM_PROC(FUN_556a40)
ASM_PROC(FUN_55f5f0)
ASM_PROC(FUN_5ef3d0)    // a model by name
ASM_PROC(FUN_5ef2b0)    // a key of a model (parent, name)
ASM_PROC(FUN_52b030)
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
ASM_VAR(DWORD, g_702650, $L_702650)
ASM_VAR(DWORD, g_702654, $L_702654)
ASM_VAR(DWORD, g_702658, $L_702658)
ASM_VAR(AsmLabel, g_6d219c, $L_6d219c)          // 1488-byte records
ASM_VAR(BYTE*, g_6ef1f8, $L_6ef1f8)
ASM_VAR(int, g_carModelCount, $L_6ce34c)
ASM_VAR(AsmLabel, g_carModelNames, $L_6c6b68)   // 876 bytes per car
typedef int CarParts[1];
ASM_VAR(CarParts, g_carPartModels, $L_6ced98)   // 11 ints per car: 5 models, 5 counts, a key
ASM_VAR(CarParts, g_carPartUses, $L_6cedac)

// the vehicle's output state from its current transform; EAX is left
// holding the dword at +444, which SceneObj_57e030 passes on
extern "C" int __fastcall Vehicle_MakeOut(BYTE* self, void*)
{
    memcpy(self + 3336, self + 500, 48);
    memcpy(self + 3384, self + 596, 48);
    float scale = 150.0f;
    CallC(FUN_4e2821, self + 416, "make_out#1", scale);
    memcpy(self + 3300, self + 416, 12);
    F(DWORD, self, 3460) = F(DWORD, self, 888);
    memcpy(self + 3312, self + 428, 12);
    memcpy(self + 3324, self + 440, 12);
    F(DWORD, self, 3432) = F(DWORD, self, 800);
    memcpy(self + 3172, self + 500, 48);
    return F(int, self, 444);
}

extern "C" void __fastcall Vehicle_Attach55ec70(BYTE* self, void*, BYTE* p)
{
    F(BYTE*, self, 21880) = p;
    F(BYTE*, self, 9528) = p ? p + 16 : 0;
    BYTE* e = F(BYTE*, self, 21600);
    F(int, self, 20936) = 0;
    F(DWORD, e, 10244) = g_702650;
    F(DWORD, e, 10248) = g_702654;
    F(DWORD, e, 10252) = g_702658;
    F(int, F(BYTE*, self, 21600), 10096) = 1;
    F(int, F(BYTE*, self, 21600), 10100) = 0;
    CallThis(FUN_4ee900, F(BYTE*, self, 21600));
    F(int, F(BYTE*, self, 21600), 10000) = 0;
    F(short, F(BYTE*, self, 21600), 10006) = 0;
    F(short, F(BYTE*, self, 21600), 10008) = 0;
    F(short, F(BYTE*, self, 21600), 10010) = 0;
    F(int, self, 21884) = 0;
    int x = F(int, (BYTE*)g_6d219c + F(int, self, 10420) * 1488, 92);
    if (x)
        CallC(FUN_4e480c, x, 0);
}

// finds a vehicle that can take this one; 0 when attached, else the reason
extern "C" int __cdecl Vehicle_FindTaker(BYTE* v, int arg)
{
    if (!v)
        return 1;
    if (CallVirt<int>(v, 4) != 1)
        return 2;
    if (!F(BYTE*, v, 21600))
        return 3;
    if (F(int, v, 21880))
        return 4;
    if (!g_6ef1f8)
        return 8;
    BYTE m[48];
    CallThis(FUN_57de90, v + 16, m);
    BYTE* found = 0;
    int r = 1;
    for (BYTE* o = g_6ef1f8 - 4; o;) {
        if (CallVirt<int>(o, 4) == 2) {
            found = o;
            r = CallC<int>(FUN_55f000, v, o);
            if (r == 0)
                break;
        }
        if (!F(BYTE*, o, 8))
            break;
        o = F(BYTE*, o, 8) - 4;
    }
    if (r != 0)
        return r;
    if (F(int, F(BYTE*, found, 21600), 13124)) {
        CallThis(FUN_4f1180, F(BYTE*, found, 21600));
        CallVirt(F(BYTE*, found, 21600), 0);
        CallThis(FUN_556a40, found);
    }
    CallC(FUN_55f5f0, v, found, arg);
    return 0;
}

// the cars' collision models and their hit keys
extern "C" void __cdecl Cars_LoadCollision()
{
    char name[100];
    char* car = g_carModelNames;
    int* rec = &g_carPartUses[0];
    for (int i = 0; i < g_carModelCount; i++, car += 876, rec += 11) {
        sprintf(name, "%s_car_cl", car);
        rec[-5] = CallC<int>(FUN_5ef3d0, name);
        rec[0] = 0;
        if (rec[-5] == 0) {
            for (int k = 0; k < 5; k++) {
                sprintf(name, "%s%d_car_cl", car, k + 1);
                rec[k - 5] = CallC<int>(FUN_5ef3d0, name);
                rec[k] = 0;
            }
        } else
            for (int k = 1; k < 5; k++) {
                rec[k - 5] = 0;
                rec[k] = 0;
            }
        sprintf(name, "hit_%s", car);
        int hit = CallC<int>(FUN_5ef2b0, 0, name);
        rec[5] = 0;
        if (hit)
            rec[5] = CallC<int>(FUN_5ef2b0, hit, "CollisionKey");
    }
}

// switches a car's part to model k, releasing and taking the models
extern "C" void __fastcall CarPart_Select(int* self, void*, int k)
{
    if (k == self[0] || k < 0 || k >= 5 || !g_carPartModels[self[1] * 11 + k])
        return;
    g_carPartUses[self[1] * 11 + self[0]]--;
    int i = self[1] * 11 + self[0];
    if (g_carPartUses[i] == 0)
        CallVirt((void*)g_carPartModels[i], 36);
    self[0] = k;
    if (!self[2])
        CallThis(FUN_52b030, self, 1);
    i = self[1] * 11 + self[0];
    if (g_carPartUses[i] == 0)
        CallVirt((void*)g_carPartModels[i], 40);
    g_carPartUses[self[1] * 11 + self[0]]++;
}

// ---------------------------------------------------------------------------
// the crowd (the ovation sound) of a scene object: its matrix is the object's
// moved by ten times the vector at 702630; fn is called when it plays

ASM_PROC(FUN_5e48f0)    // an empty function: the default callback
ASM_VAR(float, g_eng702630, $L_702630)
ASM_VAR(float, g_eng702634, $L_702634)
ASM_VAR(float, g_eng702638, $L_702638)
ASM_VAR(float, g_ten64c450, $L_64c450)          // 10.0
ASM_VAR(AsmLabel, g_strOvation, $L_678540)      // "ovation1Sound"

extern "C" void __fastcall Crowd_Init(BYTE* self, void*, void* fn)
{
    F(DWORD, self, 3580) = F(DWORD, self, 800);
    Vec3 v;
    v.x = g_eng702630 * g_ten64c450;
    v.y = g_eng702634 * g_ten64c450;
    v.z = g_eng702638 * g_ten64c450;
    Matrix t, r;
    Matrix_Translation(&t, v);
    F(Matrix, self, 3584) = *Matrix_Multiply(&r, (const Matrix*)(self + 500), &t);
    F(int, self, 3632) = 0;
    F(int, self, 3576) = 0;
    F(void*, self, 3648) = fn ? fn : (void*)FUN_5e48f0;
    int h = CallC<int>(FUN_5ef3d0, (const char*)g_strOvation);
    F(int, self, 3640) = 0;
    F(int, self, 3644) = 0;
    F(int, self, 3636) = h;
}

// Save the previous transform and motion state before advancing the scene.
extern "C" BYTE* __fastcall SceneObj_SaveState(BYTE* self, void*)
{
    F(Matrix, self, 548) = F(Matrix, self, 500);
    F(Matrix, self, 644) = F(Matrix, self, 596);
    F(Vec3, self, 452) = F(Vec3, self, 416);
    F(Vec3, self, 464) = F(Vec3, self, 440);
    F(DWORD, self, 804) = F(DWORD, self, 800);
    return self;
}

ASM_PROC(FUN_591c30)
ASM_PROC(FUN_5928d0)

// Collision routines consume both the world transform and its inverse.
extern "C" int __cdecl SceneObj_Collide(BYTE* a, BYTE* b, DWORD arg)
{
    Matrix worldA, worldB, inverseA, inverseB;
    SceneObj_Matrix1(a, 0, &worldA);
    SceneObj_Matrix1(b, 0, &worldB);
    SceneObj_Matrix2(a, 0, &inverseA);
    SceneObj_Matrix2(b, 0, &inverseB);
    return CallC<int>(FUN_591c30, a, &worldA, &inverseA, b, &worldB, &inverseB, arg);
}

extern "C" int __cdecl SceneObj_CollideShape(void* shape, DWORD arg2, DWORD arg3,
                                             BYTE* object, DWORD arg5)
{
    if (!shape || !object)
        return 0;
    Matrix world, inverse;
    SceneObj_Matrix1(object, 0, &world);
    SceneObj_Matrix2(object, 0, &inverse);
    return CallC<int>(FUN_5928d0, shape, arg2, arg3, object, &world, &inverse, arg5);
}

ASM_VAR(BYTE*, g_effectManager, $L_6f6910)

// Restart a positioned effect, releasing its previous sound first.
extern "C" const Matrix* __fastcall SceneEffect_Start(BYTE* self, void*, DWORD key,
                                                       const Matrix* transform)
{
    if (g_effectManager)
        CallThis(FUN_5f25c0, F(BYTE*, g_effectManager, 340), self + 80);
    F(DWORD, self, 232) = 0;
    F(DWORD, self, 316) = key;
    double now = Game_Time();
    F(DWORD, self, 228) = 0;
    F(float, self, 224) = (float)now;
    F(DWORD, self, 324) = 0;
    F(Matrix, self, 268) = *transform;
    F(Matrix, self, 32) = *transform;
    return transform;
}

// A position below the sentinel threshold resets both scripted positions.
ASM_VAR(float, g_positionResetThreshold, $L_650624)
ASM_VAR(AsmLabel, g_setPosition1, $L_67d4cc)
ASM_VAR(AsmLabel, g_setPosition2, $L_67d4c0)
ASM_VAR(AsmLabel, g_setPlace, $L_67d4b4)
ASM_PROC(FUN_58e7f0)

extern "C" int __fastcall SceneObj_SetPosition(BYTE* self, void*, DWORD arg, BYTE* state)
{
    if (!(F(float, state, 44) >= g_positionResetThreshold)) {
        F(float, state, 44) = -1000.0f;
        CallVirt(self, 144, &g_702650, g_setPosition1);
        CallVirt(self, 148, &g_702650, g_setPosition2);
    }
    CallVirt(self, 136, arg, state);
    return CallVirt<int>(self, 60);
}

extern "C" int __fastcall SceneObj_SetPlace(BYTE* self, void*, DWORD arg, DWORD place)
{
    CallThis(FUN_58e7f0, self, arg, g_setPlace, 0);
    if (F(BYTE, self, 1132)) {
        CallVirt(F(BYTE*, self, 32), 36);
        return CallVirt<int>(self, 40, 0, self + 1132);
    }
    CallThis(FUN_5f2190, F(BYTE*, F(BYTE*, self, 32), 68), arg, 0);
    CallVirt(F(BYTE*, self, 32), 36);
    F(DWORD, self, 56) = place;
    return CallVirt<int>(self, 40, place, 0);
}

ASM_VAR(short, g_sceneCommandCode, $L_67d10a)
ASM_VAR(BYTE*, g_sceneCommandList, $L_6f653c)

// List links point four bytes into an object's secondary base. Command
// dispatch uses the primary base, 376 bytes before that secondary base.
static BYTE* SceneObj_FindCommandTarget(int key)
{
    BYTE* link = g_sceneCommandList;
    while (link) {
        BYTE* object = link - 4;
        if (!object)
            break;
        if (F(int, object, 812) == key)
            return object - 376;
        link = F(BYTE*, object, 8);
    }
    return 0;
}

extern "C" int __cdecl SceneObj_DispatchCommand(int command, BYTE* packet)
{
    if (command != g_sceneCommandCode)
        return 0;
    BYTE* target = SceneObj_FindCommandTarget(F(int, packet, 0));
    if (!target)
        return 0;
    CallVirt(target, 196, packet);
    return 1;
}

extern "C" int __fastcall SceneObj_DispatchTarget(BYTE* self, void*, const BYTE* packet)
{
    BYTE* target = SceneObj_FindCommandTarget(F(int, packet, 8));
    if (!target)
        return 0;
    return CallVirt<int>(self, 192, target, F(DWORD, packet, 12));
}

extern "C" void __fastcall Pooled_Set4(BYTE* self, void*); // mmt.cpp

// Advance the texture frame, clamping one-shot sequences and wrapping loops.
extern "C" void __fastcall SceneTexture_Advance(BYTE* self, void*)
{
    int frame = (int)((double)F(float, self, 104) / F(float, self, 96)
                      + F(int, self, 116));
    if (F(BYTE, self, 128) & 1) {
        if (frame >= F(int, self, 112))
            frame = F(int, self, 112) - 1;
    } else
        frame %= F(int, self, 112);
    F(int, self, 108) = frame;
    ++F(DWORD, self, 124);
    F(DWORD, F(BYTE*, self, 72), 36) = F(DWORD*, self, 120)[frame];
    if (F(float, self, 104) > F(float, self, 100))
        Pooled_Set4(self, 0);
    else
        F(float, self, 104) = (float)(F(double, F(BYTE*, self, 88), 2960)
                                     + F(float, self, 104));
}
