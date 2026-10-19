// framework.cpp - small functions of the unit at 0x5d8540 of king.masm (the
// unit of CGameApp, see game_app.cpp and main_loop.cpp): windows and their
// views, the scene time, node helpers by type, the window list, the data
// path, worker threads, the resource tables (1000 slots, hashed by name),
// file streams, the font metrics and text drawing.
//
// Building with  nmake ASM_FRAMEWORK=1  makes king.masm assemble the
// original procedures again.

#include "king.h"
#include <mmsystem.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" char* __cdecl strncpy(char* d, const char* s, size_t n);
extern "C" char* __cdecl strcat(char* d, const char* s);
extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" size_t __cdecl fread(void* buf, size_t size, size_t n, FILE* f);

extern "C" void __fastcall Node_Link(BYTE* self, void*, BYTE* list);                      // core.cpp
extern "C" void __fastcall Node_ApplyMatrix(BYTE* self, void*, const void* m, int mode);   // scene.cpp
extern "C" int __fastcall Node_GetWorld(BYTE* self, void*, void* out);                     // scene.cpp
extern "C" void __fastcall Node_GetLocal(BYTE* self, void*, void* out);                    // scene.cpp
extern "C" BYTE* __fastcall Node_Root(BYTE* self, void*);                                  // scene.cpp
extern "C" BYTE* __cdecl Scene_FindObject(BYTE* scene, const char* name);                  // clusters.cpp

extern "C" void FUN_5b6e00(); KING_ALIAS("_FUN_5b6e00", "@Surface_Clear@16")
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
extern "C" void FUN_5e5130(); KING_ALIAS("_FUN_5e5130", "@Resources_Create@16")
extern "C" void FUN_4cef50(); KING_ALIAS("_FUN_4cef50", "_Effect_Zero")
extern "C" void FUN_5d3b30(); KING_ALIAS("_FUN_5d3b30", "_Video_Height")
extern "C" void FUN_5d3b20(); KING_ALIAS("_FUN_5d3b20", "_Video_Width")
ASM_PROC(FUN_5d2ba0)
ASM_PROC(FUN_5e9d40)
ASM_VAR(int, g_721754, $L_721754)
ASM_VAR(void*, g_67fb7c, $L_67fb7c)
ASM_VAR(AsmLabel, g_start603800, $L_603800)
ASM_VAR(BYTE*, g_7208fc, $L_7208fc)
ASM_VAR(AsmLabel, s_683538, $L_683538)
ASM_VAR(AsmLabel, g_enum5d8c60, $L_5d8c60)
ASM_VAR(AsmLabel, g_enum5d94d0, $L_5d94d0)
extern "C" void FUN_5b6a50(); KING_ALIAS("_FUN_5b6a50", "@Window_ConstructChild@24")
extern "C" void FUN_5e43e0(); KING_ALIAS("_FUN_5e43e0", "@Loader_Construct@12")
ASM_PROC(FUN_5dd0d0)
ASM_VAR(AsmLabel, g_vtbl652168, $L_652168)
ASM_VAR(AsmLabel, g_vtbl652738, $L_652738)
ASM_VAR(AsmLabel, g_vtbl64d81c, $L_64d81c)
ASM_PROC(FUN_5b6bf0)
extern "C" void FUN_5b6b10(); KING_ALIAS("_FUN_5b6b10", "@Surface_Present@8")
ASM_PROC(FUN_5d8710)    // window base constructor (a, rect, b)
ASM_PROC(FUN_5da950)
ASM_PROC(FUN_5dbf00)
ASM_PROC(FUN_6037e0)    // time in ms, returns double
ASM_PROC(FUN_5e48f0)
extern "C" void FUN_5e4900(); KING_ALIAS("_FUN_5e4900", "@ResTable_Charge@12")
ASM_PROC(FUN_5ea1b0)    // draws text
extern "C" void* __stdcall AfxGetThread_();
KING_ALIAS("_AfxGetThread_@0", "?AfxGetThread@@YGPAVCWinThread@@XZ")

ASM_VAR(AsmLabel, g_vtbl652238, $L_652238)
ASM_VAR(AsmLabel, g_vtbl654420, $L_654420)
ASM_VAR(AsmLabel, s_foreground, $L_682c48)       // "Foreground_object"
ASM_VAR(AsmLabel, s_background, $L_682c34)       // "Background_object"
ASM_VAR(AsmLabel, s_skyDomeSpace, $L_682c24)
ASM_VAR(AsmLabel, s_skyDomeKey, $L_682c18)
ASM_VAR(int, g_7212a0, $L_7212a0)
ASM_VAR(int, g_721298, $L_721298)
ASM_VAR(int, g_72129c, $L_72129c)
ASM_VAR(int, g_7216bc, $L_7216bc)
ASM_VAR(int, g_7212b0, $L_7212b0)
ASM_VAR(void*, g_mainThread, $L_72177c)
ASM_VAR(BYTE*, g_app, $L_721750)
ASM_VAR(int, g_721774, $L_721774)
ASM_VAR(AsmLabel, g_dataPath, $L_7209d4)         // 251 bytes
ASM_VAR(char*, g_dataDir, $L_720bb0)
ASM_VAR(AsmLabel, s_dot, $L_682a2c)
ASM_VAR(int, g_702e9c, $L_702e9c)
ASM_VAR(int, g_720930, $L_720930)
ASM_VAR(int, g_resPending, $L_7217a4)
ASM_VAR(AsmLabel, g_fileDir, $L_7217bc)
ASM_VAR(AsmLabel, g_filePath, $L_72183c)
ASM_VAR(AsmLabel, s_errUnknown, $L_6835ac)
ASM_VAR(AsmLabel, s_errFormat, $L_683570)
ASM_VAR(AsmLabel, s_errMemory, $L_68355c)
ASM_VAR(AsmLabel, s_errNotFound, $L_68354c)

extern "C" void __fastcall Resources_Unload(BYTE* self, void*, const char* name);

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

extern "C" {

// [FUN_5d8630]
void __fastcall Window_Show(BYTE* self, void*, int i)
{
    if (i >= 0 && (F(BYTE, self, 16) & 1))
        CallThis(FUN_5b6e00, F(BYTE*, self, 12), i, 0);
}

// [FUN_5d88c0]
void __fastcall Window_Refresh(BYTE* self, void*)
{
    if (F(BYTE, self, 16) & 1) {
        CallThis(FUN_5b6bf0, F(BYTE*, self, 12));
        CallThis(FUN_5b6b10, F(BYTE*, self, 12));
    }
}

// [FUN_5d8a70] a view on obj with obj's rectangle moved to the origin
BYTE* __fastcall View_Construct(BYTE* self, void*, int a, BYTE* obj)
{
    int* r = (int*)(obj + 44);
    CallThis(FUN_5d8710, self, a, r, 0);
    F(void*, self, 0) = g_vtbl652238;
    int x = r[0], y = F(int, obj, 48);
    r[0] = r[0] - x;
    r[2] = r[2] - x;
    r[1] = r[1] - y;
    r[3] = r[3] - y;
    F(BYTE*, self, 56) = obj;
    F(DWORD, self, 16) |= 4;
    F(int, self, 60) = 0;
    return self;
}

// [FUN_5d9290] the scene of this name
BYTE* __fastcall Scenes_Find(BYTE* self, void*, const char* name)
{
    for (BYTE* s = F(BYTE*, self, 468); s; s = F(BYTE*, s, 8))
        if (!strcmp(F(const char*, s, 60), name))
            return s;
    return 0;
}

// [FUN_5d9ef0]
void __fastcall Scenes_SetFirst(BYTE* self, void*, BYTE* s)
{
    F(DWORD, self, 16) |= 8;
    F(BYTE*, self, 468) = s;
}

// [FUN_5d9f10]
void __fastcall Scene_FindSpecial(BYTE* self, void*)
{
    F(BYTE*, self, 524) = Scene_FindObject(0, s_foreground);
    F(BYTE*, self, 528) = Scene_FindObject(0, s_background);
    F(BYTE*, self, 476) = Scene_FindObject(0, s_skyDomeSpace);
    F(BYTE*, self, 480) = Scene_FindObject(0, s_skyDomeKey);
}

// [FUN_5db6e0]
void __fastcall Scenes_Add(BYTE* self, void*, BYTE* s)
{
    if (F(BYTE*, self, 468))
        Node_Link(s, 0, F(BYTE*, self, 468));
    else
        F(BYTE*, self, 468) = s;
}

// [FUN_5db9f0] queues an event in the ring of 10
void __fastcall Events_Put(BYTE* self, void*, int e)
{
    int i = F(int, self, 192);
    int next = (i + 1) % 10;
    if (next != F(int, self, 196)) {
        F(int, self, 200 + i * 4) = e;
        F(int, self, 192) = next;
    }
}

// [FUN_5dbcc0]
void* __fastcall Camera_Get(BYTE* self, void*, void* out)
{
    DWORD* d = (DWORD*)self;
    CallC(FUN_5da950, out, d[2904 / 4], d[2908 / 4], d[2912 / 4], d[2916 / 4], d[2920 / 4], d[2924 / 4],
          F(double, self, 2952) + F(double, self, 2928), d[2936 / 4], d[2940 / 4], d[2944 / 4], d[2948 / 4]);
    return out;
}

// [FUN_5dbe60] takes the next message of the ring of 100
int __fastcall Messages_Take(BYTE* self, void*)
{
    int i = F(int, self, 456);
    if (i == F(int, self, 460))
        return 0;
    int m = F(int, self, 56 + i * 4);
    i++;
    F(int, self, 456) = i;
    if (i >= 100)
        F(int, self, 456) = 0;
    return m;
}

// [FUN_5dbec0]
int __stdcall Light_Set(int a, int b, int c, int d, int e, int f)
{
    g_7212a0 = d;
    g_721298 = b;
    g_72129c = c;
    g_7216bc = e;
    g_7212b0 = f;
    return CallC<int>(FUN_5dbf00, a);
}

// [FUN_5dd4d0] stops a thread other than the main one
void __cdecl App_CheckThread()
{
    void* t = AfxGetThread_();
    if (t == g_mainThread || t == F(void*, g_app, 396))
        return;
    F(int, g_app, 328) = 0;
    if (!g_721774)
        for (;;) {
        }
}

// [FUN_5de5b0]
void __fastcall Clock_Start(BYTE* self, void*)
{
    F(double, self, 608) = CallC<double>(FUN_6037e0) * 0.001;
}

// [FUN_5de5d0]
void __fastcall Clock_Stop(BYTE* self, void*)
{
    F(double, self, 600) = CallC<double>(FUN_6037e0) * 0.001 - F(double, self, 608) + F(double, self, 600);
}

// Node helpers by type (+16: 6 a frame, 18 a switch, 12 a light).

// [FUN_5e19b0]
void __cdecl Frame_Apply(BYTE* node, int mode, const void* m)
{
    if (node && F(int, node, 16) == 6)
        Node_ApplyMatrix(node, 0, m, mode);
}

// [FUN_5e19d0]
int __cdecl Frame_GetWorld(BYTE* node, void* out)
{
    if (F(int, node, 16) == 6)
        return Node_GetWorld(node, 0, out);
    return 0;
}

// [FUN_5e19f0]
void __cdecl Frame_GetLocal(BYTE* node, void* out)
{
    if (F(int, node, 16) == 6)
        Node_GetLocal(node, 0, out);
}

// [FUN_5e1a10]
void __cdecl Switch_Select(BYTE* node, int i)
{
    if (node && F(int, node, 16) == 18 && i >= 0 && i < CallVirt<int>(node, 60))
        F(int, node, 56) = i;
}

// [FUN_5e1a40]
int __cdecl Switch_Selected(BYTE* node)
{
    if (!node || F(int, node, 16) != 18)
        return -1;
    return F(int, node, 56);
}

// [FUN_5e1a60]
void __cdecl Light_SetColor(BYTE* node, const DWORD* c)
{
    DWORD* d = (DWORD*)(node + (F(int, node, 16) == 12 ? 84 : 56));
    d[0] = c[0];
    d[1] = c[1];
    d[2] = c[2];
}

// [FUN_5e1aa0]
void __cdecl Light_GetColor(BYTE* node, DWORD* c)
{
    const DWORD* s = (DWORD*)(node + (F(int, node, 16) == 12 ? 84 : 56));
    c[0] = s[0];
    c[1] = s[1];
    c[2] = s[2];
}

// [FUN_5e1ad0]
BYTE* __cdecl Node_ParentRoot(BYTE* node)
{
    BYTE* p = F(BYTE*, node, 44);
    if (!p)
        return 0;
    return Node_Root(p, 0);
}

// [FUN_5e2940] touches 6.4 MB twice (commits the memory)
void __cdecl Memory_Touch()
{
    BYTE* blocks[100];
    for (int i = 0; i < 100; i++)
        blocks[i] = (BYTE*)operator new(65536);
    for (int pass = 0; pass < 2; pass++)
        for (int i = 0; i < 100; i++)
            for (int k = 0; k < 65536; k++)
                ((volatile BYTE*)blocks[i])[k] = (BYTE)k;
    for (int i = 0; i < 100; i++)
        operator delete(blocks[i]);
}

// [FUN_5e2e50]
BYTE* __fastcall Obj654420_Construct(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl654420;
    F(int, self, 4) = 0;
    return self;
}

// The window list: +240 first, +244 last, +248 active, +324.

// [FUN_5e4210]
BYTE* __fastcall Windows_Add(BYTE* self, void*, BYTE* w)
{
    if (!F(BYTE*, self, 244)) {
        F(BYTE*, self, 240) = w;
        F(BYTE*, self, 244) = w;
        return w;
    }
    Node_Link(w, 0, F(BYTE*, self, 244));
    F(BYTE*, self, 244) = w;
    return w;
}

// [FUN_5e4140]
BYTE* __fastcall Windows_AddActive(BYTE* self, void*, BYTE* w)
{
    Windows_Add(self, 0, w);
    F(BYTE*, self, 248) = w;
    F(BYTE*, self, 324) = w;
    return w;
}

// [FUN_5e41f0]
BYTE* __fastcall Windows_Add2(BYTE* self, void*, BYTE* w)
{
    Windows_Add(self, 0, w);
    return w;
}

// [FUN_5e4250]
BYTE* __fastcall Windows_SetActive(BYTE* self, void*, BYTE* w)
{
    BYTE* old = F(BYTE*, self, 248);
    F(BYTE*, self, 248) = w;
    return old;
}

// [FUN_5e4300] the directory of path (with its slash), "." if none
const char* __cdecl DataPath_Set(const char* path)
{
    char* buf = g_dataPath;
    strncpy(buf, path, 250);
    buf[250] = 0;
    int i = (int)strlen(buf) - 1;
    for (; i >= 0; i--)
        if (buf[i] == ':' || buf[i] == '/' || buf[i] == '\\')
            break;
    if (i < 0) {
        g_dataDir = s_dot;
        return g_dataDir;
    }
    if (buf[i] != ':' && i != 0)
        buf[i] = 0;
    else
        buf[i + 1] = 0;
    g_dataDir = buf;
    return g_dataDir;
}

// [FUN_5e4450] stops the worker and waits for it
void __fastcall Worker_Stop(BYTE* self, void*)
{
    if (F(int, self, 20))
        while (!F(volatile int, self, 16))
            Sleep(10);
    F(int, self, 12) = 1;
    SetEvent(F(HANDLE, self, 36));
    while (F(volatile int, self, 32))
        Sleep(10);
}

// [FUN_5e44a0]
void __fastcall ResRef_Release(BYTE* self, void*)
{
    BYTE* owner = F(BYTE*, self, 28);
    CallVirt(owner, 84, F(int, self, 20), 2, F(int, self, 24));
    BYTE* table = CallVirt<BYTE*>(F(BYTE*, self, 28), 112);
    if (table)
        Resources_Unload(table, 0, F(const char*, self, 20));
    F(int, self, 20) = 0;
    F(int, self, 16) = 0;
}

// [FUN_5e4660]
BYTE* __fastcall Resource_Construct(BYTE* self, void*, int a, int unused)
{
    F(int, self, 4) = a;
    F(int, self, 0) = 0;
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    F(int, self, 12) = 0;
    F(int, self, 28) = 0;
    F(int, self, 16) = 0;
    F(BYTE, self, 8) = 0;
    return self;
}

// [FUN_5e4690] reads size and data from an IStream
int __fastcall Resource_Read(BYTE* self, void*, BYTE* stream)
{
    DWORD got;
    DWORD size;
    Com(stream, 12, &size, 4, &got);
    F(DWORD, self, 20) = size;
    if (size) {
        F(void*, self, 24) = operator new(size);
        Com(stream, 12, F(void*, self, 24), size, &got);
    } else {
        F(void*, self, 24) = 0;
    }
    F(int, self, 0) = 2;
    return 0;
}

// [FUN_5e4700]
int __fastcall Resource_Write(BYTE* self, void*, BYTE* stream)
{
    DWORD put;
    DWORD size = F(DWORD, self, 20);
    Com(stream, 16, &size, 4, &put);
    Com(stream, 16, F(void*, self, 24), size, &put);
    return 0;
}

// [FUN_5e4740]
void __fastcall Resource_Free(BYTE* self, void*)
{
    operator delete(F(void*, self, 24));
}

// The resource table: 1000 slots at +12, state at +4012, name at +4016.

// [FUN_5e48a0]
int __fastcall ResTable_Remove(BYTE* self, void*, BYTE* r)
{
    for (int i = 0; i < 1000; i++)
        if (F(BYTE*, self, 12 + i * 4) == r) {
            F(BYTE*, self, 12 + i * 4) = 0;
            F(int, r, 12) = 0;
            return 1;
        }
    return 0;
}

// [FUN_5e48e0]
void __fastcall ResTable_Clear(BYTE* self, void*)
{
    for (int i = 0; i < 1000; i++)
        F(int, self, 12 + i * 4) = 0;
}

// [FUN_5e49d0] loads a resource through the loader of ctx
void __fastcall Resource_Load(BYTE* self, void*, BYTE* ctx)
{
    typedef void(__cdecl * Loader)(BYTE * r, int);
    if (F(int, self, 0) != 1 || !F(Loader, ctx, 2008))
        return;
    F(int, self, 0) = 2;
    F(DWORD, ctx, 2000) += F(DWORD, self, 16);
    g_720930 = g_702e9c;
    F(Loader, ctx, 2008)(self, 0);
    if (F(DWORD, self, 16)) {
        g_resPending--;
        CallC(FUN_5e48f0, F(DWORD, ctx, 2000), self, 1);
    }
}

// [FUN_5e4a50]
void __fastcall ResTable_Unload(BYTE* self, void*, BYTE* ctx)
{
    if (F(int, self, 4012) != 2)
        return;
    F(int, self, 4012) = 1;
    if (!F(int, ctx, 2004))
        return;
    for (int i = 0; i < 1000; i++) {
        BYTE* r = F(BYTE*, self, 12 + i * 4);
        if (r && !(F(BYTE, r, 8) & 1))
            CallThis(FUN_5e4900, r, ctx);
    }
}

// [FUN_5e4aa0]
void __fastcall ResTable_Load(BYTE* self, void*, BYTE* ctx)
{
    if (F(int, self, 4012) != 1)
        return;
    if (F(int, ctx, 2008))
        for (int i = 0; i < 1000; i++) {
            BYTE* r = F(BYTE*, self, 12 + i * 4);
            if (r)
                Resource_Load(r, 0, ctx);
        }
    F(int, self, 4012) = 2;
}

// [FUN_5e4af0]
BYTE* __fastcall Resources_Construct(BYTE* self, void*)
{
    for (int i = 0; i < 400; i++)
        F(int, self, i * 4) = 0;
    F(int, self, 2000) = 0;
    F(int, self, 2004) = 0;
    F(int, self, 2008) = 0;
    for (int i = 0; i < 100; i++)
        F(int, self, 1600 + i * 4) = 0;
    return self;
}

// [FUN_5e4ec0] the table of this name
BYTE* __fastcall Resources_Find(BYTE* self, void*, const char* name)
{
    if (!name)
        return 0;
    int h = 0;
    for (const BYTE* p = (const BYTE*)name; *p; p++)
        h += *p;
    for (BYTE* t = F(BYTE*, self, 1600 + (h % 100) * 4); t; t = F(BYTE*, t, 8))
        if (!strcmp(name, F(const char*, t, 4016)))
            return t;
    return 0;
}

// [FUN_5e5370] resource id: table id / 1000, slot id % 1000
int __fastcall Resources_Get(BYTE** self, void*, int id)
{
    if (id < 0)
        return 0;
    int t = id / 1000, i = id % 1000;
    if (t > 400)
        return 0;
    int* p = (int*)self[t];
    if (!p || i >= 500)
        return 0;
    return p[i];
}

// [FUN_5e53e0]
void __fastcall Resources_Load(BYTE* self, void*, const char* name)
{
    BYTE* t = Resources_Find(self, 0, name);
    if (t)
        ResTable_Load(t, 0, self);
}

// [FUN_5e5400]
void __fastcall Resources_Unload(BYTE* self, void*, const char* name)
{
    BYTE* t = Resources_Find(self, 0, name);
    if (t)
        ResTable_Unload(t, 0, self);
}

// [FUN_5e5cb0]
void __fastcall Obj5e5cb0_Init(BYTE* self, void*)
{
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(int, self, 20) = 0;
    F(int, self, 16) = 0;
    F(BYTE, self, 12) = 0;
    F(int, self, 32) = 0;
    F(int, self, 4) = 0;
}

// A buffered mmio file: +0 handle, +4 the buffer (257 sectors),
// +131588 the read pointer, +131592 the bytes wanted.

// [FUN_5e6b90]
void __fastcall Mmio_Close(BYTE* self, void*)
{
    mmioClose(F(HMMIO, self, 0), 0);
}

// [FUN_5e6ba0]
void __fastcall Mmio_Reset(BYTE* self, void*)
{
    F(int, self, 131588) = 0;
}

// [FUN_5e6bb0]
void __fastcall Mmio_Reset2(BYTE* self, void*)
{
    F(int, self, 131588) = 0;
}

// [FUN_5e6bc0] reads the sectors holding [pos, pos + len)
void __fastcall Mmio_ReadAt(BYTE* self, void*, int pos, int len)
{
    int rem = pos % 512;
    int t = rem + len;
    int n = t / 512 + (t % 512 != 0);
    if (n > 257)
        return;
    mmioSeek(F(HMMIO, self, 0), pos - rem, 0);
    F(int, self, 131592) = len;
    mmioRead(F(HMMIO, self, 0), (HPSTR)(self + 4), n * 512);
    F(BYTE*, self, 131588) = self + 4 + rem;
}

// [FUN_5e6c50]
int __fastcall Mmio_Open(BYTE* self, void*, char* name)
{
    F(HMMIO, self, 0) = mmioOpenA(name, 0, 0);
    return 1;
}

// [FUN_5e6db0]
void __cdecl Files_SetDir(const char* dir)
{
    strcpy(g_fileDir, dir);
}

// [FUN_5e6de0]
FILE* __cdecl Files_Open(const char* name, const char* mode)
{
    strcpy(g_filePath, g_fileDir);
    strcat(g_filePath, name);
    return fopen(g_filePath, mode);
}

// [FUN_5e99a0]
int __cdecl Image_SetData(BYTE* img, int unused, int data)
{
    F(int, img, 4) = data;
    F(DWORD, img, 24) |= 4;
    return 1;
}

// [FUN_5e99c0] reads the image data (after a 44-byte header unless raw)
int __cdecl Image_Read(BYTE* img, FILE* f, long off)
{
    fseek(f, off + ((F(BYTE, img, 24) & 4) ? 0 : 44), 0);
    fread(F(void*, img, 8), F(DWORD, img, 4), 1, f);
    F(DWORD, img, 24) |= 1;
    return 1;
}

// [FUN_5e9cb0] a 24-bit BITMAPINFOHEADER for the bitmap
BITMAPINFOHEADER* __cdecl Bitmap_Header24(HBITMAP h)
{
    BITMAP bm;
    if (!GetObjectA(h, 24, &bm))
        return 0;
    BITMAPINFOHEADER* bi = (BITMAPINFOHEADER*)LocalAlloc(64, 40);
    bi->biSize = 40;
    bi->biWidth = bm.bmWidth;
    bi->biHeight = bm.bmHeight;
    bi->biPlanes = bm.bmPlanes;
    bi->biBitCount = 24;
    bi->biCompression = 0;
    bi->biClrImportant = 0;
    bi->biSizeImage = (((bi->biWidth * 24 + 31) / 32) * bi->biHeight * 32) / 8;
    return bi;
}

// [FUN_5e9ff0]
const char* __cdecl Image_ErrorText(int code)
{
    switch (code) {
    case 1:
        return s_errNotFound;
    case 2:
        return s_errMemory;
    case 3:
        return s_errFormat;
    }
    return s_errUnknown;
}

// A font: +0 flags (bit 0 proportional), +4 first, +8 last character,
// +12 widths, +24 the fixed width.

// [FUN_5ea020]
int __fastcall Font_CharWidth(BYTE* self, void*, int c)
{
    if ((BYTE)c == ' ' || !(F(BYTE, self, 0) & 1))
        return F(int, self, 24);
    return F(BYTE*, self, 12)[(c & 255) - F(int, self, 4)];
}

// [FUN_5ea050] width of up to n characters of the first line
int __fastcall Font_TextWidthN(BYTE* self, void*, const char* s, int n)
{
    if (!s)
        return 0;
    if (!(F(BYTE, self, 0) & 1)) {
        int len = (int)strlen(s);
        if (n < len)
            len = n;
        return F(int, self, 24) * len;
    }
    int w = 0;
    for (int i = 0; s[i] && i < n && s[i] != '\n'; i++) {
        int c = (BYTE)s[i];
        if (c >= F(int, self, 4) && c <= F(int, self, 8))
            w += F(BYTE*, self, 12)[c - F(int, self, 4)];
        else
            w += F(int, self, 24);
    }
    return w;
}

// [FUN_5ea0e0] the width of the widest line
int __fastcall Font_TextWidth(BYTE* self, void*, const char* s)
{
    if (!(F(BYTE, self, 0) & 1))
        return (int)strlen(s) * F(int, self, 24);
    int widest = 0, w = 0;
    for (; *s; s++) {
        int c = (BYTE)*s;
        if (c == '\n') {
            if (w > widest)
                widest = w;
            w = 0;
        } else if (c >= F(int, self, 4) && c <= F(int, self, 8)) {
            w += F(BYTE*, self, 12)[c - F(int, self, 4)];
        } else {
            w += F(int, self, 24);
        }
    }
    return w > widest ? w : widest;
}

// [FUN_5ea170]
void __cdecl Text_Draw(int a, int b, int c, BYTE* font, const char* s, int d, int e)
{
    if (font && s)
        CallC(FUN_5ea1b0, a, b, c, Font_TextWidth(font, 0, s) + 1, font, s, d, e);
}

}

// --- constructors/destructors with exception frames (the frames only rethrow)

extern "C" {

// [FUN_5d8650]
BYTE* __fastcall Fw5d8650_Construct(BYTE* self, void*, int a, int b, int c, int d)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl652168;
    BYTE* o = (BYTE*)operator new(124);
    BYTE* r = o ? CallThis<BYTE*>(FUN_5b6a50, o, a, b, c, d) : 0;
    F(int, self, 24) = 0;
    F(BYTE*, self, 12) = r;
    F(int, self, 52) = 0;
    F(int, self, 48) = 0;
    F(int, self, 28) = 0;
    F(int, self, 16) = 3;
    return self;
}

// [FUN_5da840] makes the 40-byte helper at +484 on first use
void __fastcall Fw_EnsureHelper484(BYTE* self, void*)
{
    if (F(BYTE*, self, 484))
        return;
    BYTE* o = (BYTE*)operator new(40);
    F(BYTE*, self, 484) = o ? CallThis<BYTE*>(FUN_5e43e0, o, self) : 0;
}

// [FUN_5e2730] the application's 196-byte object at +384
BYTE* __cdecl Fw_NewApp384()
{
    BYTE* o = (BYTE*)operator new(196);
    BYTE* r = o ? CallThis<BYTE*>(FUN_5dd0d0, o) : 0;
    F(BYTE*, g_app, 384) = r;
    return r;
}

// [FUN_5e4170] a new 64-byte child with flag 4, registered with self
BYTE* __fastcall Fw_NewChild64(BYTE* self, void*, int a)
{
    BYTE* o = (BYTE*)operator new(64);
    BYTE* r = o ? View_Construct(o, 0, F(int, self, 252), (BYTE*)a) : 0;
    F(DWORD, r, 16) |= 4;
    Windows_Add(self, 0, r);
    return r;
}

// [FUN_5e4750] a named table of 1000 entries
BYTE* __fastcall NamedTable_Construct(BYTE* self, void*, const char* name)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl652738;
    char* p = (char*)operator new(strlen(name) + 1);
    F(char*, self, 4016) = p;
    strcpy(p, name);
    memset(self + 12, 0, 4000);
    F(int, self, 4012) = 2;
    return self;
}

// [FUN_5e4810]
void __fastcall NamedTable_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl652738;
    operator delete(F(void*, self, 4016));
    F(void*, self, 0) = g_vtbl64d81c;
}

}

// --- windows, resources and the screen grab

typedef void (*FwFn)();

static HRESULT Stream_Read(BYTE* s, void* p, int n, ULONG* got)
{
    return CallStd<HRESULT>(F(FwFn, F(BYTE*, s, 0), 12), s, p, n, got);
}

extern "C" {

// [FUN_5d8af0] a framed window: the base with the rectangle in the arguments
BYTE* __fastcall FrameWindow_Construct(BYTE* self, void*, int a, int x0, int y0, int x1, int y1)
{
    CallThis(FUN_5d8710, self, a, &x0, 0);
    F(void*, self, 0) = g_vtbl652238;
    F(DWORD, self, 16) |= 4;
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    return self;
}

// [FUN_5d8bf0] enumerates with the callback at $L_5d8c60 over a zeroed context
int __cdecl Fw_Enumerate5d8c60(int a, int b, BYTE* obj)
{
    BYTE ctx[432];
    memset(ctx, 0, sizeof ctx);
    F(int, ctx, 0) = a;
    F(int, ctx, 16) = b;
    return CallThis<int>(F(FwFn, F(BYTE*, obj, 0), 32), obj, (void*)g_enum5d8c60, ctx);
}

// [FUN_5d9470]
int __cdecl Fw_Enumerate5d94d0(int a, BYTE* obj)
{
    BYTE ctx[432];
    memset(ctx, 0, sizeof ctx);
    F(int, ctx, 0) = a;
    return CallThis<int>(F(FwFn, F(BYTE*, obj, 0), 32), obj, (void*)g_enum5d94d0, ctx);
}

// [FUN_5e3ba0] the start-up steps when not yet running
int __cdecl Fw_Startup()
{
    if (g_721754)
        return g_721754;
    ((FwFn)g_67fb7c)();
    CallC(FUN_4cef50);
    CallC(FUN_4cef50);
    return CallC<int>((FwFn)(void*)g_start603800);
}

// [FUN_5e4b30] reads up to 400 groups of resources from the stream
int __fastcall Resources_Read(BYTE** self, void*, BYTE* stream)
{
    ULONG got;
    int t;
    for (int k = 400; k; k--) {
        Stream_Read(stream, &t, 4, &got);
        int first = t;
        Stream_Read(stream, &t, 4, &got);
        if (!first)
            continue;
        while (t != -2) {
            if (t >= 0) {
                int len;
                BYTE buf[32];
                Stream_Read(stream, &len, 4, &got);
                Stream_Read(stream, buf, len, &got);
                CallThis(FUN_5e5130, self, t, buf);
                Resource_Read((BYTE*)Resources_Get(self, 0, t), 0, stream);
            }
            Stream_Read(stream, &t, 4, &got);
        }
    }
    return 0;
}

// [FUN_5e4d30] frees all resource tables and the 100 lists
void __fastcall Resources_FreeAll(BYTE** self, void*)
{
    g_resPending = 0;
    for (int i = 0; i < 400; i++) {
        BYTE** t = (BYTE**)self[i];
        if (!t)
            continue;
        for (int j = 0; j < 500; j++) {
            BYTE* e = t[j];
            if (e) {
                Resource_Free(e, 0);
                operator delete(e);
                ((BYTE**)self[i])[j] = 0;
            }
        }
        operator delete(self[i]);
        self[i] = 0;
    }
    BYTE** lists = self + 400;
    for (int i = 0; i < 100; i++) {
        BYTE* n = lists[i];
        if (!n)
            continue;
        do {
            BYTE* next = F(BYTE*, n, 8);
            ResTable_Clear(n, 0);
            CallThis(FUN_5d21b0, n);
            if (n)
                CallVirt(n, 0, 1);
            n = next;
        } while (n);
        lists[i] = 0;
    }
    F(int, self, 2004) = 0;
    F(int, self, 2008) = 0;
}

// [FUN_5e9f00] grabs the primary surface into a bitmap and saves it
int __cdecl Screen_GrabToFile(const char* file)
{
    BYTE* surf = g_7208fc;
    HDC dc;
    CallStd(F(FwFn, F(BYTE*, surf, 0), 68), surf, &dc);
    HDC mem = CreateCompatibleDC(dc);
    int h = CallC<int>(FUN_5d3b30);
    int w = CallC<int>(FUN_5d3b20);
    HBITMAP bmp = CreateCompatibleBitmap(dc, w, h);
    if (!SelectObject(mem, bmp))
        CallC(FUN_5d2ba0, (const char*)s_683538, 0);
    h = CallC<int>(FUN_5d3b30);
    w = CallC<int>(FUN_5d3b20);
    BOOL ok = BitBlt(mem, 0, 0, w, h, dc, 0, 0, SRCCOPY);
    if (!ok)
        CallC(FUN_5d2ba0, (const char*)s_683538, 0);
    SelectObject(mem, 0);
    BITMAPINFOHEADER* dib = Bitmap_Header24(bmp);
    if (!dib)
        return 0;
    CallC(FUN_5e9d40, file, dib, bmp, mem);
    surf = g_7208fc;
    CallStd(F(FwFn, F(BYTE*, surf, 0), 104), surf, dc);
    GlobalFree(dib);
    DeleteObject(bmp);
    DeleteObject(mem);
    return 1;
}

}

// ---------------------------------------------------------------------------
extern "C" void FUN_5d27c0(); KING_ALIAS("_FUN_5d27c0", "@NameHash_ResolvePending@8")
extern "C" void FUN_5ea990(); KING_ALIAS("_FUN_5ea990", "@Room_EachGroup@8")
extern "C" void FUN_5e4740(); KING_ALIAS("_FUN_5e4740", "@Resource_Free@8")
extern "C" void FUN_5e48a0(); KING_ALIAS("_FUN_5e48a0", "@ResTable_Remove@12")
extern "C" void FUN_5e4ec0(); KING_ALIAS("_FUN_5e4ec0", "@Resources_Find@12")
extern "C" void FUN_5e4660(); KING_ALIAS("_FUN_5e4660", "@Resource_Construct@16")
ASM_PROC(FUN_5dc230)    // (this, a) -> b
ASM_PROC(FUN_5dae90)    // (this, b, c, d)
ASM_PROC(FUN_5e4f60)    // resources: load table (this, name)
ASM_PROC(FUN_5e48f0)
ASM_PROC(FUN_5d2ba0)
ASM_VAR(AsmLabel, g_objectNames, $L_71e820)
ASM_VAR(AsmLabel, s_6832d4, $L_6832d4)
ASM_VAR(AsmLabel, s_683338, $L_683338)
ASM_VAR(AsmLabel, s_6833a8, $L_6833a8)
ASM_VAR(AsmLabel, s_683384, $L_683384)
ASM_VAR(AsmLabel, s_683314, $L_683314)
ASM_VAR(AsmLabel, s_6832fc, $L_6832fc)
ASM_VAR(AsmLabel, s_6833cc, $L_6833cc)
ASM_VAR(AsmLabel, s_68337c, $L_68337c)
ASM_VAR(AsmLabel, s_68335c, $L_68335c)
ASM_VAR(int, g_702e9c, $L_702e9c)
ASM_VAR(int, g_720930, $L_720930)
ASM_VAR(int, g_resPending, $L_7217a4)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_5e4860..$L_5e488a] puts a resource into the first free one of the 1000 slots at +12; returns the table's +4012 (-1 and fatal when full)
extern "C" int __fastcall ResTable_Add(BYTE* self, void*, BYTE* res)
{
    for (int i = 0; i < 1000; i++)
        if (!F(BYTE*, self, 12 + i * 4))
        {
            F(BYTE*, self, 12 + i * 4) = res;
            F(BYTE*, res, 12) = self;
            return F(int, self, 4012);
        }
    Sys_Fatal(s_6832d4, 0);
    return -1;
}

// [FUN_5dc3e0..$L_5dc417] sets (a, b, c) through 5dc230/5dae90, resolves pending object names (fatal on a missing one) and walks the room's groups
extern "C" void __fastcall Light_Update(BYTE* self, void*, int a, int b, int c)
{
    int x = CallThis<int>(FUN_5dc230, self, a);
    CallThis(FUN_5dae90, self, x, b, c);
    const char* missing = CallThis<const char*>(FUN_5d27c0, g_objectNames);
    if (missing)
        Sys_Fatal(missing, 0);
    if (F(BYTE*, self, 468))
        CallThis(FUN_5ea990, F(BYTE*, self, 468));
}

// [FUN_5e50d0..$L_5e50ec] allocates row idx (of 400) of 500 cleared slots; 0 when it exists already
extern "C" int __fastcall Resources_NewRow(BYTE* self, void*, int idx)
{
    if (idx >= 400)
        Sys_Fatal(s_683338, 0);
    if (F(int*, self, idx * 4))
        return 0;
    F(int*, self, idx * 4) = (int*)operator new(2000);
    for (int i = 0; i < 500; i++)
        F(int*, self, idx * 4)[i] = 0;
    return 1;
}

// [FUN_5e5210..$L_5e5272] frees and deletes resource id (row id/1000, slot id%1000); -1 for a missing row, else 3
extern "C" int __fastcall Resources_Delete(BYTE* self, void*, int id)
{
    int row = id / 1000, slot = id % 1000;
    if (row >= 400)
        Sys_Fatal(s_6833a8, 0);
    if (!F(BYTE**, self, row * 4))
        return -1;
    if (slot >= 500)
        Sys_Fatal(s_683384, 0);
    BYTE* r = F(BYTE**, self, row * 4)[slot];
    if (r)
    {
        CallThis(FUN_5e4740, r);
        operator delete(r);
        F(BYTE**, self, row * 4)[slot] = 0;
    }
    return 3;
}

typedef void (__cdecl* ResCallback)(BYTE* res, int loaded);

// [FUN_5e4900..$L_5e49ba] charges a waiting resource's size to the table's budget and tells its callback (or reports it doesn't fit)
extern "C" void __fastcall ResTable_Charge(BYTE* self, void*, BYTE* tab)
{
    if (F(int, F(BYTE*, self, 12), 4012) == 2 || F(int, self, 0) != 2)
        return;
    if (!F(ResCallback, tab, 2004))
        return;
    if (F(unsigned, tab, 2000) < F(unsigned, self, 16))
    {
        F(ResCallback, tab, 2004)(self, 0);
        return;
    }
    if (F(int, self, 28))
    {
        CallC(FUN_5d2ba0, (char*)s_683314, F(int, self, 4));
        Sys_Fatal(s_6832fc, 0);
    }
    F(int, self, 0) = 1;
    F(unsigned, tab, 2000) -= F(unsigned, self, 16);
    g_720930 = g_702e9c;
    F(ResCallback, tab, 2004)(self, 1);
    if (F(unsigned, self, 16) > 0)
    {
        g_resPending++;
        CallC(FUN_5e48f0, F(int, tab, 2000), self, 0);
    }
}

// [FUN_5e5420..$L_5e549d] moves a resource into the table named name (loading the table when needed; fatal when there is none)
extern "C" int __fastcall Resources_Bind(BYTE* self, void*, const char* name, BYTE* res)
{
    if (!res)
        return -1;
    if (F(BYTE*, res, 12))
        CallThis(FUN_5e48a0, F(BYTE*, res, 12), res);
    if (!name)
        return -1;
    BYTE* t = CallThis<BYTE*>(FUN_5e4ec0, self, name);
    if (!t)
    {
        CallThis(FUN_5e4f60, self, name);
        t = CallThis<BYTE*>(FUN_5e4ec0, self, name);
        if (!t)
        {
            char text[84];
            sprintf(text, s_6833cc, name);
            Sys_Fatal(text, 0);
        }
    }
    return ResTable_Add(t, 0, res);
}

// [FUN_5e5130..$L_5e51ad] creates resource id in table name; -1 for a missing row, -2 when it exists, else the new resource's first field
extern "C" int __fastcall Resources_Create(BYTE* self, void*, int id, const char* name)
{
    int row = id / 1000, slot = id % 1000;
    if (row >= 400)
        Sys_Fatal(s_683338, 0);
    if (!F(BYTE**, self, row * 4))
        return -1;
    if (slot >= 500)
    {
        char text[12];
        sprintf(text, s_68337c, id);
        Sys_Fatal(s_68335c, text);
    }
    if (F(BYTE**, self, row * 4)[slot])
        return -2;
    void* m = operator new(32);
    BYTE* r = m ? CallThis<BYTE*>(FUN_5e4660, m, id, name) : 0;
    F(BYTE**, self, row * 4)[slot] = r;
    Resources_Bind(self, 0, name, r);
    return F(int, r, 0);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5ab790(); KING_ALIAS("_FUN_5ab790", "@Vec3_Read@12")
extern "C" void FUN_5ab190(); KING_ALIAS("_FUN_5ab190", "_File_ReadFloat")
extern "C" void FUN_5e44a0(); KING_ALIAS("_FUN_5e44a0", "@ResRef_Release@8")
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")
extern "C" void FUN_5d3460(); KING_ALIAS("_FUN_5d3460", "@Owner_Clear@8")
extern "C" void FUN_5b64e0(); KING_ALIAS("_FUN_5b64e0", "@Window_SetSurface@12")
extern "C" void Afx_BeginThread(); KING_ALIAS("_Afx_BeginThread", "?AfxBeginThread@@YGPAVCWinThread@@P6AIPAX@Z0HIKPAU_SECURITY_ATTRIBUTES@@@Z")
extern "C" void Fw_ThreadProc(); KING_ALIAS("_Fw_ThreadProc", "$L_5e4390")
ASM_PROC(FUN_5e4e00)
ASM_PROC(FUN_5baa70)
ASM_VAR(double, g_652730, $L_652730)
ASM_VAR(AsmLabel, s_6832bc, $L_6832bc)
ASM_VAR(AsmLabel, s_683294, $L_683294)
ASM_VAR(AsmLabel, s_683288, $L_683288)

// [FUN_5e5720] reads two vectors and two floats (+28, then +24)
extern "C" void __fastcall Fw_Read5e5720(BYTE* self, void*, FILE* f)
{
    CallThis(FUN_5ab790, self, f);
    CallThis(FUN_5ab790, self + 12, f);
    F(float, self, 28) = CallC<float>(FUN_5ab190, f);
    F(float, self, 24) = CallC<float>(FUN_5ab190, f);
}

// [FUN_5e56f0] reads a vector and two floats (+12, +16)
extern "C" void __fastcall Fw_Read5e56f0(BYTE* self, void*, FILE* f)
{
    CallThis(FUN_5ab790, self, f);
    F(float, self, 12) = CallC<float>(FUN_5ab190, f);
    F(float, self, 16) = CallC<float>(FUN_5ab190, f);
}

// [FUN_5e43e0] a loader thread on scenes a, with its wake event
extern "C" BYTE* __fastcall Loader_Construct(BYTE* self, void*, int a)
{
    F(int, self, 8) = 0;
    F(int, self, 28) = a;
    F(int, self, 24) = 0;
    F(int, self, 12) = 0;
    F(int, self, 20) = 0;
    F(int, self, 16) = 0;
    F(double, self, 0) = Timer_Now();
    F(void*, self, 32) = CallStd<void*>(Afx_BeginThread, Fw_ThreadProc, self, 0, 0, 0, 0);
    SECURITY_ATTRIBUTES sa = { 12, 0, 1 };
    F(HANDLE, self, 36) = CreateEventA(&sa, 0, 0, s_683288);
    return self;
}

// [FUN_5e44e0..$L_5e45ad] asks the loader thread for scene name (a 1-2 letter code), or lowers/raises its priority
extern "C" void __fastcall Loader_Request(BYTE* self, void*, int unused, const char* name)
{
    if (!name)
        return;
    if (F(int, self, 16))
    {
        CallThis(FUN_5e44a0, self);
        SetThreadPriority(F(HANDLE, F(BYTE*, self, 32), 40), 0);
        return;
    }
    if (F(int, self, 20))
    {
        double due = F(double, self, 0) + g_652730;
        if (!(Timer_Now() > due))
            return;
        F(double, self, 0) = Timer_Now();
        SetLastError(0);
        if (!SetThreadPriority(F(HANDLE, F(BYTE*, self, 32), 40), 1))
            CallC(FUN_5d2ba0, (char*)s_6832bc, GetLastError());
        return;
    }
    int code = 0, n = 0;
    while (name[n])
        code = code * 256 + (signed char)name[n++];
    if (n > 2)
        Sys_Fatal(s_683294, name);
    if (F(int, self, 8) == code)
        return;
    if (CallThis<BYTE*>(FUN_5d9290, F(BYTE*, self, 28), name))
        return;
    F(int, self, 8) = code;
    if (F(void*, self, 24))
        operator delete(F(void*, self, 24));
    DWORD* p = (DWORD*)operator new(432);
    if (p)
    {
        p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0;
        p[107] = p[106] = 0;
        memset(p + 6, 0, 400);
    }
    F(DWORD*, self, 24) = p;
    BYTE* t = CallVirt<BYTE*>(F(BYTE*, self, 28), 112);
    if (t)
        CallThis(FUN_5e4f60, t, name);
    CallVirt(F(BYTE*, self, 28), 84, name, 0, F(DWORD*, self, 24));
    F(const char*, self, 20) = name;
    F(double, self, 0) = Timer_Now();
    SetEvent(F(HANDLE, self, 36));
}

// [FUN_5e0180] clears the resources (+704) and the owner at +2992
extern "C" void __fastcall Fw_Clear5e0180(BYTE* self, void*)
{
    CallThis(FUN_5e4e00, self + 704);
    F(int, self, 3040) = 0;
    CallThis(FUN_5d3460, self + 2992);
}

// [FUN_5e0150] vtbl+68, vtbl+204, 5baa70(0) and no surface for the window at +12
extern "C" void __fastcall Fw_Close5e0150(BYTE* self, void*)
{
    CallVirt(self, 68);
    CallVirt(self, 204);
    CallC(FUN_5baa70, 0);
    CallThis(FUN_5b64e0, F(BYTE*, self, 12), 0);
}

// [FUN_5e27b0] operator delete (stdcall)
extern "C" void __stdcall Fw_Delete(void* p)
{
    operator delete(p);
}

