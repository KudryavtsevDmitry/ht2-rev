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
ASM_PROC(FUN_5a53a0)
ASM_PROC(FUN_5a63a0)
ASM_PROC(FUN_5a6aa0)
ASM_PROC(FUN_5a7240)
ASM_PROC(FUN_5a7310)
ASM_PROC(FUN_5a73f0)
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
