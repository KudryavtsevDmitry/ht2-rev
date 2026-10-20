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

extern "C" void FUN_44c710(); KING_ALIAS("_FUN_44c710", "@StrRefVec_InsertAux@16")    // vector<12 bytes>::insert_aux
extern "C" void FUN_4a1f10(); KING_ALIAS("_FUN_4a1f10", "@IntDeque_Construct@12")
ASM_PROC(FUN_57d060)
extern "C" void FUN_52e550(); KING_ALIAS("_FUN_52e550", "@Crowd_Init@12")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
ASM_VAR(AsmLabel, g_vtbl64f198, $L_64f198)
ASM_VAR(AsmLabel, g_vtbl64f14c, $L_64f14c)
ASM_VAR(AsmLabel, g_vtbl64f148, $L_64f148)
ASM_VAR(BYTE*, g_6ef1fc, $L_6ef1fc)
extern "C" void FUN_58b5b0(); KING_ALIAS("_FUN_58b5b0", "@Body58b5b0_Construct@52")
extern "C" void FUN_58b720(); KING_ALIAS("_FUN_58b720", "@Body58b720_Construct@52")
extern "C" void FUN_52ae30(); KING_ALIAS("_FUN_52ae30", "@Slot_Construct@16")
ASM_PROC(FUN_52b0e0)
ASM_PROC(FUN_5465c0)
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
ASM_PROC(FUN_57cb70)
ASM_PROC(FUN_52dc50)
ASM_VAR(AsmLabel, g_vtbl64ec88, $L_64ec88)
ASM_VAR(AsmLabel, g_vtbl64ec3c, $L_64ec3c)
ASM_VAR(AsmLabel, g_vtbl64ec38, $L_64ec38)
ASM_VAR(AsmLabel, g_vtbl64edf0, $L_64edf0)
ASM_VAR(AsmLabel, g_vtbl64eda4, $L_64eda4)
ASM_VAR(AsmLabel, g_vtbl64eda0, $L_64eda0)
ASM_VAR(AsmLabel, g_vtbl64ef2c, $L_64ef2c)
ASM_VAR(AsmLabel, g_vtbl64eee0, $L_64eee0)
ASM_VAR(AsmLabel, g_vtbl64eedc, $L_64eedc)
ASM_VAR(AsmLabel, g_vtbl64f068, $L_64f068)
ASM_VAR(AsmLabel, g_vtbl64f01c, $L_64f01c)
ASM_VAR(AsmLabel, g_vtbl64f018, $L_64f018)
ASM_VAR(AsmLabel, g_7025f0, $L_7025f0)
extern "C" void FUN_5c53e0(); KING_ALIAS("_FUN_5c53e0", "@Media5c53e0_Destroy@8")
extern "C" void FUN_5c5310(); KING_ALIAS("_FUN_5c5310", "@Timeline_Construct@12")
ASM_PROC(FUN_5dbbb0)
extern "C" void FUN_5c3800(); KING_ALIAS("_FUN_5c3800", "@SoundSystem_Construct@16")
ASM_PROC(FUN_52e740)
extern "C" void FUN_5fd5c0(); KING_ALIAS("_FUN_5fd5c0", "@Sound_SetPitch@12")
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
extern "C" void FUN_4e5140(); KING_ALIAS("_FUN_4e5140", "_Sound_PlayRange3")
extern "C" void FUN_4e51b1(); KING_ALIAS("_FUN_4e51b1", "_Sound_PlayRange")
ASM_PROC(FUN_4e4d82)
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
extern "C" void FUN_5f25a0(); KING_ALIAS("_FUN_5f25a0", "@Node_GetLocal@12")
extern "C" int __fastcall Vehicle_MakeOut(BYTE* self, void*);
ASM_PROC(FUN_581ea0)
extern "C" void FUN_5f25c0(); KING_ALIAS("_FUN_5f25c0", "@Node_GetWorld@12")
extern "C" void FUN_5f2190(); KING_ALIAS("_FUN_5f2190", "@Node_ApplyMatrix@16")

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
ASM_PROC(FUN_55f000)    // whether a vehicle can take another (vehicle, other)
extern "C" void FUN_4f1180(); KING_ALIAS("_FUN_4f1180", "@Body_SetUpright@4")
extern "C" void FUN_556a40(); KING_ALIAS("_FUN_556a40", "@Car_ApplyFound@8")
ASM_PROC(FUN_55f5f0)
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")    // a model by name
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")    // a key of a model (parent, name)
extern "C" void FUN_52b030(); KING_ALIAS("_FUN_52b030", "@Car_SendState@12")
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

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // an empty function: the default callback
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
extern "C" void FUN_5928d0(); KING_ALIAS("_FUN_5928d0", "_Bodies_Pair")

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
ASM_PROC(FUN_57d580)

// [FUN_52e1f0] 57d580(a, b) on this
extern "C" int __fastcall Scenery_Call57d580(void* self, void*, int a, int b)
{
    return CallThis<int>(FUN_57d580, self, a, b);
}


// ---------------------------------------------------------------------------
ASM_VAR(AsmLabel, g_sn6cedac, $L_6cedac)          // use counts, 11 per car row
ASM_VAR(AsmLabel, g_sn6ced98, $L_6ced98)
ASM_VAR(AsmLabel, s_sn678480, $L_678480)

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


ASM_PROC(FUN_52bd20)
ASM_PROC(FUN_52c120)
ASM_PROC(FUN_590d20)
extern "C" void FUN_52e1f0(); KING_ALIAS("_FUN_52e1f0", "@Scenery_Call57d580@16")
ASM_PROC(FUN_586240)
ASM_PROC(FUN_5864d0)
extern "C" void FUN_585fd0(); KING_ALIAS("_FUN_585fd0", "@Cars_ApplyUpdate@12")
extern "C" void FUN_4e480c(); KING_ALIAS("_FUN_4e480c", "_Switch_SelectChecked")
ASM_PROC(FUN_5f9b00)
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
ASM_VAR(AsmLabel, g_scy6cedc0, $L_6cedc0)
ASM_VAR(AsmLabel, g_scy6d1830, $L_6d1830)

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
ASM_PROC(FUN_52e060)
ASM_VAR(AsmLabel, g_scy6cf4d8, $L_6cf4d8)
ASM_VAR(AsmLabel, g_scy6d1db8, $L_6d1db8)
ASM_VAR(AsmLabel, g_scy6cf3b0, $L_6cf3b0)
ASM_VAR(AsmLabel, g_scy702650, $L_702650)
ASM_VAR(BYTE*, g_scy6ef1fc, $L_6ef1fc)
ASM_VAR(double, g_scy64b660, $L_64b660)

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
ASM_PROC(FUN_52e740)
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
ASM_VAR(AsmLabel, g_q25_64ec88, $L_64ec88)
ASM_VAR(AsmLabel, g_q25_64ec3c, $L_64ec3c)
ASM_VAR(AsmLabel, g_q25_64ec38, $L_64ec38)

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

ASM_PROC(FUN_52d6f0)
// [$L_52d3e0]
extern "C" BYTE* __fastcall Dtor52d3e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_52d6f0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

ASM_PROC(FUN_52dec0)
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

ASM_PROC(FUN_52dec0)
ASM_VAR(AsmLabel, g_q47_64f198, $L_64f198)
ASM_VAR(AsmLabel, g_q47_64f14c, $L_64f14c)
ASM_VAR(AsmLabel, g_q47_64f148, $L_64f148)
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
ASM_VAR(void*, g_q102_6cf3a4, $L_6cf3a4)
ASM_VAR(BYTE*, g_q102_6f653c, $L_6f653c)
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
ASM_VAR(AsmLabel, g_q128_6d1cd0, $L_6d1cd0)
// [$L_52f5e0] static init: constructs the 6d1cd0 object
extern "C" BYTE* __cdecl Scy_Init6d1cd0(void) { return CallThis<BYTE*>(Scy128_4e0200, (BYTE*)g_q128_6d1cd0); }
