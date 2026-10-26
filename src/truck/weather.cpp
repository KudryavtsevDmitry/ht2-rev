// weather.cpp - the unit at 0x5a3e00 of king.masm: weather (weather.ini,
// puddles, ambient light) and scene nodes with a frame (+48) or a
// position (+52).
//
// Building with  nmake ASM_WEATHER=1  makes king.masm assemble the original
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

KING_BSS(int, g_6cec68, 0x6cec68)
KING_BSS(int, g_6968ac, 0x6968ac)
KING_BSS(BYTE*, g_options, 0x6cec78)            // +386, +388, +390: volumes in percent
typedef BYTE* Ptrs[1];
typedef int Ints[1];
KING_BSS(Ptrs, g_6ced98, 0x6ced98)              // 11 per row
KING_BSS(Ints, g_6cedac, 0x6cedac)
KING_BSS(int, g_class1, 0x6ce350)               // the first type of each class
KING_BSS(int, g_class2, 0x6ce354)
KING_BSS(int, g_class3, 0x6ce358)
KING_BSS(int, g_class4, 0x6ce35c)
KING_BSS(int, g_class5, 0x6ce360)
KING_BSS(int, g_class6, 0x6ce364)
KING_BSS(int, g_classEnd, 0x6ce368)
KING_BSS(BYTE*, g_vehicleList2, 0x6ef1f8)
KING_BSS(AsmLabel, g_6d219c, 0x6d219c)          // 1488-byte records by vehicle type
KING_BSS(int, g_7025b0, 0x7025b0)
KING_BSS(double, g_6fb3e8, 0x6fb3e8)
KING_BSS(int, g_6fb3d8, 0x6fb3d8)
KING_BSS(int, g_6fb3d0, 0x6fb3d0)
KING_BSS(int, g_7025c8, 0x7025c8)
ASM_VAR(DWORD, g_67e8c8, $L_67e8c8)
KING_BSS(int, g_7025cc, 0x7025cc)
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
// The unit at 0x5a3e00: nodes with a frame (+48) or a position (+52)

extern "C" void __fastcall Node_Matrix(BYTE* self, void*, Matrix* out)
{
    if (F(void*, self, 48)) {
        CallThis(FUN_5f25c0, F(void*, self, 48), out);
        return;
    }
    Matrix m;
    *out = *Matrix_Translation(&m, *F(Vec3*, self, 52));
}

extern "C" void __fastcall Node_SetMatrix(BYTE* self, void*, const Matrix* m)
{
    if (F(void*, self, 48)) {
        CallThis(FUN_5f2190, F(void*, self, 48), m, 0);
        return;
    }
    *F(Vec3Bits*, self, 52) = F(Vec3Bits, m, 36);
    BYTE* q = F(BYTE*, self, 32) + 20;
    int w = F(int, q, 12);
    *(Vec3Bits*)q = *F(Vec3Bits*, self, 52);
    F(int, q, 12) = w;
}

extern "C" void __cdecl Set7025b0(int v)
{
    g_7025b0 = v;
}

// marks pixel (x, y) of the surface's current buffer
extern "C" void __cdecl Surface_Mark(const WORD* s, int x, int y, const BYTE* flags)
{
    if (!flags || !(flags[28] & 1))
        return;
    BYTE* buf = *(BYTE**)((BYTE*)s + 24 + F(int, s, 16) * 4);
    if (buf && y < s[1] && x < s[0] && y >= 0 && x >= 0)
        *(WORD*)(buf + s[2] * x + s[3] * y) = 0x8888;
}

extern "C" void __fastcall Obj5a58c0_Start(BYTE* self, void*)
{
    if (F(DWORD, self, 164) & 0x60)
        return;
    F(int, self, 160) = 100;
    F(DWORD, self, 164) |= 0x20;
}

extern "C" void __fastcall Obj5a6860_Set(BYTE* self, void*, int a, int b, int c, int d, int flags)
{
    if (!(flags & 16)) {
        F(int, self, 40) = a;
        F(int, self, 44) = b;
    }
    if (!(flags & 32)) {
        F(int, self, 48) = c;
        F(int, self, 52) = d;
    }
}

extern "C" void __fastcall Objects_Release(BYTE* self, void*)
{
    for (int i = 0; i < F(int, self, 24); i++) {
        void* o = F(void**, self, 32)[i];
        if (o)
            CallVirt(o, 0, 1);
        void* p = F(void**, self, 36)[i];
        if (p) {
            CallVirt(p, 36);
            p = F(void**, self, 36)[i];
            if (p)
                CallVirt(p, 0, 1);
        }
    }
    if (F(void*, self, 32))
        operator delete(F(void*, self, 32));
    if (F(void*, self, 36))
        operator delete(F(void*, self, 36));
}

// "r g b" (0..255) into a color of floats
extern "C" void __cdecl Color_Parse(float* out, char* text)
{
    out[0] = (float)(atoi(NextToken(&text)) * 0.00392156862745098);
    out[1] = (float)(atoi(NextToken(&text)) * 0.00392156862745098);
    out[2] = (float)(atoi(NextToken(&text)) * 0.00392156862745098);
}

extern "C" void __fastcall Obj5a8960_Set(BYTE* self, void*, int a, int b, double freq, int d, int e, int f, int g)
{
    F(int, self, 88) = a;
    F(int, self, 92) = b;
    F(int, self, 96) = d;
    F(int, self, 100) = e;
    F(double, self, 104) = freq * 6.2831852;
    F(int, self, 112) = f;
    F(int, self, 116) = g;
}

extern "C" void __cdecl Color_Lerp(const float* a, const float* b, float t, float* out)
{
    double x = t;
    if (x > 1.0f)
        x = 1.0f;
    else if (!(x >= 0.0f))
        x = 0.0f;
    double u = 1.0f - x;
    out[0] = (float)(x * a[0] + u * b[0]);
    out[1] = (float)(u * b[1] + x * a[1]);
    out[2] = (float)(u * b[2] + x * a[2]);
}

extern "C" void __cdecl Set6fb3e8(int a, float f)
{
    g_6fb3e8 = f;
    g_6fb3d8 = a;
    g_6fb3d0 = a;
}

// bilinear blend of the four values at p by (a, b), a folded at 0.5
extern "C" double __cdecl Blend4(float a, float b, const float* p)
{
    double x = (double)a + a;
    if (x > 1.0f)
        x = 2.0f - x;
    double nb = 1.0f - (double)b;
    double l = nb * p[2] + (double)b * p[0];
    double r = nb * p[3] + (double)b * p[1];
    return l * (1.0f - x) + r * x;
}

extern "C" double __fastcall Wave_Value(BYTE* self, void*, double phase)
{
    double w = 1.0f - ((double)F(float, self, 544) + F(float, self, 544));
    double s = x87_sin(F(double, self, 120)) * 0.3 + 1.0;
    double m = s * (1.0 - x87_fabs(w));
    double v = (m * x87_sin(F(double, self, 104) - 1.5707963 + phase) + w + 1.0) * 0.5;
    if (v > 1.0)
        return 1.0;
    if (!(v >= 0.0))
        return 0.0;
    return v;
}

extern "C" void __cdecl Fog_Set(double level, int a, int b)
{
    int i = (int)(level * 255.0);
    g_7025c8 = a;
    g_67e8c8 = ((((DWORD)i | 0xffffff00) << 8 | i) << 8) | i;
    g_7025cc = b;
}

extern "C" void __cdecl File_WriteShort(int v, FILE* f)
{
    fputc(v, f);
    fputc(v >> 8, f);
}

extern "C" int __cdecl File_ReadInt(FILE* f)
{
    int v;
    fread(&v, 4, 1, f);
    return v;
}

extern "C" void __cdecl File_WriteInt(int v, FILE* f)
{
    File_WriteShort(v, f);
    File_WriteShort(v >> 16, f);
}

extern "C" float __cdecl File_ReadFloat(FILE* f)
{
    float v;
    fread(&v, 4, 1, f);
    return v;
}

// the next word of *p (spaces and tabs separate them); *p moves past it,
// NULL at the end of the line
extern "C" char* __cdecl NextToken(char** pp)
{
    BYTE* p = (BYTE*)*pp;
    if (!p)
        return 0;
    while (*p == ' ' || *p == '\t')
        p++;
    if (*p < 32) {
        *pp = 0;
        return 0;
    }
    BYTE* start = p;
    if (*p > 32) {
        do
            p++;
        while (*p > 32);
    }
    if (*p != ' ' && *p != '\t') {
        *pp = 0;
        *p = 0;
        return (char*)start;
    }
    *p = 0;
    p++;
    if (*p < 32 && *p != '\t') {
        *pp = 0;
        return (char*)start;
    }
    *pp = (char*)p;
    return (char*)start;
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
KING_BSS(DWORD, g_702650, 0x702650)
KING_BSS(DWORD, g_702654, 0x702654)
KING_BSS(DWORD, g_702658, 0x702658)
KING_BSS(BYTE*, g_6ef1f8, 0x6ef1f8)
KING_BSS(int, g_carModelCount, 0x6ce34c)
KING_BSS(AsmLabel, g_carModelNames, 0x6c6b68)   // 876 bytes per car
typedef int CarParts[1];
KING_BSS(CarParts, g_carPartModels, 0x6ced98)   // 11 ints per car: 5 models, 5 counts, a key
KING_BSS(CarParts, g_carPartUses, 0x6cedac)

// ---------------------------------------------------------------------------
// the crowd (the ovation sound) of a scene object: its matrix is the object's
// moved by ten times the vector at 702630; fn is called when it plays

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // an empty function: the default callback
KING_BSS(float, g_eng702630, 0x702630)
KING_BSS(float, g_eng702634, 0x702634)
KING_BSS(float, g_eng702638, 0x702638)
ASM_VAR(float, g_ten64c450, $L_64c450)          // 10.0
ASM_VAR(AsmLabel, g_strOvation, $L_678540)

extern "C" void FUN_591c30(); KING_ALIAS("_FUN_591c30", "_Bodies_Overlap")
extern "C" void FUN_5928d0(); KING_ALIAS("_FUN_5928d0", "_Bodies_Pair")

KING_BSS(BYTE*, g_effectManager, 0x6f6910)

// A position below the sentinel threshold resets both scripted positions.
ASM_VAR(float, g_positionResetThreshold, $L_650624)
ASM_VAR(AsmLabel, g_setPosition1, $L_67d4cc)
ASM_VAR(AsmLabel, g_setPosition2, $L_67d4c0)
ASM_VAR(AsmLabel, g_setPlace, $L_67d4b4)
extern "C" void FUN_58e7f0(); KING_ALIAS("_FUN_58e7f0", "@MovingItem_CheckFrame@20")

ASM_VAR(short, g_sceneCommandCode, $L_67d10a)
KING_BSS(BYTE*, g_sceneCommandList, 0x6f653c)

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

// ---------------------------------------------------------------------------
extern "C" void FUN_5e1ad0(); KING_ALIAS("_FUN_5e1ad0", "_Node_ParentRoot")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_5a3f50(); KING_ALIAS("_FUN_5a3f50", "@Light5a3f50_Attach@12")    // (this, node)
extern "C" void FUN_5de950(); KING_ALIAS("_FUN_5de950", "_Matrix_ToAngles")    // decomposes a matrix (by value) into six doubles
ASM_VAR(AsmLabel, s_67e8f0, $L_67e8f0)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);
struct Weather_Mat48 { DWORD d[12]; };
struct Weather_Vec12 { DWORD d[3]; };

// [FUN_5a4250..$L_5a42b2] follows a scene node: takes its frame (kinds 2, 4, 17) or a position, the root, the world matrix and its six doubles
extern "C" void __fastcall WeatherNode_Attach(BYTE* self, void*, BYTE* node)
{
    if (!node)
        return;
    CallThis(FUN_5a3f50, self, node);
    switch (F(int, node, 16))
    {
    case 2:
        F(BYTE*, self, 48) = F(BYTE*, node, 112);
        break;
    case 4:
        F(BYTE*, self, 48) = F(BYTE*, node, 60);
        break;
    case 17:
        F(BYTE*, self, 48) = F(BYTE*, node, 68);
        if (!F(BYTE*, node, 68))
        {
            BYTE* p = F(BYTE*, node, 64);
            if (F(int, p, 16) == 11)
                F(BYTE*, self, 52) = p + 56;
            else
                Sys_Fatal(s_67e8f0, 0);
        }
        break;
    default:
        F(BYTE*, self, 48) = 0;
    }
    F(BYTE*, self, 56) = CallC<BYTE*>(FUN_5e1ad0, F(BYTE*, self, 32));
    if (F(BYTE*, self, 48))
        CallThis(FUN_5f25c0, F(BYTE*, self, 48), self + 60);
    else
    {
        Weather_Mat48 m;
        BYTE* r = CallC<BYTE*>(FUN_5abc40, &m, *F(Weather_Vec12*, self, 52));
        memcpy(self + 60, r, 48);
    }
    CallC(FUN_5de950, *(Weather_Mat48*)(self + 60), self + 160, self + 168, self + 176, self + 184, self + 192, self + 200);
    memcpy(self + 108, self + 60, 48);
}


// ---------------------------------------------------------------------------
extern "C" size_t __cdecl fread(void* p, size_t size, size_t n, FILE* f);

// [FUN_5ab0f0] a signed 16-bit value from the file
extern "C" int __cdecl File_ReadShort(FILE* f)
{
    short v;
    fread(&v, 2, 1, f);
    return v;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5e5370(); KING_ALIAS("_FUN_5e5370", "@Resources_Get@12")
extern "C" void FUN_5e54c0(); KING_ALIAS("_FUN_5e54c0", "@Resources_AddRecord@20")

// [FUN_5a4700] attaches the object's resource (+344) to a: re-adds it under a's name when that differs, then a's method +28 gets this +256; without a adds it under b and resets this
extern "C" int __fastcall Obj_AttachResource(BYTE* self, void*, BYTE* a, const char* b)
{
    int id = F(int, self, 344);
    BYTE* r = 0;
    if (id >= 0)
        r = CallThis<BYTE*>(FUN_5e5370, F(BYTE*, self, 36) + 704, id);
    if (r)
    {
        id = F(int, self, 344);
        if (!a)
        {
            if (id >= 0)
                CallThis(FUN_5e54c0, F(BYTE*, self, 36) + 704, b, r, 0);
            CallVirt(self, 28, 0);
            return 0;
        }
        if (id >= 0)
        {
            const char* name = F(char*, F(BYTE*, a, 36), 60);
            if (strcmp(name, F(char*, F(BYTE*, r, 12), 4016)))
                CallThis(FUN_5e54c0, F(BYTE*, self, 36) + 704, name, r, 0);
        }
    }
    else if (!a)
        return 0;
    if (!self)
        return CallVirt<int>(a, 28, 0);
    return CallVirt<int>(a, 28, self + 256);
}

// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_595c60(); KING_ALIAS("_FUN_595c60", "@WorldObj_DestroyBase@8")
extern "C" void FUN_5a53a0(); KING_ALIAS("_FUN_5a53a0", "@Wea5a53a0_Destroy@4")
extern "C" void FUN_5a63a0(); KING_ALIAS("_FUN_5a63a0", "@Wea5a63a0_Destroy@4")
extern "C" void FUN_5a6aa0(); KING_ALIAS("_FUN_5a6aa0", "@Wea5a6aa0_Destruct@8")
extern "C" void FUN_5a7240(); KING_ALIAS("_FUN_5a7240", "@Wea5a7240_Destroy@4")
extern "C" void FUN_5a7310(); KING_ALIAS("_FUN_5a7310", "@Wea5a7310_Destroy@4")
extern "C" void FUN_5a73f0(); KING_ALIAS("_FUN_5a73f0", "@Wea5a73f0_Destruct@8")
KING_BSS(DWORD, g_wea6fb3d0, 0x6fb3d0)
KING_BSS(DWORD, g_wea6fb3d4, 0x6fb3d4)
KING_BSS(DWORD, g_wea6fb3d8, 0x6fb3d8)
KING_BSS(DWORD, g_wea6fb3dc, 0x6fb3dc)
KING_BSS(DWORD, g_wea6fb3e0, 0x6fb3e0)
KING_BSS(DWORD, g_wea6fb3e4, 0x6fb3e4)
KING_BSS(DWORD, g_wea6fb3e8, 0x6fb3e8)
KING_BSS(DWORD, g_wea6fb3ec, 0x6fb3ec)
KING_BSS(DWORD, g_wea702620, 0x702620)
KING_BSS(DWORD, g_wea702624, 0x702624)
KING_BSS(DWORD, g_wea702628, 0x702628)
KING_BSS(DWORD, g_wea702630, 0x702630)
KING_BSS(DWORD, g_wea702634, 0x702634)
KING_BSS(DWORD, g_wea702638, 0x702638)
KING_BSS(DWORD, g_wea702640, 0x702640)
KING_BSS(DWORD, g_wea702644, 0x702644)
KING_BSS(DWORD, g_wea702648, 0x702648)
KING_BSS(DWORD, g_wea702650, 0x702650)
KING_BSS(DWORD, g_wea702654, 0x702654)
KING_BSS(DWORD, g_wea702658, 0x702658)

static BYTE* Wea_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5a72e0]
extern "C" int __fastcall Wea_5a72e0(BYTE* s) { return F(int, s, 104); }
// [$L_5aca70]
extern "C" void __cdecl WeaInit_5aca70() {  }
// [$L_5a5380]
extern "C" BYTE* __fastcall Wea5a5380_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_5a53a0); }
// [$L_5a6140]
extern "C" BYTE* __fastcall Wea5a6140_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_595c60); }
// [$L_5a6380]
extern "C" BYTE* __fastcall Wea5a6380_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_5a63a0); }
// [$L_5a6a80]
extern "C" BYTE* __fastcall Wea5a6a80_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_5a6aa0); }
// [$L_5a72a0]
extern "C" BYTE* __fastcall Wea5a72a0_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_5a7240); }
// [$L_5a72f0]
extern "C" BYTE* __fastcall Wea5a72f0_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_5a7310); }
// [$L_5a7410]
extern "C" BYTE* __fastcall Wea5a7410_DeletingDestructor(BYTE* s, void*, int flags) { return Wea_DeleteAfter(s, flags, FUN_5a73f0); }
// [$L_5ab340]
extern "C" void __cdecl WeaInit_5ab340() { g_wea702650 = 0x0; g_wea702654 = 0x0; g_wea702658 = 0x0; }
// [$L_5ab370]
extern "C" void __cdecl WeaInit_5ab370() { g_wea702640 = 0x3f800000; g_wea702644 = 0x0; g_wea702648 = 0x0; }
// [$L_5ab3a0]
extern "C" void __cdecl WeaInit_5ab3a0() { g_wea702620 = 0x0; g_wea702624 = 0x3f800000; g_wea702628 = 0x0; }
// [$L_5ab3d0]
extern "C" void __cdecl WeaInit_5ab3d0() { g_wea702630 = 0x0; g_wea702634 = 0x0; g_wea702638 = 0x3f800000; }
// [$L_5a73a0]
extern "C" void __cdecl WeaInit_5a73a0() { g_wea6fb3e4 = 0x3ff00000; g_wea6fb3dc = 0; g_wea6fb3d8 = 0; g_wea6fb3d4 = 0; g_wea6fb3d0 = 0; g_wea6fb3e0 = 0; g_wea6fb3e8 = 0; g_wea6fb3ec = 0x3ff00000; }

extern "C" void FUN_5d33d0(); KING_ALIAS("_FUN_5d33d0", "@Queue_PostEvent2@12")
extern "C" void FUN_5d3350(); KING_ALIAS("_FUN_5d3350", "@Queue_PostEvent1@12")

// [$L_5a6ab0]
extern "C" int __fastcall Wea_5a6ab0(BYTE* s, void*, int a) { return CallThis<int>(FUN_5d33d0, s, a); }
// [$L_5a6ac0]
extern "C" int __fastcall Wea_5a6ac0(BYTE* s, void*, int a) { return CallThis<int>(FUN_5d3350, s, a); }
// [$L_5ad130] 1 when +8244 is set
extern "C" int __fastcall Wea_Has8244(BYTE* s) { return F(int, s, 8244) != 0; }

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

KING_BSS(AsmLabel, g_wea7025f0, 0x7025f0)

// [$L_5ab230] static initializer: the identity matrix and zero translation at 7025f0
extern "C" void __cdecl WeaInit_5ab230()
{
    DWORD* m = (DWORD*)g_wea7025f0;
    for (int i = 0; i < 12; i++)
        m[i] = 0;
    m[0] = m[4] = m[8] = 0x3f800000;
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_5fdaf0(); KING_ALIAS("_FUN_5fdaf0", "@Sound_Restart@16")
extern "C" void FUN_5a4ca0(); KING_ALIAS("_FUN_5a4ca0", "@SheetAnim_Step@4")
KING_BSS(AsmLabel, g_wea6fd7dc, 0x6fd7dc)

// the color entry 48 of the room's palette (+100 of +468 of the weather's world +100)
static BYTE* Wea_SkyColor(BYTE* s) { return *F(BYTE**, F(BYTE*, F(BYTE*, F(BYTE*, s, 100), 468), 100), 0) + 48; }

// [$L_5a5610] the thunder sound once its time (+144 after +136) has come; lightning flashes the sky color
extern "C" void __fastcall Wea_Lightning(BYTE* s)
{
    if (F(double, F(BYTE*, s, 100), 2960) + F(double, s, 144) >= F(double, s, 136) && !F(int, s, 180)) {
        F(int, s, 180) = 1;
        if (F(BYTE*, s, 176)) {
            float v = (float)F(double, s, 184);
            CallThis(FUN_5fdaf0, F(BYTE*, s, 176), *(DWORD*)&v, *(DWORD*)&v);
        }
    }
    CallThis(FUN_5a4ca0, s);
    int m = F(int, s, 16);
    if (m != 0 && m != 1)
        return;
    BYTE* w = F(BYTE*, s, 100);
    if (F(BYTE, w, 3020) & 64 || CallVirt<int>(w, 28) != 2)
        return;
    BYTE* c = Wea_SkyColor(s);
    BYTE* save = (BYTE*)g_wea6fd7dc;
    if (m == 0) {
        save[0] = c[0];
        save[1] = c[1];
        save[2] = c[2];
        c[0] = 235;
        c[1] = 235;
        c[2] = 255;
    } else {
        c[0] = save[0];
        c[1] = save[1];
        c[2] = save[2];
    }
}

// ---------------------------------------------------------------------------
// More virtual methods.

extern "C" void FUN_5dbcc0(); KING_ALIAS("_FUN_5dbcc0", "@Camera_Get@12")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5fd870(); KING_ALIAS("_FUN_5fd870", "@Sound_UpdateAt@36")

// [$L_5a5000] the rain sound (+176) is placed at the point (+88..+96) seen from the camera
extern "C" void __fastcall Wea_RainSound(BYTE* s)
{
    if (!F(int, s, 16) && F(BYTE*, s, 176)) {
        BYTE frame[108];  // the original's stack frame: the matrix at +12, the camera buffer at +64
        float* m = (float*)(frame + 12);
        BYTE* cam = CallThis<BYTE*>(FUN_5dbcc0, F(BYTE*, s, 100), frame + 64);
        CallC(FUN_5ac2e0, m, cam);
        float px = F(float, s, 88), py = F(float, s, 92), pz = F(float, s, 96);
        float v[3];
        v[0] = (float)((double)m[1] * px + (double)m[7] * pz + (double)m[4] * py + m[10]);
        v[1] = (float)((double)m[2] * px + (double)m[8] * pz + (double)m[5] * py + m[11]);
        v[2] = (float)((double)m[3] * px + (double)m[9] * pz + (double)m[6] * py + m[12]);
        CallThis(FUN_5fd870, F(BYTE*, s, 176), *(DWORD*)&v[0], *(DWORD*)&v[1], *(DWORD*)&v[2], 50.0, F(double, s, 184));
    }
    CallThis(FUN_5a4ca0, s);
}

extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
ASM_VAR(AsmLabel, g_wea6514d4, $L_6514d4)
ASM_VAR(AsmLabel, g_wea651590, $L_651590)

// [FUN_5a53a0] (vtable 6514d4) frees +40 and +36
extern "C" void __fastcall Wea5a53a0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_wea6514d4;
    operator delete(F(void*, s, 40));
    operator delete(F(void*, s, 36));
    CallThis(FUN_5d3290, s);
}
// [FUN_5a7240] (vtable 651590) frees +76 and +80
extern "C" void __fastcall Wea5a7240_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_wea651590;
    operator delete(F(void*, s, 76));
    operator delete(F(void*, s, 80));
    CallThis(FUN_5ef660, s);
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5fa720(); KING_ALIAS("_FUN_5fa720", "@Sce5fa720_Destroy@4")
extern "C" void FUN_5bad90(); KING_ALIAS("_FUN_5bad90", "@Texture_SetPixels@20")
extern "C" void FUN_5b6a90(); KING_ALIAS("_FUN_5b6a90", "@Window_FreeBuffers@8")
extern "C" void FUN_5d33b0(); KING_ALIAS("_FUN_5d33b0", "@Handle_Release@8")
extern "C" void FUN_5e5840(); KING_ALIAS("_FUN_5e5840", "@Rec5e5840_Free@8")
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
extern "C" void Wea_VecDtor(); KING_ALIAS("_Wea_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
ASM_VAR(AsmLabel, g_wea651390, $L_651390)
ASM_VAR(AsmLabel, g_wea6513b0, $L_6513b0)
ASM_VAR(AsmLabel, g_wea651548, $L_651548)
ASM_VAR(AsmLabel, g_wea651358, $L_651358)
ASM_VAR(AsmLabel, g_wea65130c, $L_65130c)
ASM_VAR(AsmLabel, g_wea6515dc, $L_6515dc)
ASM_VAR(AsmLabel, g_wea5a3df0, $L_5a3df0)

// [FUN_5a4e90] (vtables 651390, 6513b0 at +32)
extern "C" void __fastcall Wea5a4e90_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_wea651390;
    F(void*, s, 32) = g_wea6513b0;
    CallThis(FUN_5ef780, s + 32);
    CallThis(FUN_5fa720, s ? s + 32 : 0);
    CallThis(FUN_5d3290, s);
}
// [FUN_5a63a0] (vtable 651548): its picture (+148) stopped, its surface (+152) freed
extern "C" void __fastcall Wea5a63a0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_wea651548;
    CallThis(FUN_5bad90, F(BYTE*, s, 148), 0, 0, 0);
    BYTE* p = F(BYTE*, s, 152);
    if (p) {
        CallThis(FUN_5b6a90, p);
        operator delete(p);
    }
    CallThis(FUN_5d3290, s);
}
// [FUN_5a47f0] the helicopter's object (vtables 651358, 65130c at +256)
extern "C" void __fastcall Wea5a47f0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_wea651358;
    F(void*, s, 256) = g_wea65130c;
    CallVirt(F(BYTE*, s, 32), 36);
    if (F(BYTE*, s, 324))
        CallVirt(F(BYTE*, s, 324), 0, 1);
    CallThis(FUN_5d33b0, s);
    CallThis(FUN_5ef660, s ? s + 256 : 0);
    CallThis(FUN_5d3290, s);
}
// [FUN_5a7310] (vtable 6515dc): its 28-byte items (+60), +64, +68
extern "C" void __fastcall Wea5a7310_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_wea6515dc;
    BYTE* a = F(BYTE*, s, 60);
    if (a) {
        CallStd(Wea_VecDtor, a, 28, F(int, a, -4), (BYTE*)g_wea5a3df0);
        operator delete(a - 4);
    }
    operator delete(F(void*, s, 64));
    CallThis(FUN_5e5840, s + 68);
    CallThis(FUN_5f1a70, s);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
ASM_VAR(AsmLabel, s_wea67e94c, $L_67e94c)
ASM_VAR(AsmLabel, g_wea6514f4, $L_6514f4)

// [FUN_5a5790] constructor (world, name, a..f): the weather (vtable 6514f4) registered with the world
extern "C" BYTE* __fastcall Weather_Construct(BYTE* s, void*, BYTE* w, int name, int a, int b, int c, int d, int e, int f)
{
    CallThis(FUN_5d31e0, s, name, (const char*)s_wea67e94c);
    F(int, s, 124) = a;
    F(int, s, 128) = b;
    F(int, s, 132) = c;
    F(int, s, 136) = d;
    F(int, s, 140) = e;
    F(DWORD, s, 112) = 1293080650;
    F(DWORD, s, 80) = 1431655765;
    F(DWORD, s, 88) = 1202590843;
    F(DWORD, s, 72) = 0;
    F(void*, s, 0) = g_wea6514f4;
    F(BYTE*, s, 32) = w;
    F(int, s, 144) = f;
    F(int, s, 164) = 0;
    F(DWORD, s, 116) = 1073291771;
    F(int, s, 120) = 1;
    F(DWORD, s, 84) = 1070945621;
    F(DWORD, s, 92) = 1072724705;
    F(DWORD, s, 76) = 1072693248;
    F(int, s, 96) = 200;
    F(int, s, 100) = 100;
    F(int, s, 168) = 0;
    CallThis(FUN_5d3570, w + 2992, s);
    static const int zero[] = {56, 48, 176, 184, 60, 52, 156, 180, 152, 188};
    for (int o : zero)
        F(int, s, o) = 0;
    F(int, s, 104) = 30;
    F(int, s, 108) = 97;
    return s;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

#include <string.h>
ASM_VAR(AsmLabel, s_wea67e940, $L_67e940)

static char* Wea_StrDup(const char* t)
{
    size_t n = strlen(t) + 1;
    char* d = (char*)operator new(n);
    memcpy(d, t, n);
    return d;
}
// [FUN_5a5270] constructor (world, name1, name2, a, b, c, d): a weather layer (vtable 6514d4) started now
extern "C" BYTE* __fastcall WeatherLayer_Construct(BYTE* s, void*, BYTE* w, const char* n1, const char* n2, int a, int b, int c, int d)
{
    CallThis(FUN_5d31e0, s, 100, (const char*)s_wea67e940);
    F(void*, s, 0) = g_wea6514d4;
    F(BYTE*, s, 32) = w;
    F(int, s, 72) = 0;
    F(char*, s, 40) = Wea_StrDup(n1);
    F(char*, s, 36) = Wea_StrDup(n2);
    F(DWORD, s, 48) = F(DWORD, w, 2968);
    F(DWORD, s, 52) = F(DWORD, w, 2972);
    F(int, s, 56) = a;
    F(int, s, 60) = b;
    F(int, s, 64) = c;
    F(int, s, 68) = d;
    CallThis(FUN_5d3570, F(BYTE*, s, 32) + 2992, s);
    return s;
}

// ---------------------------------------------------------------------------
// Gauge constructor (exception frame dropped).

extern "C" void FUN_5a5790(); KING_ALIAS("_FUN_5a5790", "@Weather_Construct@40")
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
extern "C" void FUN_5ef3d0(); KING_ALIAS("_FUN_5ef3d0", "_Objects_Find")
extern "C" void FUN_5b63e0(); KING_ALIAS("_FUN_5b63e0", "@Surface_Construct@12")
extern "C" void FUN_5b6e00(); KING_ALIAS("_FUN_5b6e00", "@Surface_Clear@16")
extern "C" int __cdecl sprintf(char* buf, const char* format, ...);
ASM_VAR(AsmLabel, g_q24_651548, $L_651548)
KING_BSS(int, g_q24_7025b0, 0x7025b0)
ASM_VAR(AsmLabel, s_q24_67e9f0, $L_67e9f0)
ASM_VAR(AsmLabel, s_q24_66d9c4, $L_66d9c4)
ASM_VAR(AsmLabel, s_q24_67e9d0, $L_67e9d0)
ASM_VAR(AsmLabel, s_q24_67e9ac, $L_67e9ac)
ASM_VAR(AsmLabel, s_q24_67e99c, $L_67e99c)
ASM_VAR(AsmLabel, s_q24_67e980, $L_67e980)
ASM_VAR(AsmLabel, s_q24_67e970, $L_67e970)
ASM_VAR(AsmLabel, s_q24_67e954, $L_67e954)

static BYTE* Wea_Find(const char* fmt, int name)
{
    char buf[64];
    sprintf(buf, fmt, name);
    return CallC<BYTE*>(FUN_5ef3d0, buf);
}
// [FUN_5a6160] constructor (a1, a2, world, name, a5..a10): a gauge (vtable 651548, kind 9) with its object's
// surface (+148, +152), two more images (+64, +68) and its room material (+200)
extern "C" BYTE* __fastcall Gauge_Construct(BYTE* s, void*, int a1, int a2, BYTE* w, int name, int a5, int a6,
                                            int a7, int a8, int a9, int a10)
{
    CallThis(FUN_5a5790, s, a1, 9, a5, a6, a7, a8, a9, a10);
    F(void*, s, 0) = g_q24_651548;
    if (!w)
        CallC(FUN_5d2ba0, (const char*)s_q24_67e9f0, 0);
    F(BYTE*, s, 196) = w;
    BYTE* r = Wea_Find((const char*)s_q24_66d9c4, name);
    if (!r) {
        CallC(FUN_5d2ba0, (const char*)s_q24_67e9d0, 0);
        F(int, s, 148) = 0;
    } else {
        F(int, s, 148) = F(int, r, 76);
    }
    if (!F(int, s, 148))
        CallC(FUN_5d2ba0, (const char*)s_q24_67e9ac, 0);
    F(int, s, 200) = 0;
    int k = F(int, r, 44);
    if (k >= 0 && w && g_q24_7025b0)
        F(int, s, 200) = F(int, F(BYTE*, F(BYTE*, w, 36), 76) + k * 124, 76);
    else
        F(int, r, 44) = -1;
    F(BYTE*, s, 64) = Wea_Find((const char*)s_q24_67e99c, name);
    if (!F(BYTE*, s, 64))
        CallC(FUN_5d2ba0, (const char*)s_q24_67e980, 0);
    F(BYTE*, s, 68) = Wea_Find((const char*)s_q24_67e970, name);
    if (!F(BYTE*, s, 68))
        CallC(FUN_5d2ba0, (const char*)s_q24_67e954, 0);
    F(int, s, 40) = 0;
    F(int, s, 192) = a2;
    F(DWORD, s, 176) = 2468160491u;
    F(DWORD, s, 44) = 1077149696u;
    F(DWORD, s, 180) = 3216983739u;
    F(int, s, 104) = 61;
    F(int, s, 108) = 190;
    F(int, s, 96) = 64;
    F(int, s, 100) = 234;
    if (F(int, s, 148)) {
        BYTE* m = (BYTE*)operator new(124);
        BYTE* o = m ? CallThis<BYTE*>(FUN_5b63e0, m, F(int, s, 148)) : 0;
        F(BYTE*, s, 152) = o;
        CallThis(FUN_5b6e00, o, 0, 1);
    } else {
        F(int, s, 152) = 0;
    }
    return s;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_5a47f0(); KING_ALIAS("_FUN_5a47f0", "@Wea5a47f0_Destroy@4")
// [$L_5a46e0]
extern "C" BYTE* __fastcall Dtor5a46e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5a47f0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_5a4e90(); KING_ALIAS("_FUN_5a4e90", "@Wea5a4e90_Destroy@4")
// [$L_5a4c80]
extern "C" BYTE* __fastcall Dtor5a4c80_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5a4e90, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_5a4ff0(); KING_ALIAS("_FUN_5a4ff0", "@Thunk5a4ff0@4")
// [$L_5a5250]
extern "C" BYTE* __fastcall Dtor5a5250_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5a4ff0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")
ASM_VAR(AsmLabel, g_q46_651568, $L_651568)
// [FUN_5a6aa0] destructor: the 651568 vtable, then the object
extern "C" void __fastcall Wea5a6aa0_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q46_651568;
    CallThis(FUN_5d3290, s);
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

ASM_VAR(AsmLabel, g_q47_651628, $L_651628)
KING_BSS(int, g_q47_7025b8, 0x7025b8)
// [FUN_5a73f0] destructor: the 651628 vtable, clears the 7025b8 instance, then the object
extern "C" void __fastcall Wea5a73f0_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q47_651628;
    g_q47_7025b8 = 0;
    CallThis(FUN_5d3290, s);
}

extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
ASM_VAR(AsmLabel, s_q161_67ef54, $L_67ef54)
// [$L_5aa470] debug visitor: with renderer r's +8620 set logs the position (+20..+28) of object o
extern "C" int __cdecl Weather_LogPos(void*, const BYTE* r, const BYTE* o)
{
    if (F(int, r, 8620) == 1) {
        char buf[64];
        float p[4];
        memcpy(p, o + 20, 16);
        sprintf(buf, (const char*)s_q161_67ef54, (double)p[0], (double)p[1], (double)p[2]);
        CallC(FUN_5d2ba0, buf, 0);
    }
    return 0;
}

extern "C" void FUN_5aa4f0(); KING_ALIAS("_FUN_5aa4f0", "@Water_Height@20")
ASM_VAR(double, g_q175_64b5b8, $L_64b5b8)
ASM_VAR(double, g_q175_64b780, $L_64b780)
ASM_VAR(float, g_q175_64c250, $L_64c250)
struct Wea175_V3 { float x, y, z; };
// [FUN_5aa670] the height of surface s at x, y, z (5aa4f0) and its normal from the heights a step (64b5b8) away
// in x and y into out
extern "C" double __fastcall Surface_HeightNormal(BYTE* s, void*, float x, float y, float z, float* out)
{
    Wea175_V3 v = {x, y, z};
    double h0 = CallThis<double>(FUN_5aa4f0, s, v);
    float x1 = (float)(x + g_q175_64b5b8);
    v.x = x1;
    double h1 = CallThis<double>(FUN_5aa4f0, s, v);
    v.x = (float)(x1 - g_q175_64b5b8);
    v.y = (float)(y + g_q175_64b5b8);
    double h2 = CallThis<double>(FUN_5aa4f0, s, v);
    float nx = (float)((h0 - h1) * g_q175_64b780);
    out[0] = nx;
    double ny = (h0 - h2) * g_q175_64b780;
    out[1] = (float)ny;
    out[2] = (float)x87_sqrt(g_q175_64c250 - nx * nx - ny * ny);
    return h0;
}

ASM_VAR(float, g_q176_64c320, $L_64c320)
ASM_VAR(double, g_q176_64d908, $L_64d908)
ASM_VAR(double, g_q176_64b6e0, $L_64b6e0)
ASM_VAR(double, g_q176_64d900, $L_64d900)
// [FUN_5aa4f0] the wave height of water s at p: two waves along directions +4 and +16 (wave lengths +40, +48)
// moving with the world time, each bending the other, amplitude +72
extern "C" double __fastcall Water_Height(BYTE* s, void*, float px, float py, float pz)
{
    (void)pz;
    float k = g_q176_64c320;
    double t = F(double, F(BYTE*, s, 0), 2968);
    float tf = (float)t;
    const float* d = (const float*)(s + 4);
    float ax = (float)(d[0] * k), ay = (float)(d[1] * k), az = (float)(d[2] * k);
    float tax = (float)(ax * t), tay = (float)(ay * t);
    double taz = t * az;
    float qx = (float)(px - tax), qy = (float)(py - tay);
    double dot1 = qy * d[1] + -taz * d[2];
    dot1 = dot1 + qx * d[0];
    double ph1 = dot1 * g_q176_64d908 / F(double, s, 40);
    const float* e = (const float*)(s + 16);
    double bx = e[0] * k;
    float by = (float)(e[1] * k), bz = (float)(e[2] * k);
    double bxt = bx * tf;
    float byt = (float)(by * tf), bzt = (float)(bz * tf);
    double rx = px - bxt, ry = py - byt;
    double dot2 = ry * e[1] + -bzt * e[2];
    dot2 = dot2 + rx * e[0];
    double ph2 = dot2 * g_q176_64d908 / F(double, s, 48);
    double a = F(double, s, 72);
    double t1 = x87_sin(x87_sin(ph2 * g_q176_64b6e0) * g_q176_64d900 + ph1) * a;
    double t2 = x87_sin(x87_sin(ph1 * g_q176_64b6e0) * g_q176_64d900 + ph2) * a;
    return t1 + t2;
}

ASM_VAR(float, g_q177_64c258, $L_64c258)
// [FUN_5a8c20] the colour at x of the gradient a (at f0), b (midway), c (at f1) into out
extern "C" void __cdecl Gradient3(const float* a, const float* b, const float* c, float f0, float x, float f1,
                                  float* out)
{
    double mid = (f0 + f1) * g_q177_64c258;
    if (!(x <= f1)) {
        memcpy(out, c, 12);
        return;
    }
    if (!(x >= f0)) {
        memcpy(out, a, 12);
        return;
    }
    if (x > mid) {
        double u = (x - mid) / (f1 - mid), w = g_q175_64c250 - u;
        for (int k = 0; k < 3; k++)
            out[k] = (float)(w * b[k] + u * c[k]);
    } else {
        double u = (mid - x) / (mid - f0), w = g_q175_64c250 - u;
        for (int k = 0; k < 3; k++)
            out[k] = (float)(u * a[k] + w * b[k]);
    }
}

ASM_VAR(double, g_q178_64f398, $L_64f398)
ASM_VAR(double, g_q178_64b840, $L_64b840)
ASM_VAR(double, g_q178_64b678, $L_64b678)
ASM_VAR(double, g_q178_64d338, $L_64d338)
// [FUN_5a8da0] the sky colour at height x into out from the colour table s (two 3-colour gradients, the second
// set at +48; the middle colours 1 or 3 by half day a2) mixed by t; mode 1 uses the other height range
extern "C" void __cdecl Sky_Color(float x, float a2, float t, float* out, BYTE* s, int mode)
{
    double hi = g_q178_64f398, lo = g_q178_64b840;
    if (mode == 1) {
        hi = g_q178_64b678;
        lo = g_q178_64d338;
    }
    int m = a2 <= g_q176_64b6e0 || !(a2 > g_q176_64b6e0) ? 1 : 3;
    float f0 = (float)hi, f1 = (float)lo;
    float c1[3], c2[3];
    Gradient3((const float*)s, (const float*)(s + m * 12), (const float*)(s + 24), f0, x, f1, c1);
    Gradient3((const float*)(s + 48), (const float*)(s + 48 + m * 12), (const float*)(s + 72), f0, x, f1, c2);
    Color_Lerp(c1, c2, t, out);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor5a5250_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor5a5250_DeletingDestructor@12", "@Dtor5a5250_DeletingDestructor@12")
// [$L_5ab0c0] forwards to Dtor5a5250_DeletingDestructor for the object 32 bytes before
extern "C" int __fastcall Thunk5ab0c0(BYTE* s, void* d, int a0) { return q198_Dtor5a5250_DeletingDestructor(s - 32, d, a0); }

// [$L_5a72c0] clears the seven words of s
extern "C" BYTE* __fastcall Wea5a72c0_Init(BYTE* s)
{
    F(int, s, 0) = 0;
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(int, s, 16) = 0;
    F(int, s, 12) = 0;
    F(int, s, 24) = 0;
    F(int, s, 20) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Wea5a4e90_Destroy(BYTE* s); KING_ALIAS("@q238_Wea5a4e90_Destroy@4", "@Wea5a4e90_Destroy@4")
// [FUN_5a4ff0] forwards to Wea5a4e90_Destroy
extern "C" int __fastcall Thunk5a4ff0(BYTE* s) { return q238_Wea5a4e90_Destroy(s); }

// ---------------------------------------------------------------------------
// Weather settings

extern "C" void FUN_5a88f0(); KING_ALIAS("_FUN_5a88f0", "_Color_Parse")
extern "C" void FUN_603cb0(); KING_ALIAS("_FUN_603cb0", "_Ini_GetDouble")
KING_BSS(DWORD, g_weatherFlags6fca38, 0x6fca38)    // [8]: 1 rain, 2 sun, 4 thunder
KING_BSS(BYTE, g_w6f9cc0, 0x6f9cc0)
KING_BSS(BYTE, g_w6f9ccc, 0x6f9ccc)
KING_BSS(BYTE, g_w6f9cd8, 0x6f9cd8)
KING_BSS(BYTE, g_w6f9ce4, 0x6f9ce4)
KING_BSS(BYTE, g_w6f9cf0, 0x6f9cf0)
KING_BSS(BYTE, g_w6f9cfc, 0x6f9cfc)
KING_BSS(BYTE, g_w6f9d08, 0x6f9d08)
KING_BSS(BYTE, g_w6f9d14, 0x6f9d14)
KING_BSS(BYTE, g_w6fa150, 0x6fa150)
KING_BSS(BYTE, g_w6fa15c, 0x6fa15c)
KING_BSS(BYTE, g_w6fa168, 0x6fa168)
KING_BSS(BYTE, g_w6fa174, 0x6fa174)
KING_BSS(BYTE, g_w6fa180, 0x6fa180)
KING_BSS(BYTE, g_w6fa18c, 0x6fa18c)
KING_BSS(BYTE, g_w6fa198, 0x6fa198)
KING_BSS(BYTE, g_w6fa1a4, 0x6fa1a4)
KING_BSS(BYTE, g_w6fa450, 0x6fa450)
KING_BSS(BYTE, g_w6fa45c, 0x6fa45c)
KING_BSS(BYTE, g_w6fa468, 0x6fa468)
KING_BSS(BYTE, g_w6fa474, 0x6fa474)
KING_BSS(BYTE, g_w6fa480, 0x6fa480)
KING_BSS(BYTE, g_w6fa48c, 0x6fa48c)
KING_BSS(BYTE, g_w6fa498, 0x6fa498)
KING_BSS(BYTE, g_w6fa4a4, 0x6fa4a4)
KING_BSS(BYTE, g_w6fb3f4, 0x6fb3f4)
KING_BSS(BYTE, g_w6fb400, 0x6fb400)
KING_BSS(BYTE, g_w6fb40c, 0x6fb40c)
KING_BSS(BYTE, g_w6fb418, 0x6fb418)
KING_BSS(BYTE, g_w6fb424, 0x6fb424)
KING_BSS(BYTE, g_w6fb430, 0x6fb430)
KING_BSS(BYTE, g_w6fb43c, 0x6fb43c)
KING_BSS(BYTE, g_w6fb448, 0x6fb448)
KING_BSS(BYTE, g_w6fc9b8, 0x6fc9b8)
KING_BSS(BYTE, g_w6fc9bc, 0x6fc9bc)
KING_BSS(BYTE, g_w6fc9c0, 0x6fc9c0)
KING_BSS(BYTE, g_w6fc9c4, 0x6fc9c4)
KING_BSS(BYTE, g_w6fca58, 0x6fca58)
KING_BSS(BYTE, g_w6fca5c, 0x6fca5c)
KING_BSS(BYTE, g_w6fca60, 0x6fca60)
KING_BSS(BYTE, g_w6fca64, 0x6fca64)
KING_BSS(BYTE, g_w6fd75c, 0x6fd75c)
KING_BSS(BYTE, g_w6fd760, 0x6fd760)
KING_BSS(BYTE, g_w6fd764, 0x6fd764)
KING_BSS(BYTE, g_w6fd768, 0x6fd768)
KING_BSS(BYTE, g_w701ca0, 0x701ca0)
KING_BSS(BYTE, g_w701cac, 0x701cac)
KING_BSS(BYTE, g_w701cb8, 0x701cb8)
KING_BSS(BYTE, g_w701cc4, 0x701cc4)
KING_BSS(BYTE, g_w701cd0, 0x701cd0)
KING_BSS(BYTE, g_w701cdc, 0x701cdc)
KING_BSS(BYTE, g_w701ce8, 0x701ce8)
KING_BSS(BYTE, g_w701cf4, 0x701cf4)
KING_BSS(BYTE, g_w701fa0, 0x701fa0)
KING_BSS(BYTE, g_w701fac, 0x701fac)
KING_BSS(BYTE, g_w701fb8, 0x701fb8)
KING_BSS(BYTE, g_w701fc4, 0x701fc4)
KING_BSS(BYTE, g_w701fd0, 0x701fd0)
KING_BSS(BYTE, g_w701fdc, 0x701fdc)
KING_BSS(BYTE, g_w701fe8, 0x701fe8)
KING_BSS(BYTE, g_w701ff4, 0x701ff4)
KING_BSS(BYTE, g_w7022a0, 0x7022a0)
KING_BSS(BYTE, g_w7022ac, 0x7022ac)
KING_BSS(BYTE, g_w7022b8, 0x7022b8)
KING_BSS(BYTE, g_w7022c4, 0x7022c4)
KING_BSS(BYTE, g_w7022d0, 0x7022d0)
KING_BSS(BYTE, g_w7022dc, 0x7022dc)
KING_BSS(BYTE, g_w7022e8, 0x7022e8)
KING_BSS(BYTE, g_w7022f4, 0x7022f4)

struct WeatherColorKey { const char* key; BYTE* to; };
static const WeatherColorKey s_weatherColors[] = {
    {"fog_badnight", &g_w6fb3f4},
    {"fog_badsrise", &g_w6fb400},
    {"fog_badday", &g_w6fb40c},
    {"fog_badsset", &g_w6fb418},
    {"fog_goodnight", &g_w6fb424},
    {"fog_goodsrise", &g_w6fb430},
    {"fog_goodday", &g_w6fb43c},
    {"fog_goodsset", &g_w6fb448},
    {"sunenv_badnight", &g_w701fa0},
    {"sunenv_badsrise", &g_w701fac},
    {"sunenv_badday", &g_w701fb8},
    {"sunenv_badsset", &g_w701fc4},
    {"sunenv_goodnight", &g_w701fd0},
    {"sunenv_goodsrise", &g_w701fdc},
    {"sunenv_goodday", &g_w701fe8},
    {"sunenv_goodsset", &g_w701ff4},
    {"sunsky_badnight", &g_w6f9cc0},
    {"sunsky_badsrise", &g_w6f9ccc},
    {"sunsky_badday", &g_w6f9cd8},
    {"sunsky_badsset", &g_w6f9ce4},
    {"sunsky_goodnight", &g_w6f9cf0},
    {"sunsky_goodsrise", &g_w6f9cfc},
    {"sunsky_goodday", &g_w6f9d08},
    {"sunsky_goodsset", &g_w6f9d14},
    {"suncld_badnight", &g_w701ca0},
    {"suncld_badsrise", &g_w701cac},
    {"suncld_badday", &g_w701cb8},
    {"suncld_badsset", &g_w701cc4},
    {"suncld_goodnight", &g_w701cd0},
    {"suncld_goodsrise", &g_w701cdc},
    {"suncld_goodday", &g_w701ce8},
    {"suncld_goodsset", &g_w701cf4},
    {"skyamb_badnight", &g_w7022a0},
    {"skyamb_badsrise", &g_w7022ac},
    {"skyamb_badday", &g_w7022b8},
    {"skyamb_badsset", &g_w7022c4},
    {"skyamb_goodnight", &g_w7022d0},
    {"skyamb_goodsrise", &g_w7022dc},
    {"skyamb_goodday", &g_w7022e8},
    {"skyamb_goodsset", &g_w7022f4},
    {"envamb_badnight", &g_w6fa450},
    {"envamb_badsrise", &g_w6fa45c},
    {"envamb_badday", &g_w6fa468},
    {"envamb_badsset", &g_w6fa474},
    {"envamb_goodnight", &g_w6fa480},
    {"envamb_goodsrise", &g_w6fa48c},
    {"envamb_goodday", &g_w6fa498},
    {"envamb_goodsset", &g_w6fa4a4},
    {"moon_badnight", &g_w6fa150},
    {"moon_badsrise", &g_w6fa15c},
    {"moon_badday", &g_w6fa168},
    {"moon_badsset", &g_w6fa174},
    {"moon_goodnight", &g_w6fa180},
    {"moon_goodsrise", &g_w6fa18c},
    {"moon_goodday", &g_w6fa198},
    {"moon_goodsset", &g_w6fa1a4},
};
struct WeatherDoubleKey { const char* key; DWORD hi; BYTE* to; };
static const WeatherDoubleKey s_weatherDoubles[] = {
    {"frpl_badnight", 1072693248u, &g_w6fd75c},
    {"frpl_badday", 1072693248u, &g_w6fd760},
    {"frpl_goodnight", 1072693248u, &g_w6fd764},
    {"frpl_goodday", 1072693248u, &g_w6fd768},
    {"fogn_badnight", 1077149696u, &g_w6fca58},
    {"fogn_badday", 1078525952u, &g_w6fca5c},
    {"fogn_goodnight", 1082081280u, &g_w6fca60},
    {"fogn_goodday", 1082720256u, &g_w6fca64},
    {"fogf_badnight", 1079574528u, &g_w6fc9b8},
    {"fogf_badday", 1081032704u, &g_w6fc9bc},
    {"fogf_goodnight", 1082720256u, &g_w6fc9c0},
    {"fogf_goodday", 1083461632u, &g_w6fc9c4},
};

static bool Weather_On(const char* sect, const char* key, const char* ini)
{
    char buf[70];
    GetPrivateProfileStringA(sect, key, "on", buf, 70, ini);
    return !strcmp(buf, "on");
}

extern "C" {

// [FUN_5a7b50] the eight weathers' sections WEATHER1..8 of the ini file: the fog, sky and light colours by time of
// day, the far plane and fog distances, and whether it rains, shines and thunders
void __cdecl Weather_LoadSettings(const char* ini)
{
    for (int k = 0; k < 8; k++) {
        char sect[20], buf[70];
        sprintf(sect, "WEATHER%d", k + 1);
        for (int i = 0; i < (int)(sizeof(s_weatherColors) / sizeof(s_weatherColors[0])); i++) {
            GetPrivateProfileStringA(sect, s_weatherColors[i].key, "0 0 0", buf, 70, ini);
            CallC(FUN_5a88f0, s_weatherColors[i].to + k * 96, buf);
        }
        for (int i = 0; i < (int)(sizeof(s_weatherDoubles) / sizeof(s_weatherDoubles[0])); i++) {
            DWORD bits[2] = {0, s_weatherDoubles[i].hi};
            double d;
            CallC(FUN_603cb0, sect, s_weatherDoubles[i].key, *(double*)bits, &d, ini);
            *(float*)(s_weatherDoubles[i].to + k * 16) = (float)d;
        }
        DWORD* f = &g_weatherFlags6fca38 + k;
        *f = 0;
        if (Weather_On(sect, "rain", ini))
            *f |= 1;
        if (Weather_On(sect, "sun", ini))
            *f |= 2;
        if (Weather_On(sect, "thund", ini))
            *f |= 4;
    }
}

}  // extern "C"

// === the weather object (FUN_5a7430)

ASM_VAR(AsmLabel, s_wo67eaa0, $L_67eaa0)
ASM_VAR(AsmLabel, s_wo67ea90, $L_67ea90)
ASM_VAR(AsmLabel, s_wo67ea84, $L_67ea84)
ASM_VAR(AsmLabel, s_wo67ea80, $L_67ea80)
ASM_VAR(AsmLabel, s_wo679a18, $L_679a18)
ASM_VAR(AsmLabel, s_wo679a10, $L_679a10)
ASM_VAR(AsmLabel, s_wo671488, $L_671488)
ASM_VAR(AsmLabel, s_wo67ea78, $L_67ea78)
ASM_VAR(AsmLabel, s_wo677738, $L_677738)
ASM_VAR(AsmLabel, s_wo67ea64, $L_67ea64)
ASM_VAR(AsmLabel, s_wo67ea54, $L_67ea54)
ASM_VAR(AsmLabel, s_wo67ea4c, $L_67ea4c)
ASM_VAR(AsmLabel, g_woVtbl651628, $L_651628)
ASM_VAR(AsmLabel, g_woLevels67e848, $L_67e848)  // 16 doubles
ASM_VAR(double, g_wo651650, $L_651650)
ASM_VAR(double, g_wo651648, $L_651648)
KING_BSS(BYTE*, g_wo7025b8, 0x7025b8)
KING_BSS(AsmLabel, g_wo6fb3d0, 0x6fb3d0)
ASM_PROC(FUN_5a6890)

static BYTE* Wo_Find(const char* prefix, const char* suffix)
{
    char buf[32];
    strcpy(buf, prefix);
    strcat(buf, suffix);
    return CallC<BYTE*>(FUN_5ef2b0, 0, buf);
}

static double Wo_Double(DWORD lo, DWORD hi)
{
    unsigned __int64 v = (unsigned __int64)hi << 32 | lo;
    return *(double*)&v;
}

extern "C" {

// [FUN_5a7430] constructor (view, names a, b, c, -, d): reads the weather settings, finds the scene keys of
// the sky, light and fog objects by the name prefixes, the 16 levels (+160 keys, +224 / +232 doubles), and
// makes the cloud (+500) and rain (+504) layers
BYTE* __fastcall WeatherObj_Construct(BYTE* self, void*, BYTE* view, const char* a, const char* b, const char* c,
                                   int unused, const char* d)
{
    CallThis(FUN_5d31e0, self, 100, (const char*)s_wo67eaa0);
    F(int, self, 556) = 0;
    F(int, self, 560) = 0;
    F(int, self, 564) = 0;
    F(void*, self, 0) = g_woVtbl651628;
    g_wo7025b8 = self;
    Weather_LoadSettings(s_wo67ea90);
    F(BYTE*, self, 492) = CallC<BYTE*>(FUN_5ef3d0, (const char*)s_wo67ea84);
    F(BYTE*, self, 80) = view;
    F(int, self, 132) = 0;
    F(int, self, 136) = 0;
    F(BYTE*, self, 148) = Wo_Find(a, s_wo67ea80);
    F(BYTE*, self, 512) = Wo_Find(a, s_wo679a18);
    F(BYTE*, self, 140) = Wo_Find(a, s_wo671488);
    F(BYTE*, self, 156) = Wo_Find(b, s_wo67ea80);
    F(BYTE*, self, 516) = Wo_Find(b, s_wo679a18);
    F(BYTE*, self, 152) = Wo_Find(c, s_wo67ea80);
    {
        char buf[32];
        strcpy(buf, c);
        strcpy(buf + 4, s_wo67ea78);
        F(BYTE*, self, 508) = CallC<BYTE*>(FUN_5ef3d0, buf);
    }
    F(BYTE*, self, 480) = Wo_Find(d, s_wo677738);
    F(BYTE*, self, 484) = Wo_Find(d, s_wo679a18);
    F(BYTE*, self, 488) = Wo_Find(d, s_wo679a10);
    F(BYTE*, self, 144) = Wo_Find(d, s_wo671488);
    const double* level = (const double*)(char*)g_woLevels67e848;
    for (int i = 0; i < 16; i++) {
        char buf[32];
        sprintf(buf, s_wo67ea64, i);
        F(BYTE*, self, 160 + i * 4) = CallC<BYTE*>(FUN_5ef2b0, 0, buf);
        F(double, self, 224 + i * 16) = level[i] + g_wo651650;
        if (i < 8)
            F(double, self, 232 + i * 16) = level[i] + g_wo651648;
        else
            F(double, self, 232 + i * 16) = -1.0;
    }
    F(int, self, 104) = 0;
    F(int, self, 88) = 0;
    F(int, self, 96) = 0;
    F(int, self, 112) = 0;
    F(int, self, 108) = 0;
    F(int, self, 92) = 0;
    F(DWORD, self, 100) = 1077411840;      // +96: 24.0
    F(DWORD, self, 116) = 1072693248;      // +112: 1.0
    CallThis(FUN_5d3570, F(BYTE*, self, 80) + 2992, self);
    CallThis(FUN_5d33d0, self, 1);
    BYTE* o = (BYTE*)operator new(128);
    if (o)
        o = CallThis<BYTE*>(FUN_5a6890, o, F(BYTE*, self, 80), Wo_Double(1293080650, 3219726843u),
                            Wo_Double(1893457979, 1070865199), a, 8, 215, 8);
    F(BYTE*, self, 500) = o;
    CallVirt(o, 24, 1);
    o = (BYTE*)operator new(80);
    if (o)
        o = WeatherLayer_Construct(o, 0, F(BYTE*, self, 80), s_wo67ea4c, s_wo67ea54, 0, 1076101120, 858993459,
                                   1070805811);
    F(BYTE*, self, 504) = o;
    CallVirt(o, 24, 1);
    F(DWORD, F(BYTE*, self, 504), 72) |= 1;
    F(DWORD, self, 120) = 1293080650;
    F(int, self, 496) = 0;
    F(DWORD, self, 528) = 1065353216;
    F(DWORD, self, 524) = 1065353216;
    F(DWORD, self, 520) = 1065353216;
    F(int, self, 532) = 0;
    F(int, self, 536) = 0;
    F(int, self, 568) = 0;
    F(int, self, 572) = 0;
    F(DWORD, self, 540) = 1065353216;
    F(DWORD, self, 544) = 1056964608;
    F(DWORD, self, 124) = 3220775419u;
    F(int, self, 576) = 0;
    F(DWORD, self, 548) = 1065353216;
    F(DWORD, self, 552) = 1065353216;
    DWORD* g = (DWORD*)(char*)g_wo6fb3d0;
    g[3] = 0;
    g[2] = 0;
    g[1] = 0;
    g[0] = 0;
    g[4] = 0;
    g[5] = 1072693248;
    g[6] = 0;
    g[7] = 1072693248;
    return self;
}

} // extern "C"

// === the renderer's base constructor (FUN_5acd40)

ASM_VAR(AsmLabel, g_rb651718, $L_651718)
ASM_VAR(int, g_rb67ef88, $L_67ef88)
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_5ad4c0(); KING_ALIAS("_FUN_5ad4c0", "@Light_Init@8")

// matrix m: its rows constructed, then the identity
static void Rb_Matrix(BYTE* m)
{
    CallStd(FUN_4011e0, m, 12, 4, FUN_4bd4d0);
    memcpy(m, (BYTE*)g_wea7025f0, 48);
}

static void Rb_Zero(BYTE* s, int from, int bytes) { memset(s + from, 0, bytes); }

extern "C" {

// [FUN_5acd40] constructor of a renderer on surface a (thiscall): the matrix stacks (+2288, 40; +4304 and
// +5264, 20 each), the camera matrices, 20 lights (+6224) and one more (+7824), the clip planes from the
// surface size, the fog, depth and scale defaults; +8620 whether the surface or its parent (+96) has flag
// 0x1000
BYTE* __fastcall Render_Construct(BYTE* s, void*, BYTE* a)
{
    Rb_Zero(s, 1224, 264);
    for (int i = 0; i < 40; i++)
        Rb_Matrix(s + 2288 + i * 48);
    for (int i = 0; i < 4; i++)
        CallThis(FUN_4bd4d0, s + 4208 + i * 12);
    memcpy(s + 4208, (BYTE*)g_wea7025f0, 48);
    for (int i = 0; i < 4; i++)
        CallThis(FUN_4bd4d0, s + 4256 + i * 12);
    memcpy(s + 4256, (BYTE*)g_wea7025f0, 48);
    for (int i = 0; i < 20; i++)
        Rb_Matrix(s + 4304 + i * 48);
    for (int i = 0; i < 20; i++)
        Rb_Matrix(s + 5264 + i * 48);
    for (int i = 0; i < 20; i++)
        CallThis(FUN_5ad4c0, s + 6224 + i * 80);
    CallThis(FUN_5ad4c0, s + 7824);
    Rb_Zero(s, 7904, 320);
    for (int i = 0; i < 4; i++)
        CallThis(FUN_4bd4d0, s + 8564 + i * 12);
    memcpy(s + 8564, (BYTE*)g_wea7025f0, 48);
    F(void*, s, 0) = g_rb651718;
    F(BYTE*, s, 1948) = a;
    F(int, s, 1952) = 0;
    F(int, s, 1956) = 0;
    F(int, s, 1960) = (F(WORD, a, 0) << 19) - 524288;
    F(int, s, 1964) = (F(WORD, a, 2) << 19) - 524288;
    static const int zeros[] = {8224, 8240, 8228, 8232, 8236, 8244, 8464, 8520, 8452, 8472, 8528, 8488, 8252, 8256,
                                8260, 8264, 8624, 8456, 8532, 8492, 1940, 1944, 1936, 8460, 8276};
    for (int k : zeros)
        F(int, s, k) = 0;
    F(BYTE, s, 8440) = 255;
    F(int, s, 8444) = g_rb67ef88;
    F(DWORD, s, 8468) = 0x3ff00000;
    F(DWORD, s, 8524) = 0x3ff00000;
    F(DWORD, s, 8480) = 2576980378u;
    F(DWORD, s, 8512) = 858993459;
    F(DWORD, s, 8476) = 1090021888;
    F(DWORD, s, 8484) = 1069128089;
    F(DWORD, s, 8516) = 1070805811;
    F(DWORD, s, 8540) = 1148846080;
    F(DWORD, s, 8536) = 1148846080;
    BYTE* parent = F(BYTE*, a, 96);
    F(int, s, 8620) = (F(DWORD, a, 12) & 0x1000) || (parent && (F(DWORD, parent, 12) & 0x1000));
    Rb_Zero(s, 8280, 144);
    F(int, s, 8432) = 0;
    F(int, s, 8640) = 0;
    F(int, s, 8648) = 0;
    F(int, s, 8428) = 0;
    F(int, s, 8436) = 0;
    F(int, s, 8632) = 0;
    F(DWORD, s, 8644) = 3220176896u;
    F(DWORD, s, 8652) = 3220176896u;
    F(int, s, 8660) = -1;
    F(int, s, 8656) = 0;
    F(int, s, 8664) = 0;
    F(int, s, 8668) = 0;
    memcpy(s + 8564, (BYTE*)g_wea7025f0, 48);
    F(int, s, 8628) = 0;
    return s;
}

} // extern "C"

// === spawning lightning ($L_5a5400)

extern "C" void FUN_5a50f0(); KING_ALIAS("_FUN_5a50f0", "@Lightning_Construct@44")    // a lightning flash: constructor (world, scene, x, y, z, height, a, b)
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
ASM_VAR(float, g_lt64c32c, $L_64c32c)
ASM_VAR(float, g_lt64d0c8, $L_64d0c8)
ASM_VAR(double, g_lt64b670, $L_64b670)
extern "C" int __cdecl rand();

extern "C" {

// [$L_5a5400] lightning source s of world w (+32): every +56 seconds (unless off, +72 bit 0) a flash (+64, +68)
// at a random side offset in front of the camera, 20 high, in the world's scene (+528) or room; +72 bit 1 marks
// the second after it
void __fastcall Lightning_Update(BYTE* s)
{
    BYTE* w = F(BYTE*, s, 32);
    if (F(double, s, 56) + F(double, s, 48) < F(double, w, 2968) && !(F(BYTE, s, 72) & 1)) {
        F(double, s, 48) = F(double, w, 2968);
        BYTE* scene = F(BYTE*, w, 528);
        float x = (float)(rand() % 600 - 300);
        BYTE t[48];
        const float* m = CallThis<const float*>(FUN_5dbcc0, F(BYTE*, s, 32), t);
        float px = (float)((double)x * m[0] + (double)m[6] * g_lt64c32c + (double)m[3] * g_lt64d0c8 + m[9]);
        float py = (float)((double)x * m[1] + (double)m[7] * g_lt64c32c + (double)m[4] * g_lt64d0c8 + m[10]);
        if (!scene) {
            BYTE* r = F(BYTE*, F(BYTE*, s, 32), 488);
            scene = r ? F(BYTE*, r, 76) : 0;
        }
        BYTE* o = (BYTE*)operator new(192);
        if (o)
            o = CallThis<BYTE*>(FUN_5a50f0, o, F(BYTE*, s, 32), scene, px, py, 20.0f, 20.0, F(int, s, 40),
                                F(int, s, 36));
        F(int, o, 184) = F(int, s, 64);
        F(int, o, 188) = F(int, s, 68);
        CallVirt(o, 24, 1);
        F(DWORD, s, 72) |= 2;
    }
    if (F(double, s, 48) + g_lt64b670 < F(double, F(BYTE*, s, 32), 2968))
        F(DWORD, s, 72) &= ~2u;
    CallThis(FUN_5d32f0, s, 1);
}

} // extern "C"

// === the sky colour of the weather mix (FUN_5a8e90)

KING_BSS(double, g_mx6fb3e0, 0x6fb3e0)
ASM_VAR(double, g_mx64b5a0, $L_64b5a0)
ASM_VAR(double, g_mx64da58, $L_64da58)

extern "C" {

// [FUN_5a8e90] the sky colour (Sky_Color of x, y, t, mode) of the weather mix: the four weathers 6fb3d0..6fb3dc
// (96-byte entries of table) blended by u (6fb3e0, d0 to d8) and v (6fb3e8, d0 to d4), each taken whole near 0 or 1
void __cdecl Sky_MixColor(float x, float y, float t, float* r, float* g, float* b, BYTE* table, int mode)
{
    const int idx[4] = {(int)g_wea6fb3d0, (int)g_wea6fb3d4, (int)g_wea6fb3d8, (int)g_wea6fb3dc};
    double u = g_mx6fb3e0, v = g_6fb3e8;
    int ku = u < g_mx64b5a0 ? 0 : u < g_mx64da58 ? 1 : 2, kv = v < g_mx64b5a0 ? 0 : v < g_mx64da58 ? 1 : 2;
    float c[3], c2[3], c3[3];
    // row: d0/d8 (v low) or d4/dc (v high), blended by u into out
    struct Row {
        static void Get(float x, float y, float t, const int* idx, int lo, int hi, int ku, float u, float* out,
                        float* tmp, BYTE* table, int mode)
        {
            if (ku != 2)
                Sky_Color(x, y, t, out, table + idx[lo] * 96, mode);
            if (ku == 1) {
                Sky_Color(x, y, t, tmp, table + idx[hi] * 96, mode);
                Color_Lerp(tmp, out, u, out);
            } else if (ku == 2)
                Sky_Color(x, y, t, out, table + idx[hi] * 96, mode);
        }
    };
    if (kv != 2)
        Row::Get(x, y, t, idx, 0, 2, ku, (float)u, c, c2, table, mode);
    if (kv == 1) {
        Row::Get(x, y, t, idx, 1, 3, ku, (float)u, c2, c3, table, mode);
        Color_Lerp(c2, c, (float)v, c);
    } else if (kv == 2)
        Row::Get(x, y, t, idx, 1, 3, ku, (float)u, c, c2, table, mode);
    *r = c[0];
    *g = c[1];
    *b = c[2];
}

} // extern "C"

// === a value of the weather mix (FUN_5a9240)

ASM_VAR(double, g_mv64b670, $L_64b670)

// (1 - t) * a (as float) + t * b
static double Mv_Lerp(double a, double b, double t)
{
    float af = (float)a;
    return (g_mv64b670 - t) * af + b * t;
}

extern "C" {

// [FUN_5a9240] the value (Blend4 of a, b) of the weather mix: the four weathers' 16-byte entries of table blended
// as in Sky_MixColor
double __cdecl Weather_MixValue(float a, float b, const BYTE* table)
{
    const int idx[4] = {(int)g_wea6fb3d0, (int)g_wea6fb3d4, (int)g_wea6fb3d8, (int)g_wea6fb3dc};
    double u = g_mx6fb3e0, v = g_6fb3e8;
    int ku = u < g_mx64b5a0 ? 0 : u < g_mx64da58 ? 1 : 2, kv = v < g_mx64b5a0 ? 0 : v < g_mx64da58 ? 1 : 2;
#define MV(i) Blend4(a, b, (const float*)(table + idx[i] * 16))
    int lo = kv == 2 ? 1 : 0, hi = kv == 2 ? 3 : 2;    // a row along u (d0 to d8, or d4 to dc)
    if (kv != 1) {
        if (ku != 1)
            return MV(ku == 0 ? lo : hi);
        double x = MV(lo);
        return Mv_Lerp(x, MV(hi), u);
    }
    if (ku == 1) {
        double x0 = MV(0);
        float r0 = (float)Mv_Lerp(x0, MV(2), u);
        double x1 = MV(1);
        double r1 = Mv_Lerp(x1, MV(3), u);
        return r1 * v + (g_mv64b670 - v) * r0;
    }
    lo = ku == 0 ? 0 : 2;                               // along v (d0 to d4, or d8 to dc)
    double x = MV(lo);
    return Mv_Lerp(x, MV(lo + 1), v);
#undef MV
}

} // extern "C"

// === the daylight of the weather (FUN_5a89a0)

extern "C" void Dl_Wave(); KING_ALIAS("_Dl_Wave", "@Wave_Value@16")
extern "C" void Dl_RootSurface(); KING_ALIAS("_Dl_RootSurface", "@Window_RootSurface@8")
extern "C" void Dl_ImagePrepare(); KING_ALIAS("_Dl_ImagePrepare", "@Image_Prepare@16")
ASM_VAR(double, g_dl64b678, $L_64b678)
ASM_VAR(double, g_dl64da00, $L_64da00)
ASM_VAR(double, g_dl651540, $L_651540)
ASM_VAR(double, g_dl651298, $L_651298)
ASM_VAR(double, g_dl64b7a8, $L_64b7a8)
ASM_VAR(double, g_dl651658, $L_651658)
ASM_VAR(float, g_dl64c250, $L_64c250)
ASM_VAR(double, g_dl64b840, $L_64b840)
ASM_VAR(double, g_dl64b5b8, $L_64b5b8)
ASM_VAR(double, g_dl64c958, $L_64c958)
ASM_VAR(float, g_dl64d2f8, $L_64d2f8)
ASM_VAR(double, g_dl64b670, $L_64b670)
ASM_VAR(double, g_dl64b7d8, $L_64b7d8)
ASM_VAR(double, g_dl64eba0, $L_64eba0)
KING_BSS(int, g_dl7208cc, 0x7208cc)
KING_BSS(int, g_dl7208d4, 0x7208d4)
KING_BSS(int, g_dl7025bc, 0x7025bc)
ASM_VAR(AsmLabel, s_dlRoadMat, $L_67bb30)      // "mat_refl_road"

extern "C" {

// [FUN_5a89a0] weather s's daylight (+536, the square root of its wave at the time +104 less 64da00): in the
// world's (+80) every fourth frame, unless it shows a still view (+484 +20), the road reflection materials of each
// room (+468, next +8) follow it and the screen's tint (root surface +36..+38)
void __fastcall Weather_Daylight(BYTE* s)
{
    double t = g_dl64b678;
    if (F(double, s, 104) > g_dl64da00)
        t = F(double, s, 104) < g_dl651540 ? F(double, s, 104) - g_dl64da00 : g_dl651298;
    double v = (CallThis<double>(Dl_Wave, s, -t) - g_dl64b7a8) * g_dl651658;
    F(float, s, 536) = (float)v;
    if (v < g_dl64b678)
        F(float, s, 536) = 0;
    if (F(float, s, 536) > g_dl64c250)
        F(float, s, 536) = 1.0f;
    F(float, s, 536) = (float)x87_sqrt(F(float, s, 536));
    BYTE* w = F(BYTE*, s, 80);
    if (F(int, w, 2992) % 4 != 1)
        return;
    if (F(BYTE*, w, 484) && F(int, F(BYTE*, w, 484), 20))
        return;
    for (BYTE* r = F(BYTE*, w, 468); r; r = F(BYTE*, r, 8)) {
        for (int c = 0; c < 6; c++)
            if (int id = F(int, r, 156 + c * 4))
                F(int, F(BYTE*, r, 76), id * 124 + 40) = 4;
        CallC(FUN_5ef3d0, (const char*)s_dlRoadMat);
        for (int j = 0; j < F(int, r, 72); j++) {
            BYTE* m = F(BYTE*, r, 76) + j * 124;
            if (strcmp((const char*)m, (const char*)s_dlRoadMat) || !m || !F(BYTE*, m, 76))
                continue;
            if (g_dl7208cc && g_dl7208d4) {
                F(float, m, 104) = (float)(F(float, s, 536) * g_dl64b840);
                continue;
            }
            float d = F(float, s, 536);
            if ((d > g_dl64b5b8 && d < g_dl64c958) || g_dl7025bc < 30)
                CallThis(Dl_ImagePrepare, F(BYTE*, m, 76), -(double)F(float, s, 536));
        }
    }
    if (BYTE* surf = CallThis<BYTE*>(Dl_RootSurface, F(BYTE*, F(BYTE*, s, 80), 12))) {
        double g = F(float, s, 536) * (double)g_dl64d2f8;
        if (g > g_dl64b670)
            g = g_dl64b670;
        BYTE b = (BYTE)(int)((g_dl64b670 - g * g_dl64b7d8) * g_dl64eba0);
        F(BYTE, surf, 36) = b;
        F(BYTE, surf, 37) = b;
        F(BYTE, surf, 38) = b;
    }
}

} // extern "C"

// === an animated sprite sheet (FUN_5a4ca0)

ASM_VAR(double, g_ss2_64b678, $L_64b678)
extern "C" void Sa_PostEvent(); KING_ALIAS("_Sa_PostEvent", "@Queue_PostEvent@12")

extern "C" {

// [FUN_5a4ca0] step of sheet animation s: its frame (+120: the time +144 over the frame time +128, of +108 frames,
// held at the last with +172 bit 0, else wrapped) picked from the sheet of +112 columns; its +124 quads (+84, 40
// bytes, u +8, v +12) share the cell (+152 x +160) side by side; it ends after +136 seconds when that is set
int __fastcall SheetAnim_Step(BYTE* s)
{
    if (!F(int, s, 16)) {
        F(BYTE, s, 72) &= ~1;
        F(double, s, 144) = 0;
    }
    int f = (int)(F(double, s, 144) / F(double, s, 128));
    if (F(BYTE, s, 172) & 1)
        F(int, s, 120) = f < F(int, s, 108) ? f : F(int, s, 108) - 1;
    else
        F(int, s, 120) = f % F(int, s, 108);
    f = F(int, s, 120);
    int row = f / F(int, s, 112), col = f % F(int, s, 112);
    double w = F(double, s, 152), h = F(double, s, 160);
    for (int i = 0; i < F(int, s, 124); i++) {
        int n = F(int, s, 124);
        BYTE* q = F(BYTE*, s, 84) + i * 40;
        float* u = F(float*, q, 8);
        u[0] = (float)(i * w / n + col * w);
        u[1] = (float)((i + 1) * w / F(int, s, 124) + col * w);
        u[3] = (float)(i * w / F(int, s, 124) + col * w);
        u[2] = (float)((i + 1) * w / F(int, s, 124) + col * w);
        float* v = F(float*, q, 12);
        v[0] = (float)((row + 1) * h);
        v[1] = (float)((row + 1) * h);
        v[2] = (float)(row * h);
        v[3] = (float)(row * h);
    }
    if (F(double, s, 136) > g_ss2_64b678 && F(double, s, 144) > F(double, s, 136))
        return CallVirt<int>(s, 28, 1);
    F(double, s, 144) = F(double, F(BYTE*, s, 100), 2960) + F(double, s, 144);
    return CallThis<int>(Sa_PostEvent, s, 1);
}

} // extern "C"

// === a cloud layer following the camera (a method at $L_5a4370)

extern "C" void Cl_SetMatrix(); KING_ALIAS("_Cl_SetMatrix", "@Node_SetMatrix@12")
extern "C" void Cl_Transform2(); KING_ALIAS("_Cl_Transform2", "@Matrix_Transform2@12")
extern "C" void Cl_Translation(); KING_ALIAS("_Cl_Translation", "_Matrix_Translation")
ASM_VAR(double, g_cl64b5b8, $L_64b5b8)
ASM_VAR(float, g_cl64c304, $L_64c304)
struct Cl_V3 { float x, y, z; };

extern "C" {

// [$L_5a4370] places layer s along direction d (+64 of o) from frame a: frame b shifted by its distance from a
// along d (or by 64c304 times s's height +40 when that is small), then set as s's matrix
void __fastcall Layer_Follow(BYTE* s, void*, const float* a, float* b, const BYTE* o)
{
    const float* d = (const float*)(o + 64);
    double dist = ((double)b[11] - a[11]) * d[2] + ((double)b[10] - a[10]) * d[1] + ((double)b[9] - a[9]) * d[0];
    Cl_V3 v;
    if (x87_fabs(dist) > g_cl64b5b8) {
        double t = x87_fabs(dist);
        v.x = (float)(t * d[0]);
        v.y = (float)(t * d[1]);
        v.z = (float)(t * d[2]);
    } else {
        float h = F(float, s, 40);
        double x = d[0] * (double)g_cl64c304;
        float y = d[1] * g_cl64c304, z = d[2] * g_cl64c304;
        v.x = (float)(x * h);
        v.y = y * h;
        v.z = z * h;
    }
    float m[12];
    CallThis(Cl_Transform2, b, CallC<float*>(Cl_Translation, m, v));
    CallThis(Cl_SetMatrix, s, b);
}

} // extern "C"

// === a lightning flash's constructor (FUN_5a50f0)

extern "C" void FUN_5a4880(); KING_ALIAS("_FUN_5a4880", "@SkyEffect_Construct@68")
extern "C" void Lf_Find(); KING_ALIAS("_Lf_Find", "_Objects_Find")
ASM_VAR(AsmLabel, g_lf6514b4, $L_6514b4)
ASM_VAR(AsmLabel, g_lf651468, $L_651468)
ASM_VAR(double, g_lf64b7c0, $L_64b7c0)
struct Lf_V3 { DWORD x, y, z; };

extern "C" {

// [FUN_5a50f0] constructor: the sky effect base (FUN_5a4880: a, b, position p, size k and k * 64b7c0, c, 7, 8, 1,
// 0.1, 0.7, 100, 1) with this kind's vtables; the material named name in the scene's list (+100 -> +468: +72
// count, +76 the 124-byte records) gets flag 0x100; +176 the object sound, +180 0 and +184 0.8
BYTE* __fastcall Lightning_Construct(BYTE* s, void*, int a, int b, Lf_V3 p, double k, const char* name, const char* sound)
{
    float k1 = (float)k, k2 = (float)(k * g_lf64b7c0);
    CallThis(FUN_5a4880, s, a, b, p, *(DWORD*)&k1, *(DWORD*)&k2, name, 7, 8, 1, 0x3DCCCCCDu, 0x3F333333u, 100, 1);
    F(void*, s, 0) = g_lf6514b4;
    F(void*, s, 32) = g_lf651468;
    BYTE* t = F(BYTE*, F(BYTE*, s, 100), 468);
    int n = F(int, t, 72), i = 0;
    for (BYTE* r = F(BYTE*, t, 76); i < n; i++, r += 124)
        if (!strcmp(name, (const char*)r))
            break;
    if (i < n)
        F(DWORD, F(BYTE*, t, 76) + i * 124, 32) |= 0x100;
    F(int, s, 176) = CallC<int>(Lf_Find, sound);
    F(int, s, 180) = 0;
    F(DWORD, s, 184) = 0x9999999A;
    F(DWORD, s, 188) = 0x3FE99999;
    return s;
}

} // extern "C"

// === a sky effect's base constructor (FUN_5a4880)

extern "C" void Sb_MeshConstruct(); KING_ALIAS("_Sb_MeshConstruct", "@Mesh_Construct@8")
extern "C" void Sb_Set48(); KING_ALIAS("_Sb_Set48", "@Node_Set48@16")
extern "C" void Sb_MoveTo(); KING_ALIAS("_Sb_MoveTo", "@Chain_MoveTo@8")
extern "C" void Sb_VecCtor(); KING_ALIAS("_Sb_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void Sb_RecInit(); KING_ALIAS("_Sb_RecInit", "@FxRec_Init@4")
extern "C" void Sb_RecFree(); KING_ALIAS("_Sb_RecFree", "@FxRec_Free@4")
ASM_VAR(char, s_sb67e938, $L_67e938)
ASM_VAR(double, g_sb64b670, $L_64b670)
ASM_VAR(double, g_sb64b668, $L_64b668)
ASM_VAR(double, g_sb651388, $L_651388)
ASM_VAR(float, g_sb64c258, $L_64c258)

static float* Sb_Quad(float a, float b, float c, float d)
{
    float* q = (float*)operator new(16);
    q[0] = a, q[1] = b, q[2] = c, q[3] = d;
    return q;
}

extern "C" {

// [FUN_5a4880] constructor of a sky effect (kind k, owner o at +100, parent p +104): a mesh (+32) ring of n
// 40-byte strips of width w around position pos (+88) and height h (scaled by 64b668 when n > 1), textured by
// u steps 1/su (+152) and v 1/sv (+160) with the model of name; +108 a, +112 su, +116 sv, +124 n, +128/+136 f1/f2;
// attached under p (vtbl+20 when p is the owner's +524, else vtbl+28) and to the owner (+2992)
BYTE* __fastcall SkyEffect_Construct(BYTE* s, void*, BYTE* o, BYTE* p, Lf_V3 pos, float w, float h,
    const char* name, int a, int su, int sv, float f1, float f2, int k, int n)
{
    CallThis(FUN_5d31e0, s, k, &s_sb67e938);
    BYTE* m = s + 32;
    CallThis(Sb_MeshConstruct, m);
    F(double, s, 128) = f1;
    F(double, s, 136) = f2;
    F(void*, m, 0) = g_wea6513b0;
    F(BYTE*, s, 100) = o;
    F(BYTE*, s, 104) = p;
    F(void*, s, 0) = g_wea651390;
    F(int, s, 172) = 0;
    F(int, s, 108) = a;
    F(int, s, 112) = su;
    F(int, s, 116) = sv;
    F(int, s, 124) = n;
    F(double, s, 152) = g_sb64b670 / su;
    F(double, s, 160) = g_sb64b670 / sv;
    if (n > 1)
        h = (float)(h * g_sb64b668);
    float half = w * g_sb64c258;
    float step = (float)(g_q176_64d900 / n);
    BYTE* items = 0;
    if (int* raw = (int*)operator new(n * 40 + 4)) {
        raw[0] = n;
        items = (BYTE*)(raw + 1);
        CallStd(Sb_VecCtor, items, 40, n, Sb_RecInit, Sb_RecFree);
    }
    memcpy(s + 88, &pos, 12);
    F(int, s, 168) = CallC<int>(FUN_5ef3d0, name);
    for (int i = 0; i < n; i++) {
        BYTE* it = items + i * 40;
        F(WORD, it, 0) = 2;
        F(WORD, it, 4) = 4;
        double i1 = i + 1;
        float* p8 = (float*)operator new(16);
        F(float*, it, 8) = p8;
        p8[0] = (float)(i * (F(double, s, 152) / F(int, s, 124)));
        p8[1] = (float)(i1 * (F(double, s, 152) / F(int, s, 124)));
        p8[3] = (float)(i * (F(double, s, 152) / F(int, s, 124)));
        p8[2] = (float)(F(double, s, 152) / F(int, s, 124) * i1);
        F(float*, it, 12) = Sb_Quad((float)F(double, s, 160), (float)F(double, s, 160), 0, 0);
        float* p16 = (float*)operator new(16);
        F(float*, it, 16) = p16;
        float a0 = i * step;
        float a1;
        if (F(int, s, 124) > 1) {
            double nh = -half;
            p16[0] = (float)(x87_cos(a0) * nh);
            a1 = (float)((i + 1) * (double)step);
            p16[1] = (float)(x87_cos(a1) * nh);
            p16[2] = (float)(p16[1] * g_sb64b668);
            p16[3] = (float)(p16[0] * g_sb64b668);
            float* p24 = (float*)operator new(16);
            F(float*, it, 24) = p24;
            p24[0] = (float)(x87_sin(a0) * half * g_sb651388);
            p24[1] = (float)(x87_sin(a1) * half * g_sb651388);
            p24[2] = (float)(p24[1] * g_sb64b668);
            p24[3] = (float)(p24[0] * g_sb64b668);
        } else {
            float nh = -half;
            p16[0] = (float)(x87_cos(a0) * nh);
            a1 = (float)((i + 1) * (double)step);
            p16[1] = (float)(x87_cos(a1) * nh);
            p16[2] = p16[1];
            p16[3] = p16[0];
            float* p24 = (float*)operator new(16);
            F(float*, it, 24) = p24;
            p24[0] = (float)(x87_sin(a0) * nh);
            p24[1] = (float)(x87_sin(a1) * nh);
            p24[2] = p24[1];
            p24[3] = p24[0];
        }
        float* p20 = (float*)operator new(16);
        F(float*, it, 20) = p20;
        p20[0] = 0;
        p20[1] = 0;
        p20[2] = h;
        p20[3] = h;
        F(WORD, it, 34) = 4095;
        F(int, it, 36) = F(int, s, 168);
    }
    CallThis(Sb_Set48, m, items, F(int, s, 124));
    CallThis(Sb_MoveTo, m);
    BYTE* par = F(BYTE*, s, 104);
    if (par == F(BYTE*, o, 524))
        CallVirt(par, 20, 0, s ? m : 0);
    else
        CallVirt(par, 28, s ? m : 0);
    CallThis(FUN_5d3570, F(BYTE*, s, 100) + 2992, s);
    F(BYTE, s, 72) |= 1;
    F(int, s, 144) = 0;
    F(int, s, 148) = 0;
    F(int, s, 120) = 0;
    return s;
}

} // extern "C"

// === applying the weather mix to the sky (FUN_5a9540)

extern "C" void Wa_Select(); KING_ALIAS("_Wa_Select", "_Switch_Select")
KING_BSS(int, g_wa7025c0, 0x7025c0)                 // no thunder
ASM_VAR(double, g_wa64d950, $L_64d950)
ASM_VAR(double, g_wa64b898, $L_64b898)
ASM_VAR(double, g_wa651690, $L_651690)
ASM_VAR(double, g_wa651688, $L_651688)
ASM_VAR(double, g_wa64b6d0, $L_64b6d0)
ASM_VAR(double, g_wa64f790, $L_64f790)
ASM_VAR(double, g_wa64b670, $L_64b670)
ASM_VAR(double, g_wa64b5a0, $L_64b5a0)
ASM_VAR(double, g_wa64b840, $L_64b840)
ASM_VAR(double, g_wa651680, $L_651680)
ASM_VAR(double, g_wa651678, $L_651678)
ASM_VAR(double, g_wa64d0f0, $L_64d0f0)
ASM_VAR(double, g_wa651670, $L_651670)
ASM_VAR(double, g_wa651668, $L_651668)
ASM_VAR(double, g_wa651660, $L_651660)
ASM_VAR(float, g_wa64c458, $L_64c458)
ASM_VAR(float, g_wa64c250, $L_64c250)
KING_BSS(AsmLabel, g_wa7022a0, 0x7022a0)
KING_BSS(AsmLabel, g_wa6fa450, 0x6fa450)
KING_BSS(AsmLabel, g_wa6fc9b8, 0x6fc9b8)
KING_BSS(AsmLabel, g_wa6fca58, 0x6fca58)
KING_BSS(AsmLabel, g_wa6fd75c, 0x6fd75c)
KING_BSS(AsmLabel, g_wa6fb3f4, 0x6fb3f4)
KING_BSS(AsmLabel, g_wa701fa0, 0x701fa0)
KING_BSS(AsmLabel, g_wa6f9cc0, 0x6f9cc0)
KING_BSS(AsmLabel, g_wa701ca0, 0x701ca0)
KING_BSS(AsmLabel, g_wa6fa150, 0x6fa150)

// light l (+96 strength, +108 colour, +136 bright) set from the mix of table
static void Wa_Light(BYTE* s, BYTE* l, BYTE* table, int mode, bool bright)
{
    float c[3];
    Sky_MixColor((float)F(double, s, 32), (float)F(double, s, 72), (float)F(double, s, 64), &c[0], &c[1], &c[2], table, mode);
    F(float, l, 96) = 1.0f;
    F(float, l, 108) = c[0];
    F(float, l, 112) = c[1];
    F(float, l, 116) = c[2];
    if (bright)
        F(int, l, 136) = (double)c[0] + c[1] + c[2] < g_wa64b5a0 ? 0 : 1;
}

extern "C" {

// [FUN_5a9540] applies weather s's mix (time +32, +64, +72, sun +48, clouds +56, hour +88) to the sky: the
// thunder and rain switches, the colours of the sky (+520), the lights (+480 .. +516), the sun's direction (+556),
// the fog from the brightness (+548) and the 16 hour switches (+160, ranges at +224)
void __fastcall Weather_ApplySky(BYTE* s, void*)
{
    BYTE* W = F(BYTE*, F(BYTE*, s, 80), 472);
    int idx = g_6fb3e8 > g_wa64d950 ? 1 : 0;
    int fl = (&g_weatherFlags6fca38)[(&g_wea6fb3d8)[idx]];
    if (!(fl & 1) && F(BYTE*, s, 496))
        F(DWORD, F(BYTE*, s, 496), 164) &= ~0x12u;
    BYTE* o = F(BYTE*, s, 504);
    if (!(F(double, s, 56) < g_wa64b898) && (fl & 4))
        F(DWORD, o, 72) &= ~1u;
    else
        F(DWORD, o, 72) |= 1;
    g_wa7025c0 = (((int)(signed char)~fl) & 4) >> 2;
    if (F(double, s, 72) < g_wa651690)
    {
        g_wea6fb3dc = (g_wea6fb3d4 + 1) % 4;
        g_mx6fb3e0 = F(double, s, 72) * g_wa651688;
    }
    else
    {
        g_mx6fb3e0 = 1.0;
        g_wea6fb3d4 = g_wea6fb3dc;
    }
    float r = 0, g = 0, b = 0;
    float x = (float)F(double, s, 32), y = (float)F(double, s, 72), t = (float)F(double, s, 64);
    Sky_MixColor(x, y, t, &r, &g, &b, (BYTE*)g_wa7022a0, 0);
    Sky_MixColor(x, y, t, &F(float, s, 520), &F(float, s, 524), &F(float, s, 528), (BYTE*)g_wa6fa450, 0);
    if ((F(BYTE, F(BYTE*, s, 504), 72) & 2) && !((int)(F(double, F(BYTE*, s, 80), 2968) * g_wa64b6d0) % 2))
    {
        F(float, s, 520) = r = 0.9f;
        F(float, s, 524) = g = 1.0f;
        F(float, s, 528) = b = 1.0f;
    }
    F(double, W, 48) = F(float, s, 520);
    F(double, W, 56) = F(float, s, 524);
    F(int, W, 72) = 8;
    F(double, W, 64) = F(float, s, 528);
    F(double, W, 16) = Weather_MixValue(y, t, (BYTE*)g_wa6fc9b8);
    F(double, W, 24) = Weather_MixValue(y, t, (BYTE*)g_wa6fca58);
    F(double, W, 32) = Weather_MixValue(y, t, (BYTE*)g_wa6fd75c) * F(double, W, 16);
    BYTE* L = F(BYTE*, s, 480);
    if (L)
    {
        F(double, L, 72) = r;
        F(double, L, 80) = g;
        F(double, L, 88) = b;
        F(double, L, 56) = F(double, W, 24) * g_wa64f790;
        F(double, L, 64) = F(double, W, 16) * g_wa64f790;
        F(int, L, 96) = 8;
        double k = F(double, s, 64) + F(double, s, 64);
        if (k > g_wa64b670)
            k = g_wa64b670;
        float c[3];
        Sky_MixColor(x, y, (float)k, &c[0], &c[1], &c[2], (BYTE*)g_wa6fb3f4, 0);
        int i = F(int, F(BYTE*, s, 480), 96);
        BYTE** lst = F(BYTE**, F(BYTE*, F(BYTE*, s, 80), 468), 100);
        if (i >= 0 && lst && *lst)
        {
            (*lst)[24 + i * 3] = (BYTE)(int)((double)c[0] * g_wa64c458);
            (*lst)[25 + i * 3] = (BYTE)(int)((double)c[1] * g_wa64c458);
            (*lst)[26 + i * 3] = (BYTE)(int)((double)c[2] * g_wa64c458);
        }
    }
    if (F(BYTE*, s, 512))
        Wa_Light(s, F(BYTE*, s, 512), (BYTE*)g_wa701fa0, 1, true);
    if (F(BYTE*, s, 484))
        Wa_Light(s, F(BYTE*, s, 484), (BYTE*)g_wa6f9cc0, 0, false);
    if (F(BYTE*, s, 488))
        Wa_Light(s, F(BYTE*, s, 488), (BYTE*)g_wa701ca0, 0, false);
    if (F(BYTE*, s, 516))
        Wa_Light(s, F(BYTE*, s, 516), (BYTE*)g_wa6fa150, 0, true);
    if (BYTE* P = F(BYTE*, s, 512))
    {
        if ((F(DWORD, s, 572) & 64) && (F(DWORD, s, 572) & 0x100))
            F(int, P, 96) = F(int, s, 540);
        else
            F(float, P, 96) = 1.0f;
    }
    BYTE* Q = F(BYTE*, s, 500);
    if (Q)
    {
        if (F(double, s, 32) > g_wa64b840 && F(double, s, 56) < g_wa64b840 && !(s[572] & 128))
            F(DWORD, Q, 120) &= ~128u;
        else
            F(DWORD, Q, 120) |= 128;
        double a44 = F(double, s, 72) * g_wa651680;
        double a36 = F(double, s, 48) * g_wa651678;
        Obj5a6860_Set(Q, 0, ((int*)&a44)[0], ((int*)&a44)[1], ((int*)&a36)[0], ((int*)&a36)[1], 0);
        double c36 = x87_cos(a36);
        F(float, s, 556) = (float)(x87_cos(a44) * c36);
        F(float, s, 560) = (float)(x87_sin(a44) * c36);
        F(float, s, 564) = (float)x87_sin(a36);
    }
    F(DWORD, s, 572) &= 0xfffffe3f;
    if (!(fl & 2))
    {
        if (Q)
            F(DWORD, Q, 120) |= 128;
        F(DWORD, s, 572) |= 64;
    }
    F(int, F(BYTE*, F(BYTE*, s, 80), 472), 76) = F(int, s, 576);
    BYTE* P = F(BYTE*, s, 512);
    BYTE* A = F(BYTE*, s, 516);
    double v = (double)F(float, P, 108) * g_wa64d0f0 + (double)F(float, P, 116) * g_wa651670 + (double)F(float, P, 112) * g_wa651668;
    v += (double)F(float, A, 108) * g_wa64d0f0 + (double)F(float, A, 116) * g_wa651670 + (double)F(float, A, 112) * g_wa651668;
    v += (double)F(float, s, 524) * g_wa651668 + (double)F(float, s, 520) * g_wa64d0f0 + (double)F(float, s, 528) * g_wa651670;
    F(float, s, 548) = (float)v;
    if (v > g_wa64c250)
        F(float, s, 548) = 1.0f;
    double fog = F(float, s, 532);
    Fog_Set(F(float, s, 548), ((int*)&fog)[0], ((int*)&fog)[1]);
    for (int i = 0; i < 16; i++)
    {
        BYTE* sw = F(BYTE*, s, 160 + i * 4);
        if (!sw)
            continue;
        const double* d = (const double*)(s + 224 + i * 16);
        double h = F(double, s, 88);
        int on;
        if (i < 8)
        {
            if (h > d[0] && h < d[1])
                on = 1;
            else if (!(d[0] - g_wa651660 < h))
                on = 0;
            else
                on = d[1] - g_wa651660 > h ? 1 : 0;
        }
        else
        {
            if (h > d[0])
                on = 1;
            else
                on = d[0] - g_wa651660 > h ? 1 : 0;
        }
        CallC(Wa_Select, sw, on);
    }
}

} // extern "C"

// === the sun's flare (a method at $L_5a6ad0)

extern "C" void Sf_PostEvent(); KING_ALIAS("_Sf_PostEvent", "@Queue_PostEvent@12")
extern "C" void Sf_GetWorld(); KING_ALIAS("_Sf_GetWorld", "@Node_GetWorld@12")
extern "C" void Sf_TransformDir(); KING_ALIAS("_Sf_TransformDir", "@Vec3_TransformDir@12")
extern "C" void Sf_MulAssign(); KING_ALIAS("_Sf_MulAssign", "@Vec3_MulAssign@16")
extern "C" void Sf_FrameApply(); KING_ALIAS("_Sf_FrameApply", "_Frame_Apply")
ASM_VAR(double, g_sf64d900, $L_64d900)
ASM_VAR(double, g_sf64d908, $L_64d908)
ASM_VAR(double, g_sf64d910, $L_64d910)
ASM_VAR(double, g_sf64b660, $L_64b660)
ASM_VAR(double, g_sf64b738, $L_64b738)
ASM_VAR(double, g_sf64b670, $L_64b670)
ASM_VAR(double, g_sf64da00, $L_64da00)
ASM_VAR(double, g_sf64b5b8, $L_64b5b8)
ASM_VAR(float, g_sf64c258, $L_64c258)
ASM_VAR(double, g_sf64b780, $L_64b780)
ASM_VAR(double, g_sf651588, $L_651588)

static double Sf_Wrap(double a)
{
    while (a > g_sf64d900)
        a -= g_sf64d908;
    while (a < g_sf64d910)
        a += g_sf64d908;
    return a;
}

// the strength v kept at +56, at most 1 and to the 32nd power
static void Sf_Strength(BYTE* s, double v)
{
    F(double, s, 56) = v;
    if (v > g_sf64b670)
        F(double, s, 56) = 1.0;
    double x = F(double, s, 56);
    x = x * x;
    x = x * x;
    x = x * x;
    x = x * x;
    F(double, s, 56) = x * x;
}

static float Sf_WrapF(float a)
{
    if (a < g_sf64d910)
        a = (float)(a + g_sf64d908);
    if (a > g_sf64d900)
        a = (float)(a - g_sf64d908);
    return a;
}

extern "C" {

// [$L_5a6ad0] the sun flare s (direction +40 / +48): hidden with flag 64; else its strength (+56) from how
// straight the camera (s +32) looks at it, its frame +64 at 50 along the direction, the glow +112 shown inside
// the view (+120 bit 128 hides it) and the +104 rings (+68..) along the way, or with flag 4 a blinking ring
void __fastcall Sun_FlareStep(BYTE* s, void*)
{
    BYTE* a = F(BYTE*, s, 108);
    BYTE* b = F(BYTE*, s, 112);
    if (s[120] & 64)
    {
        a[40] |= 1;
        b[40] |= 1;
        CallThis(Sf_PostEvent, s, 1);
        return;
    }
    a[40] &= 0xfe;
    b[40] &= 0xfe;
    BYTE* cam = F(BYTE*, s, 32);
    double pitch = F(double, cam, 2936);
    double d1 = Sf_Wrap(F(double, s, 40) - F(double, cam, 2928));
    double d2 = Sf_Wrap(F(double, s, 48) - pitch);
    double v = (x87_cos(d1) + x87_cos(d2) + g_sf64b660) * g_sf64b738;
    Sf_Strength(s, v);
    BYTE* viewer = F(BYTE*, cam, 488);
    Matrix m292 = g_identity, m196, tmp;
    CallThis(Sf_GetWorld, F(BYTE*, viewer, 80), &m292);
    m292 = *Matrix_Inverse(&m196, &m292);
    const float* R = Matrix_RotationX(&m196, (float)F(double, s, 48))->m;
    const float* Y = &g_axisY.x;
    float px = (float)((double)Y[2] * R[6] + (double)Y[1] * R[3] + (double)Y[0] * R[0] + R[9]);
    float py = (float)((double)Y[2] * R[7] + (double)Y[1] * R[4] + (double)Y[0] * R[1] + R[10]);
    float pz = (float)((double)Y[2] * R[8] + (double)Y[1] * R[5] + (double)Y[0] * R[2] + R[11]);
    Matrix m148;
    const float* Z = Matrix_RotationZ(&m148, (float)F(double, s, 40))->m;
    Vec3 q;
    q.y = (float)((double)pz * Z[7] + (double)py * Z[4] + (double)px * Z[1] + Z[10]);
    q.z = (float)((double)pz * Z[8] + (double)py * Z[5] + (double)px * Z[2] + Z[11]);
    q.x = (float)((double)pz * Z[6] + (double)py * Z[3] + (double)px * Z[0] + Z[9]);
    CallThis(Sf_TransformDir, &q, &m292);
    float az, pol;
    Vec3_ToAngles(q, &az, &pol);
    az = (float)(az - g_sf64da00);
    pol = (float)(g_sf64da00 - pol);
    az = Sf_WrapF(az);
    pol = Sf_WrapF(pol);
    Vec3 t3;
    CallThis(Sf_MulAssign, &q, &t3, 50.0f);
    Matrix tr, rz, rx, m100, m244;
    Matrix_Translation(&tr, q);
    Matrix_RotationZ(&rz, az);
    Matrix_RotationX(&rx, pol);
    m292 = *Matrix_Multiply(&m244, Matrix_Multiply(&m100, &rx, &rz), &tr);
    CallC(Sf_FrameApply, F(BYTE*, s, 64), 0, &m292);
    if (!(s[120] & 4))
    {
        bool hidden = !(q.y >= g_sf64b5b8)
                      || (double)F(float, viewer, 88) * g_sf64c258 < x87_fabs((double)q.x / q.y)
                      || (double)F(float, viewer, 84) * g_sf64c258 < x87_fabs((double)q.z / q.y);
        if (hidden || (s[120] & 128))
            b[40] |= 1;
        else
            b[40] &= 0xfe;
        CallThis(Sf_TransformDir, &q, &m292);
        float nx = (float)-(double)q.x, nz = (float)-(double)q.z;
        q.x = nx;
        q.y = 0;
        q.z = nz;
        int n = F(int, s, 104);
        BYTE** ring = (BYTE**)(s + 68);
        for (int k = 1; k < n + 1; k++)
        {
            double t = k + 1;
            float tx = (float)(t * q.x), ty = (float)(t * q.y);
            double tz = t * q.z;
            double dn = n;
            Vec3 o;
            o.x = (float)(tx / dn);
            o.y = (float)(ty / dn);
            o.z = (float)(tz / dn);
            Matrix mt;
            CallC(Sf_FrameApply, *ring++, 0, Matrix_Translation(&mt, o));
            n = F(int, s, 104);
        }
    }
    else
    {
        CallC(Wa_Select, b, (int)(F(double, cam, 2968) * g_sf64b780) % F(int, s, 104));
        Matrix wm, inv;
        CallThis(Sf_GetWorld, F(BYTE*, s, 64), &wm);
        const float* I = Matrix_Inverse(&inv, &wm)->m;
        const float* Zv = &g_axisZ.x;
        double dx = (double)Zv[2] * I[6] + (double)Zv[1] * I[3] + (double)Zv[0] * I[0];
        float dy = (float)((double)Zv[2] * I[7] + (double)Zv[1] * I[4] + (double)Zv[0] * I[1]);
        float dz = (float)((double)Zv[2] * I[8] + (double)Zv[1] * I[5] + (double)Zv[0] * I[2]);
        float k = (float)(x87_sin(F(double, s, 48)) * g_sf651588);
        Vec3 o;
        o.x = (float)(dx * k);
        o.y = (float)((double)dy * k);
        o.z = (float)((double)dz * k);
        Matrix mt;
        CallC(Sf_FrameApply, F(BYTE*, s, 68), 0, Matrix_Translation(&mt, o));
    }
    Sf_Strength(s, v);
    CallThis(Sf_PostEvent, s, 1);
}

} // extern "C"

// === the weather's frame (a method at $L_5a9dc0)

extern "C" void Wf_Switch(); KING_ALIAS("_Wf_Switch", "_Switch_Select")
extern "C" void Wf_FrameApply(); KING_ALIAS("_Wf_FrameApply", "_Frame_Apply")
extern "C" void Wf_SoundPlay(); KING_ALIAS("_Wf_SoundPlay", "@Sound_Play@20")
KING_BSS(int, g_wf721768, 0x721768)                 // the trace is on
KING_BSS(BYTE*, g_wfTraceData, 0x71e814)
KING_BSS(int, g_wfTraceSize, 0x71e810)
KING_BSS(int, g_wfTracePos, 0x71e818)
ASM_VAR(char, g_wf67ef40, $L_67ef40)
ASM_VAR(char, g_wf67ef2c, $L_67ef2c)
ASM_VAR(char, g_wf67ef1c, $L_67ef1c)
KING_BSS(int, g_wfPhase, 0x7025c4)                  // the demo cycle's step
KING_BSS(int, g_wf7025c0, 0x7025c0)
KING_BSS(double, g_wf6fb3e8, 0x6fb3e8)              // the loudness cap
ASM_VAR(double, g_wf64b660, $L_64b660)
ASM_VAR(double, g_wf64b678, $L_64b678)
ASM_VAR(double, g_wf6516d8, $L_6516d8)
ASM_VAR(double, g_wf6516d0, $L_6516d0)
ASM_VAR(double, g_wf64d908, $L_64d908)
ASM_VAR(double, g_wf64da00, $L_64da00)
ASM_VAR(double, g_wf64b6e0, $L_64b6e0)
ASM_VAR(double, g_wf6516c8, $L_6516c8)
ASM_VAR(double, g_wf6516c0, $L_6516c0)
ASM_VAR(double, g_wf6516b8, $L_6516b8)
ASM_VAR(double, g_wf6516b0, $L_6516b0)
ASM_VAR(double, g_wf64b7a8, $L_64b7a8)
ASM_VAR(double, g_wf6516a8, $L_6516a8)
ASM_VAR(double, g_wf6516a0, $L_6516a0)
ASM_VAR(float, g_wf64caa0, $L_64caa0)
ASM_VAR(float, g_wf64d2f8, $L_64d2f8)
ASM_VAR(double, g_wf64b840, $L_64b840)
ASM_VAR(double, g_wf651698, $L_651698)
ASM_VAR(double, g_wf64b738, $L_64b738)
ASM_VAR(double, g_wf64b5b8, $L_64b5b8)

static void Wf_Trace(const char* p)
{
    if (!g_wf721768 || !g_wfTraceData || !*p)
        return;
    while (g_wfTracePos < g_wfTraceSize)
    {
        g_wfTraceData[g_wfTracePos] = *p++;
        g_wfTracePos++;
        if (!*p)
            break;
    }
}

// the clock +88 (or the sun +104 / the moon +120) moved on by the frame time over the length +96 (+112)
static double Wf_Step(BYTE* s, double full, int len)
{
    return F(double, F(BYTE*, s, 80), 2960) * full / (F(double, s, len) * g_wf6516c8);
}

static void Wf_Wrap(BYTE* s, int off, double full)
{
    while (F(double, s, off) > full)
        F(double, s, off) -= full;
}

extern "C" {

// [$L_5a9dc0] weather s's frame: the day phase +72 / +48 from the clock +88, the darkness +56 / +64, the sky and
// daylight, then the clock and the sun and moon angles (+104 / +120) running (+568: the demo cycle 7025c4 through
// set times of day), the star dome turned (+144), the lamp level +532 by the dark, the night sounds (+492, +508,
// crickets +152)
void __fastcall Weather_Frame(BYTE* s, void*)
{
    double t = F(double, s, 88) - g_wf64b660;
    F(double, s, 72) = t;
    if (t < g_wf64b678)
        F(double, s, 72) = t + g_wf6516d8;
    double x = F(double, s, 72) * g_wf6516d0;
    F(double, s, 72) = x;
    F(double, s, 48) = x87_sin(x * g_wf64d908 - g_wf64da00) + g_wf64b6e0;
    double v = Wave_Value(s, 0, 0.0);
    F(double, s, 56) = v;
    double w = (v - g_wf64b6e0) * 2;
    F(double, s, 64) = w;
    if (w < g_wf64b678)
        F(double, s, 64) = 0;
    F(double, s, 32) = F(double, s, 48);
    Weather_ApplySky(s, 0);
    Wf_Trace(&g_wf67ef40);
    Weather_Daylight(s);
    Wf_Trace(&g_wf67ef2c);
    CallThis(FUN_5e48f0, s);
    if (F(int, s, 568))
    {
        static const double hours[4] = {2.0, 6.0, 16.0, 26.0};
        switch (g_wfPhase)
        {
        case 0:
        case 1:
        case 2:
        {
            static const DWORD sun[3] = {0, 1073291771, 1074340347};      // 0, pi / 2, pi
            F(DWORD, s, 104) = g_wfPhase ? 1293080650 : 0;
            F(DWORD, s, 108) = sun[g_wfPhase];
            F(double, s, 88) = Wf_Step(s, g_wf6516d8, 96) + F(double, s, 88);
            if (F(double, s, 88) > g_wf6516d8)
            {
                g_wfPhase++;
                if (g_wfPhase == 3)
                    F(double, s, 104) = 0;
            }
            break;
        }
        case 3:
        case 4:
        case 5:
        case 6:
            F(double, s, 88) = hours[g_wfPhase - 3];
            F(double, s, 104) = Wf_Step(s, g_wf64d908, 112) + F(double, s, 104);
            if (F(double, s, 104) > g_wf64d908)
            {
                if (g_wfPhase == 6)
                {
                    g_wfPhase = 0;
                    F(double, s, 88) = 0;
                }
                else
                    g_wfPhase++;
            }
            break;
        }
    }
    else if (!(F(DWORD, s, 572) & 0x10000))
    {
        F(double, s, 88) = Wf_Step(s, g_wf6516d8, 96) + F(double, s, 88);
        double d = F(double, s, 112) * g_wf6516c8;
        double dt = F(double, F(BYTE*, s, 80), 2960);
        F(double, s, 104) = dt * g_wf64d908 / d + F(double, s, 104);
        F(double, s, 120) = dt * g_wf64d908 / d * g_wf6516c0 + F(double, s, 120);
    }
    Wf_Wrap(s, 104, g_wf64d908);
    Wf_Wrap(s, 120, g_wf64d908);
    Wf_Wrap(s, 88, g_wf6516d8);
    if (F(int, s, 144))
    {
        Matrix r;
        CallC(Wf_FrameApply, F(int, s, 144), 0, Matrix_RotationY(&r, (float)(F(double, s, 56) * g_wf6516b8 - g_wf6516b0)));
    }
    // the lamps by the dark, flickering every third half second
    double dark = F(double, s, 56);
    int third = (int)(F(double, F(BYTE*, s, 80), 2968) * g_wf64b6e0) % 3 == 0;
    if (dark > g_wf64b7a8)
    {
        F(float, s, 532) = 2.0f;
        if (third)
            F(float, s, 532) = 4.0f;
    }
    else if (dark > g_wf6516a8 || dark > g_wf64b840)
    {
        double lv = dark > g_wf6516a8 ? g_wf64caa0 - (dark - g_wf6516a8) * g_wf6516a0 : (dark - g_wf64b840) * g_wf651698;
        F(float, s, 532) = (float)lv;
        if (third)
            F(float, s, 532) = (float)(lv * g_wf64d2f8);
    }
    else
        F(float, s, 532) = 0;
    Wf_Trace(&g_wf67ef1c);
    // the night's sounds
    double vol = F(double, s, 56) * g_wf64b738;
    vol = vol < g_wf64b5b8 ? g_wf64b678 : (vol - g_wf64b5b8) * 2;
    if (g_wf7025c0 && vol > g_wf6fb3e8)
        vol = g_wf6fb3e8;
    if (BYTE* snd = F(BYTE*, s, 492))
        CallThis(Wf_SoundPlay, snd, (float)vol, (float)vol, 0);
    if (F(double, s, 56) > g_wf64b7a8)
    {
        CallC(Wf_Switch, F(int, s, 152), 1);
        if (BYTE* snd2 = F(BYTE*, s, 508))
        {
            double v2 = (F(double, s, 56) - g_wf64b7a8) * F(float, s, 552);
            if (g_wf7025c0 && v2 > g_wf6fb3e8)
                v2 = g_wf6fb3e8;
            CallThis(Wf_SoundPlay, snd2, (float)v2, (float)v2, 0);
        }
    }
    else if (F(int, s, 152))
        CallC(Wf_Switch, F(int, s, 152), 0);
    CallThis(FUN_5d32f0, s, 1);
}

} // extern "C"

// === the water surface (a draw callback at $L_5aa750)

extern "C" BYTE* __fastcall Render_Material(BYTE* self, void*);
extern "C" double __cdecl Tsc_Now();
extern "C" void Ws_Select(); KING_ALIAS("_Ws_Select", "@Material_Select@24")
KING_BSS(BYTE, g_ws6fd758, 0x6fd758)
KING_BSS(float, g_wsPos, 0x7009e0)                      // the strip's points, normals and both texture coordinates
KING_BSS(float, g_wsNormal, 0x6fb6f8)
KING_BSS(float, g_wsUv0, 0x6fa750)
KING_BSS(float, g_wsUv1, 0x6fcad8)
KING_BSS(BYTE*, g_wsSurface, 0x7025d4)
KING_BSS(int, g_ws7025d8, 0x7025d8)
KING_BSS(BYTE*, g_ws7025b8, 0x7025b8)
KING_BSS(BYTE*, g_wsDevice, 0x720908)
KING_BSS(Vec3, g_wsAhead, 0x702620)
KING_BSS(Vec3, g_wsUp, 0x702630)
ASM_VAR(double, g_ws64b748, $L_64b748)
ASM_VAR(float, g_ws64cab0, $L_64cab0)
ASM_VAR(float, g_ws64c258, $L_64c258)
ASM_VAR(float, g_ws64ed88, $L_64ed88)
ASM_VAR(double, g_ws64b5b8, $L_64b5b8)
ASM_VAR(double, g_ws64b780, $L_64b780)
ASM_VAR(float, g_ws64c908, $L_64c908)
ASM_VAR(double, g_ws64b670, $L_64b670)
ASM_VAR(double, g_ws67e8d8, $L_67e8d8)
ASM_VAR(double, g_ws67e8d0, $L_67e8d0)
ASM_VAR(double, g_ws64b678, $L_64b678)
ASM_VAR(double, g_ws64b6e0, $L_64b6e0)
ASM_VAR(double, g_ws64b7a8, $L_64b7a8)
ASM_VAR(double, g_ws64b7b8, $L_64b7b8)
ASM_VAR(float, g_ws64f720, $L_64f720)

struct WsStream { void* data; int stride; };

extern "C" {

// [$L_5aa750] draws the water surface 7025d4 around the camera of renderer r: 25 strips of 52 points stepping
// away from the camera along its heading, each point at the surface's height with its normal, texture coordinates
// on the world grid and projected from the camera's height; the surface's wave level (+96, +64 / +72 / +80)
// follows the wind 7025b8 (+532) smoothed in 67e8d0
int __cdecl Water_Draw(void*, BYTE* r)
{
    if (!(g_ws6fd758 & 1))
    {
        g_ws6fd758 |= 1;
        memset(&g_wsPos, 0, 4800);
    }
    if (!(g_ws6fd758 & 2))
    {
        g_ws6fd758 |= 2;
        memset(&g_wsNormal, 0, 4800);
    }
    if (!g_wsSurface)
        return 0;
    WsStream desc[12];
    memset(desc, 0, sizeof(desc));
    desc[0].data = &g_wsPos;
    desc[0].stride = 12;
    desc[1].data = &g_wsNormal;
    desc[1].stride = 12;
    desc[4].data = &g_wsUv0;
    desc[4].stride = 8;
    desc[5].data = &g_wsUv1;
    desc[5].stride = 8;
    Vec3 C = *(const Vec3*)Render_Material(r, 0);
    double h0 = C.z;
    if (C.z < g_ws64b748)
        h0 = 1e-5;
    // the heading flattened, the side across it and the steps
    Matrix I;
    const float* M = Matrix_Inverse(&I, (const Matrix*)(r + 4208))->m;
    Vec3 d0, d;
    d0.x = (float)((double)g_wsAhead.z * M[6] + (double)g_wsAhead.y * M[3] + (double)g_wsAhead.x * M[0]);
    d0.y = (float)((double)g_wsAhead.z * M[7] + (double)g_wsAhead.y * M[4] + (double)g_wsAhead.x * M[1]);
    d0.z = 0;
    d = *Vec3_Normalized(&d, &d0);
    const Vec3& U = g_wsUp;
    double sx = (double)d.y * U.z - (double)U.y * d.z;
    float sy = (float)((double)U.x * d.z - (double)d.x * U.z);
    float sz = (float)((double)d.x * U.y - (double)d.y * U.x);
    double s1x = sx * g_ws64cab0;
    float s1y = (float)(sy * (double)g_ws64cab0), s1z = (float)(sz * (double)g_ws64cab0);
    double s2x = s1x * g_ws64c258;
    float s2y = (float)(s1y * (double)g_ws64c258), s2z = (float)(s1z * (double)g_ws64c258);
    Vec3 step;
    step.x = (float)(s2x * g_ws64ed88);
    step.y = (float)(s2y * (double)g_ws64ed88);
    step.z = (float)(s2z * (double)g_ws64ed88);
    double fx = d.x * (double)g_ws64cab0;
    float fy = (float)(d.y * (double)g_ws64cab0), fz = (float)(d.z * (double)g_ws64cab0);
    Vec3 D;
    D.x = (float)(fx * g_ws64ed88);
    D.y = (float)(fy * (double)g_ws64ed88);
    D.z = (float)(fz * (double)g_ws64ed88);
    double gridX = (int)(C.x * g_ws64b5b8) * g_ws64b780;
    double gridY = (int)(C.y * g_ws64b5b8) * g_ws64b780;
    g_ws7025d8 = (g_ws7025d8 + 1) % 16;
    BYTE* w = g_wsSurface;
    if (g_ws7025b8)
    {
        // the waves follow the wind
        double v = (g_ws64b670 - g_ws67e8d8) * (F(float, g_ws7025b8, 532) * (double)g_ws64c908) +
                   g_ws67e8d8 * g_ws67e8d0;
        g_ws67e8d0 = v;
        F(double, w, 96) = v * F(double, w, 88);
        if (F(double, w, 96) < g_ws64b678)
            F(double, w, 96) = 0;
        F(double, w, 64) = F(double, w, 96) * g_ws64b6e0;
        F(double, w, 72) = F(double, w, 64) * g_ws64b7a8;
        F(double, w, 80) = F(double, w, 64) * g_ws64b7b8;
    }
    CallThis(Ws_Select, r, F(BYTE*, g_wsSurface, 104), 1, 0, 0);
    void** vt = *(void***)g_wsDevice;
    ((long(__stdcall*)(BYTE*, int, int))vt[20])(g_wsDevice, 136, 1);
    double rx = step.x * (double)g_ws64f720;
    float ry = (float)(step.y * (double)g_ws64f720), rz = (float)(step.z * (double)g_ws64f720);
    Vec3 rs;
    rs.x = (float)(rx * g_ws64c258);
    rs.y = (float)(ry * (double)g_ws64c258);
    rs.z = (float)(rz * (double)g_ws64c258);
    double spent = 0;
    for (int row = 24; row >= 0; row--)
    {
        float bx = (float)(row * (double)D.x), by = (float)(D.y * (double)row);
        double bz = row * (double)D.z;
        float ox = (float)((double)bx + C.x), oy = (float)((double)by + C.y);
        Vec3 P;
        P.x = (float)((double)ox - rs.x);
        P.y = (float)((double)oy - rs.y);
        P.z = (float)(bz - rs.z);
        double t0 = Tsc_Now();
        float N[3] = {0, 0, 0};
        float* pos = &g_wsPos;
        float* nrm = &g_wsNormal;
        float* uv0 = &g_wsUv0;
        float* uv1 = &g_wsUv1;
        for (int k = 0; k < 26; k++)
        {
            // the near point, then the far one a step D on
            Vec3 Q;
            double qx = (double)P.x - D.x;
            Q.x = (float)qx;
            Q.y = (float)((double)P.y - D.y);
            Q.z = (float)((double)P.z - D.z);
            for (int e = 0; e < 2; e++)
            {
                double x = e ? qx : P.x;
                double y = e ? Q.y : P.y;
                pos[0] = (float)x;
                pos[1] = (float)y;
                pos[2] = (float)Surface_HeightNormal(w, 0, (float)x, (float)y, 0, N);
                nrm[0] = N[0];
                nrm[1] = N[1];
                nrm[2] = N[2];
                uv0[0] = (float)((x - gridX) * g_ws64b5b8);
                uv0[1] = (float)((y - gridY) * g_ws64b5b8);
                uv1[0] = (float)((x - C.x) * g_ws64b6e0 / h0 + g_ws64b6e0);
                uv1[1] = (float)((y - C.y) * g_ws64b6e0 / h0 + g_ws64b6e0);
                pos += 3;
                nrm += 3;
                uv0 += 2;
                uv1 += 2;
            }
            float ny = (float)((double)Q.y + D.y), nz = (float)((double)Q.z + D.z);
            P.x = (float)((double)Q.x + D.x + step.x);
            P.y = (float)((double)step.y + ny);
            P.z = (float)((double)step.z + nz);
        }
        spent = Tsc_Now() - t0 + spent;
        ((long(__stdcall*)(BYTE*, int, int, void*, int, int))vt[29])(g_wsDevice, 5, 530, desc, 52, 0);
    }
    return 1;
}

} // extern "C"

// === a dial's hands turned and its face copied from the screen (FUN_5a6420)

extern "C" void FUN_5a58e0(); KING_ALIAS("_FUN_5a58e0", "@Windscreen_Frame@8")
extern "C" void Dl_RotY(); KING_ALIAS("_Dl_RotY", "_Matrix_RotationY")
extern "C" void Dl_Apply(); KING_ALIAS("_Dl_Apply", "_Frame_Apply")
extern "C" void Dl_Switch(); KING_ALIAS("_Dl_Switch", "_Switch_Select")
extern "C" void Dl_Level(); KING_ALIAS("_Dl_Level", "_Mode_TextureLevel")
extern "C" void Dl_Upload(); KING_ALIAS("_Dl_Upload", "@Texture_Upload@24")
extern "C" void Dl_Post(); KING_ALIAS("_Dl_Post", "@Queue_PostEvent@12")
KING_BSS(BYTE*, g_dl7208fc, 0x7208fc)
ASM_VAR(double, g_dl64da00, $L_64da00)
ASM_VAR(int, g_dlRows, $L_67e448)

// a method of the DirectDraw surface d (vtable slot at byte offset off)
template <class... A>
static int Dl_Com(void* d, unsigned off, A... args)
{
    void* fn = *(void**)(*(char**)d + off);
    return ((int(__stdcall*)(void*, A...))fn)(d, args...);
}

// a 16-bit screen pixel as the dial's 4444 texel (alpha kept from d): 555 when level 2, else 565
static WORD Dl_Texel(WORD p, WORD d, bool l2)
{
    WORD c;
    if (l2)
        c = (WORD)((WORD)((((p >> 1) & 0x7800) | (p & 0x780)) >> 2 | (p & 30)) >> 1);
    else
        c = (WORD)((WORD)((((p >> 1) & 0x3C00) | (p & 0x3C0)) >> 1 | (p & 30)) >> 1);
    return (WORD)(c | (d & 0xF000));
}

extern "C" {

// [FUN_5a6420] dial s: its glass shown (+192, flag 16 with 2 hides it), its value from +152, its three hands
// (+128, +136, +144) turned by +112; with flag 8 its face (+196) on, and the screen under the window (s +32)
// copied into its texture (+200, n x n, every other row and column checkerboarded by the frame) and both
// textures uploaded; then its event 1
void __fastcall Dial_Update(BYTE* s)
{
    DWORD fl = F(DWORD, s, 164);
    if ((fl & 16) && F(BYTE*, s, 192) && (fl & 2))
        F(BYTE, F(BYTE*, s, 192), 40) &= ~1;
    else if (F(BYTE*, s, 192))
        F(BYTE, F(BYTE*, s, 192), 40) |= 1;
    if (F(WORD*, s, 152))
        F(double, s, 72) = *F(WORD*, s, 152);
    CallThis(FUN_5a58e0, s);
    float M[12];
    static const int hands[3] = {128, 136, 144};
    for (int k = 0; k < 3; k++)
        CallC(Dl_Apply, F(void*, s, hands[k]), 0, CallC<void*>(Dl_RotY, M, (float)(F(double, s, 112) - g_dl64da00)));
    if (!(F(BYTE, s, 164) & 8))
    {
        CallC(Dl_Switch, F(void*, s, 196), 0);
        CallThis(Dl_Post, s, 1);
        return;
    }
    CallC(Dl_Switch, F(void*, s, 196), 1);
    if (!F(BYTE*, s, 148))
    {
        CallThis(Dl_Post, s, 1);
        return;
    }
    if (g_7025b0 && F(BYTE*, s, 200))
    {
        DWORD desc[31] = {0};
        desc[0] = 124;
        desc[1] = 8387054;
        if (!Dl_Com(g_dl7208fc, 100, 0, desc, 17, 0))
        {
            int pitch = (int)desc[4];
            BYTE* surf = (BYTE*)desc[9];
            int a = CallVirt<int>(F(BYTE*, s, 32), 20) - 41;
            int b = CallVirt<int>(F(BYTE*, s, 32), 24) - 100;
            BYTE* tex = F(BYTE*, s, 200);
            int n = F(WORD, tex, 20);
            if (surf)
            {
                double nd = n;
                float step = (float)(a / nd);
                WORD* dst = F(WORD*, tex, 8);
                float y = 50.0f;
                int frame = F(int, F(BYTE*, s, 32), 2992);
                const int* rowT = &g_dlRows;
                for (int j = 0; j < n; j++)
                {
                    BYTE* row = surf + (int)y * pitch;
                    if (!((j + frame) % 2))
                    {
                        bool l2 = CallC<int>(Dl_Level) == 2;
                        double x = 20.0f;
                        const int* colT = &g_dlRows;
                        WORD* d = dst;
                        for (int i = 0; i < n; i++)
                        {
                            if (!(((frame + i) / 2) % 2))
                            {
                                int o = colT[i] + rowT[j];
                                WORD p = ((WORD*)row)[(int)x + o * 2];
                                *d = Dl_Texel(p, *d, l2);
                            }
                            d++;
                            x += step;
                        }
                    }
                    y = (float)((double)b / nd + y);
                    dst += n;
                }
            }
            Dl_Com(g_dl7208fc, 128, 0);
        }
    }
    CallThis(Dl_Upload, F(void*, s, 148), 0, 0, 1.0);
    if (F(BYTE*, s, 200))
        CallThis(Dl_Upload, F(void*, s, 200), 0, 0, 1.0);
    CallThis(Dl_Post, s, 1);
}

} // extern "C"

// === the windscreen weather: drops, splashes, the drying glass and the wiper (FUN_5a58e0 with its tail $L_5a5ad9)

extern "C" int __fastcall Wd_Post1(BYTE* self, void*, int arg); KING_ALIAS("@Wd_Post1@12", "@Queue_PostEvent1@12")
extern "C" int __fastcall Wd_Post2(BYTE* self, void*, int arg); KING_ALIAS("@Wd_Post2@12", "@Queue_PostEvent2@12")
extern "C" void __cdecl Wd_Mark(const BYTE* s, int x, int y, const BYTE* flags); KING_ALIAS("_Wd_Mark", "_Surface_Mark")
extern "C" void __cdecl Wd_MarkPixel(const BYTE* s, int x, int y, const BYTE* flags); KING_ALIAS("_Wd_MarkPixel", "_Gfx_MarkPixel")
extern "C" void __cdecl Wd_Line(BYTE* w, int x0, int y0, int x1, int y1, DWORD c, int rgb); KING_ALIAS("_Wd_Line", "_Window_DrawLine")
extern "C" BYTE* __fastcall Wd_ObjectKind(BYTE* s, void*, int kind, const char* name); KING_ALIAS("@Wd_ObjectKind@16", "@Object_ConstructKind@16")
extern "C" void __fastcall Wd_OwnerAttach(BYTE* self, void*, BYTE* node); KING_ALIAS("@Wd_OwnerAttach@12", "@Owner_Attach@12")
extern "C" int __cdecl rand();
ASM_VAR(double, g_wd651540, $L_651540)
ASM_VAR(double, g_wd64da00, $L_64da00)
KING_BSS(float, g_wd7025ac, 0x7025ac)
KING_BSS(int, g_wd7025b0, 0x7025b0)
ASM_VAR(double, g_wd64b6f0, $L_64b6f0)
ASM_VAR(double, g_wd64b678, $L_64b678)
ASM_VAR(double, g_wd64b670, $L_64b670)
ASM_VAR(double, g_wd64b660, $L_64b660)
ASM_VAR(double, g_wd64d338, $L_64d338)
ASM_VAR(double, g_wd64d0b8, $L_64d0b8)
ASM_VAR(double, g_wd650ad8, $L_650ad8)
ASM_VAR(double, g_wd651518, $L_651518)
ASM_VAR(char, g_wd672638, $L_672638)
ASM_VAR(BYTE, g_wdSplashVt651520, $L_651520)

#define SD(n) F(double, s, n)

// one wiper position at angle a: the blade between radii +104 and +108 about (+96, +100)
static void Wd_Wiper(BYTE* s, double a)
{
    double cx = F(int, s, 100);
    double ang1 = a - g_wd650ad8;
    double r1 = F(int, s, 108);
    double x0 = F(int, s, 96);
    double ang2 = a - g_wd651518;
    double r2 = F(int, s, 104);
    int y1 = (int)(cx - x87_cos(ang1) * r1);
    int x1 = (int)(x87_sin(ang1) * r1 + x0);
    int y2 = (int)(cx - x87_cos(ang2) * r2);
    int x2 = (int)(x87_sin(ang2) * r2 + x0);
    Wd_Line(F(BYTE*, s, 152), x2, y2, x1, y1, 0, 1);
}

extern "C" {

// [FUN_5a58e0..$L_5a5ad9] windscreen s (+32 the app, +152 the glass image, +164 flags: 1 hidden, 2 raining,
// 4 wiping, 8 wet, 32 drying): the wiper angle (+112) swung between +176 and its end, new drops (+48 their count
// growing with the rain +40 and speed +184) marked on the glass, splashes (+56) spawned as objects, the drying
// glass faded one level every fifth frame, and the wiper drawn along the swept angle
void __fastcall Windscreen_Frame(BYTE* s, void*)
{
    double old = SD(112);
    DWORD fl = F(DWORD, s, 164);
    if (fl & 4)
    {
        if (F(int, s, 120))
        {
            SD(112) = SD(112) - F(double, F(BYTE*, s, 32), 2960) * g_wd651540;
            if (SD(112) < SD(176))
            {
                SD(112) = SD(176);
                F(int, s, 120) = 0;
            }
        }
        else
        {
            SD(112) = F(double, F(BYTE*, s, 32), 2960) * g_wd651540 + SD(112);
            if (!(SD(112) <= g_wd64da00))
            {
                DWORD b[2] = { 1293080650u, 1073291771u };
                SD(112) = *(double*)b;
                F(int, s, 168) = 1;
                F(int, s, 120) = 1;
            }
        }
    }
    else
        F(int, s, 168) = 1;
    if ((fl & 1) && !F(int, s, 156))
    {
        Wd_Post1(s, 0, 1);
        return;
    }
    if (!F(int, s, 68) || !F(int, s, 64) || !F(int, s, 152) || !F(int, s, 148))
        return;
    g_wd7025ac = (float)SD(184);
    DWORD st = F(DWORD, s, 164);
    if ((st & 2) && (st & 8))
    {
        double x = F(double, F(BYTE*, s, 32), 2960) * SD(40);
        if (SD(184) <= g_wd64b6f0)
            x = x * (g_wd64b670 - SD(184) * g_wd64d338);
        else
            x = g_wd64b678;
        SD(48) = x + SD(48);
    }
    if (!(SD(48) <= g_wd64b670))
    {
        int n = (int)SD(48);
        SD(48) = SD(48) - n;
        for (int k = n; k > 0; k--)
        {
            int x = rand() % (int)F(WORD, F(BYTE*, s, 152), 0);
            int y = rand() % (int)F(WORD, F(BYTE*, s, 152), 2);
            if (g_wd7025b0)
                Wd_MarkPixel(F(BYTE*, s, 152), x, y, F(BYTE*, s, 64));
            else
                Wd_Mark(F(BYTE*, s, 152), x, y, F(BYTE*, s, 64));
        }
    }
    st = F(DWORD, s, 164);
    if ((st & 2) && (st & 8))
    {
        double x = F(double, F(BYTE*, s, 32), 2960) * SD(40);
        if (SD(184) <= g_wd64b6f0)
            x = x * (SD(184) * g_wd64d338 + g_wd64b670);
        else
            x = g_wd64b660;
        SD(56) = x + SD(56);
    }
    if (!(SD(56) <= g_wd64b670))
    {
        int n = (int)SD(56);
        SD(56) = SD(56) - n;
        for (int k = n; k > 0; k--)
        {
            int x = rand() % (int)F(WORD, F(BYTE*, s, 152), 0);
            int y = rand() % (int)F(WORD, F(BYTE*, s, 152), 2);
            BYTE* p = (BYTE*)operator new(56);
            if (p)
            {
                BYTE* glass = F(BYTE*, s, 152);
                BYTE* app = F(BYTE*, s, 32);
                BYTE* drop = F(BYTE*, s, 68);
                Wd_ObjectKind(p, 0, 11, &g_wd672638);
                F(BYTE*, p, 40) = glass;
                F(int, p, 52) = y;
                F(void*, p, 0) = &g_wdSplashVt651520;
                F(BYTE*, p, 32) = app;
                F(BYTE*, p, 36) = s + 156;
                F(int, p, 48) = x;
                Wd_OwnerAttach(app + 2992, 0, p);
                F(BYTE*, p, 44) = drop;
                Wd_Post2(p, 0, 1);
            }
            F(int, s, 156)++;
        }
    }
    st = F(DWORD, s, 164);
    if (st & 32)
    {
        if (F(int, s, 160) > 0)
        {
            F(DWORD, s, 164) = st & ~64u;
            BYTE* img = F(BYTE*, s, 152);
            if (F(WORD, img, 4) == 2)
            {
                WORD* pix = F(WORD*, img, F(int, img, 16) * 4 + 24);
                for (int y = 0; y < F(WORD, F(BYTE*, s, 152), 2); y++)
                {
                    for (int x = 0; x < F(WORD, F(BYTE*, s, 152), 0); x++)
                    {
                        WORD* p = pix + F(WORD, F(BYTE*, s, 152), 0) * y + x;
                        WORD v = *p;
                        DWORD c = (WORD)(v >> 12);
                        if ((WORD)c > 0 && F(int, F(BYTE*, s, 32), 2992) % 5 == 0)
                            c += 65535;
                        *p = (WORD)((v & 0xFFF) | (c << 12));
                    }
                }
            }
            F(int, s, 160)--;
        }
        else
            F(DWORD, s, 164) = (st & ~0x28u) | 64;
    }
    if (!(F(BYTE, s, 164) & 4))
        return;
    double target = SD(112);
    double a = old;
    if (old < target)
    {
        do
        {
            a = a + g_wd64d0b8;
            if (F(int, s, 124))
                Wd_Wiper(s, a);
        }
        while (a < target);
    }
    else if (!(old <= target))
    {
        do
        {
            a = a - g_wd64d0b8;
            if (F(int, s, 124))
                Wd_Wiper(s, a);
        }
        while (!(a <= target));
    }
}

} // extern "C"

#undef SD

// ---------------------------------------------------------------------------
// Batch 32: the explosion effect

extern "C" void Ex32_SkyEffect(); KING_ALIAS("_Ex32_SkyEffect", "@SkyEffect_Construct@68")
extern "C" void Ex32_ObjFind(); KING_ALIAS("_Ex32_ObjFind", "_Objects_Find")
ASM_VAR(BYTE, ex32_vtbl651448, $L_651448)
ASM_VAR(BYTE, ex32_vtbl6513fc, $L_6513fc)

extern "C" {

// [FUN_5a4f00] explosion effect constructor (world o, -, at x y z, size, -, the sound's name, n puffs, spread d):
// a sky effect of n x n particles (4 kinds unless o's kind (vtable +28) is 1) and the named sound at +176
BYTE* __fastcall Explosion_Construct(BYTE* self, void*, BYTE* o, int a1, float x, float y, float z, double size,
                                     int a7, const char* name, int n, double d)
{
    double nn = (double)n;
    float sz = (float)size;
    int kind = ((int(__thiscall*)(BYTE*))(*(void***)o)[7])(o);
    CallThis(Ex32_SkyEffect, self, o, a1, x, y, z, sz, sz, a7, n * n, n, n, (float)d, (float)(nn * nn * d), 100,
             kind == 1 ? 1 : 4);
    F(void*, self, 0) = &ex32_vtbl651448;
    F(void*, self, 32) = &ex32_vtbl6513fc;
    F(BYTE*, self, 176) = CallC<BYTE*>(Ex32_ObjFind, name);
    F(double, self, 184) = 1.0;
    return self;
}

}

// ---------------------------------------------------------------------------
// Batch 33: falling flakes, weather scene items

extern "C" void Wf33_MarkPixel(); KING_ALIAS("_Wf33_MarkPixel", "_Gfx_MarkPixel")
extern "C" void Wf33_SurfaceMark(); KING_ALIAS("_Wf33_SurfaceMark", "_Surface_Mark")
extern "C" void Wf33_PostEvent(); KING_ALIAS("_Wf33_PostEvent", "@Queue_PostEvent@12")
extern "C" void Wi33_Base(); KING_ALIAS("_Wi33_Base", "@Worobj5a4120_Construct@28")
extern "C" void Wi33_Node(); KING_ALIAS("_Wi33_Node", "@Node_Construct@16")
extern "C" void Wi33_GetWorld(); KING_ALIAS("_Wi33_GetWorld", "@Node_GetWorld@12")
extern "C" void Wi33_Root(); KING_ALIAS("_Wi33_Root", "@Node_Root@8")
extern "C" void Wi33_NodeN(); KING_ALIAS("_Wi33_NodeN", "@NodeN_Construct@20")
extern "C" void Wi33_Apply(); KING_ALIAS("_Wi33_Apply", "@Node_ApplyMatrix@16")
extern "C" void Wi33_Attach(); KING_ALIAS("_Wi33_Attach", "@WeatherNode_Attach@12")
extern "C" void Wi33_Fatal(); KING_ALIAS("_Wi33_Fatal", "_Sys_Fatal")
ASM_VAR(double, wf33_speed64b938, $L_64b938)
ASM_VAR(double, wf33_half64b6e0, $L_64b6e0)
KING_BSS(float, wf33_wind7025ac, 0x7025ac)
ASM_VAR(double, wf33_k64c958, $L_64c958)
KING_BSS(int, wf33_hw7025b0, 0x7025b0)
KING_BSS(char, wi33_name68d1c0, 0x68d1c0)
ASM_VAR(BYTE, wi33_vtbl64c7dc, $L_64c7dc)
ASM_VAR(BYTE, wi33_vtbl651358, $L_651358)
ASM_VAR(BYTE, wi33_vtbl65130c, $L_65130c)
ASM_VAR(BYTE, wi33_vtbl64d7d0, $L_64d7d0)
KING_BSS(BYTE, wi33_identity7025f0, 0x7025f0)
ASM_VAR(char, wi33_s67e918, $L_67e918)

struct Wi33_M48 { DWORD d[12]; };    // a matrix passed by value

extern "C" {

// [$L_5a5f50..$L_5a6050] a falling flake's step (vtable): it falls by the frame's speed (y +52) and drifts by
// the wind and its place across the surface (x +48, a random -4..3); its trail is marked in thirds on the
// surface (+40, colour +44), by the hardware marker (7025b0) or in software; inside the surface it posts event
// 1, else it shrinks (+36) and ends through vtable +28
void __fastcall Flake_Step(BYTE* self, void*)
{
    int dy = (int)(F(double, F(BYTE*, self, 32), 2960) * wf33_speed64b938);
    int r = rand() % 8;
    BYTE* surf = F(BYTE*, self, 40);
    int w = F(WORD, surf, 0);
    int dx = (int)((((double)F(int, self, 48) / w - wf33_half64b6e0) * wf33_wind7025ac) * wf33_k64c958 + (r - 4));
    void (*mark)() = wf33_hw7025b0 ? Wf33_MarkPixel : Wf33_SurfaceMark;
    int c = F(int, self, 44);
    CallC(mark, surf, F(int, self, 48) + dx / 3, F(int, self, 52) + dy / 3, c);
    CallC(mark, F(BYTE*, self, 40), F(int, self, 48) + dx * 2 / 3, F(int, self, 52) + dy * 2 / 3, F(int, self, 44));
    CallC(mark, F(BYTE*, self, 40), F(int, self, 48) + dx, F(int, self, 52) + dy, F(int, self, 44));
    F(int, self, 52) += dy;
    F(int, self, 48) += dx;
    surf = F(BYTE*, self, 40);
    int x = F(int, self, 48);
    if ((int)F(WORD, surf, 2) > F(int, self, 52) && x > 0 && x < (int)F(WORD, surf, 0)) {
        CallThis(Wf33_PostEvent, self, 1);
        return;
    }
    F(int, self, 36) -= 4;
    CallVirt(self, 28, 1);
}

// [FUN_5a44c0..$L_5a45b8] a weather scene item (world, object a, object b, n, scale, -, matrix m, kind): the
// world object base, its node (+256: name 68d1c0, the model of b or of b's +64, -1 slot at +344), kind 1 takes
// b's root after placing it by m (b must be of type 17); a 112-byte matrix node at +324 set to m, the item
// attached to its owner (the model's +20 or +28 of the world's +524, or b's +44) and b's colour at +276
BYTE* __fastcall WeatherItem_Construct(BYTE* self, void*, BYTE* world, BYTE* a, BYTE* b, int n, int scale, int z,
                                       Wi33_M48 m, int kind)
{
    CallThis(Wi33_Base, self, world, n, 0, scale, z);
    BYTE* model = kind == 0 ? b : F(BYTE*, b, 64);
    BYTE* node = self + 256;
    CallThis(Wi33_Node, node, (const char*)&wi33_name68d1c0, 17);
    for (int i = 48; i <= 60; i += 4)
        F(int, node, i) = 0;
    F(void*, node, 0) = &wi33_vtbl64c7dc;
    F(BYTE*, node, 64) = model;
    for (int i = 68; i <= 80; i += 4)
        F(int, node, i) = 0;
    F(int, node, 84) = -1;
    F(int, node, 36) = F(int, model, 36);
    F(int, self, 344) = -1;
    F(void*, self, 0) = &wi33_vtbl651358;
    F(void*, node, 0) = &wi33_vtbl65130c;
    BYTE* owner = a;
    if (kind == 1) {
        if (F(int, b, 16) != 17)
            CallC(Wi33_Fatal, (const char*)&wi33_s67e918, 0);
        CallThis(Wi33_GetWorld, F(BYTE*, b, 68), &m);
        owner = CallThis<BYTE*>(Wi33_Root, b);
        F(BYTE, b, 40) |= 1;
    }
    BYTE* p = (BYTE*)operator new(112);
    if (p) {
        CallThis(Wi33_NodeN, p, (const char*)&wi33_name68d1c0, 6, 1);
        memset(p + 64, 0, 48);
        memcpy(p + 64, &wi33_identity7025f0, 48);
        F(void*, p, 0) = &wi33_vtbl64d7d0;
        F(int, p, 56) = 0;
        F(int, p, 60) = 0;
    }
    F(BYTE*, self, 324) = p;
    CallThis(Wi33_Apply, p, &m, 0);
    BYTE* adj = self ? node : 0;
    if (kind == 0) {
        if (owner == F(BYTE*, world, 524))
            CallVirt(owner, 20, 0, adj);
        else
            CallVirt(owner, 28, adj);
    } else {
        CallVirt(F(BYTE*, b, 44), 20, 0, adj);
    }
    memcpy(self + 276, b + 20, 16);
    CallThis(Wi33_Attach, self, adj);
    return self;
}

}
