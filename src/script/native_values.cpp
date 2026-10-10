// native_values.cpp - sch_embd.cpp of the game, rewritten from king.masm:
// script values made by the game's code: +0 the object (reference count at
// +4), +8 flags (0x10000: immediate).
//
// Building with  nmake ASM_NATIVE_VALUES=1  makes king.masm assemble the original
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
extern "C" void FUN_5fd5c0(); KING_ALIAS("_FUN_5fd5c0", "@Sound_SetPitch@12")
ASM_PROC(FUN_5fd430)    // sound: play (volume, pitch, a)
ASM_PROC(FUN_4e5140)
ASM_PROC(FUN_4e51b1)
ASM_PROC(FUN_4e4d82)
ASM_PROC(FUN_4e480c)
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

// ---------------------------------------------------------------------------
// the crowd (the ovation sound) of a scene object: its matrix is the object's
// moved by ten times the vector at 702630; fn is called when it plays

ASM_PROC(FUN_5e48f0)    // an empty function: the default callback
ASM_VAR(float, g_eng702630, $L_702630)
ASM_VAR(float, g_eng702634, $L_702634)
ASM_VAR(float, g_eng702638, $L_702638)
ASM_VAR(float, g_ten64c450, $L_64c450)          // 10.0
ASM_VAR(AsmLabel, g_strOvation, $L_678540)

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
