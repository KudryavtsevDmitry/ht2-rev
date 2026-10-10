// scenery.cpp - the unit at 0x52abf0 of king.masm: trees, trash boxes,
// collision keys and the vehicle type classes.
//
// Building with  nmake ASM_SCENERY=1  makes king.masm assemble the original
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
extern "C" void FUN_5c5310(); KING_ALIAS("_FUN_5c5310", "@Timeline_Construct@12")
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

ASM_PROC(FUN_591c30)
ASM_PROC(FUN_5928d0)

ASM_VAR(BYTE*, g_effectManager, $L_6f6910)

// A position below the sentinel threshold resets both scripted positions.
ASM_VAR(float, g_positionResetThreshold, $L_650624)
ASM_VAR(AsmLabel, g_setPosition1, $L_67d4cc)
ASM_VAR(AsmLabel, g_setPosition2, $L_67d4c0)
ASM_VAR(AsmLabel, g_setPlace, $L_67d4b4)
ASM_PROC(FUN_58e7f0)

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

extern "C" void __fastcall Pooled_Set4(BYTE* self, void*);
