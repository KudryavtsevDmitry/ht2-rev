// startup.cpp - the unit at 0x4dd9f0 of king.exe (with CKingApp, see
// king_app.cpp): startup paths, truck.ini, cab names, sounds at the volumes
// of the options.

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

KING_VAR(BYTE*, g_effectManager, 6f6910)

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

// ---------------------------------------------------------------------------
extern "C" void FUN_4e1e10(); KING_ALIAS("_FUN_4e1e10", "@Ini4e1e10_Construct@12")
KING_VAR(AsmLabel, g_ini696750, 696750)
KING_VAR(AsmLabel, s_6733b0, 6733b0)

// [FUN_4e1de0] constructs the ini object 696750 on 6733b0
extern "C" void* __cdecl Startup_Ini696750()
{
    return CallThis<void*>(FUN_4e1e10, g_ini696750, (char*)s_6733b0);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5e52b0(); KING_ALIAS("_FUN_5e52b0", "@Resources_SetData@24")    // resources: send a message (id, kind, size, data)
KING_VAR(int, g_suTypeClass5, 6ce344)
KING_VAR(int, g_suCarTypeCount, 6ce32c)
KING_VAR(BYTE*, g_su6ef204, 6ef204)
KING_VAR(AsmLabel, s_su672f50, 672f50)
KING_VAR(AsmLabel, s_su672f78, 672f78)
KING_VAR(Vec3, g_suZero702650, 702650)
extern "C" void __cdecl World_Fatal(const char* msg);
struct SuBlob48 { DWORD d[12]; };

// [FUN_4df540..$L_4df6e9] sends the creation message of a network object by its id: class 2 (156 bytes: sub-id, index, two names) or 3 (160 bytes: id n+300000, a code from name+5, scale 600)
extern "C" int __cdecl Net_CreateById(BYTE* res, int kind, const char* name, const char* name2, int a5, int a6, int id, SuBlob48 state, int a20)
{
    int low = id % 1000;
    int cls = id / 100000;
    int mid = id % 100000 / 1000;
    if (cls == 2)
    {
        BYTE msg[156];
        memset(msg, 0, sizeof msg);
        F(int, msg, 0) = mid;
        F(int, msg, 4) = low;
        F(int, msg, 8) = 1;
        F(int, msg, 12) = 0;
        F(int, msg, 16) = a20;
        F(Vec3, msg, 20) = g_suZero702650;
        F(Vec3, msg, 32) = g_suZero702650;
        F(SuBlob48, msg, 44) = state;
        strncpy((char*)msg + 92, name, 31);
        strncpy((char*)msg + 124, name2, 31);
        CallThis(FUN_5e52b0, res + 704, id, kind, 156, msg);
        return 0;
    }
    if (cls != 3)
        return 0;
    int oid = low + 300000;
    char code[12];
    BYTE msg[160];
    memset(msg, 0, sizeof msg);
    F(int, msg, 0) = g_suTypeClass5 < g_suCarTypeCount ? g_suTypeClass5 : 0;
    F(int, msg, 4) = 11;
    F(int, msg, 12) = 1;
    F(int, msg, 16) = a20;
    F(Vec3, msg, 20) = g_suZero702650;
    F(Vec3, msg, 32) = g_suZero702650;
    F(SuBlob48, msg, 44) = state;
    F(float, msg, 92) = 600.0f;
    strncpy((char*)msg + 96, name, 31);
    strncpy((char*)msg + 128, s_su672f78, 31);
    strncpy(code, name + 5, 2);
    code[2] = 0;
    BYTE* table = res + 704;
    CallThis(FUN_5e52b0, table, oid, code, 160, msg);
    BYTE* r = CallThis<BYTE*>(FUN_5e5370, table, oid);
    if (g_su6ef204)
    {
        F(int, r, 16) = F(int, g_su6ef204, 16);
        return 0;
    }
    World_Fatal(s_su672f50);
    return 0;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_603b70(); KING_ALIAS("_FUN_603b70", "_Ini_ReadInt")
extern "C" void FUN_50a5a0(); KING_ALIAS("_FUN_50a5a0", "_VehTech_Load")
extern "C" void FUN_506f50(); KING_ALIAS("_FUN_506f50", "_Car_CopyTypeNames")
extern "C" void FUN_4e260c(); KING_ALIAS("_FUN_4e260c", "_World_Fatal")
extern "C" void FUN_4df950(); KING_ALIAS("_FUN_4df950", "_Startup_ReadSection")
extern "C" void FUN_508000(); KING_ALIAS("_FUN_508000", "_VehCache_Load")
extern "C" void FUN_508300(); KING_ALIAS("_FUN_508300", "_CarTypes_Read")
extern "C" void FUN_50a740(); KING_ALIAS("_FUN_50a740", "_ObjTypes_Read")
KING_VAR(AsmLabel, s_su671518, 671518)
KING_VAR(AsmLabel, s_su672ec8, 672ec8)      // the ini file
KING_VAR(AsmLabel, s_su6730ac, 6730ac)
KING_VAR(AsmLabel, s_su6729a8, 6729a8)
KING_VAR(AsmLabel, s_su6730a4, 6730a4)
KING_VAR(AsmLabel, s_su6730a0, 6730a0)
KING_VAR(AsmLabel, s_su672994, 672994)
KING_VAR(AsmLabel, s_su673094, 673094)
KING_VAR(AsmLabel, s_su673084, 673084)
KING_VAR(AsmLabel, s_su673070, 673070)
KING_VAR(AsmLabel, s_su673064, 673064)
KING_VAR(int, g_su696c74, 696c74)
KING_VAR(int, g_su696c6c, 696c6c)
KING_VAR(int, g_su696c70, 696c70)
KING_VAR(int, g_su672d80, 672d80)
KING_VAR(int, g_su696c58, 696c58)
KING_VAR(void*, g_su6ce36c, 6ce36c)
KING_VAR(void*, g_su697858, 697858)
KING_VAR(int, g_su697860, 697860)
KING_VAR(AsmLabel, g_su696bc8, 696bc8)

// whether the ini value of a key is the text 672994
static int Startup_IniIs(const char* key)
{
    char buf[16];
    GetPrivateProfileStringA(s_su6730a0, key, s_su6729a8, buf, 16, s_su672ec8);
    return !strcmp(buf, s_su672994);
}

// [FUN_4dfec0..$L_4dff1a] reads the startup options of the ini file (fatal when it is missing), sets the fatal hooks and loads the vehicle techs
extern "C" void __cdecl Startup_ReadOptions()
{
    FILE* f = fopen(s_su672ec8, s_su671518);
    if (!f)
    {
        char dir[200], text[300];
        GetCurrentDirectoryA(200, dir);
        sprintf(text, s_su6730ac, (char*)s_su672ec8, dir);
        World_Fatal(text);
    }
    fclose(f);
    g_su696c74 = Startup_IniIs(s_su6730a4);
    g_su696c6c = Startup_IniIs(s_su673094);
    g_su696c70 = Startup_IniIs(s_su673084);
    g_su672d80 = Startup_IniIs(s_su673070);
    int v;
    CallC(FUN_603b70, (char*)s_su6730a0, (char*)s_su673064, -1, &v, (char*)s_su672ec8);
    g_su696c58 = v;
    CallC(FUN_4df950);
    g_su6ce36c = (void*)FUN_4e260c;
    g_su697858 = (void*)FUN_4e260c;
    g_su697860 = CallC<int>(FUN_508000);
    if (g_su697860)
    {
        g_su697860 = CallC<int>(FUN_50a5a0);
        if (g_su697860)
            return;
    }
    CallC(FUN_508300, (BYTE*)g_su696bc8, 1);
    CallC(FUN_506f50);
    CallC(FUN_50a740, (BYTE*)g_su696bc8, 1);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_507fa0(); KING_ALIAS("_FUN_507fa0", "_Carat_ReadFill2")    // reads a 5-float ini value (section, key, default, out, file, n)
extern "C" void FUN_4e12d0(); KING_ALIAS("_FUN_4e12d0", "_Path_Join")    // a path moved to a drive (path, drive, out)
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
extern "C" void FUN_5d2ef0(); KING_ALIAS("_FUN_5d2ef0", "_Sys_Exit")
KING_VAR(AsmLabel, s_su672eb8, 672eb8)
KING_VAR(AsmLabel, s_su673050, 673050)
KING_VAR(AsmLabel, s_su67305c, 67305c)
KING_VAR(AsmLabel, s_su673024, 673024)
KING_VAR(AsmLabel, s_su67301c, 67301c)
KING_VAR(AsmLabel, s_su673014, 673014)
KING_VAR(AsmLabel, s_su673008, 673008)
KING_VAR(AsmLabel, s_su672ffc, 672ffc)
KING_VAR(AsmLabel, s_su672ff0, 672ff0)
KING_VAR(AsmLabel, s_su672fe4, 672fe4)
KING_VAR(AsmLabel, s_su672fd8, 672fd8)
KING_VAR(AsmLabel, s_su672fcc, 672fcc)
KING_VAR(AsmLabel, s_su672fc0, 672fc0)
KING_VAR(AsmLabel, s_su672fb8, 672fb8)
KING_VAR(AsmLabel, s_su672fb0, 672fb0)
KING_VAR(AsmLabel, s_su672fa8, 672fa8)
KING_VAR(AsmLabel, s_su672fa0, 672fa0)
KING_VAR(AsmLabel, s_su672f94, 672f94)
KING_VAR(AsmLabel, s_su672f88, 672f88)
KING_VAR(AsmLabel, s_su672f7c, 672f7c)
KING_VAR(int, g_su696c40, 696c40)
KING_VAR(float, g_su696bdc, 696bdc)
KING_VAR(float, g_su696be0, 696be0)
KING_VAR(short, g_su696bda, 696bda)
KING_VAR(int, g_su696be4, 696be4)
KING_VAR(int, g_su696be8, 696be8)
KING_VAR(int, g_su696bec, 696bec)
KING_VAR(AsmLabel, g_su696bf0, 696bf0)
KING_VAR(AsmLabel, g_su696c04, 696c04)
KING_VAR(AsmLabel, g_su696c18, 696c18)
KING_VAR(AsmLabel, g_su696c2c, 696c2c)
KING_VAR(float, g_su696c44, 696c44)
KING_VAR(int, g_su696c54, 696c54)
KING_VAR(int, g_su696c48, 696c48)
KING_VAR(int, g_su696c4c, 696c4c)
KING_VAR(int, g_su696c50, 696c50)
KING_VAR(int, g_su696c5c, 696c5c)
KING_VAR(int, g_su696c60, 696c60)
KING_VAR(int, g_su696c64, 696c64)

// whether the value of a key of section 672eb8 is the text 672994
static int Startup_OptIs(const char* key, const char* def)
{
    char buf[16];
    GetPrivateProfileStringA(s_su672eb8, key, def, buf, 16, s_su672ec8);
    return !strcmp(buf, s_su672994);
}

// [FUN_4df950..$L_4df9bd] reads the options of section 672eb8 of the ini file: the name (696bc8, fatal without it), flags, the four 5-float tables and their scales
extern "C" void __cdecl Startup_ReadSection()
{
    char buf[80];
    GetPrivateProfileStringA(s_su672eb8, s_su673050, s_su67305c, buf, 70, s_su672ec8);
    if (!strcmp(buf, s_su67305c))
        World_Fatal(s_su673024);
    sprintf(g_su696bc8, s_su67301c, buf);
    g_su696c40 = Startup_OptIs(s_su673014, s_su6729a8);
    g_su696bdc = 1.4f;
    g_su696be0 = 0.5f;
    g_su696bda = 1;
    g_su696be4 = Startup_OptIs(s_su673008, s_su6729a8);
    g_su696be8 = Startup_OptIs(s_su672ffc, s_su6729a8);
    g_su696bec = Startup_OptIs(s_su672ff0, s_su6729a8);
    CallC(FUN_507fa0, (char*)s_su672eb8, (char*)s_su672fe4, 0, (char*)g_su696bf0, (char*)s_su672ec8, 5);
    CallC(FUN_507fa0, (char*)s_su672eb8, (char*)s_su672fd8, 10, (char*)g_su696c04, (char*)s_su672ec8, 5);
    CallC(FUN_507fa0, (char*)s_su672eb8, (char*)s_su672fcc, 380, (char*)g_su696c18, (char*)s_su672ec8, 5);
    CallC(FUN_507fa0, (char*)s_su672eb8, (char*)s_su672fc0, 430, (char*)g_su696c2c, (char*)s_su672ec8, 5);
    g_su696c44 = 1.5f;
    g_su696c54 = Startup_OptIs(s_su672fb8, s_su672994);
    g_su696c48 = Startup_OptIs(s_su672fb0, s_su6729a8);
    g_su696c4c = Startup_OptIs(s_su672fa8, s_su6729a8);
    g_su696c50 = Startup_OptIs(s_su672fa0, s_su6729a8);
    g_su696c5c = Startup_OptIs(s_su672f94, s_su672994);
    g_su696c60 = Startup_OptIs(s_su672f88, s_su6729a8);
    g_su696c64 = Startup_OptIs(s_su672f7c, s_su6729a8);
}

KING_VAR(const char*, g_suIni, 696868)
KING_VAR(BYTE*, g_su696ca0, 696ca0)
KING_VAR(AsmLabel, s_su6733c4, 6733c4)
KING_VAR(AsmLabel, s_su6733cc, 6733cc)
KING_VAR(AsmLabel, s_su6733d4, 6733d4)
KING_VAR(AsmLabel, s_su6733bc, 6733bc)
KING_VAR(AsmLabel, s_su6733b0, 6733b0)
KING_VAR(AsmLabel, s_su6733ac, 6733ac)
KING_VAR(AsmLabel, s_su6733a4, 6733a4)
KING_VAR(AsmLabel, s_su67339c, 67339c)
KING_VAR(AsmLabel, s_su673398, 673398)
KING_VAR(AsmLabel, s_su67338c, 67338c)
KING_VAR(AsmLabel, s_su673384, 673384)
KING_VAR(AsmLabel, s_su67337c, 67337c)
KING_VAR(AsmLabel, s_su673374, 673374)
KING_VAR(AsmLabel, s_su67336c, 67336c)
KING_VAR(AsmLabel, s_su673364, 673364)
KING_VAR(AsmLabel, s_su673358, 673358)

// moves the ini path of a key of section 6733c4 to a drive: reads it (n chars) into buf, rewrites it as out
static void Startup_MovePath(const char* key, int n, char* buf, const char* drive, char* out)
{
    GetPrivateProfileStringA(s_su6733c4, key, s_su6733ac, buf, n, g_suIni);
    CallC(FUN_4e12d0, buf, drive, out);
    WritePrivateProfileStringA(s_su6733c4, key, out, g_suIni);
}

// a copy of the ini path of a key of section 6733c4 ending in a backslash (unless it is 673358, which is fatal when fatal is set)
static char* Startup_DirOf(const char* key, int fatal)
{
    char buf[64];
    GetPrivateProfileStringA(s_su6733c4, key, s_su6733ac, buf, 60, g_suIni);
    char* p = (char*)operator new(strlen(buf) + 2);
    strcpy(p, buf);
    char* end = p;
    while (*end)
        end++;
    if (strcmp(buf, s_su673358))
    {
        end[0] = '\\';
        end[1] = 0;
    }
    else if (fatal)
        CallC(FUN_5d2ef0, CallC<const char*>(FUN_5d2e50, 1));
    return p;
}

// [FUN_4e1320..$L_4e1991] when the game's data file isn't found, looks for it on the CD drives and points the ini paths there (by the install kind of 6733cc) and the world's two directories (+644, +648)
extern "C" void __cdecl Startup_FindCd()
{
    char buf[256];
    bool kind2 = false, kind3 = false, kind1 = false;
    GetPrivateProfileStringA(s_su6733c4, s_su6733cc, s_su6733d4, buf, 30, g_suIni);
    if (!strcmp(buf, s_su6733d4))
        kind2 = true;
    else if (!strcmp(buf, s_su6733bc))
        kind3 = true;
    else
        kind1 = true;
    GetPrivateProfileStringA(s_su6733c4, s_su6733a4, s_su6733ac, buf, 100, s_su6733b0);
    strcat(buf, s_su67339c);
    FILE* f = fopen(buf, s_su673398);
    if (f)
    {
        fclose(f);
        return;
    }
    char drives[1024];
    char out1[1024], out2[1024], out3[1024];
    GetLogicalDriveStringsA(1024, drives);
    char* d = drives;
    if (!*d)
        return;
    do
    {
        if (GetDriveTypeA(d) == 5)
        {
            sprintf(buf, s_su67338c, d);
            f = fopen(buf, s_su673398);
            if (f)
            {
                if (kind2)
                {
                    Startup_MovePath(s_su6733a4, 100, buf, d, out1);
                    Startup_MovePath(s_su673384, 255, buf, d, out1);
                    Startup_MovePath(s_su67337c, 100, buf, d, out1);
                    Startup_MovePath(s_su673374, 60, buf, d, out1);
                }
                if (kind3)
                {
                    Startup_MovePath(s_su6733a4, 100, buf, d, out2);
                    Startup_MovePath(s_su67337c, 100, buf, d, out2);
                    GetPrivateProfileStringA(s_su6733c4, s_su67336c, s_su6733ac, out2, 100, g_suIni);
                    WritePrivateProfileStringA(s_su6733c4, s_su673374, out2, g_suIni);
                    strncat(out2, s_su673364, 7);
                    WritePrivateProfileStringA(s_su6733c4, s_su673384, out2, g_suIni);
                }
                if (kind1)
                    Startup_MovePath(s_su6733a4, 100, buf, d, out3);
                if (g_su696ca0)
                {
                    char* p = Startup_DirOf(s_su67336c, 1);
                    if (F(char*, g_su696ca0, 644))
                        operator delete(F(char*, g_su696ca0, 644));
                    F(char*, g_su696ca0, 644) = p;
                    p = Startup_DirOf(s_su673374, 0);
                    if (F(char*, g_su696ca0, 648))
                        operator delete(F(char*, g_su696ca0, 648));
                    F(char*, g_su696ca0, 648) = p;
                }
                fclose(f);
            }
        }
        while (*d)
            d++;
        d++;
    } while (*d);
}

// ---------------------------------------------------------------------------
extern "C" void FUN_568b10(); KING_ALIAS("_FUN_568b10", "_Helicopters_Init")
extern "C" int __cdecl Copter_LoadHook(BYTE* rec, int flags);
extern "C" int __cdecl Item_OnEvent(BYTE* it, int e);    // vehicle.cpp
extern "C" int __cdecl TrashBox_LoadHook(BYTE* rec, int flags);
extern "C" void $L_52ee70(); KING_ALIAS("_$L_52ee70", "_Cars_Deliver")
KING_VAR(int, g_su67b2b8, 67b2b8)
KING_VAR(void*, g_su6ef24c, 6ef24c)
KING_VAR(void*, g_su6ef250, 6ef250)
KING_VAR(void*, g_su6cf3a0, 6cf3a0)
KING_VAR(void*, g_su6cf3a4, 6cf3a4)
KING_VAR(BYTE*, g_su6cec6c, 6cec6c)

// [FUN_4e0660] hooks the object's two handlers (+2708, +2712) twice, saving the old ones, and creates its copter objects in single player
extern "C" void __cdecl Obj_HookHandlers(BYTE* o)
{
    g_su67b2b8 = 0;
    g_su6ef24c = F(void*, o, 2708);
    F(void*, o, 2708) = (void*)Copter_LoadHook;
    g_su6ef250 = F(void*, o, 2712);
    F(void*, o, 2712) = (void*)Item_OnEvent;
    if (!F(int, g_su6cec6c, 1316) && F(int, g_su6cec6c, 1300) <= 1)
        CallC(FUN_568b10, o);
    g_su6cf3a0 = F(void*, o, 2708);
    F(void*, o, 2708) = (void*)TrashBox_LoadHook;
    g_su6cf3a4 = F(void*, o, 2712);
    F(void*, o, 2712) = (void*)$L_52ee70;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_4e0140(); KING_ALIAS("_FUN_4e0140", "@Wnd4e0140_Destruct@8")
extern "C" void FUN_5f0770(); KING_ALIAS("_FUN_5f0770", "@Thunk5f0770@4")
KING_VAR(AsmLabel, g_sta64d81c, 64d81c)

static BYTE* Sta_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_4e01b0]
extern "C" BYTE* __fastcall Sta4e01b0_DeletingDestructor(BYTE* s, void*, int flags) { return Sta_DeleteAfter(s, flags, FUN_4e0140); }
// [$L_4e1d90]
extern "C" BYTE* __fastcall Sta4e1d90_DeletingDestructor(BYTE* s, void*, int flags) { return Sta_DeleteAfter(s, flags, FUN_5f0770); }
// [$L_4e1db0]
extern "C" BYTE* __fastcall Sta4e1db0_DeletingDestructor(BYTE* s, void*, int flags)
{
    F(void*, s, 0) = g_sta64d81c;
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_4e24ca(); KING_ALIAS("_FUN_4e24ca", "_Static6974a8_Init")
extern "C" void FUN_5dfa90(); KING_ALIAS("_FUN_5dfa90", "@View_RenderRoom@4")
extern "C" void FUN_5dff20(); KING_ALIAS("_FUN_5dff20", "_Matrix_Angles@16")
extern "C" void FUN_5de600(); KING_ALIAS("_FUN_5de600", "@View_BeginFrame@4")
extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")
extern "C" void FUN_4eb230(); KING_ALIAS("_FUN_4eb230", "_Tables_Clear697608")
extern "C" void FUN_57cab0(); KING_ALIAS("_FUN_57cab0", "_Race_ResetState")
extern "C" void FUN_53fd60(); KING_ALIAS("_FUN_53fd60", "_Cars_ResetGlobals")
extern "C" void FUN_557510(); KING_ALIAS("_FUN_557510", "_Car_ClearGlobals6ef2b8")
extern "C" void FUN_578100(); KING_ALIAS("_FUN_578100", "_Joy_SetAutoCenter")
extern "C" void FUN_4e35cf(); KING_ALIAS("_FUN_4e35cf", "_World_ReadMapSymbols")
extern "C" void FUN_4e0280(); KING_ALIAS("_FUN_4e0280", "@VehicleSounds_Find@4")
extern "C" void FUN_53fcf0(); KING_ALIAS("_FUN_53fcf0", "_Race_Start")
extern "C" void FUN_52ad30(); KING_ALIAS("_FUN_52ad30", "_Cars_LoadCollision")
extern "C" void FUN_5c5370(); KING_ALIAS("_FUN_5c5370", "@Thunk5c5370@4")
extern "C" void FUN_4b9570(); KING_ALIAS("_FUN_4b9570", "_Effects_Init")
KING_VAR(AsmLabel, s_sta671204, 671204)
KING_VAR(double, g_sta64b658, 64b658)
KING_VAR(double, g_sta696d08, 696d08)
KING_VAR(AsmLabel, g_sta696c78, 696c78)
KING_VAR(AsmLabel, g_sta6d1cd0, 6d1cd0)
KING_VAR(int, g_sta6968d4, 6968d4)
KING_VAR(AsmLabel, g_sta696750, 696750)
KING_VAR(WORD, g_sta67d10a, 67d10a)
KING_VAR(WORD, g_sta672d84, 672d84)

// [$L_4e01a0]
extern "C" BYTE* __fastcall King_At704(BYTE* s) { return s + 704; }
// [$L_4e1e30]
extern "C" void __fastcall King_Set340(BYTE* s) { F(int, s, 340) = 1; }
// [$L_4e24c0]
extern "C" void __cdecl King_4e24c0() { CallC(FUN_4e24ca); }

// [$L_4e0160]
extern "C" int __fastcall King_4e0160(BYTE* s, void*, int a)
{
    CallVirt(s, 148, a);
    return 0;
}
// [$L_4e0180]
extern "C" int __fastcall King_4e0180(BYTE* s, void*, int a) { return CallVirt<int>(s, 196, (const char*)s_sta671204, a); }
// [$L_4e0b50]
extern "C" int __fastcall King_4e0b50(BYTE* s)
{
    CallVirt(s, 76);
    return CallThis<int>(FUN_5dfa90, s);
}
// [$L_4e0930]
extern "C" int __fastcall King_4e0930(BYTE* s, void*, int a, int b, int c, int d) { return CallThis<int>(FUN_5dff20, s, a, b, c, d); }

// [$L_4e0b80] the game clock (+2968) less the scaled real time to 696d08, resets, 696c78..696c8c cleared
extern "C" int __fastcall King_4e0b80(BYTE* s)
{
    CallThis(FUN_5de600, s);
    double t = CallC<double>(FUN_6037e0) * g_sta64b658;
    g_sta696d08 = F(double, s, 2968) - t;
    CallC(FUN_4eb230);
    CallC(FUN_57cab0);
    CallC(FUN_53fd60);
    CallC(FUN_557510);
    DWORD* z = (DWORD*)g_sta696c78;
    z[0] = z[1] = z[2] = z[3] = z[4] = z[5] = 0;
    return 0;
}

// [$L_4e0560] the world's start
extern "C" void __fastcall King_4e0560(BYTE* s)
{
    CallC(FUN_578100, 0);
    CallC(FUN_4e35cf);
    CallThis(FUN_4e0280, g_sta6d1cd0);
    CallC(FUN_53fcf0, s, (BYTE*)g_sta696750, 980, g_sta6968d4);
    g_sta672d84 = (WORD)(g_sta67d10a + 1);
    CallC(FUN_52ad30);
    F(float, s, 9376) = (float)F(double, s, 3048);
    F(float, s, 9380) = (float)F(double, s, 3056);
    CallThis(FUN_5c5370, s);
    CallC(FUN_4b9570, s);
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_4bac00(); KING_ALIAS("_FUN_4bac00", "_Effects_Shutdown")
extern "C" void FUN_5e0180(); KING_ALIAS("_FUN_5e0180", "@Fw_Clear5e0180@8")
extern "C" void FUN_56eea0(); KING_ALIAS("_FUN_56eea0", "_World_FreeAll")
ASM_FUNC(FUN_53fd30, _Startup_Call53fd30)
extern "C" void FUN_5c5910(); KING_ALIAS("_FUN_5c5910", "@Media_ObjectMessage@20")
extern "C" void FUN_56d0f0(); KING_ALIAS("_FUN_56d0f0", "_CopterNet_Handle")
extern "C" void FUN_586d90(); KING_ALIAS("_FUN_586d90", "_SceneObj_DispatchCommand")
extern "C" void FUN_46d7f0(); KING_ALIAS("_FUN_46d7f0", "_Ai_NetMessage")
extern "C" void FUN_4bc540(); KING_ALIAS("_FUN_4bc540", "_Fx_HandleMessage")
extern "C" void FUN_53fd40(); KING_ALIAS("_FUN_53fd40", "_World_ResetAll")
extern "C" void FUN_4e0630(); KING_ALIAS("_FUN_4e0630", "@Obj_4e0630@12")
extern "C" void FUN_4e5ea8(); KING_ALIAS("_FUN_4e5ea8", "_World_ClearHits")
extern "C" void FUN_5c5380(); KING_ALIAS("_FUN_5c5380", "@Media_Reset5c5380@8")
extern "C" void FUN_4b9b90(); KING_ALIAS("_FUN_4b9b90", "_Effects_SetupWorld")
extern "C" void FUN_5e30c0(); KING_ALIAS("_FUN_5e30c0", "@CGameApp_SetMousePos@16")
extern "C" void Sta_sprintf(); KING_ALIAS("_Sta_sprintf", "_sprintf")
KING_VAR(int, g_sta6d2078, 6d2078)
KING_VAR(int, g_sta696cb8, 696cb8)
KING_VAR(AsmLabel, g_sta696cbc, 696cbc)
KING_VAR(AsmLabel, s_sta673340, 673340)
KING_VAR(int, g_sta6974bc, 6974bc)
KING_VAR(BYTE, g_sta6d1db0, 6d1db0)
KING_VAR(BYTE, g_sta6d1db1, 6d1db1)
KING_VAR(int, g_sta67b2b8, 67b2b8)
KING_VAR(int, g_sta696850, 696850)
KING_VAR(int, g_sta696854, 696854)
KING_VAR(AsmLabel, g_sta6d1948, 6d1948)
KING_VAR(AsmLabel, g_sta696b08, 696b08)
KING_VAR(AsmLabel, g_sta696910, 696910)

extern "C" void FUN_530020(); KING_ALIAS("_FUN_530020", "@Words295_Reset@8")

// [$L_4e05e0] leaving the world
extern "C" void __fastcall King_LeaveWorld(BYTE* s)
{
    CallThis(FUN_4bac00, s);
    CallThis(FUN_5e0180, s);
    CallC(FUN_56eea0);
    g_sta6d2078 = 0;
    g_sta696cb8 = 0;
    CallC(Sta_sprintf, (char*)g_sta696cbc, (const char*)s_sta673340);
    CallC(FUN_53fd30);
    CallC(FUN_578100, 0);
}

// [$L_4e0ce0] a command for the first part that takes it
extern "C" int __fastcall King_Command(BYTE* s, void*, int a, int b, int c)
{
    if (CallThis<int>(FUN_5c5910, s, a, b, c))
        return 1;
    if (CallC<int>(FUN_56d0f0, a, b, c) || CallC<int>(FUN_586d90, a, b, c) || CallC<int>(FUN_46d7f0, a, b, c)
        || CallC<int>(FUN_4bc540, a, b, c))
        return 1;
    return 0;
}

// [$L_4e06f0] entering the world
extern "C" void __fastcall King_EnterWorld(BYTE* s)
{
    g_sta6d2078 = 0;
    CallC(FUN_53fd40);
    int rect[4] = {0, 40, 640, 300};
    CallThis(FUN_4e0630, s, rect);
    g_sta6974bc = 0;
    g_sta6d1db0 = 1;
    g_sta6d1db1 = 0;
    g_sta67b2b8 = 0;
    CallC(FUN_4e5ea8);
    CallThis(FUN_5c5380, s);
    CallC(FUN_4b9b90, s);
    F(int, s, 3028) = 0;
    CallThis(FUN_5e30c0, (BYTE*)g_sta696750, g_sta696850 / 2, g_sta696854 / 2);
    CallThis(FUN_530020, (BYTE*)g_sta6d1948);
    DWORD* p = (DWORD*)g_sta696b08;
    for (int i = 0; i < 36; i++)
        p[i] = 0;
    p = (DWORD*)g_sta696910;
    for (int i = 0; i < 36; i++)
        p[i] = 0;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
extern "C" void FUN_5de0a0(); KING_ALIAS("_FUN_5de0a0", "@World_SetupPasses@4")
extern "C" void FUN_531290(); KING_ALIAS("_FUN_531290", "@Car_Construct@20")
extern "C" void FUN_532430(); KING_ALIAS("_FUN_532430", "@Car_SetupDashboard@8")
extern "C" void FUN_53e320(); KING_ALIAS("_FUN_53e320", "@Controls_Construct@12")
extern "C" void FUN_56e5a0(); KING_ALIAS("_FUN_56e5a0", "_CopterScreen_Start")
KING_VAR(BYTE*, g_staCar6d2078, 6d2078)
KING_VAR(BYTE*, g_sta6cec6c, 6cec6c)
KING_VAR(AsmLabel, s_sta672ec8, 672ec8)

// [$L_4e07b0] the world's start: the player's truck (1636 bytes, kind 99), its controls in a single player game
extern "C" void __fastcall King_CreatePlayer(BYTE* s)
{
    CallThis(FUN_5de0a0, s);
    BYTE* m = (BYTE*)operator new(1636);
    g_staCar6d2078 = m ? CallThis<BYTE*>(FUN_531290, m, 99, s, (BYTE*)g_sta696750) : 0;
    F(int, g_staCar6d2078, 1404) |= 128;
    CallThis(FUN_5d3570, s + 2992, g_staCar6d2078);
    F(int, g_staCar6d2078, 1404) |= 64;
    F(int, g_staCar6d2078, 1404) |= 1;
    CallThis(FUN_532430, g_staCar6d2078);
    int mode = F(int, g_sta6cec6c, 1316);
    if (mode == 1 || mode == 0) {
        BYTE* m2 = (BYTE*)operator new(88);
        F(BYTE*, g_staCar6d2078, 1632) = m2 ? CallThis<BYTE*>(FUN_53e320, m2, s) : 0;
        CallThis(FUN_5d3570, s + 2992, F(BYTE*, g_staCar6d2078, 1632));
        F(int, F(BYTE*, g_staCar6d2078, 1632), 80) = 1;
    }
    CallC(FUN_56e5a0, s, (const char*)s_sta672ec8);
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_4bdef0(); KING_ALIAS("_FUN_4bdef0", "@Vec3_SetThis@20")
KING_VAR(AsmLabel, g_q47_6974a8, 6974a8)
// [FUN_4e24ca] sets the static vector 6974a8 to 1, 1, 1
extern "C" void __cdecl Static6974a8_Init(void)
{
    CallThis(FUN_4bdef0, (void*)g_q47_6974a8, 1.0f, 1.0f, 1.0f);
}

// ---------------------------------------------------------------------------
// Small methods: pool slots, list teardown, loader thread, frame shutdown, message dispatch.

// [$L_4e0200] constructor: zero counters and the eleven +132 values, the eleven +180 values at 4
extern "C" BYTE* __fastcall Sta4e0200_Construct(BYTE* s, void*)
{
    static const int z[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 40, 44, 48, 52, 64, 68, 72, 76, 80, 84, 88, 92, 112,
                            108, 116, 120, 124, 128};
    for (int k = 0; k < (int)(sizeof z / sizeof z[0]); k++)
        F(int, s, z[k]) = 0;
    for (int k = 0; k < 11; k++) {
        F(int, s, 132 + k * 4) = 0;
        F(float, s, 180 + k * 4) = 4.0f;
    }
    return s;
}

// ---------------------------------------------------------------------------
// Media handlers of the startup windows.

extern "C" void FUN_4d8350(); KING_ALIAS("_FUN_4d8350", "_Media_VehicleEvent")
extern "C" void FUN_4df540(); KING_ALIAS("_FUN_4df540", "_Net_CreateById")
extern "C" void FUN_5c5440(); KING_ALIAS("_FUN_5c5440", "_Media_Zero19@76")
#define STA94_ARGS a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19
#define STA94_PARAMS                                                                                            \
    DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8, DWORD a9, DWORD a10,        \
        DWORD a11, DWORD a12, DWORD a13, DWORD a14, DWORD a15, DWORD a16, DWORD a17, DWORD a18, DWORD a19

// [$L_4df7c0] a media event: the common handler first, then 4d8350 when it did not take it
extern "C" int __fastcall Sta_Media4df7c0(BYTE* s, void*, STA94_PARAMS)
{
    if (int r = CallThis<int>(FUN_5c5440, s, STA94_ARGS))
        return r;
    return CallC<int>(FUN_4d8350, s, STA94_ARGS);
}

// [$L_4df850] a media event: the common handler first, then 4df540 when it did not take it
extern "C" int __fastcall Sta_Media4df850(BYTE* s, void*, STA94_PARAMS)
{
    if (int r = CallThis<int>(FUN_5c5440, s, STA94_ARGS))
        return r;
    return CallC<int>(FUN_4df540, s, STA94_ARGS);
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
KING_VAR(char, g_q197_696750, 696750)
extern "C" void q197_FUN_4e1de0(); KING_ALIAS("_q197_FUN_4e1de0", "_Startup_Ini696750")
extern "C" void q197_FUN_4e1e60(); KING_ALIAS("_q197_FUN_4e1e60", "@CKingApp_Destruct@8")
// [$L_4e1e00] destroys the static object 696750 at exit (FUN_4e1e60)
extern "C" void __cdecl Static696750_Destroy(void) { CallThis(q197_FUN_4e1e60, &g_q197_696750); }
// [$L_4e1df0] registers the teardown at exit
extern "C" int __cdecl Static696750_AtExit(void) { return atexit((void(__cdecl*)(void))Static696750_Destroy); }
// [$L_4e1dd0] the static initializer: FUN_4e1de0, then the teardown at exit
extern "C" int __cdecl Static696750_Init(void)
{
    CallC(q197_FUN_4e1de0);
    return Static696750_AtExit();
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q240_View_EndFrame(BYTE* s); KING_ALIAS("@q240_View_EndFrame@4", "@View_EndFrame@4")
// [$L_4e0b70] forwards to View_EndFrame
extern "C" int __fastcall Thunk4e0b70(BYTE* s) { return q240_View_EndFrame(s); }

// === finding the vehicle sounds (FUN_4e0280)


static void* Vs_Find(const char* name) { return CallC<void*>(FUN_5ef3d0, name); }

extern "C" {

// [FUN_4e0280] looks up the sounds of vehicle sound set s by name; a missing helicopter, antiradar, startgloch
// or kriak sound falls back to extcar, horn, applause or repair; +124 is pss2 when pss exists
void __fastcall VehicleSounds_Find(BYTE* s)
{
    F(void*, s, 44) = Vs_Find("explosSound");
    F(void*, s, 32) = Vs_Find("startSound");
    F(void*, s, 36) = Vs_Find("start0Sound");
    F(void*, s, 40) = Vs_Find("stop_engSound");
    F(void*, s, 28) = Vs_Find("shakeSound");
    F(void*, s, 0) = Vs_Find("breakSound");
    F(void*, s, 4) = Vs_Find("hitSound");
    F(void*, s, 8) = Vs_Find("crash2Sound");
    F(void*, s, 12) = Vs_Find("crash4aSound");
    F(void*, s, 16) = Vs_Find("crash9Sound");
    F(void*, s, 20) = Vs_Find("hitmeSound");
    F(void*, s, 24) = Vs_Find("hitgrndSound");
    F(void*, s, 68) = Vs_Find("hornSound");
    F(void*, s, 72) = Vs_Find("bighornSound");
    F(void*, s, 76) = Vs_Find("extcarSound");
    F(void*, s, 88) = Vs_Find("helicopterSound");
    if (!F(void*, s, 88))
        F(void*, s, 88) = F(void*, s, 76);
    F(void*, s, 80) = Vs_Find("stonesSound");
    F(void*, s, 84) = Vs_Find("waterSound");
    F(void*, s, 48) = Vs_Find("idleSound");
    F(void*, s, 52) = Vs_Find("fullSound");
    F(void*, s, 56) = Vs_Find("jidleSound");
    F(void*, s, 60) = Vs_Find("jfullSound");
    F(void*, s, 64) = Vs_Find("gearSound");
    F(void*, s, 92) = Vs_Find("repairSound");
    F(void*, s, 96) = Vs_Find("applauseSound");
    F(void*, s, 100) = Vs_Find("carhornSound");
    F(void*, s, 104) = Vs_Find("ovationSound");
    F(void*, s, 112) = Vs_Find("pss2Sound");
    F(void*, s, 108) = Vs_Find("pssSound");
    F(void*, s, 116) = Vs_Find("antiradarSound");
    if (!F(void*, s, 116))
        F(void*, s, 116) = F(void*, s, 68);
    F(void*, s, 120) = Vs_Find("startglochSound");
    if (!F(void*, s, 120))
        F(void*, s, 120) = F(void*, s, 96);
    F(void*, s, 124) = Vs_Find("pssSound");
    if (F(void*, s, 124))
        F(void*, s, 124) = F(void*, s, 112);
    F(BYTE, s, 224) = 1;
    F(BYTE, s, 225) = 0;
    F(int, s, 228) = 0;
    F(void*, s, 128) = Vs_Find("kriakSound");
    if (!F(void*, s, 128))
        F(void*, s, 128) = F(void*, s, 92);
    F(void*, s, 176) = Vs_Find("squeakSound");
    F(void*, s, 132) = Vs_Find("hit_gluh_fSound");
    F(void*, s, 136) = Vs_Find("hit_gluh_mSound");
    F(void*, s, 140) = Vs_Find("hit_gluh_rSound");
    F(void*, s, 144) = Vs_Find("hit_gluh_squeakSound");
    F(void*, s, 148) = Vs_Find("hit_def1Sound");
    F(void*, s, 152) = Vs_Find("hit_def2Sound");
    F(void*, s, 156) = Vs_Find("hit_def_faraSound");
    F(void*, s, 160) = Vs_Find("hit_suspensionSound");
    F(void*, s, 164) = Vs_Find("hit_signSound");
    F(void*, s, 168) = Vs_Find("hit_ironpillarSound");
    F(void*, s, 172) = Vs_Find("hit_bulletSound");
}

} // extern "C"

// === drawing the world view ($L_4e0950)

extern "C" void FUN_4e298b(); KING_ALIAS("_FUN_4e298b", "_Vec3_LengthSq4e298b")
extern "C" void FUN_5dffc0(); KING_ALIAS("_FUN_5dffc0", "@View_DrawObject@4")
extern "C" void FUN_53d9d0(); KING_ALIAS("_FUN_53d9d0", "@Dash_DrawOverlays@8")    // the dashboard text overlay
extern "C" void FUN_586830(); KING_ALIAS("_FUN_586830", "@Item_AfterEvents@4")    // a moving object's per-frame step
KING_VAR(float, g_wv6969f4, 6969f4)
KING_VAR(float, g_wv6969f8, 6969f8)
KING_VAR(float, g_wv6969fc, 6969fc)
KING_VAR(float, g_wv64d7b4, 64d7b4)
KING_VAR(float, g_wv64d7b0, 64d7b0)
KING_VAR(BYTE*, g_wv69622c, 69622c)
KING_VAR(BYTE*, g_wv6d2078, 6d2078)
KING_VAR(AsmLabel, s_wv67334c, 67334c)

// the squared distance from point (x, y, z) to the position of o's matrix
static double Wv_DistSq(BYTE* o, float x, float y, float z)
{
    BYTE m[48];
    const float* p = (const float*)(CallThis<BYTE*>(FUN_57de90, o, m) + 36);
    float d[3] = {x - p[0], y - p[1], z - p[2]};
    return CallC<double>(FUN_4e298b, d);
}

extern "C" {

// [$L_4e0950] draws world view s (vtable +216 after the object), steps the moving objects (+372) and deletes a
// kind-3 one (+3568) that is far from the camera (6969f4) or from its own spot (+9588); then the dashboard text
// and the scene's objects named 67334c are removed
void __fastcall WorldView_Draw(BYTE* s)
{
    CallThis(FUN_5dffc0, s);
    CallVirt(s, 216);
    BYTE* p = g_sceneCommandList;
    if (p)
        for (p -= 4; p;) {
            BYTE* o = p - 376;
            if (F(int, p, -4))
                CallThis(FUN_586830, o);
            if (F(int, o, 392) == 2 && F(int, o, 3568) == 3) {
                if (Wv_DistSq(o, g_wv6969f4, g_wv6969f8, g_wv6969fc) > g_wv64d7b4 ||
                    Wv_DistSq(o, F(float, o, 9588), F(float, o, 9592), F(float, o, 9596)) > g_wv64d7b0) {
                    CallVirt(o, 28, 1);
                    return;
                }
            }
            p = F(BYTE*, p, 8);
            if (!p)
                break;
            p -= 4;
        }
    CallThis(FUN_53d9d0, g_wv6d2078);
    for (BYTE* e = CallVirt<BYTE*>(g_wv69622c, 64, 0); e;) {
        BYTE* next = F(BYTE*, e, 8);
        const char* name = F(const char*, e, 12);
        if (name && !strcmp(name, s_wv67334c)) {
            CallVirt(e, 36);
            CallThis(FUN_5e48f0, g_wv69622c);
            if (BYTE* x = F(BYTE*, e, 68))
                CallVirt(x, 0, 1);
            if (e)
                CallVirt(e, 0, 1);
        }
        e = next;
    }
}

} // extern "C"

// === a vector over a number, the force feedback effects (FUN_4e10d0, 4e1120, 4e1150)

extern "C" void Fe_ClearEffects(); KING_ALIAS("_Fe_ClearEffects", "_Joy_ClearEffects")
extern "C" void Fe_Periodic(); KING_ALIAS("_Fe_Periodic", "@FfPeriodic_Construct@4")
extern "C" void Fe_Condition(); KING_ALIAS("_Fe_Condition", "@FfCondition_Construct@12")
extern "C" void Fe_Damper(); KING_ALIAS("_Fe_Damper", "@FfDamper_Construct@4")
extern "C" void Fe_Constant(); KING_ALIAS("_Fe_Constant", "@FfConstant_Construct@12")
extern "C" void FUN_577ab0(); KING_ALIAS("_FUN_577ab0", "@FfForce_Construct@12")    // a ramp force effect (duration)

// a force effect of size bytes from constructor ctor (with arg when it takes one)
static int Fe_New(int size, void (*ctor)(), int arg, bool hasArg)
{
    BYTE* o = (BYTE*)operator new(size);
    if (!o)
        return 0;
    return hasArg ? CallThis<int>(ctor, o, arg) : CallThis<int>(ctor, o);
}

extern "C" {

// [FUN_4e10d0] *out = v / k
float* __cdecl Vec3_DivScalar(float* out, const float* v, float k)
{
    float x = (float)(v[0] / (double)k), y = (float)(v[1] / (double)k), z = (float)(v[2] / (double)k);
    out[0] = x;
    out[1] = y;
    out[2] = z;
    return out;
}

// (the entry 4e1120) clears the joystick's effects and forgets the six of the game (696cf0)
void __cdecl Ff_Forget()
{
    CallC(Fe_ClearEffects);
    for (int i = 0; i < 6; i++)
        g_696cf0[i] = 0;
}

// (the entry 4e1150) with a force feedback stick creates the game's six effects: periodic, condition (9000),
// damper, two ramps and a constant force (10000)
void __cdecl Ff_Create(int on)
{
    if (!on)
        return;
    g_696cf0[0] = Fe_New(16, Fe_Periodic, 0, false);
    g_696cf0[1] = Fe_New(40, Fe_Condition, 9000, true);
    g_696cf0[2] = Fe_New(12, Fe_Damper, 0, false);
    g_696cf0[3] = Fe_New(12, FUN_577ab0, 10000, true);
    g_696cf0[4] = Fe_New(12, FUN_577ab0, 10000, true);
    g_696cf0[5] = Fe_New(12, Fe_Constant, 10000, true);
}

} // extern "C"

// === a static frame's initializer ($L_4df8e0)

KING_VAR(AsmLabel, g_si696a00, 696a00)

extern "C" {

// [$L_4df8e0] the frame at 696a00: identity, +252 cleared, +256 = 15, +260 = 10
BYTE* __cdecl Init_696a00()
{
    BYTE* m = (BYTE*)g_si696a00;
    memset(m, 0, 48);
    memcpy(m, &g_identity, 48);
    F(int, m, 252) = 0;
    F(float, m, 256) = 15.0f;
    F(float, m, 260) = 10.0f;
    return m;
}

} // extern "C"

// === loading the helicopters ($L_5451c0 with its tail $L_545367) and the trash boxes ($L_52ebc0 with $L_52ee5e)
//     from the world's object records: the hooks in front of the handlers saved in 6ef24c / 6cf3a0

extern "C" void Ld_Find(); KING_ALIAS("_Ld_Find", "_Scene_FindObject")
extern "C" void Ld_Length(); KING_ALIAS("_Ld_Length", "_Vec3_Length")
extern "C" void Ld_Fatal(); KING_ALIAS("_Ld_Fatal", "_World_Fatal")
extern "C" void Ld_Copter(); KING_ALIAS("_Ld_Copter", "@Copter_Construct@20")
extern "C" void Ld_Nop(); KING_ALIAS("_Ld_Nop", "_Fw_Nop")
extern "C" void Ld_VehCopter(); KING_ALIAS("_Ld_VehCopter", "@Vehicle_ConstructCopter@28")
extern "C" void Ld_NetVeh(); KING_ALIAS("_Ld_NetVeh", "@NetVehicle_Construct@40")
extern "C" void Ld_Reset(); KING_ALIAS("_Ld_Reset", "@Copter5615f0_Reset@8")
extern "C" void Ld_Vec100(); KING_ALIAS("_Ld_Vec100", "@Copter_SetVec100@12")
extern "C" void Ld_Vec112(); KING_ALIAS("_Ld_Vec112", "@Copter_SetVec112@12")
extern "C" void Ld_Clear(); KING_ALIAS("_Ld_Clear", "@Copter561390_Clear@8")
extern "C" void Ld_Command(); KING_ALIAS("_Ld_Command", "@Copter_Command@16")
extern "C" void Ld_CarType(); KING_ALIAS("_Ld_CarType", "_CarType_FromNumber")
extern "C" BYTE* __cdecl Trash_CreateGuest(BYTE* w, BYTE* o1, BYTE* o2, BYTE* pos, int number, int num, int extra, int local);
extern "C" BYTE* __cdecl Trash_CreateHost(BYTE* w, BYTE* o1, BYTE* o2, BYTE* pos, int number, int num, int local);
KING_VAR(int, g_ld67f878, 67f878)
KING_VAR(BYTE*, g_ld6ef1fc, 6ef1fc)
KING_VAR(int, g_ld6ef208, 6ef208)
KING_VAR(int, g_ld6ce34c, 6ce34c)
KING_VAR(float, g_ld64d7b8, 64d7b8)
KING_VAR(float, g_ld64c2f8, 64c2f8)
KING_VAR(char, g_ldRoomFmt, 678604)
KING_VAR(char, g_ldObjFmt, 678620)
KING_VAR(char, g_ldVwFmt, 6785a4)
KING_VAR(char, g_ldTypeFmt, 6785d4)
KING_VAR(char, g_ldCopterFmt, 679f78)
KING_VAR(char, g_ldTrashFmt, 678588)

// the record's speed (+20) and spin (+32) within their limits
static bool Ld_Still(const BYTE* c)
{
    return !(CallC<double>(Ld_Length, c + 20) > g_ld64d7b8) && !(CallC<double>(Ld_Length, c + 32) > g_ld64c2f8);
}

static void Ld_VwText(char* buf, const BYTE* c)
{
    sprintf(buf, &g_ldVwFmt, (double)F(float, c, 20), (double)F(float, c, 24), (double)F(float, c, 28),
            (double)F(float, c, 32), (double)F(float, c, 36), (double)F(float, c, 40));
}

extern "C" {

// [$L_5451c0..$L_545367] object record rec (number +4, data +24) of kind 3 / 0: a helicopter in the scene object
// named at +96, still; with flags & 1 built: a local copter and its vehicle (or a network vehicle for data +12),
// numbered, made visible, its speed, spin, command (+4, +8) set, stored at rec +28
int __cdecl Copter_LoadHook(BYTE* rec, int flags)
{
    typedef int(__cdecl * Hook)(BYTE*, int);
    if (g_su6ef24c)
        if (int r = ((Hook)g_su6ef24c)(rec, flags))
            return r;
    int id = F(int, rec, 4);
    int num = id % 1000;
    if (id / 100000 != 3 || id % 100000 / 1000)
        return 0;
    if (!(flags & 1))
        return 1;
    BYTE c[160];
    memcpy(c, F(BYTE*, rec, 24), 160);
    BYTE* obj = CallC<BYTE*>(Ld_Find, 0, c + 96);
    if (!obj || !Ld_Still(c))
    {
        char room[100], vw[200], msg[500];
        sprintf(room, &g_ldRoomFmt, c + 96, obj);
        Ld_VwText(vw, c);
        sprintf(msg, &g_ldCopterFmt, room, vw);
        CallC(Ld_Fatal, msg);
    }
    g_ld67f878 = F(int, rec, 4);
    BYTE* cp = 0;
    BYTE* v;
    if (!F(int, c, 12))
    {
        BYTE* o = (BYTE*)operator new(3552);
        if (o)
            cp = CallThis<BYTE*>(Ld_Copter, o, obj, c + 44, F(int, c, 92));
        if (F(int, cp, 3548))
        {
            if (cp)
            {
                CallC(Ld_Nop, cp);
                operator delete(cp);
            }
            return 1;
        }
        BYTE* m = (BYTE*)operator new(21880);
        v = m ? CallThis<BYTE*>(Ld_VehCopter, m, g_ld6ef1fc, cp, g_ld6ef208, F(int, c, 0), 0) : 0;
    }
    else
    {
        BYTE* m = (BYTE*)operator new(21880);
        v = m ? CallThis<BYTE*>(Ld_NetVeh, m, g_ld6ef1fc, F(int, c, 0), obj, c + 44, c + 44, F(int, c, 16), 0,
                                g_ld6ef208)
              : 0;
    }
    if (!v)
        return 1;
    F(int, v, 20948) = num;
    F(int, v, 360) = F(int, rec, 4);
    CallVirt(v + 16, 24, 1);
    if (!F(int, c, 12))
    {
        CallThis(Ld_Reset, cp);
        CallThis(Ld_Vec100, cp, c + 20);
        CallThis(Ld_Vec112, cp, c + 32);
        CallThis(Ld_Clear, cp + 3372);
        CallThis(Ld_Command, cp, F(int, c, 4), F(int, c, 8));
    }
    F(BYTE*, rec, 28) = v;
    return 1;
}

// [$L_52ebc0..$L_52ee5e] object record rec of kind 2: a trash box of the car type numbered at data +0 between the
// scene objects named at +92 and +124, still; with flags & 1 built (moving for data +8), numbered, its speed and spin
// set, made visible for data +12, stored at rec +28
int __cdecl TrashBox_LoadHook(BYTE* rec, int flags)
{
    typedef int(__cdecl * Hook)(BYTE*, int);
    if (g_su6cf3a0)
        if (int r = ((Hook)g_su6cf3a0)(rec, flags))
            return r;
    int id = F(int, rec, 4);
    if (id / 100000 != 2)
        return 0;
    BYTE c[156];
    memcpy(c, F(BYTE*, rec, 24), 156);
    int num = id % 1000;
    int type = CallC<int>(Ld_CarType, F(int, c, 0));
    BYTE* o1 = CallC<BYTE*>(Ld_Find, 0, c + 92);
    BYTE* o2 = CallC<BYTE*>(Ld_Find, 0, c + 124);
    if (!o1 || !o2 || type < 0 || type >= g_ld6ce34c || !Ld_Still(c))
    {
        char obj[100], room[100], tp[100], vw[100], msg[500];
        sprintf(obj, &g_ldObjFmt, c + 124, o2);
        sprintf(room, &g_ldRoomFmt, c + 92, o1);
        sprintf(tp, &g_ldTypeFmt, 2, type, num, g_ld6ce34c + 1);
        Ld_VwText(vw, c);
        sprintf(msg, &g_ldTrashFmt, obj, room, tp, vw);
        CallC(Ld_Fatal, msg);
    }
    if (!(flags & 1))
        return 1;
    g_ld67f878 = F(int, rec, 4);
    BYTE* v;
    if (F(int, c, 8))
        v = Trash_CreateGuest(g_ld6ef1fc, o1, o2, c + 44, F(int, c, 0), num, F(int, c, 16), 0);
    else
        v = Trash_CreateHost(g_ld6ef1fc, o1, o2, c + 44, F(int, c, 0), num, 0);
    F(int, v, 344) = F(int, rec, 4);
    CallVirt(v, 144, c + 20, 0);
    CallVirt(v, 148, c + 32, 0);
    if (F(int, c, 12))
        CallVirt(v, 24, 1);
    F(BYTE*, rec, 28) = v;
    return 1;
}

} // extern "C"

// === the trash boxes made: a host's (FUN_4e1f10 with its tails up to $L_4e21a2) and a guest's (FUN_4e21c0 with
//     its tails up to $L_4e249e), a car, a bus or a truck by the type's range

extern "C" void Tb_CarH(); KING_ALIAS("_Tb_CarH", "@Car52b5f0_Construct@28")
extern "C" void Tb_CarHN(); KING_ALIAS("_Tb_CarHN", "@Car52b7c0_Construct@36")
extern "C" void Tb_TruckH(); KING_ALIAS("_Tb_TruckH", "@Truck52d350_Construct@28")
extern "C" void Tb_TruckHN(); KING_ALIAS("_Tb_TruckHN", "@Truck52d490_Construct@36")
extern "C" void Tb_BusH(); KING_ALIAS("_Tb_BusH", "@Bus52e3c0_Construct@32")
extern "C" void Tb_BusHN(); KING_ALIAS("_Tb_BusHN", "@Bus52e220_Construct@40")
extern "C" void Tb_CarG(); KING_ALIAS("_Tb_CarG", "@Car52b6f0_Construct@32")
extern "C" void Tb_CarGN(); KING_ALIAS("_Tb_CarGN", "@Car52b8a0_Construct@40")
extern "C" void Tb_TruckG(); KING_ALIAS("_Tb_TruckG", "@Truck52d400_Construct@32")
extern "C" void Tb_TruckGN(); KING_ALIAS("_Tb_TruckGN", "@Truck52d530_Construct@40")
extern "C" void Tb_BusG(); KING_ALIAS("_Tb_BusG", "@Bus52e470_Construct@36")
extern "C" void Tb_BusGN(); KING_ALIAS("_Tb_BusGN", "@Bus52e300_Construct@44")
extern "C" void __cdecl Bus_OnFare(void* bus, double amount);
KING_VAR(int, g_tb6ce350, 6ce350)
KING_VAR(int, g_tb6ce35c, 6ce35c)
KING_VAR(int, g_tb6ce360, 6ce360)
KING_VAR(int, g_tb6ce364, 6ce364)
KING_VAR(char, g_tbHost1, 6734ac)
KING_VAR(char, g_tbHost2, 67346c)
KING_VAR(char, g_tbGuest1, 673534)
KING_VAR(char, g_tbGuest2, 6734f4)

// the car type of number, a fatal error (fmt) when out of range
static int Tb_Type(int number, const BYTE* o2, const char* fmt)
{
    int type = CallC<int>(Ld_CarType, number);
    if (type < 0 || type >= g_ld6ce34c)
    {
        char buf[200];
        sprintf(buf, fmt, number, type, F(const char*, o2, 12));
        CallC(Ld_Fatal, buf);
    }
    return type;
}

// 0 car, 1 bus, 2 truck, -1 none
static int Tb_Kind(int type)
{
    if (type >= g_tb6ce350 && type < g_tb6ce35c)
        return 0;
    if (type == g_tb6ce35c)
        return 1;
    if (type >= g_tb6ce360 && type < g_tb6ce364)
        return 2;
    return -1;
}

static BYTE* Tb_Check(BYTE* v, int type, int num, const BYTE* o2, const char* fmt)
{
    if (!v)
    {
        char buf[200];
        sprintf(buf, fmt, type, num, F(const char*, o2, 12));
        CallC(Ld_Fatal, buf);
    }
    return v;
}

extern "C" {

// [FUN_4e1f10..$L_4e21a2] a host's trash box of car number `number` between scene objects o1 / o2 at pos in world w
BYTE* __cdecl Trash_CreateHost(BYTE* w, BYTE* o1, BYTE* o2, BYTE* pos, int number, int num, int local)
{
    int type = Tb_Type(number, o2, &g_tbHost1);
    int kind = Tb_Kind(type);
    BYTE* v = 0;
    if (kind >= 0)
    {
        BYTE* m = (BYTE*)operator new(kind == 1 ? 3656 : 9552);
        if (m)
        {
            if (local == 1)
            {
                if (kind == 1)
                    v = CallThis<BYTE*>(Tb_BusH, m, w, o2, type, num, 1, (void*)Bus_OnFare);
                else
                    v = CallThis<BYTE*>(kind ? Tb_TruckH : Tb_CarH, m, w, o2, type, num, 1);
            }
            else if (kind == 1)
                v = CallThis<BYTE*>(Tb_BusHN, m, w, o1, o2, pos, type, num, 0, (void*)Bus_OnFare);
            else
                v = CallThis<BYTE*>(kind ? Tb_TruckHN : Tb_CarHN, m, w, o1, o2, pos, type, num, 0);
        }
    }
    return Tb_Check(v, type, num, o2, &g_tbHost2);
}

// [FUN_4e21c0..$L_4e249e] a guest's trash box, as Trash_CreateHost with the extra value; local only 0 or 1
BYTE* __cdecl Trash_CreateGuest(BYTE* w, BYTE* o1, BYTE* o2, BYTE* pos, int number, int num, int extra, int local)
{
    int type = Tb_Type(number, o2, &g_tbGuest1);
    int kind = (local == 0 || local == 1) ? Tb_Kind(type) : -1;
    BYTE* v = 0;
    if (kind >= 0)
    {
        BYTE* m = (BYTE*)operator new(kind == 1 ? 3656 : 9552);
        if (m)
        {
            if (local == 1)
            {
                if (kind == 1)
                    v = CallThis<BYTE*>(Tb_BusG, m, w, o2, type, num, extra, 1, (void*)Bus_OnFare);
                else
                    v = CallThis<BYTE*>(kind ? Tb_TruckG : Tb_CarG, m, w, o2, type, num, extra, 1);
            }
            else if (kind == 1)
                v = CallThis<BYTE*>(Tb_BusGN, m, w, o1, o2, pos, type, num, extra, 0, (void*)Bus_OnFare);
            else
                v = CallThis<BYTE*>(kind ? Tb_TruckGN : Tb_CarGN, m, w, o1, o2, pos, type, num, extra, 0);
        }
    }
    return Tb_Check(v, type, num, o2, &g_tbGuest2);
}

} // extern "C"

// === the application's start ($L_4e19b0 with its tails up to $L_4e1d07, a vtable entry)

extern "C" void Ai_FindCd(); KING_ALIAS("_Ai_FindCd", "_Startup_FindCd")
extern "C" void Ai_MapSymbols(); KING_ALIAS("_Ai_MapSymbols", "_World_ReadMapSymbols")
extern "C" void Ai_Controls(); KING_ALIAS("_Ai_Controls", "_Controls_ReadDefFile")
extern "C" void Ai_Fatal(); KING_ALIAS("_Ai_Fatal", "_World_Fatal")
extern "C" void Ai_Options(); KING_ALIAS("_Ai_Options", "_Startup_ReadOptions")
extern "C" void Ai_Joy(); KING_ALIAS("_Ai_Joy", "_Joy_Init")
extern "C" void Ai_Presets(); KING_ALIAS("_Ai_Presets", "_Controls_ListPresets")
extern "C" void Ai_Preset(); KING_ALIAS("_Ai_Preset", "_Controls_SelectPreset")
extern "C" void Ai_FfHooks(); KING_ALIAS("_Ai_FfHooks", "_Mmt_Set6f4ba4")
extern "C" void Ai_FfForget(); KING_ALIAS("_Ai_FfForget", "_Ff_Forget")
extern "C" void Ai_FfCreate(); KING_ALIAS("_Ai_FfCreate", "_Ff_Create")
extern "C" void Ai_Dump(); KING_ALIAS("_Ai_Dump", "_Dump_Text")
extern "C" void Ai_Wnd(); KING_ALIAS("_Ai_Wnd", "@Wnd4e01d0_Construct@12")
extern "C" void Ai_AddActive(); KING_ALIAS("_Ai_AddActive", "@Windows_AddActive@12")
extern "C" void Ai_Add(); KING_ALIAS("_Ai_Add", "@Windows_Add@12")
extern "C" void Ai_Add2(); KING_ALIAS("_Ai_Add2", "@Windows_Add2@12")
extern "C" void Ai_WasmCreate(); KING_ALIAS("_Ai_WasmCreate", "@Wasm597410_Create@8")
extern "C" void Ai_Wasm598d80(); KING_ALIAS("_Ai_Wasm598d80", "@Wasm_598d80@8")
extern "C" void Ai_NodeN(); KING_ALIAS("_Ai_NodeN", "@NodeN_Construct@20")
extern "C" void Ai_View3D(); KING_ALIAS("_Ai_View3D", "@View3D_Construct@24")
extern "C" void Ai_Music(); KING_ALIAS("_Ai_Music", "_Obj6968ac_Clear")
extern "C" void Ai_Cluster(); KING_ALIAS("_Ai_Cluster", "@Cluster_Construct@8")
extern "C" void Ai_ObjFind(); KING_ALIAS("_Ai_ObjFind", "_Objects_Find")
extern "C" void Ai_Surface(); KING_ALIAS("_Ai_Surface", "@Window_SetSurface@12")
ASM_FUNC(Ai_World, FUN_4e24e8)
extern "C" void Ai_ClusterLoad(); KING_ALIAS("_Ai_ClusterLoad", "@Image_ReadResources@24")
extern "C" void Ai_Menu(); KING_ALIAS("_Ai_Menu", "@GameWindow_Construct@12")
extern "C" int __cdecl _stricmp(const char* a, const char* b);
KING_VAR(char, g_aiNewLines, 673414)
KING_VAR(char, g_aiDot, 671204)
KING_VAR(int, g_ai696c74, 696c74)
KING_VAR(void*, g_ai6f4bac, 6f4bac)
KING_VAR(int, g_ai696c58, 696c58)
KING_VAR(int, g_ai696ca4, 696ca4)
KING_VAR(BYTE*, g_ai696ca0, 696ca0)
KING_VAR(BYTE, g_ai696750, 696750)
KING_VAR(BYTE, g_ai6f68b0, 6f68b0)
KING_VAR(BYTE, g_ai68d1c0, 68d1c0)
KING_VAR(AsmLabel, g_ai7025f0, 7025f0)
KING_VAR(AsmLabel, g_ai64d7d0, 64d7d0)
KING_VAR(AsmLabel, g_ai64d7bc, 64d7bc)
KING_VAR(BYTE*, g_ai696ca8, 696ca8)
KING_VAR(BYTE*, g_ai696cac, 696cac)
KING_VAR(BYTE*, g_ai696cb0, 696cb0)
KING_VAR(int, g_ai696d1c, 696d1c)
struct AiRect { int d[4]; };
KING_VAR(AiRect, g_ai720938, 720938)

extern "C" {

// [$L_4e19b0..$L_4e1d07] application a started (argc, argv; "/VAS" refused): the mutex, the CD, the map symbols,
// the controls and options, the window (480 lines high at most, centered), the joystick and force feedback,
// the world, the scene objects, the 3D view, the music, load.res and screen2.txr, the menu
void __fastcall App_Start(BYTE* a, void*, int argc, char** argv)
{
    CreateMutexA(0, 1, "kotr_app_mutex");
    CallC(Ai_FindCd);
    CallC(Ai_MapSymbols);
    CallC(Ai_Controls);
    if (argc >= 2 && !_stricmp(argv[1], "/VAS"))
        CallC(Ai_Fatal, &g_aiNewLines);
    CallC(Ai_Options);
    int h = F(int, a, 260), w = F(int, a, 256);
    int top = 90, bottom = 390;
    if (h < 480)
    {
        top = (480 - h) / 2;
        bottom = top + 300;
        if (top < 0)
            top = 0;
        if (bottom > h)
            bottom = h;
    }
    if (g_ai696c74)
        g_ai6f4bac = (void*)Ai_Dump;
    g_ai696ca4 = CallC<int>(Ai_Joy, F(int, a, 388), g_ai696c58 * 100, 0) == 0;
    CallC(Ai_Presets);
    CallC(Ai_Preset, 0);
    CallC(Ai_FfHooks, (void*)Ai_FfForget, (void*)Ai_FfCreate);
    int rect[4] = {0, top, w, bottom};
    BYTE* p = (BYTE*)operator new(9408);
    if (p)
        p = CallThis<BYTE*>(Ai_Wnd, p, rect);
    g_ai696ca0 = p;
    CallThis(Ai_AddActive, a, p);
    CallC(Ai_World, g_ai696ca0, &g_ai696750, F(int, a, 388));
    F(DWORD, a, 292) |= 8;
    CallVirt(g_ai696ca0, 32);
    CallThis(Ai_WasmCreate, &g_ai6f68b0);
    CallThis(Ai_Wasm598d80, &g_ai6f68b0);
    BYTE* n = (BYTE*)operator new(112);
    if (n)
    {
        CallThis(Ai_NodeN, n, &g_ai68d1c0, 6, 1);
        memset(n + 64, 0, 48);
        memcpy(n + 64, &g_ai7025f0, 48);
        F(void*, n, 0) = &g_ai64d7d0;
        F(int, n, 56) = 0;
        F(int, n, 60) = 0;
    }
    g_ai696ca8 = n;
    BYTE* o = (BYTE*)operator new(104);
    if (o)
    {
        F(int, o, 4) = 0;
        F(BYTE*, o, 80) = g_ai696ca8;
        F(int, o, 8) = 0;
        F(void*, o, 0) = &g_ai64d7bc;
        F(int, o, 76) = 0;
        F(float, o, 88) = 1.0f;
        F(float, o, 84) = 1.0f;
        F(int, o, 96) = 0;
        F(int, o, 92) = 0;
        F(BYTE, o, 44) = 0;
        F(BYTE, o, 12) = 0;
        F(int, o, 100) = 0;
    }
    g_ai696cac = o;
    BYTE* v = (BYTE*)operator new(5152);
    if (v)
        v = CallThis<BYTE*>(Ai_View3D, v, g_ai720938);
    g_ai696cb0 = v;
    CallThis(Ai_Add, a, v);
    CallVirt(g_ai696cb0, 32);
    CallC(Ai_Music, "truck.mid");
    BYTE* c = (BYTE*)operator new(180);
    if (c)
        c = CallThis<BYTE*>(Ai_Cluster, c);
    F(BYTE*, g_ai696ca0, 9396) = c;
    FILE* f = fopen("load.res", "rb");
    if (!f)
        CallC(Ai_Fatal, "LOAD.RES not found");
    CallThis(Ai_ClusterLoad, F(BYTE*, g_ai696ca0, 9396), &g_aiDot, f, 2, 0);
    g_ai696d1c = CallC<int>(Ai_ObjFind, "screen2.txr");
    CallThis(Ai_Surface, F(BYTE*, F(BYTE*, a, 252), 12), F(int, F(BYTE*, F(BYTE*, g_ai696ca0, 9396), 100), 0));
    BYTE* m = (BYTE*)operator new(424);
    if (m)
        m = CallThis<BYTE*>(Ai_Menu, m, F(BYTE*, a, 252));
    CallThis(Ai_Add2, a, m);
    F(DWORD, a, 292) |= 3;
}

} // extern "C"

// === a host's tree box made ($L_52ef80 with its tail $L_52efbd, a table entry)

extern "C" void Tr_Fatal(); KING_ALIAS("_Tr_Fatal", "_World_Fatal")
extern "C" void Tr_Car(); KING_ALIAS("_Tr_Car", "@Car52d650_Construct@48")
extern "C" void Tr_Query(); KING_ALIAS("_Tr_Query", "_Query_4e6749")
extern "C" void Tr_Length(); KING_ALIAS("_Tr_Length", "_Vec3_Length")
extern "C" void Tr_Norm(); KING_ALIAS("_Tr_Norm", "_Vec3_Normalized")
extern "C" void Tr_Set(); KING_ALIAS("_Tr_Set", "@Vec3_SetThis@20")
extern "C" void Tr_Scaled(); KING_ALIAS("_Tr_Scaled", "_Vec3_Scaled")
extern "C" void Tr_M2(); KING_ALIAS("_Tr_M2", "@SceneObj_Matrix2@12")
extern "C" void Tr_RotBy(); KING_ALIAS("_Tr_RotBy", "_Vec3_RotateBy")
KING_VAR(Vec3, g_trUp, 702630)
KING_VAR(int, g_tr6ce364, 6ce364)
KING_VAR(float, g_tr64c968, 64c968)
KING_VAR(float, g_tr64c450, 64c450)
KING_VAR(float, g_tr64d7b8, 64d7b8)
KING_VAR(float, g_tr64c264, 64c264)
KING_VAR(float, g_tr64c260, 64c260)

// p + U * k, the components after x rounded on the way
static Vec3 Tr_Above(const float* p, float k)
{
    float y = (float)((double)g_trUp.y * k), z = (float)((double)g_trUp.z * k);
    Vec3 r = {(float)((double)g_trUp.x * k + p[0]), (float)((double)y + p[1]), (float)((double)z + p[2])};
    return r;
}

static Vec3 Tr_SetV(double x, double y, double z)
{
    Vec3 r;
    CallThis(Tr_Set, &r, (float)x, (float)y, (float)z);
    return r;
}

extern "C" {

// [$L_52ef80..$L_52efbd] tree box (a car of the last type) in room room at frame m; 0 for guests (g); its ground
// below (normal d); the first wheel with ground contact under 64d7b8 and a speed gets a push across the slope
// (speed +144, spin +148); the two old objects o1, o2 are freed
BYTE* __cdecl TreeBox_CreateHost(BYTE* tb, const float* m, BYTE* room, int a4, BYTE* o1, BYTE* o2, int g)
{
    if (!room || !tb)
        CallC(Tr_Fatal, "createHostTreeBox(): treebox==NULL||room==NULL");
    if (g)
        return 0;
    BYTE* v = (BYTE*)operator new(9600);
    if (v)
        v = CallThis<BYTE*>(Tr_Car, v, g_ld6ef1fc, tb, room, m, g_tr6ce364, 0, 0, 0, 1.0f, 0);
    CallVirt(v, 24, 1);
    Vec3 to = Tr_Above(m + 9, g_tr64c968);
    Vec3 P = {m[9], m[10], m[11]};
    Vec3 d = g_trUp;
    Vec3 from = Tr_Above(m + 9, g_tr64c450);
    CallC(Tr_Query, tb, &from, &to, &P, &d);
    int i = 0;
    for (; i < 6; i++)
        if (F(int, v, 3508 + 4 * i) && F(float, v, 3532 + 4 * i) < g_tr64d7b8)
            break;
    if (i < 6)
    {
        Vec3 V = F(Vec3, F(BYTE*, v, 3508 + 4 * i), 452);
        double lv = CallC<double>(Tr_Length, &V);
        float len = (float)lv;
        if (lv > g_tr64c264)
        {
            double c0 = (double)V.z * d.y - (double)d.z * V.y;
            double c1 = (double)d.z * V.x - (double)V.z * d.x;
            double c2 = (double)V.y * d.x - (double)d.y * V.x;
            Vec3 E = {(float)(c1 * d.z - c2 * d.y), (float)(c2 * d.x - d.z * c0), (float)(d.y * c0 - c1 * d.x)};
            Vec3 N;
            CallC(Tr_Norm, &N, &E);
            Vec3 G = Tr_SetV((double)g_trUp.x * g_tr64c260, (double)g_trUp.y * g_tr64c260, (double)g_trUp.z * g_tr64c260);
            Vec3 H = Tr_SetV((double)G.x + N.x, (double)G.y + N.y, (double)G.z + N.z);
            Vec3 Hn;
            const float* hn = &CallC<const Vec3*>(Tr_Norm, &Hn, &H)->x;
            Vec3 K = Tr_SetV((double)len * hn[0], (double)len * hn[1], (double)len * hn[2]);
            Vec3 L = Tr_SetV((double)K.x * g_tr64c260, (double)K.y * g_tr64c260, (double)K.z * g_tr64c260);
            Vec3 M = Tr_SetV((double)N.y * d.z - (double)N.z * d.y, (double)N.z * d.x - (double)d.z * N.x,
                             (double)d.y * N.x - (double)N.y * d.x);
            Vec3 s1, W;
            const Vec3* r1 = CallC<const Vec3*>(Tr_Scaled, &s1, &M, len);
            CallC(Tr_Scaled, &W, r1, 0.01f);
            Matrix t;
            const Matrix* M2 = CallThis<const Matrix*>(Tr_M2, v, &t);
            Vec3 RW;
            CallC(Tr_RotBy, &RW, &W, M2);
            CallVirt(v, 144, &L, 0);
            CallVirt(v, 148, &RW, 0);
        }
    }
    if (o1)
    {
        CallVirt(o1, 36);
        CallVirt(o1, 0, 1);
    }
    if (o2)
    {
        CallVirt(o2, 36);
        CallVirt(o2, 0, 1);
    }
    return v;
}

} // extern "C"

// === a bus's fare earned ($L_576670, the buses' callback)

extern "C" void Bf_Mode(); KING_ALIAS("_Bf_Mode", "@Shell_SetMode@12")
extern "C" void Bf_MsgNew(); KING_ALIAS("_Bf_MsgNew", "@DpMessage_Construct@8")
extern "C" void Bf_Write(); KING_ALIAS("_Bf_Write", "@Msg_Write120@20")
extern "C" void Bf_Send(); KING_ALIAS("_Bf_Send", "_Dp_Send")
extern "C" void Bf_MsgFree(); KING_ALIAS("_Bf_MsgFree", "@MsgBuf_Destroy@8")
extern "C" void Bf_Text(); KING_ALIAS("_Bf_Text", "_Table71e858_Get")
extern "C" void Bf_Message(); KING_ALIAS("_Bf_Message", "_Messages_Add")
KING_VAR(int, g_bf696900, 696900)
KING_VAR(double, g_bfMoney, 6f3338)
KING_VAR(double, g_bf64b730, 64b730)
KING_VAR(BYTE*, g_bf6ceca8, 6ceca8)

static void Bf_SendMsg(int code, const BYTE* buf, int n)
{
    __declspec(align(8)) BYTE msg[32];
    CallThis(Bf_MsgNew, msg);
    CallThis(Bf_Write, msg, code, buf, n);
    BYTE* p = F(BYTE*, msg, 4);
    CallC(Bf_Send, p, (int)(F(BYTE*, msg, 8) - p));
    CallThis(Bf_MsgFree, msg);
}

extern "C" {

// [$L_576670] the local player's money (6f3338 by player) grows by amount: reaching 64b730 ends the game (mode 3,
// messages 953 with the sum and 952), else the earned message and 953
void __cdecl Bus_OnFare(void*, double amount)
{
    int pl = g_bf696900;
    double* m = &g_bfMoney + pl;
    *m += amount;
    BYTE buf[8];
    if (!(*m < g_bf64b730))
    {
        CallThis(Bf_Mode, g_bf6ceca8, 3);
        buf[0] = (BYTE)pl;
        int t = (int)*m;
        memcpy(buf + 1, &t, 4);
        Bf_SendMsg(953, buf, 5);
        buf[0] = 3;
        Bf_SendMsg(952, buf, 3);
        return;
    }
    char text[68];
    sprintf(text, CallC<const char*>(Bf_Text, 86), (int)*m);
    CallC(Bf_Message, text);
    buf[0] = (BYTE)pl;
    int t = (int)*m;
    memcpy(buf + 1, &t, 4);
    Bf_SendMsg(953, buf, 5);
}

} // extern "C"

// === the game's main control window and its shell screens (FUN_527f30 with its tail $L_5280e2)

extern "C" BYTE* __fastcall Mn_WindowCtor(BYTE* s, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8); KING_ALIAS("@Mn_WindowCtor@40", "@ControlWindow_Construct@40")
extern "C" void* __cdecl Mn_LoadFonts(); KING_ALIAS("_Mn_LoadFonts", "_Carat_LoadFonts")
extern "C" void __cdecl Mn_Set720080(const char* s); KING_ALIAS("_Mn_Set720080", "_Set720080")
extern "C" BYTE* __fastcall Mn_OverlaysLoad(BYTE* s, void*, const char* path, BYTE* o, int size); KING_ALIAS("@Mn_OverlaysLoad@20", "@Overlays_Load@20")
extern "C" BYTE* __cdecl Mn_Find(const char* name); KING_ALIAS("_Mn_Find", "_Objects_Find")
extern "C" double __cdecl Mn_Now(); KING_ALIAS("_Mn_Now", "_Tsc_Now")
extern "C" BYTE* __cdecl Mn_FontLoad(const char* name); KING_ALIAS("_Mn_FontLoad", "_Font_LoadFile")
extern "C" void __cdecl Mn_Exit(const char* msg); KING_ALIAS("_Mn_Exit", "_Sys_Exit")
extern "C" DWORD __fastcall Mn_StyleAssign(void* self, void*, const void* s); KING_ALIAS("@Mn_StyleAssign@12", "@Style_Assign@12")
extern "C" BYTE* __fastcall Mn_ControlCtor(BYTE* s, void*, const char* name, int id); KING_ALIAS("@Mn_ControlCtor@16", "@Control_Construct@16")
extern "C" BYTE* __cdecl Mn_Button(const char* name, int id, int x, int y); KING_ALIAS("_Mn_Button", "_Shell_MakeMaskButton")
extern "C" void __fastcall Mn_AddChild(BYTE* self, void*, BYTE* child, BYTE* after); KING_ALIAS("@Mn_AddChild@16", "@Ctl_AddChild@16")
extern "C" void __fastcall Mn_Select(BYTE* self, void*, BYTE* child); KING_ALIAS("@Mn_Select@12", "@Ctl_Select@12")
extern "C" void __cdecl Mn_Nop(void* p); KING_ALIAS("_Mn_Nop", "_Fw_Nop")
extern "C" void __fastcall Mn_ClearFlags(BYTE* s, void*); KING_ALIAS("@Mn_ClearFlags@8", "@Window_ClearFlags@8")
extern "C" void __fastcall Mn_Activate(BYTE* s, void*); KING_ALIAS("@Mn_Activate@8", "@Win_Activate@8")
extern "C" BYTE* __fastcall Mn_WindowsAdd(BYTE* self, void*, BYTE* w); KING_ALIAS("@Mn_WindowsAdd@12", "@Windows_Add2@12")
typedef BYTE* (__fastcall* Mn_ScreenCtor)(BYTE* s, void*, BYTE* app);
extern "C" BYTE* __fastcall Mn_Controls(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Controls@12", "@ShellControls513b30_Construct@12")
extern "C" BYTE* __fastcall Mn_Display(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Display@12", "@ShellDisplay515ca0_Construct@12")
extern "C" BYTE* __fastcall Mn_Keys(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Keys@12", "@KeysScreen_Construct@12")
extern "C" BYTE* __fastcall Mn_Arrows(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Arrows@12", "@ShellArrows_Construct@12")
extern "C" BYTE* __fastcall Mn_Join(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Join@12", "@ShellJoin518d20_Construct@12")
extern "C" BYTE* __fastcall Mn_Providers(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Providers@12", "@ShellProviders51a580_Construct@12")
extern "C" BYTE* __fastcall Mn_DisplayOpt(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_DisplayOpt@12", "@DisplayOptions_Construct@12")
extern "C" BYTE* __fastcall Mn_Session(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_Session@12", "@ShellSession51b640_Construct@12")
extern "C" BYTE* __fastcall Mn_VideoOpt(BYTE*, void*, BYTE*); KING_ALIAS("@Mn_VideoOpt@12", "@VideoOptions_Construct@12")
extern "C" void Mn_SetView(); KING_ALIAS("_Mn_SetView", "_Carat_SetView")
extern "C" void Mn_Leave(); KING_ALIAS("_Mn_Leave", "_Game_Leave")
extern "C" void __cdecl srand(unsigned seed);
extern "C" __declspec(dllimport) long __stdcall CoInitialize(void* reserved);

KING_VAR(int, g_mn720938, 720938)
KING_VAR(int, g_mn72093c, 72093c)
KING_VAR(int, g_mn720940, 720940)
KING_VAR(int, g_mn720944, 720944)
KING_VAR(BYTE, g_mn64eb10, 64eb10)
KING_VAR(BYTE, g_mn64ead0, 64ead0)
KING_VAR(BYTE, g_mn64dc50, 64dc50)
KING_VAR(char, g_mn678358, 678358)
KING_VAR(char, g_mn67834c, 67834c)
KING_VAR(char, g_mn678340, 678340)
KING_VAR(char, g_mn67833c, 67833c)
KING_VAR(char, g_mn6729a8, 6729a8)
KING_VAR(char, g_mn672998, 672998)
KING_VAR(char, g_mn672994, 672994)
KING_VAR(char, g_mn676220, 676220)
KING_VAR(char, g_mn676820, 676820)
KING_VAR(char, g_mn67832c, 67832c)
KING_VAR(char, g_mn678320, 678320)
KING_VAR(char, g_mn676c9c, 676c9c)
KING_VAR(char, g_mn678310, 678310)
KING_VAR(char, g_mn67663c, 67663c)
KING_VAR(char, g_mn678308, 678308)
KING_VAR(char, g_mn678300, 678300)
KING_VAR(char, g_mn6782f8, 6782f8)
KING_VAR(char, g_mn6782f0, 6782f0)
KING_VAR(char, g_mn6782e8, 6782e8)
KING_VAR(char, g_mn6782e0, 6782e0)
KING_VAR(char, g_mn6782d8, 6782d8)
KING_VAR(char, g_mn6782d0, 6782d0)
KING_VAR(char, g_mn6733c4, 6733c4)
KING_VAR(char, g_mn6733cc, 6733cc)
KING_VAR(char, g_mn6733d4, 6733d4)
KING_VAR(char, g_mn6733bc, 6733bc)
KING_VAR(char, g_mn6782c0, 6782c0)
KING_VAR(char, g_mn6782b0, 6782b0)
KING_VAR(char, g_mn6782a0, 6782a0)
KING_VAR(BYTE*, g_mnApp696ca0, 696ca0)
KING_VAR(const char*, g_mnIni696868, 696868)
KING_VAR(BYTE*, g_mn6cecf4, 6cecf4)
KING_VAR(void*, g_mn6ced90, 6ced90)
KING_VAR(void*, g_mn67fb7c, 67fb7c)
KING_VAR(int, g_mn6ced48, 6ced48)
KING_VAR(int, g_mn696d1c, 696d1c)
KING_VAR(int, g_mn6ce3ac, 6ce3ac)
KING_VAR(BYTE*, g_mn6ce958, 6ce958)
KING_VAR(BYTE, g_mn6ce938, 6ce938)
KING_VAR(BYTE**, g_mn6cec60, 6cec60)
KING_VAR(void*, g_mn702f4c, 702f4c)
KING_VAR(int, g_mn6ce408, 6ce408)
KING_VAR(int, g_mn6ce40c, 6ce40c)
KING_VAR(int, g_mn6ce410, 6ce410)
KING_VAR(int, g_mn6ce414, 6ce414)
KING_VAR(int, g_mn6ce3b8, 6ce3b8)
KING_VAR(int, g_mn6ce3bc, 6ce3bc)
KING_VAR(int, g_mn6ce988, 6ce988)
KING_VAR(int, g_mn6ce990, 6ce990)
KING_VAR(int, g_mn6ce994, 6ce994)
KING_VAR(int, g_mn6ceb38, 6ceb38)
KING_VAR(int, g_mn6ceb3c, 6ceb3c)
KING_VAR(int, g_mn6ce430, 6ce430)
KING_VAR(int, g_mn6ce434, 6ce434)
KING_VAR(float, g_mn6ce3f8, 6ce3f8)
KING_VAR(int, g_mn6ce3e0, 6ce3e0)
KING_VAR(int, g_mn6ce3e4, 6ce3e4)
KING_VAR(int, g_mn6ce3e8, 6ce3e8)
KING_VAR(int, g_mn6ce3ec, 6ce3ec)
KING_VAR(float, g_mn6ce978, 6ce978)
KING_VAR(int, g_mn6ce960, 6ce960)
KING_VAR(int, g_mn6ce964, 6ce964)
KING_VAR(int, g_mn6ce968, 6ce968)
KING_VAR(int, g_mn6ce96c, 6ce96c)
KING_VAR(BYTE*, g_mn6cec80, 6cec80)
KING_VAR(BYTE, g_mnWindows696750, 696750)
KING_VAR(BYTE*, g_mn6cec6c, 6cec6c)
KING_VAR(BYTE*, g_mn6cec70, 6cec70)
KING_VAR(BYTE*, g_mn6cec7c, 6cec7c)
KING_VAR(BYTE*, g_mn6cec8c, 6cec8c)
KING_VAR(BYTE*, g_mn6cec90, 6cec90)
KING_VAR(BYTE*, g_mn6cec94, 6cec94)
KING_VAR(BYTE*, g_mn6cec98, 6cec98)
KING_VAR(BYTE*, g_mn6cecd8, 6cecd8)
KING_VAR(BYTE*, g_mn6cec9c, 6cec9c)
KING_VAR(DWORD, g_mn696874, 696874)
KING_VAR(BYTE*, g_mn6f2ed0, 6f2ed0)
KING_VAR(BYTE*, g_mn6f2ed4, 6f2ed4)
KING_VAR(BYTE*, g_mn6f2ed8, 6f2ed8)
KING_VAR(BYTE*, g_mn6f2ee8, 6f2ee8)
KING_VAR(BYTE*, g_mn6f2eec, 6f2eec)
KING_VAR(BYTE*, g_mn6f2ef0, 6f2ef0)

// a shell screen: built for app, added to the window list and shown once (vtable +32)
static BYTE* Mn_Screen(unsigned size, Mn_ScreenCtor ctor, BYTE* app)
{
    BYTE* p = (BYTE*)operator new(size);
    if (p)
        p = ctor(p, 0, app);
    return p;
}

static void Mn_AddScreen(BYTE*& g, BYTE* p)
{
    g = p;
    Mn_WindowsAdd(&g_mnWindows696750, 0, p);
    CallVirt(g, 32);
}

static BYTE** Mn_Holder(const char* name)
{
    BYTE** p = (BYTE**)operator new(12);
    if (p)
    {
        p[0] = Mn_Find(name);
        p[1] = 0;
        p[2] = 0;
    }
    return p;
}

extern "C" {

// [FUN_527f30..$L_5280e2] the game's control window s for app: its fonts, overlays and objects, the random seed,
// the menu's styles and its eight buttons (101..108, 102 selected), the shell screens (controls, display, keys,
// arrows, join, providers, display and video options, session) and the ini's choice for the controls screen (+1348)
BYTE* __fastcall GameWindow_Construct(BYTE* s, void*, BYTE* app)
{
    char buf[136];
    void* fonts = Mn_LoadFonts();
    Mn_WindowCtor(s, 0, (int)app, g_mn720938, g_mn72093c, g_mn720940, g_mn720944, 8, (int)fonts, 0);
    F(void*, s, 0) = &g_mn64eb10;
    F(void*, s, 64) = &g_mn64ead0;
    Mn_Set720080(&g_mn678358);
    BYTE* ov = (BYTE*)operator new(20);
    if (ov)
        ov = Mn_OverlaysLoad(ov, 0, &g_mn67834c, F(BYTE*, g_mnApp696ca0, 9396), 0);
    F(BYTE*, F(BYTE*, g_mnApp696ca0, 9396), 136) = ov;
    g_mn6cecf4 = Mn_Find(&g_mn678340);
    if (g_mn6cecf4)
        CallVirt(g_mn6cecf4, 40);
    F(int, s, 372) = 0;
    g_mn6ced90 = g_mn67fb7c;
    g_mn67fb7c = (void*)&Mn_Leave;
    GetPrivateProfileStringA(&g_mn672998, &g_mn67833c, &g_mn6729a8, buf, 4, g_mnIni696868);
    g_mn6ced48 = strcmp(buf, &g_mn672994) == 0;
    srand((int)Mn_Now());
    if (F(int, g_mnApp696ca0, 2864) == 0)
    {
        BYTE* font = Mn_FontLoad(&g_mn676220);
        if (!font)
            Mn_Exit(&g_mn676820);
        F(BYTE*, g_mnApp696ca0, 2864) = font;
    }
    F(BYTE*, s, 368) = Mn_Find(&g_mn67832c);
    BYTE* o = Mn_Find(&g_mn678320);
    g_mn6ce3ac = g_mn696d1c;
    g_mn6ce958 = o;
    Mn_StyleAssign(s + 132, 0, &g_mn6ce938);
    Mn_StyleAssign(s + 168, 0, &g_mn6ce938);
    Mn_StyleAssign(s + 204, 0, &g_mn6ce938);
    Mn_StyleAssign(s + 240, 0, &g_mn6ce938);
    BYTE** h = Mn_Holder(&g_mn676c9c);
    F(BYTE**, s, 52) = h;
    F(BYTE**, s, 48) = h;
    g_mn6cec60 = Mn_Holder(&g_mn678310);

    g_mn702f4c = (void*)&Mn_SetView;
    g_mn6ce40c = 6;
    g_mn6ce408 = 2;
    g_mn6ce414 = 0;
    g_mn6ce410 = 4;
    Mn_StyleAssign(&g_mn6ce3b8, 0, &g_mn6ce408);
    g_mn6ce3bc = 7;
    Mn_StyleAssign(&g_mn6ce988, 0, &g_mn6ce3b8);
    g_mn6ce994 = 4;
    g_mn6ce990 = 0;
    Mn_StyleAssign(&g_mn6ceb38, 0, &g_mn6ce988);
    g_mn6ceb3c = 6;
    Mn_StyleAssign(&g_mn6ce430, 0, &g_mn6ceb38);
    g_mn6ce434 = 3;
    g_mn6ce3f8 = 0.5f;
    g_mn6ce3e4 = 4;
    g_mn6ce3e0 = 0;
    g_mn6ce3ec = 0;
    g_mn6ce3e8 = 3;
    g_mn6ce978 = 0.5f;
    g_mn6ce964 = 4;
    g_mn6ce960 = 0;
    g_mn6ce968 = 0;
    g_mn6ce96c = 3;

    BYTE* menu = (BYTE*)operator new(296);
    if (menu)
    {
        Mn_ControlCtor(menu, 0, &g_mn67663c, 0);
        F(int, menu, 288) = 0;
        F(int, menu, 292) = 0;
        F(void*, menu, 0) = &g_mn64dc50;
    }
    Mn_AddChild(menu, 0, Mn_Button(&g_mn678308, 108, 596, 452), 0);
    Mn_AddChild(menu, 0, Mn_Button(&g_mn678300, 105, 648, 332), 0);
    Mn_AddChild(menu, 0, Mn_Button(&g_mn6782f8, 104, 648, 202), 0);
    BYTE* first = Mn_Button(&g_mn6782f0, 102, 596, 82);
    Mn_AddChild(menu, 0, first, 0);
    Mn_AddChild(menu, 0, Mn_Button(&g_mn6782e8, 101, 79, 452), 0);
    Mn_AddChild(menu, 0, Mn_Button(&g_mn6782e0, 106, 27, 332), 0);
    Mn_AddChild(menu, 0, Mn_Button(&g_mn6782d8, 103, 27, 202), 0);
    Mn_AddChild(menu, 0, Mn_Button(&g_mn6782d0, 107, 79, 82), 0);
    Mn_Select(menu, 0, first);
    Mn_Nop(menu + 44);
    Mn_AddChild(F(BYTE*, s, 56), 0, menu, 0);
    g_mn6cec80 = s;
    Mn_ClearFlags(s, 0);

    Mn_AddScreen(g_mn6cec6c, Mn_Screen(1360, Mn_Controls, app));
    Mn_AddScreen(g_mn6cec70, Mn_Screen(424, Mn_Display, app));
    Mn_AddScreen(g_mn6cec7c, Mn_Screen(376, Mn_Keys, app));
    Mn_AddScreen(g_mn6cec8c, Mn_Screen(368, Mn_Arrows, app));
    Mn_AddScreen(g_mn6cec90, Mn_Screen(448, Mn_Join, app));
    Mn_AddScreen(g_mn6cec94, Mn_Screen(816, Mn_Providers, app));
    Mn_AddScreen(g_mn6cec98, Mn_Screen(400, Mn_DisplayOpt, app));
    Mn_AddScreen(g_mn6cecd8, Mn_Screen(704, Mn_Session, app));
    Mn_AddScreen(g_mn6cec9c, Mn_Screen(408, Mn_VideoOpt, app));

    GetPrivateProfileStringA(&g_mn6733c4, &g_mn6733cc, &g_mn6733d4, buf, 30, g_mnIni696868);
    if (strcmp(buf, &g_mn6733d4) == 0)
        F(int, g_mn6cec6c, 1348) = 0;
    else if (strcmp(buf, &g_mn6733bc) == 0)
        F(int, g_mn6cec6c, 1348) = 1;
    else
        F(int, g_mn6cec6c, 1348) = 2;
    g_mn696874 |= 64;
    BYTE* a = Mn_Find(&g_mn6782c0);
    g_mn6f2ee8 = a;
    g_mn6f2ed0 = a;
    a = Mn_Find(&g_mn6782b0);
    g_mn6f2eec = a;
    g_mn6f2ed4 = a;
    a = Mn_Find(&g_mn6782a0);
    g_mn6f2ef0 = a;
    g_mn6f2ed8 = a;
    Mn_Activate(s, 0);
    CoInitialize(0);
    return s;
}

} // extern "C"

// === a media event for a vehicle resource (FUN_4d8350 with its pieces up to $L_4d88b9)

extern "C" BYTE* __fastcall Vh_CarCtor(BYTE* s, void*, BYTE* a, int b); KING_ALIAS("@Vh_CarCtor@16", "@Car53e420_Construct@16")
extern "C" void __fastcall Vh_OwnerAttach(BYTE* self, void*, BYTE* node); KING_ALIAS("@Vh_OwnerAttach@12", "@Owner_Attach@12")
extern "C" void __fastcall Vh_Restart(BYTE* self, void*); KING_ALIAS("@Vh_Restart@8", "@Obj53e490_Restart@8")
extern "C" BYTE* __fastcall Vh_ResFind(BYTE* self, void*, const char* name); KING_ALIAS("@Vh_ResFind@12", "@Resources_Find@12")
extern "C" BYTE* __fastcall Vh_ResGet(BYTE* self, void*, int id); KING_ALIAS("@Vh_ResGet@12", "@Resources_Get@12")
extern "C" void __fastcall Vh_ResLoad(BYTE* self, void*, BYTE* ctx); KING_ALIAS("@Vh_ResLoad@12", "@Resource_Load@12")
extern "C" int __fastcall Vh_ResAddRecord(BYTE* self, void*, int a, BYTE* rec, int c); KING_ALIAS("@Vh_ResAddRecord@20", "@Resources_AddRecord@20")
extern "C" int __fastcall Vh_ResDelete(BYTE* self, void*, int id); KING_ALIAS("@Vh_ResDelete@12", "@Resources_Delete@12")
extern "C" int __fastcall Vh_ResCreate(BYTE* self, void*, int id, const char* name); KING_ALIAS("@Vh_ResCreate@16", "@Resources_Create@16")
extern "C" int __fastcall Vh_ResSetData(BYTE* s, void*, int id, const char* name, unsigned size, const void* data); KING_ALIAS("@Vh_ResSetData@24", "@Resources_SetData@24")
extern "C" void __fastcall Vh_Charge(BYTE* self, void*, BYTE* tab); KING_ALIAS("@Vh_Charge@12", "@ResTable_Charge@12")
extern "C" void __fastcall Vh_RefRelease(BYTE* self, void*); KING_ALIAS("@Vh_RefRelease@8", "@ResRef_Release@8")
extern "C" BYTE* __fastcall Vh_LoadRoom(BYTE* self, void*, const char* name, const char* module); KING_ALIAS("@Vh_LoadRoom@16", "@Fw_LoadRoom@16")
extern "C" void __fastcall Vh_Players(void* out, void*); KING_ALIAS("@Vh_Players@8", "@Iter_Players@8")
extern "C" BYTE* __fastcall Vh_ObjectInit(BYTE* s, void*, int a, int b, const char* name, const void* m, int c,
                                          const void* data, int d, int e, int f); KING_ALIAS("@Vh_ObjectInit@44", "@Fx_ObjectInit@44")
extern "C" void* __fastcall Vh_Self(void* self, void*); KING_ALIAS("@Vh_Self@8", "@Obj541960_Self@8")
extern "C" void __cdecl Vh_Release(int id); KING_ALIAS("_Vh_Release", "_Vehicle_Release")
extern "C" void __cdecl Vh_LoadResource(int id); KING_ALIAS("_Vh_LoadResource", "_Vehicle_LoadResource")
extern "C" void __fastcall Vh_AttachViewer(BYTE* s, void*, BYTE* v); KING_ALIAS("@Vh_AttachViewer@12", "@Viewer_AttachVehicle@12")
extern "C" void __fastcall Vh_WorldAttach(BYTE* self, int, BYTE* src); KING_ALIAS("@Vh_WorldAttach@12", "@WorldObj_Attach@12")
extern "C" int __cdecl Vh_CarType(int n); KING_ALIAS("_Vh_CarType", "_CarType_FromNumber")
extern "C" void __cdecl Vh_Fatal(const char* fmt, ...); KING_ALIAS("_Vh_Fatal", "_Sys_Fatal")
extern "C" int __cdecl Vh_NetCreate(BYTE* res, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD id, SuBlob48 state, DWORD a20); KING_ALIAS("_Vh_NetCreate", "_Net_CreateById")
KING_VAR(char, g_vhTrafficLight, 672bd0)     // "Traffic_Light"
KING_VAR(char, g_vhNoTruck, 672bb4)          // "Not found cluster for truck"
KING_VAR(char, g_vhNoTrailer, 672b90)        // "Not found cluster for semitrailer"
KING_VAR(BYTE*, g_vhWorld6d2078, 6d2078)
KING_VAR(BYTE*, g_vhApp696ca0, 696ca0)
KING_VAR(BYTE*, g_vh6cec6c, 6cec6c)
KING_VAR(int, g_vhPlayerId672984, 672984)
KING_VAR(BYTE*, g_vh675d38, 675d38)
KING_VAR(int, g_vhModels6ce32c, 6ce32c)
KING_VAR(char, g_vhModelNames69818e, 69818e)
KING_VAR(BYTE, g_vhClusters6d24bc, 6d24bc)
KING_VAR(BYTE*, g_vhCarTypes6ced98, 6ced98)
KING_VAR(BYTE*, g_vh6ef1f8, 6ef1f8)
KING_VAR(BYTE*, g_vh696340, 696340)
KING_VAR(BYTE, g_vh6f68b0, 6f68b0)

// the vehicle model named name (2416-byte records), or the model count when none matches
static int Vh_Model(const char* name)
{
    int i = 0;
    for (; i < g_vhModels6ce32c; i++)
        if (strcmp(name, &g_vhModelNames69818e + i * 2416) == 0)
            break;
    return i;
}

extern "C" {

// [FUN_4d8350..$L_4d88b9] media event for app s: a traffic light (one per world, +1632), or resource a6 (named a1)
// recreated in s +704: a truck (id/1000 = 100: the player's own when a6 is 672984 in multiplayer, its room loaded),
// a semitrailer (101), or a networked car (id/100000 = 2 or 3); each gets its model's cluster and is charged
// into the table. Returns the traffic light, else 0
BYTE* __cdecl Media_VehicleEvent(BYTE* s, STA94_PARAMS)
{
    const char* name = (const char*)a1;
    if (strcmp(name, &g_vhTrafficLight) == 0)
    {
        if (!g_vhWorld6d2078 || F(BYTE*, g_vhWorld6d2078, 1632))
            return 0;
        BYTE* p = (BYTE*)operator new(88);
        if (p)
            p = Vh_CarCtor(p, 0, s, a19);
        F(BYTE*, g_vhWorld6d2078, 1632) = p;
        Vh_OwnerAttach(s + 2992, 0, F(BYTE*, g_vhWorld6d2078, 1632));
        if (s == g_vhApp696ca0)
            F(int, F(BYTE*, g_vhWorld6d2078, 1632), 80) = 1;
        Vh_Restart(F(BYTE*, g_vhWorld6d2078, 1632), 0);
        return F(BYTE*, g_vhWorld6d2078, 1632);
    }
    BYTE* res = s + 704;
    Vh_ResFind(res, 0, name);
    int id = (int)a6;
    BYTE* old = Vh_ResGet(res, 0, id);
    if (old)
    {
        if (F(void*, old, 28))
            Vh_ResLoad(old, 0, res);
        Vh_ResAddRecord(res, 0, 0, old, 0);
        Vh_ResDelete(res, 0, id);
    }
    Vh_ResCreate(res, 0, id, name);
    BYTE* r = Vh_ResGet(res, 0, id);
    if (!r)
        return 0;
    F(BYTE, r, 8) |= 1;
    if (F(BYTE, F(BYTE*, r, 12), 4012) & 2)
    {
        BYTE* ref = F(BYTE*, g_vhApp696ca0, 484);
        if (F(int, ref, 20))
        {
            if (!F(int, ref, 16))
                do
                    Sleep(10);
                while (!F(int, F(BYTE*, g_vhApp696ca0, 484), 16));
            Vh_RefRelease(F(BYTE*, g_vhApp696ca0, 484), 0);
        }
    }
    SuBlob48 st = { { a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18 } };
    const SuBlob48* state = &st;
    int kind = F(int, r, 4) / 1000;
    if (kind == 100)
    {
        const char* module = (const char*)a2;
        int own = 0;
        if (F(int, g_vh6cec6c, 1300) > 1)
        {
            if (id == g_vhPlayerId672984)
            {
                F(BYTE, r, 8) &= ~1;
                own = 1;
                if (F(int, F(BYTE*, r, 12), 4012) == 2)
                {
                    memcpy(s + 652, state, 48);
                    strcpy(F(char*, s, 488) + 44, module);
                    strcpy(F(char*, s, 488) + 12, name);
                    Vh_LoadRoom(g_vhApp696ca0, 0, name, module);
                    g_vh675d38 = F(BYTE*, g_vhApp696ca0, 2992);
                }
            }
        }
        else
            own = 2;
        int model = Vh_Model((const char*)a3);
        BYTE obj[300];
        DWORD players;
        Vh_Players(&players, 0);
        Vh_ObjectInit(obj, 0, model, 0, module, state, (int)players, 0, 0, (int)a19, own);
        Vh_ResSetData(res, 0, id, name, 300, obj);
        BYTE* cl = *(BYTE**)(&g_vhClusters6d24bc + model * 1488 + 12);
        if (cl)
            F(int, r, 16) = F(int, cl, 16);
        else
            Vh_Fatal(&g_vhNoTruck, 0);
        Vh_Charge(r, 0, res);
        if (F(int, g_vh6cec6c, 1300) > 1 && own == 1)
        {
            if (!F(void*, r, 28))
            {
                BYTE* o = g_vh6ef1f8 ? g_vh6ef1f8 - 4 : 0;
                BYTE* self = (BYTE*)Vh_Self(o, 0);
                if (self)
                    Vh_Release(F(int, self, 360));
                Vh_LoadResource(F(int, r, 4));
            }
            BYTE* v = F(BYTE*, r, 28);
            if (v)
            {
                g_vh696340 = v;
                if (g_vhWorld6d2078)
                {
                    Vh_AttachViewer(g_vhWorld6d2078, 0, v);
                    Vh_WorldAttach(&g_vh6f68b0, 0, v);
                }
            }
        }
        return 0;
    }
    if (kind == 101)
    {
        int model = Vh_Model((const char*)a3);
        BYTE obj[300];
        DWORD players;
        Vh_Players(&players, 0);
        Vh_ObjectInit(obj, 0, model, 0, (const char*)a2, state, (int)players, 0, 0, (int)a19, 0);
        Vh_ResSetData(res, 0, id, name, 300, obj);
        BYTE* cl = *(BYTE**)(&g_vhClusters6d24bc + model * 1488 + 12);
        if (cl)
            F(int, r, 16) = F(int, cl, 16);
        else
            Vh_Fatal(&g_vhNoTrailer, 0);
        Vh_Charge(r, 0, res);
        return 0;
    }
    int net = F(int, r, 4) / 100000;
    if (net == 2)
    {
        Vh_NetCreate(s, a1, a2, a3, a4, a5, a6, *state, a19);
        int t = Vh_CarType(F(int, r, 4) / 1000 % 100);
        BYTE* ct = (&g_vhCarTypes6ced98)[t * 11];
        if (ct)
            F(int, r, 16) = F(int, ct, 16);
        if (F(int, g_vh6cec6c, 1300) > 1)
            F(int, r, 0) = 2;
        Vh_Charge(r, 0, res);
    }
    else if (net == 3)
    {
        Vh_NetCreate(s, a1, a2, a3, a4, a5, a6, *state, a19);
        if (F(int, g_vh6cec6c, 1300) > 1)
            F(int, r, 0) = 2;
        Vh_Charge(r, 0, res);
    }
    return 0;
}

} // extern "C"

// === the game window's first draw: the info screens and the intro movie ($L_528820 with its pieces up to $L_528bd6)

ASM_PROC($L_5dd8c0)    // window: set the video mode (w, h, cres, flags)
ASM_PROC($L_5d7c20)    // tries a video mode (w, h, cres, flags, 3 outs)
extern "C" BYTE* __fastcall Is_InfoCtor(BYTE* s, void*, BYTE* parent, int mode); KING_ALIAS("@Is_InfoCtor@16", "@InfoScreen_Construct@16")
extern "C" BYTE* __fastcall Is_ShellCtor(BYTE* s, void*, BYTE* parent); KING_ALIAS("@Is_ShellCtor@12", "@Shell528c20_Construct@12")
extern "C" BYTE* __fastcall Is_WindowsAdd2(BYTE* self, void*, BYTE* w); KING_ALIAS("@Is_WindowsAdd2@12", "@Windows_Add2@12")
extern "C" BYTE* __fastcall Is_WindowsAdd(BYTE* self, void*, BYTE* w); KING_ALIAS("@Is_WindowsAdd@12", "@Windows_Add@12")
extern "C" int __fastcall Is_SetSurface(BYTE* self, void*, BYTE* s); KING_ALIAS("@Is_SetSurface@12", "@Window_SetSurface@12")
extern "C" int __cdecl Is_MoviePlay(const char* name, int mode); KING_ALIAS("_Is_MoviePlay", "_Movie_Play")
extern "C" DWORD __cdecl Is_ScreenFlags(); KING_ALIAS("_Is_ScreenFlags", "_Carat_ScreenFlags")
extern "C" void __fastcall Is_PaletteApply(BYTE* self, void*); KING_ALIAS("@Is_PaletteApply@8", "@Palette_Apply@8")
extern "C" void __fastcall Is_Refresh(BYTE* self, void*); KING_ALIAS("@Is_Refresh@8", "@Window_Refresh@8")
extern "C" void __cdecl Is_CheckSave(); KING_ALIAS("_Is_CheckSave", "_Shell_CheckSave")
extern "C" void __cdecl Is_BitmapDraw(BYTE* dst, int x, int y, BYTE* bmp); KING_ALIAS("_Is_BitmapDraw", "_Bitmap_Draw")
extern "C" void __fastcall Is_Notify(BYTE* s, void*); KING_ALIAS("@Is_Notify@8", "@Wnd5d8b90_Notify@8")
KING_VAR(int, g_isFirst675d44, 675d44)
KING_VAR(int, g_isIntro675d40, 675d40)
KING_VAR(BYTE*, g_isParent69684c, 69684c)
KING_VAR(BYTE, g_isWindows696750, 696750)
KING_VAR(BYTE*, g_isApp696ca0, 696ca0)
KING_VAR(BYTE*, g_isInfo6cecc0, 6cecc0)
KING_VAR(BYTE*, g_isInfo6cecc4, 6cecc4)
KING_VAR(BYTE*, g_isInfo6cecc8, 6cecc8)
KING_VAR(BYTE*, g_isInfo6ceccc, 6ceccc)
KING_VAR(BYTE*, g_isInfo6cecd0, 6cecd0)
KING_VAR(BYTE*, g_isInfo6cecd4, 6cecd4)
KING_VAR(BYTE*, g_isShell6cec88, 6cec88)
KING_VAR(int, g_is696878, 696878)
KING_VAR(DWORD, g_isFlags69685c, 69685c)
KING_VAR(char, g_isKotrAvi, 678368)       // "\\Kotr.avi"

static void Is_Info(BYTE*& g, int mode)
{
    BYTE* p = (BYTE*)operator new(480);
    if (p)
        p = Is_InfoCtor(p, 0, g_isParent69684c, mode);
    g = p;
    Is_WindowsAdd2(&g_isWindows696750, 0, p);
    CallVirt(g, 32);
}

extern "C" {

// [$L_528820..$L_528bd6] (game window vtable) window s drawn: the first time (675d44) the six info screens are
// built and, with the intro on (675d40), the shell screen added and Kotr.avi played at 640x480 (a key leaves to
// s's vtable +32), then the 800x600 mode restored; else the window's background (+368, unless +372) is drawn
void __fastcall GameWindow_Draw(BYTE* s, void*)
{
    if (!g_isFirst675d44)
    {
        if (!F(int, s, 372))
            Is_BitmapDraw(F(BYTE*, s, 12), 0, 0, F(BYTE*, s, 368));
        Is_Notify(s, 0);
        return;
    }
    Is_Info(g_isInfo6cecc0, 0);
    Is_Info(g_isInfo6cecc4, 1);
    Is_Info(g_isInfo6cecc8, 2);
    Is_Info(g_isInfo6ceccc, 3);
    Is_Info(g_isInfo6cecd0, 4);
    Is_Info(g_isInfo6cecd4, 5);
    Is_SetSurface(F(BYTE*, g_isParent69684c, 12), 0, *F(BYTE**, F(BYTE*, F(BYTE*, g_isApp696ca0, 9396), 100), 0));
    if (g_isIntro675d40)
    {
        BYTE* p = (BYTE*)operator new(2312);
        if (p)
            p = Is_ShellCtor(p, 0, g_isParent69684c);
        g_isShell6cec88 = p;
        Is_WindowsAdd(&g_isWindows696750, 0, p);
        g_is696878 = -1;
        CallThis($L_5dd8c0, &g_isWindows696750, 640, 480, 2, 15872);
        if (Is_MoviePlay(&g_isKotrAvi, 0))
        {
            CallVirt(s, 32);
            g_isFirst675d44 = 0;
            return;
        }
        int o0, o1, o2;
        if (!CallC<int>($L_5d7c20, 800, 600, 1, 3584, &o2, &o1, &o0))
            CallThis($L_5dd8c0, &g_isWindows696750, 800, 600, 1, ~(Is_ScreenFlags() | 0x4000) & g_isFlags69685c);
        CallThis($L_5dd8c0, &g_isWindows696750, 800, 600, 1, 3584);
        Is_PaletteApply(*F(BYTE**, F(BYTE*, F(BYTE*, g_isApp696ca0, 9396), 100), 0), 0);
        Is_Refresh(g_isParent69684c, 0);
        g_is696878 = 0;
    }
    else
    {
        CallVirt(g_isShell6cec88, 32);
        CallThis($L_5dd8c0, &g_isWindows696750, 800, 600, 1, 3584);
    }
    Is_CheckSave();
    g_isFirst675d44 = 0;
}

} // extern "C"

// === the movie screen's frame ($L_528ca0 with its pieces up to $L_528de2)

extern "C" void __cdecl Mv_FillRect(int x, int y, int w, int h, int c); KING_ALIAS("_Mv_FillRect", "_Dd_FillRect")
extern "C" int __fastcall Mv_AviUpdate(BYTE* s, void*, int a, int x, int y, int w, int h); KING_ALIAS("@Mv_AviUpdate@28", "@Avi_Update@28")
extern "C" void __fastcall Mv_Set2304(BYTE* s, void*, int v); KING_ALIAS("@Mv_Set2304@12", "@Screen_Set2304@12")
extern "C" void __fastcall Mv_AviStart(BYTE* s, void*); KING_ALIAS("@Mv_AviStart@8", "@Avi_Start@8")
extern "C" void __fastcall Mv_Close(BYTE* s, void*); KING_ALIAS("@Mv_Close@8", "@Media_Close@8")
extern "C" void __fastcall Mv_Show(BYTE* s, void*, int v); KING_ALIAS("@Mv_Show@12", "@Window_Show@12")
extern "C" void __fastcall Mv_SetMode(BYTE* s, void*, int m); KING_ALIAS("@Mv_SetMode@12", "@Shell_SetMode@12")
extern "C" void __cdecl Mv_MusicStart(); KING_ALIAS("_Mv_MusicStart", "_Music_Start")
extern "C" void __fastcall Mv_WordsReset(BYTE* s, void*); KING_ALIAS("@Mv_WordsReset@8", "@Words295_Reset@8")
extern "C" void __fastcall Mv_Wasm(BYTE* s, void*); KING_ALIAS("@Mv_Wasm@8", "@Wasm_598d60@8")
extern "C" void __cdecl Mv_EnterWorld(int a); KING_ALIAS("_Mv_EnterWorld", "_Shell_EnterWorld")
extern "C" BYTE* __cdecl Mv_ClearMessages(); KING_ALIAS("_Mv_ClearMessages", "_Messages_Clear1276")
extern "C" int __cdecl Mv_ForceFeedback(int on); KING_ALIAS("_Mv_ForceFeedback", "_Joy_ForceFeedback")
KING_VAR(BYTE*, g_mv6cec6c, 6cec6c)
KING_VAR(int, g_mv7208fc, 7208fc)
KING_VAR(int, g_mv696878, 696878)
KING_VAR(BYTE*, g_mv6cec80, 6cec80)
KING_VAR(BYTE*, g_mvShell6ceca8, 6ceca8)
KING_VAR(BYTE*, g_mv6cec78, 6cec78)
KING_VAR(BYTE, g_mvWords6d1948, 6d1948)
KING_VAR(BYTE*, g_mv696cb0, 696cb0)
KING_VAR(BYTE, g_mv6f68b0, 6f68b0)
KING_VAR(BYTE*, g_mvWorld6d2078, 6d2078)

extern "C" {

// [$L_528ca0..$L_528de2] (movie screen vtable) movie screen s's frame: while playing (+2304 < 7) the AVI drawn
// centred at most 800x600 (the margins cleared), its end stepping +2304; at 7 the movie closed, the 800x600 mode
// back (unless +2308 >= 2), and the screen left for what +2308 says: the game window, a shell mode (0, 2, 5), the
// race start (5: music, words, messages, app vtable +36, the truck's lamp) or the world (6)
void __fastcall MovieScreen_Frame(BYTE* s, void*)
{
    int st = F(int, s, 2304);
    if (st < 7)
    {
        int x = 0, y = 0;
        int w = CallVirt<int>(s, 20);
        int h = CallVirt<int>(s, 24);
        if (w > 800)
        {
            x = (w - 800) / 2;
            w = 800;
            y = (h - 600) / 2;
            int fh = CallVirt<int>(s, 24);
            int fw = CallVirt<int>(s, 20);
            Mv_FillRect(0, 0, fw, fh, 0);
            h = 600;
        }
        if (Mv_AviUpdate(s + 56, 0, g_mv7208fc, x, y, w, h))
            Mv_Set2304(s, 0, 0);
        return;
    }
    if (st != 7)
        return;
    Mv_AviStart(s + 56, 0);
    Mv_Close(s + 56, 0);
    if (F(int, s, 2308) < 2)
    {
        int o0, o1, o2;
        if (!CallC<int>($L_5d7c20, 800, 600, 1, 3584, &o2, &o1, &o0))
            CallThis($L_5dd8c0, &g_isWindows696750, 800, 600, 1, ~(Is_ScreenFlags() | 0x4000) & g_isFlags69685c);
        CallThis($L_5dd8c0, &g_isWindows696750, 800, 600, 1, 3584);
        Is_PaletteApply(*F(BYTE**, F(BYTE*, F(BYTE*, g_isApp696ca0, 9396), 100), 0), 0);
    }
    else
        g_mv696878 = -1;
    Mv_Show(g_isParent69684c, 0, 0);
    Is_Refresh(g_isParent69684c, 0);
    if (F(int, s, 2308) == 0)
        Is_CheckSave();
    int m = F(int, s, 2308);
    if (m < 2)
        CallVirt(g_mv6cec80, 36);
    else if (m == 2)
        Mv_SetMode(g_mvShell6ceca8, 0, 0);
    else if (m == 3)
        Mv_SetMode(g_mvShell6ceca8, 0, 2);
    else if (m == 4)
        Mv_SetMode(g_mvShell6ceca8, 0, 5);
    else if (m == 5)
    {
        if (F(int, g_mv6cec6c, 1344) && F(short, g_mv6cec78, 384) > 5)
            Mv_MusicStart();
        Mv_WordsReset(&g_mvWords6d1948, 0);
        Mv_ClearMessages();
        CallVirt(g_isApp696ca0, 36);
        F(DWORD, g_mv696cb0, 16) |= 3;
        Mv_Wasm(&g_mv6f68b0, 0);
        F(BYTE, F(BYTE*, F(BYTE*, g_mvWorld6d2078, 616), 21600), 9984) = 2;
        Mv_ForceFeedback(1);
    }
    else if (m == 6)
        Mv_EnterWorld(0);
    CallVirt(s, 32);
}

} // extern "C"

// [FUN_53fd30] calls virtual slot 17 of the object at 696cb0
KING_VAR(BYTE*, su_696cb0, 696cb0)
extern "C" void __cdecl Startup_Call53fd30() { CallVirt(su_696cb0, 68); }
