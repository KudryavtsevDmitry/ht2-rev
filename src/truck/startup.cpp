// startup.cpp - the unit at 0x4dd9f0 of king.masm (with CKingApp, see
// king_app.cpp): startup paths, truck.ini, cab names, sounds at the volumes
// of the options.
//
// Building with  nmake ASM_STARTUP=1  makes king.masm assemble the original
// procedures again.

#include "car.h"
#include "vecmath.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
extern "C" Matrix* __fastcall SceneObj_Matrix1(BYTE* self, void*, Matrix* out);  // moving_items.cpp
extern "C" Matrix* __fastcall SceneObj_Matrix2(BYTE* self, void*, Matrix* out);  // moving_items.cpp

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

// ---------------------------------------------------------------------------
// medium-sized functions

ASM_PROC(FUN_4e2821)    // (vector, name, scale as float)
ASM_PROC(FUN_4ee900)
ASM_PROC(FUN_4e480c)
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
extern "C" void FUN_5928d0(); KING_ALIAS("_FUN_5928d0", "_Bodies_Pair")

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
