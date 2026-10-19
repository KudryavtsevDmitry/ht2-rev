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

// ---------------------------------------------------------------------------
extern "C" void FUN_4e1e10(); KING_ALIAS("_FUN_4e1e10", "@Ini4e1e10_Construct@12")
ASM_VAR(AsmLabel, g_ini696750, $L_696750)
ASM_VAR(AsmLabel, s_6733b0, $L_6733b0)

// [FUN_4e1de0] constructs the ini object 696750 on 6733b0
extern "C" void* __cdecl Startup_Ini696750()
{
    return CallThis<void*>(FUN_4e1e10, g_ini696750, (char*)s_6733b0);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
ASM_PROC(FUN_5e52b0)    // resources: send a message (id, kind, size, data)
ASM_VAR(int, g_suTypeClass5, $L_6ce344)
ASM_VAR(int, g_suCarTypeCount, $L_6ce32c)
ASM_VAR(BYTE*, g_su6ef204, $L_6ef204)
ASM_VAR(AsmLabel, s_su672f50, $L_672f50)
ASM_VAR(AsmLabel, s_su672f78, $L_672f78)
ASM_VAR(Vec3, g_suZero702650, $L_702650)
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
ASM_PROC(FUN_508000)
ASM_PROC(FUN_508300)
ASM_PROC(FUN_50a740)
ASM_VAR(AsmLabel, s_su671518, $L_671518)
ASM_VAR(AsmLabel, s_su672ec8, $L_672ec8)      // the ini file
ASM_VAR(AsmLabel, s_su6730ac, $L_6730ac)
ASM_VAR(AsmLabel, s_su6729a8, $L_6729a8)
ASM_VAR(AsmLabel, s_su6730a4, $L_6730a4)
ASM_VAR(AsmLabel, s_su6730a0, $L_6730a0)
ASM_VAR(AsmLabel, s_su672994, $L_672994)
ASM_VAR(AsmLabel, s_su673094, $L_673094)
ASM_VAR(AsmLabel, s_su673084, $L_673084)
ASM_VAR(AsmLabel, s_su673070, $L_673070)
ASM_VAR(AsmLabel, s_su673064, $L_673064)
ASM_VAR(int, g_su696c74, $L_696c74)
ASM_VAR(int, g_su696c6c, $L_696c6c)
ASM_VAR(int, g_su696c70, $L_696c70)
ASM_VAR(int, g_su672d80, $L_672d80)
ASM_VAR(int, g_su696c58, $L_696c58)
ASM_VAR(void*, g_su6ce36c, $L_6ce36c)
ASM_VAR(void*, g_su697858, $L_697858)
ASM_VAR(int, g_su697860, $L_697860)
ASM_VAR(AsmLabel, g_su696bc8, $L_696bc8)

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
ASM_VAR(AsmLabel, s_su672eb8, $L_672eb8)
ASM_VAR(AsmLabel, s_su673050, $L_673050)
ASM_VAR(AsmLabel, s_su67305c, $L_67305c)
ASM_VAR(AsmLabel, s_su673024, $L_673024)
ASM_VAR(AsmLabel, s_su67301c, $L_67301c)
ASM_VAR(AsmLabel, s_su673014, $L_673014)
ASM_VAR(AsmLabel, s_su673008, $L_673008)
ASM_VAR(AsmLabel, s_su672ffc, $L_672ffc)
ASM_VAR(AsmLabel, s_su672ff0, $L_672ff0)
ASM_VAR(AsmLabel, s_su672fe4, $L_672fe4)
ASM_VAR(AsmLabel, s_su672fd8, $L_672fd8)
ASM_VAR(AsmLabel, s_su672fcc, $L_672fcc)
ASM_VAR(AsmLabel, s_su672fc0, $L_672fc0)
ASM_VAR(AsmLabel, s_su672fb8, $L_672fb8)
ASM_VAR(AsmLabel, s_su672fb0, $L_672fb0)
ASM_VAR(AsmLabel, s_su672fa8, $L_672fa8)
ASM_VAR(AsmLabel, s_su672fa0, $L_672fa0)
ASM_VAR(AsmLabel, s_su672f94, $L_672f94)
ASM_VAR(AsmLabel, s_su672f88, $L_672f88)
ASM_VAR(AsmLabel, s_su672f7c, $L_672f7c)
ASM_VAR(int, g_su696c40, $L_696c40)
ASM_VAR(float, g_su696bdc, $L_696bdc)
ASM_VAR(float, g_su696be0, $L_696be0)
ASM_VAR(short, g_su696bda, $L_696bda)
ASM_VAR(int, g_su696be4, $L_696be4)
ASM_VAR(int, g_su696be8, $L_696be8)
ASM_VAR(int, g_su696bec, $L_696bec)
ASM_VAR(AsmLabel, g_su696bf0, $L_696bf0)
ASM_VAR(AsmLabel, g_su696c04, $L_696c04)
ASM_VAR(AsmLabel, g_su696c18, $L_696c18)
ASM_VAR(AsmLabel, g_su696c2c, $L_696c2c)
ASM_VAR(float, g_su696c44, $L_696c44)
ASM_VAR(int, g_su696c54, $L_696c54)
ASM_VAR(int, g_su696c48, $L_696c48)
ASM_VAR(int, g_su696c4c, $L_696c4c)
ASM_VAR(int, g_su696c50, $L_696c50)
ASM_VAR(int, g_su696c5c, $L_696c5c)
ASM_VAR(int, g_su696c60, $L_696c60)
ASM_VAR(int, g_su696c64, $L_696c64)

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

ASM_VAR(const char*, g_suIni, $L_696868)
ASM_VAR(BYTE*, g_su696ca0, $L_696ca0)
ASM_VAR(AsmLabel, s_su6733c4, $L_6733c4)
ASM_VAR(AsmLabel, s_su6733cc, $L_6733cc)
ASM_VAR(AsmLabel, s_su6733d4, $L_6733d4)
ASM_VAR(AsmLabel, s_su6733bc, $L_6733bc)
ASM_VAR(AsmLabel, s_su6733b0, $L_6733b0)
ASM_VAR(AsmLabel, s_su6733ac, $L_6733ac)
ASM_VAR(AsmLabel, s_su6733a4, $L_6733a4)
ASM_VAR(AsmLabel, s_su67339c, $L_67339c)
ASM_VAR(AsmLabel, s_su673398, $L_673398)
ASM_VAR(AsmLabel, s_su67338c, $L_67338c)
ASM_VAR(AsmLabel, s_su673384, $L_673384)
ASM_VAR(AsmLabel, s_su67337c, $L_67337c)
ASM_VAR(AsmLabel, s_su673374, $L_673374)
ASM_VAR(AsmLabel, s_su67336c, $L_67336c)
ASM_VAR(AsmLabel, s_su673364, $L_673364)
ASM_VAR(AsmLabel, s_su673358, $L_673358)

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
