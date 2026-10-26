// moving_items.cpp - the unit at 0x57d580 of king.masm: scene objects that
// move and take hits (_MovingItem, _MovingBox); +1440 selects the second
// set of transforms; the force feedback switch.
//
// Building with  nmake ASM_MOVING_ITEMS=1  makes king.masm assemble the original
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

// ---------------------------------------------------------------------------
// the crowd (the ovation sound) of a scene object: its matrix is the object's
// moved by ten times the vector at 702630; fn is called when it plays

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // an empty function: the default callback
KING_VAR(float, g_eng702630, 702630)
KING_VAR(float, g_eng702634, 702634)
KING_VAR(float, g_eng702638, 702638)
KING_VAR(float, g_ten64c450, 64c450)          // 10.0
KING_VAR(AsmLabel, g_strOvation, 678540)

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

extern "C" void FUN_591c30(); KING_ALIAS("_FUN_591c30", "_Bodies_Overlap")
extern "C" void FUN_5928d0(); KING_ALIAS("_FUN_5928d0", "_Bodies_Pair")

KING_VAR(BYTE*, g_effectManager, 6f6910)

// A position below the sentinel threshold resets both scripted positions.
KING_VAR(float, g_positionResetThreshold, 650624)
KING_VAR(AsmLabel, g_setPosition1, 67d4cc)
KING_VAR(AsmLabel, g_setPosition2, 67d4c0)
KING_VAR(AsmLabel, g_setPlace, 67d4b4)
extern "C" void FUN_58e7f0(); KING_ALIAS("_FUN_58e7f0", "@MovingItem_CheckFrame@20")

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

extern "C" void __fastcall Pooled_Set4(BYTE* self, void*);

// ---------------------------------------------------------------------------
extern "C" void FUN_5e19b0(); KING_ALIAS("_FUN_5e19b0", "_Frame_Apply")
KING_VAR(AsmLabel, s_67d928, 67d928)
extern "C" void __cdecl World_Fatal(const char* msg);

// [FUN_586c30..$L_586c5e] a record "name\0" + 48-byte matrix: applies the matrix to the scene object of that name
extern "C" void __stdcall SceneObj_PlaceByName(const char* rec)
{
    int len = (int)strlen(rec);
    if (len >= 100)
        World_Fatal(s_67d928);
    DWORD m[12];
    char name[100];
    strcpy(name, rec);
    memcpy(m, rec + len + 1, 48);
    BYTE* o = CallC<BYTE*>(FUN_5ef2b0, 0, name);
    if (o)
        CallC(FUN_5e19b0, o, 0, m);
}


// ---------------------------------------------------------------------------
// [FUN_581c80] keeps the flags (+892) at +896 and clears bits 0 and 2; returns 1
extern "C" int __fastcall Item_ResetFlags(BYTE* self, void*, int, int)
{
    DWORD f = F(DWORD, self, 892);
    F(int, self, 3464) = 0;
    F(DWORD, self, 896) = f;
    F(DWORD, self, 892) = f & ~5u;
    return 1;
}


// ---------------------------------------------------------------------------
KING_VAR(Vec3, g_mi702650, 702650)

// [FUN_581ba0] asks method +84 for a value of a (into b), applies it (+92), then resets: method +140 (+800, +500) when +372, else +88 (b); clears +476 and +488
extern "C" int __fastcall Item_Apply84(BYTE* self, void*, int a, int b)
{
    int r = 0;
    if (CallVirt<int>(self, 84, a, &b))
        r = CallVirt<int>(self, 92, b);
    if (F(int, self, 372))
        CallVirt(self, 140, F(int, self, 800), self + 500);
    else
        CallVirt(self, 88, b);
    F(Vec3, self, 476) = g_mi702650;
    F(Vec3, self, 488) = g_mi702650;
    return r;
}


extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")

// [$L_586180] released once (+1200)
extern "C" void __fastcall Movite_586180(BYTE* s)
{
    if (F(int, s, 1200))
        return;
    F(int, s, 1200) = 1;
    CallThis(FUN_5d32f0, s, 1);
}

// [$L_584030] counts a step (+1432); 1 while within +1680 + +1684
extern "C" int __fastcall Movite_Step(BYTE* s)
{
    int n = ++F(int, s, 1432);
    return n <= F(int, s, 1684) + F(int, s, 1680);
}

// [$L_5866f0] o's vtbl+192 (self, b) unless o's +372 is set
extern "C" void __fastcall Movite_5866f0(BYTE* s, void*, BYTE* o, int b)
{
    if (!F(int, o, 372))
        CallVirt(o, 192, s, b);
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_5843c0(); KING_ALIAS("_FUN_5843c0", "_Item_MoveFraction")
extern "C" void FUN_5855a0(); KING_ALIAS("_FUN_5855a0", "_Item_BodyContact")

// [$L_585d30] by the kind (+392): kind 1 with flag 4 to FUN_5843c0, kind 1 or kind 2 with flag 2 to FUN_5855a0
extern "C" int __fastcall Movite_585d30(BYTE* s, void*, int a, int b, int c, int d, int e)
{
    int k = F(int, s, 392);
    if (k == 1 && (e & 4))
        return CallC<int>(FUN_5843c0, s, a, b, c, d);
    if (k == 1 || (k == 2 && (e & 2)))
        return CallC<int>(FUN_5855a0, s, a, b, c, d, e);
    return 0;
}

// ---------------------------------------------------------------------------
// More virtual methods.

// [$L_584320] runs the item's steps (vtbl+96 start, +100.. per step) until the last; the state goes back
extern "C" int __fastcall Movite_RunSteps(BYTE* s, void*, int a)
{
    int st = 0;
    if (CallVirt<int>(s, 96, a, &st)) {
        int r;
        do {
            CallVirt(s, 100, &st);
            CallVirt(s, 104, 0x3f800000);
            CallVirt(s, 108, a);
            if (!F(int, s, 1672)) {
                r = CallVirt<int>(s, 112, &st);
            } else {
                CallVirt(s, 120, a, F(int, s, 1456));
                CallVirt(s, 124, a);
                r = CallVirt<int>(s, 128);
            }
        } while (r);
        CallVirt(s, 132, &st);
    }
    F(int, s, 1440) = 0;
    return st;
}

// ---------------------------------------------------------------------------
// Network messages and small constructors (exception frames dropped).

extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
KING_VAR(short, g_movite67d10a, 67d10a)
KING_VAR(BYTE*, g_movite6ef1fc, 6ef1fc)

// sends a network message with the record d of n bytes (packet built on the stack, then sent by 6ef1fc)
static void Movite_Send(const void* d, int n)
{
    BYTE pkt[32];
    CallThis(FUN_602be0, pkt);
    CallThis(FUN_603110, pkt, (int)g_movite67d10a, d, n);
    CallThis(FUN_5c3630, g_movite6ef1fc, pkt);
    CallThis(FUN_602e80, pkt);
}
// [$L_5860e0] message 3 for the item (+1188)
extern "C" void __fastcall Movite_Send3(BYTE* s)
{
    int d[2] = {F(int, s, 1188), 3};
    Movite_Send(d, 8);
}
// [$L_5861a0] message 4 unless +372 is set
extern "C" void __fastcall Movite_Send4(BYTE* s)
{
    if (F(int, s, 372))
        return;
    int d[2] = {F(int, s, 1188), 4};
    Movite_Send(d, 8);
}
// [$L_586720] message 2 (a, the item, b)
extern "C" void __fastcall Movite_Send2(BYTE* s, void*, int a, int b)
{
    int d[4] = {2, a, F(int, s, 1188), b};
    Movite_Send(d, 16);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

#include <string.h>
// [FUN_586b40] message 12 (the item, the name and a matrix)
extern "C" void __fastcall Movite_SendNamed(BYTE* s, void*, const char* name, const DWORD* mat)
{
    BYTE d[320];
    F(int, d, 0) = F(int, s, 1188);
    F(int, d, 4) = 12;
    size_t n = strlen(name);
    memcpy(d + 8, name, n + 1);
    memcpy(d + n + 9, mat, 48);
    Movite_Send(d, (int)n + 57);
}

// ---------------------------------------------------------------------------
// Small methods: message filters, record copies, frees and deleting destructors.

extern "C" void FUN_4e260c(); KING_ALIAS("_FUN_4e260c", "_World_Fatal")
KING_VAR(AsmLabel, s_q50_67d604, 67d604)
// [$L_582920] *out = whether +1432 is set; fatal when +1672 is set
extern "C" int __fastcall Item582920_HasLink(BYTE* s, void*, int* out)
{
    if (F(int, s, 1672))
        CallC(FUN_4e260c, (const char*)s_q50_67d604);
    *out = F(int, s, 1432) != 0;
    return 0;
}
// [$L_587e00] *out = the value of a 32-typed record
extern "C" void __cdecl Rec_GetValue32(int* r, int* out)
{
    if (r && out && r[0] == 32)
        *out = r[1];
}

// ---------------------------------------------------------------------------
// Scene object messages.

extern "C" void FUN_586c30(); KING_ALIAS("_FUN_586c30", "_SceneObj_PlaceByName@4")
KING_VAR(AsmLabel, s_q57_678550, 678550)
// [FUN_586cd0] handles the message m for the object (its id at +1188, fatal otherwise): type 1 slot 176 with the
// data (+8), 3 slot 216, 4 slot 212, 12 placement by name; 1 when handled
extern "C" int __fastcall SceneObj_OnMessage(BYTE* s, void*, int* m)
{
    if (m[0] != F(int, s, 1188))
        CallC(FUN_4e260c, (const char*)s_q57_678550);
    switch (m[1]) {
    case 1: CallVirt(s, 176, m + 2); return 1;
    case 3: CallVirt(s, 216); return 1;
    case 4: CallVirt(s, 212); return 1;
    case 12: CallThis(FUN_586c30, s, m + 2); return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// A received car update.

extern "C" void FUN_5ef5c0(); KING_ALIAS("_FUN_5ef5c0", "@Node_Init@8")
KING_VAR(BYTE*, g_q106_6f653c, 6f653c)
// [FUN_585fd0] applies update record r (car id +0, a 208-byte node +4, +212, a matrix +216, two vectors +264 and
// +276) to the car of the 6f653c list with that id (+812 of its list entry) through vtbl+168 of s
extern "C" void __fastcall Cars_ApplyUpdate(BYTE* s, void*, const BYTE* r)
{
    BYTE node[208];
    CallThis(FUN_5ef5c0, node);
    int id = F(int, r, 0);
    int a = F(int, r, 212);
    memcpy(node, r + 4, 208);
    BYTE m[48];
    memcpy(m, r + 216, 48);
    float v1[3], v2[3];
    memcpy(v1, r + 264, 12);
    memcpy(v2, r + 276, 12);
    if (!g_q106_6f653c)
        return;
    for (BYTE* e = g_q106_6f653c - 4; e;) {
        if (F(int, e, 812) == id) {
            CallVirt(s, 168, node, e - 376, a, m, v1, v2);
            return;
        }
        BYTE* next = F(BYTE*, e, 8);
        if (!next)
            return;
        e = next - 4;
    }
}

// ---------------------------------------------------------------------------
// The 64-slot track of moving items.

extern "C" void __stdcall Array_ForEach(BYTE* p, int size, int count, void (*fn)());    // ai_world.cpp
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
KING_VAR(AsmLabel, g_q108_7025f0, 7025f0)
KING_VAR(AsmLabel, g_q108_702650, 702650)
// [FUN_587c80] constructor: 64 slots, each with two default positions (+0, +768), cleared values (+1536, +1792,
// +2048, +2304, +2560, +2820, +3076) and an identity matrix (+3332); the slot counter (+2816) ends at 0
extern "C" BYTE* __fastcall Track587c80_Construct(BYTE* s)
{
    memset(s, 0, 1536);
    for (int i = 0; i < 64; i++) {
        Array_ForEach(s + 3332 + i * 48, 12, 4, FUN_4bd4d0);
        memcpy(s + 3332 + i * 48, (BYTE*)g_q108_7025f0, 48);
    }
    for (int i = 0; i < 64; i++) {
        F(int, s, 2816) = i;
        memcpy(s + i * 12, (BYTE*)g_q108_702650, 12);
        memcpy(s + (i + 64) * 12, (BYTE*)g_q108_702650, 12);
        F(int, s, 1536 + i * 4) = 0;
        F(int, s, 1792 + i * 4) = 0;
        F(int, s, 2048 + i * 4) = 0;
        F(int, s, 2304 + i * 4) = 0;
        F(int, s, 2560 + i * 4) = 0;
        F(int, s, 2820 + i * 4) = 0;
        memcpy(s + 3332 + i * 48, (BYTE*)g_q108_7025f0, 48);
        F(int, s, 3076 + i * 4) = 0;
    }
    F(int, s, 2816) = 0;
    return s;
}

// [$L_582460] starts a step of moving item s: *out cleared, +1688 = whether +1432 <= +1680, +1676 cleared, the
// previous matrices (+1792 -> +1888, +1840 -> +1936) and +2032 -> +2036 kept, the point +3448 through matrix
// +1696 into +1996 and the matrix's position into +2008
extern "C" void __fastcall MovingItem_StepStart(BYTE* s, void*, int* out)
{
    *out = 0;
    F(int, s, 1688) = F(int, s, 1432) <= F(int, s, 1680);
    F(int, s, 1676) = 0;
    memcpy(s + 1888, s + 1792, 48);
    F(int, s, 2036) = F(int, s, 2032);
    memcpy(s + 1936, s + 1840, 48);
    const float* m = (const float*)(s + 1696);
    float px = F(float, s, 3448), py = F(float, s, 3452), pz = F(float, s, 3456);
    double x = m[6] * pz + m[3] * py;
    x = x + px * m[0];
    x = x + m[9];
    double y = m[7] * pz + m[1] * px;
    y = y + m[4] * py;
    y = y + m[10];
    double z = m[8] * pz + m[2] * px;
    z = z + m[5] * py;
    z = z + m[11];
    F(float, s, 1996) = (float)x;
    F(float, s, 2000) = (float)y;
    F(float, s, 2004) = (float)z;
    memcpy(s + 2008, s + 1732, 12);
}

extern "C" void FUN_5ab620(); KING_ALIAS("_FUN_5ab620", "@Vec3_MulAssign@16")
// [$L_583c30] ends a step of moving item s (unless +1432 > +1680 or flags bit 2): its velocity (+1984) is the
// move from +2020 to +416 times +396 (a tenth of it with +1672 bit 25), applied with +1516 through vtbl+164,
// then vtbl+116
extern "C" void __fastcall MovingItem_StepEnd(BYTE* s, void*, int flags)
{
    if (F(int, s, 1432) > F(int, s, 1680) || (flags & 4))
        return;
    double dx = F(float, s, 416) - F(float, s, 2020);
    float dy = (float)(F(float, s, 420) - F(float, s, 2024));
    float k = F(float, s, 396);
    float dz = (float)(F(float, s, 424) - F(float, s, 2028));
    float v[3] = {(float)(dx * k), (float)(dy * k), (float)(dz * k)};
    memcpy(s + 1984, v, 12);
    float w[3], tmp[3];
    memcpy(w, s + 1516, 12);
    if (F(DWORD, s, 1672) & 0x2000000)
        CallThis(FUN_5ab620, v, tmp, 0.1f);
    CallVirt(s, 164, w, v);
    CallVirt(s, 116);
}

extern "C" void FUN_4e2749(); KING_ALIAS("_FUN_4e2749", "_Game_FrameTime")
extern "C" void FUN_4e2821(); KING_ALIAS("_FUN_4e2821", "_World_CheckPoint")
KING_VAR(AsmLabel, s_q173_67d494, 67d494)
// [FUN_580ce0] moves item s by its velocity (+788..+796) over the frame time, keeps the frame's move (+3148) and
// spin (+3160, from +428) and checks the new position against the world
extern "C" void __fastcall MovingItem_Fly(BYTE* s)
{
    double dt = CallC<double>(FUN_4e2749);
    double a = dt * F(float, s, 788);
    float b = (float)(dt * F(float, s, 792));
    float c = (float)(dt * F(float, s, 796));
    float* p = (float*)(s + 416);
    p[0] = (float)(a + p[0]);
    p[1] = (float)(b + p[1]);
    p[2] = (float)(c + p[2]);
    for (int k = 0; k < 3; k++)
        F(float, s, 3148 + k * 4) = (float)(dt * p[k]);
    for (int k = 0; k < 3; k++)
        F(float, s, 3160 + k * 4) = (float)(dt * F(float, s, 428 + k * 4));
    CallC(FUN_4e2821, p, (const char*)s_q173_67d494, 150.0f);
}

extern "C" void FUN_4e8ae1(); KING_ALIAS("_FUN_4e8ae1", "_Body_FixedPointImpulse")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
KING_VAR(AsmLabel, s_q174_67d434, 67d434)
KING_VAR(AsmLabel, s_q174_67d420, 67d420)
// [FUN_5808c0] the contact forces of item s with a2/a3 (4e8ae1 with its matrix +3172 and its inverse, position,
// spin, +3288, +3268 and +1424) applied as a push (vtbl+144) and a turn (vtbl+148)
extern "C" void __cdecl MovingItem_Contact(BYTE* s, int a2, int a3)
{
    BYTE m[48], inv[48];
    memcpy(m, s + 3172, 48);
    CallC(FUN_5ac2e0, inv, m);
    float pos[3], spin[3], w[3];
    float f[3] = {0, 0, 0}, t[3] = {0, 0, 0};
    memcpy(pos, s + 416, 12);
    memcpy(spin, s + 428, 12);
    memcpy(w, s + 3288, 12);
    CallC(FUN_4e8ae1, m, inv, pos, spin, w, a2, a3, F(float, s, 3268), F(float, s, 1424), 0, f, t);
    CallVirt(s, 144, f, (const char*)s_q174_67d434);
    CallVirt(s, 148, t, (const char*)s_q174_67d420);
}

KING_VAR(double, g_q186_64b5a0, 64b5a0)
KING_VAR(AsmLabel, s_q186_67d5f0, 67d5f0)
// [$L_5827c0] places moving item s for a new step: with contacts (+1432) a point closer than +3284 to the plane
// (+1528 normal, +1516 point) is pushed out along the normal (+1732 from +2008); the contact list is cleared, the
// position checked against the world and the new contacts found (vtbl+68, count +1672)
extern "C" void __fastcall MovingItem_Place(BYTE* s, void*, int a)
{
    if (F(int, s, 1432) > 0) {
        double dx = F(float, s, 1996) - F(float, s, 1516);
        double dy = F(float, s, 2000) - F(float, s, 1520);
        double dz = F(float, s, 2004) - F(float, s, 1524);
        double d = dz * F(float, s, 1536) + dy * F(float, s, 1532);
        d = d + dx * F(float, s, 1528);
        if (F(float, s, 3284) + g_q186_64b5a0 > d) {
            double k = F(float, s, 3284) - d + g_q186_64b5a0;
            float a0 = (float)(k * F(float, s, 1528)), a1 = (float)(k * F(float, s, 1532));
            double a2 = k * F(float, s, 1536);
            F(float, s, 1732) = (float)(a0 + F(float, s, 2008));
            F(float, s, 1736) = (float)(a1 + F(float, s, 2012));
            F(float, s, 1740) = (float)(a2 + F(float, s, 2016));
        }
    }
    F(int, s, 1692) = 0;
    F(int, s, 1208) = 0;
    F(float, s, 1456) = 1.0f;
    memset(s + 1212, 0, 200);
    CallC(FUN_4e2821, s + 1732, (const char*)s_q186_67d5f0, 15000.0f);
    F(int, s, 1672) = CallVirt<int>(s, 68, s + 1696, s + 1456, s + 1460, s + 1464, a);
}

extern "C" void FUN_4e3132(); KING_ALIAS("_FUN_4e3132", "_Vec3_DivComponents")
KING_VAR(AsmLabel, s_q187_67d594, 67d594)
KING_VAR(AsmLabel, s_q187_66c4e4, 66c4e4)
KING_VAR(AsmLabel, s_q187_68d1c0, 68d1c0)
// [FUN_5816d0] sets the spin of moving item s to v (+440; its local form over the inertia +404 into +428, checked
// against the world under name), and keeps both (+3312, +3324)
extern "C" void __fastcall MovingItem_SetSpin(BYTE* s, void*, const float* v, const char* name)
{
    float* w = (float*)(s + 440);
    memcpy(w, v, 12);
    float l[3], t[3];
    l[0] = (float)(F(float, s, 620) * w[2] + F(float, s, 608) * w[1] + F(float, s, 596) * w[0]);
    l[1] = (float)(F(float, s, 624) * w[2] + F(float, s, 612) * w[1] + F(float, s, 600) * w[0]);
    l[2] = (float)(F(float, s, 628) * w[2] + F(float, s, 616) * w[1] + F(float, s, 604) * w[0]);
    memcpy(s + 428, CallC<float*>(FUN_4e3132, t, l, s + 404), 12);
    char buf[296];
    if (name)
        sprintf(buf, (const char*)s_q187_67d594, name, (const char*)s_q187_66c4e4);
    else
        sprintf(buf, (const char*)s_q187_67d594, (const char*)s_q187_68d1c0, (const char*)s_q187_68d1c0);
    CallC(FUN_4e2821, s + 428, buf, 50.0f);
    memcpy(s + 3312, s + 428, 12);
    memcpy(s + 3324, s + 440, 12);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor57cfa0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor57cfa0_DeletingDestructor@12", "@Dtor57cfa0_DeletingDestructor@12")
// [$L_5893b0] forwards to Dtor57cfa0_DeletingDestructor for the object 256 bytes before
extern "C" int __fastcall Thunk5893b0(BYTE* s, void* d, int a0) { return q198_Dtor57cfa0_DeletingDestructor(s - 256, d, a0); }

// === the first contact of an item with its neighbours (FUN_584570)

extern "C" void FUN_5ab6a0(); KING_ALIAS("_FUN_5ab6a0", "_Vec3_NearlyEqual")
extern "C" void FUN_57c9a0(); KING_ALIAS("_FUN_57c9a0", "_Id_Same")
extern "C" void FUN_4e5a45(); KING_ALIAS("_FUN_4e5a45", "_Query_4e5a45")
extern "C" void __fastcall Value_Free(int* s);                  // scene.cpp
extern "C" void FUN_5f6820(); KING_ALIAS("_FUN_5f6820", "@Shape_SweepHit@24")    // a shape value: hit test (kind, from, to, x)

static double Ic_Abs(double x) { return x < 0 ? -x : x; }

extern "C" {

// [FUN_584570] the first contact of item b with its up to 6 neighbours (+3508) that can be closer than their
// limits (+3532): a ball (+392 and +3976 both 2) as a sphere hit test of the move *from (b's position) to *to
// (m's position), the contact c getting the points (+52, +64) and the ball's id (+116); others by their own
// query (+256); nonzero for a contact
int __cdecl Item_NeighbourContact(BYTE* b, float* from, float* to, BYTE* m, BYTE* c)
{
    float dt = (float)Game_FrameTime();
    for (int i = 0; i < 6; i++) {
        BYTE* o = F(BYTE*, b, 3508 + i * 4);
        if (!o)
            return 0;
        double d = (Ic_Abs(F(float, o, 3300) - (double)F(float, b, 3300)) +
                    Ic_Abs(F(float, o, 3304) - (double)F(float, b, 3304)) +
                    Ic_Abs(F(float, o, 3308) - (double)F(float, b, 3308))) * dt + F(float, o, 3272) + F(float, b, 3272);
        if (d * d < F(float, b, 3532 + i * 4))
            continue;
        if (F(int, o, 392) == 2 && F(int, o, 3976) == 2) {
            float* s = (float*)operator new(16);
            if (s)
                s[0] = s[1] = s[2] = s[3] = 0;
            BYTE mt[48];
            const float* p = (const float*)(CallThis<BYTE*>(FUN_57de90, o, mt) + 36);
            s[0] = p[0];
            s[1] = p[1];
            s[2] = p[2];
            s[3] = F(float, o, 3272);
            int val[3] = {1, 0, (int)s};
            float P[3], Q[3];
            memcpy(P, CallThis<BYTE*>(FUN_57de90, b, mt) + 36, 12);
            memcpy(Q, m + 36, 12);
            memcpy(from, P, 12);
            memcpy(to, Q, 12);
            if (!CallC<int>(FUN_5ab6a0, Q, P) && CallThis<int>(FUN_5f6820, val, 2, P, Q, F(DWORD, b, 3280))) {
                memcpy(c + 52, P, 12);
                memcpy(c + 64, Q, 12);
                F(int, c, 116) = CallC<int>(FUN_57c9a0, F(BYTE*, b, 3508 + i * 4));
                Value_Free(val);
                return 1;
            }
            Value_Free(val);
        } else {
            int r = CallC<int>(FUN_4e5a45, o + 256, from, to, c, F(DWORD, b, 3280));
            if (r & 1)
                return r;
        }
    }
    return 0;
}

} // extern "C"

// === a moving item's network message (FUN_587e20)

extern "C" void FUN_5dbec0(); KING_ALIAS("_FUN_5dbec0", "_Light_Set@24")
extern "C" void FUN_56f120(); KING_ALIAS("_FUN_56f120", "_Gate_Passed")
extern "C" void FUN_56f210(); KING_ALIAS("_FUN_56f210", "_Screen_56f210")
extern "C" void FUN_4e27a5(); KING_ALIAS("_FUN_4e27a5", "_Game_Time")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
KING_VAR(float, g_im64c320, 64c320)
KING_VAR(AsmLabel, s_im67d99c, 67d99c)
KING_VAR(AsmLabel, s_im67d96c, 67d96c)

extern "C" {

// [FUN_587e20] message 2 for item s with kind k (+1208; 1, 4, 5, 6: the target a, b and position at +1212..; 5
// with a value, 6 with two strings at +1232; 2: a value at +1212; 3: passing gate n (0..2) of the record found
// at the item's position, once it moves); then marked updated (+892 bit 2) at the game time (+904); 1 for done
int __fastcall Item_OnMessage(BYTE* s, void*, int msg, int k, int, int a, int b, const DWORD* pos, BYTE* v)
{
    if (msg != 2)
        return 0;
    F(int, s, 1208) = k;
    switch (k) {
    case 1:
    case 4:
    case 5:
    case 6:
        if (a) {
            F(int, s, 1216) = a;
            F(int, s, 1212) = b;
            memcpy(s + 1220, pos, 12);
            if (k == 5)
                F(DWORD, s, 1232) = F(DWORD, v, 0);
            else if (k == 6) {
                size_t n = strlen((const char*)v);
                strcpy((char*)s + 1232, (const char*)v);
                strcpy((char*)s + 1232 + n + 1, (const char*)v + n + 1);
            }
        }
        break;
    case 2:
        F(DWORD, s, 1212) = F(DWORD, v, 0);
        break;
    case 3:
        if (!(CallC<double>(FUN_5ab4d0, s + 452) < g_im64c320)) {
            int n = F(DWORD, v, 0) & 0xFFFF;
            if (n > 2) {
                char text[100];
                sprintf(text, s_im67d99c, n);
                CallC(FUN_4e260c, text);
            }
            int r = -1;
            const DWORD* p = (const DWORD*)(s + 536);
            CallStd(FUN_5dbec0, F(int, s, 800), p[0], p[1], p[2], (void*)Rec_GetValue32, &r);
            if (r == -1)
                CallC(FUN_4e260c, (const char*)s_im67d96c);
            int f = CallC<int>(FUN_56f210, r);
            static const int k_masks[3] = {3, 12, 48};
            if (!(f & k_masks[n]))
                CallC(FUN_56f120, r, n, s);
        }
        break;
    default:
        F(int, s, 1208) = 0;
        return 0;
    }
    F(DWORD, s, 892) |= 4;
    F(float, s, 904) = (float)CallC<double>(FUN_4e27a5);
    return 1;
}

} // extern "C"

// === a contact between two items (FUN_585da0)


extern "C" {

// [FUN_585da0] item s hits item b with contact c and strength k: a local b (+372 = 0) takes the impulse (its two
// vectors +416, +428 scaled by k into +3148, +3160) by its vtable +172; a remote one is sent the contact (message
// 67d10a: ids +1188, the contact, k, b's matrix and vectors)
void __fastcall Item_SendContact(BYTE* s, void*, BYTE* b, const BYTE* c, float k)
{
    if (!F(int, b, 372)) {
        for (int j = 0; j < 3; j++) {
            F(float, b, 3148 + j * 4) = (float)(F(float, b, 416 + j * 4) * (double)k);
            F(float, b, 3160 + j * 4) = (float)(F(float, b, 428 + j * 4) * (double)k);
        }
        ((void(__fastcall*)(BYTE*, void*, const BYTE*, BYTE*, float))(*(void***)b)[43])(b, 0, c, s, k);
        return;
    }
    BYTE m[32];
    CallThis(FUN_602be0, m);
    BYTE d[296];
    F(int, d, 0) = F(int, b, 1188);
    F(int, d, 4) = 1;
    F(int, d, 8) = F(int, s, 1188);
    memcpy(d + 12, c, 208);
    F(float, d, 220) = k;
    BYTE mt[48];
    memcpy(d + 224, CallThis<BYTE*>(FUN_57de90, b, mt), 48);
    memcpy(d + 272, b + 416, 12);
    memcpy(d + 284, b + 428, 12);
    CallThis(FUN_603110, m, (int)g_sceneCommandCode, d, 296);
    CallThis(FUN_5c3630, g_movite6ef1fc, m);
    CallThis(FUN_602e80, m);
}

} // extern "C"

// === starting an item's flight (582140)

extern "C" void Mi_Length(); KING_ALIAS("_Mi_Length", "_Vec3_Length")
KING_VAR(int, g_mi67d110, 67d110)
KING_VAR(int, g_mi6f6540, 6f6540)
KING_VAR(double, g_mi6f65c0, 6f65c0)
KING_VAR(double, g_mi6f65b8, 6f65b8)
KING_VAR(double, g_mi64b6e0, 64b6e0)
KING_VAR(double, g_mi64d0f0, 64d0f0)

extern "C" {

// [$L_582140] starts item s flying (flags: bit 0 thrown): its speed (+3160) and time, the detail level (+1680, at
// most 1..3 when the frame time is long against 6f65c0 and 6f65b8), its frames (+548, +644) as the start of the
// trail, the trail points cleared; *done = 1, or 0 when it already flies
int __fastcall Item_StartFlight(BYTE* s, void*, int flags, int* done)
{
    *done = 0;
    if (F(int, s, 1420))
        return 0;
    if (flags & 1)
        F(int, s, 1420) = 1;
    F(int, s, 1440) = 1;
    F(int, s, 1436) = flags;
    F(float, s, 1444) = (float)CallC<double>(Mi_Length, s + 3160);
    float t = (float)CallC<double>(FUN_4e2749);
    F(float, s, 1456) = 1.0f;
    F(float, s, 1448) = t;
    F(float, s, 1452) = t;
    F(int, s, 1460) = 0;
    F(int, s, 1672) = 0;
    F(int, s, 1680) = g_mi67d110;
    F(int, s, 1688) = 0;
    F(int, s, 1684) = g_mi6f6540;
    F(int, s, 1676) = 0;
    F(int, s, 1692) = 0;
    if (F(int, s, 1420)) {
        F(int, s, 1680) = F(int, s, 372) ? -1 : 0;
        F(int, s, 1684) = 1;
    }
    double h = t * g_mi64b6e0, h2 = t * g_mi64d0f0;
    int cap = 0;
    if (g_mi6f65c0 > h || g_mi6f65b8 > h)
        cap = F(int, s, 1204) ? 2 : 1;
    else if (g_mi6f65c0 > h2 || g_mi6f65b8 > h2)
        cap = F(int, s, 1204) ? 3 : 2;
    if (cap && F(int, s, 1680) > cap)
        F(int, s, 1680) = cap;
    for (int k = 0; k < 3; k++) {
        memcpy(s + 1696 + k * 96, s + 548, 48);
        memcpy(s + 1744 + k * 96, s + 644, 48);
    }
    F(int, s, 2032) = F(int, s, 804);
    F(int, s, 2036) = F(int, s, 804);
    memcpy(s + 1984, (BYTE*)g_q108_702650, 12);
    memcpy(s + 2020, (BYTE*)g_q108_702650, 12);
    F(int, s, 1432) = 0;
    *done = 1;
    F(int, s, 1412) = 0;
    F(int, s, 1416) = 0;
    for (int i = 0; i < 5; i++) {
        memcpy(s + 2040 + i * 12, (BYTE*)g_q108_702650, 12);
        memcpy(s + 2156 + i * 208, (BYTE*)g_q108_702650, 12);
        memcpy(s + 2168 + i * 208, (BYTE*)g_q108_702650, 12);
    }
    return 1;
}

} // extern "C"

// === after the scene events (FUN_586830)

extern "C" void FUN_4ea27e(); KING_ALIAS("_FUN_4ea27e", "_VehLog_Value")
extern "C" void Ra_MatMul(); KING_ALIAS("_Ra_MatMul", "_Matrix_Multiply")
extern "C" void Ra_sprintf(); KING_ALIAS("_Ra_sprintf", "_sprintf")
KING_VAR(int, g_ra696c6c, 696c6c)
KING_VAR(BYTE*, g_ra6ef1fc, 6ef1fc)
KING_VAR(double, g_ra64b930, 64b930)
KING_VAR(AsmLabel, s_raEvents1, 67d888)
KING_VAR(AsmLabel, s_raEvents2, 67d860)

// logs "name_what x y z"
static void Ra_Log3(const char* fmt, const char* name, double x, double y, double z)
{
    char buf[96];
    CallC(Ra_sprintf, buf, fmt, name, x, y, z);
    CallC(FUN_4ea27e, buf);
}

extern "C" {

// [FUN_586830] after the scene events, item s (when placed, +372) takes its scene node's frame (+32 +68): when its
// set (+812/+814) changed or it moved away from +536 the frame is kept as the slave frame (+816, time +808, the
// speeds +416/+428 to +864/+876); the speeds go back to the object (vtbl+144/+148); a pending stop (+892/+896)
// runs vtbl+60 (with logging when 696c6c)
void __fastcall Item_AfterEvents(BYTE* s)
{
    if (!F(int, s, 372))
        return;
    float m[12];
    for (int i = 0; i < 4; i++)
        m[i * 3] = m[i * 3 + 1] = m[i * 3 + 2] = 0;
    memcpy(m, (BYTE*)g_q108_7025f0, 48);
    CallThis(FUN_5f25a0, F(void*, F(BYTE*, s, 32), 68), m);
    const char* name = F(const char*, F(BYTE*, s, 320), 12);
    if (g_ra696c6c) {
        char buf[96];
        CallC(FUN_4ea27e, "run_after_SRMIEvents");
        CallC(Ra_sprintf, buf, " %s_time %6.3f", name, F(double, g_ra6ef1fc, 2968));
        CallC(FUN_4ea27e, buf);
        Ra_Log3(" %s_M0 %6.3f %6.3f %6.3f ", name, m[9], m[10], m[11]);
    }
    float w[12];
    CallC(Ra_MatMul, w, s + 740, m);
    float dx = (float)((double)w[9] - F(float, s, 536)), dy = (float)((double)w[10] - F(float, s, 540));
    double dz = (double)w[11] - F(float, s, 544);
    WORD set = F(WORD, s, 814);
    if (F(WORD, s, 812) != set || (double)dy * dy + (double)dx * dx + dz * dz > g_ra64b930) {
        F(WORD, s, 812) = set;
        memcpy(s + 816, m, 48);
        F(float, s, 808) = (float)F(double, g_ra6ef1fc, 2968);
        memcpy(s + 864, s + 416, 12);
        memcpy(s + 876, s + 428, 12);
    }
    if (g_ra696c6c) {
        char buf[96];
        CallC(Ra_sprintf, buf, " %s_UpdTime %6.3f", F(const char*, F(BYTE*, s, 320), 12), (double)F(float, s, 808));
        CallC(FUN_4ea27e, buf);
        Ra_Log3(" %s_slaveM %6.3f %6.3f %6.3f", F(const char*, F(BYTE*, s, 320), 12), F(float, s, 852),
                F(float, s, 856), F(float, s, 860));
        Ra_Log3(" %s_M %6.3f %6.3f %6.3f", F(const char*, F(BYTE*, s, 320), 12), F(float, s, 536), F(float, s, 540),
                F(float, s, 544));
    }
    CallVirt(s, 144, s + 416, (const char*)s_raEvents1);
    CallVirt(s, 148, s + 428, (const char*)s_raEvents2);
    if (F(int, s, 892) || F(int, s, 896)) {
        F(int, s, 3556) = 1;
        CallVirt(s, 60);
        F(int, s, 892) = 0;
    }
}

} // extern "C"

// === an item's speeds for its flight (583d40)

KING_VAR(double, g_fs64d338, 64d338)
KING_VAR(float, g_fs64c748, 64c748)

extern "C" {

// [$L_583d40] item s's flight speeds: its spin (+3160, the turn +428 times the step +1452 or +1448) and its length
// (+1444); thrown (+1436 bit 7) against a surface (+892, normal +976) its speed (+3148) is turned along it when
// it goes into it (plus 64c748 of its length along the normal), else it is the move +416 times the step
void __fastcall Item_FlightSpeeds(BYTE* s)
{
    float t = F(int, s, 1676) ? F(float, s, 1452) : F(float, s, 1448);
    float* w = (float*)(s + 3160);
    w[0] = (float)(t * (double)F(float, s, 428));
    w[1] = (float)(t * (double)F(float, s, 432));
    w[2] = (float)(t * (double)F(float, s, 436));
    F(float, s, 1444) = (float)Vec3_Length((const Vec3*)w);
    Vec3* v = (Vec3*)(s + 3148);
    if ((F(BYTE, s, 1436) & 128) && F(int, s, 892)) {
        double l = Vec3_Length(v);
        const float* n = (const float*)(s + 976);
        double dn = (double)v->z * n[2] + (double)v->y * n[1] + (double)v->x * n[0];
        if (!(l * g_fs64d338 > dn))
            return;
        double a = l * n[0];
        float b = (float)(l * n[1]), c = (float)(l * n[2]);
        float q[3] = {(float)(a * g_fs64c748), (float)(b * (double)g_fs64c748), (float)(c * (double)g_fs64c748)};
        Vec3 w1, w2, r;
        w1.Set((float)((double)v->z * n[1] - (double)n[2] * v->y), (float)((double)n[2] * v->x - (double)v->z * n[0]),
               (float)((double)n[0] * v->y - (double)v->x * n[1]));
        w2.Set((float)((double)w1.y * n[2] - (double)w1.z * n[1]), (float)((double)w1.z * n[0] - (double)w1.x * n[2]),
               (float)((double)w1.x * n[1] - (double)w1.y * n[0]));
        r.Set((float)((double)w2.x + q[0]), (float)((double)w2.y + q[1]), (float)((double)w2.z + q[2]));
        *v = r;
        return;
    }
    Vec3 m;
    m.Set((float)(t * (double)F(float, s, 416)), (float)(t * (double)F(float, s, 420)), (float)(t * (double)F(float, s, 424)));
    *v = m;
}

} // extern "C"

// === the ground normal under an item (FUN_582960)

extern "C" void FUN_4e7261(); KING_ALIAS("_FUN_4e7261", "_Query_4e7261")
KING_VAR(Vec3, g_gn702630, 702630)
KING_VAR(double, g_gn64b668, 64b668)
KING_VAR(float, g_gn64c2f8, 64c2f8)

extern "C" {

// [FUN_582960] *out = the ground normal under item s (+536), kept with its point (+1176, +1164) until the item moves
// away by its size (+400): probed down from the item by the size times 10, else from above to below by 64c2f8
Vec3* __fastcall Item_GroundNormal(BYTE* s, void*, Vec3* out)
{
    Vec3* hp = (Vec3*)(s + 1164);
    Vec3* np = (Vec3*)(s + 1176);
    const Vec3* p = (const Vec3*)(s + 536);
    double dx = (double)hp->x - p->x, dy = (double)hp->y - p->y;
    double dz = (double)hp->z - p->z;
    float fdx = (float)dx, fdy = (float)dy;
    if (F(float, s, 400) * g_gn64b668 > (double)fdx * fdx + (double)fdy * fdy + dz * dz) {
        *out = *np;
        return out;
    }
    const Vec3& u = g_gn702630;
    double r = F(float, s, 400);
    float ux = (float)(u.x * r), uy = (float)(u.y * r);
    double uz = r * u.z;
    Vec3 h = {0, 0, 0}, n = {0, 0, 0};
    float a = (float)(ux * (double)g_ten64c450), b = (float)(uy * (double)g_ten64c450);
    double c = uz * g_ten64c450;
    Vec3 from;
    from.x = (float)((double)p->x - a);
    from.y = (float)((double)p->y - b);
    from.z = (float)((double)p->z - c);
    if (CallC<int>(FUN_4e7261, F(int, s, 800), 0, 0, p, &from, &h, &n) & 1) {
        *hp = h;
        *np = n;
        *out = n;
        return out;
    }
    double vx = u.x * (double)g_gn64c2f8;
    float vy = (float)(u.y * (double)g_gn64c2f8), vz = (float)(u.z * (double)g_gn64c2f8);
    Vec3 below, v, to;
    below.x = (float)((double)p->x - vx);
    below.y = (float)((double)p->y - vy);
    below.z = (float)((double)p->z - vz);
    v.x = (float)vx, v.y = vy, v.z = vz;
    Vec3_Add(&to, p, &v);
    if (CallC<int>(FUN_4e7261, F(int, s, 800), 0, 0, &to, &below, &h, &n) & 1) {
        *np = n;
        *hp = h;
    }
    *out = *np;
    return out;
}

} // extern "C"

// === a box receiving a hit (FUN_586240)

extern "C" void Rh_AddAssign(); KING_ALIAS("_Rh_AddAssign", "@Vec3_AddAssign@16")
KING_VAR(double, g_rh64b840, 64b840)
KING_VAR(double, g_rh64b6e0, 64b6e0)
KING_VAR(double, g_rh64d338, 64d338)
KING_VAR(Vec3, g_rh702630, 702630)
KING_VAR(AsmLabel, s_rhHit1, 67d80c)

extern "C" {

// [FUN_586240] box s hit at contact c (+52 the point, +64 the normal) by object id over time dt: the first hit wakes
// it (+1200; vtbl+208) with a hop up by its speed (at most 64b6e0); then the hit (+892 bit 1, time +904, point
// +964, normal +976, by +1028), the impulse of its speed change (+416 against +452, times its mass +396) at the
// point (vtbl+164), the speeds over dt kept (+3148, +3160; vtbl+80) and the hit time +3144 (vtbl+160 +912)
void __fastcall Box_ReceiveHit(BYTE* s, void*, const BYTE* c, int id, float dt)
{
    if (!F(int, s, 1200)) {
        CallThis(FUN_5d32f0, s, 1);
        F(int, s, 1200) = 1;
        CallVirt(s, 208);
        float* v = (float*)(s + 416);
        double l = Vec3_Length((const Vec3*)v) * g_rh64b840;
        if (l > g_rh64b6e0)
            l = g_rh64b6e0;
        l += g_rh64d338;
        const Vec3& u = g_rh702630;
        float b[3] = {(float)(u.x * l), (float)(u.y * l), (float)(u.z * l)};
        float t[3];
        CallThis(Rh_AddAssign, v, t, b);
        CallVirt(s, 144, v, (const char*)s_rhHit1);
    }
    DWORD f = F(DWORD, s, 892) | 2;
    F(int, s, 1192) = 0;
    F(DWORD, s, 892) = f;
    F(DWORD, s, 896) = f;
    F(float, s, 904) = (float)CallC<double>(FUN_4e27a5);
    memcpy(s + 964, c + 52, 12);
    F(float, s, 976) = -F(float, c, 64);
    F(float, s, 980) = -F(float, c, 68);
    F(float, s, 984) = -F(float, c, 72);
    F(int, s, 1028) = CallC<int>(FUN_57c9a0, id);
    float m = F(float, s, 396);
    double dx = (double)F(float, s, 416) - F(float, s, 452);
    float dy = (float)((double)F(float, s, 420) - F(float, s, 456));
    float dz = (float)((double)F(float, s, 424) - F(float, s, 460));
    float imp[3] = {(float)(dx * (double)m), (float)(dy * (double)m), (float)(dz * (double)m)};
    CallVirt(s, 164, c + 52, imp);
    for (int i = 0; i < 3; i++) {
        F(float, s, 3148 + i * 4) = (float)(dt * (double)F(float, s, 416 + i * 4));
        F(float, s, 3160 + i * 4) = (float)(dt * (double)F(float, s, 428 + i * 4));
    }
    CallVirt(s, 80, dt, 1);
    F(float, s, 3144) = (float)CallC<double>(FUN_4e27a5);
    CallVirt(s, 160, s + 912);
}

} // extern "C"

// === an item's frame after a step (5825a0)

extern "C" void Pf_MatMul(); KING_ALIAS("_Pf_MatMul", "_Matrix_Multiply")
extern "C" void Pf_Translation(); KING_ALIAS("_Pf_Translation", "_Matrix_Translation")
extern "C" void Pf_LookAt(); KING_ALIAS("_Pf_LookAt", "_Matrix_LookAt")
KING_VAR(AsmLabel, s_pf67d5d8, 67d5d8)
struct Pf_V3 { float x, y, z; };

extern "C" {

// [$L_5825a0] item s's frame after a step of t (+1456): its frame (+1792 in flight, +548 else) turned about its spin
// (+3160) by its spin rate (+1444) and moved by its speed (+3148), to +1696 with its inverse at +1744; checked
void __fastcall Item_StepFrame(BYTE* s, void*, float t)
{
    F(float, s, 1456) = t;
    Pf_V3 tr = {(float)(t * (double)F(float, s, 3148)), (float)(t * (double)F(float, s, 3152)),
                (float)(t * (double)F(float, s, 3156))};
    float mt[12], mr[12], m1[12], m2[12], inv[12];
    float* T = CallC<float*>(Pf_Translation, mt, tr);
    float ang = (float)((double)F(float, s, 1444) * F(float, s, 1456));
    float* R = CallC<float*>(Pf_LookAt, mr, *(const Pf_V3*)(BYTE*)g_q108_702650, *(const Pf_V3*)(s + 3160), ang);
    BYTE* base = F(int, s, 1676) ? s + 1792 : s + 548;
    float* m = CallC<float*>(Pf_MatMul, m2, CallC<float*>(Pf_MatMul, m1, R, base), T);
    memcpy(s + 1696, m, 48);
    memcpy(s + 1744, CallC<float*>(FUN_5ac2e0, inv, s + 1696), 48);
    CallC(FUN_4e2821, s + 1732, (const char*)s_pf67d5d8, 10000.0f);
}

} // extern "C"

// === a box hit with new speeds (FUN_5864d0)

KING_VAR(AsmLabel, s_rhHit2a, 67d844)
KING_VAR(AsmLabel, s_rhHit2b, 67d828)

extern "C" {

// [FUN_5864d0] box s hit at contact c by object id over dt, taking speed v (a hop up by its length added when the hit
// wakes it) and spin w; the hit kept as in Box_ReceiveHit
void __fastcall Box_ReceiveHitSet(BYTE* s, void*, const BYTE* c, int id, float dt, int, const float* v, const float* w)
{
    float nv[3];
    memcpy(nv, v, 12);
    if (!F(int, s, 1200)) {
        CallThis(FUN_5d32f0, s, 1);
        F(int, s, 1200) = 1;
        CallVirt(s, 208);
        double l = Vec3_Length((const Vec3*)v) * g_rh64b840 + g_rh64d338;
        const Vec3& u = g_rh702630;
        float b[3] = {(float)(u.x * l), (float)(u.y * l), (float)(u.z * l)};
        float t[3];
        CallThis(Rh_AddAssign, nv, t, b);
    }
    F(int, s, 1192) = 0;
    F(DWORD, s, 892) |= 2;
    F(float, s, 904) = (float)CallC<double>(FUN_4e27a5);
    memcpy(s + 964, c + 52, 12);
    F(float, s, 976) = -F(float, c, 64);
    F(float, s, 980) = -F(float, c, 68);
    F(float, s, 984) = -F(float, c, 72);
    F(int, s, 1028) = CallC<int>(FUN_57c9a0, id);
    CallVirt(s, 144, nv, (const char*)s_rhHit2a);
    CallVirt(s, 148, w, (const char*)s_rhHit2b);
    for (int i = 0; i < 3; i++)
        F(float, s, 3148 + i * 4) = (float)(dt * (double)F(float, s, 416 + i * 4));
    for (int i = 0; i < 3; i++)
        F(float, s, 3160 + i * 4) = (float)(dt * (double)F(float, s, 428 + i * 4));
    CallVirt(s, 80, dt, 1);
    CallVirt(s, 160, s + 912);
    F(float, s, 3144) = (float)CallC<double>(FUN_4e27a5);
}

} // extern "C"

// === an item's points below a plane (FUN_581ea0)

extern "C" void Pb_PointsNext(); KING_ALIAS("_Pb_PointsNext", "@Points_Next@12")
KING_VAR(double, g_pb64b658, 64b658)

extern "C" {

// [FUN_581ea0] the number of the item's points (iterator it, up to 20, placed by matrix m) whose distance from the
// plane (point p, normal n) is below 64b658: *lowest the smallest distance (from 1); per point the flag (below)
// and the distance when those arrays are given; the unused entries get 0 and 1.0
int __cdecl Points_BelowPlane(BYTE* it, const float* m, const float* p, const float* n, float* lowest, int* flags,
    float* dist)
{
    float pts[20][3], w[20][3];
    memset(pts, 0, sizeof pts);
    memset(w, 0, sizeof w);
    *(DWORD*)lowest = 0x3F800000;
    int count = 0, hits = 0;
    while (CallThis<int>(Pb_PointsNext, it, pts[count])) {
        const float* q = pts[count];
        float* o = w[count];
        o[0] = (float)((double)q[1] * m[3] + (double)q[0] * m[0] + (double)q[2] * m[6] + m[9]);
        o[1] = (float)((double)q[2] * m[7] + (double)q[1] * m[4] + (double)q[0] * m[1] + m[10]);
        o[2] = (float)((double)m[8] * q[2] + (double)q[0] * m[2] + (double)m[5] * q[1] + m[11]);
        if (flags)
            flags[count] = 0;
        double d = ((double)o[0] - p[0]) * n[0] + ((double)o[2] - p[2]) * n[2] + ((double)o[1] - p[1]) * n[1];
        if (dist)
            dist[count] = (float)d;
        if (d < *lowest)
            *lowest = (float)d;
        if (d < g_pb64b658) {
            if (d < *lowest)
                *lowest = (float)d;
            hits++;
            if (flags)
                flags[count] = 1;
            if (dist)
                dist[count] = (float)d;
        }
        count++;
    }
    for (int i = count; i < 20; i++) {
        if (flags)
            flags[i] = 0;
        if (dist)
            *(DWORD*)&dist[i] = 0x3F800000;
    }
    return hits;
}

} // extern "C"

// === an item's nearest neighbours (a method at $L_587ac0)

extern "C" void Ni_Matrix1(); KING_ALIAS("_Ni_Matrix1", "@SceneObj_Matrix1@12")
KING_VAR(BYTE*, g_ni6f653c, 6f653c)

extern "C" {

// [$L_587ac0] the six bodies (of the list 6f653c, other than s) whose centres (+3436 placed) lie nearest to item
// s's position (+536): +3508 the bodies, +3532 their squared distances (100000 when free), nearest first
void __fastcall Item_FindNeighbours(BYTE* s)
{
    BYTE* node = g_ni6f653c ? g_ni6f653c - 4 : 0;
    const float* pos = (const float*)(s + 536);
    float px = pos[0], py = pos[1], pz = pos[2];
    BYTE** nb = (BYTE**)(s + 3508);
    float* dist = (float*)(s + 3532);
    for (int k = 0; k < 6; k++) {
        nb[k] = 0;
        *(DWORD*)&dist[k] = 0x47C35000;
    }
    for (; node; node = F(BYTE*, node, 8) ? F(BYTE*, node, 8) - 4 : 0) {
        BYTE* o = node - 376;
        if (o == s)
            continue;
        BYTE m[48];
        const float* M = CallThis<const float*>(Ni_Matrix1, o, m);
        const float* c = (const float*)(o + 3436);
        double wx = (double)M[3] * c[1] + (double)M[6] * c[2] + (double)c[0] * M[0] + M[9];
        float wy = (float)((double)M[4] * c[1] + (double)M[7] * c[2] + (double)M[1] * c[0] + M[10]);
        float wz = (float)((double)M[5] * c[1] + (double)M[8] * c[2] + (double)M[2] * c[0] + M[11]);
        float dx = (float)(wx - px);
        float dy = (float)(wy - py);
        double dz = (double)wz - pz;
        double d2 = (double)dx * dx + (double)dy * dy + dz * dz;
        int i = 0;
        for (; i < 6; i++)
            if (!nb[i] || d2 < dist[i])
                break;
        if (i == 6)
            continue;
        for (int j = 5; j > i; j--) {
            nb[j] = nb[j - 1];
            dist[j] = dist[j - 1];
        }
        nb[i] = o;
        dist[i] = (float)d2;
    }
}

} // extern "C"

// === an item coming to rest (FUN_580dd0)

extern "C" void Ir_Log(); KING_ALIAS("_Ir_Log", "_VehLog_Value")
KING_VAR(int, g_ir696c6c, 696c6c)
KING_VAR(char, s_ir67d4b0, 67d4b0)
KING_VAR(char, s_ir67d138, 67d138)
KING_VAR(char, s_ir67d11c, 67d11c)

extern "C" {

// [FUN_580dd0] item s comes to rest: when asleep (+1192) and resting (+892) it only drops +1200 (an item without
// an owner +372 is told vtbl+204); else (+814 the frame number when unowned) its velocity (+476) and spin (+488)
// are cleared, it is stopped (vtbl+76), +1192 = 0, +1200 = 1 and event 1 is posted; with the log (696c6c) its
// node's frame and its position are logged
void __fastcall Item_Rest(BYTE* s)
{
    if (F(int, s, 1192) && F(int, s, 892)) {
        F(int, s, 1200) = 0;
        if (!F(int, s, 372))
            CallVirt(s, 204);
        return;
    }
    if (!F(int, s, 372))
        F(WORD, s, 814) = (WORD)F(int, g_movite6ef1fc, 2992);
    F(Vec3, s, 476) = g_mi702650;
    F(Vec3, s, 488) = g_mi702650;
    F(int, s, 892) = 0;
    CallVirt(s, 76);
    F(int, s, 1192) = 0;
    F(int, s, 1200) = 1;
    CallThis(FUN_5d32f0, s, 1);
    if (!g_ir696c6c)
        return;
    float m[12];
    memcpy(m, g_q108_7025f0, 48);
    CallThis(FUN_5f25a0, F(BYTE*, F(BYTE*, s, 32), 68), m);
    CallC(Ir_Log, &s_ir67d4b0);
    char buf[100];
    const char* name = F(const char*, F(BYTE*, s, 320), 12);
    sprintf(buf, &s_ir67d138, name, (double)m[9], (double)m[10], (double)m[11]);
    CallC(Ir_Log, buf);
    sprintf(buf, &s_ir67d11c, name, (double)F(float, s, 536), (double)F(float, s, 540), (double)F(float, s, 544));
    CallC(Ir_Log, buf);
}

} // extern "C"

// === how far an item may move toward a frame (FUN_5843c0)

extern "C" void Im_Query(); KING_ALIAS("_Im_Query", "_Query_4e6578")
extern "C" void Im_Get800(); KING_ALIAS("_Im_Get800", "@SceneObj_Get800@8")
KING_VAR(double, g_im64b6d8, 64b6d8)
KING_VAR(float, g_im64c250, 64c250)
KING_VAR(double, g_one_64b670_im, 64b670)

extern "C" {

// [FUN_5843c0] tests item o's move from its position to frame t's (+36) against the world: no hit gives 0; else
// *dist the target's height over the hit plane (h +52, normal +64) and *scale shrunk by the square root of the free
// part of the move (1 when the hit is beyond it; 0 and the result 2 when either length is tiny)
int __cdecl Item_MoveFraction(BYTE* o, const BYTE* t, float* scale, float* dist, BYTE* h)
{
    BYTE* w = CallThis<BYTE*>(Im_Get800, o);
    BYTE m[48];
    CallThis(FUN_57de90, o, m);
    float p[3] = {F(float, m, 36), F(float, m, 40), F(float, m, 44)};
    float q[3] = {F(float, t, 36), F(float, t, 40), F(float, t, 44)};
    if (!(CallC<BYTE>(Im_Query, o, w, p, q, h) & 1))
        return 0;
    float dx = q[0] - p[0], dy = q[1] - p[1];
    double dz = (double)q[2] - p[2];
    float ax = F(float, h, 52) - p[0], ay = F(float, h, 56) - p[1], az = F(float, h, 60) - p[2];
    *dist = (float)(((double)F(float, h, 60) - q[2]) * F(float, h, 72) + ((double)F(float, h, 56) - q[1]) * F(float, h, 68)
        + ((double)F(float, h, 52) - q[0]) * F(float, h, 64));
    float l1 = (float)((double)dx * dx + (double)dy * dy + dz * dz);
    float l2 = (float)((double)ax * ax + (double)ay * ay + (double)az * az);
    if (l2 < g_im64b6d8 || l1 < g_im64b6d8) {
        *scale = 0;
        return 2;
    }
    double r = l2 / l1;
    if (r > g_one_64b670_im)
        r = g_im64c250;
    *scale = (float)(x87_sqrt(r) * *scale);
    return 1;
}

} // extern "C"

// === an item's contact with a body (FUN_5855a0)

extern "C" void Ic_Matrix1(); KING_ALIAS("_Ic_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Ic_Get800(); KING_ALIAS("_Ic_Get800", "@SceneObj_Get800@8")
extern "C" void Ic_HitObject(); KING_ALIAS("_Ic_HitObject", "_Query_HitObject")
extern "C" void Ic_Query1(); KING_ALIAS("_Ic_Query1", "_Query_4e67f9")
extern "C" void Ic_Neighbour(); KING_ALIAS("_Ic_Neighbour", "_Item_NeighbourContact")
extern "C" void Ic_Query3(); KING_ALIAS("_Ic_Query3", "_Query_4e6d41")
extern "C" void Ic_TrackInit(); KING_ALIAS("_Ic_TrackInit", "@Track587c80_Construct@4")
extern "C" void FUN_585150(); KING_ALIAS("_FUN_585150", "_Item_PointContacts")
KING_VAR(double, g_ic64b6d8, 64b6d8)
KING_VAR(double, g_ic64b670, 64b670)
KING_VAR(float, g_ic64c250, 64c250)

static Vec3 Ic_Set(double x, double y, double z)
{
    Vec3 v;
    v.x = (float)x;
    v.y = (float)y;
    v.z = (float)z;
    return v;
}

// node q's point +52 moved r along its normal +64, less p
static Vec3 Ic_Off(const BYTE* q, double r, const Vec3& p)
{
    const float* n = (const float*)(q + 64);
    const float* c = (const float*)(q + 52);
    Vec3 t = Ic_Set(r * n[0], r * n[1], r * n[2]);
    Vec3 u = Ic_Set((double)t.x + c[0], (double)t.y + c[1], (double)t.z + c[2]);
    return Ic_Set((double)u.x - p.x, (double)u.y - p.y, (double)u.z - p.z);
}

extern "C" {

// [FUN_5855a0] the contact of item s (kind +392 1 or 2, its point +3448) with a body of frame m, through node q:
// a direct hit (1), a neighbour (2), the rim (3), else the traced track (4, unless flag 64); *depth gets the
// contact depth and *scale is scaled by the root of the moved share; the kind, or 0
int __cdecl Item_BodyContact(BYTE* s, const Matrix* m, float* scale, float* depth, BYTE* q, int flags)
{
    if (F(int, s, 392) != 2 && F(int, s, 392) != 1)
        return 0;
    Matrix mi;
    CallThis(Ic_Matrix1, s, &mi);
    const float* p = (const float*)(s + 3448);
    const float* I = mi.m;
    const float* M = m->m;
    Vec3 a = Ic_Set((double)I[0] * p[0] + (double)I[6] * p[2] + (double)I[3] * p[1] + I[9],
                    (double)I[1] * p[0] + (double)I[7] * p[2] + (double)I[4] * p[1] + I[10],
                    (double)I[2] * p[0] + (double)I[8] * p[2] + (double)I[5] * p[1] + I[11]);
    double bz = (double)M[8] * p[2] + (double)M[2] * p[0] + (double)M[5] * p[1] + M[11];
    Vec3 b = Ic_Set((double)M[6] * p[2] + (double)M[3] * p[1] + (double)p[0] * M[0] + M[9],
                    (double)M[7] * p[2] + (double)M[1] * p[0] + (double)M[4] * p[1] + M[10], bz);
    Vec3 d = Ic_Set((double)b.x - a.x, (double)b.y - a.y, bz - a.z);
    Vec3 v = {0, 0, 0};
    BYTE* o = CallC<BYTE*>(Ic_HitObject, CallThis<BYTE*>(Ic_Get800, s), &mi.m[9], &a);
    if (!o)
    {
        o = CallThis<BYTE*>(Ic_Get800, s);
        CallVirt(s, 28, 1);
    }
    int kind;
    if (CallC<int>(Ic_Query1, s, o, F(int, s, 3276), &a, &b, q) & 1)
    {
        kind = 1;
        double k = F(float, s, 3276);
        const Vec3* n = (const Vec3*)(q + 64);
        const Vec3* c = (const Vec3*)(q + 52);
        Vec3 t1, t2, t3;
        *depth = (float)Vec3_Dot(Vec3_Sub(&t3, Vec3_Add(&t2, c, Vec3_Scaled(&t1, n, (float)k)), &b), n);
        v = *Vec3_Sub(&t3, Vec3_Add(&t2, c, Vec3_Scaled(&t1, n, (float)k)), &a);
    }
    else if (CallC<int>(Ic_Neighbour, s, &a, &b, m, q) & 1)
    {
        kind = 2;
        double r = F(float, s, 3280);
        Vec3 t = Ic_Off(q, r, b);
        *depth = (float)((double)F(float, q, 68) * t.y + (double)F(float, q, 72) * t.z + (double)t.x * F(float, q, 64));
        v = Ic_Off(q, r, a);
    }
    else if (CallC<int>(Ic_Query3, s, o, F(int, s, 3284), &a, &b, q) & 1)
    {
        kind = 3;
        double r = F(float, s, 3284);
        Vec3 t = Ic_Off(q, r, b);
        *depth = (float)((double)F(float, q, 68) * t.y + (double)F(float, q, 72) * t.z + (double)F(float, q, 64) * t.x);
        v = Ic_Off(q, r, a);
    }
    else
    {
        if (flags & 64)
            return 0;
        BYTE tr[6400];
        CallThis(Ic_TrackInit, tr);
        int r = CallC<int>(FUN_585150, s, m, tr);
        if (!r)
            return r;
        kind = 4;
        for (int i = 0; i < F(int, tr, 2816); i++)
        {
            if (!F(int, tr, 2820 + i * 4))
                continue;
            *depth = F(float, tr, 2560 + i * 4);
            double rr = F(float, s, 3272);
            const float* n = (const float*)(tr + 768 + i * 12);
            const float* c = (const float*)(tr + i * 12);
            Vec3 t = Ic_Set(rr * n[0], rr * n[1], rr * n[2]);
            Vec3 u = Ic_Set((double)t.x + c[0], (double)t.y + c[1], (double)t.z + c[2]);
            v = Ic_Set((double)u.x - a.x, (double)u.y - a.y, (double)u.z - a.z);
            F(int, q, 116) = 0;
            F(Vec3, q, 52) = *(const Vec3*)c;
            F(Vec3, q, 64) = *(const Vec3*)n;
        }
    }
    float d2 = (float)Vec3_Dot(&d, &d);
    double m2x = Vec3_Dot(&v, &v);
    float m2 = (float)m2x;
    if (m2x < g_ic64b6d8 || d2 < g_ic64b6d8)
    {
        *scale = 0;
        return kind;
    }
    double t = (double)m2 / d2;
    if (t > g_ic64b670)
        t = g_ic64c250;
    *scale = (float)(x87_sqrt(t) * *scale);
    return kind;
}

} // extern "C"

// === an item's contacts with the world's points (FUN_585150)

extern "C" void Ip_PointHit(); KING_ALIAS("_Ip_PointHit", "_Item_PointContact")
extern "C" void Ip_SphereHit(); KING_ALIAS("_Ip_SphereHit", "_Item_SphereContact")
extern "C" void Ip_FrameTime(); KING_ALIAS("_Ip_FrameTime", "_Game_FrameTime")
extern "C" void Ip_Get800(); KING_ALIAS("_Ip_Get800", "@SceneObj_Get800@8")
extern "C" void Ip_Matrix1(); KING_ALIAS("_Ip_Matrix1", "@SceneObj_Matrix1@12")
extern "C" void Ip_Matrix2(); KING_ALIAS("_Ip_Matrix2", "@SceneObj_Matrix2@12")
extern "C" void Ip_FrameWorld(); KING_ALIAS("_Ip_FrameWorld", "_Frame_GetWorld")
extern "C" BYTE** __cdecl Enum_56f3f0(void* obj, int a, int b, int c, int d, int* count);
extern "C" int __cdecl Enum_56f650(int i, int* a, int* b, double* c, int* d, int* e, int* f);
KING_VAR(float, g_ip64c40c, 64c40c)

extern "C" {

// [FUN_585150] item s at frame m: the world's point groups (Enum_56f3f0) within its reach (+3436 through m,
// radius +3272 grown by its speed +3460) are tested point by point (plain points by FUN_584820, spheres by
// FUN_584c70, in the item's own axes too); a hit on a group with faces sends the item its message. The count
// of hits, negated when messages answered
int __cdecl Item_PointContacts(BYTE* s, const float* m, BYTE* w)
{
    double dt = CallC<double>(Ip_FrameTime);
    int count = 0;
    const float* c = (const float*)(s + 3436);
    float cen[4];
    cen[0] = (float)((double)m[6] * c[2] + (double)m[3] * c[1] + (double)c[0] * m[0] + m[9]);
    cen[1] = (float)((double)m[1] * c[0] + (double)m[7] * c[2] + (double)m[4] * c[1] + m[10]);
    cen[2] = (float)((double)m[2] * c[0] + (double)m[8] * c[2] + (double)m[5] * c[1] + m[11]);
    float r = (float)(dt * F(float, s, 3460) + F(float, s, 3272) + g_ip64c40c);
    cen[3] = r;
    BYTE** list = Enum_56f3f0(CallThis<void*>(Ip_Get800, s), F(int, cen, 0), F(int, cen, 4), F(int, cen, 8), F(int, cen, 12), &count);
    if (count <= 0)
        return 0;
    float m1[12], m2[12], mm[12], inv[12];
    CallThis(Ip_Matrix1, s, m1);
    CallThis(Ip_Matrix2, s, m2);
    Matrix_Multiply((Matrix*)mm, (const Matrix*)m2, (const Matrix*)m);
    Matrix_Inverse((Matrix*)inv, (const Matrix*)mm);
    double r2 = (double)r * r;
    int total = 0, hits = 0;
    for (int i = 0; i < count; i++)
    {
        int pts, flag, faces, f2, node;
        double rad;
        int n = Enum_56f650(i, &pts, &flag, &rad, &faces, &f2, &node);
        for (int j = 0; j < n; j++)
        {
            float W[12];
            memcpy(W, &g_identity, 48);
            if (node)
                CallC(Ip_FrameWorld, node, W);
            const float* p = (const float*)(pts + j * 12);
            double y = (double)W[1] * p[0] + (double)W[7] * p[2] + (double)W[4] * p[1] + W[10];
            double z = (double)W[2] * p[0] + (double)W[8] * p[2] + (double)W[5] * p[1] + W[11];
            float wp[3];
            wp[0] = (float)((double)W[0] * p[0] + (double)W[6] * p[2] + (double)W[3] * p[1] + W[9]);
            wp[1] = (float)y;
            wp[2] = (float)z;
            float dx = (float)((double)wp[0] - m1[9]), dy = (float)(y - m1[10]);
            double dz = z - m1[11];
            if (!((double)dx * dx + (double)dy * dy + dz * dz + rad * rad <= r2))
                continue;
            float l[3];
            l[0] = (float)((double)inv[3] * y + (double)inv[6] * z + (double)inv[0] * wp[0] + inv[9]);
            l[1] = (float)((double)inv[1] * wp[0] + (double)inv[4] * y + (double)inv[7] * z + inv[10]);
            l[2] = (float)((double)inv[2] * wp[0] + (double)inv[5] * y + (double)inv[8] * z + inv[11]);
            int h = flag ? CallC<int>(Ip_SphereHit, wp, l, (float)rad, w, s, 1) : CallC<int>(Ip_PointHit, wp, l, w, s);
            if (h && f2)
            {
                int fw = F(int, (BYTE*)faces, 0);
                int a = (short)fw, b = fw >> 16;
                hits += Item_OnMessage(s, (void*)(INT_PTR)a, a, b, 1, node, (int)list[i],
                                       (const DWORD*)(w + F(int, w, 2816) * 12 - 12), (BYTE*)faces + 4);
            }
            else if (h)
                SceneObj_SetFlags(s, 0, 4);
            total += h;
        }
    }
    return hits ? -total : total;
}

} // extern "C"

// === an item's flight leg ending (a method at $L_584060)

extern "C" void Il_Lookup(); KING_ALIAS("_Il_Lookup", "_Obj_Lookup4e5b22")
KING_VAR(float, g_il64f7b0, 64f7b0)
KING_VAR(float, g_il64c32c, 64c32c)
KING_VAR(float, g_il64c258, 64c258)

extern "C" {

// [$L_584060] item s at the end of a leg: within its time (+1432 up to +1680 + +1684) it looks up the object
// under it (else stays on +804 and is told 28) and takes its stored frame +1696 and speed; past it, *done is set
// and it keeps frame +548. Its local speed (+428, the spin +440 through the inverse frame +596 over the
// inertia +404) is limited, the spin set from it (method 152) and the pushes cleared
void __fastcall Item_EndLeg(BYTE* s, void*, int* done)
{
    const BYTE* src;
    if (F(int, s, 1432) <= F(int, s, 1684) + F(int, s, 1680))
    {
        BYTE* o = CallC<BYTE*>(Il_Lookup, F(BYTE*, s, 804), s + 584, s + 1732, s + 1132);
        F(BYTE*, s, 800) = o;
        if (!o)
        {
            F(BYTE*, s, 800) = F(BYTE*, s, 804);
            CallVirt(s, 28, 1);
        }
        F(float, s, 888) = (float)Vec3_Length(&F(Vec3, s, 416));
        src = s + 1696;
    }
    else
    {
        *done = 1;
        F(BYTE*, s, 800) = F(BYTE*, s, 804);
        src = s + 548;
    }
    memmove(s + 500, src, 48);
    Matrix inv;
    memcpy(s + 596, Matrix_Inverse(&inv, (const Matrix*)(s + 500)), 48);
    const float* I = (const float*)(s + 596);
    const float* w = (const float*)(s + 440);
    Vec3 v, tmp;
    v.x = (float)((double)I[6] * w[2] + (double)I[3] * w[1] + (double)I[0] * w[0]);
    v.y = (float)((double)I[1] * w[0] + (double)I[7] * w[2] + (double)I[4] * w[1]);
    v.z = (float)((double)I[2] * w[0] + (double)I[8] * w[2] + (double)I[5] * w[1]);
    Vec3* q = &F(Vec3, s, 428);
    *q = *Vec3_DivComponents(&tmp, &v, &F(Vec3, s, 404));
    double L = Vec3_Length(q);
    bool spin = true;
    if (L > g_il64f7b0)
    {
        double k = g_il64c32c / L;
        q->x = (float)(k * q->x);
        q->y = (float)(k * q->y);
        q->z = (float)(k * q->z);
    }
    else if (L > g_im64c320)
    {
        double m = (L - g_im64c320) * g_il64c258 + g_im64c320;
        float mx = (float)(m * q->x), my = (float)(m * q->y);
        double mz = m * q->z;
        q->x = (float)(mx / L);
        q->y = (float)(my / L);
        q->z = (float)(mz / L);
    }
    else
        spin = false;
    if (spin)
    {
        Vec3 t2;
        F(Vec3, s, 440) = *Vec3_RotateBy(&t2, Vec3_MulComponents(&tmp, q, &F(Vec3, s, 404)), (const Matrix*)(s + 500));
    }
    CallVirt(s, 152, s + 440, 0);
    F(Vec3, s, 476) = g_zeroVec;
    F(Vec3, s, 488) = g_zeroVec;
    F(int, s, 1420) = 0;
    Vehicle_MakeOut(s, 0);
}

} // extern "C"

// === an item knocked over (FUN_588090)

extern "C" void Mk_HitObject(); KING_ALIAS("_Mk_HitObject", "_Query_HitObject")
extern "C" void Mk_Query(); KING_ALIAS("_Mk_Query", "_Query_4e6749")
extern "C" void Mk_SignSwing(); KING_ALIAS("_Mk_SignSwing", "_Sign_Swing")
extern "C" void Mk_Decompose(); KING_ALIAS("_Mk_Decompose", "_Matrix_Decompose6FArray")
extern "C" void Mk_FrameWorld(); KING_ALIAS("_Mk_FrameWorld", "_Frame_GetWorld")
extern "C" void Mk_FrameLocal(); KING_ALIAS("_Mk_FrameLocal", "_Frame_GetLocal")
extern "C" void Mk_FrameApply(); KING_ALIAS("_Mk_FrameApply", "_Frame_Apply")
extern "C" void Mk_Select(); KING_ALIAS("_Mk_Select", "_Switch_Select")
extern "C" void Mk_Find(); KING_ALIAS("_Mk_Find", "_Scene_FindObject")
extern "C" void Mk_VehObject(); KING_ALIAS("_Mk_VehObject", "_Vehicle_Object")
extern "C" void Mk_Fatal(); KING_ALIAS("_Mk_Fatal", "_World_Fatal")
extern "C" void Mk_580a00(); KING_ALIAS("_Mk_580a00", "_MovingItem_PointBounce")
extern "C" void __fastcall Movite_SendNamed(BYTE* s, void*, const char* name, const DWORD* mat);
KING_VAR(Vec3, g_mkUp, 702630)
KING_VAR(Vec3, g_mkEye, 702650)
KING_VAR(float, g_mk64f3e0, 64f3e0)
KING_VAR(float, g_mk64c258, 64c258)
KING_VAR(float, g_mk64c408, 64c408)
KING_VAR(float, g_mk64c450, 64c450)
KING_VAR(double, g_mk64c958, 64c958)
KING_VAR(float, g_mk65063c, 65063c)
KING_VAR(float, g_mk650638, 650638)
KING_VAR(double, g_mk650630, 650630)
KING_VAR(double, g_mk64b678, 64b678)
KING_VAR(float, g_mk64c250, 64c250)
KING_VAR(double, g_mk650628, 650628)
KING_VAR(char, g_mk67da2c, 67da2c)
KING_VAR(char, g_mk67da28, 67da28)
KING_VAR(char, g_mk67da1c, 67da1c)
KING_VAR(char, g_mk67da10, 67da10)
KING_VAR(char, g_mk67da04, 67da04)
KING_VAR(char, g_mk67d9f8, 67d9f8)
KING_VAR(char, g_mk67d9e8, 67d9e8)

// the cross product a x b in the float steps of Vec3_SetThis
static Vec3 Mk_Cross(const Vec3& a, const Vec3& b)
{
    Vec3 r;
    r.x = (float)((double)a.y * b.z - (double)a.z * b.y);
    r.y = (float)((double)a.z * b.x - (double)a.x * b.z);
    r.z = (float)((double)a.x * b.y - (double)a.y * b.x);
    return r;
}

static Vec3 Mk_Norm(const Vec3& v)
{
    Vec3 r;
    return *Vec3_Normalized(&r, &v);
}

// the switch under the scene object named by node o's name (+44, +12) after its "xx" prefix
static BYTE* Mk_Prefixed(BYTE* o)
{
    BYTE* nd = F(BYTE*, o, 44);
    if (!nd || !F(char*, nd, 12) || strncmp(F(char*, nd, 12), &g_mk67da28, 2))
        return 0;
    char t[128], u[128];
    strcpy(t, F(char*, nd, 12));
    if ((int)strlen(t) <= 2)
        return 0;
    strcpy(u, t + 2);
    return CallC<BYTE*>(Mk_Find, 0, u);
}

extern "C" {

// [FUN_588090] item s hit (+1208: 1, 4-6 knocked over, 2 swung): the frame +1216 of object +1212 tilted away from the
// hit point (+1220) by the speed there (spin +440 and velocity +416, scale +396) as a share of the type's limit
// (+1416 keeps the largest), along the ground found under it; types 5 / 6 also mark the car (+1232) or the named
// objects and switch their models; when it falls over the object is flagged (+40) and messages with the share of
// the push sent; the new frame applied and sent as messages; whether it fell
int __fastcall Item_KnockOver(BYTE* s, void*)
{
    int type = F(int, s, 1208);
    if (type == 2)
    {
        Vec3 p0 = F(Vec3, s, 416);
        CallC(Mk_580a00, s, s + 964, s + 976);
        Vec3 d, v;
        Vec3_Scaled(&v, Vec3_Sub(&d, &p0, &F(Vec3, s, 416)), F(float, s, 396));
        CallC(Mk_SignSwing, &v, F(BYTE*, s, 1212), 0);
        return 0;
    }
    if (type != 1 && type != 4 && type != 5 && type != 6)
        return 0;
    BYTE* o = F(BYTE*, s, 1212);
    BYTE* fr = F(BYTE*, s, 1216);
    if (!fr)
        return 0;
    // the hit point's speed
    Vec3 P = F(Vec3, s, 1220);
    const float* T = (const float*)(s + 3220);
    Vec3 w;
    w.x = (float)((double)P.z * T[6] + (double)P.y * T[3] + (double)P.x * T[0] + T[9]);
    w.y = (float)((double)P.x * T[1] + (double)P.z * T[7] + (double)P.y * T[4] + T[10]);
    w.z = (float)((double)P.x * T[2] + (double)P.z * T[8] + (double)P.y * T[5] + T[11]);
    const float* sp = (const float*)(s + 440);
    Vec3 om;
    om.x = (float)((double)T[3] * sp[1] + (double)T[6] * sp[2] + (double)sp[0] * T[0]);
    om.y = (float)((double)T[4] * sp[1] + (double)T[1] * sp[0] + (double)T[7] * sp[2]);
    om.z = (float)((double)T[5] * sp[1] + (double)T[2] * sp[0] + (double)T[8] * sp[2]);
    Vec3 qv;
    const Vec3* q = Vec3_DivComponents(&qv, &om, &F(Vec3, s, 404));
    double c0 = (double)w.z * q->y - (double)w.y * q->z;
    double c1 = (double)w.x * q->z - (double)w.z * q->x;
    double c2 = (double)w.y * q->x - (double)w.x * q->y;
    const float* R = (const float*)(s + 500);
    float vx = (float)(c2 * R[6] + c1 * R[3] + c0 * R[0]);
    float vy = (float)(c2 * R[7] + c1 * R[4] + c0 * R[1]);
    double vz = c2 * R[8] + c1 * R[5] + c0 * R[2];
    float k = F(float, s, 396);
    Vec3 mv;
    mv.x = (float)((float)((double)vx + F(float, s, 416)) * (double)k);
    mv.y = (float)((float)((double)vy + F(float, s, 420)) * (double)k);
    mv.z = (float)((vz + F(float, s, 424)) * k);
    double spd0 = Vec3_Length(&mv);
    float spd = (float)spd0;
    float lim = (type == 5 || type == 6) ? 40000.0f : 20000.0f;
    double cl = spd0 > lim ? lim : spd0;
    if (!F(int, s, 1412))
        F(int, s, 1412) = type;
    if ((double)lim * F(float, s, 1416) < cl)
        F(float, s, 1416) = (float)(cl / lim);
    if (spd0 < g_mk64f3e0)
        return 0;
    Matrix A = g_identity, B = g_identity, C = g_identity;
    CallC(Mk_FrameLocal, fr, &A);
    BYTE* par = F(BYTE*, fr, 44);
    if (!par)
    {
        char buf[200];
        sprintf(buf, &g_mk67da2c, F(int, fr, 12), F(int, o, 12));
        CallC(Mk_Fatal, buf);
    }
    if (type == 5)
    {
        BYTE* car = CallC<BYTE*>(Mk_VehObject, F(int, s, 1232));
        if (car)
            car[296] |= 1;
    }
    if (type == 6)
    {
        char n1[168], n2[40];
        const char* nm = (const char*)(s + 1232);
        strcpy(n1, nm);
        strcpy(n2, nm + strlen(nm) + 1);
        BYTE* x = CallC<BYTE*>(Mk_Find, 0, n1);
        if (x)
            F(int, x, 136) = 0;
        x = CallC<BYTE*>(Mk_Find, 0, n2);
        if (x)
            x[40] |= 1;
    }
    if (type == 5 || type == 6)
    {
        BYTE* x = Mk_Prefixed(o);
        if (x)
        {
            BYTE* sel = CallC<BYTE*>(Mk_Find, x, type == 5 ? &g_mk67da1c : &g_mk67da10);
            CallC(Mk_Select, sel, 1);
        }
    }
    CallC(Mk_FrameWorld, par, &B);
    Matrix AB;
    C = *Matrix_Multiply(&AB, &A, &B);
    C.m[9] = g_mkEye.x;
    C.m[10] = g_mkEye.y;
    C.m[11] = g_mkEye.z;
    // the push's direction n, the up U and the way it falls d
    Vec3 n;
    n.x = (float)(mv.x / (double)spd);
    n.y = (float)(mv.y / (double)spd);
    n.z = (float)(mv.z / (double)spd);
    Vec3 U = g_mkUp;
    double a = (double)n.z * U.y - (double)n.y * U.z;
    double b = (double)U.z * n.x - (double)n.z * U.x;
    double c = (double)n.y * U.x - (double)U.y * n.x;
    Vec3 e;
    e.x = (float)(b * U.z - c * U.y);
    e.y = (float)(c * U.x - U.z * a);
    e.z = (float)(U.y * a - b * U.x);
    Vec3 d = Mk_Norm(e);
    void* qh = CallC<void*>(Mk_HitObject, F(BYTE*, s, 800), s + 536, s + 964);
    double tx = ((double)d.x - g_mkUp.x) * g_mk64c450;
    float ty = (float)((float)((double)d.y - g_mkUp.y) * (double)g_mk64c450);
    float tz = (float)((float)((double)d.z - g_mkUp.z) * (double)g_mk64c450);
    double a0 = (double)B.m[9] + (float)(g_mkUp.x * (double)g_mk64c408);
    float a1 = (float)((double)B.m[10] + (float)(g_mkUp.y * (double)g_mk64c408));
    float a2 = (float)((double)(float)(g_mkUp.z * (double)g_mk64c408) + B.m[11]);
    Vec3 from, to, hit;
    to.x = (float)(a0 + tx);
    to.y = (float)((double)ty + a1);
    to.z = (float)((double)tz + a2);
    from.x = (float)a0;
    from.y = a1;
    from.z = a2;
    hit.x = hit.y = hit.z = 0;
    if (!(CallC<int>(Mk_Query, qh, &from, &to, &hit, &U) & 1))
    {
        hit.x = (float)((double)B.m[9] + d.x);
        hit.y = (float)((double)B.m[10] + d.y);
        hit.z = (float)((double)d.z + B.m[11]);
    }
    else
    {
        // the ground's slope sets the up
        Vec3 r;
        r.x = (float)((double)hit.x - B.m[9]);
        r.y = (float)((double)hit.y - B.m[10]);
        r.z = (float)((double)hit.z - B.m[11]);
        U = Mk_Norm(Mk_Cross(Mk_Cross(r, U), r));
    }
    d = Mk_Norm(Mk_Cross(Mk_Cross(U, n), U));
    if (C.m[8] < g_mk64c258)
        return 0;
    Matrix I;
    Matrix_Inverse(&I, &C);
    Vec3 R3 = {C.m[6], C.m[7], C.m[8]};
    Vec3 gn = Mk_Norm(Mk_Cross(R3, d));
    Vec3 h;
    h.x = (float)((double)I.m[0] * gn.x + (double)I.m[6] * gn.z + (double)I.m[3] * gn.y);
    h.y = (float)((double)I.m[1] * gn.x + (double)I.m[7] * gn.z + (double)I.m[4] * gn.y);
    h.z = (float)((double)I.m[2] * gn.x + (double)I.m[8] * gn.z + (double)I.m[5] * gn.y);
    double ang = x87_acos((double)d.x * R3.x + (double)d.y * R3.y + (double)d.z * R3.z) * g_mk64c958;
    double lk = (double)lim - g_mk64f3e0;
    float roll = (float)(((double)spd - g_mk64f3e0) * g_mk65063c / lk);
    float push = (float)(lk * ang * g_mk650638);
    if (roll > ang)
        roll = (float)ang;
    Matrix L, M1, M2;
    Matrix_Multiply(&M1, Matrix_LookAt(&L, g_mkEye, h, roll), &A);
    Matrix_Multiply(&M2, &M1, &B);
    int fell = 0;
    bool over = (double)U.x * M2.m[6] + (double)U.y * M2.m[7] + (double)U.z * M2.m[8] < g_mk64c258 ||
                roll > g_mk650630 ||
                ((double)d.x * M2.m[6] + (double)d.y * M2.m[7] + (double)d.z * M2.m[8] < g_mk64b678 &&
                 (double)d.x * C.m[6] + (double)d.y * C.m[7] + (double)d.z * C.m[8] > g_mk64b678) ||
                M2.m[8] < g_mk64c258;
    if (over)
    {
        o[40] |= 1;
        if (spd > push)
        {
            double f = g_mk64c250 - push / (double)spd;
            float fr2 = (float)f;
            Vec3 vv;
            vv.x = (float)(f * F(float, s, 416));
            vv.y = (float)(f * F(float, s, 420));
            vv.z = (float)(f * F(float, s, 424));
            CallThis((void (*)())F(void**, s, 0)[36], s, &vv, &g_mk67da04);
            Vec3 sv;
            CallThis((void (*)())F(void**, s, 0)[37], s, Vec3_Scaled(&sv, &F(Vec3, s, 428), fr2), &g_mk67d9f8);
            fell = 1;
        }
        float yaw = (float)-x87_atan2(mv.x, mv.y);
        float dec[6];
        CallC(Mk_Decompose, &B, dec);
        Vec3 Bz = {B.m[6], B.m[7], B.m[8]};
        float ang2 = (float)(x87_acos(Vec3_Dot(&d, &Bz)) * g_mk650628);
        Matrix L2, Rz, X;
        Matrix_LookAt(&L2, g_mkEye, h, ang2);
        M1 = *Matrix_Multiply(&X, Matrix_RotationZ(&Rz, (float)((double)yaw - dec[3])), &L2);
        if (type == 4)
        {
            BYTE* x = Mk_Prefixed(o);
            if (x)
                CallC(Mk_Select, CallC<BYTE*>(Mk_Find, x, &g_mk67d9e8), 0);
        }
    }
    CallC(Mk_FrameApply, fr, 0, &M1);
    Movite_SendNamed(s, 0, F(const char*, fr, 12), (const DWORD*)&A);
    Movite_SendNamed(s, 0, F(const char*, fr, 12), (const DWORD*)&M1);
    return fell;
}

} // extern "C"

// === a sharp point's contact with an item (FUN_584820 with its tail $L_584a52)

extern "C" void Sh2_NodeInit(); KING_ALIAS("_Sh2_NodeInit", "@Node_Init@8")
extern "C" void Sh2_M1(); KING_ALIAS("_Sh2_M1", "@SceneObj_Matrix1@12")
extern "C" void Sh2_Sweep(); KING_ALIAS("_Sh2_Sweep", "@Shape_SweepHit@24")
extern "C" void Sh2_Free(); KING_ALIAS("_Sh2_Free", "@Value_Free@4")
extern "C" void Sh2_Query(); KING_ALIAS("_Sh2_Query", "_Query_4e570f")
extern "C" void Sh2_Fatal(); KING_ALIAS("_Sh2_Fatal", "_World_Fatal")
KING_VAR(double, g_sh2_64b748, 64b748)
KING_VAR(double, g_sh2_64b670, 64b670)
KING_VAR(float, g_sh2_64c250, 64c250)

// the contact array of c (+2816 its count): hit point (+0), the negated normal (+768), the depth along the
// normal (+2560), the fraction of the way (+2048, the root of the squared lengths) and the flags (+2820, +1536)
static int Sh2_Record(BYTE* c, int hit, const float* from, const float* to, const BYTE* node)
{
    int n = F(int, c, 2816);
    F(int, c, 2820 + 4 * n) = hit & 1;
    F(float, c, 2048 + 4 * n) = 1.0f;
    if (!F(int, c, 2820 + 4 * F(int, c, 2816)))
        return 0;
    n = F(int, c, 2816);
    F(int, c, 2048 + 4 * n) = 0;
    const float* h = &F(float, node, 52);
    const float* nr = &F(float, node, 64);
    const float* e = &F(float, node, 88);
    memcpy(c + 12 * n, h, 12);
    float neg[3] = {-nr[0], -nr[1], -nr[2]};
    memcpy(c + 12 * (n + 64), neg, 12);
    F(int, c, 1536 + 4 * n) = 0;
    float ex = (float)((double)e[0] - from[0]), ey = (float)((double)e[1] - from[1]);
    double ez = (double)e[2] - from[2];
    float hx = (float)((double)h[0] - from[0]), hy = (float)((double)h[1] - from[1]), hz = (float)((double)h[2] - from[2]);
    double dx = (double)h[0] - to[0], dy = (double)h[1] - to[1], dz = (double)h[2] - to[2];
    F(float, c, 2560 + 4 * n) = (float)(dy * nr[1] + dz * nr[2] + dx * nr[0]);
    float l1 = (float)((double)ex * ex + (double)ey * ey + ez * ez);
    float l2 = (float)((double)hx * hx + (double)hy * hy + (double)hz * hz);
    if (!(l2 > l1) && !(l1 < g_sh2_64b748))
    {
        double r = (double)l2 / l1;
        if (r > g_sh2_64b670)
            r = g_sh2_64c250;
        F(float, c, 2048 + 4 * F(int, c, 2816)) = (float)x87_sqrt(r);
    }
    F(int, c, 2816)++;
    return 1;
}

extern "C" {

// [FUN_584820..$L_584a52] the point moving from `from` to `to` against item b: a plain item's point (+3448 in its
// frame, radius +3284) swept by Shape_SweepHit, a body or a kind-3 item (+392, +3976) through the scene query; the
// result recorded in contacts c (at most 63)
int __cdecl Item_PointContact(const float* from, const float* to, BYTE* c, BYTE* b)
{
    __declspec(align(8)) BYTE node[208];
    CallThis(Sh2_NodeInit, node);
    int hit;
    int kind = F(int, b, 392);
    if (kind != 1 && !(kind == 2 && F(int, b, 3976) == 3))
    {
        float* o = (float*)operator new(16);
        if (o)
        {
            o[0] = 0;
            o[1] = 0;
            o[2] = 0;
            o[3] = 0;
        }
        Matrix t;
        const float* m = CallThis<const Matrix*>(Sh2_M1, b, &t)->m;
        const float* p = &F(float, b, 3448);
        double x = (double)m[3] * p[1] + (double)m[6] * p[2] + (double)p[0] * m[0] + m[9];
        float y = (float)((double)m[4] * p[1] + (double)m[1] * p[0] + (double)m[7] * p[2] + m[10]);
        float z = (float)((double)m[5] * p[1] + (double)m[2] * p[0] + (double)m[8] * p[2] + m[11]);
        o[0] = (float)x;
        o[1] = y;
        o[2] = z;
        F(int, o, 12) = F(int, b, 3284);
        int val[3] = {1, 0, (int)o};
        float f[3] = {from[0], from[1], from[2]};
        float g[3] = {to[0], to[1], to[2]};
        hit = CallThis<int>(Sh2_Sweep, val, 0, f, g, 0) ? 1 : 0;
        memcpy(node + 52, f, 12);
        memcpy(node + 64, g, 12);
        F(int, node, 116) = 0;
        CallThis(Sh2_Free, val);
    }
    else
        hit = CallC<int>(Sh2_Query, b ? b + 256 : 0, from, to, node);
    if (F(int, c, 2816) >= 63)
        CallC(Sh2_Fatal, "try_SharpPoint_col_Node_arr(): arr.cnt>=ARRCNT-1");
    return Sh2_Record(c, hit, from, to, node);
}

} // extern "C"

// === a sharp sphere's contact with an item (FUN_584c70 with its tail $L_584eb2)

extern "C" void Sh2_Query2(); KING_ALIAS("_Sh2_Query2", "_Query_4e774a")

// the sphere's contact: as Sh2_Record, the point pushed out along the normal by the radius, the flag times sign and
// the fraction negated when the contact lies behind
static int Sh2_RecordSphere(BYTE* c, int hit, int sign, float rad, const float* from, const float* to,
                            const BYTE* node)
{
    int n = F(int, c, 2816);
    F(int, c, 2820 + 4 * n) = (hit & 1) * sign;
    F(float, c, 2048 + 4 * n) = 1.0f;
    if (!F(int, c, 2820 + 4 * F(int, c, 2816)))
        return 0;
    n = F(int, c, 2816);
    F(int, c, 2048 + 4 * n) = 0;
    const float* h = &F(float, node, 52);
    const float* nr = &F(float, node, 64);
    const float* e = &F(float, node, 88);
    memcpy(c + 12 * n, h, 12);
    float neg[3] = {-nr[0], -nr[1], -nr[2]};
    memcpy(c + 12 * (n + 64), neg, 12);
    F(int, c, 1536 + 4 * n) = 0;
    double sx = (double)nr[0] * rad;
    float sy = (float)((double)nr[1] * rad), sz = (float)((double)nr[2] * rad);
    float q[3];
    q[0] = (float)(h[0] + sx);
    q[1] = (float)((double)sy + h[1]);
    q[2] = (float)((double)sz + h[2]);
    float ex = (float)((double)e[0] - from[0]), ey = (float)((double)e[1] - from[1]);
    double ez = (double)e[2] - from[2];
    float hx = (float)((double)q[0] - from[0]), hy = (float)((double)q[1] - from[1]), hz = (float)((double)q[2] - from[2]);
    double dx = (double)q[0] - to[0], dy = (double)q[1] - to[1], dz = (double)q[2] - to[2];
    F(float, c, 2560 + 4 * F(int, c, 2816)) = (float)(dy * nr[1] + dz * nr[2] + dx * nr[0]);
    float l1 = (float)((double)ex * ex + (double)ey * ey + ez * ez);
    float l2 = (float)((double)hx * hx + (double)hy * hy + (double)hz * hz);
    if (!(l2 > l1) && !(l1 < g_sh2_64b748))
    {
        double r = (double)l2 / l1;
        if (r > g_sh2_64b670)
            r = g_sh2_64c250;
        int k = F(int, c, 2816);
        F(float, c, 2048 + 4 * k) = (float)x87_sqrt(r);
        if (F(float, c, 2560 + 4 * k) < 0.0f)
            F(float, c, 2048 + 4 * k) = -F(float, c, 2048 + 4 * k);
    }
    F(int, c, 2816)++;
    return 1;
}

extern "C" {

// [FUN_584c70..$L_584eb2] the sphere of radius rad moving from `from` to `to` against item b, as
// Item_PointContact, recorded in contacts c with sign
int __cdecl Item_SphereContact(const float* from, const float* to, float rad, BYTE* c, BYTE* b, int sign)
{
    __declspec(align(8)) BYTE node[208];
    CallThis(Sh2_NodeInit, node);
    int hit;
    int kind = F(int, b, 392);
    if (kind != 1 && !(kind == 2 && F(int, b, 3976) == 3))
    {
        float* o = (float*)operator new(16);
        if (o)
        {
            o[0] = 0;
            o[1] = 0;
            o[2] = 0;
            o[3] = 0;
        }
        Matrix t;
        const float* m = CallThis<const Matrix*>(Sh2_M1, b, &t)->m;
        const float* p = &F(float, b, 3448);
        double x = (double)m[3] * p[1] + (double)m[6] * p[2] + (double)m[0] * p[0] + m[9];
        float y = (float)((double)m[4] * p[1] + (double)m[7] * p[2] + (double)m[1] * p[0] + m[10]);
        float z = (float)((double)m[5] * p[1] + (double)m[8] * p[2] + (double)m[2] * p[0] + m[11]);
        o[0] = (float)x;
        o[1] = y;
        o[2] = z;
        F(int, o, 12) = F(int, b, 3284);
        int val[3] = {1, 0, (int)o};
        float f[3] = {from[0], from[1], from[2]};
        float g[3] = {to[0], to[1], to[2]};
        hit = CallThis<int>(Sh2_Sweep, val, 2, f, g, rad) ? 1 : 0;
        memcpy(node + 52, f, 12);
        memcpy(node + 64, g, 12);
        F(int, node, 116) = 0;
        CallThis(Sh2_Free, val);
    }
    else
        hit = CallC<int>(Sh2_Query2, b ? b + 256 : 0, rad, from, to, node);
    if (F(int, c, 2816) >= 63)
        CallC(Sh2_Fatal, "try_SharpSphere_col_Node_arr(): arr.cnt>=ARRCNT-1");
    return Sh2_RecordSphere(c, hit, sign, rad, from, to, node);
}

} // extern "C"

#include "x87.h"

// === a moving item's frame: carried, net-driven or free (FUN_586df0 with its tail $L_58738c)

extern "C" void Mf_Local(); KING_ALIAS("_Mf_Local", "@Node_GetLocal@12")
extern "C" void Mf_Root(); KING_ALIAS("_Mf_Root", "@Node_Root@8")
extern "C" void Mf_Hit(); KING_ALIAS("_Mf_Hit", "_Query_HitObject")
extern "C" void Mf_Log(); KING_ALIAS("_Mf_Log", "_VehLog_Value")
extern "C" void Mf_Decompose(); KING_ALIAS("_Mf_Decompose", "_Matrix_Decompose6F")
extern "C" void Mf_Pose(); KING_ALIAS("_Mf_Pose", "_Matrix_FromPose")
extern "C" void Mf_MulAssign(); KING_ALIAS("_Mf_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Mf_MakeOut(); KING_ALIAS("_Mf_MakeOut", "@Vehicle_MakeOut@8")
extern "C" void Mf_Post(); KING_ALIAS("_Mf_Post", "@Queue_PostEvent@12")
extern "C" void Mf_FrameTime(); KING_ALIAS("_Mf_FrameTime", "_Game_FrameTime")
KING_VAR(BYTE*, g_mf6cec6c, 6cec6c)
KING_VAR(float, g_mf67d114, 67d114)
KING_VAR(float, g_mf64da74, 64da74)
KING_VAR(float, g_mf64c250, 64c250)
KING_VAR(float, g_mf64c264, 64c264)
KING_VAR(float, g_mf64c450, 64c450)
KING_VAR(float, g_mf64d2f8, 64d2f8)
KING_VAR(double, g_mf64b5a0, 64b5a0)
KING_VAR(double, g_mf64b780, 64b780)
KING_VAR(double, g_mf64b828, 64b828)
KING_VAR(double, g_mf64b820, 64b820)
KING_VAR(double, g_mf64b818, 64b818)
KING_VAR(double, g_mf64b668, 64b668)
KING_VAR(double, g_mf64b6a0, 64b6a0)
KING_VAR(double, g_mf64b670, 64b670)
KING_VAR(char, g_mfStart, 67d960)
KING_VAR(char, g_mfEnd, 67d954)
KING_VAR(char, g_mfPos, 67d138)
KING_VAR(char, g_mfMine, 67d11c)

static double Mf_Dt()
{
    return CallC<double>(Mf_FrameTime);
}

// the logged positions of item s: its node's (m) and its own (+536)
static void Mf_LogPos(BYTE* s, const char* tag, const float* m)
{
    CallC(Mf_Log, tag);
    char buf[100];
    const char* name = F(const char*, F(BYTE*, s, 320), 12);
    sprintf(buf, &g_mfPos, name, (double)m[9], (double)m[10], (double)m[11]);
    CallC(Mf_Log, buf);
    sprintf(buf, &g_mfMine, name, (double)F(float, s, 536), (double)F(float, s, 540), (double)F(float, s, 544));
    CallC(Mf_Log, buf);
}

// an angle a moved by a whole turn when it is more than half a turn away from b
static void Mf_Wrap(float* a, float b)
{
    double d = (double)*a - b;
    if (!(x87_fabs(d) > g_mf64b828))
        return;
    if (d > g_mf64b828)
        *a = (float)(*a - g_mf64b820);
    if ((double)*a - b < g_mf64b818)
        *a = (float)(*a + g_mf64b820);
}

extern "C" {

// [FUN_586df0..$L_58738c] moving item s: its frame from its node (+740 times the node's) and the scene frame
// (+692 times +548); in net play (6cec6c +1300 > 1) the node's frame taken; else when carried (flag 4 of +1196,
// a close carrier among +3508) eased towards the carrier's speed and blended into its pose, the move and angles
// recorded (+3148, +3160) and its own step (vtable +80) run, or its frame taken with its speed and spin slowed;
// then its hit object, outputs and event 1; logged when 696c6c
void __fastcall MovingItem_Frame(BYTE* s)
{
    F(int, s, 3556) = 2;
    CallVirt(s, 76);
    float dt = (float)Mf_Dt();
    Matrix M, A, B;
    memcpy(&M, &g_identity, 48);
    CallThis(Mf_Local, F(BYTE*, F(BYTE*, s, 32), 68), &M);
    Matrix_Multiply(&A, (const Matrix*)(s + 740), &M);
    Matrix_Multiply(&B, (const Matrix*)(s + 692), (const Matrix*)(s + 548));
    if (g_ra696c6c)
        Mf_LogPos(s, &g_mfStart, M.m);
    float tmp[3];
    bool takeFrame = true;
    if (F(int, g_mf6cec6c, 1300) > 1)
    {
        memcpy(s + 500, &A, 48);
        Matrix I;
        const float* i = (const float*)memcpy(s + 596, Matrix_Inverse(&I, (const Matrix*)(s + 500)), 48);
        const float* w = &F(float, s, 440);
        float v[3] = {(float)((double)i[3] * w[1] + (double)i[6] * w[2] + (double)i[0] * w[0]),
                      (float)((double)i[4] * w[1] + (double)i[7] * w[2] + (double)i[1] * w[0]),
                      (float)((double)i[5] * w[1] + (double)i[8] * w[2] + (double)i[2] * w[0])};
        Vec3 d;
        memcpy(s + 428, Vec3_DivComponents(&d, (const Vec3*)v, (const Vec3*)(s + 404)), 12);
        BYTE* r = CallThis<BYTE*>(Mf_Root, s + 256);
        F(int, s, 800) = CallC<int>(Mf_Hit, r, &M.m[9], s + 536);
        goto outputs;
    }
    {
        float P[3];
        bool carried = false;
        if (F(BYTE, s, 1196) & 4)
        {
            int i;
            for (i = 0; i < 6; i++)
            {
                BYTE* o = F(BYTE*, s, 3508 + 4 * i);
                if (o && F(int, o, 1204) && F(float, s, 3532 + 4 * i) < g_mf64da74)
                    break;
            }
            BYTE* o = i < 6 ? F(BYTE*, s, 3508 + 4 * i) : 0;
            if (o && F(int, o, 1204))
            {
                carried = true;
                float dv[3] = {(float)((double)dt * F(float, s, 416)), (float)((double)dt * F(float, s, 420)),
                               (float)((double)dt * F(float, s, 424))};
                const float* bp = &B.m[9];
                float d[3] = {(float)((double)F(float, s, 852) - bp[0]), (float)((double)F(float, s, 856) - bp[1]),
                              (float)((double)F(float, s, 860) - bp[2])};
                float len = (float)Vec3_Length((const Vec3*)d);
                if (len > g_mf64b5a0)
                {
                    double r = Vec3_Length((const Vec3*)dv) / len * g_mf67d114;
                    r = r / (r + g_mf64c250);
                    double q = g_mf64c250 - r;
                    float q0 = (float)(d[0] * q), q1 = (float)(d[1] * q);
                    double q2 = q * d[2];
                    float t0 = (float)((double)q0 * dt), t1 = (float)((double)q1 * dt);
                    double t2 = q2 * dt;
                    float k0 = (float)((double)t0 * g_mf64c450), k1 = (float)((double)t1 * g_mf64c450);
                    float k2 = (float)(t2 * g_mf64c450);
                    float r0 = (float)(dv[0] * r), r1 = (float)(dv[1] * r);
                    double r2 = r * dv[2];
                    float u0 = (float)((double)r0 + F(float, s, 584)), u1 = (float)((double)r1 + F(float, s, 588));
                    double u2 = r2 + F(float, s, 592);
                    P[0] = (float)((double)u0 + k0);
                    P[1] = (float)((double)k1 + u1);
                    P[2] = (float)(k2 + u2);
                }
                else
                {
                    P[0] = (float)((double)dv[0] + F(float, s, 584));
                    P[1] = (float)((double)dv[1] + F(float, s, 588));
                    P[2] = (float)((double)dv[2] + F(float, s, 592));
                }
                float a[6] = {0}, b[3] = {0};
                CallC(Mf_Decompose, &B, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
                CallC(Mf_Decompose, s + 816, &a[0], &a[1], &a[2], &b[0], &b[1], &b[2]);
                float p = (float)x87_pow(0.6, Mf_Dt() * g_mf64b780);
                Mf_Wrap(&b[0], a[3]);
                Mf_Wrap(&b[1], a[4]);
                Mf_Wrap(&b[2], a[5]);
                double qq = (double)g_mf64c250 - p;
                float b0 = (float)(qq * b[0]), b1 = (float)(qq * b[1]);
                double b2 = qq * b[2];
                float c1 = (float)((double)a[4] * p), c2 = (float)((double)a[5] * p);
                float y0 = (float)((double)a[3] * p + b0);
                float y1 = (float)((double)b1 + c1);
                double y2 = b2 + c2;
                CallC(Mf_Pose, &A, (double)P[0], (double)P[1], (double)P[2], (double)y0, (double)y1, y2);
            }
        }
        if (F(DWORD, s, 1196) & 4)
        {
            int i;
            for (i = 0; i < 6; i++)
            {
                BYTE* o = F(BYTE*, s, 3508 + 4 * i);
                if (!o || !F(int, o, 1204))
                    continue;
                double sq = x87_sqrt(F(float, s, 3532 + 4 * i));
                double lim = (((double)F(float, o, 3460) + F(float, s, 3460)) * dt + F(float, o, 3272) +
                              F(float, s, 3272)) * g_mf64b668;
                if (lim <= sq)
                    continue;
                double dx = (double)P[0] - F(float, s, 584);
                double dy = (double)P[1] - F(float, s, 588);
                double dz = (double)P[2] - F(float, s, 592);
                if (dx * dx + dy * dy + dz * dz <= g_mf64b6a0)
                    break;
            }
            (void)carried;
            if (i < 6)
            {
                takeFrame = false;
                F(float, s, 3148) = (float)((double)P[0] - F(float, s, 584));
                F(float, s, 3152) = (float)((double)P[1] - F(float, s, 588));
                F(float, s, 3156) = (float)((double)P[2] - F(float, s, 592));
                Matrix C;
                Matrix_Multiply(&C, &A, (const Matrix*)(s + 644));
                float t[3] = {0, 0, 0};
                CallC(Mf_Decompose, &C, &t[0], &t[1], &t[2], s + 3168, s + 3160, s + 3164);
                F(float, s, 3160) = -F(float, s, 3160);
                CallVirt(s, 80, (float)Mf_Dt(), 0);
            }
        }
        if (takeFrame)
        {
            memcpy(s + 500, &A, 48);
            Matrix I;
            const float* i = (const float*)memcpy(s + 596, Matrix_Inverse(&I, (const Matrix*)(s + 500)), 48);
            const float* w = &F(float, s, 440);
            float v[3] = {(float)((double)i[3] * w[1] + (double)i[6] * w[2] + (double)i[0] * w[0]),
                          (float)((double)i[4] * w[1] + (double)i[1] * w[0] + (double)i[7] * w[2]),
                          (float)((double)i[5] * w[1] + (double)i[2] * w[0] + (double)i[8] * w[2])};
            Vec3 d;
            memcpy(s + 428, Vec3_DivComponents(&d, (const Vec3*)v, (const Vec3*)(s + 404)), 12);
            BYTE* r = CallThis<BYTE*>(Mf_Root, s + 256);
            F(int, s, 800) = CallC<int>(Mf_Hit, r, &M.m[9], s + 536);
            F(int, s, 892) = 0;
            F(int, s, 896) = 0;
            F(int, s, 3464) = 0;
            if (F(BYTE, s, 1196) & 4)
                CallVirt(s, 60);
        }
        if (F(double, g_ra6ef1fc, 2968) - F(float, s, 808) > g_mf64b670)
        {
            float* v = &F(float, s, 416);
            float* w = &F(float, s, 428);
            float lv = (float)Vec3_Length((const Vec3*)v);
            float lw = (float)Vec3_Length((const Vec3*)w);
            double nv = lv - dt * g_mf64d2f8;
            float nw = (float)((double)lw - dt);
            if (nv < g_mf64c264)
                nv = g_mf64c264;
            if (nw < g_mf64c264)
                nw = 0;
            if (lv > g_mf64c264)
                CallThis(Mf_MulAssign, v, tmp, (float)(nv / lv));
            if (lw > g_mf64c264)
                CallThis(Mf_MulAssign, w, tmp, (float)((double)nw / lw));
            Vec3 q;
            const float* e = &Vec3_MulComponents(&q, (const Vec3*)w, (const Vec3*)(s + 404))->x;
            F(float, s, 440) = (float)((double)F(float, s, 524) * e[2] + (double)F(float, s, 512) * e[1] +
                                       (double)e[0] * F(float, s, 500));
            F(float, s, 444) = (float)((double)F(float, s, 528) * e[2] + (double)F(float, s, 504) * e[0] +
                                       (double)F(float, s, 516) * e[1]);
            F(float, s, 448) = (float)((double)F(float, s, 532) * e[2] + (double)F(float, s, 508) * e[0] +
                                       (double)F(float, s, 520) * e[1]);
        }
    }
outputs:
    CallVirt(s, 52);
    CallThis(Mf_MakeOut, s);
    CallThis(Mf_Post, s, 1);
    if (g_ra696c6c)
    {
        CallThis(Mf_Local, F(BYTE*, F(BYTE*, s, 32), 68), &M);
        Mf_LogPos(s, &g_mfEnd, M.m);
    }
    F(int, s, 3556) = 0;
}

} // extern "C"

// === a moving item's physics step ($L_582c30 with its pieces up to $L_583c18)

extern "C" void __cdecl Mx_Check(const void* v, const char* name, float lim); KING_ALIAS("_Mx_Check", "_World_CheckPoint")
extern "C" double __cdecl Mx_Time(); KING_ALIAS("_Mx_Time", "_Game_Time")
extern "C" void __cdecl Mx_Relative(const BYTE* a, const BYTE* b, const BYTE* m, float* offset, float* angles); KING_ALIAS("_Mx_Relative", "_SceneObj_Relative")
extern "C" int __cdecl Mx_5820c0(const BYTE* o, const BYTE* m, float* a, void* b); KING_ALIAS("_Mx_5820c0", "_SceneObj_5820c0")
extern "C" void __cdecl Mx_HoldOnRail(BYTE* o, const void* r, void* out); KING_ALIAS("_Mx_HoldOnRail", "_Obj_HoldOnRail")
extern "C" void __cdecl Mx_Surface(BYTE* b, BYTE* c, const void* m); KING_ALIAS("_Mx_Surface", "_Body_SurfaceResponse")
extern "C" void __cdecl Mx_Contact(BYTE* s, const void* a2, const void* a3); KING_ALIAS("_Mx_Contact", "_MovingItem_Contact")
extern "C" int __fastcall Mx_KnockOver(BYTE* s, void*); KING_ALIAS("@Mx_KnockOver@8", "@Item_KnockOver@8")
extern "C" BYTE* __cdecl Mx_MovingOf(BYTE* o); KING_ALIAS("_Mx_MovingOf", "_Moving_Of")
extern "C" BYTE* __cdecl Mx_Hit(BYTE* obj, const void* p, const void* d); KING_ALIAS("_Mx_Hit", "_Query_HitObject")
extern "C" int __cdecl Mx_Out(const void* v, float lim); KING_ALIAS("_Mx_Out", "_Vec3_OutOfBox")
extern "C" void __cdecl Mx_Fatal(const char* msg); KING_ALIAS("_Mx_Fatal", "_World_Fatal")
extern "C" void Mx_AddA(); KING_ALIAS("_Mx_AddA", "@Vec3_AddAssign@16")
extern "C" void Mx_MulA(); KING_ALIAS("_Mx_MulA", "@Vec3_MulAssign@16")
extern "C" void Mx_580a00(); KING_ALIAS("_Mx_580a00", "_MovingItem_PointBounce")
extern "C" void Mx_581990(); KING_ALIAS("_Mx_581990", "_MovingItem_BodyContact")
KING_VAR(char, g_mx67d788, 67d788)
KING_VAR(char, g_mx67d76c, 67d76c)
KING_VAR(char, g_mx67d74c, 67d74c)
KING_VAR(char, g_mx67d6f4, 67d6f4)
KING_VAR(char, g_mx67d6d8, 67d6d8)
KING_VAR(char, g_mx67d6c0, 67d6c0)
KING_VAR(char, g_mx67d68c, 67d68c)
KING_VAR(char, g_mx67d670, 67d670)
KING_VAR(char, g_mx67d650, 67d650)
KING_VAR(char, g_mx67d640, 67d640)
KING_VAR(char, g_mx67d630, 67d630)
KING_VAR(double, g_mx64b7a8, 64b7a8)
KING_VAR(float, g_mx64c250, 64c250)
KING_VAR(float, g_mx64c264, 64c264)
KING_VAR(double, g_mx64b5a0, 64b5a0)
KING_VAR(double, g_mx64b658, 64b658)
KING_VAR(double, g_mx64ed98, 64ed98)
KING_VAR(double, g_mx64b6e0, 64b6e0)
KING_VAR(double, g_mx64b670, 64b670)
KING_VAR(double, g_mx64d318, 64d318)
KING_VAR(double, g_mx64b5b8, 64b5b8)
KING_VAR(float, g_mx64c304, 64c304)
KING_VAR(int, g_mx702650, 702650)
KING_VAR(int, g_mx702654, 702654)
KING_VAR(int, g_mx702658, 702658)

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))
#define D(n) (*(double*)(fr + (n)))
#define SV(n) F(float, s, n)

static Vec3 Mx_Vec(float x, float y, float z)
{
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static Vec3 Mx_Zero()
{
    Vec3 v;
    *(int*)&v.x = g_mx702650;
    *(int*)&v.y = g_mx702654;
    *(int*)&v.z = g_mx702658;
    return v;
}

extern "C" {

// [$L_582c30..$L_583c18] (moving item vtable) step of item s with flags fl and time step k: the history slot of this
// step (+1432), its pose moved by its speed and spin (with a rail +1676: rotated about +3160) or relative to its
// carrier (+1696), its ground contact (5820c0) pushing it out of the ground along the slope, speeds damped by
// 1 - k; then held on a rail (fl 4), the surface response, a knock over, a contact with a moving body, or the step
// back along the history while the body is in front. The asm's frame is kept as fr (V / I / D slots)
void __fastcall MovingItem_Step(BYTE* s, void*, int fl, float k)
{
    BYTE fr[2152];
    {
        int idx = F(int, s, 1432);
        memcpy(s + (idx + 170) * 12, s + 428, 12);
        memcpy(s + idx * 208 + 2104, s + 1464, 208);
    }
    Mx_Check(s + 1516, &g_mx67d788, 10000.0f);
    Mx_Check(s + 1528, &g_mx67d76c, 2.0f);
    if (F(int, s, 1672))
        CallVirt(s, 160, s + 1464);
    if (!F(int, s, 1028))
    {
        if (!(SV(984) <= g_mx64b7a8) && !(F(BYTE, s, 892) & 4))
            F(int, s, 1676) = 1;
        BYTE* c = F(BYTE*, s, 1012);
        if (!F(int, s, 1028) && c && F(int, c, 0) == 1 && !(F(BYTE, s, 892) & 4))
        {
            memcpy(s + 1176, s + 976, 12);
            memcpy(s + 1164, s + 964, 12);
        }
    }
    double damp = g_mx64c250 - k;
    F(float, s, 1456) = k;
    V(100) = (float)damp;
    SV(1452) = (float)(damp * SV(1452));
    memcpy(s + 1120, s + 976, 12);
    memcpy(s + 912, s + 1464, 208);
    F(DWORD, s, 892) |= 1;
    double t = Mx_Time();
    F(DWORD, s, 3464) |= 1;
    SV(904) = (float)t;
    F(int, s, 1692) = 0;
    memcpy(fr + 120, s + 3148, 12);
    memcpy(fr + 108, s + 3160, 12);
    I(144) = F(int, s, 1444);
    const BYTE* res;
    if (F(int, s, 1676))
    {
        SV(1444) = (float)Vec3_Length((const Vec3*)(s + 3160));
        V(52) = k * SV(3148);
        V(56) = k * SV(3152);
        V(60) = k * SV(3156);
        Matrix* T = Matrix_Translation((Matrix*)(fr + 328), Mx_Vec(V(52), V(56), V(60)));
        float ang = k * SV(1444);
        Matrix* R = Matrix_LookAt((Matrix*)(fr + 280), Mx_Zero(), *(const Vec3*)(s + 3160), ang);
        Matrix* A = Matrix_Multiply((Matrix*)(fr + 232), R, (const Matrix*)(s + 1888));
        res = (const BYTE*)Matrix_Multiply((Matrix*)(fr + 184), A, T);
    }
    else
    {
        Mx_Relative(s + 1696, s + 548, s + 644, &V(120), &V(108));
        V(64) = (float)Vec3_Length((const Vec3*)&V(108));
        V(52) = V(120) * k;
        V(56) = V(124) * k;
        V(60) = V(128) * k;
        Matrix* T = Matrix_Translation((Matrix*)(fr + 184), Mx_Vec(V(52), V(56), V(60)));
        float ang = V(64) * k;
        Matrix* R = Matrix_LookAt((Matrix*)(fr + 232), Mx_Zero(), *(const Vec3*)&V(108), ang);
        Matrix* A = Matrix_Multiply((Matrix*)(fr + 280), R, (const Matrix*)(s + 548));
        res = (const BYTE*)Matrix_Multiply((Matrix*)(fr + 328), A, T);
    }
    memcpy(s + 1792, res, 48);
    I(104) = (int)Mx_MovingOf(F(BYTE*, s, 1028));
    I(64) = Mx_5820c0(s, s + 1792, &V(96), fr + 376);
    Mx_Check(s + 1828, &g_mx67d74c, 10000.0f);
    if (I(64))
    {
        V(36) = (float)(g_mx64b5a0 - V(96));
        if (F(int, s, 1676))
            V(36) = (float)(g_mx64b658 - V(96));
        I(64) = I(36);
        memcpy(fr + 24, s + 976, 12);
        CallVirt(s, 220, fr + 40);
        double d = (V(24) * V(40) + V(28) * V(44)) + V(32) * V(48);
        double c1 = V(32) * V(44) - V(28) * V(48);
        double c2 = V(24) * V(48) - V(32) * V(40);
        double c3 = V(28) * V(40) - V(24) * V(44);
        V(80) = (float)(c2 * V(48) - c3 * V(44));
        I(52) = I(80);
        V(84) = (float)(c3 * V(40) - c1 * V(48));
        I(56) = I(84);
        V(88) = (float)(c1 * V(44) - c2 * V(40));
        I(60) = I(88);
        D(80) = d;
        if (d < g_mx64ed98)
            I(36) = 0;
        else if (d < g_mx64c264)
        {
            double r = g_mx64c250 / Vec3_Length((const Vec3*)&V(52));
            V(12) = (float)(V(52) * r);
            I(24) = I(12);
            V(16) = (float)(V(56) * r);
            I(28) = I(16);
            V(20) = (float)(V(60) * r);
            I(32) = I(20);
            double q = r * V(36);
            V(64) = (float)q;
            V(36) = (float)q;
            if (q > g_mx64b6e0)
                V(36) = 0.5f;
        }
        V(12) = V(24) * V(36);
        I(68) = I(12);
        V(16) = V(28) * V(36);
        I(72) = I(16);
        V(20) = V(32) * V(36);
        I(76) = I(20);
        CallThis(Mx_AddA, s + 1828, fr + 132, fr + 68);
        if (Mx_Out(s + 1828, 10000.0f))
        {
            char* a = (char*)fr + 656;
            char* b = (char*)fr + 908;
            char* c = (char*)fr + 456;
            char* dd = (char*)fr + 1160;
            sprintf(a, &g_mx67d6f4, 0.001, 0.01, (double)V(96), (double)V(36), D(80), (double)V(64), (double)V(24),
                    (double)V(28), (double)V(32), (double)V(40), (double)V(44), (double)V(48));
            double r = g_mx64c250 / Vec3_Length((const Vec3*)&V(52));
            sprintf(b, &g_mx67d6d8, (double)V(52), (double)V(56), (double)V(60), r);
            sprintf(c, &g_mx67d6c0, F(const char*, F(BYTE*, s, 320), 12), (double)SV(396), (double)SV(584),
                    (double)SV(588), (double)SV(592));
            sprintf(dd, &g_mx67d68c, (double)SV(1828), (double)SV(1832), (double)SV(1836), a, b, c);
            Mx_Fatal(dd);
        }
    }

    // $L_5833f4: the new pose stored, its inverse, the object under it
    memcpy(s + 3148, fr + 120, 12);
    memcpy(s + 3160, fr + 108, 12);
    F(int, s, 1444) = I(144);
    memcpy(s + 1840, Matrix_Inverse((Matrix*)(fr + 184), (const Matrix*)(s + 1792)), 48);
    F(BYTE*, s, 2032) = Mx_Hit(F(BYTE*, s, 2036), s + 1924, s + 1828);
    if (!F(BYTE*, s, 2032))
    {
        F(BYTE*, s, 2032) = F(BYTE*, s, 2036);
        CallVirt(s, 28, 1);
    }
    {
        double r = V(100);
        V(68) = (float)(r * SV(476));
        V(72) = (float)(r * SV(480));
        double z = r * SV(484);
        V(12) = (float)(SV(416) - V(68));
        I(160) = I(12);
        V(16) = (float)(SV(420) - V(72));
        I(164) = I(16);
        V(20) = (float)(SV(424) - z);
        I(168) = I(20);
        r = V(100);
        V(68) = (float)(r * SV(488));
        V(72) = (float)(r * SV(492));
        z = r * SV(496);
        V(12) = (float)(SV(428) - V(68));
        I(148) = I(12);
        V(16) = (float)(SV(432) - V(72));
        I(152) = I(16);
        V(20) = (float)(SV(436) - z);
        I(156) = I(20);
    }
    CallThis(Mx_MulA, s + 476, fr + 132, k);
    CallThis(Mx_MulA, s + 488, fr + 132, k);
    CallVirt(s, 144, fr + 160, &g_mx67d670);
    CallVirt(s, 148, fr + 148, &g_mx67d650);
    if (F(int, s, 1432) > F(int, s, 1680))
        return;
    int has = F(int, s, 1672);
    if (has > 0)
    {
        memcpy(s + 3172, s + 1792, 48);
        memcpy(s + 3220, s + 1840, 48);
    }
    else
    {
        memcpy(s + 3172, s + 548, 48);
        memcpy(s + 3220, s + 644, 48);
    }
    memcpy(s + 2020, s + 416, 12);
    if (fl & 4)
    {
        Mx_HoldOnRail(s, s + 912, s + 500);
        return;
    }
    if (F(int, s, 392) == 1 && F(int, s, 1028) == 0)
    {
        Mx_Surface(s, s + 912, s + 500);
        return;
    }

    // $L_58369d
    if (I(104))
    {
        if (F(int, s, 1688))
            CallC(Mx_581990, s, s + 912, has, F(int, s, 1456));
        F(DWORD, s, 3464) |= 8;
        return;
    }
    if (F(BYTE, s, 892) & 4)
    {
        F(DWORD, s, 3464) |= 4;
        if (!Mx_KnockOver(s, 0))
            CallC(Mx_580a00, s, s + 964, s + 976);
        F(int, s, 1692) = 1;
        F(DWORD, s, 892) &= ~4u;
        return;
    }

    // $L_583700: a contact with the body under it
    F(DWORD, s, 3464) |= 16;
    V(12) = SV(964) - SV(3208);
    I(40) = I(12);
    V(16) = SV(968) - SV(3212);
    I(44) = I(16);
    V(20) = SV(972) - SV(3216);
    I(48) = I(20);
    Vec3_Normalized((Vec3*)(fr + 172), (const Vec3*)&V(40));
    {
        double a = (SV(3184) * SV(432) + SV(3196) * SV(436)) + SV(428) * SV(3172);
        double b = (SV(3188) * SV(432) + SV(3200) * SV(436)) + SV(3176) * SV(428);
        double c = (SV(3192) * SV(432) + SV(3204) * SV(436)) + SV(3180) * SV(428);
        V(68) = (float)(V(48) * b - V(44) * c);
        V(72) = (float)(c * V(40) - V(48) * a);
        double z = a * V(44) - b * V(40);
        V(12) = V(68) + SV(416);
        V(16) = V(72) + SV(420);
        double zz = z + SV(424);
        V(104) = (float)(((V(12) * V(40)) + zz * V(48)) + V(16) * V(44));
    }
    if (!(fl & 8) && !(F(DWORD, s, 1672) & 0x4000000) && !(Mx_Time() - SV(3144) < g_mx64b670)
        && !(SV(984) < g_mx64d318) && !F(int, s, 1420) && V(104) <= g_mx64c264)
    {
        Mx_Contact(s, s + 964, s + 976);
        return;
    }
    CallC(Mx_580a00, s, s + 964, s + 976);

    // $L_5838cb: back along the history to the last step in front of a body
    int i = F(int, s, 1432);
    if (i > 0)
    {
        i--;
        BYTE* e = s + i * 208 + 2220;
        for (;;)
        {
            if (!Mx_MovingOf(F(BYTE*, e, 0)))
                break;
            i--;
            e -= 208;
            if (i < 0)
                break;
        }
        if (i < 0)
            i = F(int, s, 1432);
    }
    int n = F(int, s, 1432);
    if (i == n)
        return;
    BYTE* pa = s + i * 208 + 2156;
    BYTE* pb = s + n * 208 + 2156;
    V(12) = F(float, pb, 0) - F(float, pa, 0);
    V(16) = F(float, pb, 4) - F(float, pa, 4);
    I(68) = I(12);
    I(72) = I(16);
    V(20) = F(float, pb, 8) - F(float, pa, 8);
    I(76) = I(20);
    Vec3_Normalized((Vec3*)&V(52), (const Vec3*)&V(68));
    n = F(int, s, 1432);
    BYTE* vv = s + n * 208 + 2168;
    const BYTE* pp = s + (n * 3 + 510) * 4;
    V(24) = (float)(V(56) * F(float, vv, 8) - V(60) * F(float, vv, 4));
    V(28) = (float)(V(60) * F(float, vv, 0) - V(52) * F(float, vv, 8));
    V(32) = (float)(V(52) * F(float, vv, 4) - V(56) * F(float, vv, 0));
    double e1 = (SV(436) * V(32) + SV(432) * V(28)) + SV(428) * V(24);
    double e2 = (F(float, pp, 8) * V(32) + F(float, pp, 4) * V(28)) + V(24) * F(float, pp, 0);
    if (!(e2 * e1 < g_mx64c264))
        return;
    if (!(x87_fabs(e1) < g_mx64b5b8))
        return;
    {
        double q1 = V(28) * SV(436) - V(32) * SV(432);
        double q2 = V(32) * SV(428) - V(24) * SV(436);
        double q3 = V(24) * SV(432) - V(28) * SV(428);
        V(12) = (float)(V(32) * q2 - V(28) * q3);
        I(132) = I(12);
        V(16) = (float)(q3 * V(24) - V(32) * q1);
        I(136) = I(16);
        V(20) = (float)(V(28) * q1 - q2 * V(24));
        I(140) = I(20);
    }
    {
        double a0 = F(float, vv, 0) * g_mx64c304;
        V(84) = F(float, vv, 4) * g_mx64c304;
        V(88) = F(float, vv, 8) * g_mx64c304;
        double r1 = SV(424) * F(float, vv, 4) - F(float, vv, 8) * SV(420);
        double r2 = F(float, vv, 8) * SV(416) - F(float, vv, 0) * SV(424);
        double r3 = F(float, vv, 0) * SV(420) - SV(416) * F(float, vv, 4);
        V(12) = (float)(r2 * F(float, vv, 8) - r3 * F(float, vv, 4));
        V(16) = (float)(r3 * F(float, vv, 0) - r1 * F(float, vv, 8));
        V(20) = (float)(r1 * F(float, vv, 4) - r2 * F(float, vv, 0));
        V(24) = (float)(V(12) + a0);
        V(28) = V(84) + V(16);
        I(12) = I(24);
        V(32) = V(88) + V(20);
        I(16) = I(28);
        I(20) = I(32);
    }
    CallVirt(s, 148, fr + 132, &g_mx67d640);
    CallVirt(s, 144, fr + 12, &g_mx67d630);
}

} // extern "C"

#undef V
#undef I
#undef D
#undef SV

// === a moving item bouncing off a point (FUN_580a00 with its tail $L_580b25)

extern "C" void __cdecl Mi2_PointImpulse(const void* pa, const void* nb, const void* ma, const void* mb, float k24,
                                         float k28, float k32, const void* pc, const void* vc, const void* ic,
                                         void* outP, void* outV); KING_ALIAS("_Mi2_PointImpulse", "_Body_MovingPointImpulse")
KING_VAR(char, g_mi2_67d470, 67d470)
KING_VAR(char, g_mi2_67d458, 67d458)

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))

extern "C" {

// [FUN_580a00..$L_580b25] item s touching point pa with normal nb: the impulse of Body_MovingPointImpulse on its
// pose (+3172), position (+416), speed (+428) and inertia (+3288); the new speed split into its normal part and its
// sliding part, which loses twice the normal bounce; speed and spin set by vtable +144 / +148
void __cdecl MovingItem_PointBounce(BYTE* s, const float* pa, const float* nb)
{
    BYTE fr[240];
    memcpy(fr + 140, s + 3172, 48);
    Matrix_Inverse((Matrix*)(fr + 188), (const Matrix*)(fr + 140));
    I(12) = F(int, s, 3268);
    memcpy(fr + 44, s + 416, 12);
    I(16) = 0;
    memcpy(fr + 128, s + 428, 12);
    I(20) = 0;
    memcpy(fr + 116, s + 3288, 12);
    I(24) = 0;
    I(56) = 0;
    I(60) = 0;
    I(64) = 0;
    Mi2_PointImpulse(pa, nb, fr + 140, fr + 188, V(12), F(float, s, 1428), 1.0f, fr + 44, fr + 128, fr + 116,
                     fr + 16, fr + 56);
    double d0 = V(16) - V(44);
    double d1 = V(20) - V(48);
    double d2 = V(24) - V(52);
    double e = x87_fabs((d2 * nb[2] + nb[0] * d0) + d1 * nb[1]) * V(12);
    V(12) = (float)((e + e) / V(12));
    V(40) = (float)((nb[2] * V(24) + nb[0] * V(16)) + nb[1] * V(20));
    double c1 = V(24) * nb[1] - V(20) * nb[2];
    double c2 = V(16) * nb[2] - V(24) * nb[0];
    double c3 = V(20) * nb[0] - V(16) * nb[1];
    V(28) = (float)(c2 * nb[2] - c3 * nb[1]);
    I(68) = I(28);
    V(32) = (float)(c3 * nb[0] - c1 * nb[2]);
    I(72) = I(32);
    V(36) = (float)(c1 * nb[1] - c2 * nb[0]);
    I(76) = I(36);
    V(12) = (float)(Vec3_Length((const Vec3*)(fr + 68)) - V(12));
    if (V(12) < g_mx64c264)
        I(12) = 0;
    Vec3_Normalized((Vec3*)(fr + 80), (const Vec3*)(fr + 68));
    double x = V(80) * V(12);
    V(96) = V(84) * V(12);
    V(100) = V(88) * V(12);
    double nx = V(40) * nb[0];
    V(108) = V(40) * nb[1];
    V(112) = V(40) * nb[2];
    V(28) = (float)(nx + x);
    I(16) = I(28);
    V(32) = V(96) + V(108);
    I(20) = I(32);
    V(36) = V(100) + V(112);
    I(24) = I(36);
    CallVirt(s, 144, fr + 16, &g_mi2_67d470);
    CallVirt(s, 148, fr + 56, &g_mi2_67d458);
}

} // extern "C"

#undef V
#undef I

// === a moving item touching another body (FUN_581990 with its tail $L_581b84)

extern "C" void __cdecl Mi3_Decompose(const BYTE* m, float* a, float* b, float* c, float* d, float* e, float* f); KING_ALIAS("_Mi3_Decompose", "_Matrix_Decompose6F")
extern "C" void __cdecl Mi3_Impulse(BYTE* b, BYTE* c, BYTE* o); KING_ALIAS("_Mi3_Impulse", "_Bodies_Impulse")
extern "C" void __fastcall Mi3_ActivateAll(BYTE* s, void*); KING_ALIAS("@Mi3_ActivateAll@8", "@Points_ActivateAll@8")
extern "C" double __cdecl Mi3_FrameTime(); KING_ALIAS("_Mi3_FrameTime", "_Game_FrameTime")
extern "C" BYTE* __fastcall Mi3_Matrix1(BYTE* self, void*, void* out); KING_ALIAS("@Mi3_Matrix1@12", "@SceneObj_Matrix1@12")

#define V(n) (*(float*)(fr + (n)))
#define I(n) (*(int*)(fr + (n)))

extern "C" {

// [FUN_581990..$L_581b84] item s touching contact c (+4 its relative move, +52 point, +64 normal, +116 the other
// object): against a plain object a point bounce; against a body (+392 1 or 2) first, with flag 0x8000000, the
// body moved by the contact's motion scaled by k, then the impulse between both, its points woken (kind 2), the
// item's vtable +200 with the frame time and its contact time (+3144)
void __cdecl MovingItem_BodyContact(BYTE* s, BYTE* c, DWORD fl, float k)
{
    BYTE fr[276];
    BYTE* b = Mx_MovingOf(F(BYTE*, c, 116));
    if (!b || (F(int, b, 392) != 2 && F(int, b, 392) != 1))
    {
        MovingItem_PointBounce(s, (const float*)(c + 52), (const float*)(c + 64));
        return;
    }
    if (fl & 0x8000000)
    {
        I(28) = 0;
        I(32) = 0;
        I(36) = 0;
        I(16) = 0;
        I(20) = 0;
        I(24) = 0;
        Mi3_Decompose(c + 4, &V(28), &V(32), &V(36), &V(24), &V(16), &V(20));
        V(16) = -V(16);
        CallThis(Mx_MulA, fr + 16, fr + 40, k);
        CallThis(Mx_MulA, fr + 28, fr + 40, k);
        Matrix* t = Matrix_Translation((Matrix*)(fr + 52), Mx_Vec(V(28), V(32), V(36)));
        BYTE* m1 = Mi3_Matrix1(b, 0, fr + 192);
        float len = (float)Vec3_Length((const Vec3*)(fr + 16));
        Matrix* r = Matrix_LookAt((Matrix*)(fr + 100), Mx_Zero(), Mx_Vec(V(16), V(20), V(24)), len);
        Matrix* a = Matrix_Multiply((Matrix*)(fr + 148), r, (const Matrix*)m1);
        memcpy(b + 3172, Matrix_Multiply((Matrix*)(fr + 244), a, t), 48);
    }
    Mi3_Impulse(s, c, b);
    if (F(int, b, 392) == 2)
        Mi3_ActivateAll(b, 0);
    float dt = (float)Mi3_FrameTime();
    CallVirt(s, 200, b, c, dt);
    F(float, s, 3144) = (float)Mx_Time();
}

} // extern "C"

#undef V
#undef I

// ---------------------------------------------------------------------------
// Batch 32: a moving item's settle step

extern "C" void Ms32_MovingOf(); KING_ALIAS("_Ms32_MovingOf", "_Moving_Of")
extern "C" void Ms32_MulAssign(); KING_ALIAS("_Ms32_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Ms32_FrameTime(); KING_ALIAS("_Ms32_FrameTime", "_Game_FrameTime")
KING_VAR(double, ms32_still64d668, 64d668)
KING_VAR(double, ms32_settle650628, 650628)
KING_VAR(float, ms32_one64c250, 64c250)
KING_VAR(double, ms32_spin64b738, 64b738)
KING_VAR(char, ms32_s67d5c4, 67d5c4)
KING_VAR(char, ms32_s67d5b0, 67d5b0)

extern "C" {

// [$L_581cb0..$L_581de7] a moving item's settle step (vtable): without flag 8 (+1196) it stays posed (vtable
// +140 with +800, +500); moving and nearly still (speed^2 and spin^2 at most 64d668) it takes the rest pose
// (+804, +548), flag 2 marking it settled (+1192) unless another body holds it; else after the settle time
// (+984) its slow velocity and spin decay by 0.97 per half second; +896 set while moving
void __fastcall MovingItem_Settle(BYTE* s, void*, int)
{
    const float* v = (const float*)(s + 416);
    const float* w = (const float*)(s + 428);
    float v2 = (float)(((double)v[2] * v[2] + (double)v[1] * v[1]) + (double)v[0] * v[0]);
    float w2 = (float)(((double)w[2] * w[2] + (double)w[1] * w[1]) + (double)w[0] * w[0]);
    if (!(F(BYTE, s, 1196) & 8)) {
        CallVirt(s, 140, F(int, s, 800), s + 500);
    } else if (F(int, s, 892) && !(v2 > ms32_still64d668) && !(w2 > ms32_still64d668)) {
        CallVirt(s, 140, F(int, s, 804), s + 548);
        if (F(BYTE, s, 1196) & 2) {
            if (!F(int, s, 892) || !CallC<int>(Ms32_MovingOf, F(int, s, 1028)))
                F(int, s, 1192) = 1;
        }
    } else {
        if (F(int, s, 892) && F(float, s, 984) > ms32_settle650628) {
            float p = (float)x87_pow(0.97, CallC<double>(Ms32_FrameTime) * 2.0);
            float tmp[3];
            if (v2 < ms32_one64c250) {
                CallThis(Ms32_MulAssign, s + 416, tmp, p);
                CallVirt(s, 144, s + 416, &ms32_s67d5c4);
            }
            if (w2 < ms32_spin64b738) {
                CallThis(Ms32_MulAssign, s + 428, tmp, p);
                CallVirt(s, 148, s + 428, &ms32_s67d5b0);
            }
        }
        CallVirt(s, 140, F(int, s, 800), s + 500);
    }
    F(int, s, 896) = F(int, s, 896) || F(int, s, 892) ? 1 : 0;
    F(DWORD, s, 900) = F(DWORD, s, 892);
}

}
