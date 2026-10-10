// clusters.cpp - small functions of the unit at 0x5ea5d0 of king.masm: the
// rooms of the scene (*.rmp clusters): their node lists (+64 objects,
// +68 frames), the counted object arrays of a cluster, palette switching,
// and the lookup of scene objects by name.
//
// Building with  nmake ASM_CLUSTERS=1  makes king.masm assemble the original
// procedures again.

#include "king.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl fgetc(FILE* f);

extern "C" void __fastcall Node_Link(BYTE* self, void*, BYTE* list);                // core.cpp
extern "C" void __fastcall Group_EachOwner(BYTE* self, void*);                      // scene.cpp
extern "C" int __fastcall Hash_Lookup(BYTE* self, void*, const char* name, int* out); // core.cpp

ASM_PROC(FUN_5eb310)
ASM_PROC(FUN_5acd40)    // base constructor
ASM_PROC(FUN_5bad60)    // size of an object
ASM_PROC(FUN_5bbef0)    // size of an object
ASM_PROC(FUN_5edeb0)    // base constructor (a, b)
ASM_PROC(FUN_5eddf0)    // base destructor
ASM_PROC(FUN_5b64e0)    // selects a palette
ASM_FUNC(Scene_FindCallback, $L_5ef310)

ASM_VAR(AsmLabel, g_vtbl65274c, $L_65274c)
ASM_VAR(AsmLabel, g_vtbl652538, $L_652538)
ASM_VAR(AsmLabel, g_vtbl652760, $L_652760)
ASM_VAR(AsmLabel, g_vtbl6527e4, $L_6527e4)
ASM_VAR(AsmLabel, g_objectNames, $L_71e820)   // name hash of the scene objects

extern "C" {

// [FUN_5ea5d0]
BYTE* __fastcall Cluster_Construct(BYTE* self, void*)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl65274c;
    static const int z[] = {56, 152, 36, 52, 60, 148, 132, 96, 80, 88, 72, 104, 112, 32, 28, 24, 20, 16, 12,
                            64, 68, 76, 84, 92, 100, 108, 116, 124, 128, 120, 140, 144, 136};
    for (int k = 0; k < sizeof(z) / sizeof(z[0]); k++)
        F(int, self, z[k]) = 0;
    for (int k = 0; k < 6; k++)
        F(int, self, 156 + k * 4) = 0;
    return self;
}

// [FUN_5ea990]
void __fastcall Room_EachGroup(BYTE* self, void*)
{
    for (BYTE* n = F(BYTE*, self, 64); n; n = F(BYTE*, n, 8))
        if (F(int, n, 16) == 15)
            Group_EachOwner(n, 0);
}

// [FUN_5ead00] calls vtbl+32 (a, b) of every object and frame
void __fastcall Room_Each(BYTE* self, void*, void* a, void* b)
{
    for (BYTE* n = F(BYTE*, self, 64); n; n = F(BYTE*, n, 8))
        CallVirt(n, 32, a, b);
    for (BYTE* n = F(BYTE*, self, 68); n; n = F(BYTE*, n, 8))
        CallVirt(n, 32, a, b);
}

// [FUN_5eb1e0]
void __fastcall Camera_Init(BYTE* self, void*)
{
    DWORD* d = (DWORD*)self;
    d[116 / 4] = -1;
    d[88 / 4] = -1;
    d[84 / 4] = -1;
    d[16 / 4] = 0;
    d[24 / 4] = 0;
    d[32 / 4] = 0;
    d[40 / 4] = 0;
    d[120 / 4] = 0;
    d[128 / 4] = 0;
    d[48 / 4] = 858993459;
    d[64 / 4] = 0;
    d[56 / 4] = 0;
    d[96 / 4] = 0;
    d[104 / 4] = 0;
    d[4 / 4] = 0;
    d[8 / 4] = 0;
    F(void*, self, 0) = g_vtbl652538;
    d[20 / 4] = 1082925056;
    d[28 / 4] = 1081262080;
    d[72 / 4] = 8;
    d[36 / 4] = 1090021888;
    d[44 / 4] = 1072693248;
    d[124 / 4] = 1072693248;
    d[80 / 4] = 0;
    d[112 / 4] = 100;
    d[76 / 4] = 0;
    d[132 / 4] = 0;
    d[52 / 4] = 1070805811;
    d[68 / 4] = 3220176896;
    d[60 / 4] = 3220176896;
    d[100 / 4] = 0;
    d[108 / 4] = 1072693248;
}

// [FUN_5ee410] switches the palette for the lowest set bit of the pending mask
void __fastcall Palettes_Update(BYTE* self, void*, BYTE* r)
{
    if (!F(int, self, 8))
        return;
    DWORD m = F(DWORD, self, 16);
    F(DWORD, self, 16) = 0xFFFFFFFF;
    if (!m)
        return;
    int i = 0;
    while (!(m & 1)) {
        m >>= 1;
        i++;
    }
    BYTE* t = F(BYTE*, self, 12);
    if (F(int, t, 140) == i || F(int, t, 96) <= i)
        return;
    F(int, t, 140) = i;
    CallThis(FUN_5b64e0, F(BYTE*, r, 1948), F(int*, F(BYTE*, self, 12), 100)[i]);
}

// [FUN_5eb280]
void __fastcall Camera_Render(BYTE* self, void*, BYTE* obj, int a, int b, int c)
{
    CallThis(FUN_5eb310, self, obj, F(int, obj, 76), a, b, c);
    if (F(BYTE*, self, 136))
        Palettes_Update(F(BYTE*, self, 136), 0, (BYTE*)c);
}

// [FUN_5eb2c0]
void __fastcall Camera_Apply(BYTE* self, void*, BYTE* r)
{
    F(int, r, 8264) = F(int, self, 84);
    F(int, r, 1936) = F(int, self, 80);
    F(int, r, 8260) = F(int, self, 76);
    F(int, r, 1940) = F(int, self, 72);
    F(int, r, 8268) = F(int, self, 108);
    F(int, r, 1944) = F(int, self, 104);
    F(BYTE*, r, 8624) = self;
}

// [FUN_5ec0b0] reads a zero-terminated string
void __cdecl File_ReadString(char* out, FILE* f)
{
    int c = fgetc(f);
    while ((char)c && !(F(BYTE, f, 12) & 16)) {
        *out++ = (char)c;
        c = fgetc(f);
    }
    *out = 0;
}

// [FUN_5ed660]
void __fastcall Room_AddNode(BYTE* self, void*, BYTE* n)
{
    if (F(int, n, 16) == 6) {
        if (!F(BYTE*, self, 68))
            F(BYTE*, self, 68) = n;
        else
            Node_Link(n, 0, F(BYTE*, self, 68));
        return;
    }
    if (!F(BYTE*, self, 64))
        F(BYTE*, self, 64) = n;
    else
        Node_Link(n, 0, F(BYTE*, self, 64));
    F(BYTE*, n, 36) = self;
}

// [FUN_5ed970]
BYTE* __fastcall Renderer_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_5acd40, self, a);
    F(void*, self, 0) = g_vtbl652760;
    F(int, self, 8624) = 0;
    return self;
}

// [FUN_5edb90]
int __fastcall Cluster_AddA(BYTE* self, void*, BYTE* o)
{
    if (F(int, self, 48) >= F(int, self, 44))
        return 0;
    F(int, self, 16) += CallThis<int>(FUN_5bad60, o);
    F(BYTE**, self, 68)[F(int, self, 48)] = o;
    F(int, self, 48) = F(int, self, 48) + 1;
    return 1;
}

// [FUN_5edbe0]
int __fastcall Cluster_AddB(BYTE* self, void*, BYTE* o)
{
    if (F(int, self, 56) >= F(int, self, 52))
        return 0;
    F(int, self, 16) += CallThis<int>(FUN_5bbef0, o);
    F(BYTE**, self, 72)[F(int, self, 56)] = o;
    F(int, self, 56) = F(int, self, 56) + 1;
    return 1;
}

// [FUN_5edc30]
int __fastcall Cluster_AddC(BYTE* self, void*, BYTE* o)
{
    int n = F(int, self, 64);
    if (n >= F(int, self, 60))
        return 0;
    F(int, self, 16) += F(int, o, 4);
    F(BYTE**, self, 76)[n] = o;
    F(int, self, 64) = F(int, self, 64) + 1;
    return 1;
}

// [FUN_5ee1d0]
BYTE* __fastcall Texture_Construct(BYTE* self, void*, int a, int b)
{
    CallThis(FUN_5edeb0, self, a, b);
    F(void*, self, 0) = g_vtbl6527e4;
    F(int, self, 44) = 0;
    F(int, self, 48) = 0;
    return self;
}

// [FUN_5ee220]
void __fastcall Texture_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl6527e4;
    if (F(int, self, 48)) {
        GlobalUnlock(F(HGLOBAL, self, 52));
        GlobalFree(F(HGLOBAL, self, 52));
        F(int, self, 48) = 0;
    }
    CallThis(FUN_5eddf0, self);
}

// [FUN_5ee3d0]
int __fastcall Overlay_Draw(BYTE* self, void*, BYTE* obj, DWORD* flags)
{
    *flags &= F(DWORD, self, 16);
    CallVirt(obj, 20);
    CallVirt(F(BYTE*, self, 20), 40);
    CallVirt(obj, 24);
    return 1;
}

// [FUN_5ee470]
int __fastcall Overlay_DrawOn(BYTE* self, void*, BYTE* target, BYTE* obj)
{
    return Overlay_Draw(target, 0, obj, (DWORD*)(self + 16));
}

// [FUN_5ef2b0] the scene object of this name (in scene, or anywhere)
BYTE* __cdecl Scene_FindObject(BYTE* scene, const char* name)
{
    if (scene) {
        int q[3] = {0, (int)name, -1};
        CallVirt(scene, 32, (void*)Scene_FindCallback, q);
        return (BYTE*)q[0];
    }
    int out = (int)name;
    Hash_Lookup((BYTE*)g_objectNames, 0, name, &out);
    return (BYTE*)out;
}

// [FUN_5ef390]
BYTE* __fastcall Room_FindObject(BYTE* self, void*, const char* name)
{
    int q[3] = {0, (int)name, -1};
    Room_Each(self, 0, (void*)Scene_FindCallback, q);
    return (BYTE*)q[0];
}

// [FUN_5ef3d0]
BYTE* __cdecl Objects_Find(const char* name)
{
    int out = (int)name;
    Hash_Lookup((BYTE*)g_objectNames, 0, name, &out);
    return (BYTE*)out;
}

}
