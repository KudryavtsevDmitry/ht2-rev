// scene.cpp - the unit at 0x5ef3f0 of king.masm: scene nodes (a 3x4 local
// matrix at +64, the parent at +44, the transform mode at +56), lists of
// nodes by slot, named object tables, the near-equality test of the
// positions, and the sound channels (32 slots of playing sounds with their
// buffers, volume through the DirectSound mixer of directsound.cpp).
//
// Building with  nmake ASM_SCENE=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"
#include <math.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef DWORD Dwords[1];
typedef BYTE* Ptrs[1];

extern "C" void __fastcall Matrix_PreMultiply(void* self, void*, const void* m);   // vecmath.cpp
extern "C" void __fastcall Matrix_Transform2(void* self, void*, const void* m);    // vecmath.cpp
extern "C" void __fastcall Vec3_TransformPoint(void* self, void*, const void* m);  // vecmath.cpp
extern "C" BYTE* __fastcall Ctl_Owner(BYTE* self, void*);                         // controls.cpp
extern "C" void __cdecl Mixer_SetVolume(float v);                                   // directsound.cpp
extern "C" int __cdecl Mixer_Open(int a, HWND w);                                   // directsound.cpp
extern "C" void __cdecl Channel_Stop(int id);                                       // directsound.cpp
extern "C" void __cdecl Channel_SetFrequency(int id, int f);                        // directsound.cpp
extern "C" int __cdecl Mixer_Load(int idx, int a, int b);                           // directsound.cpp

extern "C" void FUN_5f2a40(); KING_ALIAS("_FUN_5f2a40", "_VertexDesc_Build")
extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")    // base node constructor (name, kind)
extern "C" void FUN_5e5840(); KING_ALIAS("_FUN_5e5840", "@Rec5e5840_Free@8")
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
extern "C" void FUN_5f4a90(); KING_ALIAS("_FUN_5f4a90", "@Value_Free@4")
extern "C" void FUN_5e5cb0(); KING_ALIAS("_FUN_5e5cb0", "@Obj5e5cb0_Init@8")
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
ASM_VAR(AsmLabel, g_dtor5a3df0, $L_5a3df0)
ASM_VAR(AsmLabel, g_dtor5e57a0, @Fw5e57a0_Destroy@4)
ASM_VAR(AsmLabel, g_vtbl65282c, $L_65282c)
ASM_VAR(AsmLabel, g_vtbl64d81c, $L_64d81c)
ASM_VAR(AsmLabel, g_vtbl652840, $L_652840)
ASM_VAR(AsmLabel, g_vtbl652b84, $L_652b84)
ASM_VAR(AsmLabel, g_vtbl6528d8, $L_6528d8)
ASM_VAR(AsmLabel, g_vtbl652c68, $L_652c68)
ASM_VAR(AsmLabel, g_vtbl652cb4, $L_652cb4)
ASM_VAR(AsmLabel, g_vtbl652924, $L_652924)
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")    // named object constructor (name, kind, flag)
extern "C" void FUN_6050f0(); KING_ALIAS("_FUN_6050f0", "_Mixer_GetVolume")    // mixer volume, returns double
extern "C" void FUN_604730(); KING_ALIAS("_FUN_604730", "_Sound_ErrorText")
extern "C" void FUN_604d70(); KING_ALIAS("_FUN_604d70", "_Channels_Create")
extern "C" void FUN_605130(); KING_ALIAS("_FUN_605130", "_Streams_Collect")
extern "C" void FUN_604ed0(); KING_ALIAS("_FUN_604ed0", "_Snd_Free")
extern "C" void FUN_5e98c0(); KING_ALIAS("_FUN_5e98c0", "_Data_Read5e99c0")
extern "C" void FUN_5e99c0(); KING_ALIAS("_FUN_5e99c0", "_Image_Read")
extern "C" void Mixer_Close(); KING_ALIAS("_Mixer_Close", "_Mixer_ShutdownOnce")
extern "C" void Scene_EachCallback(); KING_ALIAS("_Scene_EachCallback", "_Path_FindJoins")

ASM_VAR(Dwords, g_7025f0, $L_7025f0)            // the identity 3x4 matrix
ASM_VAR(int, g_720908, $L_720908)
ASM_VAR(HWND, g_hWnd, $L_720920)
ASM_VAR(BYTE*, g_723aa0, $L_723aa0)
ASM_VAR(Ptrs, g_channels, $L_723aa8)            // 32 playing sounds
ASM_VAR(int, g_soundOn, $L_723b28)
ASM_VAR(int, g_mixerOpen, $L_723b2c)
ASM_VAR(int, g_bufferCount, $L_723b30)
ASM_VAR(BYTE**, g_buffers, $L_723b34)
ASM_VAR(int, g_soundFrame, $L_723b38)
ASM_VAR(AsmLabel, g_vtbl652d10, $L_652d10)
ASM_VAR(AsmLabel, g_vtbl652d68, $L_652d68)
ASM_VAR(AsmLabel, g_vtbl64d578, $L_64d578)

static __forceinline void Copy48(void* d, const void* s)
{
    for (int k = 0; k < 12; k++)
        ((DWORD*)d)[k] = ((const DWORD*)s)[k];
}

static char* StrDup(const char* s)
{
    size_t n = strlen(s) + 1;
    char* d = (char*)operator new(n);
    memcpy(d, s, n);
    return d;
}

extern "C" {

// [FUN_5ef540] the 124-byte entry named name in the table of obj, its index in *index
BYTE* __cdecl Table124_Find(const char* name, BYTE* obj, int* index)
{
    int n = F(int, obj, 72);
    BYTE* base = F(BYTE*, obj, 76);
    int i = 0;
    for (; i < n; i++)
        if (strcmp(name, (const char*)base + i * 124) == 0)
            break;
    if (i >= n)
        return 0;
    *index = i;
    return base + i * 124;
}

// [FUN_5ef5c0]
BYTE* __fastcall Node_Init(BYTE* self, void*)
{
    for (int k = 4; k < 52; k += 4)
        F(int, self, k) = 0;
    Copy48(self + 4, g_7025f0);
    static const int z[] = {52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 192, 0, 100, 104, 108, 120, 116, 196, 204, 200};
    for (int k = 0; k < sizeof(z) / sizeof(z[0]); k++)
        F(int, self, z[k]) = 0;
    return self;
}

// [FUN_5ef640]
BYTE* __fastcall Node_Root(BYTE* self, void*)
{
    while (F(BYTE*, self, 44))
        self = F(BYTE*, self, 44);
    return self;
}

// [FUN_5f2080] calls vtbl+48 (a, b) of every node in slot i, backwards when a & 1
void __fastcall Slots_Call48(BYTE* self, void*, int i, int a, int b)
{
    if (i >= F(int, self, 52) || i < 0)
        return;
    BYTE* n = F(BYTE**, self, 48)[i];
    if (!n)
        return;
    if (a & 1) {
        while (F(BYTE*, n, 8))
            n = F(BYTE*, n, 8);
        for (; n; n = F(BYTE*, n, 4))
            CallVirt(n, 48, a, b);
    } else {
        for (; n; n = F(BYTE*, n, 8))
            CallVirt(n, 48, a, b);
    }
}

// [FUN_5f2100]
void __fastcall Slots_Call56(BYTE* self, void*, int i, int a, int b, int c, int d)
{
    if (i >= F(int, self, 52) || i < 0)
        return;
    BYTE* n = F(BYTE**, self, 48)[i];
    if (!n)
        return;
    if (a & 1) {
        while (F(BYTE*, n, 8))
            n = F(BYTE*, n, 8);
        for (; n; n = F(BYTE*, n, 4))
            CallVirt(n, 56, a, b, c, d);
    } else {
        for (; n; n = F(BYTE*, n, 8))
            CallVirt(n, 56, a, b, c, d);
    }
}

// [FUN_5f2190] sets (0), premultiplies (1) or transforms (2) the local matrix
void __fastcall Node_ApplyMatrix(BYTE* self, void*, const void* m, int mode)
{
    if (mode == 0)
        Copy48(self + 64, m);
    else if (mode == 1)
        Matrix_PreMultiply(self + 64, 0, m);
    else if (mode == 2)
        Matrix_Transform2(self + 64, 0, m);
}

// [FUN_5f25a0]
void __fastcall Node_GetLocal(BYTE* self, void*, void* out)
{
    Copy48(out, self + 64);
}

// [FUN_5f25c0] the world matrix
int __fastcall Node_GetWorld(BYTE* self, void*, void* out)
{
    BYTE* parent = F(BYTE*, self, 44);
    if (parent)
        F(int, self, 56) = 2;
    switch (F(int, self, 56)) {
    case 2:
        if (parent) {
            int r = Node_GetWorld(parent, 0, out);
            Matrix_PreMultiply(out, 0, self + 64);
            return r;
        }
        // fall through
    case 0:
        Copy48(out, self + 64);
        return 0;
    case 1:
        Copy48(out, self + 64);
        return 1;
    }
    return 0;
}

// [FUN_5f2dd0]
void __fastcall Node_Call5f2a40a(BYTE* self, void*, int a)
{
    CallC(FUN_5f2a40, self + 68, self + 108, a, F(int, self, 56), F(int, self, 60), F(int, self, 64));
}

// [FUN_5f33e0]
void __fastcall Node_Call5f2a40b(BYTE* self, void*, int a)
{
    CallC(FUN_5f2a40, self + 72, self + 108, a, F(int, self, 60), F(int, self, 64), F(int, self, 68));
}

// [FUN_5fa4f0]
void __fastcall Node_Set48(BYTE* self, void*, int a, int b)
{
    F(int, self, 48) = b;
    F(int, self, 52) = a;
}

// [FUN_5fa6a0]
BYTE* __fastcall Mesh_Construct(BYTE* self, void*)
{
    CallThis(FUN_5ef6d0, self, 0, 11);
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    F(int, self, 64) = 0;
    F(int, self, 48) = 0;
    F(int, self, 52) = 0;
    F(void*, self, 0) = g_vtbl652d10;
    return self;
}

// [FUN_5fa6f0]
BYTE* __fastcall Mesh_ConstructNamed(BYTE* self, void*, int name)
{
    CallThis(FUN_5ef6d0, self, name, 11);
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    F(int, self, 64) = 0;
    F(int, self, 48) = 0;
    F(int, self, 52) = 0;
    F(void*, self, 0) = g_vtbl652d10;
    return self;
}

// [FUN_5fa910] 32-byte vertices from the positions (+56) and colors (+60)
void __fastcall Mesh_BuildVertices(BYTE* self, void*, int unused)
{
    if (!g_720908) {
        F(BYTE*, self, 52) = 0;
        return;
    }
    BYTE* v = (BYTE*)operator new(F(int, self, 48) * 32);
    F(BYTE*, self, 52) = v;
    for (int i = 0; i < F(int, self, 48); i++) {
        DWORD* r = (DWORD*)(F(BYTE*, self, 52) + i * 32);
        r[0] = F(DWORD*, self, 56)[i * 3];
        r[1] = F(DWORD*, self, 56)[i * 3 + 1];
        r[2] = F(DWORD*, self, 56)[i * 3 + 2];
        r[7] = 0;
        r[6] = 0;
        r[4] = F(DWORD*, self, 60)[i];
        r[5] = 0xFF000000;
    }
}

// [FUN_5fb740]
void __cdecl Node_TransformByFrame(void* point, int on, BYTE* src)
{
    if (!on)
        return;
    DWORD m[12];
    Copy48(m, g_7025f0);
    Copy48(m, src + 5216 + F(int, src, 8236) * 48);
    Vec3_TransformPoint(point, 0, m);
}

// [FUN_5fb7d0] a lies within |c - b| + 0.001 of both b and c
int __cdecl Near3(float a, float b, float c)
{
    double t = fabs((double)c - b) + 0.001;
    if (fabs((double)a - c) > t)
        return 0;
    if (fabs((double)a - b) > t)
        return 0;
    return 1;
}

// [FUN_5fc6e0]
BYTE* __fastcall Group_Construct(BYTE* self, void*, int name)
{
    CallThis(FUN_5f19b0, self, name, 15, 1);
    F(void*, self, 0) = g_vtbl652d68;
    F(int, self, 68) = 0;
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    F(int, self, 64) = 0;
    F(int, self, 272) = 0;
    F(int, self, 284) = 0;
    for (int k = 0; k < 50; k++)
        F(int, self, 72 + k * 4) = 0;
    F(int, self, 276) = 0;
    F(int, self, 280) = 0;
    return self;
}

// [FUN_5fc740]
void __fastcall Group_EachOwner(BYTE* self, void*)
{
    BYTE** p = F(BYTE**, self, 64);
    if (!p)
        return;
    g_723aa0 = *p;
    while (g_723aa0) {
        CallVirt(self, 32, (void*)Scene_EachCallback, 0);
        g_723aa0 = Ctl_Owner(g_723aa0, 0);
    }
}

// [FUN_5fcd40] sets the sound volume (0..1), returns the previous one
float __cdecl Sound_SetVolume(float v)
{
    if (!g_soundOn)
        return v;
    float old = (float)(CallC<double>(FUN_6050f0) * 1.0101010101010102);
    double x = v;
    if (x > 1.0f)
        x = 1.0f;
    else if (!(x >= 0.0f))
        x = 0.0f;
    Mixer_SetVolume((float)(x * 0.99));
    return old;
}

// [FUN_5fce60]
int __cdecl Sound_Open()
{
    if (g_mixerOpen)
        return 0;
    g_mixerOpen = 1;
    int r = Mixer_Open(0, g_hWnd);
    if (!r)
        return 0;
    return CallC<int>(FUN_604730, r);
}

// [FUN_5fcea0]
void __cdecl Sound_AllocBuffers(int n)
{
    if (!g_soundOn)
        return;
    g_bufferCount = n;
    g_buffers = (BYTE**)operator new(n * 4);
    for (int i = 0; i < n; i++)
        g_buffers[i] = 0;
    CallC(FUN_604d70, n);
}

// [FUN_5fcef0]
int __cdecl Sound_Close()
{
    if (!g_mixerOpen)
        return 0;
    g_mixerOpen = 0;
    return CallC<int>(Mixer_Close);
}

// [FUN_5fd150]
int __cdecl Sound_IsOn()
{
    return g_soundOn;
}

// [FUN_5fd160] stops the finished sounds of earlier frames
void __cdecl Sound_Frame()
{
    if (!g_soundOn)
        return;
    for (int i = 0; i < 32; i++) {
        BYTE* s = g_channels[i];
        if (s && F(int, s, 24) != -1 && F(int, s, 12) != g_soundFrame && !F(int, s, 16)) {
            Channel_Stop(F(int, s, 24));
            F(int, s, 24) = -1;
            F(int, s, 20) = 0;
            g_channels[i] = 0;
        }
    }
    CallC(FUN_605130);
    g_soundFrame++;
}

// [FUN_5fd1d0]
BYTE* __fastcall SoundFile_Construct(BYTE* self, void*, const char* name)
{
    F(char*, self, 0) = StrDup(name);
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 24) = 0;
    F(int, self, 16) = 0;
    return self;
}

// [FUN_5fd230]
BYTE* __fastcall SoundFile_ConstructData(BYTE* self, void*, int a, int b)
{
    F(int, self, 0) = 0;
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 24) = 0;
    F(int, self, 20) = b;
    F(int, self, 16) = a;
    return self;
}

// [FUN_5fd250]
void __fastcall SoundFile_Destroy(BYTE* self, void*)
{
    if (!(F(BYTE, self, 24) & 2))
        operator delete(F(void*, self, 8));
    operator delete(F(void*, self, 0));
}

// [FUN_5fd280]
int __fastcall SoundFile_SetBuffer(BYTE* self, void*, int buf)
{
    if (!g_soundOn)
        return 0;
    if (F(int, self, 8) && F(int, self, 8) != buf)
        return 0;
    F(int, self, 8) = buf;
    F(DWORD, self, 24) |= 2;
    CallC(FUN_604ed0, F(int, self, 12));
    Mixer_Load(F(int, self, 12), F(int, self, 8), F(int, self, 4));
    return 1;
}

// [FUN_5fd2e0] loads the file or the data once
int __fastcall SoundFile_Load(BYTE* self, void*)
{
    if (!g_soundOn)
        return 0;
    DWORD f = F(DWORD, self, 24);
    if (!(f & 2) || (f & 1))
        return 1;
    if (F(int, self, 0)) {
        CallC(FUN_5e98c0, self, F(int, self, 0));
        F(DWORD, self, 24) |= 1;
        return 1;
    }
    if (F(int, self, 16)) {
        CallC(FUN_5e99c0, self, F(int, self, 16), F(int, self, 20));
        F(DWORD, self, 24) |= 1;
        return 1;
    }
    return 0;
}

// [FUN_5fd350]
void __fastcall SoundFile_Unload(BYTE* self, void*)
{
    if (g_soundOn && (F(BYTE, self, 24) & 2))
        F(DWORD, self, 24) &= ~1u;
}

// [FUN_5fd370]
BYTE* __fastcall Sound_Construct(BYTE* self, void*, const char* name, int a, int b)
{
    F(void*, self, 0) = g_vtbl64d578;
    F(char*, self, 4) = StrDup(name);
    F(int, self, 12) = 0;
    F(int, self, 20) = 0;
    F(int, self, 28) = a;
    F(int, self, 8) = b;
    F(int, self, 24) = -1;
    return self;
}

// [FUN_5fd3e0] stops every playing sound
void __cdecl Sound_StopAll()
{
    if (!g_soundOn)
        return;
    for (int i = 0; i < 32; i++) {
        BYTE* s = g_channels[i];
        if (s && F(int, s, 24) != -1) {
            Channel_Stop(F(int, s, 24));
            F(int, s, 24) = -1;
            F(int, s, 20) = 0;
            g_channels[i] = 0;
        }
    }
}

// [FUN_5fd580] a free channel for s, -1 if none
int __cdecl Sound_TakeChannel(BYTE* s)
{
    if (!g_soundOn)
        return -1;
    for (int i = 0; i < 32; i++)
        if (!g_channels[i]) {
            g_channels[i] = s;
            return i;
        }
    return -1;
}

// [FUN_5fd5c0]
void __fastcall Sound_SetPitch(BYTE* self, void*, float f)
{
    int id = F(int, self, 24);
    if (id != -1)
        Channel_SetFrequency(id, (int)((double)f * 1000.0f));
}

}

// --- constructors/destructors with exception frames (the frames only rethrow)

// delete[] of an array with a count prefix, destroying the elements last to first
static void ArrayDelete(BYTE* p, unsigned size, void (*dtor)())
{
    if (!p)
        return;
    int n = F(int, p, -4);
    for (int i = n - 1; i >= 0; i--)
        CallThis(dtor, p + i * size);
    operator delete(p - 4);
}

extern "C" {

// [FUN_5ef6d0] base node constructor (name, kind)
BYTE* __fastcall Node_Construct(BYTE* self, void*, const char* name, int kind)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(int, self, 32) = 0;
    F(void*, self, 0) = g_vtbl652840;
    if (name && *name) {
        char* p = (char*)operator new(strlen(name) + 1);
        F(char*, self, 12) = p;
        strcpy(p, name);
    } else {
        F(char*, self, 12) = 0;
    }
    F(BYTE, self, 40) = 0;
    F(int, self, 16) = kind;
    F(int, self, 44) = 0;
    F(int, self, 36) = 0;
    return self;
}

// [FUN_5ef450] base node destructor
void __fastcall Node_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl65282c;
    operator delete(F(void*, self, 12));
    F(void*, self, 0) = g_vtbl64d81c;
}

// [FUN_5f19b0] a node with n child slots
BYTE* __fastcall NodeN_Construct(BYTE* self, void*, const char* name, int kind, int n)
{
    Node_Construct(self, 0, name, kind);
    F(void*, self, 0) = g_vtbl652cb4;
    F(int, self, 52) = n;
    F(void**, self, 48) = n ? (void**)operator new(n * 4) : 0;
    for (int i = 0; i < F(int, self, 52); i++)
        F(void**, self, 48)[i] = 0;
    return self;
}

// [FUN_5f1530]
BYTE* __fastcall Node5f1530_Construct(BYTE* self, void*, const char* name)
{
    Node_Construct(self, 0, name, 1);
    CallThis(FUN_5e5cb0, self + 60);
    F(short, self, 96) = 0;
    F(short, self, 98) = 0;
    F(short, self, 52) = 0;
    F(int, self, 56) = 0;
    F(int, self, 48) = 0;
    F(void*, self, 0) = g_vtbl652c68;
    return self;
}

// [FUN_5f15a0]
void __fastcall Node5f1530_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652c68;
    ArrayDelete(F(BYTE*, self, 56), 88, (void (*)())(void*)g_dtor5e57a0);
    CallThis(FUN_5e5840, self + 60);
    CallThis(FUN_5ef660, self);
}

// [FUN_5f06e0]
void __fastcall Node5f06e0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652b84;
    ArrayDelete(F(BYTE*, self, 64), 28, (void (*)())(void*)g_dtor5a3df0);
    operator delete(F(void*, self, 68));
    CallThis(FUN_5e5840, self + 72);
    CallThis(FUN_5f1a70, self);
}

// [FUN_5f07a0]
void __fastcall Node5f07a0_Destroy(BYTE* self, void*)
{
    CallThis(FUN_5f4a90, self + 56);
    CallThis(FUN_5f1a70, self);
}

// [FUN_5f0810]
void __fastcall Node5f0810_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl6528d8;
    operator delete(F(void*, self, 80));
    CallThis(FUN_5f1a70, self);
}

// [FUN_5faa60]
void __fastcall Node5faa60_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652924;
    operator delete(F(void*, self, 56));
    operator delete(F(void*, self, 60));
    if (F(void*, self, 52))
        operator delete(F(void*, self, 52));
    CallThis(FUN_5ef660, self);
}

// [FUN_5fcc70]
void __fastcall Node5fcc70_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652d68;
    operator delete(F(void*, self, 64));
    CallThis(FUN_5f1a70, self);
}

}

// ---------------------------------------------------------------------------
// [FUN_5ef800] counts a test call; returns !(flag 1 of +40)
extern "C" int __fastcall Scene_TestInfo(BYTE* self, void*, int, int)
{
    g_testInfoCount++;
    return ~F(BYTE, self, 40) & 1;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_604a80(); KING_ALIAS("_FUN_604a80", "_Mixer_Init")
extern "C" void FUN_604730(); KING_ALIAS("_FUN_604730", "_Sound_ErrorText")
extern "C" void $L_5fcdc0(); KING_ALIAS("_$L_5fcdc0", "_Slots723aa8_Destroy")
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")    // reads an int from a file
ASM_VAR(int, g_sc723b28, $L_723b28)
ASM_VAR(int, g_sc723b30, $L_723b30)
ASM_VAR(int, g_sc723b34, $L_723b34)
ASM_VAR(int, g_sc723b38, $L_723b38)
ASM_VAR(AsmLabel, g_sc723aa8, $L_723aa8)
ASM_VAR(CRITICAL_SECTION, g_sc723b40, $L_723b40)
extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);

// [FUN_5fcdf0] starts the sound system once; returns the error text of a failure, else 0
extern "C" const char* __cdecl Sound_Start()
{
    if (g_sc723b28)
        return 0;
    g_sc723b28 = 1;
    g_sc723b30 = 0;
    g_sc723b34 = 0;
    int r = CallC<int>(FUN_604a80, 10, $L_5fcdc0);
    g_sc723b38 = 0;
    memset(g_sc723aa8, 0, 128);
    if (!r)
        return 0;
    return CallC<const char*>(FUN_604730, r);
}

// [FUN_5f92b0] reads three ints and the third's count of ints (+12, new) from a file
extern "C" void __fastcall Ints3_Read(BYTE* self, void*, FILE* f)
{
    F(int, self, 0) = CallC<int>(FUN_5ab140, f);
    F(int, self, 4) = CallC<int>(FUN_5ab140, f);
    F(int, self, 8) = CallC<int>(FUN_5ab140, f);
    if (F(int, self, 8))
    {
        void* p = operator new(F(int, self, 8) * 4);
        F(void*, self, 12) = p;
        fread(p, 4, F(int, self, 8), f);
    }
}

// [FUN_5f1cc0] puts a node at the head of bucket i (+48, count +52) of this table; 0 when i is out of range
extern "C" int __fastcall Buckets_Push(BYTE* self, void*, int i, BYTE* n)
{
    if (i >= F(int, self, 52))
        return 0;
    F(BYTE*, n, 44) = self;
    F(int, n, 36) = F(int, self, 36);
    BYTE** slot = F(BYTE**, self, 48) + i;
    BYTE* head = *slot;
    if (!head)
    {
        *slot = n;
        F(BYTE*, n, 8) = 0;
        F(BYTE*, n, 4) = 0;
        return 1;
    }
    F(BYTE*, n, 8) = head;
    F(BYTE*, n, 4) = 0;
    F(BYTE*, head, 4) = n;
    F(BYTE**, self, 48)[i] = n;
    return 1;
}

// [FUN_5fcdb0] resets +20 and +24 (-1)
extern "C" void __fastcall Sound_ResetSlot(BYTE* self, void*)
{
    F(int, self, 20) = 0;
    F(int, self, 24) = -1;
}

// [FUN_5ef820] whether bit 0 of +40 is clear (4 arguments ignored)
extern "C" int __fastcall Obj_Flag0Clear(BYTE* self, void*, int, int, int, int)
{
    return ~F(BYTE, self, 40) & 1;
}

// [FUN_5ff220] initializes the sound lock
extern "C" void __cdecl Sound_InitLock() { InitializeCriticalSection(&g_sc723b40); }


// ---------------------------------------------------------------------------
extern "C" void FUN_5ef800(); KING_ALIAS("_FUN_5ef800", "@Scene_TestInfo@16")
ASM_PROC(FUN_5b4fc0)
extern "C" void FUN_604e10(); KING_ALIAS("_FUN_604e10", "_Channels_Release")
extern "C" void FUN_5fd250(); KING_ALIAS("_FUN_5fd250", "@SoundFile_Destroy@8")
extern "C" void FUN_605050(); KING_ALIAS("_FUN_605050", "_Mixer_Shutdown")
extern "C" void FUN_5e6800(); KING_ALIAS("_FUN_5e6800", "@Outline_BoundingSphere@12")
extern "C" void FUN_5e5580(); KING_ALIAS("_FUN_5e5580", "@Sphere_Merge@12")
ASM_VAR(BYTE**, g_sc723b34p, $L_723b34)
struct ScVec { float x, y, z; };
typedef int (__cdecl* ScTest)(int, BYTE*, BYTE*);

// [FUN_5f3570] tests (a, b) against this object: 5ef800, then b's 5b4fc0 with +20, then the test hook +48 (a, b, this); 0 when one fails
extern "C" int __fastcall Obj_Test(BYTE* self, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, self, a, b))
        return 0;
    if (!CallThis<int>(FUN_5b4fc0, b, self + 20))
        return 0;
    ScTest t = F(ScTest, self, 48);
    if (!t)
        return 0;
    return t(a, b, self);
}

// [FUN_5fcf10] stops the sound system: frees the sounds (723b34, count 723b30) and the tables
extern "C" void __cdecl Sound_Stop()
{
    if (!g_sc723b28)
        return;
    g_sc723b28 = 0;
    CallC(FUN_604e10);
    for (int i = 0; i < g_sc723b30; i++)
    {
        BYTE* p = g_sc723b34p[i];
        if (p)
        {
            CallThis(FUN_5fd250, p);
            operator delete(p);
        }
    }
    operator delete(g_sc723b34p);
    memset(g_sc723aa8, 0, 128);
    CallC(FUN_605050);
}

// [FUN_5fa5f0] moves the chain of n (+48) segments (40 bytes each, at +52) to the point +56: the first to it, the others following
extern "C" void __fastcall Chain_MoveTo(BYTE* self, void*)
{
    ScVec t = {0, 0, 0};
    if (F(int, self, 48) < 1)
        return;
    F(ScVec, self, 20) = F(ScVec, self, 56);
    CallThis(FUN_5e6800, F(BYTE*, self, 52), self + 20);
    for (int i = 1; i < F(int, self, 48); i++)
    {
        t = F(ScVec, self, 56);
        CallThis(FUN_5e6800, F(BYTE*, self, 52) + i * 40, &t);
        CallThis(FUN_5e5580, self + 20, &t);
    }
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")

// [FUN_5ef780] removes this node from its bucket table (+44) or, when linked (+4), from its list; 0 when it isn't in one
extern "C" int __fastcall Node_Remove(BYTE* self, void*)
{
    if (F(BYTE*, self, 4))
    {
        CallThis(FUN_5d21b0, self);
        F(int, self, 44) = 0;
        F(int, self, 36) = 0;
        return 1;
    }
    BYTE* t = F(BYTE*, self, 44);
    if (!t)
        return 0;
    int n = F(int, t, 52), i = 0;
    for (; i < n; i++)
        if (F(BYTE**, t, 48)[i] == self)
            break;
    if (i == n)
        return 0;
    F(BYTE**, t, 48)[i] = F(BYTE*, self, 8);
    if (F(BYTE*, self, 8))
        F(BYTE*, F(BYTE*, self, 8), 4) = 0;
    F(BYTE*, self, 4) = 0;
    F(BYTE*, self, 8) = 0;
    F(BYTE*, self, 44) = 0;
    return 1;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5f2080(); KING_ALIAS("_FUN_5f2080", "@Slots_Call48@20")
extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020", "@Material_Select@24")
ASM_PROC(FUN_5af9e0)
extern "C" void FUN_5b1950(); KING_ALIAS("_FUN_5b1950", "@Rn_DrawStrided@24")
extern "C" void FUN_5b1df0(); KING_ALIAS("_FUN_5b1df0", "@Rn_DrawIndexedStrided@24")
ASM_VAR(DWORD, g_sc702e8c, $L_702e8c)
ASM_VAR(BYTE*, g_scDevice, $L_720908)
ASM_VAR(AsmLabel, s_sc683640, $L_683640)
ASM_VAR(int, g_sc721770, $L_721770)
ASM_VAR(BYTE*, g_sc723a90, $L_723a90)
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);

// a device render state
static void Sc_RenderState(int s, int v)
{
    ((long(__stdcall*)(BYTE*, int, int))(*(void***)g_scDevice)[20])(g_scDevice, s, v);
}

// [FUN_5f36d0] draws the object when it passes the tests; names starting with 683640 are drawn with states 14 = 0 and 23 = 8
extern "C" int __fastcall Obj_DrawTested(BYTE* self, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, self, a, b))
        return 0;
    if (!CallThis<int>(FUN_5b4fc0, b, self + 20))
        return 0;
    int swap = 0;
    if (F(char*, self, 12) && (g_sc702e8c & 0x4000) && !strncmp(F(char*, self, 12), s_sc683640, 3))
    {
        g_sc702e8c &= ~0x4000u;
        swap = 1;
        Sc_RenderState(14, 0);
        Sc_RenderState(23, 8);
    }
    CallThis(FUN_5f2080, self, 0, a, b);
    if (swap)
    {
        g_sc702e8c |= 0x4000;
        Sc_RenderState(14, 1);
        Sc_RenderState(23, 4);
    }
    return 1;
}

struct ScVec3 { DWORD x, y, z; };

// [FUN_5f1620] draws a mesh: prepares the material (5b1020), transforms (5af9e0) and draws its strided vertices, or indexed with the shared vertices 723a90
extern "C" long __cdecl Mesh_Draw(BYTE* r, BYTE* mesh, BYTE* mat, ScVec3* p, int prim)
{
    if (!F(int, mesh, 32))
    {
        CallThis(FUN_5b1020, r, mat, F(int, mesh, 24) != 0, g_sc721770 ? 2 : 0, 0);
        CallThis(FUN_5af9e0, r, mesh, (int)F(WORD, mesh, 0), mat, *p);
        return CallThis<long>(FUN_5b1950, r, mat, prim, mesh, (int)F(WORD, mesh, 0));
    }
    BYTE* v = g_sc723a90;
    if (!v)
        return 0;
    CallThis(FUN_5b1020, r, mat, (F(DWORD, v, 8) >> 4) & 1, g_sc721770 ? 3 : 0, 0);
    CallThis(FUN_5af9e0, r, v, (int)F(WORD, v, 0), mat, *p);
    return CallThis<long>(FUN_5b1df0, r, mat, prim, mesh, g_sc723a90);
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
extern "C" void FUN_5f06e0(); KING_ALIAS("_FUN_5f06e0", "@Node5f06e0_Destroy@8")
extern "C" void FUN_5f07a0(); KING_ALIAS("_FUN_5f07a0", "@Node5f07a0_Destroy@8")
extern "C" void FUN_5f0810(); KING_ALIAS("_FUN_5f0810", "@Node5f0810_Destroy@8")
extern "C" void FUN_5f08c0(); KING_ALIAS("_FUN_5f08c0", "@Sce5f08c0_Destruct@8")
extern "C" void FUN_5f15a0(); KING_ALIAS("_FUN_5f15a0", "@Node5f1530_Destroy@8")
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
extern "C" void FUN_5fa720(); KING_ALIAS("_FUN_5fa720", "@Sce5fa720_Destroy@4")
extern "C" void FUN_5faa60(); KING_ALIAS("_FUN_5faa60", "@Node5faa60_Destroy@8")
extern "C" void FUN_5fcc70(); KING_ALIAS("_FUN_5fcc70", "@Node5fcc70_Destroy@8")

static BYTE* Sce_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5ef6b0]
extern "C" BYTE* __fastcall Sce5ef6b0_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5ef660); }
// [$L_5f06c0]
extern "C" BYTE* __fastcall Sce5f06c0_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5f06e0); }
// [$L_5f0780]
extern "C" BYTE* __fastcall Sce5f0780_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5f07a0); }
// [$L_5f07f0]
extern "C" BYTE* __fastcall Sce5f07f0_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5f0810); }
// [$L_5f0880]
extern "C" BYTE* __fastcall Sce5f0880_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5faa60); }
// [$L_5f08a0]
extern "C" BYTE* __fastcall Sce5f08a0_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5f08c0); }
// [$L_5f1510]
extern "C" BYTE* __fastcall Sce5f1510_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5f15a0); }
// [$L_5f1a50]
extern "C" BYTE* __fastcall Sce5f1a50_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5f1a70); }
// [$L_5fa6d0]
extern "C" BYTE* __fastcall Sce5fa6d0_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5fa720); }
// [$L_5fc6c0]
extern "C" BYTE* __fastcall Sce5fc6c0_DeletingDestructor(BYTE* s, void*, int flags) { return Sce_DeleteAfter(s, flags, FUN_5fcc70); }

ASM_PROC(FUN_5f21e0)
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")
ASM_PROC(FUN_5f1db0)
extern "C" void FUN_5ac7f0(); KING_ALIAS("_FUN_5ac7f0", "@Plane_Read@12")
extern "C" void FUN_5f8bf0(); KING_ALIAS("_FUN_5f8bf0", "@Shape_ReadData@12")
extern "C" void FUN_5ef800(); KING_ALIAS("_FUN_5ef800", "@Scene_TestInfo@16")
extern "C" void FUN_5f2080(); KING_ALIAS("_FUN_5f2080", "@Slots_Call48@20")
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5ab790(); KING_ALIAS("_FUN_5ab790", "@Vec3_Read@12")

// [$L_5fa5d0] the vector at +56 to out
extern "C" int __fastcall Sce_Get56(BYTE* s, void*, DWORD* out)
{
    out[0] = F(DWORD, s, 56);
    out[1] = F(DWORD, s, 60);
    out[2] = F(DWORD, s, 64);
    return 1;
}
// [$L_5fab30] the vector at +56 from v
extern "C" int __fastcall Sce_Set56(BYTE* s, void*, const DWORD* v)
{
    F(DWORD, s, 56) = v[0];
    F(DWORD, s, 60) = v[1];
    F(DWORD, s, 64) = v[2];
    return 1;
}
// [$L_5fada0] the vector at +84 to out
extern "C" int __fastcall Sce_Get84(BYTE* s, void*, DWORD* out)
{
    out[0] = F(DWORD, s, 84);
    out[1] = F(DWORD, s, 88);
    out[2] = F(DWORD, s, 92);
    return 1;
}

// [$L_5fb7b0] FUN_5f21e0 (0, a, b) unless hidden (+40 bit 0)
extern "C" int __fastcall Sce_5fb7b0(BYTE* s, void*, int a, int b)
{
    if (F(BYTE, s, 40) & 1)
        return 0;
    return CallThis<int>(FUN_5f21e0, s, 0, a, b);
}

// [$L_5f96e0] FUN_5f21e0 with the count (+56, or +56 of the source at +60) limited to +52
extern "C" int __fastcall Sce_5f96e0(BYTE* s, void*, int a, int b)
{
    if (F(BYTE, s, 40) & 1)
        return 0;
    int n = F(int, s, 56);
    if (F(BYTE*, s, 60))
        n = F(int, F(BYTE*, s, 60), 56);
    if (n > F(int, s, 52))
        n = F(int, s, 52);
    return CallThis<int>(FUN_5f21e0, s, n, a, b);
}

// the node's part of a load: FUN_5f1db0 (file, FUN_5ab140 (file), kind, b, c)
static int Sce_LoadTail(BYTE* s, int file, int kind, int b, int c)
{
    int r = CallC<int>(FUN_5ab140, file);
    return CallThis<int>(FUN_5f1db0, s, file, r, kind, b, c);
}
// [$L_5fca20]
extern "C" int __fastcall Sce_Load5fca20(BYTE* s, void*, int f, int b, int c) { return Sce_LoadTail(s, f, 1, b, c); }
// [$L_5fb510]
extern "C" int __fastcall Sce_Load5fb510(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    return Sce_LoadTail(s, f, 1, b, c);
}
// [$L_5f4960]
extern "C" int __fastcall Sce_Load5f4960(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    CallThis(FUN_5ac7f0, s + 56, f);
    return Sce_LoadTail(s, f, 3, b, c);
}
// [$L_5f8ed0]
extern "C" int __fastcall Sce_Load5f8ed0(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    CallThis(FUN_5f8bf0, s + 56, f);
    return Sce_LoadTail(s, f, 2, b, c);
}

// [$L_5faad0] reads +20, the flag/count word (bit 0 clear sets +48, the rest to +52), +56 and +68
extern "C" int __fastcall Sce_Load5faad0(BYTE* s, void*, int f, int, int)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    int r = CallC<int>(FUN_5ab140, f);
    F(int, s, 48) = (r & 1) ? 0 : 1;
    F(int, s, 52) = r >> 1;
    CallThis(FUN_5ab790, s + 56, f);
    F(int, s, 68) = CallC<int>(FUN_5ab140, f);
    return 1;
}

// [$L_5fa4b0]
extern "C" int __fastcall Sce_5fa4b0(BYTE* s, void*, int a, int b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    CallThis(FUN_5f2080, s, 0, a, b);
    return 1;
}

// [$L_5f9720] after FUN_5ef780: the object at +76 gets vtbl+36, is deleted and cleared
extern "C" int __fastcall Sce_5f9720(BYTE* s)
{
    int r = CallThis<int>(FUN_5ef780, s);
    if (!r)
        return r;
    if (F(BYTE*, s, 76)) {
        CallVirt(F(BYTE*, s, 76), 36);
        if (F(BYTE*, s, 76))
            CallVirt(F(BYTE*, s, 76), 0, 1);
        F(int, s, 76) = 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Virtual methods of the scene nodes: event handling, drawing passes, loading.

extern "C" void FUN_5ef820(); KING_ALIAS("_FUN_5ef820", "@Obj_Flag0Clear@24")
extern "C" void FUN_5f2100(); KING_ALIAS("_FUN_5f2100", "@Slots_Call56@28")
extern "C" void FUN_5b4cd0(); KING_ALIAS("_FUN_5b4cd0", "@Draw_ListItem@32")
extern "C" void FUN_5aba10(); KING_ALIAS("_FUN_5aba10", "@Matrix_Read@12")
ASM_PROC(FUN_5d2650)
extern "C" void FUN_5ab190(); KING_ALIAS("_FUN_5ab190", "_File_ReadFloat")
extern "C" void FUN_5d2fa0(); KING_ALIAS("_FUN_5d2fa0", "_Time_NowMs")
extern "C" void FUN_5d2fc0(); KING_ALIAS("_FUN_5d2fc0", "_Time_Delta")
extern "C" void FUN_5ae560(); KING_ALIAS("_FUN_5ae560", "@Render_CurrentFrame@8")
extern "C" void FUN_5f88d0(); KING_ALIAS("_FUN_5f88d0", "@Volume_Contains@12")
extern "C" void FUN_5ad180(); KING_ALIAS("_FUN_5ad180", "@Renderer_PushLight@12")
extern "C" void FUN_5adc20(); KING_ALIAS("_FUN_5adc20", "@Render_PopLight@12")
extern "C" void FUN_5e6520(); KING_ALIAS("_FUN_5e6520", "@Poly_BoundingSphere@16")
extern "C" void FUN_5e5580(); KING_ALIAS("_FUN_5e5580", "@Sphere_Merge@12")
extern "C" void FUN_5f92b0(); KING_ALIAS("_FUN_5f92b0", "@Ints3_Read@12")
extern "C" void FUN_5b4f70(); KING_ALIAS("_FUN_5b4f70", "@Clip_InFront@12")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")
ASM_PROC(FUN_5b4fc0)
extern "C" void Sce_fread(); KING_ALIAS("_Sce_fread", "_fread")
extern "C" void Sce_strncmp(); KING_ALIAS("_Sce_strncmp", "_strncmp")
ASM_VAR(AsmLabel, g_sce71e820, $L_71e820)
ASM_VAR(int, g_sce723aa4, $L_723aa4)
ASM_VAR(AsmLabel, g_sce71f800, $L_71f800)
ASM_VAR(AsmLabel, g_sce71f810, $L_71f810)
ASM_VAR(AsmLabel, s_sce67fb20, $L_67fb20)
ASM_VAR(AsmLabel, g_sce723b58, $L_723b58)
ASM_VAR(DWORD, g_sce723c30, $L_723c30)
ASM_VAR(DWORD, g_sce723c34, $L_723c34)
ASM_VAR(DWORD, g_sce723c38, $L_723c38)
ASM_VAR(void*, g_sce720908, $L_720908)

typedef void (*AsmFn)();

// a 32-byte name from the file, looked up in the name table 71e820 into *out
static void Sce_ReadName(void* f, void* out)
{
    char buf[64];
    CallC(Sce_fread, buf, 32, 1, f);
    CallThis(FUN_5d2650, g_sce71e820, buf, out);
}

// the children (+48 list, next +8) get the event through vtbl+28; unhandled it goes to vtbl+20 (0, ev)
static int Sce_PassToChildren(BYTE* s, int ev)
{
    for (BYTE* o = *F(BYTE**, s, 48); o; o = F(BYTE*, o, 8))
        if (CallVirt<int>(o, 28, ev))
            return 1;
    CallVirt(s, 20, 0, ev);
    return 1;
}
// [$L_5f3ef0]
extern "C" int __fastcall Sce_5f3ef0(BYTE* s, void*, int ev) { return Sce_PassToChildren(s, ev); }
// [$L_5f3a10] not in state 1 (+140)
extern "C" int __fastcall Sce_5f3a10(BYTE* s, void*, int ev) { return F(int, s, 140) == 1 ? 0 : Sce_PassToChildren(s, ev); }
// [$L_5f3800] not with +56 set
extern "C" int __fastcall Sce_5f3800(BYTE* s, void*, int ev) { return F(int, s, 56) ? 0 : Sce_PassToChildren(s, ev); }

// [$L_5fab50] after FUN_5ef800: FUN_5b4cd0 of b with +48, +52, the vector +56 and +68
extern "C" int __fastcall Sce_5fab50(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    CallThis(FUN_5b4cd0, b, F(int, s, 48), F(int, s, 52), F(DWORD, s, 56), F(DWORD, s, 60), F(DWORD, s, 64), F(int, s, 68));
    return 1;
}

// after FUN_5ef820 (a, b, c, d): FUN_5f2100 (k, a, b, c, d)
static int Sce_Pass4(BYTE* s, int k, int a, int b, int c, int d)
{
    if (!CallThis<int>(FUN_5ef820, s, a, b, c, d))
        return 0;
    CallThis(FUN_5f2100, s, k, a, b, c, d);
    return 1;
}
// [$L_5f37b0]
extern "C" int __fastcall Sce_5f37b0(BYTE* s, void*, int a, int b, int c, int d) { return Sce_Pass4(s, 0, a, b, c, d); }
// [$L_5f2790]
extern "C" int __fastcall Sce_5f2790(BYTE* s, void*, int a, int b, int c, int d) { return Sce_Pass4(s, 0, a, b, c, d); }
// [$L_5fa310] the last slot (+52 - 1)
extern "C" int __fastcall Sce_5fa310(BYTE* s, void*, int a, int b, int c, int d)
{
    if (!CallThis<int>(FUN_5ef820, s, a, b, c, d))
        return 0;
    CallThis(FUN_5f2100, s, F(int, s, 52) - 1, a, b, c, d);
    return 1;
}

// the current slot: +56, or +56 of the source at +60, at most +52
static int Sce_CurrentSlot(BYTE* s)
{
    int n = F(int, s, 56);
    if (F(BYTE*, s, 60))
        n = F(int, F(BYTE*, s, 60), 56);
    if (n > F(int, s, 52))
        n = F(int, s, 52);
    return n;
}
// [$L_5f9680]
extern "C" int __fastcall Sce_5f9680(BYTE* s, void*, int a, int b, int c, int d)
{
    if (!CallThis<int>(FUN_5ef820, s, a, b, c, d))
        return 0;
    CallThis(FUN_5f2100, s, Sce_CurrentSlot(s), a, b, c, d);
    return 1;
}
// [$L_5f9630]
extern "C" int __fastcall Sce_5f9630(BYTE* s, void*, int a, int b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    CallThis(FUN_5f2080, s, Sce_CurrentSlot(s), a, b);
    return 1;
}

// [$L_5f48f0] slots 0, 1 and 2
extern "C" int __fastcall Sce_5f48f0(BYTE* s, void*, int a, int b, int c, int d)
{
    if (!CallThis<int>(FUN_5ef820, s, a, b, c, d))
        return 0;
    CallThis(FUN_5f2100, s, 0, a, b, c, d);
    CallThis(FUN_5f2100, s, 1, a, b, c, d);
    CallThis(FUN_5f2100, s, 2, a, b, c, d);
    return 1;
}

// [$L_5f39a0] slot 0; in state 1 (+140) with +136 then FUN_5ad180 of d with +56
extern "C" int __fastcall Sce_5f39a0(BYTE* s, void*, int a, int b, int c, BYTE* d)
{
    if (!CallThis<int>(FUN_5ef820, s, a, b, c, d))
        return 0;
    CallThis(FUN_5f2100, s, 0, a, b, c, d);
    if (F(int, s, 140) == 1 && F(int, s, 136))
        CallThis(FUN_5ad180, d, s + 56);
    return 1;
}

// [$L_5f8f70] slot 0 when FUN_5f88d0 of +56 accepts +36 of the record FUN_5ac2e0 gives for b, else slot 1
extern "C" int __fastcall Sce_5f8f70(BYTE* s, void*, int a, int b, int c, int d)
{
    if (!CallThis<int>(FUN_5ef820, s, a, b, c, d))
        return 0;
    BYTE loc[48];
    BYTE* r = CallC<BYTE*>(FUN_5ac2e0, loc, b);
    int k = CallThis<int>(FUN_5f88d0, s + 56, r + 36) ? 0 : 1;
    CallThis(FUN_5f2100, s, k, a, b, c, d);
    return 1;
}

// [$L_5f8f10] the same choice for FUN_5ae560 of b, with FUN_5f2080
extern "C" int __fastcall Sce_5f8f10(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    int x = CallThis<int>(FUN_5ae560, b);
    int k = CallThis<int>(FUN_5f88d0, s + 56, x) ? 0 : 1;
    CallThis(FUN_5f2080, s, k, a, b);
    return 1;
}

// [$L_5f9380] b's vtbl+0 with +48, without flag 2 and +64
extern "C" int __fastcall Sce_5f9380(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    if (a & 2)
        return 0;
    if (F(int, s, 64))
        return 0;
    CallVirt(b, 0, s + 48);
    return 1;
}

// [$L_5f3920] not in a state (+140); with +136 the effect at +56 is applied around slot 0
extern "C" int __fastcall Sce_5f3920(BYTE* s, void*, int a, BYTE* b)
{
    if (F(int, s, 140))
        return 0;
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    if (F(int, s, 136))
        CallThis(FUN_5ad180, b, s + 56);
    CallThis(FUN_5f2080, s, 0, a, b);
    if (F(int, s, 136))
        CallThis(FUN_5adc20, b, s + 56);
    return 1;
}

// [$L_5f4880] FUN_5b4f70 of b with +56 decides the order of slots 0 and 2; slot 1 last
extern "C" int __fastcall Sce_5f4880(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    int r = CallThis<int>(FUN_5b4f70, b, s + 56);
    int first = (r ^ (a & 1)) ? 0 : 2;
    CallThis(FUN_5f2080, s, first, a, b);
    CallThis(FUN_5f2080, s, 2 - first, a, b);
    CallThis(FUN_5f2080, s, 1, a, b);
    return 1;
}

// [$L_5fccd0] FUN_5f21e0 inside the lock 71f800 (count 723aa4)
extern "C" int __fastcall Sce_5fccd0(BYTE* s, void*, int a, int b)
{
    if (F(BYTE, s, 40) & 1)
        return 0;
    if (!g_sce723aa4)
        CallC(FUN_5d2fa0, (BYTE*)g_sce71f800);
    g_sce723aa4++;
    int r = CallThis<int>(FUN_5f21e0, s, 0, a, b);
    if (--g_sce723aa4 == 0)
        CallC(FUN_5d2fc0, (BYTE*)g_sce71f800, (BYTE*)g_sce71f810);
    return r;
}

// [$L_5f35c0] an event with flag 4: to the target (+56, vtbl+52, flag 1 added) or the handler (+52)
extern "C" int __fastcall Sce_5f35c0(BYTE* s, void*, int a, int b)
{
    if (F(BYTE, s, 40) & 1)
        return 0;
    if (!(a & 4))
        return 0;
    BYTE* o = F(BYTE*, s, 56);
    if (o) {
        if (a & 1)
            return 0;
        return CallVirt<int>(o, 52, a | 1, b);
    }
    AsmFn fn = F(AsmFn, s, 52);
    if (fn)
        return CallC<int>(fn, a, b, s);
    return 0;
}

// an event: flag 1 or no target goes to FUN_5f21e0; with flag 4 the target gets it, and when it
// takes it, b's +116 gets the value at vo
static int Sce_EventToTarget(BYTE* s, int a, BYTE* b, int to, int vo)
{
    if (F(BYTE, s, 40) & 1)
        return 0;
    if (!(a & 1)) {
        BYTE* o = F(BYTE*, s, to);
        if (o && (a & 4)) {
            int r = CallVirt<int>(o, 52, a | 1, b);
            if (r & 1)
                F(int, b, 116) = F(int, s, vo);
            return r;
        }
    }
    return CallThis<int>(FUN_5f21e0, s, 0, a, b);
}
// [$L_5f2e00]
extern "C" int __fastcall Sce_5f2e00(BYTE* s, void*, int a, BYTE* b) { return Sce_EventToTarget(s, a, b, 104, 112); }
// [$L_5f38b0]
extern "C" int __fastcall Sce_5f38b0(BYTE* s, void*, int a, BYTE* b) { return Sce_EventToTarget(s, a, b, 56, 60); }

// [$L_5f0a80] the bounds (+20) of the parts (+56, 88 bytes each, count +52 word) under the matrix +48
extern "C" void __fastcall Sce_Bounds(BYTE* s)
{
    DWORD loc[4] = {0, 0, 0, 0};
    if (F(WORD, s, 52) < 1)
        return;
    CallThis(FUN_5e6520, F(BYTE*, s, 56), s + 20, F(int, s, 48));
    for (int i = 1; i < F(WORD, s, 52); i++) {
        CallThis(FUN_5e6520, F(BYTE*, s, 56) + i * 88, loc, F(int, s, 48));
        CallThis(FUN_5e5580, s + 20, loc);
    }
}

// [$L_5f2640] loads +64 and the on flag (+56), then the node part
extern "C" int __fastcall Sce_Load5f2640(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5aba10, s + 64, f);
    int r = CallC<int>(FUN_5ab140, f);
    if (r == 0)
        F(int, s, 56) = 0;
    else if (r == 1)
        F(int, s, 56) = 1;
    return Sce_LoadTail(s, f, 1, b, c);
}
// [$L_5f3850] +20 and the name +56
extern "C" int __fastcall Sce_Load5f3850(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    Sce_ReadName((void*)f, s + 56);
    return Sce_LoadTail(s, f, 1, b, c);
}
// [$L_5f4340] +20 and the names +60, +56
extern "C" int __fastcall Sce_Load5f4340(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    Sce_ReadName((void*)f, s + 60);
    Sce_ReadName((void*)f, s + 56);
    return Sce_LoadTail(s, f, 1, b, c);
}
// [$L_5f9850] +20 and the names +68, +64
extern "C" int __fastcall Sce_Load5f9850(BYTE* s, void*, int f, int, int)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    Sce_ReadName((void*)f, s + 68);
    Sce_ReadName((void*)f, s + 64);
    return 1;
}
// [$L_5f3620] a name starting with 67fb20 goes to +52, others to +56; then the name +48 and +60
extern "C" int __fastcall Sce_Load5f3620(BYTE* s, void*, int f, int, int)
{
    char buf[64];
    CallThis(FUN_5ac7f0, s + 20, f);
    CallC(Sce_fread, buf, 32, 1, f);
    if (CallC<int>(Sce_strncmp, buf, (const char*)s_sce67fb20, 2) == 0) {
        CallThis(FUN_5d2650, g_sce71e820, buf, s + 52);
        F(int, s, 56) = 0;
    } else {
        CallThis(FUN_5d2650, g_sce71e820, buf, s + 56);
        F(int, s, 52) = 0;
    }
    Sce_ReadName((void*)f, s + 48);
    CallThis(FUN_5f92b0, s + 60, f);
    return 1;
}
// [$L_5f4030] +20, the doubles +72, +56, +64 (a word skipped) and +96
extern "C" int __fastcall Sce_Load5f4030(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    F(double, s, 72) = CallC<double>(FUN_5ab190, f);
    CallC(FUN_5ab140, f);
    F(double, s, 56) = CallC<double>(FUN_5ab190, f);
    F(double, s, 64) = CallC<double>(FUN_5ab190, f);
    F(int, s, 96) = CallC<int>(FUN_5ab140, f);
    return Sce_LoadTail(s, f, 1, b, c);
}
// [$L_5f9300] by the kind (+64): 6 and 7 read +48 first, 7 also +20 and +68
extern "C" int __fastcall Sce_Load5f9300(BYTE* s, void*, int f, int, int)
{
    int t = F(int, s, 64);
    if (t == 6 || t == 7) {
        if (t == 7) {
            CallThis(FUN_5ac7f0, s + 20, f);
            F(int, s, 68) = CallC<int>(FUN_5ab140, f);
        }
        CallThis(FUN_5f92b0, s + 48, f);
        CallThis(FUN_5f8bf0, s + 64, f);
    } else {
        CallThis(FUN_5ac7f0, s + 20, f);
        CallThis(FUN_5f8bf0, s + 64, f);
        CallThis(FUN_5f92b0, s + 48, f);
    }
    return 1;
}
// [$L_5fadc0] the light: words +52, +48, +56, the name +80, +84, +96 and the floats +64..+76, +60
extern "C" int __fastcall Sce_Load5fadc0(BYTE* s, void*, int f, int, int)
{
    F(int, s, 52) = CallC<int>(FUN_5ab140, f);
    F(int, s, 48) = CallC<int>(FUN_5ab140, f);
    F(int, s, 56) = CallC<int>(FUN_5ab140, f);
    Sce_ReadName((void*)f, s + 80);
    CallThis(FUN_5ab790, s + 84, f);
    CallThis(FUN_5ab790, s + 96, f);
    F(float, s, 64) = (float)CallC<double>(FUN_5ab190, f);
    F(float, s, 68) = (float)CallC<double>(FUN_5ab190, f);
    F(float, s, 72) = (float)CallC<double>(FUN_5ab190, f);
    F(float, s, 76) = (float)CallC<double>(FUN_5ab190, f);
    F(float, s, 60) = (float)CallC<double>(FUN_5ab190, f);
    return 1;
}

// [$L_5ff160] static initializer: the subtractive random generator (55 words at 723b58)
extern "C" void __cdecl SceInit_5ff160()
{
    DWORD* a = (DWORD*)g_sce723b58;
    for (int i = 0; i < 55; i++)
        a[i] = 0;
    DWORD mj = 161803398, mk = 1;
    g_sce723c30 = mj;
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
    g_sce723c34 = 0;
    g_sce723c38 = 31;
}

// [$L_5f1d20] links node into slot i (+48, count +52), after parent or at the end of the slot's chain
extern "C" BYTE* __fastcall Sce_LinkChild(BYTE* s, void*, int i, BYTE* node, BYTE* parent)
{
    if (i >= F(int, s, 52))
        return 0;
    if (parent) {
        F(int, node, 44) = F(int, parent, 44);
        CallThis(FUN_5d2190, node, parent);
        F(int, node, 36) = F(int, parent, 36);
        return node;
    }
    F(BYTE*, node, 44) = s;
    F(int, node, 36) = F(int, s, 36);
    BYTE** slot = &F(BYTE**, s, 48)[i];
    if (!*slot) {
        *slot = node;
        F(int, node, 8) = 0;
        F(int, node, 4) = 0;
        return node;
    }
    BYTE* last = *slot;
    while (F(BYTE*, last, 8))
        last = F(BYTE*, last, 8);
    CallThis(FUN_5d2190, node, last);
    return node;
}

// [$L_5fa9d0] render state 136 from b's +8272, the light (+52, +48) to stage 482
extern "C" int __fastcall Sce_5fa9d0(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    int r = CallThis<int>(FUN_5b4fc0, b, s + 20);
    if (!r)
        return r;
    void* d = g_sce720908;
    void* fn = *(void**)(*(char**)d + 80);
    ((int(__stdcall*)(void*, int, int))fn)(d, 136, F(int, b, 8272) ? 1 : 0);
    if (F(int, s, 52)) {
        d = g_sce720908;
        fn = *(void**)(*(char**)d + 100);
        ((int(__stdcall*)(void*, int, int, int, int, int))fn)(d, 1, 482, F(int, s, 52), F(int, s, 48), 0);
    }
    return 1;
}

// [$L_5fcba0] b's material from the table (+276, count +284) by b's +8660, and the colors +288..+312
extern "C" int __fastcall Sce_5fcba0(BYTE* s, void*, int a, BYTE* b)
{
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    if (F(int, s, 56))
        CallVirt(b, 4, F(int, s, 56));
    int i = F(int, b, 8660);
    if (i >= 0 && i < F(int, s, 284)) {
        int k = F(int, s, 276 + i * 4);
        F(int, b, 8632) = F(int, F(BYTE*, b, 8260), k * 124 + 76);
        F(int, b, 8640) = F(int, b, 8648);
        F(int, b, 8644) = F(int, b, 8652);
        F(float, b, 8672) = (float)F(double, s, 288);
        F(float, b, 8676) = (float)F(double, s, 296);
        F(float, b, 8680) = (float)F(double, s, 304);
        F(float, b, 8684) = (float)F(double, s, 312);
    } else {
        F(int, b, 8632) = 0;
    }
    CallThis(FUN_5f2080, s, 0, a, b);
    return 1;
}

// ---------------------------------------------------------------------------
// Larger scene node methods: loaders of tables, bounds, visitors, planes.

extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
ASM_PROC(FUN_5b3010)
ASM_VAR(float, g_sce64c258, $L_64c258)
ASM_VAR(double, g_sce64b678, $L_64b678)
ASM_VAR(BYTE*, g_sce723a94, $L_723a94)
ASM_VAR(int, g_sce723a50, $L_723a50)
ASM_VAR(int, g_sce723a48, $L_723a48)
ASM_VAR(int, g_sce723a8c, $L_723a8c)
ASM_VAR(BYTE*, g_sce723a90, $L_723a90)

// [$L_5fabb0] +20, the count +72, +56, the flag/count word (+48, +52), +84 and the count's pairs (float +76, word +80)
extern "C" int __fastcall Sce_Load5fabb0(BYTE* s, void*, int f, int, int)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    F(int, s, 72) = CallC<int>(FUN_5ab140, f);
    CallThis(FUN_5ac7f0, s + 56, f);
    int r = CallC<int>(FUN_5ab140, f);
    F(int, s, 48) = (r & 1) ? 0 : 1;
    F(int, s, 52) = r >> 1;
    CallThis(FUN_5ab790, s + 84, f);
    F(void*, s, 76) = operator new(F(int, s, 72) * 4);
    F(void*, s, 80) = operator new(F(int, s, 72) * 4);
    for (int i = 0; i < F(int, s, 72); i++) {
        F(float*, s, 76)[i] = (float)CallC<double>(FUN_5ab190, f);
        F(int*, s, 80)[i] = CallC<int>(FUN_5ab140, f);
    }
    return 1;
}

// [$L_5fa860] +20, a word skipped, the count +48 of 12-byte vectors (+56, zeroed) with words (+60)
extern "C" int __fastcall Sce_Load5fa860(BYTE* s, void*, int f, int, int)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    CallC(FUN_5ab140, f);
    int n = CallC<int>(FUN_5ab140, f);
    F(int, s, 48) = n;
    DWORD* p = (DWORD*)operator new(n * 12);
    if (p)
        for (int i = 0; i < n * 3; i++)
            p[i] = 0;
    F(DWORD*, s, 56) = p;
    F(void*, s, 60) = operator new(F(int, s, 48) * 4);
    for (int i = 0; i < F(int, s, 48); i++) {
        CallThis(FUN_5ab790, F(BYTE*, s, 56) + i * 12, f);
        F(int*, s, 60)[i] = CallC<int>(FUN_5ab140, f);
    }
    return 1;
}

// [$L_5f9e70] +20, the count n and flags (mode 0-2 to +56, flag 8 to +60 bit 0), +64, n floats (+80), then the node part of kind n
extern "C" int __fastcall Sce_Load5f9e70(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    int n = CallC<int>(FUN_5ab140, f);
    int fl = CallC<int>(FUN_5ab140, f);
    int m = fl & 3;
    if (m == 0)
        F(int, s, 56) = 0;
    else if (m == 1)
        F(int, s, 56) = 1;
    else if (m == 2)
        F(int, s, 56) = 2;
    if (fl & 8)
        F(int, s, 60) |= 1;
    CallThis(FUN_5ac7f0, s + 64, f);
    F(void*, s, 80) = operator new(n * 4);
    for (int i = 0; i < n; i++)
        F(float*, s, 80)[i] = (float)CallC<double>(FUN_5ab190, f);
    return Sce_LoadTail(s, f, n, b, c);
}

// [$L_5f1be0] the bounds (+20) of all children: the first child of slot 0 starts them
extern "C" void __fastcall Sce_ChildBounds(BYTE* s)
{
    for (int j = 0; j < F(int, s, 52); j++) {
        for (BYTE* o = F(BYTE**, s, 48)[j]; o; o = F(BYTE*, o, 8)) {
            CallVirt(o, 72);
            if (j == 0 && o == F(BYTE**, s, 48)[0]) {
                F(DWORD, s, 20) = F(DWORD, o, 20);
                F(DWORD, s, 32) = F(DWORD, o, 32);
                F(DWORD, s, 24) = F(DWORD, o, 24);
                F(DWORD, s, 28) = F(DWORD, o, 28);
            } else {
                DWORD box[4] = {F(DWORD, o, 20), F(DWORD, o, 24), F(DWORD, o, 28), F(DWORD, o, 32)};
                CallThis(FUN_5e5580, s + 20, box);
            }
        }
    }
}

// [$L_5fa360] +20; of three vectors the second and third give the middle (+56) and half the length (+68)
extern "C" int __fastcall Sce_Load5fa360(BYTE* s, void*, int f, int b, int c)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    float v1[3] = {0, 0, 0}, v2[3] = {0, 0, 0}, v3[3] = {0, 0, 0};
    CallThis(FUN_5ab790, v1, f);
    CallThis(FUN_5ab790, v2, f);
    CallThis(FUN_5ab790, v3, f);
    double sx = (double)v3[0] + v2[0];
    float sy = (float)((double)v3[1] + v2[1]);
    float sz = (float)((double)v3[2] + v2[2]);
    F(float, s, 56) = (float)(sx * g_sce64c258);
    F(float, s, 60) = (float)((double)sy * g_sce64c258);
    F(float, s, 64) = (float)((double)sz * g_sce64c258);
    float d[3];
    d[0] = (float)((double)v3[0] - v2[0]);
    d[1] = (float)((double)v3[1] - v2[1]);
    d[2] = (float)((double)v3[2] - v2[2]);
    F(float, s, 68) = (float)(CallC<double>(FUN_5ab4d0, d) * g_sce64c258);
    return Sce_LoadTail(s, f, 1, b, c);
}

// [$L_5f26a0] a light: its parameters (+56.., kind +108) are published in 723a.. while slot 0 is drawn
extern "C" int __fastcall Sce_Light(BYTE* s, void*, int a, BYTE* b)
{
    g_sce723a94 = s;
    int r = CallThis<int>(FUN_5ef800, s, a, b);
    if (!r)
        return r;
    r = CallThis<int>(FUN_5b4fc0, b, s + 20);
    if (!r)
        return r;
    F(int, b, 8252) = F(int, s, 60);
    F(int, b, 8256) = F(int, s, 64);
    g_sce723a50 = F(int, s, 108);
    g_sce723a48 = F(int, s, 56);
    int k = F(int, s, 108);
    if (k == 1 || k == 3) {
        g_sce723a8c = F(int, s, 96);
        g_sce723a90 = s + 68;
    } else if (k == 2) {
        g_sce723a8c = F(int, s, 92);
        g_sce723a90 = s + 68;
    } else {
        g_sce723a8c = 0;
        g_sce723a90 = 0;
    }
    if (CallThis<int>(FUN_5b4fc0, b, s + 20))
        CallThis(FUN_5f2080, s, 0, a, b);
    g_sce723a8c = 0;
    g_sce723a48 = 0;
    g_sce723a50 = 0;
    g_sce723a90 = 0;
    g_sce723a94 = 0;
    return 1;
}

// [$L_5f49a0] the viewer's point (b's matrix +4.. times +76..) in front of the plane (+56..+68): slots 0 then 2, else 2 then 0
extern "C" int __fastcall Sce_PlaneOrder(BYTE* s, void*, int a, BYTE* b)
{
    if (F(BYTE, s, 40) & 1)
        return 0;
    int r = CallThis<int>(FUN_5f21e0, s, 1, a, b);
    if (r)
        return r;
    double x = (double)F(float, b, 16) * F(float, b, 80) + (double)F(float, b, 28) * F(float, b, 84);
    x = x + (double)F(float, b, 76) * F(float, b, 4);
    x = x + F(float, b, 40);
    double y = (double)F(float, b, 8) * F(float, b, 76) + (double)F(float, b, 20) * F(float, b, 80);
    y = y + (double)F(float, b, 32) * F(float, b, 84);
    y = y + F(float, b, 44);
    double z = (double)F(float, b, 12) * F(float, b, 76) + (double)F(float, b, 24) * F(float, b, 80);
    z = z + (double)F(float, b, 36) * F(float, b, 84);
    z = z + F(float, b, 48);
    double d = y * F(float, s, 60) + z * F(float, s, 64);
    d = d + x * F(float, s, 56);
    d = d + F(float, s, 68);
    int first = d > g_sce64b678 ? 0 : 2;
    r = CallThis<int>(FUN_5f21e0, s, first, a, b);
    if (r)
        return r;
    return CallThis<int>(FUN_5f21e0, s, 2 - first, a, b);
}

// visits node o with fn: vtbl+32 (fn, x); nodes marked +40 bit 2 are closed (vtbl+36) and deleted
static void Sce_VisitChain(BYTE* o, AsmFn fn, int x)
{
    while (o) {
        BYTE* next = F(BYTE*, o, 8);
        CallVirt(o, 32, fn, x);
        if (F(BYTE, o, 40) & 4) {
            CallVirt(o, 36);
            if (o)
                CallVirt(o, 0, 1);
        }
        o = next;
    }
}
// [$L_5f1b20] fn (self, x) picks a slot, or all slots when negative
extern "C" void __fastcall Sce_Visit(BYTE* s, void*, AsmFn fn, int x)
{
    int k = CallC<int>(fn, s, x);
    if (k >= 0) {
        Sce_VisitChain(F(BYTE**, s, 48)[k], fn, x);
        return;
    }
    for (int j = 0; j < F(int, s, 52); j++)
        Sce_VisitChain(F(BYTE**, s, 48)[j], fn, x);
}

// [$L_5fa790] the 40-byte items (+52, count +48) drawn with the color +56, backwards with flag 1
extern "C" int __fastcall Sce_DrawItems(BYTE* s, void*, int a, BYTE* b)
{
    int r = CallThis<int>(FUN_5ef800, s, a, b);
    if (!r)
        return r;
    r = CallThis<int>(FUN_5b4fc0, b, s + 20);
    if (!r)
        return r;
    int n = F(int, s, 48);
    if (a & 1) {
        for (int i = n - 1; i >= 0; i--)
            CallThis(FUN_5b3010, b, F(BYTE*, s, 52) + i * 40, F(DWORD, s, 56), F(DWORD, s, 60), F(DWORD, s, 64));
    } else {
        for (int i = 0; i < F(int, s, 48); i++)
            CallThis(FUN_5b3010, b, F(BYTE*, s, 52) + i * 40, F(DWORD, s, 56), F(DWORD, s, 60), F(DWORD, s, 64));
    }
    return 1;
}

ASM_VAR(AsmLabel, g_sce652840, $L_652840)
ASM_VAR(AsmLabel, g_sce64d81c, $L_64d81c)

// [FUN_5ef660] (vtable 652840) frees +12
extern "C" void __fastcall Sce5ef660_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_sce652840;
    operator delete(F(void*, s, 12));
    F(void*, s, 0) = g_sce64d81c;
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void Sce_VecDtor(); KING_ALIAS("_Sce_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
ASM_VAR(AsmLabel, g_sce652d10, $L_652d10)
extern "C" AsmLabel g_sce4c47f0; KING_ALIAS("_g_sce4c47f0", "@FxRec_Free@4")

// [FUN_5fa720] (vtable 652d10) deletes its array of 40-byte items (+52)
extern "C" void __fastcall Sce5fa720_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_sce652d10;
    BYTE* a = F(BYTE*, s, 52);
    if (a) {
        CallStd(Sce_VecDtor, a, 40, F(int, a, -4), (BYTE*)g_sce4c47f0);
        operator delete(a - 4);
    }
    CallThis(FUN_5ef660, s);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

extern "C" void FUN_5fd1d0(); KING_ALIAS("_FUN_5fd1d0", "@SoundFile_Construct@12")
extern "C" void FUN_5e9860(); KING_ALIAS("_FUN_5e9860", "_Data_Read5e99a0")
extern "C" void FUN_5e9800(); KING_ALIAS("_FUN_5e9800", "_Data_Read5e9910")
extern "C" void FUN_604e90(); KING_ALIAS("_FUN_604e90", "_Mixer_Load")
ASM_VAR(AsmLabel, g_sce652cb4, $L_652cb4)
ASM_VAR(int, g_sce723b28, $L_723b28)
ASM_VAR(BYTE**, g_sce723b34, $L_723b34)

// [FUN_5f1a70] a node (vtable 652cb4): the children of every slot (+48, count +52) unlinked and deleted
extern "C" void __fastcall Node5f1a70_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_sce652cb4;
    for (int j = 0; j < F(int, s, 52); j++) {
        if (!F(BYTE**, s, 48)[j])
            continue;
        BYTE* c;
        while ((c = F(BYTE*, F(BYTE**, s, 48)[j], 8))) {
            CallThis(FUN_5d21b0, c);
            if (c)
                CallVirt(c, 0, 1);
        }
        if (F(BYTE**, s, 48)[j])
            CallVirt(F(BYTE**, s, 48)[j], 0, 1);
    }
    operator delete(F(void*, s, 48));
    CallThis(FUN_5ef660, s);
}

// [FUN_5fcf80] a sound (id, slot, streamed): made (streamed: a 28-byte stream object) and entered in the table 723b34
extern "C" BYTE* __cdecl Sound_Create(int id, int slot, int streamed)
{
    if (!g_sce723b28)
        return 0;
    BYTE* r;
    if (streamed) {
        BYTE* m = (BYTE*)operator new(28);
        BYTE* o = m ? CallThis<BYTE*>(FUN_5fd1d0, m, id) : 0;
        r = CallC<BYTE*>(FUN_5e9860, o, id);
    } else {
        r = CallC<BYTE*>(FUN_5e9800, id);
    }
    if (!r)
        return 0;
    g_sce723b34[slot] = r;
    F(int, r, 12) = slot;
    CallC(FUN_604e90, slot, F(int, r, 8), F(int, r, 4));
    return r;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_5fd230(); KING_ALIAS("_FUN_5fd230", "@SoundFile_ConstructData@16")
extern "C" void FUN_5e99a0(); KING_ALIAS("_FUN_5e99a0", "_Image_SetData")
extern "C" void FUN_5e9910(); KING_ALIAS("_FUN_5e9910", "_Sound_ReadFile")
extern "C" void Sce_ftell(); KING_ALIAS("_Sce_ftell", "_ftell")

// [FUN_5fd060] a sound from an open file (file, slot, size, streamed): like Sound_Create
extern "C" BYTE* __cdecl Sound_CreateFromFile(void* file, int slot, int size, int streamed)
{
    if (!g_sce723b28)
        return 0;
    BYTE* r;
    if (streamed) {
        BYTE* m = (BYTE*)operator new(28);
        BYTE* o = m ? CallThis<BYTE*>(FUN_5fd230, m, file, CallC<int>(Sce_ftell, file)) : 0;
        r = CallC<BYTE*>(FUN_5e99a0, o, file, size);
    } else {
        r = CallC<BYTE*>(FUN_5e9910, file, size);
    }
    if (!r)
        return 0;
    g_sce723b34[slot] = r;
    F(int, r, 12) = slot;
    CallC(FUN_604e90, slot, F(int, r, 8), F(int, r, 4));
    return r;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
ASM_VAR(AsmLabel, g_q46_722940, $L_722940)
ASM_VAR(AsmLabel, g_q46_723190, $L_723190)
ASM_VAR(AsmLabel, g_q46s_5ef410, $L_5ef410)
ASM_VAR(AsmLabel, g_q46_652a08, $L_652a08)
// [FUN_5ef4d0] constructs the static 722940
extern "C" int __cdecl Static722940_Construct(void)
{
    return CallThis<int>((void (*)())(void*)g_q46s_5ef410, (void*)g_q46_722940);
}
// [FUN_5ef510] constructs the static 723190
extern "C" int __cdecl Static723190_Construct(void)
{
    return CallThis<int>((void (*)())(void*)g_q46s_5ef410, (void*)g_q46_723190);
}
// [FUN_5f08c0] destructor: the 652a08 vtable, then the base
extern "C" void __fastcall Sce5f08c0_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q46_652a08;
    CallThis(FUN_5ef660, s);
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
ASM_VAR(AsmLabel, g_q47_723d28, $L_723d28)
ASM_VAR(AsmLabel, g_q47_723d38, $L_723d38)
// [FUN_5ff120] constructs the static vector 723d28
extern "C" void __cdecl Static723d28_Construct(void)
{
    char a;
    CallThis(FUN_487730, (void*)g_q47_723d28, &a);
}
// [FUN_5ff5a0] constructs the static vector 723d38
extern "C" void __cdecl Static723d38_Construct(void)
{
    char a;
    CallThis(FUN_487730, (void*)g_q47_723d38, &a);
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void FUN_5ef450(); KING_ALIAS("_FUN_5ef450", "@Node_Destroy@8")
static void __cdecl Static722940_Destroy(void)
{
    CallThis(FUN_5ef450, (void*)g_q46_722940);
}
// [$L_5ef4e0] destroys the static node 722940 at exit
extern "C" int __cdecl Static722940_AtExit(void)
{
    return atexit(Static722940_Destroy);
}

// ---------------------------------------------------------------------------
// Small methods: table constructors and clears, list teardown, cursor handling, flag maps.

ASM_VAR(BYTE*, g_q52_723aa8, $L_723aa8)
// [$L_5fcdc0] destroys the object in slot i of 723aa8 (slot 0) and clears it
extern "C" void __cdecl Slots723aa8_Destroy(int i)
{
    BYTE* o = (&g_q52_723aa8)[i];
    if (o) {
        CallVirt(o, 0);
        (&g_q52_723aa8)[i] = 0;
    }
}

// ---------------------------------------------------------------------------
// Freeing a typed value.

extern "C" void Sce108_VecDtor(); KING_ALIAS("_Sce108_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
ASM_PROC($L_5f4b20)
// [FUN_5f4a90] frees the data (+8) of value s by its type (+0): 1-5 and 7 a plain block, 6 an array of 48-byte
// records (count before it) each freeing its +4
extern "C" void __fastcall Value_Free(int* s)
{
    switch (s[0]) {
    case 1: case 2: case 3: case 4: case 5: case 7:
        operator delete((void*)s[2]);
        break;
    case 6:
        if (BYTE* p = (BYTE*)s[2]) {
            CallStd(Sce108_VecDtor, p, 48, F(int, p, -4), $L_5f4b20);
            operator delete(p - 4);
        }
        break;
    }
}

// ---------------------------------------------------------------------------
// Reading scene nodes.

ASM_PROC(FUN_5f1db0)
extern "C" void FUN_5ae580(); KING_ALIAS("_FUN_5ae580", "@Render_PushMatrix@16")
ASM_PROC(FUN_5b4fc0)
ASM_PROC(FUN_5d2650)
extern "C" void FUN_5ac7f0(); KING_ALIAS("_FUN_5ac7f0", "@Plane_Read@12")
extern "C" void FUN_5ab1b0(); KING_ALIAS("_FUN_5ab1b0", "_NextToken")
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")
extern "C" void FUN_5ef800(); KING_ALIAS("_FUN_5ef800", "@Scene_TestInfo@16")
extern "C" void FUN_5f25c0(); KING_ALIAS("_FUN_5f25c0", "@Node_GetWorld@12")
extern "C" void FUN_5f2080(); KING_ALIAS("_FUN_5f2080", "@Slots_Call48@20")
extern "C" void FUN_5ae8a0(); KING_ALIAS("_FUN_5ae8a0", "@Clip_Pop@8")
extern "C" double __cdecl atof(const char* s);
extern "C" long __cdecl atol(const char* s);
ASM_VAR(AsmLabel, g_q109_7025f0, $L_7025f0)
ASM_VAR(AsmLabel, g_q109_71e820, $L_71e820)
static void Sce109_SetBits(BYTE* s, int off, DWORD lo, DWORD hi)
{
    F(DWORD, s, off) = lo;
    F(DWORD, s, off + 4) = hi;
}
// [FUN_5f3f30] reads node s of a scene file f: its plane (+20), a 32-byte text of up to four values (+56 default
// 300, +64, the int +96 default 8, +72 default 0.3), then its children
extern "C" void __fastcall SceneNode5f3f30_Read(BYTE* s, void*, FILE* f, int a2, int a3)
{
    char buf[64];
    CallThis(FUN_5ac7f0, s + 20, f);
    fread(buf, 32, 1, f);
    char* p = buf;
    const char* t;
    if ((t = CallC<const char*>(FUN_5ab1b0, &p)) != 0)
        F(double, s, 56) = atof(t);
    else
        Sce109_SetBits(s, 56, 0, 1081262080);
    if ((t = CallC<const char*>(FUN_5ab1b0, &p)) != 0)
        F(double, s, 64) = atof(t);
    else
        Sce109_SetBits(s, 64, 0, 1084715008);
    if ((t = CallC<const char*>(FUN_5ab1b0, &p)) != 0)
        F(int, s, 96) = atol(t);
    else
        F(int, s, 96) = 8;
    if ((t = CallC<const char*>(FUN_5ab1b0, &p)) != 0)
        F(double, s, 72) = atof(t);
    else
        Sce109_SetBits(s, 72, 858993459, 1070805811);
    int n = CallC<int>(FUN_5ab140, f);
    CallThis(FUN_5f1db0, s, f, n, 1, a2, a3);
}

// [$L_5f9580] reads node s of a scene file f: its plane (+20), a value kept for its children, +56; a "refer"
// node (by name +12) registers the referred name (from its 7th character) to +60; then its children
extern "C" void __fastcall SceneNode5f9580_Read(BYTE* s, void*, FILE* f, int a2, int a3)
{
    char name[64];
    CallThis(FUN_5ac7f0, s + 20, f);
    int x = CallC<int>(FUN_5ab140, f);
    F(int, s, 56) = CallC<int>(FUN_5ab140, f);
    strcpy(name, F(const char*, s, 12));
    if (strncmp(name, "refer", 5) == 0)
        CallThis(FUN_5d2650, (BYTE*)g_q109_71e820, name + 6, s + 60);
    int n = CallC<int>(FUN_5ab140, f);
    CallThis(FUN_5f1db0, s, f, n, x, a2, a3);
}

// [$L_5f4170] draws node s through view b when its info test passes: the world matrix, the clip test of its
// plane (+20) and the slots (+48 calls); always 1 after a passed test, else 0
extern "C" int __fastcall SceneNode5f4170_Draw(BYTE* s, void*, BYTE* a, BYTE* b)
{
    BYTE m[48];
    memcpy(m, (BYTE*)g_q109_7025f0, 48);
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    BYTE* w = CallThis<BYTE*>(FUN_5f25c0, F(BYTE*, s, 60), m);
    CallThis(FUN_5ae580, b, m, w);
    if (CallThis<int>(FUN_5b4fc0, b, s + 20))
        CallThis(FUN_5f2080, s, 0, a, b);
    CallThis(FUN_5ae8a0, b);
    return 1;
}

ASM_PROC(FUN_5fbdf0)
// [$L_5fc2c0] the base test of node s for a and b; when it sets bit 2 the name "<+132>::<+164>" goes to b + 124
// (and 0 is returned)
extern "C" int __fastcall SceneNode5fc2c0_Test(BYTE* s, void*, BYTE* a, BYTE* b)
{
    int r = CallThis<int>(FUN_5fbdf0, s, a, b);
    if (!(r & 4))
        return r;
    char* d = (char*)b + 124;
    strcpy(d, (const char*)s + 132);
    strcat(d, "::");
    strcat(d, (const char*)s + 164);
    return 0;
}

// [FUN_5fca50] reads node s of a scene file f: four floats as doubles (+288..+312), a count (+284) of ints stored from
// +276 (a third one lands on the count, as in the original), then its children
extern "C" void __fastcall SceneNode5fca50_Read(BYTE* s, void*, FILE* f, int a2, int a3)
{
    F(double, s, 288) = CallC<double>(FUN_5ab190, f);
    F(double, s, 296) = CallC<double>(FUN_5ab190, f);
    F(double, s, 304) = CallC<double>(FUN_5ab190, f);
    F(double, s, 312) = CallC<double>(FUN_5ab190, f);
    F(int, s, 284) = CallC<int>(FUN_5ab140, f);
    for (int i = 0; i < F(int, s, 284); i++)
        F(int, s, 276 + i * 4) = CallC<int>(FUN_5ab140, f);
    int n = CallC<int>(FUN_5ab140, f);
    CallThis(FUN_5f1db0, s, f, n, 1, a2, a3);
}

ASM_VAR(unsigned, g_q111_702e8c, $L_702e8c)
// [$L_5fcaf0] passes a to the children of s (+48's list): first the type 14 ones from the last back, then all
// from the first, stopping at one that takes it (vtbl+28); else (or with 702e8c bit 14) s's own vtbl+20; 1
// unless there are no children
extern "C" int __fastcall SceneNode5fcaf0_Pass(BYTE* s, void*, void* a)
{
    if (g_q111_702e8c & 0x4000) {
        CallVirt(s, 20, 0, a);
        return 1;
    }
    BYTE* n = F(BYTE*, F(BYTE*, s, 48), 0);
    if (!n)
        return 0;
    while (F(BYTE*, n, 8))
        n = F(BYTE*, n, 8);
    for (; n; n = F(BYTE*, n, 4))
        if (F(int, n, 16) == 14 && CallVirt<int>(n, 28, a))
            return 1;
    for (n = F(BYTE*, F(BYTE*, s, 48), 0); n; n = F(BYTE*, n, 8))
        if (CallVirt<int>(n, 28, a))
            return 1;
    CallVirt(s, 20, 0, a);
    return 1;
}

// ---------------------------------------------------------------------------
// Drawing animated and transformed nodes.

extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
ASM_VAR(BYTE*, g_q140_723a94, $L_723a94)
ASM_VAR(int, g_q140_723a50, $L_723a50)
ASM_VAR(int, g_q140_723a48, $L_723a48)
ASM_VAR(int, g_q140_723a8c, $L_723a8c)
ASM_VAR(BYTE*, g_q140_723a90, $L_723a90)
// [$L_5f2f40] draws animated node s through view b: like 5f4170, with the node's texture animation published
// while its slots draw (723a94 the node, 723a50 its kind +108, 723a48 +60, 723a8c/723a90 the frame and its
// table +72 for kinds 1-3) and its +64/+68 passed to the view
extern "C" int __fastcall SceneNode5f2f40_Draw(BYTE* s, void*, BYTE* a, BYTE* b)
{
    BYTE m[48];
    memcpy(m, (BYTE*)g_q109_7025f0, 48);
    g_q140_723a94 = s;
    if (!CallThis<int>(FUN_5ef800, s, a, b))
        return 0;
    BYTE* w = CallThis<BYTE*>(FUN_5f25c0, F(BYTE*, s, 112), m);
    CallThis(FUN_5ae580, b, m, w);
    F(int, b, 8252) = F(int, s, 64);
    F(int, b, 8256) = F(int, s, 68);
    g_q140_723a50 = F(int, s, 108);
    g_q140_723a48 = F(int, s, 60);
    switch (F(int, s, 108)) {
    case 1:
    case 3:
        g_q140_723a8c = F(int, s, 100);
        g_q140_723a90 = s + 72;
        break;
    case 2:
        g_q140_723a8c = F(int, s, 96);
        g_q140_723a90 = s + 72;
        break;
    default:
        g_q140_723a8c = 0;
        g_q140_723a90 = 0;
    }
    if (CallThis<int>(FUN_5b4fc0, b, s + 20))
        CallThis(FUN_5f2080, s, 0, a, b);
    g_q140_723a8c = 0;
    g_q140_723a48 = 0;
    g_q140_723a50 = 0;
    g_q140_723a90 = 0;
    g_q140_723a94 = 0;
    CallThis(FUN_5ae8a0, b);
    return 1;
}

extern "C" void FUN_5ef820(); KING_ALIAS("_FUN_5ef820", "@Obj_Flag0Clear@24")
extern "C" void FUN_5f2100(); KING_ALIAS("_FUN_5f2100", "@Slots_Call56@28")
// the draw of transform nodes with the world node at +node
static int Sce141_DrawXform(BYTE* s, int node, void* a1, BYTE* m, BYTE* a3, BYTE* b)
{
    BYTE w1[48], saved[48], t[48];
    memcpy(w1, (BYTE*)g_q109_7025f0, 48);
    if (!CallThis<int>(FUN_5ef820, s, a1, m, a3, b))
        return 0;
    BYTE* w = CallThis<BYTE*>(FUN_5f25c0, F(BYTE*, s, node), w1);
    memcpy(saved, m, 48);
    memcpy(m, CallC<BYTE*>(FUN_5aba40, t, w1, a3), 48);
    CallThis(FUN_5ae580, b, w1, w);
    CallThis(FUN_5f2100, s, 0, a1, m, a3, b);
    CallThis(FUN_5ae8a0, b);
    memcpy(m, saved, 48);
    return 1;
}
// [$L_5f3060] draws transform node s for (a1, matrix m, a3, view b) when its test passes: m becomes the node's
// world matrix times a3 while its slots draw, then is restored; 1 after a passed test
extern "C" int __fastcall SceneNode5f3060_Draw(BYTE* s, void*, void* a1, BYTE* m, BYTE* a3, BYTE* b)
{
    return Sce141_DrawXform(s, 112, a1, m, a3, b);
}

ASM_PROC(FUN_5f21e0)
// the hit test of transform nodes with the world node at +node and the hit value at +copy
static int Sce141_HitXform(BYTE* s, int node, int copy, int flags, BYTE* rec)
{
    BYTE w1[48], saved[48], t[48];
    memcpy(w1, (BYTE*)g_q109_7025f0, 48);
    if (F(BYTE, s, 40) & 1)
        return 0;
    BYTE* m = rec + 4;
    memcpy(saved, m, 48);
    int kind = CallThis<int>(FUN_5f25c0, F(BYTE*, s, node), w1);
    memcpy(w1, CallC<BYTE*>(FUN_5ac2e0, t, w1), 48);
    if (kind == 0)
        memcpy(m, w1, 48);
    else if (kind == 1)
        memcpy(m, CallC<BYTE*>(FUN_5aba40, t, m, w1), 48);
    int r;
    if (flags & 1)
        r = CallThis<int>(FUN_5f21e0, s, 0, flags, rec);
    else {
        BYTE* o = F(BYTE*, s, 56);
        if (o && (flags & 4)) {
            r = CallVirt<int>(o, 52, flags | 1, rec);
            if (r & 1) {
                F(int, rec, 116) = F(int, s, copy);
                return r;
            }
        } else
            r = CallThis<int>(FUN_5f21e0, s, 0, flags, rec);
    }
    if (!(r & 1))
        memcpy(m, saved, 48);
    return r;
}
// [$L_5f3410] a ray/hit test of transform node s (skipped with +40 bit 0): the record's matrix (+4) is moved
// into the node's space (the inverse world matrix, alone for kind 0 or applied for 1) for the children test
// (5f21e0, or the +56 object's vtbl+52 with flags bit 2, copying +116 on a hit); restored unless bit 0 of the
// result is set
extern "C" int __fastcall SceneNode5f3410_Hit(BYTE* s, void*, int flags, BYTE* rec)
{
    return Sce141_HitXform(s, 112, 116, flags, rec);
}

// [$L_5f4210] 5f3060 for nodes with the world node at +60
extern "C" int __fastcall SceneNode5f4210_Draw(BYTE* s, void*, void* a1, BYTE* m, BYTE* a3, BYTE* b)
{
    return Sce141_DrawXform(s, 60, a1, m, a3, b);
}

// [$L_5f43d0] 5f3410 for nodes with the world node at +60 and the hit value at +64
extern "C" int __fastcall SceneNode5f43d0_Hit(BYTE* s, void*, int flags, BYTE* rec)
{
    return Sce141_HitXform(s, 60, 64, flags, rec);
}

// [FUN_5f9b00] a hit test of reference node s (skipped with +40 bit 0, +80 set or flags without bit 2): the
// referred object (+64) tests the record with its matrix (+4) moved into the space of node +68; a miss, or a
// hit on the record's own owner (+72 = rec +120), restores the record (matrix, +76, +88); else the hit is
// marked with +72 (rec +116)
extern "C" int __fastcall SceneNode5f9b00_Hit(BYTE* s, void*, int flags, BYTE* rec)
{
    BYTE w1[48], saved[48], t1[48], t2[48];
    memcpy(w1, (BYTE*)g_q109_7025f0, 48);
    if ((F(BYTE, s, 40) & 1) || F(int, s, 80) || !(flags & 4))
        return 0;
    BYTE* m = rec + 4;
    memcpy(saved, m, 48);
    if (!F(BYTE*, s, 64))
        return 0;
    DWORD v1[3], v2[3];
    memcpy(v1, rec + 76, 12);
    memcpy(v2, rec + 88, 12);
    if (BYTE* n = F(BYTE*, s, 68)) {
        int kind = CallThis<int>(FUN_5f25c0, n, w1);
        if (kind == 0)
            memcpy(m, CallC<BYTE*>(FUN_5ac2e0, t1, w1), 48);
        else if (kind == 1) {
            BYTE* inv = CallC<BYTE*>(FUN_5ac2e0, t1, w1);
            memcpy(m, CallC<BYTE*>(FUN_5aba40, t2, m, inv), 48);
        }
    }
    int r = CallVirt<int>(F(BYTE*, s, 64), 52, flags, rec);
    if (!r) {
        memcpy(m, saved, 48);
        return 0;
    }
    int c = F(int, s, 72);
    if (c && c == F(int, rec, 120)) {
        memcpy(rec + 76, v1, 12);
        memcpy(m, saved, 48);
        memcpy(rec + 88, v2, 12);
        return 0;
    }
    F(int, rec, 116) = c;
    return r;
}

// [FUN_5f9d20] draws reference node s for (a1, matrix m, a3, view b) when its test passes: the referred object
// (+64) draws with vtbl+56, in the space of node +68 when there is one (m moved into it for the call); its result
extern "C" int __fastcall SceneNode5f9d20_Draw(BYTE* s, void*, void* a1, BYTE* m, BYTE* a3, BYTE* b)
{
    BYTE w1[48], saved[48], t[48];
    memcpy(w1, (BYTE*)g_q109_7025f0, 48);
    if (!CallThis<int>(FUN_5ef820, s, a1, m, a3, b))
        return 0;
    if (!F(BYTE*, s, 64))
        return 0;
    memcpy(saved, m, 48);
    if (BYTE* n = F(BYTE*, s, 68)) {
        BYTE* w = CallThis<BYTE*>(FUN_5f25c0, n, w1);
        CallThis(FUN_5ae580, b, w1, w);
        memcpy(m, CallC<BYTE*>(FUN_5aba40, t, w1, a3), 48);
    }
    int r = CallVirt<int>(F(BYTE*, s, 64), 56, a1, m, a3, b);
    if (F(BYTE*, s, 68))
        CallThis(FUN_5ae8a0, b);
    memcpy(m, saved, 48);
    return r;
}

extern "C" void FUN_5ac640(); KING_ALIAS("_FUN_5ac640", "_Matrix_Transposed")
extern "C" void FUN_5ab8f0(); KING_ALIAS("_FUN_5ab8f0", "@Plane_Transform@12")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_5ab660(); KING_ALIAS("_FUN_5ab660", "@Vec3_DivAssign@16")
extern "C" void FUN_5ad150(); KING_ALIAS("_FUN_5ad150", "@Render_PushShade@12")
extern "C" void FUN_5adc00(); KING_ALIAS("_FUN_5adc00", "@Render_PopShade@8")
// [FUN_5f98c0] draws shaded reference node s through view b when its info test passes, in the space of node
// +68: with +80 (or +76 without flags bit 4) its plane (+48) is moved into view space, normalised and pushed as
// the shade around the referred object's (+64) draw (vtbl+48, flags bit 4 added with +80; its material +36 and
// the view's +8276 from +84 set for it); 1 after a passed test
extern "C" int __fastcall SceneNode5f98c0_Draw(BYTE* s, void*, int flags, BYTE* b)
{
    BYTE w1[48], t1[48], t2[48], t3[48], m[48], out[12];
    memcpy(w1, (BYTE*)g_q109_7025f0, 48);
    if (!CallThis<int>(FUN_5ef800, s, flags, b))
        return 0;
    if (BYTE* n = F(BYTE*, s, 68)) {
        BYTE* w = CallThis<BYTE*>(FUN_5f25c0, n, w1);
        CallThis(FUN_5ae580, b, w1, w);
    }
    if (CallThis<int>(FUN_5b4fc0, b, s + 20)) {
        bool shade = F(int, s, 80) || (F(int, s, 76) && !(flags & 16));
        if (shade) {
            float p[4];
            memcpy(p, s + 48, 16);
            BYTE* a = b + 2240 + F(int, b, 8224) * 48;
            BYTE* r = CallC<BYTE*>(FUN_5ac2e0, t1, b + 5216 + F(int, b, 8236) * 48);
            r = CallC<BYTE*>(FUN_5aba40, m, r, a);
            memcpy(m, CallC<BYTE*>(FUN_5ac2e0, t2, m), 48);
            memcpy(m, CallC<BYTE*>(FUN_5ac640, t3, m), 48);
            CallThis(FUN_5ab8f0, p, m);
            double len = CallC<double>(FUN_5ab4d0, p);
            CallThis(FUN_5ab660, p, out, (float)len);
            p[3] = (float)(p[3] / len);
            CallThis(FUN_5ad150, b, p);
        }
        BYTE* o = F(BYTE*, s, 64);
        if (F(int, o, 36) != F(int, s, 36))
            CallVirt(b, 8, F(int, o, 36));
        int keep = F(int, b, 8276);
        if (F(int, s, 84) >= 0)
            F(int, b, 8276) = F(int, s, 84);
        CallVirt(F(BYTE*, s, 64), 48, (F(int, s, 80) ? 16 : 0) | flags, b);
        F(int, b, 8276) = keep;
        if (F(int, F(BYTE*, s, 64), 36) != F(int, s, 36))
            CallVirt(b, 8, F(int, s, 36));
        if (F(int, s, 80) || (F(int, s, 76) && !(flags & 16)))
            CallThis(FUN_5adc00, b);
    }
    if (F(BYTE*, s, 68))
        CallThis(FUN_5ae8a0, b);
    return 1;
}

// the world position of node s's point (+20..+28) through world node n into out; the node's kind
static int Sce163_WorldPos(BYTE* s, BYTE* n, float* out)
{
    float m[12];
    memcpy(m, (BYTE*)g_q109_7025f0, 48);
    int k = CallThis<int>(FUN_5f25c0, n, m);
    float x = F(float, s, 20), y = F(float, s, 24), z = F(float, s, 28);
    double X = m[6] * z + m[3] * y;
    X = X + m[0] * x;
    X = X + m[9];
    double Y = m[7] * z + m[4] * y;
    Y = Y + m[1] * x;
    Y = Y + m[10];
    double Z = m[8] * z + m[5] * y;
    Z = Z + m[2] * x;
    Z = Z + m[11];
    out[0] = (float)X;
    out[1] = (float)Y;
    out[2] = (float)Z;
    return k;
}

// [$L_5f2e70] the world position of node s's point (+20..+28) through its world node (+112) into out
extern "C" int __fastcall SceneNode5f2e70_WorldPos(BYTE* s, void*, float* out)
{
    return Sce163_WorldPos(s, F(BYTE*, s, 112), out);
}

// [$L_5f40a0] 5f2e70 with the world node at +60
extern "C" int __fastcall SceneNode5f40a0_WorldPos(BYTE* s, void*, float* out)
{
    return Sce163_WorldPos(s, F(BYTE*, s, 60), out);
}

// [$L_5f9760] the point (+20..+28) of node s: through the world node +68 when there is one, else as it is (1)
extern "C" int __fastcall SceneNode5f9760_WorldPos(BYTE* s, void*, float* out)
{
    if (!F(BYTE*, s, 68)) {
        memcpy(out, s + 20, 12);
        return 1;
    }
    return Sce163_WorldPos(s, F(BYTE*, s, 68), out);
}

extern "C" void FUN_5fd580(); KING_ALIAS("_FUN_5fd580", "_Sound_TakeChannel")
extern "C" void FUN_604f20(); KING_ALIAS("_FUN_604f20", "_Snd_Start")
extern "C" void FUN_605000(); KING_ALIAS("_FUN_605000", "_Channel_Call8")
ASM_VAR(int, g_q182_723b28, $L_723b28)
ASM_VAR(BYTE**, g_q182_723b34, $L_723b34)
ASM_VAR(int, g_q182_723b38, $L_723b38)
ASM_VAR(float, g_q182_683c0c, $L_683c0c)
ASM_VAR(float, g_q182_64c250, $L_64c250)
ASM_VAR(float, g_q182_64c264, $L_64c264)
ASM_VAR(double, g_q182_64da58, $L_64da58)
// v scaled by 683c0c and kept in 0..1
static float Sce182_Unit(float v)
{
    v = g_q182_683c0c * v;
    if (v > g_q182_64c250)
        return 1.0f;
    if (!(v >= g_q182_64c264))
        return 0;
    return v;
}
// [FUN_5fd430] plays sound s (when sound is on and its sample +28 is loaded) at volume vol and pitch pitch: a new
// channel (5fd580) is started (+20 playing, +24 the channel), a playing one updated; +12 the play stamp
extern "C" void __fastcall Sound_Play(BYTE* s, void*, float vol, float pitch, int a3)
{
    if (!g_q182_723b28 || !(F(BYTE, g_q182_723b34[F(int, s, 28)], 24) & 1))
        return;
    float v = Sce182_Unit(vol), p = Sce182_Unit(pitch);
    if (!F(int, s, 20)) {
        int ch = CallC<int>(FUN_5fd580, s);
        if (ch == -1)
            return;
        F(int, s, 24) = ch;
        F(int, s, 20) = 1;
        F(int, s, 16) = 0;
        CallC(FUN_604f20, F(int, s, 28), ch, (float)(v * g_q182_64da58), (float)(p * g_q182_64da58), a3,
              (int)F(BYTE, s, 8));
    } else
        CallC(FUN_605000, F(int, s, 24), (float)(v * g_q182_64da58), (float)(p * g_q182_64da58), a3);
    F(int, s, 12) = g_q182_723b38;
}

extern "C" void $L_5ffc60(); KING_ALIAS("_$L_5ffc60", "@Vec56_Destroy@8")
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
// [$L_5ff150] destroys the static vector 723d28
extern "C" void __cdecl Static723d28_Destroy(void) { CallThis($L_5ffc60, (void*)g_q47_723d28); }
// [$L_5ff140] destroys it at exit
extern "C" int __cdecl Static723d28_AtExit(void) { return atexit(Static723d28_Destroy); }
// [$L_5ff110] the static initializer of 723d28
extern "C" int __cdecl Static723d28_Init(void)
{
    Static723d28_Construct();
    return Static723d28_AtExit();
}
// [$L_5ff240] deletes the sound lock
extern "C" void __cdecl Sound_DeleteLock(void) { DeleteCriticalSection(&g_sc723b40); }
// [$L_5ff230] deletes it at exit
extern "C" int __cdecl Sound_LockAtExit(void) { return atexit(Sound_DeleteLock); }
// [$L_5ff210] the static initializer of the sound lock
extern "C" int __cdecl Sound_LockInit(void)
{
    Sound_InitLock();
    return Sound_LockAtExit();
}
// [$L_5ff5d0] frees the static vector 723d38
extern "C" void __cdecl Static723d38_Destroy(void) { CallThis(FUN_404420, (void*)g_q47_723d38); }
// [$L_5ff5c0] frees it at exit
extern "C" int __cdecl Static723d38_AtExit(void) { return atexit(Static723d38_Destroy); }
// [$L_5ff590] the static initializer of 723d38
extern "C" int __cdecl Static723d38_Init(void)
{
    Static723d38_Construct();
    return Static723d38_AtExit();
}

extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")
extern "C" void FUN_5ff5e0(); KING_ALIAS("_FUN_5ff5e0", "_Music_FindFiles")
extern "C" __declspec(dllimport) DWORD __stdcall timeGetTime(void);
ASM_VAR(AsmLabel, s_q194_68321c, $L_68321c)
ASM_VAR(AsmLabel, s_q194_683c70, $L_683c70)
ASM_VAR(AsmLabel, s_q194_683c68, $L_683c68)
ASM_VAR(AsmLabel, s_q194_683c60, $L_683c60)
// [FUN_5ff890] reseeds the random generator 723b58 from the time, keeps dir's full path (with a trailing
// backslash) in the vector 723d38 and adds the three default search folders (5ff5e0)
extern "C" int __cdecl Sce_SetRootDir(const char* dir)
{
    DWORD a[57];
    memset(a, 0, 55 * 4);
    DWORD mj = timeGetTime() ^ 161803398, mk = 1;
    a[54] = mj;
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
    a[55] = 0;
    a[56] = 31;
    memcpy((void*)g_sce723b58, a, sizeof a);
    char buf[8192];
    char* part = 0;
    GetFullPathNameA(dir, 8192, buf, &part);
    if (part && buf[strlen(buf) - 1] != '\\')
        strncat(buf, (const char*)s_q194_68321c, 8192);
    char** v = (char**)(void*)g_q47_723d38;
    v[1] = v[0];
    CallThis(FUN_44c220, v, v[1], buf, buf + strlen(buf));
    CallC(FUN_5ff5e0, (const char*)s_q194_683c70);
    CallC(FUN_5ff5e0, (const char*)s_q194_683c68);
    CallC(FUN_5ff5e0, (const char*)s_q194_683c60);
    return 0;
}

typedef void* SceFreeLists[17];
ASM_VAR(SceFreeLists, g_q195_68d1c0, $L_68d1c0)
ASM_VAR(volatile LONG, g_q195_68d204, $L_68d204)
extern "C" void __fastcall Stl_Lock(volatile LONG* l, void*);
extern "C" void __cdecl free(void*);
// [$L_5ffc60] destroys a vector of 56-byte records (each one's storage freed) and returns its storage to the STL
// allocator
extern "C" void __fastcall Vec56_Destroy(BYTE** v, void*)
{
    for (BYTE* e = v[0]; e != v[1]; e += 56)
        CallThis(FUN_404420, e);
    BYTE* p = v[0];
    int n = (int)(v[2] - p) / 56;
    if (!n)
        return;
    unsigned bytes = n * 56;
    if (bytes > 128) {
        free(p);
        return;
    }
    void** list = &g_q195_68d1c0[(bytes + 7) >> 3];
    Stl_Lock(&g_q195_68d204, 0);
    *(void**)p = *list;
    *list = p;
    g_q195_68d204 = 0;
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
ASM_VAR(char, g_q197_723190, $L_723190)
extern "C" void q197_FUN_5ef450(); KING_ALIAS("_q197_FUN_5ef450", "@Node_Destroy@8")
extern "C" void q197_FUN_5ef510(); KING_ALIAS("_q197_FUN_5ef510", "_Static723190_Construct")
// [$L_5ef530] destroys the static object 723190 at exit (FUN_5ef450)
extern "C" void __cdecl Static723190_Destroy(void) { CallThis(q197_FUN_5ef450, &g_q197_723190); }
// [$L_5ef520] registers the teardown at exit
extern "C" int __cdecl Static723190_AtExit(void) { return atexit((void(__cdecl*)(void))Static723190_Destroy); }
// [$L_5ef500] the static initializer: FUN_5ef510, then the teardown at exit
extern "C" int __cdecl Static723190_Init(void)
{
    CallC(q197_FUN_5ef510);
    return Static723190_AtExit();
}

// ---------------------------------------------------------------------------
// Thunks.

ASM_VAR(char, g_q198_723978, $L_723978)
extern "C" int __fastcall q198_Node_Destroy(BYTE* s, void* d); KING_ALIAS("@q198_Node_Destroy@8", "@Node_Destroy@8")
// [$L_5ef4b0] Node_Destroy on the static object 723978
extern "C" void __cdecl Static723978_Destroy(void) { q198_Node_Destroy((BYTE*)&g_q198_723978, 0); }

extern "C" void FUN_5adc80(); KING_ALIAS("_FUN_5adc80", "@Render_SetFog@28")
extern "C" void FUN_5addf0(); KING_ALIAS("_FUN_5addf0", "@Render_SetAmbient@32")
extern "C" void FUN_5f2080(); KING_ALIAS("_FUN_5f2080", "@Slots_Call48@20")
ASM_VAR(unsigned, g_q217_702e8c, $L_702e8c)
// [$L_5f3da0] draws node s with its own fog (+56, +64, +96; flag 4 of 702e8c on when +96 >= 0) and ambient (+72..)
// on renderer r, then puts r's fog (its range +8536, +8540 scaled by +8464, mode +8544 or none) and ambient (+8512)
// back; 1
extern "C" int __fastcall Node_DrawWithFog(BYTE* s, void*, int a, BYTE* r)
{
    unsigned saved = g_q217_702e8c & 4;
    double end = F(float, r, 8540) * F(double, r, 8464);
    int mode = F(int, r, 8544);
    double amb = F(double, r, 8512);
    double start = F(float, r, 8536) * F(double, r, 8464);
    if (F(int, s, 96) >= 0)
        g_q217_702e8c |= 4;
    else
        g_q217_702e8c &= ~4u;
    CallThis(FUN_5adc80, r, F(double, s, 56), F(double, s, 64), F(int, s, 96));
    CallThis(FUN_5addf0, r, F(double, s, 72), F(double, s, 80), F(double, s, 88));
    CallThis(FUN_5f2080, s, 0, a, r);
    g_q217_702e8c &= ~4u;
    if (saved) {
        g_q217_702e8c |= saved;
        CallThis(FUN_5adc80, r, end, start, mode);
    } else {
        g_q217_702e8c &= ~4u;
        CallThis(FUN_5adc80, r, end, start, -1);
    }
    CallThis(FUN_5addf0, r, amb, -1.0, -1.0);
    return 1;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Node5f1a70_Destroy(BYTE* s); KING_ALIAS("@q238_Node5f1a70_Destroy@4", "@Node5f1a70_Destroy@4")
extern "C" int __fastcall q238_Sce5ef660_Destroy(BYTE* s); KING_ALIAS("@q238_Sce5ef660_Destroy@4", "@Sce5ef660_Destroy@4")
// [FUN_5f0770] forwards to Node5f1a70_Destroy
extern "C" int __fastcall Thunk5f0770(BYTE* s) { return q238_Node5f1a70_Destroy(s); }
// [FUN_5f0870] forwards to Sce5ef660_Destroy
extern "C" int __fastcall Thunk5f0870(BYTE* s) { return q238_Sce5ef660_Destroy(s); }

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q241_L_5ef4b0(); KING_ALIAS("_q241_L_5ef4b0", "_Static723978_Destroy")
// [$L_5ef4a0] registers 5ef4b0 at exit
extern "C" int __cdecl Static5ef4a0_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_5ef4b0); }

// === reading a node's shape data (FUN_5f8bf0)

extern "C" void FUN_5ab6a0(); KING_ALIAS("_FUN_5ab6a0", "_Vec3_NearlyEqual")
extern "C" void FUN_5e5720(); KING_ALIAS("_FUN_5e5720", "@Fw_Read5e5720@12")
extern "C" void Sce_VecCtorEh(); KING_ALIAS("_Sce_VecCtorEh", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void FUN_5e69a0(); KING_ALIAS("_FUN_5e69a0", "@Polygon_Read@12")    // reads a 48-byte item (file)
ASM_VAR(AsmLabel, g_sd5e6980, $L_5e6980)    // its constructor
ASM_VAR(AsmLabel, g_sd5f4b20, $L_5f4b20)    // its destructor
ASM_VAR(char, s_sd683bcc, $L_683bcc)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// a zeroed n-byte block
static BYTE* Sd_New(int n)
{
    BYTE* p = (BYTE*)operator new(n);
    if (p)
        memset(p, 0, n);
    return p;
}

extern "C" {

// [FUN_5f8bf0] reads the data of shape s (+0 kind) from file f into +8: a plane (kinds 1, 2, 5), a 32-byte
// record (3, 4), +4 48-byte items (6, the count read first) or +4 points (7; fatal for two equal in a row)
void __fastcall Shape_ReadData(BYTE* s, void*, BYTE* f)
{
    switch (F(int, s, 0)) {
    case 1:
    case 2:
    case 5:
        F(BYTE*, s, 8) = Sd_New(16);
        CallThis(FUN_5ac7f0, F(BYTE*, s, 8), f);
        break;
    case 3:
    case 4:
        F(BYTE*, s, 8) = Sd_New(32);
        CallThis(FUN_5e5720, F(BYTE*, s, 8), f);
        break;
    case 6: {
        int n = CallC<int>(FUN_5ab140, f);
        F(int, s, 4) = n;
        BYTE* a = (BYTE*)operator new(n * 48 + 4);
        BYTE* items = 0;
        if (a) {
            F(int, a, 0) = n;
            items = a + 4;
            CallStd(Sce_VecCtorEh, items, 48, n, (void*)g_sd5e6980, (void*)g_sd5f4b20);
        }
        F(BYTE*, s, 8) = items;
        for (int i = 0; i < F(int, s, 4); i++)
            CallThis(FUN_5e69a0, F(BYTE*, s, 8) + i * 48, f);
        break;
    }
    case 7: {
        F(BYTE*, s, 8) = Sd_New(F(int, s, 4) * 12);
        for (int i = 0; i < F(int, s, 4); i++) {
            float* v = (float*)(F(BYTE*, s, 8) + i * 12);
            CallThis(FUN_5ab790, v, f);
            if (i > 0 && CallC<int>(FUN_5ab6a0, v, v - 3)) {
                char text[128];
                sprintf(text, &s_sd683bcc, (double)v[0], (double)v[1], (double)v[2]);
                Sys_Fatal(text, 0);
            }
        }
        break;
    }
    }
}

} // extern "C"

// === vector<56 bytes>::insert_aux (FUN_5ffdf0)

ASM_PROC(FUN_6000c0)    // copy constructor of a 56-byte record (this, src)
ASM_PROC(FUN_600420)    // uninitialized copy of 56-byte records (first, last, dest, x)
extern "C" void FUN_6002f0(); KING_ALIAS("_FUN_6002f0", "_Music_CopyBackward")
extern "C" void FUN_44c160(); KING_ALIAS("_FUN_44c160", "@PodVec_Assign@12")
extern "C" void FUN_44dd90(); KING_ALIAS("_FUN_44dd90", "@VecChar_Copy@12")
extern "C" void FUN_44b090(); KING_ALIAS("_FUN_44b090", "@Vec1_Free@8")
extern "C" void FUN_44b740(); KING_ALIAS("_FUN_44b740", "_Stl_Allocate")
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")

extern "C" {

// [FUN_5ffdf0] inserts a copy of record x (56 bytes: a byte vector, then 40 bytes) at pos of vector v: shifted up
// within the capacity, or into a new buffer of twice the size (1 when empty)
void __fastcall Vec56_InsertAux(BYTE** v, void*, BYTE* pos, const BYTE* x)
{
    if (v[1] != v[2]) {
        BYTE* end = v[1];
        if (end)
            CallThis(FUN_6000c0, end, end - 56);
        v[1] = end + 56;
        BYTE t[56];
        CallThis(FUN_44dd90, t, x);
        memcpy(t + 16, x + 16, 40);
        CallC(FUN_6002f0, pos, end - 56, end, x, 0);
        CallThis(FUN_44c160, pos, t);
        memcpy(pos + 16, t + 16, 40);
        CallThis(FUN_44b090, t);
        return;
    }
    int n = (int)(v[1] - v[0]) / 56;
    int len = n ? n * 2 : 1;
    BYTE* p = CallC<BYTE*>(FUN_44b740, len * 56);
    BYTE* q = CallC<BYTE*>(FUN_600420, v[0], pos, p, x);
    if (q)
        CallThis(FUN_6000c0, q, x);
    BYTE* e = CallC<BYTE*>(FUN_600420, pos, v[1], q + 56, x);
    for (BYTE* r = v[0]; r != v[1]; r += 56)
        CallThis(FUN_404420, r);
    if (int bytes = (int)(v[2] - v[0]) / 56 * 56)
        CallC(FUN_44b4a0, v[0], bytes);
    v[1] = e;
    v[0] = p;
    v[2] = p + len * 56;
}

} // extern "C"

// === loading an animated node ($L_5f3190)

extern "C" void FUN_5e56f0(); KING_ALIAS("_FUN_5e56f0", "@Fw_Read5e56f0@12")
extern "C" void __fastcall Wea5a72c0_Init(BYTE* s);

extern "C" {

// [$L_5f3190] reads animated node s from file f: its plane (+20), two names (+112, +56), the key kind (+108,
// the low byte; the rest the values per key), the keys (+64, 28 bytes each, count +60) with their value pairs
// (+20, count +24) and, by kind, a point (1, 2) or a value (3) per key (+68), then the node's part
int __fastcall AnimNode_Load(BYTE* s, void*, void* f, int, int)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    Sce_ReadName(f, s + 112);
    Sce_ReadName(f, s + 56);
    if (F(int, s, 108))
        F(int, s, 108) = CallC<int>(FUN_5ab140, f);
    int per = F(int, s, 108) >> 8;
    F(int, s, 108) &= 255;
    int n = CallC<int>(FUN_5ab140, f);
    F(int, s, 60) = n;
    F(BYTE*, s, 64) = 0;
    if (n) {
        BYTE* a = (BYTE*)operator new(n * 28 + 4);
        if (a) {
            F(int, a, 0) = n;
            CallStd(Sce_VecCtorEh, a + 4, 28, n, (void*)Wea5a72c0_Init, (void*)g_dtor5a3df0);
            F(BYTE*, s, 64) = a + 4;
        }
    }
    if (F(int, s, 108) > 0 && F(int, s, 60) > 0) {
        int m = F(int, s, 60);
        BYTE* b = (BYTE*)operator new((m + 1) * 12);
        if (b)
            memset(b, 0, (m + 1) * 12);
        F(BYTE*, s, 68) = b;
    }
    for (int i = 0; i < F(int, s, 60); i++) {
        BYTE* k = F(BYTE*, s, 64) + i * 28;
        CallThis(FUN_5e56f0, k, f);
        if (per > 0) {
            F(int, k, 24) = per;
            float* v = (float*)operator new(per * 8);
            F(float*, k, 20) = v;
            for (int j = 0; j < per; j++) {
                F(float*, F(BYTE*, s, 64) + i * 28, 20)[j * 2] = (float)CallC<double>(FUN_5ab190, f);
                F(float*, F(BYTE*, s, 64) + i * 28, 20)[j * 2 + 1] = (float)CallC<double>(FUN_5ab190, f);
            }
        }
        int kind = F(int, s, 108);
        if (kind == 1 || kind == 2)
            CallThis(FUN_5ab790, F(BYTE*, s, 68) + i * 12, f);
        else if (kind == 3)
            F(float, F(BYTE*, s, 68), i * 12) = (float)CallC<double>(FUN_5ab190, f);
    }
    return Sce_LoadTail(s, (int)f, 1, F(int, s, 64), F(int, s, 68));
}

} // extern "C"

// === loading an animated node with one name ($L_5f2800)

extern "C" {

// [$L_5f2800] like AnimNode_Load with one name (+104): the plane (+20), the key kind (+108), the keys (+60,
// count +56) with their value pairs and, by kind, a point or a value per key (+64), then the node's part
int __fastcall AnimNode1_Load(BYTE* s, void*, void* f, int, int)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    Sce_ReadName(f, s + 104);
    if (F(int, s, 108))
        F(int, s, 108) = CallC<int>(FUN_5ab140, f);
    int per = F(int, s, 108) >> 8;
    F(int, s, 108) &= 255;
    int n = CallC<int>(FUN_5ab140, f);
    F(int, s, 56) = n;
    F(BYTE*, s, 60) = 0;
    if (n) {
        BYTE* a = (BYTE*)operator new(n * 28 + 4);
        if (a) {
            F(int, a, 0) = n;
            CallStd(Sce_VecCtorEh, a + 4, 28, n, (void*)Wea5a72c0_Init, (void*)g_dtor5a3df0);
            F(BYTE*, s, 60) = a + 4;
        }
    }
    if (F(int, s, 108) > 0 && F(int, s, 56) > 0) {
        int m = F(int, s, 56);
        BYTE* b = (BYTE*)operator new((m + 1) * 12);
        if (b)
            memset(b, 0, (m + 1) * 12);
        F(BYTE*, s, 64) = b;
    }
    for (int i = 0; i < F(int, s, 56); i++) {
        BYTE* k = F(BYTE*, s, 60) + i * 28;
        CallThis(FUN_5e56f0, k, f);
        if (per > 0) {
            F(int, k, 24) = per;
            F(float*, k, 20) = (float*)operator new(per * 8);
            for (int j = 0; j < per; j++) {
                F(float*, F(BYTE*, s, 60) + i * 28, 20)[j * 2] = (float)CallC<double>(FUN_5ab190, f);
                F(float*, F(BYTE*, s, 60) + i * 28, 20)[j * 2 + 1] = (float)CallC<double>(FUN_5ab190, f);
            }
        }
        int kind = F(int, s, 108);
        if (kind == 1 || kind == 2)
            CallThis(FUN_5ab790, F(BYTE*, s, 64) + i * 12, f);
        else if (kind == 3)
            F(float, F(BYTE*, s, 64), i * 12) = (float)CallC<double>(FUN_5ab190, f);
    }
    return Sce_LoadTail(s, (int)f, 1, F(int, s, 60), F(int, s, 64));
}

} // extern "C"

// === building a vertex buffer description (FUN_5f2a40)

extern "C" void FUN_5abee0(); KING_ALIAS("_FUN_5abee0", "_Matrix_RotationX")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
ASM_VAR(BYTE, g_vb723a4c, $L_723a4c)
ASM_VAR(float, g_vbLightX, $L_7239f8)
ASM_VAR(float, g_vbLightY, $L_7239fc)
ASM_VAR(float, g_vbLightZ, $L_723a00)
ASM_VAR(BYTE*, g_vb720904, $L_720904)
ASM_VAR(AsmLabel, g_vb723a9c, $L_723a9c)
ASM_VAR(double, g_vb64b670, $L_64b670)
ASM_VAR(double, g_vb64b6e0, $L_64b6e0)
ASM_VAR(float, g_vb64c458, $L_64c458)

extern "C" {

// [FUN_5f2a40] fills the strided description s of n vertices v (12 bytes) of format *fmt with the texture
// coordinates of the n 28-byte records r (r+12, r+16, then r+20 the further pairs, r+24 their count); hdr
// = {0, fmt, n, v}. Format 1 packs the colours, format 2 (unless 702e8c bit 6) becomes 3, the vertices shaded
// by the fixed light (rotated once by pi/4 about x and pi/6 about z)
void __cdecl VertexDesc_Build(BYTE* s, int* fmt, int* hdr, int n, BYTE* r, float* v)
{
    if (!(g_vb723a4c & 1)) {
        g_vb723a4c |= 1;
        float a[12], b[12];
        float* m = CallC<float*>(FUN_5abee0, a, 0.785398185f);
        float t[3] = {(float)((double)m[9] + m[3]), (float)((double)m[10] + m[4]), (float)((double)m[11] + m[5])};
        float* z = CallC<float*>(FUN_5ac0c0, b, 0.523598790f);
        g_vbLightZ = (float)((double)t[2] * z[8] + (double)t[1] * z[5] + (double)t[0] * z[2] + z[11]);
        g_vbLightX = (float)((double)t[2] * z[6] + (double)t[1] * z[3] + (double)t[0] * z[0] + z[9]);
        g_vbLightY = (float)((double)t[2] * z[7] + (double)t[1] * z[4] + (double)t[0] * z[1] + z[10]);
        // (the original registers an empty atexit handler here)
    }
    hdr[0] = 0;
    hdr[1] = *fmt;
    hdr[3] = (int)v;
    hdr[2] = n;
    if (!g_vb720904 || !*fmt || !n)
        return;
    F(WORD, s, 0) = (WORD)n;
    BYTE k = F(BYTE, r, 24) + 1;
    F(BYTE, s, 12) = k;
    if (k)
        F(void*, s, 16) = operator new(k * n * 8);
    BYTE* d = (BYTE*)operator new(96);
    F(BYTE*, s, 4) = d;
    memset(d, 0, 96);
    F(void*, d, 24) = (BYTE*)g_vb723a9c;
    F(void*, d, 16) = F(void*, d, 24);
    for (int i = 0; i < n; i++) {
        float* p = v + i * 3;
        BYTE* q = r + i * 28;
        if (*fmt == 1) {
            DWORD c = ((DWORD)(int)((double)p[0] * g_vb64c458) | 0xFFFFFF00) << 8;
            c = (c | (int)((double)p[1] * g_vb64c458)) << 8;
            F(DWORD, p, 0) = c | (int)((double)p[2] * g_vb64c458);
        } else if (*fmt == 2 && !(g_sc702e8c & 64)) {
            float l = (float)(((double)p[1] * g_vbLightY + (double)p[2] * g_vbLightZ + (double)g_vbLightX * p[0] +
                               g_vb64b670) * g_vb64b6e0);
            p[2] = l;
            p[1] = l;
            p[0] = l;
            *fmt = 3;
            hdr[1] = 3;
        }
        DWORD* uv = F(DWORD*, s, 16);
        uv[F(BYTE, s, 12) * i * 2] = F(DWORD, q, 12);
        uv[F(BYTE, s, 12) * i * 2 + 1] = F(DWORD, q, 16);
        for (int j = 0; j < F(BYTE, s, 12) - 1; j++) {
            DWORD* e = F(DWORD*, s, 16) + (F(BYTE, s, 12) * i + j) * 2;
            e[2] = F(DWORD*, q, 20)[j * 2];
            e[3] = F(DWORD*, q, 20)[j * 2 + 1];
        }
    }
    d = F(BYTE*, s, 4);
    F(BYTE*, d, 0) = r;
    F(int, F(BYTE*, s, 4), 4) = 28;
    F(int, s, 8) = 2;
    for (int c = 0; c < F(BYTE, s, 12); c++) {
        F(BYTE*, F(BYTE*, s, 4), 32 + c * 8) = F(BYTE*, s, 16) + c * 8;
        F(int, F(BYTE*, s, 4), 36 + c * 8) = F(BYTE, s, 12) * 8;
    }
    if (*fmt == 2) {
        F(int, s, 8) |= 16;
        F(float*, F(BYTE*, s, 4), 8) = v;
        F(int, F(BYTE*, s, 4), 12) = 12;
    } else {
        F(int, s, 8) |= 64;
        F(float*, F(BYTE*, s, 4), 16) = v;
        F(int, F(BYTE*, s, 4), 20) = 12;
    }
}

} // extern "C"

// === reading a portal (5fc360)

extern "C" void FUN_5ab400(); KING_ALIAS("_FUN_5ab400", "_Vec3_Normalized")
ASM_VAR(AsmLabel, s_sce683c00, $L_683c00)
ASM_VAR(double, g_po64b5b8, $L_64b5b8)
ASM_VAR(float, g_po64c264, $L_64c264)

extern "C" {

// [$L_5fc360] reads portal s from file f: its plane (+112), its target name (32 bytes; "room:node" splits into +132
// and +164 and sets +40 bit 1, else it is looked up into +128 and a name starting like 683c00 sets +108) and two
// corners a, b: the four corners at +60 (a, then across, b, back) and the normal at +48; a level portal (a, b at
// the same height) spans x and y, else a wall from a's height to b's
int __fastcall Portal_Read(BYTE* s, void*, void* f, int, int)
{
    float a[3] = {0, 0, 0}, b[3] = {0, 0, 0};
    CallThis(FUN_5ac7f0, s + 112, f);
    char buf[32];
    CallC(Sce_fread, buf, 32, 1, f);
    int i = 0;
    for (char c = buf[0]; c && c != ':'; c = buf[++i])
        ;
    F(int, s, 128) = 0;
    if (!buf[i]) {
        if (F(char*, s, 12) && !CallC<int>(Sce_strncmp, F(char*, s, 12), (char*)s_sce683c00, 6))
            F(int, s, 108) = 1;
        CallThis(FUN_5d2650, g_sce71e820, buf, s + 128);
    } else {
        F(BYTE, s, 40) |= 2;
        buf[i] = 0;
        strcpy((char*)s + 132, buf);
        strcpy((char*)s + 164, buf + i + 1);
    }
    CallThis(FUN_5ab790, a, f);
    CallThis(FUN_5ab790, b, f);
    float c1[3], c3[3], n[3];
    if (fabs((double)a[2] - b[2]) < g_po64b5b8) {
        double t0 = (double)b[0] - b[0], t1 = (double)b[1] - a[1], t2 = (double)b[2] - b[2];
        float t3 = (float)((double)a[0] - b[0]);
        double t4 = (double)a[2] - b[2];
        n[0] = (float)(t2 * 0.0 - t1 * t4);
        n[1] = (float)(t4 * t0 - t2 * t3);
        n[2] = (float)(t1 * t3 - 0.0 * t0);
        c1[0] = b[0], c1[1] = a[1], c1[2] = b[2];
        c3[0] = a[0], c3[1] = b[1], c3[2] = b[2];
    } else {
        double u0 = (double)b[0] - b[0], u1 = (double)b[1] - b[1], u2 = (double)b[2] - a[2];
        float u3 = (float)((double)a[0] - b[0]), u4 = (float)((double)a[1] - b[1]);
        double z = g_po64c264;
        n[0] = (float)(u2 * u4 - u1 * z);
        n[1] = (float)(z * u0 - u2 * u3);
        n[2] = (float)(u1 * u3 - u4 * u0);
        c1[0] = b[0], c1[1] = b[1], c1[2] = a[2];
        c3[0] = a[0], c3[1] = a[1], c3[2] = b[2];
    }
    float nn[3];
    memcpy(s + 48, CallC<float*>(FUN_5ab400, nn, n), 12);
    memcpy(s + 60, a, 12);
    memcpy(s + 72, c1, 12);
    memcpy(s + 84, b, 12);
    memcpy(s + 96, c3, 12);
    return 1;
}

} // extern "C"

// === sorting an item into a split node (5f4530)

extern "C" void FUN_5cda50(); KING_ALIAS("_FUN_5cda50", "@Shade_Assign@12")
ASM_VAR(AsmLabel, g_vtbl64c7dc, $L_64c7dc)

// the plane p at a point v: (p.n . v) + p.d
static double Bsp_At(const float* p, const float* v)
{
    return (double)p[2] * v[2] + (double)p[1] * v[1] + (double)p[0] * v[0] + p[3];
}

// offers o to the children of side k of s (+48[k], next +8; vtbl+28), else s's own vtbl+20 (k, o)
static void Bsp_Give(BYTE* s, int k, BYTE* o)
{
    for (BYTE* c = F(BYTE**, s, 48)[k]; c; c = F(BYTE*, c, 8))
        if (CallVirt<int>(c, 28, o))
            return;
    CallVirt(s, 20, k, o);
}

extern "C" {

// [$L_5f4530] sorts item o into the side of split node s's plane (+56) its bounding sphere (+20) lies on (0 in
// front, 2 behind); a plain item (kind 17, no piece yet) crossing it (unless 702e8c bit 14) gets a piece (+76) for
// the other side, each clipped by the plane (+48) facing its side
int __fastcall Bsp_SortItem(BYTE* s, void*, BYTE* o)
{
    float c[3] = {0, 0, 0};
    CallVirt(o, 40, c);
    const float* p = (const float*)(s + 56);
    float sp[4];
    memcpy(sp, o + 20, 16);
    int side, other = 0;
    if ((double)F(float, s, 64) * c[2] + (double)F(float, s, 60) * c[1] + (double)p[0] * c[0] + p[3] - sp[3] >
        g_sce64b678)
        side = 0;
    else if ((double)p[2] * c[2] + (double)p[1] * c[1] + (double)p[0] * c[0] + sp[3] + p[3] < g_sce64b678)
        side = 2;
    else if (g_sc702e8c & 0x4000)
        side = 0;
    else {
        if (Bsp_At(p, c) > g_sce64b678)
            side = 0, other = 2;
        else
            side = 2, other = 0;
        if (F(int, o, 16) == 17 && !F(int, o, 80) && !F(int, o, 76)) {
            CallVirt(o, 40, sp);
            double e = Bsp_At(p, sp);
            if (e < 0)
                e = -e;
            if (sp[3] > e) {
                BYTE* n = (BYTE*)operator new(88);
                if (n) {
                    CallThis(FUN_5ef6d0, n, (void*)&g_q195_68d1c0, 17);
                    F(int, n, 48) = F(int, n, 52) = F(int, n, 56) = F(int, n, 60) = 0;
                    F(void*, n, 0) = (BYTE*)g_vtbl64c7dc;
                    F(BYTE*, n, 64) = o;
                    F(int, n, 68) = F(int, n, 72) = F(int, n, 76) = F(int, n, 80) = 0;
                    F(int, n, 84) = -1;
                    F(int, n, 36) = F(int, o, 36);
                }
                F(BYTE, n, 40) |= F(BYTE, o, 40) & 1;
                memcpy(n + 20, sp, 16);
                F(int, n, 80) = 1;
                float neg[4] = {-p[0], -p[1], -p[2], -p[3]};
                if (other == 2) {
                    CallThis(FUN_5cda50, n + 48, neg);
                    CallThis(FUN_5cda50, o + 48, p);
                } else {
                    CallThis(FUN_5cda50, n + 48, p);
                    CallThis(FUN_5cda50, o + 48, neg);
                }
                F(BYTE*, o, 76) = n;
                Bsp_Give(s, other, n);
            }
        }
    }
    Bsp_Give(s, side, o);
    return 1;
}

} // extern "C"

// === reading lights (5f3a60, 5f3bc0)

ASM_VAR(double, g_lr64d900, $L_64d900)
ASM_VAR(float, g_lrUp, $L_702630)

extern "C" {

// [$L_5f3a60] reads light node s from file f: its plane (+20), +136, the on flag (+140), the kind (0..3 to +56;
// directional +128 = 210, spot 130), position (+60), direction (+72), colour and attenuation (+84..+96), the cone
// (+100 at most pi, +104 so the two make at most 64d900) and +108..+116; then the node's part (a, b)
int __fastcall Light_Read(BYTE* s, void*, int f, int a, int b)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    F(int, s, 136) = CallC<int>(FUN_5ab140, f);
    F(int, s, 140) = CallC<int>(FUN_5ab140, f) ? 1 : 0;
    switch (CallC<int>(FUN_5ab140, f)) {
    case 0:
        F(int, s, 56) = 0;
        break;
    case 1:
        F(int, s, 56) = 1;
        F(int, s, 128) = 210;
        break;
    case 2:
        F(int, s, 56) = 2;
        break;
    case 3:
        F(int, s, 56) = 3;
        F(int, s, 128) = 130;
        break;
    }
    CallThis(FUN_5ab790, s + 60, f);
    CallThis(FUN_5ab790, s + 72, f);
    for (int o = 84; o <= 96; o += 4)
        F(float, s, o) = (float)CallC<double>(FUN_5ab190, f);
    double c = CallC<double>(FUN_5ab190, f);
    F(float, s, 100) = (float)c;
    if (c > g_lr64d900)
        F(DWORD, s, 100) = 0x40490FDA;
    c = CallC<double>(FUN_5ab190, f);
    F(float, s, 104) = (float)c;
    if (c + F(float, s, 100) > g_lr64d900)
        F(float, s, 104) = (float)(g_lr64d900 - F(float, s, 100));
    for (int o = 108; o <= 116; o += 4)
        F(float, s, o) = (float)CallC<double>(FUN_5ab190, f);
    return Sce_LoadTail(s, f, 1, a, b);
}

// (the entry 5f3bc0 inside it) reads the older light node: its plane (+20, its normal also the position +60), the kind by name
// ("direction"/"direct" 1, "position" 2, "spot" 3, else 0), shining down; full colour, cone pi/4, attenuation
// 1 / (+32)^2 unless directional; then the node's part (a, b)
int __fastcall Light_ReadOld(BYTE* s, void*, int f, int a, int b)
{
    CallThis(FUN_5ac7f0, s + 20, f);
    char name[32];
    CallC(Sce_fread, name, 32, 1, f);
    if (!strcmp(name, "direction") || !strcmp(name, "direct"))
        F(int, s, 56) = 1;
    else if (!strcmp(name, "position"))
        F(int, s, 56) = 2;
    else if (!strcmp(name, "spot"))
        F(int, s, 56) = 3;
    else
        F(int, s, 56) = 0;
    memcpy(s + 60, s + 20, 12);
    const float* u = &g_lrUp;
    F(float, s, 72) = -u[0];
    F(float, s, 76) = -u[1];
    F(DWORD, s, 100) = 0x3F490FDA;
    F(float, s, 84) = 1.0f;
    F(float, s, 80) = -u[2];
    F(int, s, 88) = 0;
    if (F(int, s, 56) == 1)
        F(int, s, 92) = 0;
    else {
        double r = F(float, s, 32);
        F(float, s, 92) = (float)(g_q182_64c250 / (r * r));
    }
    return Sce_LoadTail(s, f, 1, a, b);
}

} // extern "C"

// === whether a point is in a volume (FUN_5f88d0)

// p less the shape's point (+0), as floats
static void Vt_Rel(float* v, const float* p, const float* g)
{
    v[0] = (float)((double)p[0] - g[0]);
    v[1] = (float)((double)p[1] - g[1]);
    v[2] = (float)((double)p[2] - g[2]);
}

extern "C" {

// [FUN_5f88d0] whether point p is in volume s (kind +0, shape +8): 0 everywhere, 1 inside or 2 outside a sphere
// (+12 the radius), 3 inside or 4 outside a cylinder (+12 the axis, +24 the radius, +28 the half length), 5 in
// front of a plane; else never
int __fastcall Volume_Contains(DWORD* s, void*, const float* p)
{
    const float* g = (const float*)s[2];
    float v[3];
    switch (s[0]) {
    case 0:
        return 1;
    case 1:
    case 2: {
        Vt_Rel(v, p, g);
        double d = CallC<double>(FUN_5ab4d0, v);
        return s[0] == 1 ? !(d > g[3]) : d > g[3];
    }
    case 3:
    case 4: {
        Vt_Rel(v, p, g);
        double t = (double)g[5] * v[2] + (double)g[4] * v[1] + (double)g[3] * v[0];
        double at = t < 0 ? -t : t;
        if (g[7] < at)
            return s[0] == 4;
        float w[3];
        if (s[0] == 3) {
            float py = (float)(t * g[4]), pz = (float)(t * g[5]);
            w[0] = (float)(v[0] - t * g[3]);
            w[1] = (float)((double)v[1] - py);
            w[2] = (float)((double)v[2] - pz);
            return !(CallC<double>(FUN_5ab4d0, w) > g[6]);
        }
        float q[3] = {(float)(t * g[3]), (float)(t * g[4]), (float)(t * g[5])};
        Vt_Rel(w, v, q);
        return CallC<double>(FUN_5ab4d0, w) > g[6];
    }
    case 5:
        return (double)g[2] * p[2] + (double)g[1] * p[1] + (double)p[0] * g[0] + g[3] > g_sce64b678;
    }
    return 0;
}

} // extern "C"

// === finding the music files (FUN_5ff5e0)

extern "C" void Mf_Allocate(); KING_ALIAS("_Mf_Allocate", "_Stl_Allocate")
extern "C" void Mf_PoolFree(); KING_ALIAS("_Mf_PoolFree", "_Ai_PoolFree")
extern "C" void Mf_Copy1(); KING_ALIAS("_Mf_Copy1", "_Copy1")
extern "C" void Mf_InsertRange(); KING_ALIAS("_Mf_InsertRange", "@CharVec_InsertRange44c220@20")
extern "C" void Mf_Reserve(); KING_ALIAS("_Mf_Reserve", "@Vec1_Reserve@16")
extern "C" void Mf_FilterConstruct(); KING_ALIAS("_Mf_FilterConstruct", "@Filter1_Construct@8")
extern "C" void Mf_FilterSetLength(); KING_ALIAS("_Mf_FilterSetLength", "@Filter1_SetLength@12")
extern "C" void Mf_MusicConstruct(); KING_ALIAS("_Mf_MusicConstruct", "_Music_Construct")
extern "C" void Mf_InsertAux(); KING_ALIAS("_Mf_InsertAux", "@Vec56_InsertAux@16")

struct Mf_Vec { BYTE* begin; BYTE* end; BYTE* cap; };

// a copy of the music directory (723d38) with the string s (and its 0) appended
static void Mf_Path(Mf_Vec* v, const char* s)
{
    Mf_Vec* dir = (Mf_Vec*)(BYTE*)g_q47_723d38;
    int n = (int)(dir->end - dir->begin);
    BYTE* p = n ? CallC<BYTE*>(Mf_Allocate, n) : 0;
    v->begin = v->end = p;
    v->cap = p + n;
    v->end = CallC<BYTE*>(Mf_Copy1, dir->begin, dir->end, p);
    CallThis(Mf_InsertRange, v, v->end, s, s + strlen(s) + 1);
}

// frees the storage of v
static void Mf_Free(Mf_Vec* v)
{
    if (int n = (int)(v->cap - v->begin))
        CallC(Mf_PoolFree, v->begin, n);
}

extern "C" {

// [FUN_5ff5e0] adds a music record (its path, a 4-step filter, 0.1 and 0) for each file matching pattern in the
// music directory to the list 723d28
void __cdecl Music_FindFiles(const char* pattern)
{
    Mf_Vec path;
    Mf_Path(&path, pattern);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((const char*)path.begin, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        char fill = 0;
        do {
            Mf_Vec name;
            Mf_Path(&name, fd.cFileName);
            BYTE rec[56];
            CallThis(Mf_Reserve, rec, (int)(name.end - name.begin), &fill);
            F(BYTE*, rec, 4) = CallC<BYTE*>(Mf_Copy1, name.begin, name.end, F(BYTE*, rec, 0));
            CallThis(Mf_FilterConstruct, rec + 16);
            F(double, rec, 40) = 0.1;
            F(double, rec, 48) = 0;
            CallThis(Mf_FilterSetLength, rec + 16, 4);
            Mf_Vec* list = (Mf_Vec*)(BYTE*)g_q47_723d28;
            if (list->end != list->cap) {
                CallC(Mf_MusicConstruct, list->end, rec);
                list->end += 56;
            } else
                CallThis(Mf_InsertAux, list, list->end, rec);
            Mf_Free((Mf_Vec*)rec);
            Mf_Free(&name);
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    Mf_Free(&path);
}

} // extern "C"

// === an item crossing a portal (5fb9e0)

// a piece of item o for the other side of a plane, kind 17 (vtable 64c7dc)
static BYTE* Pc_NewPiece(BYTE* o)
{
    BYTE* n = (BYTE*)operator new(88);
    if (n) {
        CallThis(FUN_5ef6d0, n, (void*)&g_q195_68d1c0, 17);
        F(int, n, 48) = F(int, n, 52) = F(int, n, 56) = F(int, n, 60) = 0;
        F(void*, n, 0) = (BYTE*)g_vtbl64c7dc;
        F(BYTE*, n, 64) = o;
        F(int, n, 68) = F(int, n, 72) = F(int, n, 76) = F(int, n, 80) = 0;
        F(int, n, 84) = -1;
        F(int, n, 36) = F(int, o, 36);
    }
    return n;
}

extern "C" {

// [$L_5fb9e0] a plain item o (kind 17, no piece yet) whose bounding sphere crosses portal s's plane (+48, through
// +60) close to its opening (+84.. the reach) gets a piece for the room behind (+128 vtbl+28), each clipped by
// the plane facing its side (unless 702e8c bit 14); 0
int __fastcall Portal_SplitItem(BYTE* s, void*, BYTE* o)
{
    if (!F(BYTE*, s, 128) || (g_sc702e8c & 0x4000) || F(int, o, 16) != 17 || F(int, o, 80) || F(int, o, 76))
        return 0;
    float sp[4];
    memcpy(sp, o + 20, 16);
    CallVirt(o, 40, sp);
    const float* n = (const float*)(s + 48);
    const float* p = (const float*)(s + 60);
    double d0 = (-(double)p[1] * n[1] + -(double)p[2] * n[2]) + -(double)p[0] * n[0];
    double dist = (double)n[2] * sp[2] + (double)n[1] * sp[1] + (double)n[0] * sp[0] + d0;
    if (sp[3] < (dist < 0 ? -dist : dist))
        return 0;
    float qz = (float)(dist * n[2]), qy = (float)(dist * n[1]);
    double qx = dist * n[0];
    float pz = (float)((double)sp[2] - qz), py = (float)((double)sp[1] - qy), px = (float)(sp[0] - qx);
    if (!Near3(px, p[0], F(float, s, 84)) || !Near3(py, p[1], F(float, s, 88)) || !Near3(pz, p[2], F(float, s, 92)))
        return 0;
    BYTE* c = Pc_NewPiece(o);
    F(BYTE, c, 40) |= F(BYTE, o, 40) & 1;
    memcpy(c + 20, sp, 16);
    F(int, c, 80) = 1;
    float front[4] = {n[0], n[1], n[2], (float)d0};
    CallThis(FUN_5cda50, c + 48, front);
    float back[4] = {-n[0], -n[1], -n[2], (float)-d0};
    CallThis(FUN_5cda50, o + 48, back);
    F(BYTE*, o, 76) = c;
    CallVirt(F(BYTE*, s, 128), 28, c);
    return 0;
}

} // extern "C"

// === finding the paths that join one (5fc780)

// whether the positions (+36 of two frames) are less than 1 apart
static bool Pj_Near(const BYTE* a, const BYTE* b)
{
    float d[3] = {(float)((double)F(float, a, 36) - F(float, b, 36)), (float)((double)F(float, a, 40) - F(float, b, 40)),
                  (float)((double)F(float, a, 44) - F(float, b, 44))};
    return CallC<double>(FUN_5ab4d0, d) < g_q182_64c250;
}

extern "C" {

// [$L_5fc780] for path group node g (kind 14, its paths +128 +64): the paths with an end at path 723aa0's first
// frame join it at its start (+64, count +56), those with an end at its last frame at its end (+104, count +60); -1
int __cdecl Path_FindJoins(BYTE* g)
{
    if (F(int, g, 16) != 14 || !F(BYTE*, g, 128) || !F(BYTE*, F(BYTE*, g, 128), 64))
        return -1;
    for (BYTE* p = *F(BYTE**, F(BYTE*, g, 128), 64); p; p = Ctl_Owner(p, 0)) {
        BYTE* c = g_723aa0;
        BYTE first[48], last[48], pf[48], pl[48];
        memcpy(first, F(BYTE*, c, 8), 48);
        memcpy(last, F(BYTE*, c, 8) + F(int, c, 4) * 48 - 48, 48);
        memcpy(pf, F(BYTE*, p, 8), 48);
        memcpy(pl, F(BYTE*, p, 8) + F(int, p, 4) * 48 - 48, 48);
        if (Pj_Near(first, pf) || Pj_Near(first, pl)) {
            F(BYTE*, g_723aa0, 64 + F(int, g_723aa0, 56) * 4) = p;
            F(int, g_723aa0, 56)++;
        } else if (Pj_Near(last, pl) || Pj_Near(last, pf)) {
            F(BYTE*, g_723aa0, 104 + F(int, g_723aa0, 60) * 4) = p;
            F(int, g_723aa0, 60)++;
        }
    }
    return -1;
}

} // extern "C"

// === stereo sounds at a point (FUN_5fd5f0, FUN_5fd870)

ASM_PROC(FUN_5fdaf0)    // updates a sound's two channel volumes
extern "C" void Ss_RotZ(); KING_ALIAS("_Ss_RotZ", "_Matrix_RotationZ")
ASM_VAR(double, g_ss64b6d0, $L_64b6d0)
ASM_VAR(double, g_ss64b670, $L_64b670)
ASM_VAR(double, g_ss64b678, $L_64b678)
ASM_VAR(double, g_ss652d60, $L_652d60)
ASM_VAR(double, g_ss651690, $L_651690)
ASM_VAR(float, g_ss702640, $L_702640)
struct Ss_V3 { float x, y, z; };

// v moved by matrix m
static Ss_V3 Ss_Move(const float* m, const float* v)
{
    Ss_V3 r;
    r.x = (float)((double)v[2] * m[6] + (double)v[1] * m[3] + (double)v[0] * m[0] + m[9]);
    r.y = (float)((double)v[2] * m[7] + (double)v[1] * m[4] + (double)v[0] * m[1] + m[10]);
    r.z = (float)((double)v[2] * m[8] + (double)v[1] * m[5] + (double)v[0] * m[2] + m[11]);
    return r;
}

static double Ss_Clamp(double x)
{
    if (x < g_ss64b678)
        return 0;
    if (x > g_ss64b670)
        return 1.0;
    return x;
}

// the gains of the left and right ears (the side 702640 turned by -+pi/8) for a sound at p within range; 0 out of it
static bool Ss_Gains(Ss_V3 p, double range, double* l, double* r)
{
    double d = CallC<double>(FUN_5ab4d0, &p);
    if (!(d < range * g_ss64b6d0))
        return false;
    float m[12];
    const float* side = &g_ss702640;
    Ss_V3 rt = Ss_Move(CallC<float*>(Ss_RotZ, m, 0.392699093f), side);
    float neg[3] = {-side[0], -side[1], -side[2]};
    Ss_V3 lt = Ss_Move(CallC<float*>(Ss_RotZ, m, -0.392699093f), neg);
    Ss_V3 u;
    CallC(FUN_5ab400, &u, &p);
    double gl = ((double)u.z * lt.z + (double)u.y * lt.y + (double)lt.x * u.x + g_ss64b670) * g_ss652d60 + g_ss651690;
    double gr = ((double)u.z * rt.z + (double)u.y * rt.y + (double)u.x * rt.x + g_ss64b670) * g_ss652d60 + g_ss651690;
    if (range < g_ss64b670)
        range = g_ss64b670;
    double att = d / range + g_ss64b670;
    *l = Ss_Clamp(gl / att);
    *r = Ss_Clamp(gr / att);
    return true;
}

extern "C" {

// [FUN_5fd5f0] plays sound s at p (from the listener) within range at volume vol, panned between the ears (a3 to
// Sound_Play)
void __fastcall Sound_PlayAt(BYTE* s, void*, Ss_V3 p, double range, double vol, int a3)
{
    double l, r;
    if (Ss_Gains(p, range, &l, &r))
        Sound_Play(s, 0, (float)(l * vol), (float)(r * vol), a3);
}

// [FUN_5fd870] the same for the channel volumes of a playing sound (5fdaf0)
void __fastcall Sound_UpdateAt(BYTE* s, void*, Ss_V3 p, double range, double vol)
{
    double l, r;
    if (Ss_Gains(p, range, &l, &r))
        CallThis(FUN_5fdaf0, s, (float)(l * vol), (float)(r * vol));
}

} // extern "C"

// === drawing children far to near (5fb550)

extern "C" BYTE* __fastcall Render_Material(BYTE* self, void*);    // renderer.cpp
ASM_VAR(double, g_fn64b748, $L_64b748)

// the distance from the camera c of node n's centre (vtbl+40) in renderer r's frame
static double Fn_Dist(BYTE* n, BYTE* r, const float* c)
{
    float p[3];
    int on = (int)CallVirt<BYTE*>(n, 40, p);
    Node_TransformByFrame(p, on, r);
    float d[3] = {(float)((double)c[0] - p[0]), (float)((double)c[1] - p[1]), (float)((double)c[2] - p[2])};
    return CallC<double>(FUN_5ab4d0, d);
}

extern "C" {

// [$L_5fb550] draws node s's children (+48, next +8) with (a, r) after sorting them nearest the camera first (swaps
// until none is needed, at most 10000 passes); 0 when the test fails
int __fastcall Node_DrawSorted(BYTE* s, void*, int a, BYTE* r)
{
    if (!CallThis<int>(FUN_5ef800, s, a, r))
        return 0;
    float c[3];
    memcpy(c, Render_Material(r, 0), 12);
    BYTE* n = *F(BYTE**, s, 48);
    if (!n)
        return 1;
    for (int pass = 0;; pass++) {
        bool swapped = false;
        float dp = (float)Fn_Dist(n, r, c);
        for (BYTE* m = F(BYTE*, n, 8); m; m = F(BYTE*, n, 8)) {
            double dm = Fn_Dist(m, r, c);
            if (dp - g_fn64b748 > dm) {
                Node_Remove(n, 0);
                F(int, n, 44) = F(int, m, 44);
                CallThis(FUN_5d2190, n, m);
                F(int, n, 36) = F(int, m, 36);
                swapped = true;
            } else {
                dp = (float)dm;
                n = m;
            }
        }
        if (pass > 10000 || !swapped)
            break;
        n = *F(BYTE**, s, 48);
        if (!n)
            return 1;
    }
    CallThis(FUN_5f2080, s, 0, a, r);
    return 1;
}

} // extern "C"

// === choosing the next music track (FUN_5ff3b0)

extern "C" void Mt_FilterUpdate(); KING_ALIAS("_Mt_FilterUpdate", "@Filter1_Update@16")
extern "C" int __fastcall Music_Run(BYTE* self, void*, int unused);       // sound_gate.cpp
extern "C" int __fastcall Music_SetVolume(BYTE* self, void*, double v);
extern "C" int __fastcall Music_Open(BYTE* self, void*, const char* path);
ASM_VAR(double, g_mt64b678, $L_64b678)
ASM_VAR(double, g_mt64b6d8, $L_64b6d8)

extern "C" {

// [FUN_5ff3b0] player s picks the next track of the music list (723d28, 56-byte records: +0 the path, +16 the
// recent-play filter, +40 the weight, +48 the filter's value) at random by weight; every filter is fed (1 for the
// pick, 0 else) and the weights become how long ago each played; then the pick is opened and played on s's music
// (+20) at its volume (+24); 0
int __fastcall Music_PickNext(BYTE* s)
{
    Mf_Vec* list = (Mf_Vec*)(BYTE*)g_q47_723d28;
    BYTE* b = list->begin;
    BYTE* e = list->end;
    double total = g_mt64b678;
    for (BYTE* r = b; r != e; r += 56)
        total += F(double, r, 40);
    DWORD* t = (DWORD*)g_sce723b58;
    g_sce723c34 = (g_sce723c34 + 1) % 55;
    g_sce723c38 = (g_sce723c38 + 1) % 55;
    t[g_sce723c34] -= t[g_sce723c38];
    double x = (double)(__int64)(t[g_sce723c34] % 1000000) * total * g_mt64b6d8;
    double acc = g_mt64b678;
    BYTE* pick = e;
    for (BYTE* r = b; r != e; r += 56) {
        if (!(x < acc) && !(acc + F(double, r, 40) < x)) {
            pick = r;
            break;
        }
        acc += F(double, r, 40);
    }
    for (BYTE* r = b; r != pick; r += 56)
        F(double, r, 48) = CallThis<double>(Mt_FilterUpdate, r + 16, 0.0);
    F(double, pick, 48) = CallThis<double>(Mt_FilterUpdate, pick + 16, 1.0);
    for (BYTE* r = pick + 56; r != list->end; r += 56)
        F(double, r, 48) = CallThis<double>(Mt_FilterUpdate, r + 16, 0.0);
    for (BYTE* r = list->begin; r != list->end; r += 56)
        F(double, r, 48) = F(double, r, 48) * F(double, r, 48);
    double mx = g_mt64b678;
    for (BYTE* r = list->begin; r != list->end; r += 56)
        if (mx < F(double, r, 48))
            mx = F(double, r, 48);
    for (BYTE* r = list->begin; r != list->end; r += 56)
        F(double, r, 40) = (mx - F(double, r, 48)) / mx;
    Music_Open(F(BYTE*, s, 20), 0, F(const char*, pick, 0));
    Music_Run(F(BYTE*, s, 20), 0, 0);
    Music_SetVolume(F(BYTE*, s, 20), 0, F(double, s, 24));
    return 0;
}

} // extern "C"
