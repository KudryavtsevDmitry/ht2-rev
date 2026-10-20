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
ASM_PROC(FUN_55f000)    // whether a vehicle can take another (vehicle, other)
ASM_PROC(FUN_4f1180)
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

// ---------------------------------------------------------------------------
// the crowd (the ovation sound) of a scene object: its matrix is the object's
// moved by ten times the vector at 702630; fn is called when it plays

extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")    // an empty function: the default callback
ASM_VAR(float, g_eng702630, $L_702630)
ASM_VAR(float, g_eng702634, $L_702634)
ASM_VAR(float, g_eng702638, $L_702638)
ASM_VAR(float, g_ten64c450, $L_64c450)          // 10.0
ASM_VAR(AsmLabel, g_strOvation, $L_678540)

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

// ---------------------------------------------------------------------------
extern "C" void FUN_5e1ad0(); KING_ALIAS("_FUN_5e1ad0", "_Node_ParentRoot")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
ASM_PROC(FUN_5a3f50)    // (this, node)
ASM_PROC(FUN_5de950)    // decomposes a matrix (by value) into six doubles
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
ASM_VAR(DWORD, g_wea6fb3d0, $L_6fb3d0)
ASM_VAR(DWORD, g_wea6fb3d4, $L_6fb3d4)
ASM_VAR(DWORD, g_wea6fb3d8, $L_6fb3d8)
ASM_VAR(DWORD, g_wea6fb3dc, $L_6fb3dc)
ASM_VAR(DWORD, g_wea6fb3e0, $L_6fb3e0)
ASM_VAR(DWORD, g_wea6fb3e4, $L_6fb3e4)
ASM_VAR(DWORD, g_wea6fb3e8, $L_6fb3e8)
ASM_VAR(DWORD, g_wea6fb3ec, $L_6fb3ec)
ASM_VAR(DWORD, g_wea702620, $L_702620)
ASM_VAR(DWORD, g_wea702624, $L_702624)
ASM_VAR(DWORD, g_wea702628, $L_702628)
ASM_VAR(DWORD, g_wea702630, $L_702630)
ASM_VAR(DWORD, g_wea702634, $L_702634)
ASM_VAR(DWORD, g_wea702638, $L_702638)
ASM_VAR(DWORD, g_wea702640, $L_702640)
ASM_VAR(DWORD, g_wea702644, $L_702644)
ASM_VAR(DWORD, g_wea702648, $L_702648)
ASM_VAR(DWORD, g_wea702650, $L_702650)
ASM_VAR(DWORD, g_wea702654, $L_702654)
ASM_VAR(DWORD, g_wea702658, $L_702658)

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

ASM_VAR(AsmLabel, g_wea7025f0, $L_7025f0)

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

ASM_PROC(FUN_5fdaf0)
ASM_PROC(FUN_5a4ca0)
ASM_VAR(AsmLabel, g_wea6fd7dc, $L_6fd7dc)

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
ASM_PROC(FUN_5fd870)

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
ASM_VAR(int, g_q24_7025b0, $L_7025b0)
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

ASM_PROC(FUN_5a4ff0)
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
ASM_VAR(int, g_q47_7025b8, $L_7025b8)
// [FUN_5a73f0] destructor: the 651628 vtable, clears the 7025b8 instance, then the object
extern "C" void __fastcall Wea5a73f0_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q47_651628;
    g_q47_7025b8 = 0;
    CallThis(FUN_5d3290, s);
}
