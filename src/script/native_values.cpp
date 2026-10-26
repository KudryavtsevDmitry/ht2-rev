// native_values.cpp - sch_embd.cpp of the game, rewritten from king.masm:
// script values made by the game's code: +0 the object (reference count at
// +4), +8 flags (0x10000: immediate).

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
extern "C" void FUN_5c53e0(); KING_ALIAS("_FUN_5c53e0", "@Media5c53e0_Destroy@8")
extern "C" void FUN_5c5310(); KING_ALIAS("_FUN_5c5310", "@Timeline_Construct@12")
extern "C" void FUN_5dbbb0(); KING_ALIAS("_FUN_5dbbb0", "@Fw_ResetRenderer@24")
extern "C" void FUN_5c3800(); KING_ALIAS("_FUN_5c3800", "@SoundSystem_Construct@16")
extern "C" void FUN_52e740(); KING_ALIAS("_FUN_52e740", "@Scenery_StartEvent@20")
extern "C" void FUN_5fd5c0(); KING_ALIAS("_FUN_5fd5c0", "@Sound_SetPitch@12")
extern "C" void FUN_5fd430(); KING_ALIAS("_FUN_5fd430", "@Sound_Play@20")    // sound: play (volume, pitch, a)
extern "C" void FUN_4e5140(); KING_ALIAS("_FUN_4e5140", "_Sound_PlayRange3")
extern "C" void FUN_4e51b1(); KING_ALIAS("_FUN_4e51b1", "_Sound_PlayRange")
ASM_PROC(FUN_4e4d82)
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

// ---------------------------------------------------------------------------
// the crowd (the ovation sound) of a scene object: its matrix is the object's
// moved by ten times the vector at 702630; fn is called when it plays

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // an empty function: the default callback
KING_VAR(float, g_eng702630, 702630)
KING_VAR(float, g_eng702634, 702634)
KING_VAR(float, g_eng702638, 702638)
KING_VAR(float, g_ten64c450, 64c450)          // 10.0
KING_VAR(AsmLabel, g_strOvation, 678540)

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

extern "C" void FUN_4a52e0(); KING_ALIAS("_FUN_4a52e0", "_Sch_ErrorCode")
extern "C" void FUN_4a5420(); KING_ALIAS("_FUN_4a5420", "_Sch_TypeError")    // script type error (code, level, type name)

// a typed handle copied from a script value; errors unless the value is
// borrowed (0x10000) and of the kind
static DWORD* TypedHandle_From(DWORD* self, const DWORD* v, BYTE kind, const char* name)
{
    self[0] = v[0];
    self[1] = v[1];
    self[2] = v[2];
    if (!(v[2] & 0x10000))
        CallC(FUN_4a52e0, 6, 1);
    if ((BYTE)v[2] != kind)
        CallC(FUN_4a5420, 2, 1, name);
    return self;
}

extern "C" {

// [FUN_492150]
DWORD* __fastcall BoolHandle_From(DWORD* self, void*, const DWORD* v) { return TypedHandle_From(self, v, 7, "Boolean"); }

// [FUN_4921d0]
DWORD* __fastcall CharHandle_From(DWORD* self, void*, const DWORD* v) { return TypedHandle_From(self, v, 8, "Char"); }

}

// ---------------------------------------------------------------------------
// Static initializers of random generators.

KING_VAR(AsmLabel, g_natvalRng691cd8, 691cd8)
KING_VAR(DWORD, g_natvalRng691db0, 691db0)
KING_VAR(DWORD, g_natvalRng691db4, 691db4)
KING_VAR(DWORD, g_natvalRng691db8, 691db8)

// the subtractive random generator's start (Knuth): 55 words at a, then the seed and the two indices
static void Natval_RngInit(DWORD* a, DWORD* seed, DWORD* i1, DWORD* i2)
{
    for (int i = 0; i < 55; i++)
        a[i] = 0;
    DWORD mj = 161803398, mk = 1;
    *seed = mj;
    for (unsigned i = 0; i < 1134; i += 21) {
        unsigned d = i % 55;
        mj -= mk;
        a[d] = mk;
        DWORD t = mj;
        mj = a[d];
        mk = t;
    }
    for (int k = 0; k < 4; k++)
        for (unsigned j = 0; j < 55; j++)
            a[j] -= a[(j + 31) % 55];
    *i1 = 0;
    *i2 = 31;
}

// [$L_49e760]
extern "C" void __cdecl NatvalInit_49e760() { Natval_RngInit((DWORD*)g_natvalRng691cd8, &g_natvalRng691db0, &g_natvalRng691db4, &g_natvalRng691db8); }
