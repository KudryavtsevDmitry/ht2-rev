// scenery.cpp - the unit at 0x52abf0 of king.exe: trees, trash boxes,
// collision keys and the vehicle type classes.

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

extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")    // vector<12 bytes>::insert_aux
extern "C" void FUN_4a1f10(); KING_ALIAS("_FUN_4a1f10", "@IntDeque_Construct@12")
extern "C" void FUN_57d060(); KING_ALIAS("_FUN_57d060", "@MovingObj_Construct@44")
extern "C" void FUN_52e550(); KING_ALIAS("_FUN_52e550", "@Crowd_Init@12")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
KING_VAR(AsmLabel, g_vtbl64f198, 64f198)
KING_VAR(AsmLabel, g_vtbl64f14c, 64f14c)
KING_VAR(AsmLabel, g_vtbl64f148, 64f148)
KING_VAR(BYTE*, g_6ef1fc, 6ef1fc)
extern "C" void FUN_58b5b0(); KING_ALIAS("_FUN_58b5b0", "@Body58b5b0_Construct@52")
extern "C" void FUN_58b720(); KING_ALIAS("_FUN_58b720", "@Body58b720_Construct@52")
extern "C" void FUN_52ae30(); KING_ALIAS("_FUN_52ae30", "@Slot_Construct@16")
extern "C" void FUN_52b0e0(); KING_ALIAS("_FUN_52b0e0", "@Parked_Setup@16")
extern "C" void FUN_5465c0(); KING_ALIAS("_FUN_5465c0", "_Vehicle_MakeDamageKeys")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
extern "C" void FUN_57cb70(); KING_ALIAS("_FUN_57cb70", "@MovingObj_ConstructListed@44")
extern "C" void FUN_52dc50(); KING_ALIAS("_FUN_52dc50", "@Car_SetModel@16")
KING_VAR(AsmLabel, g_vtbl64ec88, 64ec88)
KING_VAR(AsmLabel, g_vtbl64ec3c, 64ec3c)
KING_VAR(AsmLabel, g_vtbl64ec38, 64ec38)
KING_VAR(AsmLabel, g_vtbl64edf0, 64edf0)
KING_VAR(AsmLabel, g_vtbl64eda4, 64eda4)
KING_VAR(AsmLabel, g_vtbl64eda0, 64eda0)
KING_VAR(AsmLabel, g_vtbl64ef2c, 64ef2c)
KING_VAR(AsmLabel, g_vtbl64eee0, 64eee0)
KING_VAR(AsmLabel, g_vtbl64eedc, 64eedc)
KING_VAR(AsmLabel, g_vtbl64f068, 64f068)
KING_VAR(AsmLabel, g_vtbl64f01c, 64f01c)
KING_VAR(AsmLabel, g_vtbl64f018, 64f018)
KING_VAR(AsmLabel, g_7025f0, 7025f0)
extern "C" void FUN_5c53e0(); KING_ALIAS("_FUN_5c53e0", "@Media5c53e0_Destroy@8")
extern "C" void FUN_5c5310(); KING_ALIAS("_FUN_5c5310", "@Timeline_Construct@12")
extern "C" void FUN_5dbbb0(); KING_ALIAS("_FUN_5dbbb0", "@Fw_ResetRenderer@24")
extern "C" void FUN_5c3800(); KING_ALIAS("_FUN_5c3800", "@SoundSystem_Construct@16")
extern "C" void FUN_52e740(); KING_ALIAS("_FUN_52e740", "@Scenery_StartEvent@20")
extern "C" void FUN_5fd5c0(); KING_ALIAS("_FUN_5fd5c0", "@Sound_SetPitch@12")
extern "C" void FUN_5fd430(); KING_ALIAS("_FUN_5fd430", "@Sound_Play@20")    // sound: play (volume, pitch, a)
extern "C" void FUN_4e5140(); KING_ALIAS("_FUN_4e5140", "_Sound_PlayRange3")
extern "C" void FUN_4e51b1(); KING_ALIAS("_FUN_4e51b1", "_Sound_PlayRange")
ASM_FUNC(FUN_4e4d82, _Sound_PlayPannedCos)
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5f25a0(); KING_ALIAS("_FUN_5f25a0", "@Node_GetLocal@12")
extern "C" int __fastcall Vehicle_MakeOut(BYTE* self, void*);
extern "C" void FUN_581ea0(); KING_ALIAS("_FUN_581ea0", "_Points_BelowPlane")
extern "C" void FUN_5f25c0(); KING_ALIAS("_FUN_5f25c0", "@Node_GetWorld@12")
extern "C" void FUN_5f2190(); KING_ALIAS("_FUN_5f2190", "@Node_ApplyMatrix@16")

KING_VAR(int, g_6cec68, 6cec68)
KING_VAR(int, g_6968ac, 6968ac)
KING_VAR(BYTE*, g_options, 6cec78)            // +386, +388, +390: volumes in percent
typedef BYTE* Ptrs[1];
typedef int Ints[1];
KING_VAR(Ptrs, g_6ced98, 6ced98)              // 11 per row
KING_VAR(Ints, g_6cedac, 6cedac)
KING_VAR(int, g_class1, 6ce350)               // the first type of each class
KING_VAR(int, g_class2, 6ce354)
KING_VAR(int, g_class3, 6ce358)
KING_VAR(int, g_class4, 6ce35c)
KING_VAR(int, g_class5, 6ce360)
KING_VAR(int, g_class6, 6ce364)
KING_VAR(int, g_classEnd, 6ce368)
KING_VAR(BYTE*, g_vehicleList2, 6ef1f8)
KING_VAR(AsmLabel, g_6d219c, 6d219c)          // 1488-byte records by vehicle type
KING_VAR(int, g_7025b0, 7025b0)
KING_VAR(double, g_6fb3e8, 6fb3e8)
KING_VAR(int, g_6fb3d8, 6fb3d8)
KING_VAR(int, g_6fb3d0, 6fb3d0)
KING_VAR(int, g_7025c8, 7025c8)
KING_VAR(DWORD, g_67e8c8, 67e8c8)
KING_VAR(int, g_7025cc, 7025cc)
KING_VAR(AsmLabel, g_strCabCl, 672e40)        // "%s_cab_cl"
KING_VAR(AsmLabel, g_vtbl64d578, 64d578)
KING_VAR(AsmLabel, g_vtbl64d6c8, 64d6c8)
KING_VAR(AsmLabel, g_vtbl64d830, 64d830)

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

// [FUN_52e8c0] calls FUN_52e740 for the vehicle with id p[0] (NULL if there is none); the
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

extern "C" void FUN_4e2821(); KING_ALIAS("_FUN_4e2821", "_World_CheckPoint")    // (vector, name, scale as float)
extern "C" void FUN_4ee900(); KING_ALIAS("_FUN_4ee900", "@Car_CountType@8")
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_55f000(); KING_ALIAS("_FUN_55f000", "_Trailer_HitchCheck")    // whether a vehicle can take another (vehicle, other)
extern "C" void FUN_4f1180(); KING_ALIAS("_FUN_4f1180", "@Body_SetUpright@4")
extern "C" void FUN_556a40(); KING_ALIAS("_FUN_556a40", "@Car_ApplyFound@8")
extern "C" void FUN_55f5f0(); KING_ALIAS("_FUN_55f5f0", "_Truck_HitchTrailer")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")    // a model by name
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")    // a key of a model (parent, name)
extern "C" void FUN_52b030(); KING_ALIAS("_FUN_52b030", "@Car_SendState@12")
extern "C" void FUN_57de90(); KING_ALIAS("_FUN_57de90", "@SceneObj_Matrix1@12")
KING_VAR(DWORD, g_702650, 702650)
KING_VAR(DWORD, g_702654, 702654)
KING_VAR(DWORD, g_702658, 702658)
KING_VAR(BYTE*, g_6ef1f8, 6ef1f8)
KING_VAR(int, g_carModelCount, 6ce34c)
KING_VAR(AsmLabel, g_carModelNames, 6c6b68)   // 876 bytes per car
typedef int CarParts[1];
KING_VAR(CarParts, g_carPartModels, 6ced98)   // 11 ints per car: 5 models, 5 counts, a key
KING_VAR(CarParts, g_carPartUses, 6cedac)

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

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // an empty function: the default callback
KING_VAR(float, g_eng702630, 702630)
KING_VAR(float, g_eng702634, 702634)
KING_VAR(float, g_eng702638, 702638)
KING_VAR(float, g_ten64c450, 64c450)          // 10.0
KING_VAR(AsmLabel, g_strOvation, 678540)      // "ovation1Sound"

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

extern "C" void FUN_591c30(); KING_ALIAS("_FUN_591c30", "_Bodies_Overlap")
extern "C" void FUN_5928d0(); KING_ALIAS("_FUN_5928d0", "_Bodies_Pair")

KING_VAR(BYTE*, g_effectManager, 6f6910)

// A position below the sentinel threshold resets both scripted positions.
KING_VAR(float, g_positionResetThreshold, 650624)
KING_VAR(AsmLabel, g_setPosition1, 67d4cc)
KING_VAR(AsmLabel, g_setPosition2, 67d4c0)
KING_VAR(AsmLabel, g_setPlace, 67d4b4)
extern "C" void FUN_58e7f0(); KING_ALIAS("_FUN_58e7f0", "@MovingItem_CheckFrame@20")

KING_VAR(short, g_sceneCommandCode, 67d10a)
KING_VAR(BYTE*, g_sceneCommandList, 6f653c)

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

// --- vehicle constructors: the base (FUN_58b5b0 / FUN_58b720 / FUN_57cb70)
// is built from the car model table entry (876 bytes) of the model.

static BYTE* CarModel(int model)
{
    return (BYTE*)g_carModelNames + model * 876;
}

static void Car_SetVtbls(BYTE* self, void* a, void* b, void* c)
{
    F(void*, self, 0) = a;
    F(void*, self, 256) = b;
    F(void*, self, 376) = c;
}

extern "C" {

// [FUN_52b5f0] a car of the model (identity placement)
BYTE* __fastcall Car52b5f0_Construct(BYTE* self, void*, int a1, int a2, int model, int a4, int a5)
{
    BYTE* t = CarModel(model);
    CallThis(FUN_58b5b0, self, a1, 0, a2, t + 120, (void*)g_7025f0, F(int, t, 196), 100, F(int, t, 200),
             0x3f000000, t, a5);
    CallThis(FUN_52ae30, self + 9528, model, 0);
    Car_SetVtbls(self, g_vtbl64ec88, g_vtbl64ec3c, g_vtbl64ec38);
    CallThis(FUN_52b0e0, self, model, a4);
    return self;
}

// [FUN_52b6f0] the trailer variant
BYTE* __fastcall Car52b6f0_Construct(BYTE* self, void*, int a1, int a2, int model, int a4, int a5, int a6)
{
    BYTE* t = CarModel(model);
    CallThis(FUN_58b720, self, a1, 0, a2, t + 120, (void*)g_7025f0, F(int, t, 196), 100, F(int, t, 200),
             0x3f000000, a5, a6);
    CallThis(FUN_52ae30, self + 9528, model, 1);
    Car_SetVtbls(self, g_vtbl64ec88, g_vtbl64ec3c, g_vtbl64ec38);
    CallThis(FUN_52b0e0, self, model, a4);
    return self;
}

// [FUN_52b7c0] a car placed by the matrix built with [FUN_5aba40]
BYTE* __fastcall Car52b7c0_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int model, int a6, int a7)
{
    BYTE* t = CarModel(model);
    BYTE m[48];
    void* r = CallC<void*>(FUN_5aba40, m, t + 120, a4);
    CallThis(FUN_58b5b0, self, a1, a2, a3, r, a4, F(int, t, 196), 100, F(int, t, 200), 0x3f000000, t + 35, a7);
    CallThis(FUN_52ae30, self + 9528, model, 0);
    Car_SetVtbls(self, g_vtbl64ec88, g_vtbl64ec3c, g_vtbl64ec38);
    CallThis(FUN_52b0e0, self, model, a6);
    return self;
}

// [FUN_52b8a0]
BYTE* __fastcall Car52b8a0_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int model, int a6, int a7,
                                     int a8)
{
    BYTE* t = CarModel(model);
    BYTE m[48];
    void* r = CallC<void*>(FUN_5aba40, m, t + 120, a4);
    CallThis(FUN_58b720, self, a1, a2, a3, r, a4, F(int, t, 196), 100, F(int, t, 200), 0x3f000000, a7, a8);
    CallThis(FUN_52ae30, self + 9528, model, 1);
    Car_SetVtbls(self, g_vtbl64ec88, g_vtbl64ec3c, g_vtbl64ec38);
    CallThis(FUN_52b0e0, self, model, a6);
    return self;
}

static void Truck_Finish(BYTE* self)
{
    Car_SetVtbls(self, g_vtbl64edf0, g_vtbl64eda4, g_vtbl64eda0);
    CallC(FUN_5465c0, self + 9520, self + 9516);
}

// [FUN_52d350]
BYTE* __fastcall Truck52d350_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5)
{
    Car52b5f0_Construct(self, 0, a1, a2, a3, a4, a5);
    Truck_Finish(self);
    return self;
}

// [FUN_52d400]
BYTE* __fastcall Truck52d400_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6)
{
    Car52b6f0_Construct(self, 0, a1, a2, a3, a4, a5, a6);
    Truck_Finish(self);
    return self;
}

// [FUN_52d490]
BYTE* __fastcall Truck52d490_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    Car52b7c0_Construct(self, 0, a1, a2, a3, a4, a5, a6, a7);
    Truck_Finish(self);
    return self;
}

// [FUN_52d530]
BYTE* __fastcall Truck52d530_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7,
                                       int a8)
{
    Car52b8a0_Construct(self, 0, a1, a2, a3, a4, a5, a6, a7, a8);
    Truck_Finish(self);
    return self;
}

// [FUN_52d650] a car with its own placement matrix (a4) kept at +9552
BYTE* __fastcall Car52d650_Construct(BYTE* self, void*, int a1, int a2, int a3, const BYTE* a4, int a5, int a6,
                                     int a7, int a8, int a9, int a10)
{
    Car52b7c0_Construct(self, 0, a1, a2, a3, (int)a4, a5, a6, a10);
    BYTE* m = self + 9552;
    memset(m, 0, 48);
    memcpy(m, g_7025f0, 48);
    Car_SetVtbls(self, g_vtbl64ef2c, g_vtbl64eee0, g_vtbl64eedc);
    memcpy(m, a4, 48);
    F(int, self, 3568) = 3;
    return self;
}

// [FUN_52d930] a vehicle on the [FUN_57cb70] base
BYTE* __fastcall Vehicle52d930_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int model, int a6, int a7)
{
    BYTE* t = CarModel(model);
    BYTE m[48];
    void* r = CallC<void*>(FUN_5aba40, m, t + 120, a4);
    CallThis(FUN_57cb70, self, a1, a2, a3, r, a4, F(int, t, 196), 500, t, a7);
    Car_SetVtbls(self, g_vtbl64f068, g_vtbl64f01c, g_vtbl64f018);
    CallThis(FUN_52dc50, self, model, a6);
    return self;
}

}

extern "C" BYTE* __fastcall DpMessage_Construct(BYTE* self, void*);   // sound_gate.cpp

extern "C" {

// [FUN_52da30] a vehicle on the [FUN_57d060] base, placed by a matrix
BYTE* __fastcall Vehicle52da30_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int model, int a6, int a7,
                                         int a8)
{
    BYTE* t = CarModel(model);
    BYTE m[48];
    void* r = CallC<void*>(FUN_5aba40, m, t + 120, a4);
    CallThis(FUN_57d060, self, a1, a2, a3, r, a4, F(int, t, 196), 500, a7, a8);
    Car_SetVtbls(self, g_vtbl64f068, g_vtbl64f01c, g_vtbl64f018);
    CallThis(FUN_52dc50, self, model, a6);
    return self;
}

// [FUN_52daf0] a vehicle on the [FUN_57cb70] base (identity placement)
BYTE* __fastcall Vehicle52daf0_Construct(BYTE* self, void*, int a1, int a2, int model, int a4, int a5)
{
    BYTE* t = CarModel(model);
    CallThis(FUN_57cb70, self, a1, 0, a2, t + 120, (void*)g_7025f0, F(int, t, 196), 500, t, a5);
    Car_SetVtbls(self, g_vtbl64f068, g_vtbl64f01c, g_vtbl64f018);
    CallThis(FUN_52dc50, self, model, a4);
    return self;
}

// [FUN_52dba0] a vehicle on the [FUN_57d060] base (identity placement)
BYTE* __fastcall Vehicle52dba0_Construct(BYTE* self, void*, int a1, int a2, int model, int a4, int a5, int a6)
{
    BYTE* t = CarModel(model);
    CallThis(FUN_57d060, self, a1, 0, a2, t + 120, (void*)g_7025f0, F(int, t, 196), 500, a5, a6);
    Car_SetVtbls(self, g_vtbl64f068, g_vtbl64f01c, g_vtbl64f018);
    CallThis(FUN_52dc50, self, model, a4);
    return self;
}

static void Bus_Finish(BYTE* self, int v)
{
    BYTE* m = self + 3584;
    memset(m, 0, 48);
    memcpy(m, g_7025f0, 48);
    Car_SetVtbls(self, g_vtbl64f198, g_vtbl64f14c, g_vtbl64f148);
    CallThis(FUN_52e550, self, v);
}

// [FUN_52e220]
BYTE* __fastcall Bus52e220_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    Vehicle52d930_Construct(self, 0, a1, a2, a3, a4, a5, a6, a7);
    Bus_Finish(self, a8);
    return self;
}

// [FUN_52e300]
BYTE* __fastcall Bus52e300_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8,
                                     int a9)
{
    Vehicle52da30_Construct(self, 0, a1, a2, a3, a4, a5, a6, a7, a8);
    Bus_Finish(self, a9);
    return self;
}

// [FUN_52e3c0]
BYTE* __fastcall Bus52e3c0_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6)
{
    Vehicle52daf0_Construct(self, 0, a1, a2, a3, a4, a5);
    Bus_Finish(self, a6);
    return self;
}

// [FUN_52e470]
BYTE* __fastcall Bus52e470_Construct(BYTE* self, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    Vehicle52dba0_Construct(self, 0, a1, a2, a3, a4, a5, a6);
    Bus_Finish(self, a7);
    return self;
}

// [FUN_52b030] sends the car's state message (13) to the other players
void __fastcall Car_SendState(BYTE* self, void*, int a)
{
    BYTE buf[228];
    BYTE* msg = buf;
    BYTE* rec = buf + 28;
    F(int, rec, 0) = F(int, self, 12);
    F(int, rec, 8) = a;
    F(int, rec, 4) = 13;
    int size = 12;
    if (a == 1) {
        F(int, rec, 12) = F(int, self, 0);
        size = 16;
    }
    DpMessage_Construct(msg, 0);
    CallThis(FUN_603110, msg, (int)g_sceneCommandCode, rec, size);
    CallThis(FUN_5c3630, g_6ef1fc, msg);
    CallThis(FUN_602e80, msg);
}

// [FUN_52e7c0] sends message 10 about the car c with the text name
void __fastcall Car_SendNamed(BYTE* self, void*, const BYTE* c, int b, const char* name)
{
    BYTE buf[428];
    BYTE* msg = buf;
    BYTE* rec = buf + 28;
    DpMessage_Construct(msg, 0);
    F(int, rec, 0) = F(int, self, 1188);
    F(int, rec, 12) = b;
    F(int, rec, 8) = F(int, c, 9544);
    int n = (int)strlen(name) + 1;
    F(int, rec, 16) = n;
    F(int, rec, 4) = 10;
    memcpy(rec + 20, name, n);
    CallThis(FUN_603110, msg, (int)g_sceneCommandCode, rec, n + 20);
    CallThis(FUN_5c3630, g_6ef1fc, msg);
    CallThis(FUN_602e80, msg);
}

}

// ---------------------------------------------------------------------------
extern "C" void FUN_57d580(); KING_ALIAS("_FUN_57d580", "@Item_DrawMarked@16")

// [FUN_52e1f0] 57d580(a, b) on this
extern "C" int __fastcall Scenery_Call57d580(void* self, void*, int a, int b)
{
    return CallThis<int>(FUN_57d580, self, a, b);
}


// ---------------------------------------------------------------------------
KING_VAR(AsmLabel, g_sn6cedac, 6cedac)          // use counts, 11 per car row
KING_VAR(AsmLabel, g_sn6ced98, 6ced98)
KING_VAR(AsmLabel, s_sn678480, 678480)

// [FUN_52afa0] releases a use of the part model (row +4, column +0); the last use calls its method +36
extern "C" void __fastcall Part_Release(BYTE* self, void*)
{
    int i = F(int, self, 4) * 11 + F(int, self, 0);
    int* cnt = (int*)(char*)g_sn6cedac;
    cnt[i]--;
    if (!cnt[i])
    {
        BYTE* m = ((BYTE**)(char*)g_sn6ced98)[i];
        if (m)
            CallVirt(m, 36);
    }
    if (cnt[i] < 0)
    {
        char buf[200];
        sprintf(buf, s_sn678480, F(int, self, 4), cnt[i]);
    }
}


extern "C" void FUN_52bd20(); KING_ALIAS("_FUN_52bd20", "@Scenery_Hit@16")
extern "C" void FUN_52c120(); KING_ALIAS("_FUN_52c120", "@ParkedCar_DrawShadow@16")
extern "C" void FUN_590d20(); KING_ALIAS("_FUN_590d20", "@Item_DrawBody@16")
extern "C" void FUN_52e1f0(); KING_ALIAS("_FUN_52e1f0", "@Scenery_Call57d580@16")
extern "C" void FUN_586240(); KING_ALIAS("_FUN_586240", "@Box_ReceiveHit@20")
extern "C" void FUN_5864d0(); KING_ALIAS("_FUN_5864d0", "@Box_ReceiveHitSet@32")
extern "C" void FUN_585fd0(); KING_ALIAS("_FUN_585fd0", "@Cars_ApplyUpdate@12")
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5f9b00(); KING_ALIAS("_FUN_5f9b00", "@SceneNode5f9b00_Hit@16")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
KING_VAR(AsmLabel, g_scy6cedc0, 6cedc0)
KING_VAR(AsmLabel, g_scy6d1830, 6d1830)

// [$L_52b6c0]
extern "C" void __fastcall Scy_Set1204(BYTE* s, void*, int v) { F(int, s, 1204) = v; }
// [$L_52d5d0] forwarders
extern "C" int __fastcall Scy_52d5d0(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_52bd20, s, a, b); }
// [$L_52d630]
extern "C" int __fastcall Scy_52d630(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_52c120, s, a, b); }
// [$L_52d700]
extern "C" int __fastcall Scy_52d700(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_590d20, s, a, b); }
// [$L_52e710]
extern "C" int __fastcall Scy_52e710(BYTE* s, void*, int a, int b) { return CallThis<int>(FUN_52e1f0, s, a, b); }
// [$L_52d8e0]
extern "C" int __fastcall Scy_52d8e0(BYTE* s, void*, int a, int b, int c) { return CallThis<int>(FUN_586240, s, a, b, c); }
// [$L_52d900]
extern "C" int __fastcall Scy_52d900(BYTE* s, void*, int a, int b, int c, int d, int e, int f)
{
    return CallThis<int>(FUN_5864d0, s, a, b, c, d, e, f);
}

// [$L_52e6f0] FUN_585fd0 (a) unless +3632 is set
extern "C" int __fastcall Scy_52e6f0(BYTE* s, void*, int a)
{
    int v = F(int, s, 3632);
    if (v)
        return v;
    return CallThis<int>(FUN_585fd0, s, a);
}

// [$L_52d5f0] the kind's entry (44-byte records at 6cedc0, index +9288) to FUN_4e480c, then FUN_5f9b00 (a, b)
extern "C" int __fastcall Scy_52d5f0(BYTE* s, void*, int a, int b)
{
    CallC(FUN_4e480c, ((int*)g_scy6cedc0)[F(int, s, 9288) * 11], 1);
    return CallThis<int>(FUN_5f9b00, s, a, b);
}

// [$L_52f6e0] static initializer: FUN_5ac0c0 (6d1830, -pi/2)
extern "C" void __cdecl ScyInit_52f6e0() { CallC(FUN_5ac0c0, (BYTE*)g_scy6d1830, 0xBFC90FDA); }

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_52fd90(); KING_ALIAS("_FUN_52fd90", "@Obj52fd90_Reset@8")
extern "C" void FUN_52e060(); KING_ALIAS("_FUN_52e060", "@Scenery_Run@4")
KING_VAR(AsmLabel, g_scy6cf4d8, 6cf4d8)
KING_VAR(AsmLabel, g_scy6d1db8, 6d1db8)
KING_VAR(AsmLabel, g_scy6cf3b0, 6cf3b0)
KING_VAR(AsmLabel, g_scy702650, 702650)
KING_VAR(BYTE*, g_scy6ef1fc, 6ef1fc)
KING_VAR(double, g_scy64b660, 64b660)

extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")

// [$L_52f6a0] static initializer: the 13 records of 696 bytes at 6cf4d8 and the one at 6d1db8
extern "C" void __cdecl ScyInit_52f6a0()
{
    BYTE* p = (BYTE*)g_scy6cf4d8;
    for (int i = 0; i < 13; i++, p += 696)
        CallThis(FUN_52fd90, p);
    CallThis(FUN_52fd90, (BYTE*)g_scy6d1db8);
}

// [$L_52f600] static initializer: 6cf3b0..6cf4b7 zero
extern "C" void __cdecl ScyInit_52f600()
{
    DWORD* d = (DWORD*)g_scy6cf3b0;
    for (int i = 0; i < 66; i++)
        d[i] = 0;
}

// [$L_52e630] state 1 (+3632) places the object (+3580, +3584) at rest; the light (+3640) goes out after 64b660
extern "C" void __fastcall Scy_52e630(BYTE* s)
{
    if (F(int, s, 3632) == 1) {
        F(int, s, 3632) = 2;
        CallVirt(s, 140, F(int, s, 3580), s + 3584);
        CallVirt(s, 144, (BYTE*)g_scy702650, 0);
        CallVirt(s, 148, (BYTE*)g_scy702650, 0);
    }
    int c = F(int, s, 3640);
    if (c && F(double, g_scy6ef1fc, 2968) - F(float, s, 3644) > g_scy64b660) {
        CallC(FUN_5e1a10, c, 0);
        F(int, s, 3640) = 0;
    }
    CallThis(FUN_52e060, s);
    if (F(int, s, 896) && F(int, s, 3632) == 2)
        F(int, s, 3632) = 0;
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_52e7c0(); KING_ALIAS("_FUN_52e7c0", "@Car_SendNamed@20")
extern "C" void FUN_52e740(); KING_ALIAS("_FUN_52e740", "@Scenery_StartEvent@20")
extern "C" void FUN_52e540(); KING_ALIAS("_FUN_52e540", "@Obj52e540_Set3576@12")
extern "C" void FUN_57c9e0(); KING_ALIAS("_FUN_57c9e0", "_Moving_Of")
extern "C" void FUN_53f7b0(); KING_ALIAS("_FUN_53f7b0", "_Car_53f7b0")

// [$L_52e970] a message for the object: a named place (kinds 2, 2 with a name) moves it there; a moving body is attached
extern "C" void __fastcall Scy_Message(BYTE* s, void*, BYTE* p)
{
    if (F(int, s, 372))
        return;
    BYTE* m = F(BYTE*, p, 100);
    if (!F(int, p, 116) && m && F(int, m, 0) == 2 && F(int, m, 4) == 2 && F(int, m, 8) && F(BYTE*, m, 12)) {
        int x = *F(int*, m, 12);
        if (F(int, s, 3640))
            CallC(FUN_5e1a10, F(int, s, 3640), 0);
        int e = F(int, s, 3576);
        if (e) {
            char buf[101];
            const char* src = F(char*, F(BYTE*, p, 100), 12) + 4;
            int n = 0;
            while (src[n] && n < 100) {
                buf[n] = src[n];
                n++;
            }
            buf[n] = 0;
            CallThis(FUN_52e7c0, s, e, x, buf);
            CallThis(FUN_52e740, s, F(int, s, 3576), x, buf);
            F(int, s, 3632) = 1;
        }
        CallThis(FUN_52e540, s, 0);
    }
    int t = F(int, p, 116);
    if (t) {
        int r = CallC<int>(FUN_57c9e0, t);
        if (r)
            CallThis(FUN_52e540, s, CallC<int>(FUN_53f7b0, r));
    }
}

// ---------------------------------------------------------------------------
// Scenery object destructor (exception frame dropped).

extern "C" void FUN_52ea90(); KING_ALIAS("_FUN_52ea90", "_CarType_Number")
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5e52b0(); KING_ALIAS("_FUN_5e52b0", "@Resources_SetData@24")
extern "C" void FUN_57def0(); KING_ALIAS("_FUN_57def0", "@SceneObj_Get800@8")
extern "C" void FUN_58b870(); KING_ALIAS("_FUN_58b870", "@Body58b870_Destroy@8")
#include <string.h>
KING_VAR(AsmLabel, g_q25_64ec88, 64ec88)
KING_VAR(AsmLabel, g_q25_64ec3c, 64ec3c)
KING_VAR(AsmLabel, g_q25_64ec38, 64ec38)

// [FUN_52bb40] destructor: a scenery object (vtables 64ec88, 64ec3c at +256, 64ec38 at +376); if the world keeps
// its record (key cell +9544 / slot +9548) the 156-byte state is saved there; its part is released
extern "C" void __fastcall Scenery_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_q25_64ec88;
    F(void*, s, 256) = g_q25_64ec3c;
    F(void*, s, 376) = g_q25_64ec38;
    int key = F(int, s, 9548) + (CallC<int>(FUN_52ea90, F(int, s, 9544)) + 200) * 1000;
    if (CallThis<int>(FUN_5e5370, F(BYTE*, s, 36) + 704, key)) {
        const char* n1 = F(const char*, F(BYTE*, s, 320), 12);
        const char* n2 = F(const char*, CallThis<BYTE*>(FUN_57def0, s), 12);
        BYTE t[156];
        memset(t, 0, sizeof t);
        F(int, t, 0) = CallC<int>(FUN_52ea90, F(int, s, 9544));
        F(int, t, 4) = F(int, s, 9548);
        F(int, t, 8) = F(int, s, 372);
        F(int, t, 12) = F(int, s, 1200);
        F(int, t, 16) = 0;
        memcpy(t + 20, s + 416, 24);
        memcpy(t + 44, s + 500, 48);
        strncpy((char*)t + 92, n2, 31);
        strncpy((char*)t + 124, n1, 31);
        CallThis(FUN_5e52b0, F(BYTE*, s, 36) + 704, key, F(int, F(BYTE*, s, 292), 60), 156, t);
    }
    Part_Release(s ? s + 9528 : 0, 0);
    CallThis(FUN_58b870, s);
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_52bb40(); KING_ALIAS("_FUN_52bb40", "@Scenery_Destroy@4")
// [$L_52b6d0]
extern "C" BYTE* __fastcall Dtor52b6d0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_52bb40, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_52d6f0(); KING_ALIAS("_FUN_52d6f0", "@Thunk52d6f0@4")
// [$L_52d3e0]
extern "C" BYTE* __fastcall Dtor52d3e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_52d6f0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_52dec0(); KING_ALIAS("_FUN_52dec0", "@ParkedCar_Destroy@4")
// [$L_52da10]
extern "C" BYTE* __fastcall Dtor52da10_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_52dec0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_52e520(); KING_ALIAS("_FUN_52e520", "@Scy52e520_Destruct@8")
// [$L_52e2e0]
extern "C" BYTE* __fastcall Dtor52e2e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_52e520, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_52dec0(); KING_ALIAS("_FUN_52dec0", "@ParkedCar_Destroy@4")
KING_VAR(AsmLabel, g_q47_64f198, 64f198)
KING_VAR(AsmLabel, g_q47_64f14c, 64f14c)
KING_VAR(AsmLabel, g_q47_64f148, 64f148)
// [FUN_52e520] destructor: the vtables 64f198, 64f14c at +256, 64f148 at +376, then the base
extern "C" void __fastcall Scy52e520_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q47_64f198;
    F(void*, s, 256) = g_q47_64f14c;
    F(void*, s, 376) = g_q47_64f148;
    CallThis(FUN_52dec0, s);
}

// ---------------------------------------------------------------------------
// A scenery record constructor.

// [$L_52fce0] constructor: ten 12-byte entries (+592), 590 zero bytes, 47 zero words at +712, then the flags
extern "C" BYTE* __fastcall Scy52fce0_Construct(BYTE* s, void*)
{
    for (int k = 0; k < 10; k++) {
        F(float, s, 592 + k * 12) = 0.5f;
        F(float, s, 596 + k * 12) = 1.0f;
        F(int, s, 600 + k * 12) = 0;
    }
    for (int k = 0; k < 590; k++)
        s[k] = 0;
    for (int k = 0; k < 47; k++)
        F(int, s, 712 + k * 4) = 0;
    for (int k = 0; k < 10; k++) {
        F(int, s, 592 + k * 12) = 0;
        F(int, s, 600 + k * 12) = 0;
    }
    static const int ones[] = {712, 876, 716, 720, 724, 728, 796, 808, 812, 816, 820};
    for (int k = 0; k < 11; k++)
        F(int, s, ones[k]) = 1;
    return s;
}

// ---------------------------------------------------------------------------
// Messages to cars.

extern "C" void FUN_52ea90(); KING_ALIAS("_FUN_52ea90", "_CarType_Number")
KING_VAR(void*, g_q102_6cf3a4, 6cf3a4)
KING_VAR(BYTE*, g_q102_6f653c, 6f653c)
// [$L_52ee70] delivers message m (id +4, sender +28) unless the 6cf3a4 hook takes it: a message of the 2xxxxx
// range goes to the first active car of the 6f653c list (as a driven car: +3568 = 1, or one with +392 = 1) whose
// object or car id (type number + 200, times 1000, plus its number) matches; that car's vtbl+0 gets 1
extern "C" int __cdecl Cars_Deliver(BYTE* m, int b)
{
    if (g_q102_6cf3a4)
        if (int r = ((int(__cdecl*)(BYTE*, int))g_q102_6cf3a4)(m, b))
            return r;
    if (F(int, m, 4) / 100000 != 2 || !g_q102_6f653c)
        return 0;
    for (BYTE* n = g_q102_6f653c - 4; n;) {
        BYTE* c = n - 376;
        if (F(int, n, 16) == 2 && F(int, c, 3568) == 1) {
            if (F(BYTE*, m, 28) == c ||
                (CallC<int>(FUN_52ea90, F(int, c, 9544)) + 200) * 1000 + F(int, c, 9548) == F(int, m, 4)) {
                CallVirt(c, 0, 1);
                return 1;
            }
        }
        if (F(int, c, 392) == 1) {
            if (F(BYTE*, m, 28) == c ||
                (CallC<int>(FUN_52ea90, F(int, c, 3568)) + 200) * 1000 + F(int, c, 3572) == F(int, m, 4)) {
                CallVirt(c, 0, 1);
                return 1;
            }
        }
        BYTE* next = F(BYTE*, n, 8);
        if (!next)
            return 0;
        n = next - 4;
    }
    return 0;
}

extern "C" void Scy128_4e0200(); KING_ALIAS("_Scy128_4e0200", "@Sta4e0200_Construct@8")
KING_VAR(AsmLabel, g_q128_6d1cd0, 6d1cd0)
// [$L_52f5e0] static init: constructs the 6d1cd0 object
extern "C" BYTE* __cdecl Scy_Init6d1cd0(void) { return CallThis<BYTE*>(Scy128_4e0200, (BYTE*)g_q128_6d1cd0); }

// [$L_52d9f0] copies matrix b to a; a
extern "C" BYTE* __stdcall Matrix52d9f0_Copy(BYTE* a, const BYTE* b, int, int, int)
{
    memcpy(a, b, 48);
    return a;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor52b6d0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor52b6d0_DeletingDestructor@12", "@Dtor52b6d0_DeletingDestructor@12")
extern "C" int __fastcall q198_MovingItem_Fly(BYTE* s); KING_ALIAS("@q198_MovingItem_Fly@4", "@MovingItem_Fly@4")
// [$L_52e210] forwards to MovingItem_Fly
extern "C" int __fastcall Thunk52e210(BYTE* s) { return q198_MovingItem_Fly(s); }
// [$L_52f4b0] forwards to Dtor52b6d0_DeletingDestructor for the object 256 bytes before
extern "C" int __fastcall Thunk52f4b0(BYTE* s, void* d, int a0) { return q198_Dtor52b6d0_DeletingDestructor(s - 256, d, a0); }

extern "C" void FUN_52f9d0(); KING_ALIAS("_FUN_52f9d0", "_Sound_Loop386")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
KING_VAR(AsmLabel, g_q215_702620, 702620)
// [FUN_52e740] starts scene s's event for object o (its handler +3648 with t, unless o is busy at +388): the loop
// sound +3636, the switch object named name selected (+3640), and the clock 6ef1fc's time kept (+3644)
extern "C" void __fastcall Scenery_StartEvent(BYTE* s, void*, BYTE* o, float t, const char* name)
{
    if (o && !F(int, o, 388))
        ((void(__cdecl*)(BYTE*, double))F(void*, s, 3648))(o, t);
    CallC(FUN_52f9d0, F(int, s, 3636), 1.0f, 1.0f, (void*)g_q215_702620);
    BYTE* n = CallC<BYTE*>(FUN_5ef2b0, 0, name);
    F(BYTE*, s, 3640) = n;
    CallC(FUN_5e1a10, n, 1);
    F(float, s, 3644) = (float)F(double, g_6ef1fc, 2968);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Scenery_Destroy(BYTE* s); KING_ALIAS("@q238_Scenery_Destroy@4", "@Scenery_Destroy@4")
extern "C" int __fastcall q238_Thunk52e210(BYTE* s); KING_ALIAS("@q238_Thunk52e210@4", "@Thunk52e210@4")
// [FUN_52d6f0] forwards to Scenery_Destroy
extern "C" int __fastcall Thunk52d6f0(BYTE* s) { return q238_Scenery_Destroy(s); }
// [$L_52e730] forwards to Thunk52e210
extern "C" int __fastcall Thunk52e730(BYTE* s) { return q238_Thunk52e210(s); }

// === the default control preset (FUN_52fdb0)

KING_VAR(int, g_dp696890, 696890)
KING_VAR(AsmLabel, g_dp6d1948, 6d1948)    // the key states, 2 bytes each
KING_VAR(AsmLabel, s_dp678814, 678814)    // "CLAVIER..."
KING_VAR(AsmLabel, s_dp678800, 678800)
KING_VAR(AsmLabel, s_dp6787f0, 6787f0)

extern "C" {

// [FUN_52fdb0] constructor of control preset p: named by the input device (696890: 3, 1, else), no device
// (+260 = -1), the action slots (+264, +452) cleared and the default keys bound, the axes (+640..+692) reset
void __fastcall ControlPreset_Default(BYTE* p)
{
    static const unsigned short k_keys[][2] = {
        {264, 0x4c},
        {268, 0x4a},
        {272, 0x4e},
        {276, 0x50},
        {280, 0x22},
        {284, 0x82},
        {288, 0xb4},
        {292, 0xb0},
        {296, 0x62},
        {300, 0x64},
        {304, 0x66},
        {308, 0x68},
        {312, 0x6a},
        {316, 0x6c},
        {320, 0x6e},
        {324, 0x70},
        {328, 0x72},
        {332, 0x60},
        {344, 0xac},
        {348, 0x90},
        {352, 0x84},
        {356, 0x98},
        {360, 0x48},
        {364, 0x46},
        {368, 0x5c},
        {372, 0x44},
        {376, 0x9a},
        {380, 0xee},
        {384, 0xf0},
        {388, 0xe8},
        {392, 0xea},
        {396, 0xd6},
        {400, 0xda},
        {404, 0x12},
        {408, 0xe2},
        {412, 0xe4},
        {416, 0xae},
        {420, 0xe0},
        {424, 0xa8},
        {428, 0x40},
        {432, 0x10},
        {436, 0xa0},
        {440, 0xe6}
};
    strcpy((char*)p, g_dp696890 == 3 ? s_dp678814 : g_dp696890 == 1 ? s_dp678800 : s_dp6787f0);
    F(int, p, 260) = -1;
    for (int i = 0; i < 47; i++) {
        F(int, p, 264 + i * 4) = 0;
        F(int, p, 452 + i * 4) = 0;
    }
    for (const auto& k : k_keys)
        F(BYTE*, p, k[0]) = (BYTE*)g_dp6d1948 + k[1];
    for (int i = 0; i < 3; i++) {
        F(int, p, 640 + i * 4) = 0;
        F(int, p, 652 + i * 4) = 0;
    }
    F(int, p, 688) = 0;
    F(int, p, 676) = 0;
    F(int, p, 680) = 0;
    F(int, p, 692) = 0;
    F(int, p, 684) = 4;
    F(int, p, 664) = 1;
    F(int, p, 668) = 1;
    F(int, p, 672) = 1;
    memset(p + 452, 0, 188);
}

} // extern "C"

// === placing a car of a model (FUN_52dc50)

extern "C" void FUN_4e497a(); KING_ALIAS("_FUN_4e497a", "_Frame_GetLocalChecked")
extern "C" void FUN_57e030(); KING_ALIAS("_FUN_57e030", "@SceneObj_57e030@12")
extern "C" void FUN_57e070(); KING_ALIAS("_FUN_57e070", "@Vehicle_MakeOut@8")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")

extern "C" {

// [FUN_52dc50] makes car c model i (6c6b68, 876 bytes) of kind 1 with b: its mass (+192), its centre (+180)
// moved into the car's frame (+692) at +3436, its size (+168) at +404 and +3288, the model values +196, +200,
// the scene root, the body matrix (+740 by the frame of +324) and its inverse (+500, +596), zero speeds
void __fastcall Car_SetModel(BYTE* c, void*, int i, int b)
{
    F(int, c, 3568) = i;
    F(int, c, 392) = 1;
    F(int, c, 3572) = b;
    BYTE* r = (BYTE*)g_carModelNames + i * 876;
    F(DWORD, c, 396) = F(DWORD, r, 192);
    float rx = F(float, r, 180), ry = F(float, r, 184), rz = F(float, r, 188);
    float* p = (float*)(c + 3436);
    p[0] = (float)((double)F(float, c, 716) * rz + (double)F(float, c, 704) * ry + (double)rx * F(float, c, 692) +
                   F(float, c, 728));
    p[1] = (float)((double)F(float, c, 720) * rz + (double)F(float, c, 696) * rx + (double)F(float, c, 708) * ry +
                   F(float, c, 732));
    p[2] = (float)((double)F(float, c, 724) * rz + (double)F(float, c, 700) * rx + (double)F(float, c, 712) * ry +
                   F(float, c, 736));
    F(DWORD, c, 400) = F(DWORD, r, 196);
    memcpy(c + 404, r + 168, 12);
    memcpy(c + 3288, r + 168, 12);
    F(DWORD, c, 1424) = F(DWORD, r, 200);
    F(DWORD, c, 1428) = F(DWORD, r, 200);
    F(int, c, 800) = CallThis<int>(FUN_5ef640, c + 256);
    BYTE m[48], t[48];
    memcpy(m, g_7025f0, 48);
    CallC(FUN_4e497a, F(BYTE*, c, 324), m);
    memcpy(c + 500, CallC<BYTE*>(FUN_5aba40, t, c + 740, m), 48);
    memcpy(c + 596, CallC<BYTE*>(FUN_5ac2e0, t, c + 500), 48);
    for (int k = 0; k < 3; k++)
        memcpy(c + 416 + k * 12, &g_702650, 12);
    CallVirt(c, 52);
    CallThis(FUN_57e030, c, c + 3268);
    CallThis(FUN_57e070, c);
}

} // extern "C"

// === a parked car's destructor (FUN_52dec0)

extern "C" void Pc_ItemDestroy(); KING_ALIAS("_Pc_ItemDestroy", "@MovingItem_Destroy@4")

// [FUN_52dec0] destructor: a parked car (vtables 64f068, 64f01c at +256, 64f018 at +376) saves its 156-byte state
// in the world's record (key cell +3568 / slot +3572), then the moving item is destroyed
extern "C" void __fastcall ParkedCar_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_vtbl64f068;
    F(void*, s, 256) = g_vtbl64f01c;
    F(void*, s, 376) = g_vtbl64f018;
    const char* n1 = F(const char*, F(BYTE*, s, 320), 12);
    const char* n2 = F(const char*, CallThis<BYTE*>(FUN_57def0, s), 12);
    BYTE t[156];
    memset(t, 0, sizeof t);
    F(int, t, 0) = CallC<int>(FUN_52ea90, F(int, s, 3568));
    F(int, t, 4) = F(int, s, 3572);
    memcpy(t + 20, s + 416, 24);
    memcpy(t + 44, s + 500, 48);
    strncpy((char*)t + 92, n2, 31);
    strncpy((char*)t + 124, n1, 31);
    F(int, t, 8) = F(int, s, 372);
    F(int, t, 12) = F(int, s, 1200);
    F(int, t, 16) = 0;
    int key = F(int, s, 3572) + (CallC<int>(FUN_52ea90, F(int, s, 3568)) + 200) * 1000;
    CallThis(FUN_5e52b0, F(BYTE*, s, 36) + 704, key, F(int, F(BYTE*, s, 292), 60), 156, t);
    CallThis(Pc_ItemDestroy, s);
}

// === a hit denting a scenery object (FUN_52bd20)

extern "C" void Dn_Debris(); KING_ALIAS("_Dn_Debris", "_Fx_DebrisBurst")
extern "C" void Dn_Length(); KING_ALIAS("_Dn_Length", "_Vec3_Length")
extern "C" void Dn_Normalized(); KING_ALIAS("_Dn_Normalized", "_Vec3_Normalized")
extern "C" void Dn_MulAssign(); KING_ALIAS("_Dn_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Dn_Advance(); KING_ALIAS("_Dn_Advance", "@Progresses_Advance@24")
KING_VAR(float, g_dn64c40c, 64c40c)
KING_VAR(float, g_dn64c450, 64c450)
KING_VAR(float, g_dn64ed88, 64ed88)
KING_VAR(double, g_dn64ed80, 64ed80)
KING_VAR(double, g_dn64b670, 64b670)
KING_VAR(float, g_dn64d4e8, 64d4e8)
KING_VAR(float, g_dn64c320, 64c320)
KING_VAR(float, g_dn64c250, 64c250)
KING_VAR(float, g_dn64c264, 64c264)
KING_VAR(double, g_dn64ed78, 64ed78)
KING_VAR(double, g_dn64b5b8, 64b5b8)

extern "C" {

// [FUN_52bd20] object s (whole, with damage stages +9516 / +9520) hit at p with impulse v: a hit stronger than
// 64c40c (over the mass +396), not from below, advances the damage stages 1..3 by how much it comes from the
// side (stage 2 also by the corner) and throws debris (+976) from the dent, more when one finished
void __fastcall Scenery_Hit(BYTE* s, void*, const float* p, const float* v)
{
    if (F(int, s, 372) || !F(int, s, 9520) || !F(int, s, 9516))
        return;
    float vc[3] = {v[0], v[1], v[2]};
    double L = CallC<double>(Dn_Length, vc) / F(float, s, 396);
    if (L < g_dn64c40c)
        return;
    L = L - g_dn64c40c;
    double y = (L - g_dn64c450) * g_dn64ed88;
    double g = g_dn64b670 - x87_exp(-(y * y)) * g_dn64ed80;
    float strength = (float)(g * (L * g_dn64d4e8 / (L + g_dn64c320) + g_dn64c250) * L);
    float w[3], nb[3];
    w[0] = (float)((double)F(float, s, 608) * p[1] + (double)F(float, s, 620) * p[2] + (double)F(float, s, 596) * p[0] +
        F(float, s, 632));
    w[1] = (float)((double)F(float, s, 600) * p[0] + (double)F(float, s, 612) * p[1] + (double)F(float, s, 624) * p[2] +
        F(float, s, 636));
    w[2] = (float)((double)F(float, s, 604) * p[0] + (double)F(float, s, 616) * p[1] + (double)F(float, s, 628) * p[2] +
        F(float, s, 640));
    const float* nr = CallC<const float*>(Dn_Normalized, nb, w);
    float n[3] = {nr[0], nr[1], nr[2]};
    if (x87_fabs(w[1]) < w[2] && x87_fabs(w[0]) < w[2])
        return;
    int cnt = 0, fin = 0;
    if (n[1] > g_dn64c264) {
        float k = n[1] * n[1] * strength;
        for (int i = 1; i <= 3; i++)
            CallThis(Dn_Advance, s, i, *(DWORD*)&k, &cnt, &fin);
    }
    double q = ((double)n[0] + n[1]) * g_dn64ed78;
    if (q > g_dn64c264) {
        float k = (float)(q * q * strength);
        CallThis(Dn_Advance, s, 2, *(DWORD*)&k, &cnt, &fin);
    }
    if (!cnt)
        return;
    const float* c = (const float*)(s + 428);
    double X = (double)c[1] * w[2] - (double)c[2] * w[1];
    double Y = (double)c[2] * w[0] - (double)c[0] * w[2];
    double Z = (double)c[0] * w[1] - (double)c[1] * w[0];
    float pos[3], t[3];
    pos[0] = (float)((float)(Z * F(float, s, 524) + Y * F(float, s, 512) + X * F(float, s, 500)) + F(float, s, 416));
    pos[1] = (float)((float)(Z * F(float, s, 528) + Y * F(float, s, 516) + X * F(float, s, 504)) + F(float, s, 420));
    pos[2] = (float)(Y * F(float, s, 520) + Z * F(float, s, 532) + X * F(float, s, 508) + F(float, s, 424));
    int num = cnt < 2 ? 4 : (cnt >= 3 ? 10 : 7);
    if (fin)
        num += num;
    DWORD speed = 0x3E19999A;
    float size = (float)CallC<double>(Dn_Length, pos);
    if (fin) {
        speed = 0x3E666667;
        size = (float)((x87_sqrt((double)fin) + g_dn64b670) * size);
    }
    CallThis(Dn_MulAssign, pos, t, 0x3F000000u);
    double lim = size * g_dn64b5b8;
    if (pos[2] < lim)
        pos[2] = (float)lim;
    CallC(Dn_Debris, p, s + 976, pos, *(DWORD*)&size, num, speed, 1);
}

} // extern "C"

// === setting up a parked car from its model (FUN_52b0e0)

extern "C" void Pk_FlagsSet(); KING_ALIAS("_Pk_FlagsSet", "@Flags1196_Set@12")
extern "C" void Pk_FlagsClear(); KING_ALIAS("_Pk_FlagsClear", "@Flags1196_Clear@12")
extern "C" void Pk_MoveMode(); KING_ALIAS("_Pk_MoveMode", "@Body_SetMoveMode@12")
extern "C" void Pk_PointsAdd(); KING_ALIAS("_Pk_PointsAdd", "@Points_Add@20")
extern "C" void Pk_Plane(); KING_ALIAS("_Pk_Plane", "@Plane_SetWithPoint@12")
extern "C" void Pk_Records(); KING_ALIAS("_Pk_Records", "@Records_58d290@8")
extern "C" void Pk_NodeRoot(); KING_ALIAS("_Pk_NodeRoot", "@Node_Root@8")
extern "C" void Pk_LocalChecked(); KING_ALIAS("_Pk_LocalChecked", "_Frame_GetLocalChecked")
extern "C" void Pk_ProbeGround(); KING_ALIAS("_Pk_ProbeGround", "@Body_ProbeGround@12")
extern "C" void Pk_57e030(); KING_ALIAS("_Pk_57e030", "@SceneObj_57e030@12")
extern "C" void Pk_MakeOut(); KING_ALIAS("_Pk_MakeOut", "@Vehicle_MakeOut@8")
extern "C" void FUN_58d2b0(); KING_ALIAS("_FUN_58d2b0", "@Body_UpdatePlanes@8")

extern "C" {

// [FUN_52b0e0] sets parked car s up as model idx (the 876-byte records of 6c6b68) in mode m: flags and move
// mode, its center, body points of both kinds, planes, frames from its node, and a probe of the ground
void __fastcall Parked_Setup(BYTE* s, void*, int idx, int m)
{
    F(int, s, 9548) = m;
    F(int, s, 9540) = F(int, s, 1188);
    F(int, s, 3568) = 1;
    CallThis(Pk_FlagsSet, s, 2);
    CallThis(Pk_FlagsClear, s, 1);
    CallThis(Pk_MoveMode, s, 3);
    F(int, s, 9544) = idx;
    BYTE* T = (BYTE*)g_carModelNames + idx * 876;
    const float* R = (const float*)(s + 692);
    F(int, s, 396) = F(int, T, 192);
    {
        const float* t = (const float*)(T + 180);
        float* o = (float*)(s + 3436);
        o[0] = (float)((double)R[3] * t[1] + (double)R[6] * t[2] + (double)t[0] * R[0] + R[9]);
        o[1] = (float)((double)R[4] * t[1] + (double)R[1] * t[0] + (double)R[7] * t[2] + R[10]);
        o[2] = (float)((double)R[5] * t[1] + (double)R[2] * t[0] + (double)R[8] * t[2] + R[11]);
    }
    F(int, s, 400) = F(int, T, 196);
    F(Vec3, s, 404) = F(Vec3, T, 168);
    F(Vec3, s, 3288) = F(Vec3, T, 168);
    float pt[3];
    for (int i = 0; i < F(int, (BYTE*)g_carModelNames + F(int, s, 9544) * 876, 480); i++)
    {
        const float* q = (const float*)(T + 204 + i * 12);
        double y = (double)q[1] * R[4] + (double)R[1] * q[0] + (double)R[7] * q[2] + R[10];
        pt[2] = (float)((double)q[1] * R[5] + (double)R[8] * q[2] + (double)R[2] * q[0] + R[11]);
        pt[0] = (float)((double)q[1] * R[3] + (double)R[0] * q[0] + (double)q[2] * R[6] + R[9]);
        pt[1] = (float)y;
        CallThis(Pk_PointsAdd, s, 0, pt, 0);
    }
    for (int i = 0; i < F(int, (BYTE*)g_carModelNames + F(int, s, 9544) * 876, 484); i++)
    {
        const float* q = (const float*)((BYTE*)g_carModelNames + F(int, s, 9544) * 876 + 488 + i * 12);
        double y = (double)R[4] * q[1] + (double)q[0] * R[1] + (double)R[7] * q[2] + R[10];
        pt[2] = (float)((double)q[1] * R[5] + (double)R[8] * q[2] + (double)q[0] * R[2] + R[11]);
        pt[0] = (float)((double)q[2] * R[6] + (double)R[0] * q[0] + (double)R[3] * q[1] + R[9]);
        pt[1] = (float)y;
        CallThis(Pk_PointsAdd, s, 1, pt, 0);
    }
    int np = 0;
    for (; np < F(int, (BYTE*)g_carModelNames + F(int, s, 9544) * 876, 856); np++)
        CallThis(Pk_Plane, s + 4548 + np * 28, (BYTE*)g_carModelNames + F(int, s, 9544) * 876 + 536 + np * 16);
    CallThis(Pk_Records, s);
    F(int, s, 5132) = np;
    CallThis(FUN_58d2b0, s);
    F(int, s, 3588) = 0;
    F(BYTE*, s, 800) = CallThis<BYTE*>(Pk_NodeRoot, s + 256);
    Matrix local = g_identity, mt;
    CallC(Pk_LocalChecked, F(BYTE*, s, 324), &local);
    memcpy(s + 500, Matrix_Multiply(&mt, (const Matrix*)(s + 740), &local), 48);
    memcpy(s + 596, Matrix_Inverse(&mt, (const Matrix*)(s + 500)), 48);
    CallThis(Pk_ProbeGround, s, 1);
    F(Vec3, s, 416) = g_zeroVec;
    F(Vec3, s, 428) = g_zeroVec;
    F(Vec3, s, 440) = g_zeroVec;
    CallVirt(s, 52);
    CallThis(Pk_57e030, s, s + 3268);
    CallThis(Pk_MakeOut, s);
}

} // extern "C"

// === a parked car's draw and shadow (FUN_52c120)

extern "C" void Ps_PointsNext(); KING_ALIAS("_Ps_PointsNext", "@Points_Next@12")
extern "C" void Ps_ZeroThis(); KING_ALIAS("_Ps_ZeroThis", "@Vec3_ZeroThis@8")
extern "C" void Ps_NodeInit(); KING_ALIAS("_Ps_NodeInit", "@Node_Init@8")
extern "C" void Ps_HitObject(); KING_ALIAS("_Ps_HitObject", "_Query_HitObject")
extern "C" void Ps_Query(); KING_ALIAS("_Ps_Query", "_Query_4e6578")
extern "C" void Ps_Shadow(); KING_ALIAS("_Ps_Shadow", "_Fx_Shadow")
KING_VAR(float, g_ps6969f4, 6969f4)               // the camera
KING_VAR(float, g_ps6969f8, 6969f8)
KING_VAR(float, g_ps6969fc, 6969fc)
KING_VAR(float, g_ps64d7b8, 64d7b8)
KING_VAR(double, g_ps64b7a8, 64b7a8)
KING_VAR(double, g_ps64ed98, 64ed98)
KING_VAR(double, g_ps64b6e0, 64b6e0)
KING_VAR(double, g_ps64ed90, 64ed90)

static Vec3 Ps_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// a corner of the lying-flat box: (xv, yv) across, the middle height turned in as (xm, ym, zm)
static Vec3 Ps_CornerXY(const float* M, double xv, double yv, float xm, float ym, double zm)
{
    return Ps_Set((double)M[3] * yv + (double)M[0] * xv + M[9] + xm, (double)M[4] * yv + (double)M[1] * xv + M[10] + ym,
                  (double)M[5] * yv + (double)M[2] * xv + M[11] + zm);
}

// a corner of the box on its side: (xv, zv) with the middle depth turned in
static Vec3 Ps_CornerXZ(const float* M, double xv, double zv, float xm, float ym, double zm)
{
    return Ps_Set((double)M[6] * zv + (double)M[0] * xv + M[9] + xm, (double)M[7] * zv + (double)M[1] * xv + M[10] + ym,
                  (double)M[8] * zv + (double)M[2] * xv + M[11] + zm);
}

// the same with the middle depth mid summed as the other code does
static Vec3 Ps_CornerXZm(const float* M, double xv, double zv, float mid)
{
    return Ps_Set((double)M[6] * zv + (double)M[0] * xv + (double)M[3] * mid + M[9],
                  (double)M[1] * xv + (double)M[4] * mid + (double)M[7] * zv + M[10],
                  (double)M[2] * xv + (double)M[5] * mid + (double)M[8] * zv + M[11]);
}

static Vec3 Ps_Through(const float* M, float x, float y, float z)
{
    Vec3 p, r;
    p.x = x;
    p.y = y;
    p.z = z;
    return *Vec3_MulMatrix(&r, &p, (const Matrix*)M);
}

extern "C" {

// [FUN_52c120] parked car s drawn (Item_DrawBody a, b) and, near the camera, its shadow: the model's (+9288,
// 6c6b68 + 876 i) box corners turned by its frame (+436 times +244) for the side it lies on (+276, +260, +256), the
// ground under it from its contact points (+4888 + 208 i records of its owner s - 256) or a ray down; the draw's result
void* __fastcall ParkedCar_DrawShadow(BYTE* s, void*, int a, int b)
{
    void* r = CallThis<void*>(FUN_590d20, s, a, b);
    float dx = (float)((double)g_ps6969f4 - F(float, s, 280));
    float dy = (float)((double)g_ps6969f8 - F(float, s, 284));
    double dz = (double)g_ps6969fc - F(float, s, 288);
    if (!((double)dx * dx + (double)dy * dy + dz * dz <= g_ps64d7b8))
        return r;
    BYTE nodes[20][208];
    for (int k = 0; k < 20; k++)
    {
        CallThis(Ps_NodeInit, nodes[k]);
        F(Vec3, nodes[k], 52) = g_zeroVec;
        F(Vec3, nodes[k], 64) = g_axisZ;
        F(int, nodes[k], 108) = 0;
        F(int, nodes[k], 100) = 0;
    }
    Vec3 C[4];
    for (int k = 0; k < 4; k++)
        C[k] = g_zeroVec;
    int count = 0, mark = 0;
    Matrix Mm;
    Matrix_Multiply(&Mm, (const Matrix*)(s + 436), (const Matrix*)(s + 244));
    const float* M = Mm.m;
    BYTE* R = (BYTE*)g_carModelNames + F(int, s, 9288) * 876;
    float R456 = F(float, R, 456), R460 = F(float, R, 460), R464 = F(float, R, 464);
    float R468 = F(float, R, 468), R472 = F(float, R, 472), R476 = F(float, R, 476);
    int kind = F(int, R, 872);
    if (F(float, s, 276) > g_ps64b7a8 || F(float, s, 276) < g_ps64ed98)
    {
        double mid = ((double)R464 + R476) * g_ps64b6e0;
        float xm = (float)((double)M[6] * mid), ym = (float)((double)M[7] * mid);
        double zm = (double)M[8] * mid;
        Vec3 c0 = Ps_CornerXY(M, R468, R460, xm, ym, zm), c1 = Ps_CornerXY(M, R456, R460, xm, ym, zm);
        Vec3 c2 = Ps_CornerXY(M, R468, R472, xm, ym, zm), c3 = Ps_CornerXY(M, R456, R472, xm, ym, zm);
        if (F(float, s, 276) > g_ps64b7a8)
        {
            C[0] = c0;
            C[1] = c1;
            C[2] = c2;
            C[3] = c3;
        }
        else
        {
            C[3] = c0;
            C[2] = c1;
            C[1] = c2;
            C[0] = c3;
        }
        if (kind == 3)
            mark = 1;
    }
    else if (F(float, s, 260) < g_ps64ed98)
    {
        double mid = ((double)R472 + R460) * g_ps64b6e0;
        float xm = (float)((double)M[3] * mid), ym = (float)((double)M[4] * mid);
        double zm = (double)M[5] * mid;
        C[0] = Ps_CornerXZ(M, R456, R476, xm, ym, zm);
        C[1] = Ps_CornerXZ(M, R468, R476, xm, ym, zm);
        C[2] = Ps_CornerXZ(M, R456, R464, xm, ym, zm);
        C[3] = Ps_CornerXZ(M, R468, R464, xm, ym, zm);
        if (kind == 2)
            mark = 2;
    }
    else if (F(float, s, 260) > g_ps64b7a8)
    {
        float mid = (float)(((double)R472 + R460) * g_ps64b6e0);
        C[3] = Ps_CornerXZm(M, R468, R476, mid);
        C[2] = Ps_CornerXZm(M, R456, R476, mid);
        C[1] = Ps_CornerXZm(M, R468, R464, mid);
        C[0] = Ps_CornerXZm(M, R456, R464, mid);
        if (kind == 2)
            mark = 2;
    }
    else if (F(float, s, 256) > g_ps64b7a8)
    {
        float mid = (float)(((double)R456 + R468) * g_ps64b6e0);
        C[0] = Ps_Set((double)M[6] * R464 + (double)M[3] * R460 + (double)M[0] * mid + M[9],
                      (double)M[1] * mid + (double)M[7] * R464 + (double)M[4] * R460 + M[10],
                      (double)M[2] * mid + (double)M[8] * R464 + (double)M[5] * R460 + M[11]);
        C[1] = Ps_Through(M, mid, R460, R476);
        C[2] = Ps_Through(M, mid, R472, R464);
        C[3] = Ps_Through(M, mid, R472, R476);
        if (kind == 1)
            mark = 1;
    }
    else
    {
        float mid = (float)(((double)R456 + R468) * g_ps64b6e0);
        C[3] = Ps_Through(M, mid, R460, R464);
        C[2] = Ps_Through(M, mid, R460, R476);
        C[1] = Ps_Through(M, mid, R472, R464);
        C[0] = Ps_Through(M, mid, R472, R476);
        if (kind == 1)
            mark = 1;
    }
    // the ground: the owner's contacts near the points, or a ray down
    Vec3Bits pt;
    CallThis(Ps_ZeroThis, &pt);
    BYTE* owner = s - 256;
    int i = CallThis<int>(Ps_PointsNext, owner, &pt);
    bool rayDown = true;
    if (i)
    {
        while (i)
        {
            Vec3 w, d;
            Vec3_MulMatrix(&w, (const Vec3*)&pt, (const Matrix*)(s + 244));
            BYTE* rec = s + i * 208;
            Vec3_Sub(&d, &F(Vec3, rec, 4940), &w);
            if (!F(int, rec, 5004) && Vec3_Dot(&d, &d) < g_ps64ed90 && count < 20)
            {
                memcpy(nodes[count], rec + 4888, 208);
                count++;
            }
            i = CallThis<int>(Ps_PointsNext, owner, &pt);
        }
        rayDown = count == 0;
    }
    if (rayDown)
    {
        Vec3 a1, b1, base, c1, up, c2, down;
        Vec3_Scaled(&a1, &g_axisZ, F(float, s, 480));
        Vec3_Sub(&b1, &F(Vec3, s, 472), &a1);
        Vec3_MulMatrix(&base, &b1, (const Matrix*)(s + 244));
        Vec3_Scaled(&c1, &g_axisZ, 10.0f);
        Vec3_Add(&up, &base, &c1);
        Vec3_Scaled(&c2, &g_axisZ, 10.0f);
        Vec3_Sub(&down, &base, &c2);
        BYTE nd[208];
        CallThis(Ps_NodeInit, nd);
        BYTE* hit = CallC<BYTE*>(Ps_HitObject, F(BYTE*, s, 544), s + 280, &up);
        int q = CallC<int>(Ps_Query, owner, hit, &up, &down, nd);
        if ((q & 1) && !F(int, nd, 116))
        {
            memcpy(nodes[0], nd, 208);
            count = 1;
        }
    }
    CallC(Ps_Shadow, nodes, count, C, F(int, R, 452), mark ? 1 : 0);
    return r;
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 32: the run steps of falling trees, trash boxes and items

#include "x87.h"
extern "C" void Sr32_CheckFrame(); KING_ALIAS("_Sr32_CheckFrame", "@MovingItem_CheckFrame@20")
extern "C" void Sr32_ItemFrame(); KING_ALIAS("_Sr32_ItemFrame", "@MovingItem_Frame@4")
extern "C" void Sr32_UpdateLying(); KING_ALIAS("_Sr32_UpdateLying", "@Body_UpdateLying@4")
extern "C" void Sr32_GroundStep(); KING_ALIAS("_Sr32_GroundStep", "@Body_GroundStep@12")
extern "C" void Sr32_MulComponents(); KING_ALIAS("_Sr32_MulComponents", "_Vec3_MulComponents")
extern "C" void Sr32_FrameTime(); KING_ALIAS("_Sr32_FrameTime", "_Game_FrameTime")
extern "C" void Sr32_Rest(); KING_ALIAS("_Sr32_Rest", "@Item_Rest@4")
KING_VAR(double, sr32_damp64b780, 64b780)
KING_VAR(char, sr32_ftree1, 678530)
KING_VAR(char, sr32_ftree2, 678520)
KING_VAR(char, sr32_ftree3, 678510)
KING_VAR(char, sr32_trash1, 6784fc)
KING_VAR(char, sr32_trash2, 6784e8)
KING_VAR(char, sr32_trash3, 6784d4)

// the shared run step: when moving (+892) the velocity (+428) decays by 0.95 per 1/64b780 s and the turn (+440)
// is the spin (+428 x +404 componentwise) through the body matrix (+500); then the state flags and vtable +80
// with the frame time, +896 set while moving; names are the profile marks (0 for the plain step)
static void Sr32_Run(BYTE* s, const char* const* names)
{
    BYTE* m = s + 500;
    if (names)
        CallThis(Sr32_CheckFrame, s, m, names[0], 0);
    if (F(int, s, 372)) {
        CallThis(Sr32_ItemFrame, s);
        return;
    }
    CallVirt(s, 52);
    if (names) {
        CallThis(Sr32_UpdateLying, s);
        CallThis(Sr32_GroundStep, s, 0);
    } else {
        CallVirt(s, 56);
    }
    if (F(int, s, 892)) {
        double k = x87_pow(0.95, CallC<double>(Sr32_FrameTime) * sr32_damp64b780);
        float* v = (float*)(s + 428);
        float vx = (float)(k * v[0]);
        float vy = (float)(k * v[1]);
        float vz = (float)(k * v[2]);
        v[0] = vx;
        v[1] = vy;
        v[2] = vz;
        float tmp[3];
        const float* r = CallC<float*>(Sr32_MulComponents, tmp, v, s + 404);
        const float* M = (const float*)m;
        float* w = (float*)(s + 440);
        float wx = (float)(((double)M[3] * r[1] + (double)M[6] * r[2]) + (double)r[0] * M[0]);
        float wy = (float)(((double)M[4] * r[1] + (double)M[7] * r[2]) + (double)M[1] * r[0]);
        float wz = (float)(((double)M[5] * r[1] + (double)M[8] * r[2]) + (double)M[2] * r[0]);
        w[0] = wx;
        w[1] = wy;
        w[2] = wz;
    }
    if (names)
        F(int, s, 3588) = 0;
    F(DWORD, s, 896) = F(DWORD, s, 892);
    F(BYTE, s, 892) &= 0xfa;
    float ft = (float)CallC<double>(Sr32_FrameTime);
    CallVirt(s, 80, ft, 0);
    int moving = F(int, s, 896) || F(int, s, 892) ? 1 : 0;
    F(DWORD, s, 900) = F(DWORD, s, 892);
    F(int, s, 896) = moving;
    if (names)
        CallThis(Sr32_CheckFrame, s, m, names[1], 0);
    CallThis(Sr32_Rest, s);
    if (names)
        CallThis(Sr32_CheckFrame, s, m, names[2], 0);
}

extern "C" {

// [$L_52d720..$L_52d79c] _FTree::run, a falling tree's step
void __fastcall FTree_Run(BYTE* s)
{
    const char* names[3] = {&sr32_ftree1, &sr32_ftree2, &sr32_ftree3};
    Sr32_Run(s, names);
}

// [$L_52b980..$L_52b9fc] _TrashBox::run, a trash box's step
void __fastcall TrashBox_Run(BYTE* s)
{
    const char* names[3] = {&sr32_trash1, &sr32_trash2, &sr32_trash3};
    Sr32_Run(s, names);
}

// [FUN_52e060..$L_52e0bc] the plain run step of a scenery item (vtable +56 instead of the lying and ground steps)
void __fastcall Scenery_Run(BYTE* s)
{
    Sr32_Run(s, 0);
}

}
