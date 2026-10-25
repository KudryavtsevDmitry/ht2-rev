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

extern "C" void FUN_5eb310(); KING_ALIAS("_FUN_5eb310", "@Camera_Draw@28")
extern "C" void FUN_5acd40(); KING_ALIAS("_FUN_5acd40", "@Render_Construct@12")    // base constructor
extern "C" void FUN_5bad60(); KING_ALIAS("_FUN_5bad60", "@Texture_Size@8")    // size of an object
extern "C" void FUN_5bbef0(); KING_ALIAS("_FUN_5bbef0", "@Bitmap_Bytes@8")    // size of an object
extern "C" void FUN_5edeb0(); KING_ALIAS("_FUN_5edeb0", "@Clu5edeb0_Construct@16")    // base constructor (a, b)
extern "C" void FUN_5eddf0(); KING_ALIAS("_FUN_5eddf0", "@Clu5eddf0_Destroy@4")    // base destructor
extern "C" void FUN_5b64e0(); KING_ALIAS("_FUN_5b64e0", "@Window_SetSurface@12")    // selects a palette
extern "C" int __cdecl Scene_FindCallback(BYTE* n, int* q);

ASM_VAR(AsmLabel, g_vtbl65274c, $L_65274c)
ASM_VAR(AsmLabel, g_vtbl652538, $L_652538)
ASM_VAR(AsmLabel, g_vtbl652760, $L_652760)
ASM_VAR(AsmLabel, g_vtbl6527e4, $L_6527e4)
ASM_VAR(AsmLabel, g_objectNames, $L_71e820)   // name hash of the scene objects

extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);
extern "C" char* __cdecl fgets(char* s, int n, FILE* f);
extern "C" void FUN_5adc80(); KING_ALIAS("_FUN_5adc80", "@Render_SetFog@28")
extern "C" void FUN_5addf0(); KING_ALIAS("_FUN_5addf0", "@Render_SetAmbient@32")
extern "C" void FUN_5eb4b0(); KING_ALIAS("_FUN_5eb4b0", "@CamNode_Apply@12")
extern "C" void FUN_5b0120(); KING_ALIAS("_FUN_5b0120", "_Device_SetStates")
extern "C" void FUN_5d2650(); KING_ALIAS("_FUN_5d2650", "@NameHash_AddList@16")    // name hash: add (name, value pointer)
extern "C" void FUN_5ab1b0(); KING_ALIAS("_FUN_5ab1b0", "_NextToken")    // next token of a string cursor
ASM_VAR(int, g_720904, $L_720904)
ASM_VAR(BYTE*, g_device, $L_720908)
ASM_VAR(AsmLabel, g_listLine, $L_7228ec)
ASM_VAR(char*, g_listCursor, $L_7218d0)
ASM_VAR(char*, g_listToken, $L_7231a0)
ASM_VAR(AsmLabel, s_end, $L_683b50)
ASM_VAR(AsmLabel, s_txr, $L_683b4c)
ASM_VAR(AsmLabel, s_tga, $L_683b48)
ASM_VAR(AsmLabel, s_msk, $L_678acc)
ASM_VAR(AsmLabel, s_wav, $L_683b44)
ASM_VAR(AsmLabel, s_plm, $L_683b40)

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

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

extern "C" {

// [FUN_5eb310] applies the camera's parameters p to the renderer r and draws
// the scene
void __fastcall Camera_Draw(BYTE* self, void*, BYTE* cam, BYTE* scene, BYTE* p, int a, BYTE* r)
{
    if (!cam || !r || !scene)
        return;
    int v = 100 - F(int, p, 112);
    F(int, r, 8456) = v;
    if (v > 100)
        F(int, r, 8456) = 100;
    if (F(int, r, 8456) < 0)
        F(int, r, 8456) = 0;
    F(int, r, 8520) = F(int, p, 120);
    F(int, r, 8524) = F(int, p, 124);
    F(int, r, 8472) = F(int, p, 32);
    F(int, r, 8476) = F(int, p, 36);
    F(int, r, 8464) = F(int, p, 40);
    F(int, r, 8468) = F(int, p, 44);
    CallThis(FUN_5adc80, r, F(int, p, 24), F(int, p, 28), F(int, p, 16), F(int, p, 20), F(int, p, 72));
    CallThis(FUN_5addf0, r, F(int, p, 48), F(int, p, 52), F(int, p, 56), F(int, p, 60), F(int, p, 64),
             F(int, p, 68));
    F(int, r, 8660) = F(int, p, 116);
    F(int, r, 8648) = F(int, p, 128);
    F(int, r, 8652) = F(int, p, 132);
    CallThis(FUN_5eb4b0, cam, r);
    if (g_720904) {
        CallC(FUN_5b0120, F(int, r, 8664) ? 1 : F(int, r, 8668) ? 2 : 0);
        Com(g_device, 80, 22, F(int, cam, 100) ? 3 : 2);
    }
    CallVirt(r, 8, F(int, scene, 36));
    CallVirt(scene, 48, a, r);
    if (g_720904)
        Com(g_device, 80, 7, 0);
}

// [FUN_5eb6a0] reads a cluster's names
int __fastcall Cluster_ReadNames(BYTE* self, void*, FILE* f)
{
    char buf[64];
    fread(buf, 32, 1, f);
    CallThis(FUN_5d2650, (BYTE*)g_objectNames, buf, self + 80);
    fread(buf, 32, 1, f);
    int i = 0;
    while (buf[i] && buf[i] != ':')
        i++;
    if (!buf[i]) {
        CallThis(FUN_5d2650, (BYTE*)g_objectNames, buf, self + 76);
    } else {
        buf[i] = 0;
        strcpy((char*)self + 12, buf);
        strcpy((char*)self + 44, buf + i + 1);
    }
    F(DWORD, self, 88) = 0x3F4CCCCD;     // 0.8f
    F(DWORD, self, 84) = 0x3F19999A;     // 0.6f
    F(int, self, 92) = 0;
    F(int, self, 96) = 0;
    return 1;
}

// [FUN_5ef070] reads a line of a file list: 0 END, 1 texture, 2 mask,
// 3 sound, 7 palette, 8 other; 4 end of file, 5 empty, 6 no extension
int __cdecl FileList_ReadLine(FILE* f, char** name)
{
    fgets(g_listLine, 70, f);
    if (F(BYTE, f, 12) & 16)
        return 4;
    g_listCursor = g_listLine;
    char* tok = CallC<char*>(FUN_5ab1b0, &g_listCursor);
    g_listToken = tok;
    *name = tok;
    if (!tok)
        return 5;
    if (!strcmp(tok, s_end))
        return 0;
    char* p = tok;
    g_listCursor = p;
    while (*p) {
        p++;
        g_listCursor = p;
    }
    while (*p != '.') {
        if (p <= tok)
            return 6;
        p--;
        g_listCursor = p;
    }
    if (p <= tok)
        return 6;
    p++;
    g_listCursor = p;
    if (!strcmp(p, s_txr) || !strcmp(p, s_tga))
        return 1;
    if (!strcmp(p, s_msk))
        return 2;
    if (!strcmp(p, s_wav))
        return 3;
    return 7 + (strcmp(p, s_plm) != 0);
}

}

// ---------------------------------------------------------------------------
ASM_VAR(AsmLabel, s_683604, $L_683604)
ASM_VAR(AsmLabel, s_68381c, $L_68381c)
ASM_VAR(AsmLabel, s_683834, $L_683834)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_5eacc0..$L_5eacd9] appends a value to the array at +100 (count +32, capacity +96); returns its index
extern "C" int __fastcall Group_Push(BYTE* self, void*, int v)
{
    if (F(int, self, 32) == F(int, self, 96))
        Sys_Fatal(s_683604, 0);
    F(int*, self, 100)[F(int, self, 32)] = v;
    return F(int, self, 32)++;
}

// [FUN_5ee490..$L_5ee4b1] deletes the overlay lists at +0 (fatal for one still locked) and +8
extern "C" void __fastcall Overlays_Destroy(BYTE* self, void*)
{
    for (BYTE* n = F(BYTE*, self, 0); n; )
    {
        BYTE* next = F(BYTE*, n, 8);
        if (F(int, n, 40))
            Sys_Fatal(s_68381c, 0);
        if (n)
            CallVirt(n, 0, 1);
        n = next;
    }
    for (BYTE* n = F(BYTE*, self, 8); n; )
    {
        BYTE* next = F(BYTE*, n, 8);
        if (n)
            CallVirt(n, 0, 1);
        n = next;
    }
}

// [FUN_5ee4f0..$L_5ee53f] mode 1 gives each overlay of the list a locked global block of its size (+16); then hands each its block
extern "C" int __fastcall Overlays_Alloc(BYTE* self, void*, int mode)
{
    if (!mode)
        return 1;
    if (mode == 1)
        for (BYTE* n = F(BYTE*, self, 0); n; n = F(BYTE*, n, 8))
        {
            HGLOBAL h = GlobalAlloc(0, F(int, n, 16));
            F(HGLOBAL, n, 52) = h;
            F(void*, n, 48) = GlobalLock(h);
            if (!F(void*, n, 48))
                Sys_Fatal(s_683834, 0);
        }
    for (BYTE* n = F(BYTE*, self, 0); n; n = F(BYTE*, n, 8))
        CallVirt(n, 24, F(void*, n, 48));
    return 1;
}


// ---------------------------------------------------------------------------

// [FUN_5ee300] adds an unowned object (+40 owner) to the list (+32, count +28, capacity +24); 1 when added
extern "C" int __fastcall List5ee300_Add(BYTE* self, void*, BYTE* o)
{
    if (F(int, self, 28) >= F(int, self, 24))
        return 0;
    if (F(BYTE*, o, 40))
        return 0;
    F(BYTE*, o, 40) = self;
    F(BYTE**, self, 32)[F(int, self, 28)] = o;
    F(int, self, 28)++;
    return 1;
}

// clears a flag of +20 and calls a method of every member; 0 when the flag wasn't set
static int Cl_ClearAndCall(BYTE* self, DWORD flag, unsigned method)
{
    if (!(F(DWORD, self, 20) & flag))
        return 0;
    F(DWORD, self, 20) &= ~flag;
    for (int i = 0; i < F(int, self, 24); i++)
        CallVirt(F(BYTE**, self, 32)[i], method);
    return 1;
}

// [FUN_5edf50] clears flag 1 and calls method +36 of every member
extern "C" int __fastcall Cluster_Clear1(BYTE* self, void*) { return Cl_ClearAndCall(self, 1, 36); }

// [FUN_5edfe0] clears flag 2 and calls method +44 of every member
extern "C" int __fastcall Cluster_Clear2(BYTE* self, void*) { return Cl_ClearAndCall(self, 2, 44); }


// ---------------------------------------------------------------------------

// [FUN_5edf90] sets flag 1 and calls method +40 of every member and +28 of the owner (+40); 0 when it was set
extern "C" int __fastcall Cluster_Set1(BYTE* self, void*)
{
    if (F(DWORD, self, 20) & 1)
        return 0;
    F(DWORD, self, 20) |= 1;
    for (int i = 0; i < F(int, self, 24); i++)
        CallVirt(F(BYTE**, self, 32)[i], 40);
    if (F(BYTE*, self, 40))
        CallVirt(F(BYTE*, self, 40), 28, self);
    return 1;
}

// [FUN_5ee020] calls method +48 of every member and +32 of the owner, then sets flag 2; 0 when it was set
extern "C" int __fastcall Cluster_Set2(BYTE* self, void*)
{
    if (F(BYTE, self, 20) & 2)
        return 0;
    for (int i = 0; i < F(int, self, 24); i++)
        CallVirt(F(BYTE**, self, 32)[i], 48);
    if (F(BYTE*, self, 40))
        CallVirt(F(BYTE*, self, 40), 32, self);
    F(DWORD, self, 20) |= 2;
    return 1;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_5ea9b0(); KING_ALIAS("_FUN_5ea9b0", "@ClusterFile_Destroy@4")
extern "C" void FUN_5edb20(); KING_ALIAS("_FUN_5edb20", "@Clu5edb20_Destroy@4")
extern "C" void FUN_5eddf0(); KING_ALIAS("_FUN_5eddf0", "@Clu5eddf0_Destroy@4")
extern "C" void FUN_5ee220(); KING_ALIAS("_FUN_5ee220", "@Texture_Destroy@8")
extern "C" void FUN_5ef450(); KING_ALIAS("_FUN_5ef450", "@Node_Destroy@8")

static BYTE* Clu_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5ede70]
extern "C" void __fastcall Clu_5ede70(BYTE*, void*, int) {}
// [$L_5ea670]
extern "C" BYTE* __fastcall Clu5ea670_DeletingDestructor(BYTE* s, void*, int flags) { return Clu_DeleteAfter(s, flags, FUN_5ea9b0); }
// [$L_5edb00]
extern "C" BYTE* __fastcall Clu5edb00_DeletingDestructor(BYTE* s, void*, int flags) { return Clu_DeleteAfter(s, flags, FUN_5edb20); }
// [$L_5ede90]
extern "C" BYTE* __fastcall Clu5ede90_DeletingDestructor(BYTE* s, void*, int flags) { return Clu_DeleteAfter(s, flags, FUN_5eddf0); }
// [$L_5ee200]
extern "C" BYTE* __fastcall Clu5ee200_DeletingDestructor(BYTE* s, void*, int flags) { return Clu_DeleteAfter(s, flags, FUN_5ee220); }
// [$L_5ef430]
extern "C" BYTE* __fastcall Clu5ef430_DeletingDestructor(BYTE* s, void*, int flags) { return Clu_DeleteAfter(s, flags, FUN_5ef450); }

extern "C" void FUN_5eb2c0(); KING_ALIAS("_FUN_5eb2c0", "@Camera_Apply@12")
extern "C" void FUN_5ee470(); KING_ALIAS("_FUN_5ee470", "@Overlay_DrawOn@16")
extern "C" void FUN_5edfe0(); KING_ALIAS("_FUN_5edfe0", "@Cluster_Clear2@8")
extern "C" void FUN_5bac90(); KING_ALIAS("_FUN_5bac90", "@Texture_ReleaseSurface@8")
extern "C" void FUN_5ee020(); KING_ALIAS("_FUN_5ee020", "@Cluster_Set2@8")
extern "C" void FUN_5bae30(); KING_ALIAS("_FUN_5bae30", "@Texture_Create@12")
extern "C" void FUN_5ee300(); KING_ALIAS("_FUN_5ee300", "@List5ee300_Add@12")

// [$L_5ed9a0] FUN_5eb2c0 of a with self
extern "C" int __fastcall Clu_5ed9a0(BYTE* s, void*, BYTE* a) { return CallThis<int>(FUN_5eb2c0, a, s); }
// [$L_5edaf0] the owner (+40) through its vtbl+32
extern "C" int __fastcall Clu_5edaf0(BYTE* s, void*, int a) { return CallVirt<int>(F(BYTE*, s, 40), 32, a); }
// [$L_5ede80] the owner (+40) through its vtbl+28
extern "C" int __fastcall Clu_5ede80(BYTE* s, void*, int a) { return CallVirt<int>(F(BYTE*, s, 40), 28, a); }
// [$L_5ee2f0] vtbl+40 when a is set
extern "C" int __fastcall Clu_5ee2f0(BYTE* s, void*, int a) { return a ? CallVirt<int>(s, 40) : a; }

// [$L_5ed9b0] FUN_5ee470 (a, self) of +136 of the object at +8624
extern "C" void __fastcall Clu_5ed9b0(BYTE* s, void*, int a)
{
    if (!a)
        return;
    BYTE* o = F(BYTE*, s, 8624);
    if (!o || !F(BYTE*, o, 136))
        return;
    CallThis(FUN_5ee470, F(BYTE*, o, 136), a, s);
}

// [$L_5ee3a0] vtbl+24 (a) of each child (+32, count +28)
extern "C" void __fastcall Clu_ChildrenCall24(BYTE* s, void*, int a)
{
    for (int i = 0; i < F(int, s, 28); i++)
        CallVirt(F(BYTE**, s, 32)[i], 24, a);
}

// [$L_5edd70] after FUN_5edfe0: FUN_5bac90 of each item (+68, count +44)
extern "C" int __fastcall Clu_5edd70(BYTE* s)
{
    int r = CallThis<int>(FUN_5edfe0, s);
    if (!r)
        return r;
    for (int i = 0; i < F(int, s, 44); i++)
        CallThis(FUN_5bac90, F(BYTE**, s, 68)[i]);
    return 1;
}
// [$L_5eddb0] after FUN_5ee020: FUN_5bae30 (0) of each item
extern "C" int __fastcall Clu_5eddb0(BYTE* s)
{
    int r = CallThis<int>(FUN_5ee020, s);
    if (!r)
        return r;
    for (int i = 0; i < F(int, s, 44); i++)
        CallThis(FUN_5bae30, F(BYTE**, s, 68)[i], 0);
    return 1;
}

// [$L_5ee340] the larger of the two +16 when FUN_5ee300 (p) agrees
extern "C" int __fastcall Clu_Max16(BYTE* s, void*, BYTE* p)
{
    int r = CallThis<int>(FUN_5ee300, s, p);
    if (r && F(int, p, 16) > F(int, s, 16))
        F(int, s, 16) = F(int, p, 16);
    return r;
}
// [$L_5ee370] p's +16 added when FUN_5ee300 (p) agrees
extern "C" int __fastcall Clu_Add16(BYTE* s, void*, BYTE* p)
{
    int r = CallThis<int>(FUN_5ee300, s, p);
    if (r)
        F(int, s, 16) += F(int, p, 16);
    return r;
}

// [$L_5ee260] attaches o at +44 (the old one gets vtbl+36), marks +20 and tells the owner (+40, vtbl+28)
extern "C" void __fastcall Clu_Attach(BYTE* s, void*, BYTE* o)
{
    if (!o)
        return;
    BYTE* old = F(BYTE*, s, 44);
    if (old != o) {
        if (old)
            CallVirt(old, 36);
        F(BYTE*, s, 44) = o;
    }
    F(int, s, 20) |= 1;
    if (F(BYTE*, s, 40))
        CallVirt(F(BYTE*, s, 40), 28, s);
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

extern "C" void FUN_5edf50(); KING_ALIAS("_FUN_5edf50", "@Cluster_Clear1@8")
extern "C" void FUN_5bad10(); KING_ALIAS("_FUN_5bad10", "@Texture_Release@8")
extern "C" void FUN_5bbe80(); KING_ALIAS("_FUN_5bbe80", "@Bitmap_Unload@8")
extern "C" void FUN_5fd350(); KING_ALIAS("_FUN_5fd350", "@SoundFile_Unload@8")

// [$L_5ee2a0] vtbl+44 of each child (+32, count +24), marks +20 and tells the owner (+40, vtbl+32)
extern "C" void __fastcall Clu_5ee2a0(BYTE* s, void*, int a)
{
    if (!a)
        return;
    for (int i = 0; i < F(int, s, 24); i++)
        CallVirt(F(BYTE**, s, 32)[i], 44);
    F(int, s, 20) |= 2;
    if (F(BYTE*, s, 40))
        CallVirt(F(BYTE*, s, 40), 32, s);
}

// [$L_5edc70] after FUN_5edf50: the items of the three lists (+68/+44, +72/+52, +76/+60)
extern "C" int __fastcall Clu_5edc70(BYTE* s)
{
    int r = CallThis<int>(FUN_5edf50, s);
    if (!r)
        return r;
    for (int i = 0; i < F(int, s, 44); i++)
        CallThis(FUN_5bad10, F(BYTE**, s, 68)[i]);
    for (int i = 0; i < F(int, s, 52); i++)
        CallThis(FUN_5bbe80, F(BYTE**, s, 72)[i]);
    for (int i = 0; i < F(int, s, 60); i++)
        CallThis(FUN_5fd350, F(BYTE**, s, 76)[i]);
    return 1;
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

typedef void (*CluFn)();
extern "C" void FUN_5edf90(); KING_ALIAS("_FUN_5edf90", "@Cluster_Set1@8")
extern "C" void FUN_5ba810(); KING_ALIAS("_FUN_5ba810", "@Image_LoadData@8")
extern "C" void FUN_5bbe10(); KING_ALIAS("_FUN_5bbe10", "@Texture_LoadData@8")
extern "C" void FUN_5fd2e0(); KING_ALIAS("_FUN_5fd2e0", "@SoundFile_Load@8")

// [$L_5edce0] after FUN_5edf90 the items of the three lists, each followed by the owner's (+12) hook +56
extern "C" int __fastcall Clu_5edce0(BYTE* s)
{
    int r = CallThis<int>(FUN_5edf90, s);
    if (!r)
        return r;
    for (int i = 0; i < F(int, s, 44); i++) {
        CallThis(FUN_5ba810, F(BYTE**, s, 68)[i]);
        CluFn fn = F(CluFn, F(BYTE*, s, 12), 56);
        if (fn)
            CallThis(fn, F(BYTE*, s, 12));
    }
    for (int i = 0; i < F(int, s, 52); i++) {
        CallThis(FUN_5bbe10, F(BYTE**, s, 72)[i]);
        CluFn fn = F(CluFn, F(BYTE*, s, 12), 56);
        if (fn)
            CallThis(fn, F(BYTE*, s, 12));
    }
    for (int i = 0; i < F(int, s, 60); i++) {
        CallThis(FUN_5fd2e0, F(BYTE**, s, 76)[i]);
        CluFn fn = F(CluFn, F(BYTE*, s, 12), 56);
        if (fn)
            CallThis(fn, F(BYTE*, s, 12));
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_5eddf0(); KING_ALIAS("_FUN_5eddf0", "@Clu5eddf0_Destroy@4")
ASM_VAR(AsmLabel, g_clu65277c, $L_65277c)
ASM_VAR(AsmLabel, g_clu6527b0, $L_6527b0)
ASM_VAR(AsmLabel, g_clu64d81c, $L_64d81c)

// [FUN_5eddf0] a cluster list (vtable 6527b0) deletes its items (+32, count +28) and frees its arrays
extern "C" void __fastcall Clu5eddf0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_clu6527b0;
    for (int i = 0; i < F(int, s, 28); i++)
        if (F(BYTE**, s, 32)[i])
            CallVirt(F(BYTE**, s, 32)[i], 0, 1);
    operator delete(F(void*, s, 32));
    operator delete(F(void*, s, 36));
    F(void*, s, 0) = g_clu64d81c;
}
// [FUN_5edb20] (vtable 65277c) frees +68, +72, +76
extern "C" void __fastcall Clu5edb20_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_clu65277c;
    operator delete(F(void*, s, 68));
    operator delete(F(void*, s, 72));
    operator delete(F(void*, s, 76));
    CallThis(FUN_5eddf0, s);
}

// ---------------------------------------------------------------------------
// Constructors and window resizing (exception frames dropped).

// [FUN_5edeb0] constructor (n, b): a cluster list (vtable 6527b0) with n empty slots (+32)
extern "C" BYTE* __fastcall Clu5edeb0_Construct(BYTE* s, void*, int n, int b)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_clu6527b0;
    F(int, s, 12) = b;
    F(int, s, 16) = 0;
    F(int, s, 20) = 0;
    F(int, s, 40) = 0;
    F(int, s, 36) = 0;
    F(int, s, 24) = n;
    if (n) {
        F(void*, s, 32) = operator new(n * 4);
        for (int i = 0; i < n; i++)
            F(int*, s, 32)[i] = 0;
    } else {
        F(int, s, 32) = 0;
    }
    F(int, s, 28) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

#include <string.h>
ASM_VAR(AsmLabel, g_clu65274c, $L_65274c)
// [FUN_5ea880] constructor (name): a cluster (vtable 65274c) with its name copied
extern "C" BYTE* __fastcall Cluster5ea880_Construct(BYTE* s, void*, const char* name)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_clu65274c;
    F(int, s, 56) = 0;
    F(int, s, 148) = 0;
    F(int, s, 152) = 0;
    F(int, s, 36) = 0;
    F(int, s, 52) = 0;
    size_t n = strlen(name) + 1;
    char* t = (char*)operator new(n);
    F(char*, s, 60) = t;
    memcpy(t, name, n);
    static const int zero[] = {132, 96, 80, 88, 72, 104, 112, 32, 28, 24, 20, 16, 12, 124, 128, 64, 68, 76, 84, 92,
                               100, 108, 120, 116, 140, 136};
    for (int o : zero)
        F(int, s, o) = 0;
    for (int o = 156; o < 180; o += 4)
        F(int, s, o) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

// a zeroed array of n words, or 0
static int* Clu_NewZeroed(int n)
{
    if (!n)
        return 0;
    int* p = (int*)operator new(n * 4);
    for (int i = 0; i < n; i++)
        p[i] = 0;
    return p;
}
// [FUN_5ed9e0] constructor (n1, n2, n3, slots, b): a cluster set (vtable 65277c) with three item arrays
extern "C" BYTE* __fastcall ClusterSet_Construct(BYTE* s, void*, int n1, int n2, int n3, int slots, int b)
{
    Clu5edeb0_Construct(s, 0, slots, b);
    F(void*, s, 0) = g_clu65277c;
    F(int, s, 48) = 0;
    F(int, s, 44) = n1;
    F(int*, s, 68) = Clu_NewZeroed(n1);
    F(int, s, 56) = 0;
    F(int, s, 52) = n2;
    F(int*, s, 72) = Clu_NewZeroed(n2);
    F(int, s, 64) = 0;
    F(int, s, 60) = n3;
    F(int*, s, 76) = Clu_NewZeroed(n3);
    return s;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

ASM_VAR(AsmLabel, g_q46_723978, $L_723978)
extern "C" void g_q46_5ef410(); KING_ALIAS("_g_q46_5ef410", "@ListHead_Construct@8")
// [FUN_5ef400] constructs the static 723978
extern "C" int __cdecl Static723978_Construct(void)
{
    return CallThis<int>(g_q46_5ef410, (void*)g_q46_723978);
}

// ---------------------------------------------------------------------------
// The scene search callback.

extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
// [$L_5ef310] scene search visitor: takes the node n into q (+0) when its name (+12) matches q's name (+4), the
// whole name, or its first +8 characters when positive; always -1 (go on)
extern "C" int __cdecl Scene_FindCallback(BYTE* n, int* q)
{
    const char* name = (const char*)q[1];
    const char* nn = F(const char*, n, 12);
    if (name && nn) {
        int len = q[2];
        if (len <= 0 ? strcmp(name, nn) == 0 : strncmp(name, nn, len) == 0)
            q[0] = (int)n;
    }
    return -1;
}

// [$L_5ea830] constructor: cleared, -1 at +44 and +52, scales +96 and +100 at 1, +120 = 238
extern "C" BYTE* __fastcall Clu5ea830_Construct(BYTE* s)
{
    static const short z[15] = {32, 40, 88, 76, 48, 108, 84, 80, 56, 60, 92, 36, 104, 68, 64};
    F(int, s, 44) = -1;
    F(int, s, 52) = -1;
    for (int k = 0; k < 15; k++)
        F(int, s, z[k]) = 0;
    F(float, s, 100) = 1.0f;
    F(float, s, 96) = 1.0f;
    F(BYTE, s, 0) = 0;
    F(BYTE, s, 120) = 238;
    return s;
}

// === the material description reader (FUN_5eb7a0)

extern "C" int __cdecl atoi(const char* s);
extern "C" double __cdecl atof(const char* s);
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
extern "C" int __cdecl strcmp(const char* a, const char* b);
ASM_VAR(double, g_mt64dae0, $L_64dae0)
ASM_VAR(AsmLabel, s_mt6836bc, $L_6836bc)
ASM_VAR(AsmLabel, s_mt6836b8, $L_6836b8)
ASM_VAR(AsmLabel, s_mt6836b4, $L_6836b4)
ASM_VAR(AsmLabel, s_mt6836b0, $L_6836b0)
ASM_VAR(AsmLabel, s_mt678acc, $L_678acc)
ASM_VAR(AsmLabel, s_mt6836a8, $L_6836a8)
ASM_VAR(AsmLabel, s_mt68369c, $L_68369c)
ASM_VAR(AsmLabel, s_mt683694, $L_683694)
ASM_VAR(AsmLabel, s_mt683688, $L_683688)
ASM_VAR(AsmLabel, s_mt683680, $L_683680)
ASM_VAR(AsmLabel, s_mt683674, $L_683674)
ASM_VAR(AsmLabel, s_mtPower676e94, $L_676e94)    // "power"
ASM_VAR(AsmLabel, s_mt68366c, $L_68366c)
ASM_VAR(AsmLabel, s_mt677738, $L_677738)
ASM_VAR(AsmLabel, s_mt683664, $L_683664)
ASM_VAR(AsmLabel, s_mt683660, $L_683660)
ASM_VAR(AsmLabel, s_mt68365c, $L_68365c)
ASM_VAR(AsmLabel, s_mt683654, $L_683654)
ASM_VAR(AsmLabel, s_mt683648, $L_683648)
ASM_VAR(AsmLabel, s_mt683644, $L_683644)
ASM_VAR(AsmLabel, s_mt683640, $L_683640)
ASM_VAR(AsmLabel, s_mt68363c, $L_68363c)
ASM_VAR(AsmLabel, s_mt683634, $L_683634)
ASM_VAR(AsmLabel, s_mt68362c, $L_68362c)
ASM_VAR(AsmLabel, s_mt683624, $L_683624)

// keywords that only set flags in +32
static const struct { const char* name; DWORD flag; } k_mtFlags[] = {
    {s_mt683688, 0x4000}, {s_mt683680, 0x100000}, {s_mt683674, 0x80000},
};
static const struct { const char* name; DWORD flag; } k_mtFlags2[] = {
    {s_mt683640, 0x80}, {s_mt68363c, 0x100}, {s_mt683634, 0x200}, {s_mt68362c, 0x8000}, {s_mt683624, 0x10000},
};

static char* Mt_Next(char** p)
{
    return CallC<char*>(FUN_5ab1b0, p);
}

// t: an index (1-based in the text) into +off unless there is no token
static bool Mt_Index(char** p, BYTE* o, int off)
{
    char* t = Mt_Next(p);
    if (!t)
        return false;
    F(int, o, off) = atoi(t) - 1;
    return true;
}

static void Mt_Float(char** p, BYTE* o, int off)
{
    if (char* t = Mt_Next(p))
        F(float, o, off) = (float)atof(t);
}

static bool Mt_Is(const char* t, const char* k)
{
    return strcmp(t, k) == 0;
}

extern "C" {

// [FUN_5eb7a0] reads a material description (keywords and values, quotes read as spaces) into o: flags at +32,
// indices, factors and colors; returns 1
int __cdecl Material_Read(char* text, BYTE* o)
{
    for (char* c = text; *c; c++)
        if (*c == '"')
            *c = ' ';
    char* tok = Mt_Next(&text);
    F(DWORD, o, 32) = 0;
    F(int, o, 76) = 0;
    F(int, o, 72) = 0;
    while (tok) {
        DWORD& flags = F(DWORD, o, 32);
        bool found = false;
        if (Mt_Is(tok, s_mt6836bc)) {
            F(int, o, 40) = atoi(Mt_Next(&text)) - 1;
            goto next;
        }
        {
            DWORD f = Mt_Is(tok, s_mt6836b8) ? 1 : Mt_Is(tok, s_mt6836b4) ? 3 : Mt_Is(tok, s_mt6836b0) ? 65 : 0;
            if (f) {
                char* t = Mt_Next(&text);
                flags |= f;
                F(int, o, 56) = atoi(t) - 1;
                goto next;
            }
        }
        if (Mt_Is(tok, s_mt678acc)) {
            char* t = Mt_Next(&text);
            flags |= 16;
            F(int, o, 60) = atoi(t) - 1;
            goto next;
        }
        if (Mt_Is(tok, s_mt6836a8)) {
            char* t = Mt_Next(&text);
            flags |= 32;
            if (g_720904 == 0)
                flags |= 384;
            F(float, o, 88) = (float)atof(t);
            goto next;
        }
        if (Mt_Is(tok, s_mt68369c)) {
            Mt_Float(&text, o, 92);
            goto next;
        }
        if (Mt_Is(tok, s_mt683694)) {
            Mt_Float(&text, o, 104);
            flags |= 0x2000;
            goto next;
        }
        for (int i = 0; i < 3; i++)
            if (Mt_Is(tok, k_mtFlags[i].name)) {
                flags |= k_mtFlags[i].flag;
                found = true;
                break;
            }
        if (found)
            goto next;
        if (Mt_Is(tok, s_mtPower676e94)) {
            if (char* t = Mt_Next(&text))
                F(int, o, 36) = atoi(t);
            goto next;
        }
        if (Mt_Is(tok, s_mt68366c)) {
            flags |= 0x1000;
            goto next;
        }
        if (Mt_Is(tok, s_mt677738)) {
            Mt_Float(&text, o, 96);
            Mt_Float(&text, o, 100);
            F(int, o, 64) = 0;
            flags |= 0x800;
            goto next;
        }
        if (strncmp(tok, s_mt677738, 3) == 0) {
            F(int, o, 64) = tok[3] ? tok[3] - '0' : 0;
            Mt_Float(&text, o, 96);
            Mt_Float(&text, o, 100);
            flags |= 0x800;
            goto next;
        }
        if (Mt_Is(tok, s_mt683664)) {
            Mt_Index(&text, o, 48);
            goto next;
        }
        if (Mt_Is(tok, s_mt683660)) {
            if (Mt_Index(&text, o, 44))
                flags |= 0x400;
            goto next;
        }
        if (Mt_Is(tok, s_mt68365c)) {
            Mt_Index(&text, o, 52);
            goto next;
        }
        if (Mt_Is(tok, s_mt683654)) {
            flags |= 4;
            F(float, o, 80) = (float)atof(Mt_Next(&text));
            F(float, o, 84) = (float)atof(Mt_Next(&text));
            goto next;
        }
        if (Mt_Is(tok, s_mt683648)) {
            F(float, o, 112) = (float)atof(Mt_Next(&text));
            F(float, o, 116) = (float)atof(Mt_Next(&text));
            goto next;
        }
        if (Mt_Is(tok, s_mt683644)) {
            flags |= 8;
            F(float, o, 108) = (float)(atof(Mt_Next(&text)) * g_mt64dae0);
            goto next;
        }
        for (int i = 0; i < 5; i++)
            if (Mt_Is(tok, k_mtFlags2[i].name)) {
                flags |= k_mtFlags2[i].flag;
                break;
            }
    next:
        tok = Mt_Next(&text);
    }
    return 1;
}

} // extern "C"

// === a cluster file's destructor (FUN_5ea9b0)

extern "C" void FUN_5d2430(); KING_ALIAS("_FUN_5d2430", "@NameHash_RemoveOwner@12")
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
extern "C" void FUN_5b9520(); KING_ALIAS("_FUN_5b9520", "@TexSets_Free@4")
extern "C" void FUN_5bbdd0(); KING_ALIAS("_FUN_5bbdd0", "@Bitmap_Destroy@8")
extern "C" void FUN_5e6b90(); KING_ALIAS("_FUN_5e6b90", "@Mmio_Close@8")
extern "C" void FUN_50d190(); KING_ALIAS("_FUN_50d190", "@Shell_Nop@8")
extern "C" void Clu_VecDtor(); KING_ALIAS("_Clu_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
ASM_VAR(AsmLabel, g_clu64d578, $L_64d578)

// deletes the nodes of list l (unlinking each from +8) and then the list
static void Clf_DeleteList(BYTE* l)
{
    if (!l)
        return;
    while (F(BYTE*, l, 8)) {
        BYTE* n = F(BYTE*, l, 8);
        CallThis(FUN_5d21b0, n);
        if (n)
            CallVirt(n, 0, 1);
    }
    CallVirt(l, 0, 1);
}

extern "C" {

// [FUN_5ea9b0] a cluster file (vtable 65274c): drops its names, frees +60, the 124-byte items (+76), the objects
// (+84, count +80; +92, +88), texture sets (+100, +96), bitmaps (+108, +104), named buffers (+116, +112), the
// name pairs (+124/+128, count +132), the lists +120, +68, +64, the overlays (+136); closes the file (+36), its
// reader (+52) and, when mapped (+152 bit 0), the view (+40) and handles (+48, +44)
void __fastcall ClusterFile_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_vtbl65274c;
    CallThis(FUN_5d2430, g_objectNames, s);
    if (F(void*, s, 60))
        operator delete(F(void*, s, 60));
    if (BYTE* a = F(BYTE*, s, 76)) {
        CallStd(Clu_VecDtor, a, 124, F(int, a, -4), FUN_50d190);
        operator delete(a - 4);
    }
    for (int i = 0; i < F(int, s, 80); i++)
        if (F(BYTE**, s, 84)[i])
            CallVirt(F(BYTE**, s, 84)[i], 0, 1);
    if (F(void*, s, 84))
        operator delete(F(void*, s, 84));
    for (int i = 0; i < F(int, s, 88); i++)
        if (F(BYTE**, s, 92)[i])
            CallVirt(F(BYTE**, s, 92)[i], 0, 1);
    if (F(void*, s, 92))
        operator delete(F(void*, s, 92));
    for (int i = 0; i < F(int, s, 96); i++)
        if (BYTE* p = F(BYTE**, s, 100)[i]) {
            CallThis(FUN_5b9520, p);
            operator delete(p);
        }
    if (F(void*, s, 100))
        operator delete(F(void*, s, 100));
    for (int i = 0; i < F(int, s, 104); i++)
        if (BYTE* p = F(BYTE**, s, 108)[i]) {
            CallThis(FUN_5bbdd0, p);
            operator delete(p);
        }
    if (F(void*, s, 108))
        operator delete(F(void*, s, 108));
    for (int i = 0; i < F(int, s, 112); i++)
        if (BYTE* p = F(BYTE**, s, 116)[i]) {
            F(void*, p, 0) = g_clu64d578;
            operator delete(F(void*, p, 4));
            operator delete(p);
        }
    if (F(void*, s, 116))
        operator delete(F(void*, s, 116));
    for (int i = 0; i < F(int, s, 132); i++) {
        operator delete(F(void**, s, 124)[i]);
        operator delete(F(void**, s, 128)[i]);
    }
    if (F(void*, s, 124))
        operator delete(F(void*, s, 124));
    if (F(void*, s, 128))
        operator delete(F(void*, s, 128));
    Clf_DeleteList(F(BYTE*, s, 120));
    Clf_DeleteList(F(BYTE*, s, 68));
    Clf_DeleteList(F(BYTE*, s, 64));
    if (BYTE* o = F(BYTE*, s, 136)) {
        Overlays_Destroy(o, 0);
        operator delete(o);
    }
    if (F(FILE*, s, 36)) {
        fclose(F(FILE*, s, 36));
        if (BYTE* m = F(BYTE*, s, 52)) {
            CallThis(FUN_5e6b90, m);
            operator delete(m);
        }
        if (F(BYTE, s, 152) & 1) {
            UnmapViewOfFile(F(void*, s, 40));
            CloseHandle(F(HANDLE, s, 48));
            CloseHandle(F(HANDLE, s, 44));
        }
    }
    F(void*, s, 0) = g_clu64d81c;
}

} // extern "C"

// === reading a cluster file's tables and nodes (FUN_5ed6b0)

extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" void Clu_VecCtorEh(); KING_ALIAS("_Clu_VecCtorEh", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")
typedef void (*Cr_Progress)();
extern "C" void FUN_5ef830(); KING_ALIAS("_FUN_5ef830", "_Scene_ReadNode")    // reads a node record (file, cluster, &kind, 0, 0, &name); the object or 0
extern "C" void FUN_5d2540(); KING_ALIAS("_FUN_5d2540", "@NameHash_Add@16")    // name hash: adds (name, object)
ASM_VAR(int, g_cr72006c, $L_72006c)
ASM_VAR(unsigned, g_cr723988, $L_723988)
ASM_VAR(AsmLabel, g_cr64d7bc, $L_64d7bc)

extern "C" {

// [FUN_5ed6b0] reads cluster file f into c: after the header the 32-byte names (+76, 124 bytes each, count +72)
// and the node list (111, then 333 records until 222; 444 skipped, 555 between records): objects go to their
// room lists, kind 0 records are skipped, kind 1 a named 104-byte group chained at +120; +56 is called every
// 5 records; 0 for a bad file
int __fastcall Cluster_ReadFile(BYTE* c, void*, FILE* f)
{
    g_cr72006c = 0;
    char magic[4];
    magic[0] = (char)fgetc(f);
    magic[1] = (char)fgetc(f);
    magic[2] = (char)fgetc(f);
    fgetc(f);
    magic[3] = 0;
    CallC<int>(FUN_5ab140, f);
    int names = CallC<int>(FUN_5ab140, f);
    CallC<int>(FUN_5ab140, f);
    int nodes = CallC<int>(FUN_5ab140, f);
    CallC<int>(FUN_5ab140, f);
    if (names) {
        fseek(f, names * 4, 0);
        int n = CallC<int>(FUN_5ab140, f);
        BYTE* a = (BYTE*)operator new(n * 124 + 4);
        BYTE* items = 0;
        if (a) {
            F(int, a, 0) = n;
            items = a + 4;
            CallStd(Clu_VecCtorEh, items, 124, n, (void*)Clu5ea830_Construct, (void*)FUN_50d190);
        }
        F(BYTE*, c, 76) = items;
        F(int, c, 72) = n;
        for (int i = 0; i < n; i++) {
            char buf[32];
            fread(buf, 32, 1, f);
            strcpy((char*)F(BYTE*, c, 76) + i * 124, buf);
        }
    }
    if (!nodes)
        return 1;
    fseek(f, nodes * 4, 0);
    if (CallC<int>(FUN_5ab140, f) != 111)
        return 0;
    for (;;) {
        int t = CallC<int>(FUN_5ab140, f);
        if (t == 444)
            continue;
        if (t == 222)
            return 1;
        if (t != 333)
            return 0;
        int kind;
        char* name;
        BYTE* o = CallC<BYTE*>(FUN_5ef830, f, c, &kind, 0, 0, &name);
        if (++g_cr723988 % 5 == 0 && F(Cr_Progress, c, 56))
            F(Cr_Progress, c, 56)();
        if (o)
            Room_AddNode(c, 0, o);
        else if (kind == 0)
            fseek(f, 44, 1);
        else if (kind == 1) {
            BYTE* g = (BYTE*)operator new(104);
            if (g) {
                F(int, g, 4) = 0;
                F(int, g, 8) = 0;
                F(void*, g, 0) = g_cr64d7bc;
                F(int, g, 80) = 0;
                F(int, g, 76) = 0;
                F(float, g, 88) = 1.0f;
                F(float, g, 84) = 1.0f;
                F(int, g, 96) = 0;
                F(int, g, 92) = 0;
                F(BYTE, g, 44) = 0;
                F(BYTE, g, 12) = 0;
                F(int, g, 100) = 0;
            }
            if (name[0])
                CallThis(FUN_5d2540, g_objectNames, name, g);
            Cluster_ReadNames(g, 0, f);
            if (F(BYTE*, c, 120))
                Node_Link(F(BYTE*, c, 120), 0, g);
            F(BYTE*, c, 120) = g;
        } else
            return 0;
        if (CallC<int>(FUN_5ab140, f) != 555)
            return 0;
    }
}

} // extern "C"

// === a scrolling backdrop strip (FUN_5ead50)

extern "C" void Bd_DrawPart(); KING_ALIAS("_Bd_DrawPart", "_Image_DrawPart")
extern "C" void Bd_FillRect(); KING_ALIAS("_Bd_FillRect", "_Surface_FillRect")
extern "C" void Bd_Pixel(); KING_ALIAS("_Bd_Pixel", "@Texture_Pixel@16")

extern "C" {

// [FUN_5ead50] draws w columns of the wrapping texture t from column off at (x, bottom - height) of dst, then
// fills above it with top (0: t's top-left pixel) and below bottom with bot (0: its bottom-left pixel); a
// negative colour fills nothing
void __cdecl Backdrop_Draw(BYTE* dst, BYTE* t, int off, int w, int x, int bottom, int top, int bot)
{
    int tw = F(WORD, t, 20);
    int th = F(WORD, t, 22);
    int r = off % tw;
    int y = bottom - th;
    int rc[4] = {r, 0, 0, th};
    if (r < 0) {
        rc[0] = tw + r;
        rc[2] = tw - 1;
        if (tw - 1 - rc[0] > w)
            rc[2] = rc[0] + w - 1;
        CallC(Bd_DrawPart, dst, t, rc, x, y);
        rc[0] = 0;
        rc[2] = r + w - 1;
        CallC(Bd_DrawPart, dst, t, rc, x - r, y);
    } else if (r + w > tw) {
        rc[2] = tw - 1;
        CallC(Bd_DrawPart, dst, t, rc, x, y);
        rc[0] = 0;
        rc[2] = r - tw + w - 1;
        CallC(Bd_DrawPart, dst, t, rc, tw - r + x, y);
    } else {
        rc[2] = r + w - 1;
        CallC(Bd_DrawPart, dst, t, rc, x, y);
    }
    if (top >= 0 && y - 1 >= 0) {
        int c = top ? top : CallThis<int>(Bd_Pixel, t, 0, 0);
        int f[4] = {x, 0, x + w - 1, y - 1};
        CallC(Bd_FillRect, dst, f, c, 1);
    }
    if (bot >= 0 && bottom < (int)F(WORD, dst, 2)) {
        int c = bot ? bot : CallThis<int>(Bd_Pixel, t, 0, th - 1);
        int f[4] = {x, bottom, x + w - 1, F(WORD, dst, 2)};
        CallC(Bd_FillRect, dst, f, c, 1);
    }
}

} // extern "C"

// === pointing the renderer through a camera node (FUN_5eb4b0)

extern "C" void Cn_World(); KING_ALIAS("_Cn_World", "@Node_GetWorld@12")
extern "C" void Cn_Inverse(); KING_ALIAS("_Cn_Inverse", "_Matrix_Inverse")
extern "C" void Cn_RotZ(); KING_ALIAS("_Cn_RotZ", "_Matrix_RotationZ")
extern "C" void Cn_Translation(); KING_ALIAS("_Cn_Translation", "_Matrix_Translation")
extern "C" void Cn_Multiply(); KING_ALIAS("_Cn_Multiply", "_Matrix_Multiply")
extern "C" void Cn_Transform2(); KING_ALIAS("_Cn_Transform2", "@Matrix_Transform2@12")
extern "C" void Cn_Scale(); KING_ALIAS("_Cn_Scale", "_Matrix_Scale")
extern "C" void Cn_SetCamera(); KING_ALIAS("_Cn_SetCamera", "@Render_SetCamera@16")
extern "C" void Cn_GetRect(); KING_ALIAS("_Cn_GetRect", "@Window_GetRect@12")
ASM_VAR(AsmLabel, g_cn7025f0, $L_7025f0)
ASM_VAR(DWORD, g_cn702e8c, $L_702e8c)
ASM_VAR(double, g_cn64b6e0, $L_64b6e0)
ASM_VAR(double, g_cn64b670, $L_64b670)
ASM_VAR(double, g_cn64b660, $L_64b660)
struct Cn_V3 { float x, y, z; };

extern "C" {

// [FUN_5eb4b0] sets renderer r's camera from camera c: the inverse of its node's (+80) world matrix (with 702e8c
// bit 8 turned by r's eye angle +8504 and shifted by half its eye distance +8496, mirrored for the other eye
// +8460), and the projection scale from the field of view (+88, +84) and the window's part on screen (mirrored
// with +100)
void __fastcall CamNode_Apply(BYTE* c, void*, BYTE* r)
{
    float m[12];
    memcpy(m, g_cn7025f0, 48);
    CallThis(Cn_World, F(BYTE*, c, 80), m);
    float t[12];
    memcpy(m, CallC<float*>(Cn_Inverse, t, m), 48);
    if (g_cn702e8c & 0x100) {
        double half = F(double, r, 8496) * g_cn64b6e0;
        double turn = F(double, r, 8504);
        float angle, shift;
        if (F(int, r, 8460)) {
            angle = (float)turn;
            shift = (float)half;
        } else {
            angle = (float)-turn;
            shift = (float)-half;
        }
        float rz[12], tr[12], pr[12];
        float* rot = CallC<float*>(Cn_RotZ, rz, *(DWORD*)&angle);
        Cn_V3 v = {shift, 0, 0};
        float* mv = CallC<float*>(Cn_Translation, tr, v);
        CallThis(Cn_Transform2, m, CallC<float*>(Cn_Multiply, pr, mv, rot));
    }
    int rc[4];
    CallThis(Cn_GetRect, F(BYTE*, r, 1948), rc);
    double sx = 1.0, sy = g_cn64b670;
    if (rc[0] < 0)
        sx = (double)rc[2] / (rc[2] - rc[0]);
    if (rc[1] < 0)
        sy = (double)rc[3] / (rc[3] - rc[1]);
    int s = F(int, c, 100) ? -2 : 2;
    Cn_V3 k;
    k.x = (float)(s / (F(float, c, 88) * sx));
    k.y = 1.0f;
    k.z = (float)(g_cn64b660 / (sy * F(float, c, 84)));
    float sc[12];
    CallThis(Cn_SetCamera, r, m, CallC<float*>(Cn_Scale, sc, k));
}

} // extern "C"

// === a backdrop around the view (FUN_5eaf40)

#include "x87.h"

ASM_VAR(float, g_bp64c258, $L_64c258)
ASM_VAR(float, g_bp64c264, $L_64c264)
ASM_VAR(double, g_bp64d908, $L_64d908)
ASM_VAR(double, g_bp64b658, $L_64b658)
ASM_VAR(double, g_bp64b7c0, $L_64b7c0)

extern "C" {

// [FUN_5eaf40] draws backdrop picture bd+76 of set s (pictures +92, count +88) on r's surface for camera cam
// (fields of view +84 / +88) turned by angle a: centred at the height a * 'scale' below the middle plus bd+80,
// in columns of width w from the middle outwards, each step one line higher or lower by the tilt t (none when
// level); the clip rows are bd+84 / +88
void __fastcall Backdrop_Place(BYTE* s, void*, BYTE* cam, float a, float scale, float t, BYTE* r, BYTE* bd)
{
    int idx = F(int, bd, 76);
    BYTE** pics = F(BYTE**, s, 92);
    if (!pics || idx < 0 || idx >= F(int, s, 88))
        return;
    BYTE* pic = pics[idx];
    if (!pic)
        return;
    BYTE* surf = F(BYTE*, r, 1948);
    int W = F(WORD, surf, 0), H = F(WORD, surf, 2);
    double av = x87_atan2(F(float, cam, 88) * g_bp64c258, 1.0);
    double hs = H * (double)scale;
    double ah = x87_atan2(F(float, cam, 84) * g_bp64c258, 1.0);
    float y = (float)(hs / (ah + ah) + H / 2 + F(int, bd, 80));
    double ang = -a;
    if (ang < g_bp64c264)
        ang = ang + g_bp64d908;
    int x = (int)(W * ang / (av + av));
    int rem = x % (int)F(WORD, pic, 20);
    double frem = rem;
    int half = W / 2, w, col;
    if (x87_fabs(t) < g_bp64b658) {
        w = half;
        col = half;
    } else {
        w = (int)(3 * W / (x87_sin(t) / x87_cos(t) * H * g_bp64b7c0));
        if (w < 4 && w >= 0)
            w = 4;
        else if (w > -4 && w <= 0)
            w = -4;
        if (w > half)
            w = half;
        if (w < -half)
            w = -half;
        col = half;
    }
    int dir;
    if (w > 0)
        dir = 1;
    else {
        dir = -1;
        w = -w;
    }
    if (w >= col)
        dir = 0;
    int off0 = (int)frem;
    int yi = (int)y;
    if (col < W) {
        int off = off0 - half + col;
        for (int c = col; c < W; c += w, off += w, yi += dir)
            Backdrop_Draw(surf, pic, off, w, c, yi, F(int, bd, 84), F(int, bd, 88));
    }
    yi = (int)(y - dir);
    int c = half - w;
    int n = c + w;
    if (n <= 0)
        return;
    int off = off0 - half + c;
    do {
        Backdrop_Draw(surf, pic, off, w, c, yi, F(int, bd, 84), F(int, bd, 88));
        n -= w;
        c -= w;
        off -= w;
        yi -= dir;
    } while (n > 0);
}

} // extern "C"

// === reading the overlay description file (FUN_5ee580)

extern "C" void Ol_Token(); KING_ALIAS("_Ol_Token", "_NextToken")
extern "C" void Ol_Line(); KING_ALIAS("_Ol_Line", "_FileList_ReadLine")
extern "C" void Ol_Fatal(); KING_ALIAS("_Ol_Fatal", "_Sys_Fatal")
extern "C" void Ol_Find(); KING_ALIAS("_Ol_Find", "_Objects_Find")
extern "C" void Ol_FindScene(); KING_ALIAS("_Ol_FindScene", "_Scene_FindObject")
extern "C" void Ol_SoundOn(); KING_ALIAS("_Ol_SoundOn", "_Sound_IsOn")
extern "C" void Ol_Lookup(); KING_ALIAS("_Ol_Lookup", "@Hash_Lookup@16")
extern "C" void Ol_Link(); KING_ALIAS("_Ol_Link", "@Node_Link@12")
extern "C" void Ol_Set(); KING_ALIAS("_Ol_Set", "@ClusterSet_Construct@28")
extern "C" void Ol_AddA(); KING_ALIAS("_Ol_AddA", "@Cluster_AddA@12")
extern "C" void Ol_AddB(); KING_ALIAS("_Ol_AddB", "@Cluster_AddB@12")
extern "C" void Ol_AddC(); KING_ALIAS("_Ol_AddC", "@Cluster_AddC@12")
extern "C" void Ol_Texture(); KING_ALIAS("_Ol_Texture", "@Texture_Construct@16")
extern "C" void Ol_Alloc(); KING_ALIAS("_Ol_Alloc", "@Overlays_Alloc@12")
extern "C" void Ol_AddName(); KING_ALIAS("_Ol_AddName", "@NameHash_Add@16")
ASM_VAR(char, g_olLine, $L_722970)
ASM_VAR(BYTE*, g_olImages, $L_7229c0)
ASM_VAR(BYTE*, g_olImagesEnd, $L_72318c)
ASM_VAR(BYTE*, g_olMasks, $L_7231a4)
ASM_VAR(BYTE*, g_olMasksEnd, $L_723974)
ASM_VAR(BYTE*, g_olSounds, $L_7218d4)
ASM_VAR(BYTE*, g_olSoundsEnd, $L_7220a4)
ASM_VAR(BYTE*, g_olClusters, $L_72211c)
ASM_VAR(BYTE*, g_olClustersEnd, $L_7228ec)
ASM_VAR(AsmLabel, g_olNames, $L_71e820)
ASM_VAR(AsmLabel, g_ol652818, $L_652818)

// sprintf into the line buffer, then the fatal error
static void Ol_Error(const char* fmt, const void* a = 0, const void* b = 0)
{
    sprintf(&g_olLine, fmt, a, b);
    CallC(Ol_Fatal, &g_olLine, 0);
}

static char* Ol_Copy(const char* s)
{
    char* p = (char*)operator new(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

// a cluster named tok into *slot (the hash of names), not yet used by another overlay
static void Ol_Cluster(const char* tok, BYTE** slot)
{
    if (slot >= &g_olClustersEnd)
        Ol_Error("Too many clasters in overlay.");
    CallThis(Ol_Lookup, &g_olNames, tok, slot);
    if (!*slot)
        Ol_Error("Not found %s cluster.", tok);
    if (F(int, *slot, 40))
        Ol_Error("Twice using cluster %s.", tok);
}

static void Ol_Append(BYTE** head, BYTE* o)
{
    if (!*head)
        *head = o;
    else
        CallThis(Ol_Link, o, *head);
}

extern "C" {

// [FUN_5ee580] the overlay system s read from file path (o the overlay object at +12): CLUSTER blocks (images,
// masks, sounds and the clusters they overlay) made into cluster sets (+4 list, named), OVERLAY blocks into
// textures (+0 list when unnamed) and ROOM blocks into room descriptions (+8 list, the room's +56); then the
// buffers for size
BYTE* __fastcall Overlays_Load(BYTE* s, void*, const char* path, BYTE* o, int size)
{
    F(BYTE*, s, 12) = o;
    F(int, s, 0) = 0;
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(int, s, 16) = -1;
    if (!path)
        return s;
    FILE* f = fopen(path, "r");
    if (!f)
        return s;
    for (fgets(&g_olLine, 70, f); !(F(BYTE, f, 12) & 16); fgets(&g_olLine, 70, f))
    {
        char* p = &g_olLine;
        char* tok = CallC<char*>(Ol_Token, &p);
        if (!tok)
            continue;
        if (!strcmp(tok, "CLUSTER"))
        {
            int clusters = 0, sounds = 0, masks = 0, images = 0;
            tok = CallC<char*>(Ol_Token, &p);
            if (!tok)
                CallC(Ol_Fatal, "Cluster have not name.", 0);
            char* nm = Ol_Copy(tok);
            BYTE** cl = &g_olClusters;
            BYTE** snd = &g_olSounds;
            BYTE** msk = &g_olMasks;
            BYTE** img = &g_olImages - 1;
            int k;
            while ((k = CallC<int>(Ol_Line, f, &tok)) != 0)
            {
                switch (k)
                {
                case 1:
                    if (img >= &g_olImagesEnd)
                        Ol_Error("Too many images in cluster %s.", nm);
                    *++img = CallC<BYTE*>(Ol_Find, tok);
                    images++;
                    if (!*img)
                        Ol_Error("Not found %s image in cluster %s.", tok, nm);
                    if (!(F(BYTE, *img, 52) & 4))
                        Ol_Error("Image %s in cluster %s has not \"noload\".", tok, nm);
                    break;
                case 2:
                    if (msk >= &g_olMasksEnd)
                        Ol_Error("Too many masks in cluster %s.", nm);
                    *msk = CallC<BYTE*>(Ol_Find, tok);
                    masks++;
                    if (!*msk++)
                        Ol_Error("Not found %s mask in cluster %s.", tok, nm);
                    break;
                case 3:
                    if (!CallC<int>(Ol_SoundOn))
                        break;
                    if (snd >= &g_olSoundsEnd)
                        Ol_Error("Too many sounds in cluster %s.", nm);
                    *snd = CallC<BYTE*>(Ol_Find, tok);
                    sounds++;
                    if (!*snd++)
                        Ol_Error("Not found %s sound in cluster %s.", tok, nm);
                    break;
                case 5:
                    break;
                case 6:
                    Ol_Cluster(tok, cl);
                    clusters++;
                    cl++;
                    break;
                default:
                    Ol_Error("Unrecognize error in cluster %s.", nm);
                    break;
                }
            }
            BYTE* c = (BYTE*)operator new(80);
            if (c)
                c = CallThis<BYTE*>(Ol_Set, c, images, masks, sounds, clusters, o);
            F(char*, c, 36) = nm;
            for (int i = 0; i < images; i++)
                CallThis(Ol_AddA, c, (&g_olImages)[i]);
            for (int i = 0; i < masks; i++)
                CallThis(Ol_AddB, c, (&g_olMasks)[i]);
            for (int i = 0; i < sounds; i++)
                CallThis(Ol_AddC, c, (&g_olSounds)[i]);
            for (int i = 0; i < clusters; i++)
                if (!CallVirt<int>(c, 20, (&g_olClusters)[i]))
                    Ol_Error("Error in using %d overlay in cluster %s.", (const void*)(INT_PTR)i, nm);
            Ol_Append(&F(BYTE*, s, 4), c);
            CallThis(Ol_AddName, &g_olNames, nm, c);
        }
        else if (!strcmp(tok, "OVERLAY"))
        {
            int clusters = 0;
            tok = CallC<char*>(Ol_Token, &p);
            if (!tok)
                tok = (char*)"";
            char* nm = Ol_Copy(tok);
            BYTE** cl = &g_olClusters;
            int k;
            while ((k = CallC<int>(Ol_Line, f, &tok)) != 0)
            {
                if (k == 5)
                    continue;
                if (k == 6)
                {
                    Ol_Cluster(tok, cl);
                    clusters++;
                    cl++;
                }
                else
                    Ol_Error("Unrecognize error in overlay %s.", nm);
            }
            BYTE* t = (BYTE*)operator new(56);
            if (t)
                t = CallThis<BYTE*>(Ol_Texture, t, clusters, o);
            F(char*, t, 36) = nm;
            for (int i = 0; i < clusters; i++)
                if (!CallVirt<int>(t, 20, (&g_olClusters)[i]))
                    Ol_Error("Error in using cluster in overlay %s.", nm);
            if (!strcmp(nm, ""))
                Ol_Append(&F(BYTE*, s, 0), t);
            else
                CallThis(Ol_AddName, &g_olNames, nm, t);
        }
        else if (!strcmp(tok, "ROOM"))
        {
            DWORD pals = 0;
            tok = CallC<char*>(Ol_Token, &p);
            if (!tok)
                CallC(Ol_Fatal, "Room have not name.", 0);
            char* nm = Ol_Copy(tok);
            BYTE* room = CallC<BYTE*>(Ol_FindScene, 0, tok);
            if (!room)
            {
                char buf[64];
                sprintf(buf, "Not found room %s.", tok);
                CallC(Ol_Fatal, buf, 0);
            }
            BYTE* cluster = 0;
            BYTE* ov = F(BYTE*, s, 12);
            BYTE* image = F(int, ov, 88) > 0 ? *F(BYTE**, ov, 92) : 0;
            int k;
            while ((k = CallC<int>(Ol_Line, f, &tok)) != 0)
            {
                switch (k)
                {
                case 5:
                    break;
                case 1:
                    image = CallC<BYTE*>(Ol_Find, tok);
                    if (!image && strcmp(tok, "null.txr"))
                        Ol_Error("Not found %s image", tok);
                    break;
                case 6:
                    CallThis(Ol_Lookup, &g_olNames, tok, &cluster);
                    if (!cluster)
                        Ol_Error("Not found cluster %s.", tok);
                    break;
                case 7:
                {
                    BYTE* pal;
                    CallThis(Ol_Lookup, &g_olNames, tok, &pal);
                    if (!pal)
                        Ol_Error("Not found Palette %s.", tok);
                    BYTE* q = F(BYTE*, s, 12);
                    int i = 0;
                    while (i < F(int, q, 96) && pal != F(BYTE**, q, 100)[i])
                        i++;
                    pals |= 1u << i;
                    break;
                }
                default:
                    Ol_Error("Unrecognize error in ROOM OVL DESCRIPTION %s.", nm);
                    break;
                }
            }
            operator delete(nm);
            BYTE* q = F(BYTE*, s, 12);
            int n = F(int, q, 88), idx;
            if (!n)
                idx = -2;
            else
            {
                idx = 0;
                while (idx < n && image != F(BYTE**, q, 92)[idx])
                    idx++;
                if (idx >= n)
                    idx = -1;
            }
            BYTE* r = (BYTE*)operator new(28);
            if (r)
            {
                F(BYTE*, r, 12) = room;
                F(int, r, 4) = 0;
                F(int, r, 8) = 0;
                F(void*, r, 0) = &g_ol652818;
                F(DWORD, r, 16) = pals;
                F(BYTE*, r, 20) = cluster;
                F(int, r, 24) = idx;
            }
            F(BYTE*, room, 56) = r;
            Ol_Append(&F(BYTE*, s, 8), r);
        }
    }
    if (!CallThis<int>(Ol_Alloc, s, size))
        CallC(Ol_Fatal, "Not enough memory for Overlay System.", 0);
    fclose(f);
    return s;
}

} // extern "C"

// === a list head (a method at $L_5ef410)

ASM_VAR(AsmLabel, g_lh65282c, $L_65282c)

extern "C" {

// [$L_5ef410] list head s: empty (+4, +8, +12) with the vtable 65282c
BYTE* __fastcall ListHead_Construct(BYTE* s, void*)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = &g_lh65282c;
    F(int, s, 12) = 0;
    return s;
}

} // extern "C"

// === names added to a name hash: by bucket (FUN_5d2540 with its tail $L_5d25fd) or to its one list (FUN_5d2650
//     with its tail $L_5d26cb)

extern "C" void Nh_Alloc(); KING_ALIAS("_Nh_Alloc", "_Pool71e830_Alloc")
extern "C" void Nh_New(); KING_ALIAS("_Nh_New", "@Cor5d28a0_Construct@12")
extern "C" void Nh_Fatal(); KING_ALIAS("_Nh_Fatal", "_Sys_Fatal")
extern "C" void Nh_Hash(); KING_ALIAS("_Nh_Hash", "@Hash_Name@12")
extern "C" void Nh_Link(); KING_ALIAS("_Nh_Link", "@Node_Link@12")
ASM_VAR(char, g_nhSkip, $L_67fb1c)
ASM_VAR(char, g_nhNoHeap, $L_67fae0)

// a name entry (24 bytes from the name heap) of name with value v in hash h, 0 when name is blank
static BYTE* Nh_Entry(BYTE* h, const char* name, void* v)
{
    const char* c = name;
    while (*c == ' ')
        c++;
    if (!*c)
        return 0;
    BYTE* e = CallC<BYTE*>(Nh_Alloc, 24);
    if (e)
        e = CallThis<BYTE*>(Nh_New, e, name);
    if (!e)
        CallC(Nh_Fatal, &g_nhNoHeap, 0);
    F(void*, e, 16) = v;
    F(int, e, 20) = F(int, h, 12);
    return e;
}

static void Nh_Put(BYTE** slot, BYTE* e)
{
    if (!*slot)
        *slot = e;
    else
        CallThis(Nh_Link, e, *slot);
}

extern "C" {

// [FUN_5d2540..$L_5d25fd] hash h gets name (not empty, blank or the skipped name) with value v in its bucket
void __fastcall NameHash_Add(BYTE* h, void*, const char* name, void* v)
{
    if (!name || !*name || !strcmp(name, &g_nhSkip))
        return;
    if (BYTE* e = Nh_Entry(h, name, v))
        Nh_Put(F(BYTE**, h, 8) + CallThis<int>(Nh_Hash, h, name), e);
}

// [FUN_5d2650..$L_5d26cb] hash h gets name (not blank) with value v in its one list (+0)
void __fastcall NameHash_AddList(BYTE* h, void*, const char* name, void* v)
{
    if (!name)
        return;
    if (BYTE* e = Nh_Entry(h, name, v))
        Nh_Put((BYTE**)h, e);
}

} // extern "C"

// === an empty cluster made with room for its parts (FUN_5ea690 with its tail $L_5ea6ea)

extern "C" void Ce_Fatal(); KING_ALIAS("_Ce_Fatal", "_Sys_Fatal")
extern "C" void Ce_Sounds(); KING_ALIAS("_Ce_Sounds", "_Sound_AllocBuffers")
ASM_VAR(AsmLabel, g_ce65274c, $L_65274c)

extern "C" {

// [FUN_5ea690..$L_5ea6ea] cluster c with pal palettes (+96 / +100, at least 1), items (+72 / +76, constructed),
// images (+80 / +84), masks (+88 / +92), sounds (+104 / +108, their buffers made) and nodes (+112 / +116)
BYTE* __fastcall Cluster_ConstructEmpty(BYTE* c, void*, int pal, int items, int images, int masks, int sounds,
                                        int bufs, int nodes)
{
    F(int, c, 4) = 0;
    F(int, c, 8) = 0;
    F(void*, c, 0) = &g_ce65274c;
    F(int, c, 56) = 0;
    F(int, c, 148) = 0;
    F(int, c, 152) = 0;
    F(int, c, 36) = 0;
    F(int, c, 52) = 0;
    if (!pal)
        CallC(Ce_Fatal, "Palettes may be 1 or more!", 0);
    F(int, c, 104) = sounds;
    F(int, c, 88) = masks;
    F(int, c, 112) = nodes;
    for (int off = 12; off <= 32; off += 4)
        F(int, c, off) = 0;
    F(int, c, 60) = 0;
    F(int, c, 132) = 0;
    F(int, c, 72) = items;
    F(int, c, 80) = images;
    F(int, c, 96) = pal;
    F(int, c, 64) = 0;
    F(int, c, 68) = 0;
    F(int, c, 124) = 0;
    F(int, c, 128) = 0;
    BYTE* a = (BYTE*)operator new(items * 124 + 4);
    BYTE* it = 0;
    if (a)
    {
        F(int, a, 0) = items;
        it = a + 4;
        CallStd(Clu_VecCtorEh, it, 124, items, (void*)Clu5ea830_Construct, (void*)FUN_50d190);
    }
    F(BYTE*, c, 76) = it;
    F(void*, c, 84) = operator new(images * 4);
    F(void*, c, 92) = operator new(masks * 4);
    F(void*, c, 100) = operator new(pal * 4);
    F(void*, c, 108) = operator new(sounds * 4);
    F(void*, c, 116) = operator new(nodes * 4);
    CallC(Ce_Sounds, bufs);
    F(int, c, 120) = 0;
    F(int, c, 140) = 0;
    F(int, c, 136) = 0;
    memset(c + 156, 0, 24);
    return c;
}

} // extern "C"
