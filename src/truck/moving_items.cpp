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
ASM_PROC(FUN_5dbbb0)
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
ASM_VAR(float, g_eng702630, $L_702630)
ASM_VAR(float, g_eng702634, $L_702634)
ASM_VAR(float, g_eng702638, $L_702638)
ASM_VAR(float, g_ten64c450, $L_64c450)          // 10.0
ASM_VAR(AsmLabel, g_strOvation, $L_678540)

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
extern "C" void FUN_5928d0(); KING_ALIAS("_FUN_5928d0", "_Bodies_Pair")

ASM_VAR(BYTE*, g_effectManager, $L_6f6910)

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

extern "C" void __fastcall Pooled_Set4(BYTE* self, void*);

// ---------------------------------------------------------------------------
extern "C" void FUN_5e19b0(); KING_ALIAS("_FUN_5e19b0", "_Frame_Apply")
ASM_VAR(AsmLabel, s_67d928, $L_67d928)
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
ASM_VAR(Vec3, g_mi702650, $L_702650)

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

ASM_PROC(FUN_5843c0)
ASM_PROC(FUN_5855a0)

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
ASM_VAR(short, g_movite67d10a, $L_67d10a)
ASM_VAR(BYTE*, g_movite6ef1fc, $L_6ef1fc)

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
ASM_VAR(AsmLabel, s_q50_67d604, $L_67d604)
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
ASM_VAR(AsmLabel, s_q57_678550, $L_678550)
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
ASM_VAR(BYTE*, g_q106_6f653c, $L_6f653c)
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
ASM_VAR(AsmLabel, g_q108_7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_q108_702650, $L_702650)
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
ASM_VAR(AsmLabel, s_q173_67d494, $L_67d494)
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

ASM_PROC(FUN_4e8ae1)
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
ASM_VAR(AsmLabel, s_q174_67d434, $L_67d434)
ASM_VAR(AsmLabel, s_q174_67d420, $L_67d420)
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

ASM_VAR(double, g_q186_64b5a0, $L_64b5a0)
ASM_VAR(AsmLabel, s_q186_67d5f0, $L_67d5f0)
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
ASM_VAR(AsmLabel, s_q187_67d594, $L_67d594)
ASM_VAR(AsmLabel, s_q187_66c4e4, $L_66c4e4)
ASM_VAR(AsmLabel, s_q187_68d1c0, $L_68d1c0)
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
ASM_PROC(FUN_5f6820)    // a shape value: hit test (kind, from, to, x)

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
ASM_VAR(float, g_im64c320, $L_64c320)
ASM_VAR(AsmLabel, s_im67d99c, $L_67d99c)
ASM_VAR(AsmLabel, s_im67d96c, $L_67d96c)

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
