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

ASM_PROC(FUN_5f2a40)
extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")    // base node constructor (name, kind)
ASM_PROC(FUN_5e5840)
ASM_PROC(FUN_5f1a70)
ASM_PROC(FUN_5f4a90)
extern "C" void FUN_5e5cb0(); KING_ALIAS("_FUN_5e5cb0", "@Obj5e5cb0_Init@8")
ASM_PROC(FUN_5ef660)
ASM_VAR(AsmLabel, g_dtor5a3df0, $L_5a3df0)
ASM_VAR(AsmLabel, g_dtor5e57a0, $L_5e57a0)
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
ASM_PROC(FUN_604730)
ASM_PROC(FUN_604d70)
ASM_PROC(FUN_605130)
extern "C" void FUN_604ed0(); KING_ALIAS("_FUN_604ed0", "_Snd_Free")
extern "C" void FUN_5e98c0(); KING_ALIAS("_FUN_5e98c0", "_Data_Read5e99c0")
extern "C" void FUN_5e99c0(); KING_ALIAS("_FUN_5e99c0", "_Image_Read")
ASM_FUNC(Mixer_Close, $L_604a40)
ASM_FUNC(Scene_EachCallback, $L_5fc780)

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
ASM_PROC(FUN_604a80)
ASM_PROC(FUN_604730)
ASM_PROC($L_5fcdc0)
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

