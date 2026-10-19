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
ASM_PROC(FUN_5c3630)
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
ASM_PROC(FUN_5c3800)
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
ASM_PROC(FUN_4f1180)
ASM_PROC(FUN_556a40)
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

