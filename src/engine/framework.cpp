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
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
extern "C" void FUN_5e9d40(); KING_ALIAS("_FUN_5e9d40", "_Bmp_Write")
KING_VAR(int, g_721754, 721754)
KING_VAR(void*, g_67fb7c, 67fb7c)
ASM_VAR(AsmLabel, g_start603800, _Effect_Zero)
KING_VAR(BYTE*, g_7208fc, 7208fc)
KING_VAR(AsmLabel, s_683538, 683538)
extern "C" int __cdecl Node_LinkByNames(BYTE* n, BYTE** ctx);
extern "C" int __cdecl Node_ClearLinkByName(BYTE* n, BYTE** ctx);
extern "C" void FUN_5b6a50(); KING_ALIAS("_FUN_5b6a50", "@Window_ConstructChild@24")
extern "C" void FUN_5e43e0(); KING_ALIAS("_FUN_5e43e0", "@Loader_Construct@12")
extern "C" void FUN_5dd0d0(); KING_ALIAS("_FUN_5dd0d0", "@MainFrame_Construct@4")
KING_VAR(AsmLabel, g_vtbl652168, 652168)
KING_VAR(AsmLabel, g_vtbl652738, 652738)
KING_VAR(AsmLabel, g_vtbl64d81c, 64d81c)
extern "C" void FUN_5b6bf0(); KING_ALIAS("_FUN_5b6bf0", "@Surface_Prepare@4")
extern "C" void FUN_5b6b10(); KING_ALIAS("_FUN_5b6b10", "@Surface_Present@8")
extern "C" void FUN_5d8710(); KING_ALIAS("_FUN_5d8710", "@Window_ConstructSub@20")    // window base constructor (a, rect, b)
extern "C" void FUN_5da950(); KING_ALIAS("_FUN_5da950", "_Matrix_FromPose")
extern "C" void FUN_5dbf00(); KING_ALIAS("_FUN_5dbf00", "_Probe_Walk")
extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")    // time in ms, returns double
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5e4900(); KING_ALIAS("_FUN_5e4900", "@ResTable_Charge@12")
extern "C" void FUN_5ea1b0(); KING_ALIAS("_FUN_5ea1b0", "_Font_DrawText")    // draws text
extern "C" void* __stdcall AfxGetThread_();
KING_ALIAS("_AfxGetThread_@0", "_Afx_GetThread@0")

KING_VAR(AsmLabel, g_vtbl652238, 652238)
KING_VAR(AsmLabel, g_vtbl654420, 654420)
KING_VAR(AsmLabel, s_foreground, 682c48)       // "Foreground_object"
KING_VAR(AsmLabel, s_background, 682c34)       // "Background_object"
KING_VAR(AsmLabel, s_skyDomeSpace, 682c24)
KING_VAR(AsmLabel, s_skyDomeKey, 682c18)
KING_VAR(int, g_7212a0, 7212a0)
KING_VAR(int, g_721298, 721298)
KING_VAR(int, g_72129c, 72129c)
KING_VAR(int, g_7216bc, 7216bc)
KING_VAR(int, g_7212b0, 7212b0)
KING_VAR(void*, g_mainThread, 72177c)
KING_VAR(BYTE*, g_app, 721750)
KING_VAR(int, g_721774, 721774)
KING_VAR(AsmLabel, g_dataPath, 7209d4)         // 251 bytes
KING_VAR(char*, g_dataDir, 720bb0)
KING_VAR(AsmLabel, s_dot, 682a2c)
KING_VAR(int, g_720930, 720930)
KING_VAR(int, g_resPending, 7217a4)
KING_VAR(AsmLabel, g_fileDir, 7217bc)
KING_VAR(AsmLabel, g_filePath, 72183c)
KING_VAR(AsmLabel, s_errUnknown, 6835ac)
KING_VAR(AsmLabel, s_errFormat, 683570)
KING_VAR(AsmLabel, s_errMemory, 68355c)
KING_VAR(AsmLabel, s_errNotFound, 68354c)

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
    return CallThis<int>(F(FwFn, F(BYTE*, obj, 0), 32), obj, (void*)Node_LinkByNames, ctx);
}

// [FUN_5d9470]
int __cdecl Fw_Enumerate5d94d0(int a, BYTE* obj)
{
    BYTE ctx[432];
    memset(ctx, 0, sizeof ctx);
    F(int, ctx, 0) = a;
    return CallThis<int>(F(FwFn, F(BYTE*, obj, 0), 32), obj, (void*)Node_ClearLinkByName, ctx);
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
extern "C" void FUN_5dc230(); KING_ALIAS("_FUN_5dc230", "@App_Path@12")    // (this, a) -> b
extern "C" void FUN_5dae90(); KING_ALIAS("_FUN_5dae90", "@Module_Load@20")    // (this, b, c, d)
extern "C" void FUN_5e4f60(); KING_ALIAS("_FUN_5e4f60", "@Resources_AddTable@12")    // resources: load table (this, name)
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
KING_VAR(AsmLabel, g_objectNames, 71e820)
KING_VAR(AsmLabel, s_6832d4, 6832d4)
KING_VAR(AsmLabel, s_683338, 683338)
KING_VAR(AsmLabel, s_6833a8, 6833a8)
KING_VAR(AsmLabel, s_683384, 683384)
KING_VAR(AsmLabel, s_683314, 683314)
KING_VAR(AsmLabel, s_6832fc, 6832fc)
KING_VAR(AsmLabel, s_6833cc, 6833cc)
KING_VAR(AsmLabel, s_68337c, 68337c)
KING_VAR(AsmLabel, s_68335c, 68335c)
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
extern "C" void Afx_BeginThread(); KING_ALIAS("_Afx_BeginThread", "_Afx_BeginThread@24")
extern "C" unsigned __cdecl Fw_LoaderThread(BYTE* p);
extern "C" void FUN_5e4e00(); KING_ALIAS("_FUN_5e4e00", "@Resources_FreeTables@8")
extern "C" void FUN_5baa70(); KING_ALIAS("_FUN_5baa70", "_TexPool_Return")
KING_VAR(double, g_652730, 652730)
KING_VAR(AsmLabel, s_6832bc, 6832bc)
KING_VAR(AsmLabel, s_683294, 683294)
KING_VAR(AsmLabel, s_683288, 683288)

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
    F(void*, self, 32) = CallStd<void*>(Afx_BeginThread, (void*)Fw_LoaderThread, self, 0, 0, 0, 0);
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


// ---------------------------------------------------------------------------
extern "C" void FUN_5ead00(); KING_ALIAS("_FUN_5ead00", "@Room_Each@16")
extern "C" void FUN_5ec0f0(); KING_ALIAS("_FUN_5ec0f0", "@Image_ReadResources@24")    // image: read (this, name, file, kind, path)
extern "C" void FUN_5d2f90(); KING_ALIAS("_FUN_5d2f90", "_Sys_FreeMemory")
extern "C" void FUN_5e9ff0(); KING_ALIAS("_FUN_5e9ff0", "_Image_ErrorText")
KING_VAR(AsmLabel, s_682b34, 682b34)
KING_VAR(AsmLabel, s_682cdc, 682cdc)
KING_VAR(AsmLabel, s_6703e8, 6703e8)
KING_VAR(AsmLabel, s_6765fc, 6765fc)
KING_VAR(AsmLabel, s_682cd8, 682cd8)
KING_VAR(AsmLabel, s_673398, 673398)
KING_VAR(AsmLabel, s_682cd4, 682cd4)
KING_VAR(AsmLabel, s_682cb8, 682cb8)
KING_VAR(AsmLabel, s_682ca4, 682ca4)
KING_VAR(AsmLabel, s_682c64, 682c64)
KING_VAR(int, g_7218bc, 7218bc)

// for each object of a room: a type 14 object with flag 2 whose names (+132 in the old room's list, +164 in its children) match is enumerated into the context's room
static int __cdecl Fw_LinkOldObject(BYTE* obj, BYTE* ctx)
{
    if (F(int, obj, 16) != 14 || !(F(BYTE, obj, 40) & 2))
        return -1;
    BYTE* e = F(BYTE*, ctx, 4);
    if (!e)
        return -1;
    while (strcmp(F(char*, e, 60), (char*)obj + 132))
    {
        e = F(BYTE*, e, 8);
        if (!e)
            return -1;
    }
    e = F(BYTE*, e, 64);
    if (!e)
        return -1;
    while (strcmp(F(char*, e, 12), (char*)obj + 164))
    {
        e = F(BYTE*, e, 8);
        if (!e)
            return -1;
    }
    Fw_Enumerate5d94d0(F(int, ctx, 8), e);
    F(int, obj, 128) = 0;
    return -1;
}

// [FUN_5d9530..$L_5d9567] links the objects of a new room to the current one (+468); fatal when the current room is the base one
extern "C" void __fastcall Fw_LinkRoom(BYTE* self, void*, BYTE* room)
{
    if (F(BYTE*, self, 468) == room)
        return;
    if (F(BYTE*, self, 468) == F(BYTE*, F(BYTE*, F(BYTE*, self, 488), 76), 36))
        Sys_Fatal(s_682b34, F(char*, room, 60));
    BYTE ctx[432];
    memset(ctx, 0, sizeof ctx);
    F(BYTE*, ctx, 4) = F(BYTE*, self, 468);
    F(BYTE*, ctx, 8) = room;
    CallThis(FUN_5ead00, room, (int(__cdecl*)(BYTE*, BYTE*))Fw_LinkOldObject, ctx);
}

// [FUN_5dac90..$L_5dae22] reads an image whose path's extension is tried as 682cdc (kind 3), 6765fc (2), 682cd8 and 682cd4 (1); returns the kind, 0 without an extension
extern "C" int __cdecl Image_LoadAny(BYTE* img, int a2, char* path)
{
    char* p = path;
    while (*p)
        p++;
    if (*p != '.')
    {
        while (p > path)
            if (*--p == '.')
                break;
        if (*p != '.')
            return 0;
    }
    p++;
    int kind;
    strcpy(p, s_682cdc);
    kind = 3;
    FILE* f = fopen(path, s_6703e8);
    if (!f)
    {
        strcpy(p, s_6765fc);
        kind = 2;
        f = fopen(path, s_6703e8);
        if (!f)
        {
            strcpy(p, s_682cd8);
            kind = 1;
            f = fopen(path, s_673398);
            if (!f)
            {
                strcpy(p, s_682cd4);
                f = fopen(path, s_673398);
                if (!f)
                    Sys_Fatal(s_682cb8, path);
            }
        }
    }
    int err = CallThis<int>(FUN_5ec0f0, img, a2, f, kind, path);
    if (err)
    {
        if (CallC<int>(FUN_5d2f90) < 150000)
            Sys_Fatal(s_682ca4, 0);
        else
        {
            sprintf(path, s_682c64, F(char*, img, 60), err, CallC<const char*>(FUN_5e9ff0, g_7218bc));
            Sys_Fatal(path, 0);
        }
    }
    if (kind == 1)
        fclose(f);
    return kind;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5ef390(); KING_ALIAS("_FUN_5ef390", "@Room_FindObject@12")
extern "C" int __cdecl Room_QueueDelete(BYTE* node, BYTE* ctx);     // room object callback: queue the objects to delete
KING_VAR(BOOL, g_fwTaskDump, 721768)
KING_VAR(BYTE*, g_fwTraceData, 71e814)
KING_VAR(int, g_fwTracePos, 71e818)
KING_VAR(int, g_fwTraceSize, 71e810)
KING_VAR(AsmLabel, s_682bc8, 682bc8)
KING_VAR(AsmLabel, s_682bb8, 682bb8)
KING_VAR(AsmLabel, s_682ba4, 682ba4)
KING_VAR(AsmLabel, s_682b94, 682b94)
KING_VAR(AsmLabel, s_682b84, 682b84)
KING_VAR(AsmLabel, s_682b74, 682b74)
KING_VAR(AsmLabel, s_682b58, 682b58)

// appends a text to the trace buffer when tasks are dumped
static void Fw_TraceText(const char* p)
{
    if (!g_fwTaskDump || !g_fwTraceData || !*p)
        return;
    while (g_fwTracePos < g_fwTraceSize)
    {
        g_fwTraceData[g_fwTracePos] = *p++;
        g_fwTracePos++;
        if (!*p)
            break;
    }
}

// [FUN_5d96c0..$L_5d99fe] switches to a room by name: deletes the old room's queued objects, unloads it, loads the new room's file, resources and objects, then finds the module (fatal on failures); returns the room
extern "C" BYTE* __fastcall Fw_LoadRoom(BYTE* self, void*, const char* name, const char* module)
{
    BYTE ctx[432];
    memset(ctx, 0, sizeof ctx);
    F(BYTE*, ctx, 20) = self;
    BYTE* res = CallVirt<BYTE*>(self, 112);
    BYTE* cur = F(BYTE*, F(BYTE*, self, 488), 76);
    if (cur)
    {
        BYTE* old = F(BYTE*, cur, 36);
        CallThis(FUN_5ead00, old, (void*)Room_QueueDelete, ctx);
        BYTE** items = (BYTE**)(ctx + 24);
        int& tail = F(int, ctx, 428);
        while (F(int, ctx, 424) != tail)
        {
            if (items[tail])
                CallVirt(items[tail], 0, 1);
            items[tail] = 0;
            tail = (tail + 1) % 100;
        }
        if (res)
            Resources_Load(res, 0, F(const char*, old, 60));
        Fw_LinkRoom(self, 0, old);
        CallThis(FUN_5d21b0, old);
        if (old)
            CallVirt(old, 0, 1);
    }
    BYTE a[80], b[80], c[80];
    CallVirt(self, 88, name, a, b, c);
    BYTE* room = CallThis<BYTE*>(FUN_5dae90, self, a, b, c);
    if (!room)
        Sys_Fatal(s_682bc8, name);
    const char* missing = CallThis<const char*>(FUN_5d27c0, g_objectNames);
    if (missing)
        Sys_Fatal(missing, 0);
    if (res)
    {
        Fw_TraceText(s_682bb8);
        CallThis(FUN_5e4f60, res, F(const char*, room, 60));
        Fw_TraceText(s_682ba4);
        CallVirt(self, 92, room);
        Fw_TraceText(s_682b94);
        Resources_Unload(res, 0, F(const char*, room, 60));
    }
    Fw_TraceText(s_682b84);
    CallVirt(self, 80, room);
    Fw_TraceText(s_682b74);
    F(BYTE*, F(BYTE*, self, 488), 76) = CallThis<BYTE*>(FUN_5ef390, room, module);
    if (!F(BYTE*, F(BYTE*, self, 488), 76))
        Sys_Fatal(s_682b58, 0);
    F(int, F(BYTE*, self, 484), 8) = -1;
    return room;
}

// ---------------------------------------------------------------------------
extern "C" void FUN_5e7770(); KING_ALIAS("_FUN_5e7770", "_Image_FromFile")    // image: load (name, 0, 0)
extern "C" void FUN_5d8350(); KING_ALIAS("_FUN_5d8350", "_Dd_BitDepth")
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")    // DirectDraw error text (hr, where)
KING_VAR(BYTE*, g_fwDevice, 720908)
KING_VAR(BYTE*, g_fwD3d, 72090c)
KING_VAR(AsmLabel, s_6834e8, 6834e8)
KING_VAR(AsmLabel, s_6834d4, 6834d4)
KING_VAR(AsmLabel, s_6834b4, 6834b4)

template <class R = long, class... A>
static R Fw_ComCall(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// whether the pixel at p (of the image's type: 1 bytes, 2 words, else 3-byte) is set
static bool Font_PixelSet(int type, BYTE* p)
{
    if (type == 1 || type != 2)
        return *p != 0;
    return *(WORD*)p != 0;
}

// [FUN_5e8e20..$L_5e92dc] loads a font from an image of 16x16 cells: per character the first and end columns of its top row (+16, +12) and 1-bit masks of the other rows (+28); the image itself becomes a surface (+36)
extern "C" BYTE* __cdecl Font_LoadImage(const char* name)
{
    BYTE* f = (BYTE*)operator new(40);
    if (f)
    {
        F(int, f, 8) = 0;
        F(int, f, 4) = 0;
        F(int, f, 28) = 0;
        F(int, f, 12) = 0;
        F(int, f, 0) = 0;
        F(int, f, 32) = -1;
        F(int, f, 16) = 0;
        F(int, f, 36) = 0;
    }
    else
        f = 0;
    F(int, f, 4) = 0;
    F(int, f, 8) = 255;
    BYTE* img = CallC<BYTE*>(FUN_5e7770, name, 0, 0);
    F(int, f, 24) = F(WORD, img, 20) >> 4;
    F(int, f, 32) = -1;
    F(int, f, 20) = (F(WORD, img, 22) >> 4) - 1;
    F(BYTE*, f, 12) = (BYTE*)operator new(256);
    F(BYTE*, f, 16) = (BYTE*)operator new(256);
    int rowBytes = (F(int, f, 24) - 1) / 8 + 1;
    int charBytes = rowBytes * F(int, f, 20);
    F(BYTE*, f, 28) = (BYTE*)operator new(charBytes << 8);
    int type = F(WORD, img, 24);
    int width = F(WORD, img, 20);
    int bpp = type == 1 ? 1 : type == 2 ? 2 : 3;
    for (int c = 0; c < 256; c++)
    {
        int cw = F(int, f, 24);
        BYTE* row = F(BYTE*, img, 8) + ((F(int, f, 20) + 1) * (c / 16) * width + c % 16 * cw) * bpp;
        int x = 0;
        while (x < cw && !Font_PixelSet(type, row + x * bpp))
            x++;
        int start = x;
        while (x < cw && Font_PixelSet(type, row + x * bpp))
            x++;
        F(BYTE*, f, 16)[c] = (BYTE)start;
        if (x)
            x++;
        F(BYTE*, f, 12)[c] = (BYTE)x;
        BYTE* bits = F(BYTE*, f, 28) + c * charBytes;
        for (int y = 0; y < F(int, f, 20); y++, bits += rowBytes)
        {
            row += width * bpp;
            for (int i = 0; i < F(int, f, 24); i++)
            {
                if (!(i % 8))
                    bits[i / 8] = 0;
                if (Font_PixelSet(type, row + i * bpp))
                    bits[i / 8] |= 128 >> (i % 8);
            }
        }
    }
    F(DWORD, f, 0) |= 1;
    if (g_fwDevice)
    {
        int depth = CallC<int>(FUN_5d8350);
        DWORD desc[31];
        memset(desc, 0, sizeof desc);
        desc[0] = 124;
        desc[1] = 7;
        desc[2] = F(WORD, img, 22);
        desc[3] = F(WORD, img, 20);
        desc[26] = 2048;
        BYTE* surf;
        long hr = Fw_ComCall(g_fwD3d, 24, desc, &surf, 0);
        if (hr)
            Sys_Fatal(CallC<const char*>(FUN_5d4c10, hr, (char*)s_6834b4), 0);
        else
        {
            if (!surf)
                Sys_Fatal(s_6834e8, 0);
            DWORD key[2] = {0, 0};
            Fw_ComCall(surf, 116, 8, key);
            DWORD lock[31];
            lock[0] = 124;
            lock[1] = 8387054;
            BYTE* dst;
            if (!Fw_ComCall(surf, 100, (void*)0, lock, 1, 0) && (dst = (BYTE*)lock[9]) != 0)
            {
                memcpy(dst, F(BYTE*, img, 8), (unsigned)F(WORD, img, 22) * F(WORD, img, 20) * depth);
                Fw_ComCall(surf, 128, 0);
            }
            else
                CallC(FUN_5d2ba0, (char*)s_6834d4, 0);
            F(BYTE*, f, 36) = surf;
        }
    }
    if (img)
        CallVirt(img, 0, 1);
    return f;
}

// ---------------------------------------------------------------------------
typedef void (__cdecl* FwHook2)(int, int);
KING_VAR(FwHook2, g_fw702f4c, 702f4c)
extern "C" void FUN_5e6de0(); KING_ALIAS("_FUN_5e6de0", "_Files_Open")    // opens a data file (name, mode)
extern "C" void FUN_5e6e90(); KING_ALIAS("_FUN_5e6e90", "_Palette_Load")
extern "C" void FUN_5e7930(); KING_ALIAS("_FUN_5e7930", "_Tga_ReadHeader")
extern "C" void FUN_5e9690(); KING_ALIAS("_FUN_5e9690", "_Mask_ReadPixels")
extern "C" void FUN_5e99c0(); KING_ALIAS("_FUN_5e99c0", "_Image_Read")
extern "C" void FUN_5e9360(); KING_ALIAS("_FUN_5e9360", "_Texture_ReadFile")
extern "C" void FUN_5e9910(); KING_ALIAS("_FUN_5e9910", "_Sound_ReadFile")
extern "C" void FUN_5e99a0(); KING_ALIAS("_FUN_5e99a0", "_Image_SetData")
KING_VAR(AsmLabel, g_fwPath72183c, 72183c)
extern "C" int __cdecl _stat(const char* path, void* buf);

// [FUN_5d8a50] clears the two low flags of +16 and calls the hook 702f4c (0, 0)
extern "C" void __fastcall Window_ClearFlags(BYTE* self, void*)
{
    F(DWORD, self, 16) &= ~3u;
    if (g_fw702f4c)
        g_fw702f4c(0, 0);
}

// [FUN_5d8bb0] asks the frame (+56) for flags of a; bit 15 set: passes the other bits back and returns 1
extern "C" int __fastcall Frame_Ask(BYTE* self, void*, int a)
{
    DWORD r = CallVirt<DWORD>(F(BYTE*, self, 56), 56, a);
    if (!(r & 0x8000))
        return 0;
    r &= ~0x8000u;
    if (r)
        CallVirt(F(BYTE*, self, 56), 60, r);
    return 1;
}

// opens a data file with mode 6703e8; 7218bc = 1 when it fails
static FILE* Fw_OpenData(const char* name)
{
    FILE* f = CallC<FILE*>(FUN_5e6de0, name, (char*)s_6703e8);
    if (!f)
        g_7218bc = 1;
    return f;
}

// the size of the file named by 72183c
static int Fw_DataSize()
{
    DWORD st[9];
    _stat(g_fwPath72183c, st);
    return (int)st[5];
}

// [FUN_5e6e50] reads a data file through 5e6e90
extern "C" void* __cdecl Data_Read5e6e90(const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e6e90, f);
    fclose(f);
    return r;
}

// [FUN_5e7730] loads an image from a data file
extern "C" void* __cdecl Image_LoadFile(const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e7770, f, 0, 0);
    fclose(f);
    return r;
}

// [FUN_5e78e0] reads a data file into a through 5e7930
extern "C" void* __cdecl Data_Read5e7930(void* a, const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e7930, a, f, 0);
    fclose(f);
    return r;
}

// [FUN_5e9480] reads a data file into a through 5e9690
extern "C" void* __cdecl Data_Read5e9690(void* a, const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e9690, a, f, 0);
    fclose(f);
    return r;
}

// [FUN_5e98c0] reads a data file into a through 5e99c0
extern "C" void* __cdecl Data_Read5e99c0(void* a, const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e99c0, a, f, 0);
    fclose(f);
    return r;
}

// [FUN_5e9300] reads a whole data file through 5e9360 (file, size)
extern "C" void* __cdecl Data_Read5e9360(const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e9360, f, Fw_DataSize());
    fclose(f);
    return r;
}

// [FUN_5e9800] reads a whole data file through 5e9910 (file, size)
extern "C" void* __cdecl Data_Read5e9910(const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e9910, f, Fw_DataSize());
    fclose(f);
    return r;
}

// [FUN_5e9860] reads a whole data file into a through 5e99a0 (a, file, size)
extern "C" void* __cdecl Data_Read5e99a0(void* a, const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e99a0, a, f, Fw_DataSize());
    fclose(f);
    return r;
}

// [FUN_5e1720] does nothing (one argument)
extern "C" void __fastcall Fw_Nop1(BYTE* self, void*, int) {}

// [FUN_5e48f0] does nothing
extern "C" void __cdecl Fw_Nop() {}


// ---------------------------------------------------------------------------
extern "C" void FUN_5e94d0(); KING_ALIAS("_FUN_5e94d0", "_Mask_ReadHeader")
extern "C" void FUN_5e8160(); KING_ALIAS("_FUN_5e8160", "_Tga_Read")

// [FUN_5e9420] reads a whole data file into a through 5e94d0 (a, file, size, 0)
extern "C" void* __cdecl Data_Read5e94d0(void* a, const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e94d0, a, f, Fw_DataSize(), 0);
    fclose(f);
    return r;
}

// [FUN_5e7ed0] reads a data file into a through 5e8160
extern "C" void* __cdecl Data_Read5e8160(void* a, const char* name)
{
    FILE* f = Fw_OpenData(name);
    if (!f)
        return 0;
    void* r = CallC<void*>(FUN_5e8160, a, f, 0);
    fclose(f);
    return r;
}

// [FUN_5dd720] resets the window's state fields
extern "C" void __fastcall Window_ResetState(BYTE* c, void*)
{
    const DWORD k9a = 2576980378u;
    F(DWORD, c, 3912) = 0;
    F(BYTE*, c, 472) = c + 2720;
    F(DWORD, c, 2864) = 0;
    F(DWORD, c, 552) = 0;
    F(DWORD, c, 2716) = 0;
    F(BYTE*, c, 3016) = c + 2968;
    F(DWORD, c, 2968) = 0;
    F(DWORD, c, 548) = 0;
    F(DWORD, c, 2972) = 0;
    F(DWORD, c, 2856) = 0;
    F(DWORD, c, 2860) = 0;
    F(DWORD, c, 632) = 0;
    F(DWORD, c, 3080) = 0;
    F(DWORD, c, 2880) = 0;
    F(DWORD, c, 2872) = 0;
    F(DWORD, c, 2888) = 0;
    F(DWORD, c, 592) = 0;
    F(DWORD, c, 2960) = k9a;
    F(DWORD, c, 2976) = k9a;
    F(DWORD, c, 600) = 0;
    F(DWORD, c, 2928) = 0;
    F(DWORD, c, 2936) = 0;
    F(DWORD, c, 2944) = 0;
    F(DWORD, c, 2904) = 0;
    F(DWORD, c, 2912) = 0;
    F(DWORD, c, 2920) = 0;
    F(DWORD, c, 3048) = k9a;
    F(DWORD, c, 3056) = 858993459;
    F(DWORD, c, 636) = 1073217536;
    F(DWORD, c, 2984) = 0;
    F(DWORD, c, 3084) = 1072693248;
    F(BYTE, c, 3090) = 0;
    F(BYTE, c, 3089) = 0;
    F(BYTE, c, 3088) = 0;
    F(DWORD, c, 3036) = 0;
    F(DWORD, c, 3024) = 0;
    F(DWORD, c, 3040) = 0;
    F(DWORD, c, 3028) = 1;
    F(DWORD, c, 3044) = 1;
    F(DWORD, c, 2884) = 0;
    F(DWORD, c, 2876) = 0;
    F(DWORD, c, 2892) = 0;
    F(DWORD, c, 596) = 0;
    F(DWORD, c, 2964) = 1069128089;
    F(DWORD, c, 2980) = 1069128089;
    F(DWORD, c, 604) = 0;
    F(DWORD, c, 3032) = 0;
    F(DWORD, c, 2932) = 0;
    F(DWORD, c, 2940) = 0;
    F(DWORD, c, 2948) = 0;
    F(DWORD, c, 2908) = 0;
    F(DWORD, c, 2916) = 0;
    F(DWORD, c, 2924) = 0;
    F(DWORD, c, 544) = 0xffffffff;
    F(DWORD, c, 3020) = 0;
    F(DWORD, c, 3052) = 1072273817;
    F(DWORD, c, 3060) = 1071854387;
    F(DWORD, c, 644) = 0;
    F(DWORD, c, 648) = 0;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5e4450(); KING_ALIAS("_FUN_5e4450", "@Worker_Stop@8")
extern "C" void FUN_5fcf10(); KING_ALIAS("_FUN_5fcf10", "_Sound_Stop")
extern "C" void FUN_5d2370(); KING_ALIAS("_FUN_5d2370", "@NameHash_Clear@8")
extern "C" void FUN_5fd250(); KING_ALIAS("_FUN_5fd250", "@SoundFile_Destroy@8")
extern "C" void FUN_5bbdd0(); KING_ALIAS("_FUN_5bbdd0", "@Bitmap_Destroy@8")
extern "C" void FUN_5e5420(); KING_ALIAS("_FUN_5e5420", "@Resources_Bind@16")
extern "C" void FUN_5e49d0(); KING_ALIAS("_FUN_5e49d0", "@Resource_Load@12")
extern "C" void FUN_5e4740(); KING_ALIAS("_FUN_5e4740", "@Resource_Free@8")
KING_VAR(BYTE*, g_fw721750, 721750)
KING_VAR(int, g_fw71e844, 71e844)
KING_VAR(int, g_fw7217a4, 7217a4)
extern "C" long __cdecl ftell(FILE* f);

// [FUN_5e5840] frees the six buffers of a record
extern "C" void __fastcall Rec5e5840_Free(BYTE* self, void*)
{
    static const int offs[6] = {24, 28, 32, 4, 16, 20};
    for (int i = 0; i < 6; i++)
        if (F(void*, self, offs[i]))
            operator delete(F(void*, self, offs[i]));
}

// [FUN_5da8a0] unloads the current room (+468): frees +484, and unless flag 8 stops the sound and deletes the room with its children
extern "C" void __fastcall Fw_UnloadRoom(BYTE* self, void*)
{
    if (!F(BYTE*, self, 468))
        return;
    BYTE* p = F(BYTE*, self, 484);
    if (p)
    {
        CallThis(FUN_5e4450, p);
        operator delete(p);
    }
    F(BYTE*, self, 484) = 0;
    if (F(BYTE, self, 16) & 8)
    {
        F(BYTE*, self, 468) = 0;
        return;
    }
    if (F(int, g_fw721750, 344))
        CallC(FUN_5fcf10);
    for (BYTE* c = F(BYTE*, F(BYTE*, self, 468), 8); c; )
    {
        BYTE* next = F(BYTE*, c, 8);
        CallVirt(c, 0, 1);
        c = next;
    }
    if (F(BYTE*, self, 468))
        CallVirt(F(BYTE*, self, 468), 0, 1);
    F(BYTE*, self, 468) = 0;
    if (g_fw71e844)
        CallThis(FUN_5d2370, (BYTE*)g_objectNames);
}

// [FUN_5e9910] reads a sound (28 bytes, its +4 bytes of data at +8) from a file; 0 on failure
extern "C" BYTE* __cdecl Sound_ReadFile(FILE* f, int size)
{
    long pos = ftell(f);
    BYTE* o = (BYTE*)operator new(28);
    if (o)
    {
        F(int, o, 8) = 0;
        F(int, o, 4) = 0;
        F(int, o, 0) = 0;
        F(int, o, 16) = 0;
        F(int, o, 24) = 0;
        F(int, o, 12) = -1;
    }
    if (CallC<int>(FUN_5e99a0, o, f, size))
    {
        F(void*, o, 8) = operator new(F(int, o, 4));
        if (CallC<int>(FUN_5e99c0, o, f, pos))
            return o;
    }
    if (o)
    {
        CallThis(FUN_5fd250, o);
        operator delete(o);
    }
    return 0;
}

// [FUN_5e9360] reads a texture (40 bytes, +12 bytes of data at +16) from a file; 0 on failure (7218bc = 2 without memory)
extern "C" BYTE* __cdecl Texture_ReadFile(FILE* f, int size)
{
    BYTE* o = (BYTE*)operator new(40);
    if (!o)
        return 0;
    F(int, o, 12) = 0;
    F(WORD, o, 8) = 0;
    F(WORD, o, 6) = 0;
    F(WORD, o, 4) = 0;
    F(int, o, 16) = 0;
    F(int, o, 0) = 0;
    F(int, o, 24) = 0;
    F(int, o, 28) = 0;
    F(int, o, 36) = 0;
    long pos = ftell(f);
    CallC(FUN_5e94d0, o, f, size, 0);
    F(void*, o, 16) = operator new(F(int, o, 12));
    if (!F(void*, o, 16))
    {
        CallThis(FUN_5bbdd0, o);
        operator delete(o);
        g_7218bc = 2;
        return 0;
    }
    if (!CallC<int>(FUN_5e9690, o, f, pos))
    {
        CallThis(FUN_5bbdd0, o);
        operator delete(o);
        return 0;
    }
    return o;
}

// [FUN_5e54c0] adds a resource record through 5e5420: kind 1 charges it (unless flagged), kind 2 of a type-1 record loads it (c) or counts its size; returns the kind, -1 without a record
extern "C" int __fastcall Resources_AddRecord(BYTE* self, void*, int a, BYTE* rec, int c)
{
    if (!rec)
        return -1;
    int r = CallThis<int>(FUN_5e5420, self, a, rec);
    if (r == -1)
        return -1;
    if (r == 1)
    {
        if (!(F(BYTE, rec, 8) & 1))
            CallThis(FUN_5e4900, rec, self);
        return r;
    }
    if (r == 2 && F(int, rec, 0) == 1)
    {
        if (c)
        {
            CallThis(FUN_5e49d0, rec, self);
            return r;
        }
        F(int, self, 2000) += F(int, rec, 16);
        if (F(unsigned, rec, 16))
        {
            g_fw7217a4--;
            CallC(FUN_5e48f0, F(int, self, 2000), rec, 2);
        }
    }
    return r;
}

// [FUN_5e4e00] frees the resource tables: 400 slots of 500 records and 100 lists
extern "C" void __fastcall Resources_FreeTables(BYTE* self, void*)
{
    BYTE** slots = (BYTE**)self;
    for (int i = 0; i < 400; i++)
    {
        if (!slots[i])
            continue;
        for (int j = 0; j < 500; j++)
        {
            BYTE* q = ((BYTE**)slots[i])[j];
            if (q)
            {
                CallThis(FUN_5e4740, q);
                operator delete(q);
            }
        }
        operator delete(slots[i]);
        slots[i] = 0;
    }
    BYTE** lists = (BYTE**)(self + 1600);
    for (int i = 0; i < 100; i++)
    {
        if (!lists[i])
            continue;
        BYTE* n;
        while ((n = F(BYTE*, lists[i], 8)) != 0)
        {
            CallThis(FUN_5d21b0, n);
            CallVirt(n, 0, 1);
        }
        if (lists[i])
            CallVirt(lists[i], 0, 1);
        lists[i] = 0;
    }
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5e8e20(); KING_ALIAS("_FUN_5e8e20", "_Font_LoadImage")
extern "C" void FUN_5e8d10(); KING_ALIAS("_FUN_5e8d10", "_Font_ReadFile")
KING_VAR(AsmLabel, s_6834ac, 6834ac)
extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" int __cdecl getc(FILE* f);

// [FUN_5e8c60] loads a font file: an image of cells for extension 6834ac, else the font format
extern "C" BYTE* __cdecl Font_LoadFile(const char* name)
{
    FILE* f = fopen(name, s_6703e8);
    if (!f)
    {
        g_7218bc = 1;
        return 0;
    }
    int n = strlen(name) - 1;
    if (n > 0)
        while (name[n] != '.' && --n > 0)
            ;
    if (!n)
        return 0;
    BYTE* r = !strcmp(name + n, s_6834ac) ? CallC<BYTE*>(FUN_5e8e20, f) : CallC<BYTE*>(FUN_5e8d10, f);
    fclose(f);
    return r;
}

// [FUN_5e8d10] reads a font file: the character count and first code, cell size, the widths (+12), and the 1-bit masks (+28)
extern "C" BYTE* __cdecl Font_ReadFile(FILE* f)
{
    BYTE* o = (BYTE*)operator new(40);
    if (o)
    {
        F(int, o, 8) = 0;
        F(int, o, 4) = 0;
        F(int, o, 28) = 0;
        F(int, o, 12) = 0;
        F(int, o, 0) = 0;
        F(int, o, 32) = -1;
        F(int, o, 16) = 0;
        F(int, o, 36) = 0;
    }
    fseek(f, 16, 0);
    int n = getc(f);
    int first = getc(f) + 32;
    F(int, o, 4) = first;
    F(int, o, 8) = first + n - 1;
    getc(f);
    F(int, o, 24) = getc(f);
    F(int, o, 20) = getc(f);
    F(int, o, 32) = -1;
    fseek(f, 250, 0);
    F(void*, o, 12) = operator new(n);
    void* p = operator new(n);
    F(void*, o, 16) = p;
    memset(p, 0, n);
    fread(F(void*, o, 12), n, 1, f);
    if (*F(BYTE*, o, 12))
        F(DWORD, o, 0) |= 1;
    fseek(f, 344, 0);
    int size = ((F(int, o, 24) - 1) / 8 + 1) * F(int, o, 20) * n;
    void* m = operator new(size);
    F(void*, o, 28) = m;
    fread(m, size, 1, f);
    return o;
}


// ---------------------------------------------------------------------------
// Small virtual methods of the framework's windows and their static
// initializers.

extern "C" void FUN_5de5b0(); KING_ALIAS("_FUN_5de5b0", "@Clock_Start@8")
extern "C" void FUN_5de5d0(); KING_ALIAS("_FUN_5de5d0", "@Clock_Stop@8")
extern "C" void FUN_5da8a0(); KING_ALIAS("_FUN_5da8a0", "@Fw_UnloadRoom@8")
extern "C" void FUN_5d8850(); KING_ALIAS("_FUN_5d8850", "@Window_Destroy@4")
extern "C" void FUN_5d8b30(); KING_ALIAS("_FUN_5d8b30", "@Fw5d8b30_Destroy@4")
extern "C" void FUN_5da790(); KING_ALIAS("_FUN_5da790", "@RoomWindow_Destroy@4")
extern "C" void FUN_5ddb40(); KING_ALIAS("_FUN_5ddb40", "@MainView_Destroy@4")
ASM_PROC(FUN_5dd2d0)
extern "C" void FUN_5b6700(); KING_ALIAS("_FUN_5b6700", "@Window_RootSurface@8")
extern "C" int __cdecl fprintf(FILE*, const char*, ...);
extern "C" void FUN_5ba3d0(); KING_ALIAS("_FUN_5ba3d0", "@Palette_Apply@8")
extern "C" void FUN_5c06a0(); KING_ALIAS("_FUN_5c06a0", "@Thunk5c06a0@4")
extern "C" void FUN_5e2e80(); KING_ALIAS("_FUN_5e2e80", "@GdiObject5e2e80_Destroy@4")
extern "C" void FUN_5e2ee0(); KING_ALIAS("_FUN_5e2ee0", "@GdiObject5e2ee0_Destroy@4")
extern "C" void FUN_5e2f40(); KING_ALIAS("_FUN_5e2f40", "@Fw5e2f40_Destroy@4")
extern "C" void FUN_5e4810(); KING_ALIAS("_FUN_5e4810", "@NamedTable_Destroy@8")
extern "C" void FUN_5daa30(); KING_ALIAS("_FUN_5daa30", "@Fw_MakeCluster@36")
extern "C" void FUN_5eb310(); KING_ALIAS("_FUN_5eb310", "@Camera_Draw@28")
extern "C" void FUN_5dbe60(); KING_ALIAS("_FUN_5dbe60", "@Messages_Take@8")
extern "C" void FUN_5e05c0(); KING_ALIAS("_FUN_5e05c0", "@Dbg_OnInput@12")
extern "C" void FUN_5d3ca0(); KING_ALIAS("_FUN_5d3ca0", "_Video_CreatePalette")
KING_VAR(AsmLabel, s_fw651ee8, 651ee8)
KING_VAR(AsmLabel, s_fw652130, 652130)
KING_VAR(AsmLabel, s_fw671204, 671204)
KING_VAR(AsmLabel, g_fw720938, 720938)
KING_VAR(int, g_fw683b54, 683b54)
KING_VAR(HDC, g_fw72091c, 72091c)
KING_VAR(HWND, g_fw720920, 720920)

// [$L_5d9ed0] the clock at +20984
extern "C" void __fastcall Fw_ClockStart(BYTE* s) { CallThis(FUN_5de5b0, F(BYTE*, s, 20984)); }
// [$L_5d9ee0]
extern "C" void __fastcall Fw_ClockStop(BYTE* s) { CallThis(FUN_5de5d0, F(BYTE*, s, 20984)); }

// [$L_5da170]
extern "C" int __fastcall Fw_Zero4(BYTE*, void*, int) { return 0; }
// [$L_5dd6f0]
extern "C" int __fastcall Fw_One4(BYTE*, void*, int) { return 1; }
// [$L_5dd710] class names
extern "C" const char* __fastcall Fw_Name5dd710(BYTE*) { return s_fw651ee8; }
// [$L_5e2930]
extern "C" const char* __fastcall Fw_Name5e2930(BYTE*) { return s_fw652130; }

// [$L_5e1730] fields of the application
extern "C" int __cdecl Fw_App256() { return F(int, g_fw721750, 256); }
// [$L_5e1740]
extern "C" int __cdecl Fw_App260() { return F(int, g_fw721750, 260); }

// [$L_5dd090] unloads the room and clears +488
extern "C" void __fastcall Fw_Unload(BYTE* s)
{
    CallThis(FUN_5da8a0, s);
    F(int, s, 488) = 0;
}

// [$L_5de060] passes a to the vtbl+160 of the object at +300
extern "C" int __fastcall Fw_5de060(BYTE* s, void*, int a) { return CallVirt<int>(F(BYTE*, s, 300), 160, a); }

// [$L_5dd070] vtbl+188 ("...", a, 0)
extern "C" int __fastcall Fw_5dd070(BYTE* s, void*, int a) { return CallVirt<int>(s, 188, (const char*)s_fw671204, a, 0); }

// [$L_5dd2e0] the application's vtbl+184 (a, b), 1
extern "C" int __fastcall Fw_5dd2e0(BYTE*, void*, int a, int b)
{
    CallVirt(g_fw721750, 184, a, b);
    return 1;
}

// [$L_5d9ea0] 1 when the object at +20976 has +520 or +8244 is set
extern "C" int __fastcall Fw_5d9ea0(BYTE* s)
{
    return F(int, F(BYTE*, s, 20976), 520) || F(int, s, 8244) ? 1 : 0;
}

// [$L_5dd530] applies the palette of the application's window root surface
extern "C" void __cdecl Fw_ApplyPalette()
{
    BYTE* w = F(BYTE*, g_fw721750, 252);
    if (!w)
        return;
    BYTE* r = CallThis<BYTE*>(FUN_5b6700, F(BYTE*, w, 12));
    if (r)
        CallThis(FUN_5ba3d0, r);
}

// [$L_5dc470] forwards 7 arguments
extern "C" int __fastcall Fw_5dc470(BYTE* s, void*, DWORD a, DWORD b, DWORD c, DWORD d, DWORD e, DWORD f, DWORD g)
{
    return CallThis<int>(FUN_5daa30, s, a, b, c, d, e, f, g);
}

// [$L_5dff90] draws the camera (+468) when +524 is set
extern "C" void __fastcall Fw_DrawCamera(BYTE* s)
{
    int v = F(int, s, 524);
    if (v)
        CallThis(FUN_5eb310, F(BYTE*, s, 468), F(int, s, 488), v, F(int, s, 472), 1, F(int, s, 464));
}

// [$L_5df670] hands each waiting message to vtbl+72
extern "C" void __fastcall Fw_DispatchMessages(BYTE* s)
{
    for (int m = CallThis<int>(FUN_5dbe60, s); m; m = CallThis<int>(FUN_5dbe60, s))
        CallVirt(s, 72, m);
}

// [$L_5dd300] 0 only for windowed (+268 bit 10) without +300 and a == 0
extern "C" int __fastcall Fw_5dd300(BYTE*, void*, int a)
{
    BYTE* app = g_fw721750;
    if ((F(int, app, 268) & 0x400) && !F(int, app, 300) && !a)
        return 0;
    return 1;
}

// [$L_5dbb70] keys 116-118 go to FUN_5e05c0, 120 toggles 683b54, the rest to vtbl+52
extern "C" int __fastcall Fw_OnKey(BYTE* s, void*, BYTE* ev)
{
    int k = F(short, ev, 6);
    if (k >= 116) {
        if (k <= 118)
            return CallThis<int>(FUN_5e05c0, s, ev);
        if (k == 120) {
            g_fw683b54 ^= 1;
            return k;
        }
    }
    return CallVirt<int>(s, 52, ev);
}

// [$L_5dd3f0] the window's DC gets the palette, a 20 ms timer starts (id to +188)
extern "C" int __fastcall Fw_OnCreate(BYTE* s, void*, int)
{
    HDC dc = GetDC(F(HWND, s, 28));
    g_fw72091c = dc;
    CallC(FUN_5d3ca0, dc);
    g_fw720920 = F(HWND, s, 28);
    F(UINT_PTR, s, 188) = SetTimer(F(HWND, s, 28), 0, 20, 0);
    return 1;
}

// [$L_5dbe90] static initializers
extern "C" void __cdecl Fw_Init721298()
{
    (&g_721298)[0] = 0;
    (&g_721298)[1] = 0;
    (&g_721298)[2] = 0;
}
// [$L_5dc4a0]
extern "C" void __cdecl Fw_Init720938()
{
    DWORD* d = (DWORD*)g_fw720938;
    d[0] = d[1] = d[2] = d[3] = 0;
}

static BYTE* Fw_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5d86f0]
extern "C" BYTE* __fastcall Fw5d86f0_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5d8850); }
// [$L_5d8ad0]
extern "C" BYTE* __fastcall Fw5d8ad0_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5d8b30); }
// [$L_5d9e20]
extern "C" BYTE* __fastcall Fw5d9e20_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5da790); }
// [$L_5dd0b0]
extern "C" BYTE* __fastcall Fw5dd0b0_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5ddb40); }
// [$L_5dd2b0]
extern "C" BYTE* __fastcall Fw5dd2b0_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5dd2d0); }
// [$L_5de080]
extern "C" BYTE* __fastcall Fw5de080_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5c06a0); }
// [$L_5e2e60]
extern "C" BYTE* __fastcall Fw5e2e60_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5e2e80); }
// [$L_5e2ec0]
extern "C" BYTE* __fastcall Fw5e2ec0_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5e2ee0); }
// [$L_5e2f20]
extern "C" BYTE* __fastcall Fw5e2f20_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5e2f40); }
// [$L_5e47f0]
extern "C" BYTE* __fastcall Fw5e47f0_DeletingDestructor(BYTE* s, void*, int flags) { return Fw_DeleteAfter(s, flags, FUN_5e4810); }

// ---------------------------------------------------------------------------
// More window methods: camera drawing, key commands, window sizing.

extern "C" void FUN_5eb280(); KING_ALIAS("_FUN_5eb280", "@Camera_Render@24")
extern "C" void FUN_5b39a0(); KING_ALIAS("_FUN_5b39a0", "@Rn_Chain5af000@20")
extern "C" void FUN_5e4250(); KING_ALIAS("_FUN_5e4250", "@Windows_SetActive@12")
extern "C" void FUN_5d3f70(); KING_ALIAS("_FUN_5d3f70", "_Video_Get67fcf0")
extern "C" void FUN_5d3f80(); KING_ALIAS("_FUN_5d3f80", "_Video_Get67fcf4")
KING_VAR(AsmLabel, g_fra7025f0, 7025f0)

typedef void (*AsmFn)();
extern "C" void FUN_5bae30(); KING_ALIAS("_FUN_5bae30", "@Texture_Create@12")

// [$L_5df6a0] the room's camera (+76 of +488) gets the matrix 7025f0, then the scene is drawn
extern "C" void __fastcall Fw_DrawRoom(BYTE* s)
{
    BYTE* c = F(BYTE*, F(BYTE*, s, 488), 76);
    if (c)
        CallVirt(c, 56, 4, (BYTE*)g_fra7025f0, (BYTE*)g_fra7025f0, F(int, s, 464));
    CallThis(FUN_5eb280, F(BYTE*, s, 468), F(int, s, 488), F(int, s, 472), 1, F(int, s, 464));
}

// [$L_5d9e60] FUN_5b39a0 (a, b, c), then the hook (+520 of +20976) gets (self, a, result, c)
extern "C" int __fastcall Fw_5d9e60(BYTE* s, void*, int a, int b, int c)
{
    int r = CallThis<int>(FUN_5b39a0, s, a, b, c);
    AsmFn hook = F(AsmFn, F(BYTE*, s, 20976), 520);
    if (hook)
        return CallC<int>(hook, s, a, r, c);
    return r;
}

// [$L_5dae40] after FUN_5b6bf0 of +12 the sound (+92 of +468) restarts when the application wants it
extern "C" void __fastcall Fw_5dae40(BYTE* s)
{
    CallThis(FUN_5b6bf0, F(BYTE*, s, 12));
    BYTE* o = F(BYTE*, s, 468);
    if (!o || !F(int, o, 88))
        return;
    if (!(F(int, g_fw721750, 268) & 0x800) || !F(int, g_fw721750, 336))
        return;
    CallThis(FUN_5bae30, *F(BYTE**, o, 92), 0);
}

// [$L_5e1940] commands 3 and 4 set +3024 / +3036; they and 27 close the dialog (+2856)
extern "C" int __fastcall Fw_5e1940(BYTE* s, void*, int cmd)
{
    if (cmd == 3)
        F(int, s, 3024) = 1;
    else if (cmd == 4)
        F(int, s, 3036) = 1;
    else if (cmd != 27)
        return 0;
    if (F(BYTE*, s, 2856)) {
        CallVirt(F(BYTE*, s, 2856), 32);
        CallThis(FUN_5e4250, g_fw721750, s);
    }
    return 1;
}

// [$L_5dd5d0] a window size (type 0 or 2) in a window that may be resized: the screen size in units
extern "C" void __fastcall Fw_OnSize(BYTE*, void*, int type, int cx, int cy)
{
    BYTE* app = g_fw721750;
    if (F(int, app, 268) & 0x400)
        return;
    if (F(BYTE, app, 292) & 1)
        return;
    if (type != 0 && type != 2)
        return;
    int w = cx / CallC<int>(FUN_5d3f70);
    int h = cy / CallC<int>(FUN_5d3f80);
    w = (w + 1) & ~3;
    app = g_fw721750;
    if (F(BYTE, app, 292) & 16)
        h = F(int, app, 260) * w / F(int, app, 256);
    F(int, app, 284) = w;
    F(int, g_fw721750, 288) = h;
}

// ---------------------------------------------------------------------------
// Destructors (their exception frames dropped: exceptions are always fatal).

extern "C" void Fw_GdiDelete(); KING_ALIAS("_Fw_GdiDelete", "@GdiObject_DeleteObject@4")
extern "C" void Fw_FrameWndDtor(); KING_ALIAS("_Fw_FrameWndDtor", "@FrameWnd_Destruct@4")
extern "C" void FUN_5bda90(); KING_ALIAS("_FUN_5bda90", "@ConGroup_Destroy@4")
KING_VAR(AsmLabel, g_fra654420, 654420)
KING_VAR(AsmLabel, g_fra652238, 652238)
KING_VAR(AsmLabel, g_fra652570, 652570)
KING_VAR(AsmLabel, g_fra6521f0, 6521f0)
KING_VAR(AsmLabel, g_fra6521b0, 6521b0)

// [FUN_5e2e80] a GDI object (vtable 654420) deletes its handle
extern "C" void __fastcall GdiObject5e2e80_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra654420;
    CallThis(Fw_GdiDelete, s);
}
// [FUN_5e2ee0]
extern "C" void __fastcall GdiObject5e2ee0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra654420;
    CallThis(Fw_GdiDelete, s);
}

// [FUN_5d8b30] (vtable 652238) deletes its child (+56)
extern "C" void __fastcall Fw5d8b30_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra652238;
    if (F(BYTE*, s, 56))
        CallVirt(F(BYTE*, s, 56), 0, 1);
    CallThis(FUN_5d8850, s);
}

// [FUN_5e2f40] a frame window (vtable 652570) deletes its child (+188, vtbl+4)
extern "C" void __fastcall Fw5e2f40_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra652570;
    if (F(BYTE*, s, 188))
        CallVirt(F(BYTE*, s, 188), 4, 1);
    CallThis(Fw_FrameWndDtor, s);
}

// [FUN_5d89e0] (vtables 6521f0, 6521b0 at +64)
extern "C" void __fastcall Fw5d89e0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra6521f0;
    F(void*, s, 64) = g_fra6521b0;
    F(int, s, 56) = 0;
    CallThis(FUN_5bda90, s ? s + 64 : 0);
    CallThis(FUN_5d8b30, s);
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_5b6a90(); KING_ALIAS("_FUN_5b6a90", "@Window_FreeBuffers@8")
extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0", "_Fw_Nop")
KING_VAR(AsmLabel, g_fra652168, 652168)
KING_VAR(AsmLabel, g_fra64d81c, 64d81c)
KING_VAR(AsmLabel, g_fra65229c, 65229c)
extern "C" void Fw_VecDtor(); KING_ALIAS("_Fw_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")

// [FUN_5d8850] a window (vtable 652168): its surface (+12) and its buffer (+48)
extern "C" void __fastcall Window_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra652168;
    BYTE* p = F(BYTE*, s, 12);
    if (p) {
        CallThis(FUN_5b6a90, p);
        operator delete(p);
    }
    operator delete(F(void*, s, 48));
    F(void*, s, 0) = g_fra64d81c;
}

// [FUN_5da790] a room window (vtable 65229c): its room data (+464, with 20 80-byte items at +6224) and the room
extern "C" void __fastcall RoomWindow_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra65229c;
    BYTE* o = F(BYTE*, s, 464);
    if (o) {
        CallThis(FUN_5e48f0, o + 7824);
        CallStd(Fw_VecDtor, o + 6224, 80, 20, FUN_5e48f0);
        operator delete(o);
    }
    if (F(int, s, 468) && !(F(BYTE, s, 16) & 8))
        CallThis(FUN_5da8a0, s);
    CallThis(FUN_5d8850, s);
}

// ---------------------------------------------------------------------------
// Constructors and window resizing (exception frames dropped).

extern "C" void FUN_5e5840(); KING_ALIAS("_FUN_5e5840", "@Rec5e5840_Free@8")
extern "C" void FUN_5da180(); KING_ALIAS("_FUN_5da180", "@Fw_RemakeSurface@24")
extern "C" void FUN_5ed970(); KING_ALIAS("_FUN_5ed970", "@Renderer_Construct@12")
extern "C" void FUN_5b6530(); KING_ALIAS("_FUN_5b6530", "@SubSurface_Construct@20")
extern "C" void FUN_5d8710(); KING_ALIAS("_FUN_5d8710", "@Window_ConstructSub@20")
KING_VAR(AsmLabel, g_fra652280, 652280)

// [$L_5e57a0] frees its buffers (+8, +12..+24), then the table at +52
extern "C" void __fastcall Fw5e57a0_Destroy(BYTE* s)
{
    operator delete(F(void*, s, 8));
    for (int o = 12; o <= 24; o += 4)
        if (F(void*, s, o))
            operator delete(F(void*, s, o));
    CallThis(FUN_5e5840, s + 52);
}

// deletes the room data (+464): its 20 80-byte items at +6224 and the one at +7824
static void Fw_DeleteRoomData(BYTE* s)
{
    BYTE* o = F(BYTE*, s, 464);
    if (!o)
        return;
    CallThis(FUN_5e48f0, o + 7824);
    CallStd(Fw_VecDtor, o + 6224, 80, 20, FUN_5e48f0);
    operator delete(o);
}
// new room data (20976 bytes) for the window's surface (+12)
static BYTE* Fw_NewRoomData(BYTE* s, unsigned size)
{
    BYTE* m = (BYTE*)operator new(size);
    return m ? CallThis<BYTE*>(FUN_5ed970, m, F(int, s, 12)) : 0;
}

// [$L_5da5f0] a new size (FUN_5da180) with new room data
extern "C" void __fastcall Fw_Resize(BYTE* s, void*, int a, int b, int c, int d)
{
    CallThis(FUN_5da180, s, a, b, c, d);
    Fw_DeleteRoomData(s);
    F(BYTE*, s, 464) = Fw_NewRoomData(s, 20976);
}
// [$L_5dba80] the same with the 20992-byte room data of the main view (vtable 652280, owner at +20976, +20984)
extern "C" void __fastcall Fw_ResizeMain(BYTE* s, void*, int a, int b, int c, int d)
{
    CallThis(FUN_5da180, s, a, b, c, d);
    Fw_DeleteRoomData(s);
    BYTE* m = (BYTE*)operator new(20992);
    if (!m) {
        F(int, s, 464) = 0;
        return;
    }
    CallThis(FUN_5ed970, m, F(int, s, 12));
    F(BYTE*, m, 20976) = s;
    F(void*, m, 0) = g_fra652280;
    F(BYTE*, m, 20984) = s;
    F(BYTE*, s, 464) = m;
}

// [FUN_5da360] a new surface (124 bytes, FUN_5b6530) of the rectangle (a..d) unless it is the main window
extern "C" void __fastcall Fw_NewSurface(BYTE* s, void*, int a, int b, int c, int d)
{
    BYTE* main = F(BYTE*, g_fw721750, 252);
    if (main == s)
        return;
    BYTE* old = F(BYTE*, s, 12);
    int flags = F(int, old, 12);
    if (old) {
        CallThis(FUN_5b6a90, old);
        operator delete(old);
    }
    BYTE* m = (BYTE*)operator new(124);
    F(BYTE*, s, 12) = m ? CallThis<BYTE*>(FUN_5b6530, m, F(int, main, 12), &a, flags & 0x4000) : 0;
    CallVirt(s, 60);
}

// [$L_5da6b0] a new surface and new room data, keeping the room data's +8452
extern "C" void __fastcall Fw_NewSurfaceRoom(BYTE* s, void*, int a, int b, int c, int d)
{
    Fw_NewSurface(s, 0, a, b, c, d);
    BYTE* o = F(BYTE*, s, 464);
    int keep = F(int, o, 8452);
    Fw_DeleteRoomData(s);
    F(BYTE*, s, 464) = Fw_NewRoomData(s, 20976);
    F(int, F(BYTE*, s, 464), 8452) = keep;
}

// [FUN_5d9f60] constructor (a, b, c, d) of a room window (vtable 65229c)
extern "C" BYTE* __fastcall RoomWindow_Construct(BYTE* s, void*, int a, int b, int c, int d)
{
    CallThis(FUN_5d8710, s, a, b, F(int, g_fw721750, 268) & 0x4000);
    F(void*, s, 0) = g_fra65229c;
    F(int, s, 520) = 0;
    F(BYTE*, s, 464) = Fw_NewRoomData(s, 20976);
    F(int, s, 472) = d;
    F(int, s, 16) |= 3;
    static const int zero[] = {512, 496, 504, 468, 536, 532, 528, 524, 456, 460, 480, 476, 484, 516, 500, 508};
    for (int o : zero)
        F(int, s, o) = 0;
    F(int, s, 488) = c;
    return s;
}

// ---------------------------------------------------------------------------
// Constructors, destructors and messages (exception frames dropped).

extern "C" void FUN_5d8af0(); KING_ALIAS("_FUN_5d8af0", "@FrameWindow_Construct@28")
extern "C" void FUN_5bfde0(); KING_ALIAS("_FUN_5bfde0", "@Panel_ConstructRect@44")
extern "C" void FUN_5e44a0(); KING_ALIAS("_FUN_5e44a0", "@ResRef_Release@8")
extern "C" void FUN_5e0150(); KING_ALIAS("_FUN_5e0150", "@Fw_Close5e0150@8")
extern "C" void FUN_5bc2f0(); KING_ALIAS("_FUN_5bc2f0", "@Poly_Destroy@8")
extern "C" void FUN_5b9520(); KING_ALIAS("_FUN_5b9520", "@TexSets_Free@4")
extern "C" void FUN_5d3560(); KING_ALIAS("_FUN_5d3560", "@Owner_Clear@8")
extern "C" void FUN_5e4eb0(); KING_ALIAS("_FUN_5e4eb0", "@Resources_FreeTables@8")
KING_VAR(AsmLabel, g_fra68d1c0, 68d1c0)
KING_VAR(AsmLabel, g_fra652310, 652310)

// [FUN_5d8900] constructor (a1, rect a2..a5, a6, a7, a8): a window with its control group (+64, vtable 6521b0)
extern "C" BYTE* __fastcall ControlWindow_Construct(BYTE* s, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    CallThis(FUN_5d8af0, s, a1, a2, a3, a4, a5);
    BYTE* g = s + 64;
    CallThis(FUN_5bfde0, g, (BYTE*)g_fra68d1c0, a6, a7, 4, a2, a3, a4, a5, a8);
    F(void*, s, 0) = g_fra6521f0;
    F(void*, g, 0) = g_fra6521b0;
    F(BYTE*, s, 56) = s ? g : 0;
    F(int, F(BYTE*, s, 56), 44) = 0;
    F(int, F(BYTE*, s, 56), 48) = 0;
    F(int, F(BYTE*, s, 56), 52) = F(WORD, F(BYTE*, s, 12), 0);
    F(int, F(BYTE*, s, 56), 56) = F(WORD, F(BYTE*, s, 12), 2);
    return s;
}

// [FUN_5d9cf0] constructor (a, b, view): the main room window with the view's room data (20992 bytes)
extern "C" BYTE* __fastcall MainRoomWindow_Construct(BYTE* s, void*, int a, int b, BYTE* view)
{
    CallThis(FUN_5d8710, s, a, b, F(int, g_fw721750, 268) & 0x4000);
    F(void*, s, 0) = g_fra65229c;
    F(int, s, 484) = 0;
    F(int, s, 520) = 0;
    BYTE* m = (BYTE*)operator new(20992);
    if (m) {
        CallThis(FUN_5ed970, m, F(int, s, 12));
        F(BYTE*, m, 20976) = view;
        F(void*, m, 0) = g_fra652280;
        F(BYTE*, m, 20984) = view;
    }
    F(int, s, 16) |= 3;
    static const int zero[] = {512, 496, 504, 468, 488, 536, 532, 528, 524, 456, 460, 480, 476, 516, 500, 508};
    for (int o : zero)
        F(int, s, o) = 0;
    F(BYTE*, s, 464) = m;
    F(BYTE*, s, 472) = view + 2720;
    return s;
}

// [FUN_5ddb40] the main view (vtable 652310): waits for its loader thread (+484) to end, then its parts
extern "C" void __fastcall MainView_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_fra652310;
    BYTE* t = F(BYTE*, s, 484);
    if (t && F(int, t, 20)) {
        while (!F(int, F(BYTE*, s, 484), 16))
            Sleep(10);
        CallThis(FUN_5e44a0, F(BYTE*, s, 484));
    }
    CallThis(FUN_5e0150, s);
    if (F(BYTE*, s, 3912))
        CallVirt(F(BYTE*, s, 3912), 0, 1);
    if (F(void*, s, 640))
        operator delete(F(void*, s, 640));
    BYTE* p = F(BYTE*, s, 2864);
    if (p) {
        CallThis(FUN_5bc2f0, p);
        operator delete(p);
    }
    CallThis(FUN_5b9520, s + 3096);
    CallThis(FUN_5d3560, s + 2992);
    F(void*, s, 2720) = g_fra64d81c;
    CallThis(FUN_5e4eb0, s + 704);
    CallThis(FUN_5da790, s);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

extern "C" void FUN_5f25c0(); KING_ALIAS("_FUN_5f25c0", "@Node_GetWorld@12")
extern "C" void FUN_5da840(); KING_ALIAS("_FUN_5da840", "@Fw_EnsureHelper484@8")
extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
extern "C" void FUN_5ef6d0(); KING_ALIAS("_FUN_5ef6d0", "@Node_Construct@16")
extern "C" void FUN_5d9f10(); KING_ALIAS("_FUN_5d9f10", "@Scene_FindSpecial@8")
KING_VAR(AsmLabel, s_fra682f40, 682f40)
KING_VAR(AsmLabel, g_fra64c7dc, 64c7dc)

// [$L_5e01b0] the room is set up: its camera (+616) and matrix, the sky texture holder (+532)
extern "C" void __fastcall Fw_SetupRoom(BYTE* s)
{
    F(int, s, 488) = F(int, F(BYTE*, s, 468), 120);
    BYTE* r = F(BYTE*, s, 488);
    F(int, s, 616) = r ? F(int, r, 80) : 0;
    if (F(BYTE*, s, 616))
        CallThis(FUN_5f25c0, F(BYTE*, s, 616), s + 652);
    if (F(BYTE*, s, 488))
        F(int, s, 700) = F(int, F(BYTE*, s, 488), 76);
    if (!F(int, s, 700))
        CallThis(FUN_5da840, s);
    if (F(BYTE*, s, 532))
        CallVirt(F(BYTE*, s, 532), 0, 1);
    F(BYTE*, s, 532) = CallC<BYTE*>(FUN_5ef2b0, 0, (const char*)s_fra682f40);
    if (F(BYTE*, s, 532)) {
        BYTE* o = (BYTE*)operator new(88);
        if (o) {
            CallThis(FUN_5ef6d0, o, 0, 17);
            for (int k = 48; k <= 80; k += 4)
                F(int, o, k) = 0;
            F(void*, o, 0) = g_fra64c7dc;
            F(int, o, 84) = -1;
        }
        F(int, o, 68) = F(int, s, 616);
        F(int, o, 64) = F(int, s, 532);
        F(BYTE*, s, 532) = o;
    }
    CallThis(FUN_5d9f10, s);
    CallVirt(s, 140);
    F(int, s, 3040) = 1;
}

// ---------------------------------------------------------------------------
// Constructors and surfaces (exception frames dropped).

extern "C" void FUN_5b6a90(); KING_ALIAS("_FUN_5b6a90", "@Window_FreeBuffers@8")
extern "C" void FUN_5b6a50(); KING_ALIAS("_FUN_5b6a50", "@Window_ConstructChild@24")
extern "C" void FUN_5b6e00(); KING_ALIAS("_FUN_5b6e00", "@Surface_Clear@16")
extern "C" void FUN_5b6530(); KING_ALIAS("_FUN_5b6530", "@SubSurface_Construct@20")
KING_VAR(BYTE*, g_q20_721750, 721750)

// [FUN_5da180] (x, y, w, h): the window's surface (+12) is made again; the main window gets a new one of that
// size, a child one the part of its parent's (anchored by flags +16 bits 16/32, 64/128)
extern "C" void __fastcall Fw_RemakeSurface(BYTE* s, void*, int a, int b, int c, int d)
{
    BYTE* p = F(BYTE*, g_q20_721750, 252);
    if (p == s) {
        BYTE* o = F(BYTE*, s, 12);
        if (o) {
            CallThis(FUN_5b6a90, o);
            operator delete(o);
        }
        BYTE* m = (BYTE*)operator new(124);
        BYTE* r = m ? CallThis<BYTE*>(FUN_5b6a50, m, a, b, c, d) : 0;
        F(BYTE*, s, 12) = r;
        CallThis(FUN_5b6e00, r, 0, 0);
        return;
    }
    int l[4];
    memcpy(l, F(BYTE*, s, 12) + 100, 16);
    int fl = F(int, s, 16);
    if (fl & 16) {
        if (fl & 32) {
            l[0] = 0;
            l[2] = F(WORD, F(BYTE*, p, 12), 0);
        } else {
            l[2] -= l[0];
            l[0] = 0;
        }
    } else if (l[0] >= CallVirt<int>(p, 20)) {
        int dd = l[2] - l[0];
        l[2] = CallVirt<int>(p, 20);
        l[0] = l[2] - dd;
        if (l[0] < 0)
            l[0] = 0;
    }
    fl = F(int, s, 16);
    if (fl & 64) {
        if (fl & 128) {
            l[1] = 0;
            l[3] = F(WORD, F(BYTE*, p, 12), 2);
        } else {
            l[3] -= l[1];
            l[1] = 0;
        }
    } else if (l[1] >= CallVirt<int>(p, 24)) {
        int dd = l[3] - l[1];
        l[3] = CallVirt<int>(p, 24);
        l[1] = l[3] - dd;
        if (l[1] < 0)
            l[1] = 0;
    }
    BYTE* o = F(BYTE*, s, 12);
    if (o) {
        CallThis(FUN_5b6a90, o);
        operator delete(o);
    }
    BYTE* m = (BYTE*)operator new(124);
    F(BYTE*, s, 12) = m ? CallThis<BYTE*>(FUN_5b6530, m, F(BYTE*, p, 12), l, d & 16384) : 0;
}

// ---------------------------------------------------------------------------
// Trail effect constructor, image from file (exception frames dropped).

extern "C" void FUN_5ba710(); KING_ALIAS("_FUN_5ba710", "@Texture_Construct@12")
extern "C" void FUN_5e7930(); KING_ALIAS("_FUN_5e7930", "_Tga_ReadHeader")
extern "C" void FUN_5e8160(); KING_ALIAS("_FUN_5e8160", "_Tga_Read")
KING_VAR(int, g_q26_720904, 720904)
KING_VAR(int, g_q26_7218bc, 7218bc)

// [FUN_5e7770] (file, name, flags): an image (84 bytes) read from the open file; a streamed one (flag 0x400 with
// streaming on) keeps the file and position; otherwise its pixels are read now. 0 and 7218bc = 2 on failure
extern "C" BYTE* __cdecl Image_FromFile(FILE* f, int name, int flags)
{
    BYTE* m = (BYTE*)operator new(84);
    BYTE* o = m ? CallThis<BYTE*>(FUN_5ba710, m, name) : 0;
    F(int, o, 52) |= flags;
    if (!o)
        return 0;
    long pos = ftell(f);
    CallC(FUN_5e7930, o, f, 0);
    int fl = F(int, o, 52);
    if ((fl & 0x400) && g_q26_720904) {
        F(int, o, 32) = pos;
        F(FILE*, o, 48) = f;
        F(int, o, 52) = (fl & ~3) | flags;
        return o;
    }
    int size = F(WORD, o, 24) * F(WORD, o, 20) * F(WORD, o, 22);
    F(void*, o, 8) = operator new(size);
    if (F(BYTE, o, 52) & 16)
        F(void*, o, 12) = operator new(size / 2);
    if (F(BYTE*, o, 8) && (!(F(BYTE, o, 52) & 16) || F(BYTE*, o, 12))) {
        F(int, o, 28) = CallC<int>(FUN_4cef50, F(BYTE*, o, 8), F(WORD, o, 24) * F(WORD, o, 20) * F(WORD, o, 22));
        if (CallC<int>(FUN_5e8160, o, f, pos))
            return o;
        CallVirt(o, 0, 1);
        return 0;
    }
    CallVirt(o, 0, 1);
    g_q26_7218bc = 2;
    return 0;
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_5d89e0(); KING_ALIAS("_FUN_5d89e0", "@Fw5d89e0_Destroy@4")
// [$L_5d88e0]
extern "C" BYTE* __fastcall Dtor5d88e0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5d89e0, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

// [FUN_5d8b90] tells the +56 object (slot 52) the +12 value
extern "C" void __fastcall Wnd5d8b90_Notify(BYTE* s, void*)
{
    BYTE* o = F(BYTE*, s, 56);
    if (o)
        CallVirt(o, 52, F(int, s, 12));
}

// ---------------------------------------------------------------------------
// Small methods: message filters, record copies, frees and deleting destructors.

extern "C" void FUN_5e44e0(); KING_ALIAS("_FUN_5e44e0", "@Loader_Request@16")
// [$L_5dfef0] on a 4095 message, asks the loader l for its +12 item
extern "C" void __cdecl Loader_OnMessage(int* m, BYTE* l)
{
    if (m[0] == 4095 && l)
        CallThis(FUN_5e44e0, l, 4095, m[3]);
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

extern "C" void Mfc_AfxGetThread(); KING_ALIAS("_Mfc_AfxGetThread", "_Afx_GetThread@0")
extern "C" void Mfc_WndDefault(); KING_ALIAS("_Mfc_WndDefault", "@Wnd_Default@4")
KING_VAR(void*, g_q51_72177c, 72177c)
KING_VAR(int, g_q51_721778, 721778)
KING_VAR(BYTE*, g_q51_721750, 721750)
// [$L_5dd510] the current thread, fetched once into 72177c
extern "C" void* __stdcall App_Thread(int)
{
    if (!g_q51_72177c)
        g_q51_72177c = CallStd<void*>(Mfc_AfxGetThread);
    return g_q51_72177c;
}
// [$L_5dd330] on activation (a): stores it in the 721750 frame (+336, +328) and tells its view (+252, slot 60),
// then the default handling
extern "C" int __fastcall Frame5dd330_OnActivate(BYTE* s, void*, int a, int)
{
    if (g_q51_721778) {
        F(int, g_q51_721750, 336) = a;
        F(int, g_q51_721750, 328) = a;
        if (F(int, g_q51_721750, 328))
            CallVirt(F(BYTE*, g_q51_721750, 252), 60);
    }
    return CallThis<int>(Mfc_WndDefault, s);
}

// ---------------------------------------------------------------------------
// Small methods: table constructors and clears, list teardown, cursor handling, flag maps.

extern "C" void Mfc_FrameOnSetCursor(); KING_ALIAS("_Mfc_FrameOnSetCursor", "@FrameWnd_OnSetCursor@20")
// [$L_5dd3a0] hides the cursor while the 721750 frame has flag 0x400 or over the client area, else the default
extern "C" int __fastcall Frame5dd3a0_OnSetCursor(BYTE* s, void*, void* wnd, unsigned hit, unsigned msg)
{
    if ((F(DWORD, g_q51_721750, 268) & 0x400) || hit == 1) {
        SetCursor(0);
        return 1;
    }
    return CallThis<int>(Mfc_FrameOnSetCursor, s, wnd, hit, msg);
}

// ---------------------------------------------------------------------------
// Small methods: pool slots, list teardown, loader thread, frame shutdown, message dispatch.

extern "C" void FUN_5ead00(); KING_ALIAS("_FUN_5ead00", "@Room_Each@16")
extern "C" void FUN_5e3ba0(); KING_ALIAS("_FUN_5e3ba0", "_Fw_Startup")
extern "C" void FUN_5f33e0(); KING_ALIAS("_FUN_5f33e0", "@Node_Call5f2a40b@12")
extern "C" void FUN_5f2dd0(); KING_ALIAS("_FUN_5f2dd0", "@Node_Call5f2a40a@12")
extern "C" void FUN_5fa910(); KING_ALIAS("_FUN_5fa910", "@Mesh_BuildVertices@12")
extern "C" void FUN_5f0b10(); KING_ALIAS("_FUN_5f0b10", "@Mesh_PrepareDraw@12")
extern "C" void Mfc_AfxEndThread(); KING_ALIAS("_Mfc_AfxEndThread", "_Afx_EndThread@8")
extern "C" int __cdecl Room_LinkDoor(BYTE* n, BYTE* rec);
KING_VAR(HDC, g_q53_72091c, 72091c)
// [$L_5d9300] switching the current room (+468) to r: walks r with the 5d9370 visitor and a zeroed 432-byte
// record holding the old and the new room
extern "C" void __fastcall Wnd5d9300_EnterRoom(BYTE* s, void*, BYTE* r)
{
    if (!r || F(BYTE*, s, 468) == r)
        return;
    DWORD rec[108];
    for (int k = 0; k < 108; k++)
        rec[k] = 0;
    rec[1] = F(DWORD, s, 468);
    rec[2] = (DWORD)r;
    CallThis(FUN_5ead00, r, (void*)Room_LinkDoor, rec);
}
// [$L_5e4390] the loader thread p: every wake (or 30 s) ends when +12 is set, else runs the pending request
// (+20, once per +16 flag) on the +28 object with +24
extern "C" unsigned __cdecl Fw_LoaderThread(BYTE* p)
{
    for (;;) {
        WaitForSingleObject(F(HANDLE, p, 36), 30000);
        if (F(int, p, 12)) {
            F(int, p, 32) = 0;
            CallStd(Mfc_AfxEndThread, 0, 1);
        }
        if (F(int, p, 20) && F(int, p, 16) != 1) {
            CallVirt(F(BYTE*, p, 28), 84, F(int, p, 20), 1, F(int, p, 24));
            F(int, p, 16) = 1;
        }
    }
}
// [$L_5dd560] on destroy: leaves the 721750 frame's full screen (+328, +340; spins when +396 is set) and starts it,
// releases the 72091c DC of the window (+28) and quits
extern "C" void __fastcall Frame5dd560_OnDestroy(BYTE* s, void*)
{
    BYTE* f = g_q51_721750;
    if (f) {
        if (F(int, f, 328)) {
            F(int, f, 328) = 0;
            F(int, g_q51_721750, 340) = 1;
            if (F(int, g_q51_721750, 396))
                for (;;) {
                }
        }
        CallThis(FUN_5e3ba0, g_q51_721750);
    }
    if (g_q53_72091c)
        ReleaseDC(F(HWND, s, 28), g_q53_72091c);
    PostQuitMessage(0);
}

// ---------------------------------------------------------------------------
// Node link clearing by room name.

// [$L_5d94d0] clears the +128 link of a type-14 node with flag 2 whose name (+132) is the context room name
extern "C" int __cdecl Node_ClearLinkByName(BYTE* n, BYTE** ctx)
{
    if (F(int, n, 16) == 14 && (F(BYTE, n, 40) & 2))
        if (strcmp(F(const char*, *ctx, 60), (const char*)n + 132) == 0)
            F(int, n, 128) = 0;
    return -1;
}

// ---------------------------------------------------------------------------
// The window size limits.

extern "C" void FUN_5d3f70(); KING_ALIAS("_FUN_5d3f70", "_Video_Get67fcf0")
extern "C" void FUN_5d3f80(); KING_ALIAS("_FUN_5d3f80", "_Video_Get67fcf4")
extern "C" int __cdecl Wnd_FrameExtra(int edge);    // directdraw.cpp
// [$L_5dd650] the size limits (MINMAXINFO) of the 721750 frame with flag 1: the video size by the scale (+256,
// +260) plus the frame, as the maximized, minimum and maximum track sizes; else the default
extern "C" void __fastcall Frame5dd650_OnGetMinMaxInfo(BYTE* s, void*, MINMAXINFO* mm)
{
    if (!(F(BYTE, g_q51_721750, 292) & 1)) {
        CallThis(Mfc_WndDefault, s);
        return;
    }
    mm->ptMaxPosition.x = 0;
    mm->ptMaxPosition.y = 0;
    int sx = F(int, g_q51_721750, 256);
    int w = CallC<int>(FUN_5d3f70) * sx;
    w += Wnd_FrameExtra(2);
    w += Wnd_FrameExtra(1);
    int sy = F(int, g_q51_721750, 260);
    int h = CallC<int>(FUN_5d3f80) * sy;
    h += Wnd_FrameExtra(4);
    h += Wnd_FrameExtra(3);
    mm->ptMaxSize.x = w;
    mm->ptMinTrackSize.x = w;
    mm->ptMaxTrackSize.x = w;
    mm->ptMaxSize.y = h;
    mm->ptMinTrackSize.y = h;
    mm->ptMaxTrackSize.y = h;
}

// ---------------------------------------------------------------------------
// Node link by room and object name.

// [$L_5d8c60] links a type-14 node with flag 2 (+128) to the context object (+16) when its names (+132, +164)
// are the context room's (+0, name +60) and the object's (+12)
extern "C" int __cdecl Node_LinkByNames(BYTE* n, BYTE** ctx)
{
    if (F(int, n, 16) == 14 && (F(BYTE, n, 40) & 2) &&
        strcmp(F(const char*, ctx[0], 60), (const char*)n + 132) == 0) {
        BYTE* o = ctx[4];
        if (strcmp(F(const char*, o, 12), (const char*)n + 164) == 0)
            F(BYTE*, n, 128) = o;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Node message dispatch.

// [$L_5db660] passes the message m (by its type +16: 1, 2, 3, 22) with a to its handler; always -1
extern "C" int __cdecl Node_Dispatch(BYTE* m, int a)
{
    switch (F(int, m, 16)) {
    case 1: CallThis(FUN_5f0b10, m, a); break;
    case 2: CallThis(FUN_5f33e0, m, a); break;
    case 3: CallThis(FUN_5f2dd0, m, a); break;
    case 22: CallThis(FUN_5fa910, m, a); break;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// File paths of the application folder.

KING_VAR(AsmLabel, s_q72_671204, 671204)
KING_VAR(AsmLabel, s_q72_6766ac, 6766ac)
KING_VAR(AsmLabel, s_q72_6766a4, 6766a4)
KING_VAR(char, g_q72_720ad4, 720ad4)
KING_VAR(char, g_q72_720d8c, 720d8c)
extern "C" void FUN_5e5cb0(); KING_ALIAS("_FUN_5e5cb0", "@Obj5e5cb0_Init@8")

// [FUN_5dc230] the folder of s (+648, nothing when it is ".") followed by name, in a static buffer
extern "C" char* __fastcall App_Path(BYTE* s, void*, const char* name)
{
    char* b = &g_q72_720ad4;
    const char* d = F(const char*, s, 648);
    if (strcmp(d, (const char*)s_q72_671204) == 0)
        *b = 0;
    else
        strcpy(b, d);
    strcat(b, name);
    return b;
}

// [FUN_5dc2d0] like 5dc230 with a subfolder sub (left out when it is ".") before name, in another buffer
extern "C" char* __fastcall App_SubPath(BYTE* s, void*, const char* sub, const char* name)
{
    char* b = &g_q72_720d8c;
    const char* d = F(const char*, s, 648);
    if (strcmp(d, (const char*)s_q72_671204) == 0)
        *b = 0;
    else
        strcpy(b, d);
    if (strcmp(sub, (const char*)s_q72_671204) != 0)
        strcat(b, sub);
    strcat(b, name);
    return b;
}

// [$L_5d9c20] the files of a scene name: folder ".", name.b3d and name.res
extern "C" void __fastcall App_SceneFiles(void*, void*, const char* name, char* dir, char* b3d, char* res)
{
    strcpy(dir, (const char*)s_q72_671204);
    strcpy(b3d, name);
    strcat(b3d, (const char*)s_q72_6766ac);
    strcpy(res, name);
    strcat(res, (const char*)s_q72_6766a4);
}

// [$L_5e5760] constructor: everything cleared, the scale (+28) 1, the +52 part initialised
extern "C" BYTE* __fastcall Obj5e5760_Construct(BYTE* s)
{
    F(int, s, 32) = 0;
    F(int, s, 36) = 0;
    F(int, s, 40) = 0;
    F(int, s, 44) = 0;
    CallThis(FUN_5e5cb0, s + 52);
    F(short, s, 4) = 0;
    F(short, s, 6) = 0;
    F(int, s, 20) = 0;
    F(int, s, 16) = 0;
    F(int, s, 12) = 0;
    F(int, s, 8) = 0;
    F(int, s, 24) = 0;
    F(float, s, 28) = 1.0f;
    return s;
}

// ---------------------------------------------------------------------------
// Resource records and mask headers.

extern "C" void FUN_5ab0f0(); KING_ALIAS("_FUN_5ab0f0", "_File_ReadShort")
extern "C" void FUN_5bbeb0(); KING_ALIAS("_FUN_5bbeb0", "@Bitmap_SetPixels@16")

// [FUN_5e52b0] replaces the data of record id (tables of 1000 at s) with a copy of size bytes of data, marks it
// loaded (2) and binds it under a; -1 when there is no such record
extern "C" int __fastcall Resources_SetData(BYTE** s, void*, int id, int a, unsigned size, const void* data)
{
    if (id < 0)
        return -1;
    BYTE** t = (BYTE**)s[id / 1000];
    if (!t)
        return -1;
    BYTE* e = t[id % 1000];
    if (!e)
        return -1;
    if (F(void*, e, 24))
        operator delete(F(void*, e, 24));
    F(unsigned, e, 20) = size;
    F(void*, e, 24) = size ? operator new(size) : 0;
    F(int, e, 28) = 0;
    memcpy(F(void*, e, 24), data, size);
    F(int, e, 0) = 2;
    return CallThis<int>(FUN_5e5420, s, a, e);
}

// [FUN_5e94d0] reads a mask header (MASK, MSKR, MS16 or MSK8; R and 8 set +28 bit 7, MS16 is 2 bytes a pixel)
// from f into m: width, height, the data size (size less the 776-byte header, doubled at 16 bits); with pixels
// the bitmap takes them from the file; 0 for an unknown tag
extern "C" int __cdecl Mask_ReadHeader(BYTE* m, FILE* f, int size, int pixels)
{
    char tag[5];
    fread(tag, 4, 1, f);
    tag[4] = 0;
    if (strcmp(tag, "MASK") == 0)
        F(short, m, 8) = 1;
    else if (strcmp(tag, "MSKR") == 0) {
        F(short, m, 8) = 1;
        F(unsigned, m, 28) |= 128;
    } else if (strcmp(tag, "MS16") == 0)
        F(short, m, 8) = 2;
    else if (strcmp(tag, "MSK8") == 0) {
        F(short, m, 8) = 1;
        F(unsigned, m, 28) |= 128;
    } else
        return 0;
    F(short, m, 4) = CallC<short>(FUN_5ab0f0, f);
    F(short, m, 6) = CallC<short>(FUN_5ab0f0, f);
    F(int, m, 12) = size - 776;
    if (CallC<int>(FUN_5d8350) == 2)
        F(int, m, 12) *= 2;
    F(int, m, 16) = 0;
    if (pixels) {
        CallThis(FUN_5bbeb0, m, ftell(f) + pixels + 768, 1);
        F(unsigned, m, 28) |= 0x200;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Mask data and room entry.

extern "C" void FUN_5ef640(); KING_ALIAS("_FUN_5ef640", "@Node_Root@8")

// [FUN_5e9690] reads the pixels of the mask at off in f into m (+16, +12 bytes) after checking its tag (MSKR and
// MSK8 set +28 bit 7); +28 bit 0 marks it loaded; 0 for an unknown tag
extern "C" int __cdecl Mask_ReadPixels(BYTE* m, FILE* f, long off)
{
    char tag[5];
    fseek(f, off, 0);
    fread(tag, 4, 1, f);
    tag[4] = 0;
    if (strcmp(tag, "MASK") != 0) {
        if (strcmp(tag, "MSKR") == 0)
            F(unsigned, m, 28) |= 128;
        else if (strcmp(tag, "MS16") != 0) {
            if (strcmp(tag, "MSK8") != 0)
                return 0;
            F(unsigned, m, 28) |= 128;
        }
    }
    CallC<short>(FUN_5ab0f0, f);
    CallC<short>(FUN_5ab0f0, f);
    fseek(f, off + 776, 0);
    fread(F(void*, m, 16), F(int, m, 12), 1, f);
    F(unsigned, m, 28) |= 1;
    return 1;
}

// [$L_5d9370] room visitor: a door node (type 14, +40 bit 1) whose target room (+132) is the new room of rec
// (rec[1]'s list) and whose target node (+164) is in that room is linked to it (+128) and enumerated
extern "C" int __cdecl Room_LinkDoor(BYTE* n, BYTE* rec)
{
    if (F(int, n, 16) != 14 || !(F(BYTE, n, 40) & 2))
        return -1;
    BYTE* e = F(BYTE*, rec, 4);
    for (; e; e = F(BYTE*, e, 8))
        if (F(const char*, e, 60) && strcmp(F(const char*, e, 60), (const char*)n + 132) == 0)
            break;
    if (!e)
        return -1;
    BYTE* g = F(BYTE*, e, 64);
    for (; g; g = F(BYTE*, g, 8))
        if (strcmp(F(const char*, g, 12), (const char*)n + 164) == 0)
            break;
    if (!g)
        return -1;
    Fw_Enumerate5d8c60(F(int, rec, 8), CallThis<int>(FUN_5ef640, n), g);
    F(BYTE*, n, 128) = g;
    return -1;
}

// ---------------------------------------------------------------------------
// Saving the resource tables; window commands.

extern "C" void FUN_5e4700(); KING_ALIAS("_FUN_5e4700", "@Resource_Write@12")
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
typedef HRESULT(__stdcall* Fw_StreamWrite)(void*, const void*, ULONG, ULONG*);
static void Fw_WriteInt(void* st, int v)
{
    ULONG n;
    ((Fw_StreamWrite)(*(void***)st)[4])(st, &v, 4, &n);
}

// [FUN_5e4c00] writes the 400 resource tables of s to stream st: 0 for an empty table, else 1 and per each of
// its 500 records -1 or the id, the name length, the name and the record itself; -2 closes each table
extern "C" int __fastcall Resources_Save(BYTE** s, void*, void* st)
{
    for (int i = 0; i < 400; i++) {
        BYTE** t = (BYTE**)s[i];
        if (!t)
            Fw_WriteInt(st, 0);
        else {
            Fw_WriteInt(st, 1);
            for (int j = 0; j < 500; j++) {
                BYTE* e = t[j];
                if (!e) {
                    Fw_WriteInt(st, -1);
                    continue;
                }
                Fw_WriteInt(st, F(int, e, 4));
                const char* name = F(const char*, F(BYTE*, e, 12), 4016);
                ULONG len = strlen(name) + 1, n;
                Fw_WriteInt(st, len);
                ((Fw_StreamWrite)(*(void***)st)[4])(st, F(const char*, F(BYTE*, e, 12), 4016), len, &n);
                CallThis(FUN_5e4700, e, st);
            }
        }
        Fw_WriteInt(st, -2);
    }
    return 0;
}

// [$L_5de430] runs the command of +36 on the window +32 (0: vtbl+184; 10/11: +548 = 0/1 and vtbl+108; 20/21:
// vtbl+100; 30/31: vtbl+104; 40: vtbl+172; 400: vtbl+180), then posts event 1
extern "C" void __fastcall Cmd5de430_Run(BYTE* s)
{
    BYTE* w = F(BYTE*, s, 32);
    switch (F(int, s, 36)) {
    case 0: CallVirt(w, 184); break;
    case 10: F(int, w, 548) = 0; CallVirt(F(BYTE*, s, 32), 108); break;
    case 11: F(int, w, 548) = 1; CallVirt(F(BYTE*, s, 32), 108); break;
    case 20: F(int, w, 548) = 0; CallVirt(F(BYTE*, s, 32), 100); break;
    case 21: F(int, w, 548) = 1; CallVirt(F(BYTE*, s, 32), 100); break;
    case 30: F(int, w, 548) = 0; CallVirt(F(BYTE*, s, 32), 104); break;
    case 31: F(int, w, 548) = 1; CallVirt(F(BYTE*, s, 32), 104); break;
    case 40: CallVirt(w, 172); break;
    case 400: CallVirt(w, 180); break;
    }
    CallThis(FUN_5d32f0, s, 1);
}

// ---------------------------------------------------------------------------
// Writing a bitmap file.

extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")
KING_VAR(AsmLabel, s_q76_68352c, 68352c)
extern "C" size_t __cdecl fwrite(const void* p, size_t size, size_t n, FILE* f);
// [FUN_5e9d40] writes bitmap hbm of dc as a BMP file to f with the header bi; palette images become 24-bit ones
extern "C" void __cdecl Bmp_Write(FILE* f, BITMAPINFO* bi, HBITMAP hbm, HDC dc)
{
    BITMAPINFOHEADER* h = &bi->bmiHeader;
    BYTE* bits = (BYTE*)GlobalAlloc(0, h->biSizeImage);
    GetLastError();
    SetLastError(0);
    if (!GetDIBits(dc, hbm, 0, (WORD)h->biHeight, bits, bi, 0)) {
        GetLastError();
        CallC(FUN_5d2ba0, (const char*)s_q76_68352c, 0);
    }
    BITMAPINFOHEADER t;
    if (h->biBitCount < 24) {
        t = *h;
        t.biBitCount = 24;
        t.biClrUsed = 0;
        t.biSizeImage = (((int)t.biWidth * 24 + 31) / 32 * (int)t.biHeight * 32) / 8;
        BYTE* out = (BYTE*)GlobalAlloc(0, t.biSizeImage);
        BYTE* src = bits;
        BYTE* row = out;
        for (int y = 0; y < (int)t.biHeight; y++) {
            BYTE* d = row;
            for (int x = 0; x < (int)t.biWidth; x++, d += 3) {
                const BYTE* c = (const BYTE*)&bi->bmiColors[src[x]];
                d[0] = c[0];
                d[1] = c[1];
                d[2] = c[2];
            }
            src += h->biSizeImage / (DWORD)h->biHeight;
            row += t.biSizeImage / (DWORD)t.biHeight;
        }
        GlobalFree(bits);
        bits = out;
        h = &t;
    }
    BITMAPFILEHEADER fh;
    fh.bfType = 19778;
    fh.bfSize = h->biSize + h->biClrUsed * 4 + h->biSizeImage + 14;
    fh.bfReserved1 = 0;
    fh.bfReserved2 = 0;
    fh.bfOffBits = h->biSize + h->biClrUsed * 4 + 14;
    fwrite(&fh, 14, 1, f);
    fwrite(h, h->biClrUsed * 4 + 40, 1, f);
    fwrite(bits, h->biSizeImage, 1, f);
    GlobalFree(bits);
}

// ---------------------------------------------------------------------------
// Drawing a view.

extern "C" void FUN_5da950(); KING_ALIAS("_FUN_5da950", "_Matrix_FromPose")
extern "C" void FUN_5ae0e0(); KING_ALIAS("_FUN_5ae0e0", "@Render_SetCamera@16")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5b6190(); KING_ALIAS("_FUN_5b6190", "_Device_WaitIdle")
extern "C" void FUN_5b6180(); KING_ALIAS("_FUN_5b6180", "_Device_BeginScene")
extern "C" void FUN_5d2fa0(); KING_ALIAS("_FUN_5d2fa0", "_Time_NowMs")
extern "C" void FUN_5d2fc0(); KING_ALIAS("_FUN_5d2fc0", "_Time_Delta")
extern "C" void FUN_5adc70(); KING_ALIAS("_FUN_5adc70", "@Render_Set8460@12")
extern "C" void FUN_5eb280(); KING_ALIAS("_FUN_5eb280", "@Camera_Render@24")
KING_VAR(AsmLabel, g_q77_7025f0, 7025f0)
KING_VAR(AsmLabel, g_q77_71e850, 71e850)
KING_VAR(AsmLabel, g_q77_71f808, 71f808)
KING_VAR(unsigned, g_q77_702e8c, 702e8c)
KING_VAR(BYTE*, g_q77_721750, 721750)

// [FUN_5dbd30] the camera matrix of view s: the inverse of the one built from its position and angles
// (+2904..+2948, the angle +2928 turned by +2952)
extern "C" BYTE* __fastcall View_CameraMatrix(BYTE* s, void*, BYTE* out)
{
    BYTE m[48], t[48];
    memcpy(m, CallC<BYTE*>(FUN_5da950, t, F(double, s, 2904), F(double, s, 2912), F(double, s, 2920),
                           F(double, s, 2952) + F(double, s, 2928), F(double, s, 2936), F(double, s, 2944)), 48);
    memcpy(m, CallC<BYTE*>(FUN_5ac2e0, t, m), 48);
    memcpy(out, m, 48);
    return out;
}

// [$L_5df6f0] draws view s: the overlay (+2860) first, then with a room (+488) the camera (+468) renders the room
// with the renderer (+464, fog colours from +2960), the room's sky object (+76) drawn first and the overlay again
// with its own matrix; the frame time goes to 71e850/71f808
extern "C" void __fastcall View_Draw(BYTE* s)
{
    BYTE m1[48], m2[48], t[48];
    memcpy(m2, (BYTE*)g_q77_7025f0, 48);
    memcpy(m1, (BYTE*)g_q77_7025f0, 48);
    if (F(BYTE*, s, 2860)) {
        CallC(FUN_5b6190);
        CallVirt(F(BYTE*, s, 2860), 48);
        CallC(FUN_5b6180);
    }
    if (!F(BYTE*, s, 488))
        return;
    memcpy(m1, View_CameraMatrix(s, 0, t), 48);
    memcpy(m2, m1, 48);
    CallC(FUN_5d2fa0, (BYTE*)g_q77_71e850);
    if (g_q77_702e8c & 0x100)
        CallThis(FUN_5adc70, F(BYTE*, s, 464), F(int, s, 548));
    BYTE* r = F(BYTE*, s, 464);
    F(int, r, 8480) = F(int, s, 2960);
    F(int, r, 8484) = F(int, s, 2964);
    r = F(BYTE*, s, 464);
    F(int, r, 8488) = F(int, s, 2968);
    F(int, r, 8492) = F(int, s, 2972);
    BYTE* room = F(BYTE*, s, 488);
    if (room && F(BYTE*, room, 76)) {
        CallThis(FUN_5ae0e0, F(BYTE*, s, 464), (BYTE*)g_q77_7025f0, (BYTE*)g_q77_7025f0);
        room = F(BYTE*, s, 488);
        BYTE* o = room ? F(BYTE*, room, 76) : 0;
        CallVirt(o, 56, F(int, g_q77_721750, 344) ? 1 : 5, m2, m1, F(BYTE*, s, 464));
    }
    CallThis(FUN_5eb280, F(BYTE*, s, 468), F(BYTE*, s, 488), F(BYTE*, s, 472), 1, F(BYTE*, s, 464));
    if (F(BYTE*, s, 2860)) {
        F(int, F(BYTE*, s, 464), 8668) = 1;
        BYTE* p = CallC<BYTE*>(FUN_5ac2e0, t, F(BYTE*, s, 2860) + 920);
        memcpy(F(BYTE*, s, 464) + 8564, p, 48);
        CallThis(FUN_5eb280, F(BYTE*, s, 468), F(BYTE*, s, 488), F(BYTE*, s, 472), 1, F(BYTE*, s, 464));
        F(int, F(BYTE*, s, 464), 8668) = 0;
    }
    CallC(FUN_5d2fc0, (BYTE*)g_q77_71e850, (BYTE*)g_q77_71f808);
}

// ---------------------------------------------------------------------------
// The pause sign.

extern "C" void FUN_5ea0e0(); KING_ALIAS("_FUN_5ea0e0", "@Font_TextWidth@12")
extern "C" void FUN_5ea170(); KING_ALIAS("_FUN_5ea170", "_Text_Draw")
extern "C" void FUN_5b6b10(); KING_ALIAS("_FUN_5b6b10", "@Surface_Present@8")
extern "C" void __cdecl Ctl_DrawFrame(BYTE* dst, const int* r, DWORD flags, int color, int a5, int light, int dark,
                                      int dx, int dy, int fill, BYTE* bmp, int pattern);    // controls.cpp
KING_VAR(AsmLabel, s_q78_682f50, 682f50)
// [$L_5e04d0] with a font (+2864) draws PAUSE in a frame at 130,90 on the surface (+12) and presents it
extern "C" void __fastcall View_DrawPause(BYTE* s)
{
    if (!F(BYTE*, s, 2864))
        return;
    int w = CallThis<int>(FUN_5ea0e0, F(BYTE*, s, 2864), (const char*)s_q78_682f50) + 140;
    int h = F(int, F(BYTE*, s, 2864), 20) + 10;
    int r[4] = {130, 90, w, h + 90};
    Ctl_DrawFrame(F(BYTE*, s, 12), r, 513, 2, 0, 4, 0, 0, 0, 0x3f000000, 0, 0);
    CallC(FUN_5ea170, F(BYTE*, s, 12), 135, h + 85, F(BYTE*, s, 2864), (const char*)s_q78_682f50, 5, 0);
    CallThis(FUN_5b6b10, F(BYTE*, s, 12));
}

// ---------------------------------------------------------------------------
// Sector reads of an mmio file.

// [FUN_5e6c70] copies n bytes at pos to dst: from the pending data (+131588, +131592 left) when there is some,
// else reading whole 512-byte sectors into the buffer (+4, at most 257 at a time)
extern "C" int __fastcall Mmio_Read(BYTE* s, void*, int pos, BYTE* dst, int n)
{
    if (BYTE* p = F(BYTE*, s, 131588)) {
        memcpy(dst, p, n);
        int left = F(int, s, 131592) - n;
        F(BYTE*, s, 131588) += n;
        F(int, s, 131592) = left;
        if (left <= 0)
            F(BYTE*, s, 131588) = 0;
        return 1;
    }
    int off = pos % 512;
    int sectors = (off + n) / 512 + ((off + n) % 512 != 0);
    mmioSeek(F(HMMIO, s, 0), pos - off, 0);
    while (n > 0) {
        int chunk, k;
        if (sectors > 257) {
            chunk = 257;
            k = 131584 - off;
        } else {
            chunk = sectors;
            k = n;
        }
        mmioRead(F(HMMIO, s, 0), (HPSTR)(s + 4), chunk * 512);
        memcpy(dst, s + 4 + off, k);
        dst += k;
        n -= k;
        off = 0;
        sectors -= chunk;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Texture chunks.

extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")
extern "C" int __cdecl fgetc(FILE* f);
// [FUN_5e7f20] reads the chunks of a texture (size bytes of f) into dst: DIMS gives the levels, height and width
// (with several levels dst moves past the bigger ones); DATA is read as is when the file and wanted depths match
// (1/1 or 2/2), or turned from palette indices (pal, 3 bytes each) into 4-byte BGR0 pixels for 4/1; 0 when DATA
// comes before DIMS
extern "C" int __cdecl Texture_ReadChunks(BYTE* dst, FILE* f, int depth, int size, const BYTE* pal, int want)
{
    int levels = -1, h = -1, w = -1;
    int end = ftell(f) + size;
    if (end <= ftell(f))
        return 1;
    do {
        char tag[4];
        tag[0] = (char)fgetc(f);
        tag[1] = (char)fgetc(f);
        tag[2] = (char)fgetc(f);
        tag[3] = (char)fgetc(f);
        int len = (CallC<int>(FUN_5ab140, f) + 3) & ~3;
        int start = ftell(f);
        if (strncmp(tag, "DIMS", 4) == 0) {
            int a = CallC<int>(FUN_5ab140, f);
            levels = a;
            h = CallC<int>(FUN_5ab140, f);
            w = CallC<int>(FUN_5ab140, f);
            int n = w * h * depth * 4;
            if (a > 1) {
                levels = 1;
                for (int k = a - 1; k; k--) {
                    dst += n;
                    n <<= 2;
                }
            }
        } else if (strncmp(tag, "DATA", 4) == 0) {
            if (levels == -1)
                return 0;
            int n = w * h * depth;
            if (depth == 1) {
                if (want == 1)
                    fread(dst, n, 1, f);
            } else if (depth == 2) {
                if (want == 2)
                    fread(dst, n, 1, f);
            } else if (depth == 4 && want == 1) {
                BYTE* d = dst;
                for (int y = h; y > 0; y--)
                    for (int x = w; x > 0; x--, d += 4) {
                        const BYTE* p = pal + (BYTE)fgetc(f) * 3;
                        d[0] = p[2];
                        d[1] = p[1];
                        d[2] = p[0];
                        d[3] = 0;
                    }
            }
        }
        fseek(f, start + len, 0);
    } while (end > ftell(f));
    return 1;
}

// ---------------------------------------------------------------------------
// Writing a TGA file.

extern "C" void FUN_5d4200(); KING_ALIAS("_FUN_5d4200", "_Dd_LockBack")
extern "C" void FUN_5d41e0(); KING_ALIAS("_FUN_5d41e0", "_Dd_WaitVsync")
extern "C" void FUN_5ab110(); KING_ALIAS("_FUN_5ab110", "_File_WriteShort")
extern "C" int __cdecl fputc(int c, FILE* f);
extern "C" void FUN_5d3ab0(); KING_ALIAS("_FUN_5d3ab0", "_Mode_TextureLevel")
// [FUN_5e9a10] writes surface s to f as a TGA: 8-bit images with the root palette (BGR), 16-bit ones as
// uncompressed true colour; a surface on the screen (+116) is read from the locked back buffer, its 565 pixels
// turned into 555 ones unless the mode is 3
extern "C" void __cdecl Tga_Write(BYTE* s, FILE* f)
{
    fputc(0, f);
    int pal = F(WORD, s, 4) != 2;
    fputc(pal, f);
    int type = F(WORD, s, 4) == 2 ? 2 : 1;
    fputc(type, f);
    CallC(FUN_5ab110, 0, f);
    CallC(FUN_5ab110, 256, f);
    fputc(24, f);
    CallC(FUN_5ab110, 0, f);
    CallC(FUN_5ab110, 0, f);
    int w = F(WORD, s, 0);
    CallC(FUN_5ab110, w, f);
    int h = F(WORD, s, 2);
    CallC(FUN_5ab110, h, f);
    fputc(F(WORD, s, 4) != 2 ? 8 : 16, f);
    fputc(32, f);
    if (pal) {
        const BYTE* q = CallThis<BYTE*>(FUN_5b6700, s) + 24;
        for (int k = 256; k; k--, q += 3) {
            fputc(q[2], f);
            fputc(q[1], f);
            fputc(q[0], f);
        }
    }
    BYTE* data = F(BYTE*, s, 24 + F(int, s, 16) * 4);
    if (type == 1) {
        fwrite(data, h * w, 1, f);
        return;
    }
    if (type != 2)
        return;
    if (!F(int, s, 116)) {
        fwrite(data, F(WORD, s, 4) * h * w, 1, f);
        return;
    }
    int pitch;
    BYTE* p = CallC<BYTE*>(FUN_5d4200, &pitch);
    if (CallC<int>(FUN_5d3ab0) == 3) {
        for (int y = 0; y < (int)F(WORD, s, 2); y++) {
            fwrite(p, F(WORD, s, 4) * w, 1, f);
            p += pitch;
        }
    } else {
        WORD* buf = (WORD*)operator new(F(WORD, s, 0) * 2);
        for (int y = 0; y < (int)F(WORD, s, 2); y++) {
            const WORD* src = (const WORD*)p;
            for (int x = 0; x < (int)F(WORD, s, 0); x++)
                buf[x] = (WORD)(((src[x] >> 1) & 0x7fe0) | (src[x] & 31));
            fwrite(buf, F(WORD, s, 4) * w, 1, f);
            p += pitch;
        }
        operator delete(buf);
    }
    CallC(FUN_5d41e0, p);
}

// ---------------------------------------------------------------------------
// Resetting the world.

extern "C" void FUN_5e4d30(); KING_ALIAS("_FUN_5e4d30", "@Resources_FreeAll@8")
extern "C" void FUN_5d3460(); KING_ALIAS("_FUN_5d3460", "@Owner_Clear@8")
extern "C" void FUN_5fd160(); KING_ALIAS("_FUN_5fd160", "_Sound_Frame")
KING_VAR(double, g_q105_64b658, 64b658)
KING_VAR(BYTE*, g_q105_721750, 721750)
// [FUN_5e03c0] resets world s: frees its resources, clears the view and time state (fog 0.1, the start time
// scaled by 64b658), the camera's +140 and the owned windows, runs the sound frame with sound on and calls the
// reset methods (vtbl+124, +120)
extern "C" void __fastcall World_Reset(BYTE* s)
{
    CallThis(FUN_5e4d30, s + 704);
    if (BYTE* o = F(BYTE*, s, 484))
        F(int, o, 8) = 0;
    F(int, s, 2968) = 0;
    F(int, s, 2972) = 0;
    double t = CallC<double>(FUN_6037e0) * g_q105_64b658;
    F(double, s, 2960) = 0.1;
    F(double, s, 2976) = 0.1;
    F(int, s, 600) = 0;
    F(double, s, 592) = t;
    static const short z[14] = {2888, 2872, 2880, 2896, 2952, 604, 2892, 2876, 2884, 2900, 2956, 2984, 460, 456};
    for (int k = 0; k < 14; k++)
        F(int, s, z[k]) = 0;
    F(int, s, 544) = -1;
    F(int, F(BYTE*, s, 468), 140) = 0;
    if (F(int, g_q105_721750, 344))
        CallC(FUN_5fd160);
    F(int, s, 2988) = 0;
    CallThis(FUN_5d3460, s + 2992);
    CallVirt(s, 124);
    CallVirt(s, 120);
    F(int, s, 3024) = 0;
}

// ---------------------------------------------------------------------------
// Rendering a frame; camera angles.

extern "C" void FUN_5de950(); KING_ALIAS("_FUN_5de950", "_Matrix_ToAngles")
extern "C" void FUN_5df3d0(); KING_ALIAS("_FUN_5df3d0", "@Camera_Follow@12")
extern "C" void FUN_5b6ed0(); KING_ALIAS("_FUN_5b6ed0", "@Surface_ClearRows@8")
extern "C" void FUN_5ae9b0(); KING_ALIAS("_FUN_5ae9b0", "@Render_BeginFrame@12")
extern "C" void FUN_5aea50(); KING_ALIAS("_FUN_5aea50", "@Render_EndFrame@8")
extern "C" void FUN_5f2190(); KING_ALIAS("_FUN_5f2190", "@Node_ApplyMatrix@16")
extern "C" void FUN_5d96c0(); KING_ALIAS("_FUN_5d96c0", "@Fw_LoadRoom@16")
KING_VAR(double, g_q130_71f7f8, 71f7f8)
KING_VAR(double, g_q130_71f808, 71f808)
KING_VAR(double, g_q130_720020, 720020)
struct Fw130_M { DWORD v[12]; };
// [FUN_5da080] renders a frame of view s with a room (+488): the timers cleared, the frame number (camera +148)
// passed to the renderer (+464), the three draw passes (vtbl+104, +100, +108); the times of the passes kept in
// +496, +504, +512
extern "C" void __fastcall View_RenderFrame(BYTE* s)
{
    if (!F(BYTE*, s, 488))
        return;
    CallThis(FUN_5b6ed0, F(BYTE*, s, 12));
    g_q130_71f808 = 0;
    g_q130_71f7f8 = 0;
    g_q130_720020 = 0;
    F(int, F(BYTE*, s, 464), 8452) = F(int, F(BYTE*, s, 468), 148);
    F(int, F(BYTE*, s, 468), 148)++;
    CallC(FUN_5b6180);
    CallThis(FUN_5ae9b0, F(BYTE*, s, 464), -1);
    CallVirt(s, 104);
    CallVirt(s, 100);
    CallVirt(s, 108);
    CallThis(FUN_5aea50, F(BYTE*, s, 464));
    CallC(FUN_5b6190);
    F(double, s, 496) = g_q130_71f808 * g_q105_64b658;
    F(double, s, 504) = g_q130_71f7f8 * g_q105_64b658;
    F(double, s, 512) = g_q130_720020 * g_q105_64b658;
}

// [FUN_5dff20] the angles of matrix m: heading, and pitch and roll negated
extern "C" void __stdcall Matrix_Angles(const Fw130_M* m, float* h, float* p, float* r)
{
    double d[6];
    CallC(FUN_5de950, *m, d + 5, d + 4, d + 3, d, d + 1, d + 2);
    *h = (float)d[0];
    *p = (float)-d[1];
    *r = (float)-d[2];
}

// [FUN_5e02f0] places the camera of view s: with a room (+488) the camera node (+616) gets the matrix +652, the
// room its distances (+3048, +3056) and the view its room (+700, or loaded from the room's names); the camera
// position (+2904..) goes to +568 and +580 as floats
extern "C" void __fastcall View_PlaceCamera(BYTE* s)
{
    if (F(BYTE*, s, 488)) {
        CallThis(FUN_5f2190, F(BYTE*, s, 616), s + 652, 0);
        F(float, F(BYTE*, s, 488), 88) = (float)F(double, s, 3048);
        F(float, F(BYTE*, s, 488), 84) = (float)F(double, s, 3056);
        CallThis(FUN_5df3d0, s, s + 652);
        if (!F(int, s, 700))
            CallThis(FUN_5d96c0, s, F(BYTE*, s, 488) + 12, F(BYTE*, s, 488) + 44);
        else
            F(int, F(BYTE*, s, 488), 76) = F(int, s, 700);
    }
    float x = (float)F(double, s, 2904), y = (float)F(double, s, 2912), z = (float)F(double, s, 2920);
    F(float, s, 568) = x;
    F(float, s, 580) = x;
    F(float, s, 572) = y;
    F(float, s, 584) = y;
    F(float, s, 576) = z;
    F(float, s, 588) = z;
}

extern "C" void FUN_5abee0(); KING_ALIAS("_FUN_5abee0", "_Matrix_RotationX")
extern "C" void FUN_5abfd0(); KING_ALIAS("_FUN_5abfd0", "_Matrix_RotationY")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
// [FUN_5df560] puts the camera of view s at node n (5df3d0, its position +36) turned by the view's angles
// (+2928 z, +2936 x negated, +2944 y) and makes room r the view room's sky (+488's +76)
extern "C" void __fastcall View_SetCamera(BYTE* s, void*, int r, BYTE* n)
{
    CallThis(FUN_5df3d0, s, n);
    BYTE m[48], tz[48], tx[48], ty[48], t1[48], t2[48];
    memcpy(m, (BYTE*)g_q77_7025f0, 48);
    BYTE* rz = CallC<BYTE*>(FUN_5ac0c0, tz, (float)F(double, s, 2928));
    BYTE* rx = CallC<BYTE*>(FUN_5abee0, tx, (float)-F(double, s, 2936));
    BYTE* ry = CallC<BYTE*>(FUN_5abfd0, ty, (float)F(double, s, 2944));
    BYTE* a = CallC<BYTE*>(FUN_5aba40, t1, ry, rx);
    memcpy(m, CallC<BYTE*>(FUN_5aba40, t2, a, rz), 48);
    memcpy(m + 36, n + 36, 12);
    CallThis(FUN_5f2190, F(BYTE*, s, 616), m, 0);
    F(int, F(BYTE*, s, 488), 76) = r;
}

extern "C" void FUN_5eb310(); KING_ALIAS("_FUN_5eb310", "@Camera_Draw@28")
KING_VAR(float, g_q132_721210, 721210)
// [FUN_5dffc0] draws object +524 of view s alone (its sky draw first) with the camera (+468) of the room
// (+488); the time it took goes to 721210
extern "C" void __fastcall View_DrawObject(BYTE* s)
{
    BYTE m1[48], m2[48], t[48];
    memcpy(m2, (BYTE*)g_q77_7025f0, 48);
    memcpy(m1, (BYTE*)g_q77_7025f0, 48);
    if (!F(BYTE*, s, 488))
        return;
    memcpy(m1, View_CameraMatrix(s, 0, t), 48);
    memcpy(m2, m1, 48);
    float t0 = (float)CallC<double>(FUN_6037e0);
    if (g_q77_702e8c & 0x100)
        CallThis(FUN_5adc70, F(BYTE*, s, 464), F(int, s, 548));
    BYTE* r = F(BYTE*, s, 464);
    F(int, r, 8480) = F(int, s, 2960);
    F(int, r, 8484) = F(int, s, 2964);
    r = F(BYTE*, s, 464);
    F(int, r, 8488) = F(int, s, 2968);
    F(int, r, 8492) = F(int, s, 2972);
    if (BYTE* o = F(BYTE*, s, 524)) {
        CallThis(FUN_5ae0e0, F(BYTE*, s, 464), (BYTE*)g_q77_7025f0, (BYTE*)g_q77_7025f0);
        CallVirt(F(BYTE*, s, 524), 56, F(int, g_q77_721750, 344) ? 1 : 5, m2, m1, F(BYTE*, s, 464));
    }
    if (F(BYTE*, s, 524))
        CallThis(FUN_5eb310, F(BYTE*, s, 468), F(BYTE*, s, 488), F(BYTE*, s, 524), F(BYTE*, s, 472), 1,
                 F(BYTE*, s, 464));
    g_q132_721210 = (float)(CallC<double>(FUN_6037e0) - t0);
}

extern "C" void FUN_5eaf40(); KING_ALIAS("_FUN_5eaf40", "@Backdrop_Place@32")
extern "C" void FUN_5e19b0(); KING_ALIAS("_FUN_5e19b0", "_Frame_Apply")
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")
extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
struct Fw133_V3 { float x, y, z; };
// [$L_5df920] prepares the view's overlay: the room's sky node (+488's +80) placement, the view's projection
// (vtbl+96) set on the camera when +472's +76 is set (and its switch +480 chosen), the frame +476 moved to the
// sky height scaled by +472 (+96, +104), then the overlay object (+528) drawn
extern "C" void __fastcall View_PrepareOverlay(BYTE* s)
{
    BYTE m[48];
    memcpy(m, (BYTE*)g_q77_7025f0, 48);
    CallThis(FUN_5f25c0, F(BYTE*, F(BYTE*, s, 488), 80), m);
    DWORD v[4];
    DWORD a, b;
    CallVirt(s, 96, m, v, &a, &b);
    BYTE* c = F(BYTE*, s, 472);
    if (F(int, c, 76) >= 0) {
        CallThis(FUN_5eaf40, F(BYTE*, s, 468), F(BYTE*, s, 488), v[0], a, b, F(BYTE*, s, 464), c);
        CallC(FUN_5e1a10, F(BYTE*, s, 480), F(int, F(BYTE*, s, 472), 76) + 1);
    } else
        CallC(FUN_5e1a10, F(BYTE*, s, 480), 0);
    c = F(BYTE*, s, 472);
    Fw133_V3 t;
    t.x = F(float, m, 36);
    t.y = F(float, m, 40);
    t.z = (float)((F(float, m, 44) - F(double, c, 96)) * F(double, c, 104) + F(double, c, 96));
    BYTE tm[48];
    CallC(FUN_5e19b0, F(BYTE*, s, 476), 0, CallC<BYTE*>(FUN_5abc40, tm, t));
    if (BYTE* o = F(BYTE*, s, 528))
        CallThis(FUN_5eb310, F(BYTE*, s, 468), F(BYTE*, s, 488), o, F(BYTE*, s, 472), 1, F(BYTE*, s, 464));
    if (BYTE* o = F(BYTE*, s, 528))
        CallVirt(o, 56, 4, (BYTE*)g_q77_7025f0, (BYTE*)g_q77_7025f0, F(BYTE*, s, 464));
}

// [FUN_5da950] the matrix of position x, y, z and angles a (z), b (x, negated), c (y) into out: the rotations
// y, x, z applied in turn, then the translation
extern "C" BYTE* __cdecl Matrix_FromPose(BYTE* out, double x, double y, double z, double a, double b, double c)
{
    Fw133_V3 v = {(float)x, (float)y, (float)z};
    BYTE t[48], rz[48], rx[48], ry[48], m1[48], m2[48];
    BYTE* r1 = CallC<BYTE*>(FUN_5abc40, t, v);
    BYTE* r2 = CallC<BYTE*>(FUN_5ac0c0, rz, (float)a);
    BYTE* r3 = CallC<BYTE*>(FUN_5abee0, rx, (float)-b);
    BYTE* r4 = CallC<BYTE*>(FUN_5abfd0, ry, (float)c);
    BYTE* r5 = CallC<BYTE*>(FUN_5aba40, m1, r4, r3);
    BYTE* r6 = CallC<BYTE*>(FUN_5aba40, m2, r5, r2);
    CallC(FUN_5aba40, out, r6, r1);
    return out;
}

#include "x87.h"
extern "C" void FUN_5abc00(); KING_ALIAS("_FUN_5abc00", "@Matrix_Transform2@12")
// [$L_5dec80] the view's mirror step: with +2984 = 1 a copies m; else a is moved along the plane normal of p
// (+64..+72) by its distance from m's position
extern "C" void __fastcall View_MirrorMove(BYTE* s, void*, float* a, const float* m, const float* p)
{
    if (F(int, s, 2984) == 1) {
        memcpy(a, m, 48);
        return;
    }
    double dx = a[9] - m[9], dy = a[10] - m[10], dz = a[11] - m[11];
    double d = dz * p[18] + dy * p[17];
    d = d + dx * p[16];
    d = x87_fabs(d);
    Fw133_V3 v = {(float)(d * p[16]), (float)(d * p[17]), (float)(d * p[18])};
    BYTE t[48];
    CallThis(FUN_5abc00, a, CallC<BYTE*>(FUN_5abc40, t, v));
}

extern "C" void FUN_5ab710(); KING_ALIAS("_FUN_5ab710", "@Vec3_AddAssign@16")
extern "C" void FUN_5ab660(); KING_ALIAS("_FUN_5ab660", "@Vec3_DivAssign@16")
extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
// [FUN_5e6520] the sphere around polygon s's vertices (+4 count, +8 indices into v, 28 bytes each): their centre
// and the largest distance from it into out
extern "C" void __fastcall Poly_BoundingSphere(BYTE* s, void*, float* out, const BYTE* v)
{
    float c[3] = {0, 0, 0}, tmp[3];
    int n = F(WORD, s, 4);
    for (int i = 0; i < n; i++)
        CallThis(FUN_5ab710, c, tmp, v + F(WORD*, s, 8)[i] * 28);
    CallThis(FUN_5ab660, c, tmp, (float)n);
    float r = 0;
    for (int i = 0; i < (int)F(WORD, s, 4); i++) {
        const float* p = (const float*)(v + F(WORD*, s, 8)[i] * 28);
        float d[3] = {(float)(c[0] - p[0]), (float)(c[1] - p[1]), (float)(c[2] - p[2])};
        double l = CallC<double>(FUN_5ab4d0, d);
        if (r < l)
            r = (float)l;
    }
    memcpy(out, c, 12);
    out[3] = r;
}

extern "C" void FUN_5ab400(); KING_ALIAS("_FUN_5ab400", "_Vec3_Normalized")
KING_VAR(float, g_q183_64c258, 64c258)
// [FUN_5e5580] grows sphere s (centre, radius +12) to also hold sphere b: the new one spans from s's far side to
// b's far side along the line between their centres
extern "C" void __fastcall Sphere_Merge(float* s, void*, const float* b)
{
    float d[3] = {(float)(s[0] - b[0]), (float)(s[1] - b[1]), (float)(s[2] - b[2])};
    float n[3];
    CallC(FUN_5ab400, n, d);
    double ra = s[3];
    float p1x = (float)((float)(n[0] * ra) + s[0]);
    float p1y = (float)((float)(n[1] * ra) + s[1]);
    double p1z = ra * n[2] + s[2];
    double rb = b[3];
    float p2x = (float)(b[0] - (float)(n[0] * rb));
    float p2y = (float)(b[1] - (float)(n[1] * rb));
    double p2z = b[2] - rb * n[2];
    double sx = p2x + p1x;
    float sy = (float)(p1y + p2y);
    float sz = (float)(p1z + p2z);
    float h = g_q183_64c258;
    s[0] = (float)(sx * h);
    s[1] = (float)(sy * h);
    s[2] = (float)(sz * h);
    float e[3] = {(float)(p1x - p2x), (float)(p1y - p2y), (float)(p1z - p2z)};
    s[3] = (float)(CallC<double>(FUN_5ab4d0, e) * h);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor5d88e0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor5d88e0_DeletingDestructor@12", "@Dtor5d88e0_DeletingDestructor@12")
// [$L_5e5570] forwards to Dtor5d88e0_DeletingDestructor for the object 64 bytes before
extern "C" int __fastcall Thunk5e5570(BYTE* s, void* d, int a0) { return q198_Dtor5d88e0_DeletingDestructor(s - 64, d, a0); }

// [$L_5e5ca0] nothing to do
extern "C" void __fastcall Fw5e5ca0_Nop(BYTE*) {}

extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")
extern "C" void FUN_5ae9b0(); KING_ALIAS("_FUN_5ae9b0", "@Render_BeginFrame@12")
extern "C" void FUN_5ba2c0(); KING_ALIAS("_FUN_5ba2c0", "@Palette_Copy@12")
KING_VAR(double, g_q219_64b658, 64b658)
KING_VAR(double, g_q219_64d0f0, 64d0f0)
KING_VAR(double, g_q219_64b678, 64b678)
KING_VAR(double, g_q219_64b6a0, 64b6a0)
KING_VAR(int, g_q219_702e90, 702e90)
KING_VAR(int, g_q219_702e8c, 702e8c)
// [FUN_5de600] starts a frame of view s: the frame step +2960 (the fixed step +3032 when set, else the time since
// the last frame +592 less the pause +600 and the app's lag (+360), at most 0.05 unless the step is negative) is
// added to the clock +2968 and kept only within 0..64b6a0; then the renderer (+464) and the palette (+3096) are set
// up for the world (+468) and the frame flags restored (+3020)
extern "C" void __fastcall View_BeginFrame(BYTE* s)
{
    double t = CallC<double>(FUN_6037e0) * g_q219_64b658;
    int fixed = F(int, s, 3032);
    double e = t - F(double, s, 592);
    F(double, s, 2976) = e;
    if (fixed > 0)
        F(double, s, 2960) = fixed * g_q219_64b658;
    else {
        double dt = e - F(double, s, 600) - F(double, g_app, 360) * g_q219_64b658;
        F(double, s, 2960) = dt;
        if (dt > g_q219_64d0f0 && fixed == 0)
            F(double, s, 2960) = 0.05;
    }
    F(double, s, 2968) = F(double, s, 2968) + F(double, s, 2960);
    if (!(F(double, s, 2960) >= g_q219_64b678) || F(double, s, 2960) > g_q219_64b6a0)
        F(double, s, 2960) = 0;
    BYTE* r = F(BYTE*, s, 464);
    F(double, s, 600) = 0;
    F(double, s, 592) = t;
    CallThis(FUN_5ae9b0, r, -1);
    r = F(BYTE*, s, 464);
    F(double, r, 8496) = F(double, s, 3064);
    F(double, r, 8504) = F(double, s, 3072);
    BYTE* w = F(BYTE*, s, 468);
    F(int, F(BYTE*, s, 464), 8452) = F(int, w, 148);
    F(int, w, 148)++;
    g_q219_702e90 = 0;
    w = F(BYTE*, s, 468);
    CallThis(FUN_5ba2c0, s + 3096, F(int*, w, 100)[F(int, w, 140)]);
    int room = F(int, F(BYTE*, s, 468), 140);
    if (F(int, s, 544) != room) {
        F(int, s, 544) = room;
        F(int, s, 3092) = 1;
    }
    g_q219_702e8c = F(int, s, 3020);
}

// [FUN_5e6800] the sphere around outline s's points (+4 count; x at +16, z at +20) placed at v: their centre and
// the largest distance from it, into v
extern "C" void __fastcall Outline_BoundingSphere(BYTE* s, void*, float* v)
{
    float c[3] = {0, 0, 0}, tmp[3];
    int n = F(WORD, s, 4);
    for (int i = 0; i < n; i++) {
        float p[3] = {(float)(F(float*, s, 16)[i] + v[0]), v[1], (float)(F(float*, s, 20)[i] + v[2])};
        CallThis(FUN_5ab710, c, tmp, p);
    }
    CallThis(FUN_5ab660, c, tmp, (float)n);
    float r = 0;
    for (int i = 0; i < (int)F(WORD, s, 4); i++) {
        double px = F(float*, s, 16)[i] + v[0];
        float py = v[1];
        float pz = (float)(F(float*, s, 20)[i] + v[2]);
        float d[3] = {(float)(c[0] - px), (float)(c[1] - py), (float)(c[2] - pz)};
        double l = CallC<double>(FUN_5ab4d0, d);
        if (!(r >= l))
            r = (float)l;
    }
    memcpy(v, c, 12);
    v[3] = r;
}

extern "C" void FUN_5ba3d0(); KING_ALIAS("_FUN_5ba3d0", "@Palette_Apply@8")
extern "C" void FUN_5b64e0(); KING_ALIAS("_FUN_5b64e0", "@Window_SetSurface@12")
extern "C" void FUN_5aea50(); KING_ALIAS("_FUN_5aea50", "@Render_EndFrame@8")
extern "C" void FUN_5fd160(); KING_ALIAS("_FUN_5fd160", "_Sound_Frame")
extern "C" void FUN_5de5b0(); KING_ALIAS("_FUN_5de5b0", "@Clock_Start@8")
extern "C" void FUN_5de5d0(); KING_ALIAS("_FUN_5de5d0", "@Clock_Stop@8")
KING_VAR(int, g_q221_721764, 721764)
KING_VAR(AsmLabel, s_q221_67fb80, 67fb80)
KING_VAR(AsmLabel, s_q221_682f34, 682f34)
KING_VAR(AsmLabel, s_q221_682f30, 682f30)
KING_VAR(AsmLabel, s_q221_682f24, 682f24)
// [$L_5de7d0] ends a frame of view s: a changed palette (+3092) is applied to the window, the renderer finishes,
// the sound is updated (when on) and, with the camera log on (721764), the clock and the camera matrix are written
// to the log file (the clock paused meanwhile)
extern "C" void __fastcall View_EndFrame(BYTE* s)
{
    if (F(int, s, 3092)) {
        CallThis(FUN_5ba3d0, s + 3096);
        BYTE* w = F(BYTE*, s, 468);
        CallThis(FUN_5b64e0, F(BYTE*, s, 12), F(int*, w, 100)[F(int, w, 140)]);
        F(int, s, 3092) = 0;
    }
    CallThis(FUN_5aea50, F(BYTE*, s, 464));
    if (F(int, g_fw721750, 344))
        CallC(FUN_5fd160);
    if (!g_q221_721764)
        return;
    CallThis(FUN_5de5b0, s);
    FILE* f = fopen((const char*)s_q221_682f34, (const char*)s_q221_67fb80);
    fprintf(f, (const char*)s_q221_682f30, F(double, s, 2968));
    float m[12];
    Camera_Get(s, 0, m);
    for (int i = 0; i < 12; i += 3)
        fprintf(f, (const char*)s_q221_682f24, (double)m[i], (double)m[i + 1], (double)m[i + 2]);
    fclose(f);
    CallThis(FUN_5de5d0, s);
}

// ---------------------------------------------------------------------------
// The main view's constructor

extern "C" void FUN_5d9cf0(); KING_ALIAS("_FUN_5d9cf0", "@MainRoomWindow_Construct@20")
extern "C" void FUN_5dd720(); KING_ALIAS("_FUN_5dd720", "@Window_ResetState@8")
extern "C" void FUN_5e4af0(); KING_ALIAS("_FUN_5e4af0", "@Resources_Construct@8")
extern "C" void FUN_5eb1e0(); KING_ALIAS("_FUN_5eb1e0", "@Camera_Init@8")
extern "C" void FUN_5b94e0(); KING_ALIAS("_FUN_5b94e0", "@Palette_Construct@8")
extern "C" void FUN_5d2100(); KING_ALIAS("_FUN_5d2100", "@MapFile_Open@16")
extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")
extern "C" void FUN_5d2ef0(); KING_ALIAS("_FUN_5d2ef0", "_Sys_Exit")
extern "C" void FUN_5ab1b0(); KING_ALIAS("_FUN_5ab1b0", "_NextToken")
extern "C" void FUN_603b70(); KING_ALIAS("_FUN_603b70", "_Ini_ReadInt")
extern "C" void FUN_603cb0(); KING_ALIAS("_FUN_603cb0", "_Ini_GetDouble")
extern "C" double __cdecl atof(const char* s);
extern "C" long __cdecl atol(const char* s);
KING_VAR(BYTE, g_mapFile71e808, 71e808)
KING_VAR(int, g_showFaceMode721770, 721770)
KING_VAR(double, g_tangLimit682a08, 682a08)
KING_VAR(double, g_stereo64dae0, 64dae0)
KING_VAR(double, g_fog64b788, 64b788)
KING_VAR(char, s_on672994, 672994)
KING_VAR(char, s_off6729a8, 6729a8)
KING_VAR(char, s_noinst673358, 673358)
KING_VAR(char, s_dot6733ac, 6733ac)
KING_VAR(char, s_ENV6730a0, 6730a0)
KING_VAR(char, s_RENDER682e68, 682e68)
KING_VAR(char, s_INSTALL6733c4, 6733c4)
KING_VAR(char, s_taskdump682e9c, 682e9c)
KING_VAR(char, s_statedump682e90, 682e90)
KING_VAR(char, s_debugtime682e84, 682e84)
KING_VAR(char, s_stereo682e7c, 682e7c)
KING_VAR(char, s_skys682e70, 682e70)
KING_VAR(char, s_showfacemode682e58, 682e58)
KING_VAR(char, s_ambient682e50, 682e50)
KING_VAR(char, s_detail682e48, 682e48)
KING_VAR(char, s_intensity682e3c, 682e3c)
KING_VAR(char, s_intenmap682e30, 682e30)
KING_VAR(char, s_alphamir682e24, 682e24)
KING_VAR(char, s_shadow672650, 672650)
KING_VAR(char, s_multitex682e18, 682e18)
KING_VAR(char, s_newkeying682e0c, 682e0c)
KING_VAR(char, s_fog682e08, 682e08)
KING_VAR(char, s_dcoeff682e00, 682e00)
KING_VAR(char, s_farplane682df4, 682df4)
KING_VAR(char, s_nearplane682de8, 682de8)
KING_VAR(char, s_skyzb682de0, 682de0)
KING_VAR(char, s_skyfun682dd8, 682dd8)
KING_VAR(char, s_tanglimith682dcc, 682dcc)
KING_VAR(char, s_zbuf682dc4, 682dc4)
KING_VAR(char, s_antialiasing682db4, 682db4)
KING_VAR(char, s_ponly682dac, 682dac)
KING_VAR(char, s_xpyramid67e1ac, 67e1ac)
KING_VAR(char, s_ypyramid682da0, 682da0)
KING_VAR(char, s_home67336c, 67336c)
KING_VAR(char, s_base673374, 673374)
KING_VAR(char, s_setup682d98, 682d98)

static char* View_StrDup(const char* s, int extra)
{
    char* d = (char*)operator new(strlen(s) + 1 + extra);
    memcpy(d, s, strlen(s) + 1);
    return d;
}

// a [RENDER] switch: whether its value (default def) is "on"
static bool View_On(BYTE* self, const char* key, const char* def, char* buf)
{
    GetPrivateProfileStringA(&s_RENDER682e68, key, def, buf, 60, F(const char*, self, 640));
    return !strcmp(buf, &s_on672994);
}

static void View_RenderDouble(BYTE* self, const char* key, double def, double* out)
{
    CallC(FUN_603cb0, &s_RENDER682e68, key, def, out, F(const char*, self, 640));
}

static double View_Bits(DWORD lo, DWORD hi)
{
    DWORD b[2] = {lo, hi};
    return *(double*)b;
}

// an [INSTALL] directory with a backslash appended; false when it is "__noinst"
static bool View_InstallDir(BYTE* self, const char* key, int at, char* buf)
{
    GetPrivateProfileStringA(&s_INSTALL6733c4, key, &s_dot6733ac, buf, 60, F(const char*, self, 640));
    char* d = View_StrDup(buf, 1);
    F(char*, self, at) = d;
    char* e = d + strlen(d);
    if (!strcmp(buf, &s_noinst673358))
        return false;
    e[0] = 92;
    e[1] = 0;
    return true;
}

extern "C" {

// [FUN_5dc4d0..$L_5dd046] the main view's constructor (vtable 652310): reads [ENV] taskdump, debugtime and stereo,
// the [RENDER] settings into the view's flags (+3020) and its render parameters (+472), and the [INSTALL]
// home and base directories; leaves the game when home is "__noinst" or setup is "off"
BYTE* __fastcall MainView_Construct(BYTE* self, void*, int arg)
{
    CallThis(FUN_5d9cf0, self, F(int, g_app, 252), arg, self);
    for (int o = 568; o <= 588; o += 4)
        F(int, self, o) = 0;
    for (int o = 652; o < 700; o += 4)
        F(int, self, o) = 0;
    memcpy(self + 652, &g_fra7025f0, 48);
    CallThis(FUN_5e4af0, self + 704);
    CallThis(FUN_5eb1e0, self + 2720);
    for (int o = 2992; o <= 3016; o += 4)
        F(int, self, o) = 0;
    CallThis(FUN_5b94e0, self + 3096);
    F(void*, self, 0) = g_fra652310;
    F(char*, self, 640) = View_StrDup(F(const char*, g_app, 280), 0);
    const char* ini = F(const char*, self, 640);
    CallThis(FUN_5dd720, self);
    DWORD& flags = F(DWORD, self, 3020);
    if (F(DWORD, g_app, 268) & 256)
        flags |= 256;
    if (F(int, g_app, 264) == 1)
        flags |= 32;

    int v;
    CallC(FUN_603b70, &s_ENV6730a0, &s_taskdump682e9c, 0, &v, ini);
    if (v) {
        g_fwTaskDump = 1;
        F(int, self, 3012) = 1;
        CallThis(FUN_5d2100, &g_mapFile71e808, &s_statedump682e90, 524288);
    }
    CallC(FUN_603b70, &s_ENV6730a0, &s_debugtime682e84, 0, &v, ini);
    F(int, self, 3032) = v;
    char buf[60];
    char* cur = buf;
    GetPrivateProfileStringA(&s_ENV6730a0, &s_stereo682e7c, &s_off6729a8, buf, 60, ini);
    const char* t = CallC<const char*>(FUN_5ab1b0, &cur);
    if (t && !strcmp(t, &s_on672994)) {
        F(double, self, 3064) = 2.0;
        F(double, self, 3072) = 0.0;
        if ((t = CallC<const char*>(FUN_5ab1b0, &cur)) != 0) {
            F(double, self, 3064) = atof(t);
            if ((t = CallC<const char*>(FUN_5ab1b0, &cur)) != 0)
                F(double, self, 3072) = atof(t) * g_stereo64dae0;
        }
    }

    CallC(FUN_603b70, &s_RENDER682e68, &s_skys682e70, 0, F(BYTE*, self, 472) + 80, ini);
    CallC(FUN_603b70, &s_RENDER682e68, &s_showfacemode682e58, 0, &v, ini);
    g_showFaceMode721770 = v;
    View_RenderDouble(self, &s_ambient682e50, View_Bits(858993459u, 1070805811u), (double*)(F(BYTE*, self, 472) + 48));
    CallC(FUN_603b70, &s_RENDER682e68, &s_detail682e48, 100, F(BYTE*, self, 472) + 112, ini);
    if (View_On(self, &s_intensity682e3c, &s_on672994, buf))
        flags |= 64;
    if (View_On(self, &s_intenmap682e30, &s_off6729a8, buf))
        flags |= 16;
    if (View_On(self, &s_alphamir682e24, &s_on672994, buf))
        flags |= 0x1000000;
    if (View_On(self, &s_shadow672650, &s_off6729a8, buf))
        flags |= 8;
    if (!View_On(self, &s_multitex682e18, &s_on672994, buf))
        flags |= 0x2000000;
    if (View_On(self, &s_newkeying682e0c, &s_off6729a8, buf))
        flags |= 0x4000000;
    GetPrivateProfileStringA(&s_RENDER682e68, &s_fog682e08, &s_off6729a8, buf, 60, ini);
    cur = buf;
    t = CallC<const char*>(FUN_5ab1b0, &cur);
    if (t && !strcmp(t, &s_on672994)) {
        flags |= 4;
        BYTE* r = F(BYTE*, self, 472);
        F(double, r, 24) = 304.0;
        F(double, r, 16) = 900.0;
        F(int, r, 72) = 8;
        if ((t = CallC<const char*>(FUN_5ab1b0, &cur)) != 0) {
            F(double, F(BYTE*, self, 472), 24) = atof(t);
            if ((t = CallC<const char*>(FUN_5ab1b0, &cur)) != 0) {
                F(double, F(BYTE*, self, 472), 16) = atof(t);
                if ((t = CallC<const char*>(FUN_5ab1b0, &cur)) != 0)
                    F(int, F(BYTE*, self, 472), 72) = atol(t);
            } else {
                r = F(BYTE*, self, 472);
                F(double, r, 16) = F(double, r, 24) * g_fog64b788;
            }
        }
        if (F(int, g_app, 296) >= 0)
            F(int, g_app, 296) = F(int, F(BYTE*, self, 472), 72);
    }
    View_RenderDouble(self, &s_dcoeff682e00, 1.0, (double*)(F(BYTE*, self, 472) + 120));
    View_RenderDouble(self, &s_farplane682df4, 98304.0, (double*)(F(BYTE*, self, 472) + 32));
    View_RenderDouble(self, &s_nearplane682de8, 1.0, (double*)(F(BYTE*, self, 472) + 40));
    View_RenderDouble(self, &s_skyzb682de0, 0.0, (double*)(F(BYTE*, self, 472) + 96));
    View_RenderDouble(self, &s_skyfun682dd8, 1.0, (double*)(F(BYTE*, self, 472) + 104));
    View_RenderDouble(self, &s_tanglimith682dcc, View_Bits(1412872442u, 1081320076u), &g_tangLimit682a08);
    if (View_On(self, &s_zbuf682dc4, &s_on672994, buf))
        flags |= 0x4000;
    if (View_On(self, &s_antialiasing682db4, &s_off6729a8, buf))
        flags = (flags & ~0x4000u) | 128;
    if (View_On(self, &s_ponly682dac, &s_off6729a8, buf))
        flags |= 2;
    View_RenderDouble(self, &s_xpyramid67e1ac, View_Bits(2576980378u, 1072273817u), (double*)(self + 3048));
    View_RenderDouble(self, &s_ypyramid682da0, View_Bits(858993459u, 1071854387u), (double*)(self + 3056));
    if (!View_InstallDir(self, &s_home67336c, 644, buf))
        CallC(FUN_5d2ef0, CallC<int>(FUN_5d2e50, 1));
    View_InstallDir(self, &s_base673374, 648, buf);
    GetPrivateProfileStringA(&s_INSTALL6733c4, &s_setup682d98, &s_on672994, buf, 60, ini);
    if (!strcmp(buf, &s_off6729a8))
        CallC(FUN_5d2ef0, CallC<int>(FUN_5d2e50, 2));
    g_q77_702e8c = flags;
    return self;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// Palette files

extern "C" int __cdecl getc(FILE* f);
extern "C" void FUN_4cef50(); KING_ALIAS("_FUN_4cef50", "_Effect_Zero")
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")
extern "C" void FUN_5b9520(); KING_ALIAS("_FUN_5b9520", "@TexSets_Free@4")
extern "C" void FUN_5b9740(); KING_ALIAS("_FUN_5b9740", "@Textures_FreeAll@8")
extern "C" void FUN_5d8350(); KING_ALIAS("_FUN_5d8350", "_Dd_BitDepth")
extern "C" void FUN_5b9850(); KING_ALIAS("_FUN_5b9850", "@Fog_BuildTables@16")    // palette: builds the opacity tables (a, b)
extern "C" void FUN_5b9b60(); KING_ALIAS("_FUN_5b9b60", "@Fade_BuildTables@16")    // palette: builds the fog tables (a, b)
extern "C" void FUN_5b9e30(); KING_ALIAS("_FUN_5b9e30", "@Dark_BuildTables@12")    // palette: builds the intensity tables (a)
KING_VAR(char, s_PLM683424, 683424)
KING_VAR(char, s_PALT68341c, 68341c)
KING_VAR(char, s_OPAC683414, 683414)
KING_VAR(char, s_OP1668340c, 68340c)
KING_VAR(char, s_FOG683408, 683408)
KING_VAR(char, s_FO16683400, 683400)
KING_VAR(char, s_INTE6833f8, 6833f8)
KING_VAR(char, s_IN166833f0, 6833f0)

static int Plm_Int(FILE* f) { return CallC<int>(FUN_5ab140, f); }

// a table set of the palette: count at +cnt, pointers at +tab, ids at +tab+4; each table of size bytes, 8-bit
// tables read byte by byte and given an id, 16-bit ones read whole; false when out of memory (count = i + 1)
static bool Plm_Tables(BYTE* pal, FILE* f, int cnt, int tab, int size, bool wide)
{
    int n = F(int, pal, cnt);
    F(BYTE**, pal, tab) = (BYTE**)operator new(n * 4);
    F(WORD*, pal, tab + 4) = (WORD*)operator new(n * 2);
    for (int i = 0; i < F(int, pal, cnt); i++) {
        BYTE* t = (BYTE*)operator new(size);
        F(BYTE**, pal, tab)[i] = t;
        if (wide) {
            F(WORD*, pal, tab + 4)[i] = 0;
            if (!F(BYTE**, pal, tab)[i]) {
                F(int, pal, cnt) = i + 1;
                return false;
            }
            fread(F(BYTE**, pal, tab)[i], size, 1, f);
        } else {
            F(WORD*, pal, tab + 4)[i] = (WORD)CallC<int>(FUN_4cef50, F(BYTE**, pal, tab)[i], size);
            if (!F(BYTE**, pal, tab)[i]) {
                F(int, pal, cnt) = i + 1;
                return false;
            }
            for (int k = 0; k < size; k++)
                F(BYTE**, pal, tab)[i][k] = (BYTE)getc(f);
        }
    }
    return true;
}

extern "C" {

// [FUN_5e6e90] a palette (816 bytes) read from a PLM file: PALT colours, OPAC/OP16 opacity, FOG/FO16 fog and
// INTE/IN16 intensity tables; tables missing or of another bit depth than the screen's are built
BYTE* __cdecl Palette_Load(FILE* f)
{
    char tag[5];
    tag[0] = (char)getc(f);
    tag[1] = (char)getc(f);
    tag[2] = (char)getc(f);
    tag[3] = (char)getc(f);
    tag[4] = 0;
    int c = strcmp(tag, &s_PLM683424);
    if (c)
        return (BYTE*)(c < 0 ? -1 : 1);
    BYTE* pal = (BYTE*)operator new(816);
    if (pal)
        pal = CallThis<BYTE*>(FUN_5b94e0, pal);
    F(int, pal, 796) = 0;
    F(int, pal, 800) = 0;
    F(int, pal, 0) = 0;
    F(int, pal, 8) = 0;
    F(int, pal, 808) = 0;
    int end = Plm_Int(f);
    end += ftell(f);
    while (ftell(f) < end) {
        tag[0] = (char)getc(f);
        tag[1] = (char)getc(f);
        tag[2] = (char)getc(f);
        tag[3] = (char)getc(f);
        int size = Plm_Int(f);
        int padded = (size + 3) & ~3;
        bool ok = true;
        if (!strcmp(tag, &s_PALT68341c)) {
            for (int i = 0; i < size / 3; i++) {
                pal[24 + i * 3] = (BYTE)getc(f);
                pal[25 + i * 3] = (BYTE)getc(f);
                pal[26 + i * 3] = (BYTE)getc(f);
            }
        } else if (!strcmp(tag, &s_OPAC683414) || !strcmp(tag, &s_OP1668340c)) {
            bool wide = strcmp(tag, &s_OPAC683414) != 0;
            F(int, pal, 808) = wide ? 2 : 1;
            F(int, pal, 796) = Plm_Int(f);
            F(int, pal, 792) = Plm_Int(f);
            Plm_Int(f);
            ok = Plm_Tables(pal, f, 796, 0, F(int, pal, 792) << (wide ? 16 : 8), wide);
        } else if (!strcmp(tag, &s_FOG683408) || !strcmp(tag, &s_FO16683400)) {
            bool wide = strcmp(tag, &s_FOG683408) != 0;
            F(int, pal, 808) = wide ? 2 : 1;
            F(int, pal, 812) = Plm_Int(f);
            F(int, pal, 800) = Plm_Int(f);
            Plm_Int(f);
            ok = Plm_Tables(pal, f, 800, 8, wide ? 65536 : 256, wide);
        } else if (!strcmp(tag, &s_INTE6833f8) || !strcmp(tag, &s_IN166833f0)) {
            bool wide = strcmp(tag, &s_INTE6833f8) != 0;
            F(int, pal, 808) = wide ? 2 : 1;
            Plm_Int(f);
            F(int, pal, 804) = Plm_Int(f);
            Plm_Int(f);
            ok = Plm_Tables(pal, f, 804, 16, wide ? 65536 : 256, wide);
        } else {
            for (int i = 0; i < size; i++)
                getc(f);
        }
        if (!ok) {
            if (pal) {
                CallThis(FUN_5b9520, pal);
                operator delete(pal);
            }
            return 0;
        }
        for (int i = padded - size; i; i--)
            getc(f);
    }
    if (F(int, pal, 808) != CallC<int>(FUN_5d8350)) {
        CallThis(FUN_5b9740, pal);
        F(int, pal, 808) = CallC<int>(FUN_5d8350);
        CallThis(FUN_5b9850, pal, 1, 5);
        CallThis(FUN_5b9b60, pal, 8, 4);
        CallThis(FUN_5b9e30, pal, 8);
    } else {
        if (!F(BYTE*, pal, 0))
            CallThis(FUN_5b9850, pal, 1, 5);
        if (!F(BYTE*, pal, 8))
            CallThis(FUN_5b9b60, pal, 32, 4);
        if (!F(BYTE*, pal, 16))
            CallThis(FUN_5b9e30, pal, 32);
    }
    return pal;
}

}  // extern "C"

// === reading a TGA image header (FUN_5e7930)

KING_VAR(AsmLabel, s_tg683484, 683484)
KING_VAR(AsmLabel, s_tg68347c, 68347c)
KING_VAR(AsmLabel, s_tg683460, 683460)
KING_VAR(AsmLabel, s_tg683458, 683458)
KING_VAR(AsmLabel, s_tg683450, 683450)
KING_VAR(AsmLabel, s_tg683430, 683430)
KING_VAR(AsmLabel, s_tg683428, 683428)
extern "C" void FUN_5ab0f0(); KING_ALIAS("_FUN_5ab0f0", "_File_ReadShort")    // File_ReadShort
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")    // File_ReadInt
extern "C" void FUN_5b94e0(); KING_ALIAS("_FUN_5b94e0", "@Palette_Construct@8")
extern "C" void FUN_5bad90(); KING_ALIAS("_FUN_5bad90", "@Texture_SetPixels@20")
extern "C" void FUN_5d8350(); KING_ALIAS("_FUN_5d8350", "_Dd_BitDepth")
extern "C" void FUN_5d2e70(); KING_ALIAS("_FUN_5d2e70", "_Sys_Fatal")
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);

static int Tg_Short(FILE* f)
{
    return CallC<int>(FUN_5ab0f0, f);
}

static int Tg_Int(FILE* f)
{
    return CallC<int>(FUN_5ab140, f);
}

static void Tg_Tag(char* tag, FILE* f)
{
    tag[0] = (char)fgetc(f);
    tag[1] = (char)fgetc(f);
    tag[2] = (char)fgetc(f);
    tag[3] = (char)fgetc(f);
}

extern "C" {

// [FUN_5e7930] reads the header of a TGA image at file offset base of f into texture t: its size, mip levels,
// the pixel format (+64) by the screen depth or the extension chunks (masks, alpha), the palette (+16), the
// pixel offset (+36), and with base the pixels; 1 when done, 0 for an unsupported image
int __cdecl Tga_ReadHeader(BYTE* t, FILE* f, int base)
{
    long start = ftell(f);
    int idLen = fgetc(f);
    int cmType = fgetc(f);
    fgetc(f);
    Tg_Short(f);
    int cmLen = Tg_Short(f);
    int cmDepth = fgetc(f);
    Tg_Short(f);
    Tg_Short(f);
    int width = Tg_Short(f);
    int height = Tg_Short(f);
    int ebx = fgetc(f);                     // the bits per pixel, later reused like the assembly's EBX
    fgetc(f);
    long hdrEnd = ftell(f);
    int alphaOff = 0;
    switch (CallC<int>(FUN_5d8350)) {
    case 1: F(int, t, 64) = 1; break;
    case 2: F(int, t, 64) = 3; break;
    case 3: F(int, t, 64) = (ebx == 32) + 6; break;
    case 4: F(int, t, 64) = 7; break;
    default: F(int, t, 64) = 0; break;
    }
    if (idLen > 0) {
        char tag[4];
        Tg_Tag(tag, f);
        if (strncmp(tag, s_tg683484, 4) == 0) {
            if (Tg_Int(f) != 4)
                return 0;
            ebx = start;
            fseek(f, Tg_Int(f) + start, 0);
            int m5 = 0, m6 = 0;
            for (;;) {
                Tg_Tag(tag, f);
                int size = Tg_Int(f);
                long pos = ftell(f);
                if (strncmp(tag, s_tg68347c, 4) == 0) {
                    F(DWORD, t, 52) |= 16;
                    if (base)
                        CallC(FUN_5d2e70, (const char*)s_tg683460, 0);
                } else if (strncmp(tag, s_tg683458, 4) == 0) {
                    F(DWORD, t, 52) |= 16;
                    F(int, t, 68) = Tg_Int(f) + 1;
                    alphaOff = ftell(f) + 12;
                    F(int, t, 40) = alphaOff - start;
                } else if (strncmp(tag, s_tg683450, 4) == 0) {
                    DWORD r = Tg_Int(f);
                    DWORD g = Tg_Int(f);
                    DWORD b = Tg_Int(f);
                    DWORD a = Tg_Int(f);
                    ebx = g;
                    if (size > 16) {
                        m5 = Tg_Int(f);
                        m6 = Tg_Int(f);
                        Tg_Int(f);
                    }
                    int fmt = 0;
                    if (b == 31) {
                        if (g == 992 && r == 31744 && a == 0)
                            fmt = 3;
                        else if (g == 2016 && r == 63488 && a == 0)
                            fmt = 2;
                    } else if (b == 15) {
                        if (g == 240 && r == 3840 && a == 61440)
                            fmt = 5;
                    } else if (b == 255 && g == 65280 && r == 16711680) {
                        if (a == 0)
                            fmt = 6;
                        else if (a == 0xFF000000)
                            fmt = 7;
                    }
                    if (!fmt && m5 == 65280 && m6 == 255)
                        fmt = 8;
                    if (fmt)
                        F(int, t, 64) = fmt;
                    else
                        CallC(FUN_5d2e70, F(const char*, t, 4), (const char*)s_tg683430);
                } else {
                    strncmp(tag, s_tg683428, 4);
                    break;
                }
                fseek(f, ((size + 3) & ~3) + pos, 0);
            }
        }
        fseek(f, hdrEnd + idLen, 0);
    }
    if (cmType) {
        if (cmDepth != 24)
            return 0;
        ebx = 3;
        if (cmLen > 256)
            return 0;
        if (!F(BYTE*, t, 16)) {
            BYTE* p = (BYTE*)operator new(816);
            F(BYTE*, t, 16) = p ? CallThis<BYTE*>(FUN_5b94e0, p) : 0;
        }
        for (int i = 0; i < cmLen; i++) {
            F(BYTE, F(BYTE*, t, 16), 26 + i * 3) = (BYTE)fgetc(f);
            F(BYTE, F(BYTE*, t, 16), 25 + i * 3) = (BYTE)fgetc(f);
            F(BYTE, F(BYTE*, t, 16), 24 + i * 3) = (BYTE)fgetc(f);
        }
        if FILE_EOF(f)
            return 0;
    }
    F(WORD, t, 20) = (WORD)width;
    F(WORD, t, 22) = (WORD)height;
    short depth = (short)CallC<int>(FUN_5d8350);
    F(short, t, 24) = depth;
    if (depth == 3 && (F(int, t, 64) == 7 || F(int, t, 64) == 5))
        F(short, t, 24) = 4;
    F(int, t, 8) = 0;
    F(int, t, 28) = 0;
    F(short, t, 26) = 0;
    if (width == height) {
        for (int w = width; w > 0; w >>= 1)
            F(short, t, 26)++;
        F(short, t, 26)--;
    }
    int palBytes = ebx * cmLen;
    F(int, t, 36) = palBytes - start + hdrEnd + idLen;
    if (base) {
        CallThis(FUN_5bad90, t, palBytes + hdrEnd + idLen + base, alphaOff + base, 1);
        F(DWORD, t, 52) |= 0x200;
    }
    return 1;
}

} // extern "C"

// === the frame statistics overlay ($L_5d8d10)

KING_VAR(BYTE*, g_stCfg721750, 721750)
KING_VAR(AsmLabel, s_st682b30, 682b30)
KING_VAR(AsmLabel, s_st682b28, 682b28)
KING_VAR(AsmLabel, s_st682b20, 682b20)
KING_VAR(AsmLabel, s_st682b18, 682b18)
KING_VAR(AsmLabel, s_st682aec, 682aec)
KING_VAR(AsmLabel, s_st682ac0, 682ac0)
KING_VAR(AsmLabel, s_st682a94, 682a94)
KING_VAR(AsmLabel, s_st682a58, 682a58)
KING_VAR(AsmLabel, s_st682a48, 682a48)
extern "C" void FUN_5b7ce0(); KING_ALIAS("_FUN_5b7ce0", "_Surface_HLine")

static __declspec(noinline) int St_Int(double d)
{
    return (int)d;
}

extern "C" {

// [$L_5d8d10] draws the frame statistics on view above line y when enabled (config 721750 +292 bits 28-29):
// time bars (scale +368) for the frame parts (+24, +32, +40) and the timers (+496/+504/+512), and the counters
// of the renderer (+464); returns the line above it (y - 100), or y
int __fastcall Stats_Draw(BYTE* self, void*, BYTE* view, int y)
{
    BYTE* cfg = g_stCfg721750;
    if (!(F(DWORD, cfg, 292) & 0x20000000) || !view)
        return y;
    BYTE* font = F(BYTE*, cfg, 272);
    if (!font)
        return y;
    double k = F(double, cfg, 368);
    if ((F(DWORD, cfg, 292) & 0x30000000) != 0x30000000 || !F(BYTE*, self, 464))
        return y;
    BYTE* s = F(BYTE*, view, 12);
#define LINE(yy, x0, x1, c) CallC(FUN_5b7ce0, F(BYTE*, view, 12), yy, x0, x1, c, 0)
    CallC(FUN_5ea170, s, 50, y, font, (const char*)s_st682b30, 5, 0);
    int a = St_Int(k * F(double, self, 40));
    int b = St_Int((F(double, self, 24) - F(double, self, 40) - F(double, self, 32)) * k);
    int c = St_Int(k * F(double, self, 24));
    LINE(y, 100, c + 100, 7);
    LINE(y - 1, 100, c + 100, 7);
    LINE(y - 2, 100, c + 100, 7);
    LINE(y - 3, 100, a + 100, 5);
    LINE(y - 4, 100, a + 100, 5);
    a += 105;
    LINE(y - 3, a, b + 105, 5);
    LINE(y - 4, a, b + 105, 5);
    c += 110;
    b += 110;
    LINE(y - 3, b, c, 5);
    LINE(y - 4, b, c, 5);
    CallC(FUN_5ea170, s, 50, y - 10, font, (const char*)s_st682b28, 5, 0);
    for (int i = 0; i < 3; i++)
        LINE(y - 10 - i, 100, St_Int((F(double, self, 496) - F(double, self, 504)) * k) + 100, 5);
    CallC(FUN_5ea170, F(BYTE*, view, 12), 50, y - 20, font, (const char*)s_st682b20, 5, 0);
    for (int i = 0; i < 3; i++)
        LINE(y - 20 - i, 100, St_Int((F(double, self, 504) - F(double, self, 512)) * k) + 100, 5);
    CallC(FUN_5ea170, F(BYTE*, view, 12), 50, y - 30, font, (const char*)s_st682b18, 5, 0);
    for (int i = 0; i < 3; i++)
        LINE(y - 30 - i, 100, St_Int(F(double, self, 512) * k) + 100, 5);
#undef LINE
    static const AsmLabel* const fmts[3] = {&s_st682aec, &s_st682ac0, &s_st682a94};
    for (int i = 0; i < 3; i++) {
        const int* v = (const int*)(F(BYTE*, self, 464) + 8280 + i * 36);
        char buf[128];
        sprintf(buf, *fmts[i], v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
        CallC(FUN_5ea170, F(BYTE*, view, 12), 50, y - 40 - i * 10, font, buf, 7, 0);
    }
    CallC(FUN_5ea170, F(BYTE*, view, 12), 50, y - 70, font, (const char*)s_st682a58, 7, 0);
    BYTE* r = F(BYTE*, self, 464);
    char buf[128];
    sprintf(buf, s_st682a48, F(int, r, 8428), F(double, r, 8432));
    CallC(FUN_5ea170, F(BYTE*, view, 12), 50, y - 80, font, buf, 7, 0);
    return y - 100;
}

} // extern "C"

// === the view's room render (FUN_5dfa90)

extern "C" void FUN_5d4450(); KING_ALIAS("_FUN_5d4450", "_Dd_WaitFlip")
KING_VAR(float, g_vr721744, 721744)
KING_VAR(AsmLabel, g_vr652538, 652538)

extern "C" {

// [FUN_5dfa90] renders view s's room: waits for the flip, sets the room light (+484), takes the camera matrix,
// places the sky node, sets the projection on a copy of the camera +472 (its switch from the room's camera
// type 15), moves the frame +476 to the scaled sky height, draws the overlay +528; the time goes to 721744
void __fastcall View_RenderRoom(BYTE* s)
{
    BYTE m[48], cam[48], cam2[48];
    memcpy(m, (BYTE*)g_q77_7025f0, 48);
    memcpy(cam2, (BYTE*)g_q77_7025f0, 48);
    memcpy(cam, (BYTE*)g_q77_7025f0, 48);
    CallC(FUN_5d4450);
    BYTE* room = F(BYTE*, s, 488);
    if (!room)
        return;
    if (int l = F(int, s, 484)) {
        const int* c = (const int*)(s + 580);
        Light_Set(F(int, room, 76), c[0], c[1], c[2], (int)(void*)Loader_OnMessage, l);
    }
    BYTE tmp[48];
    memcpy(cam, View_CameraMatrix(s, 0, tmp), 48);
    memcpy(cam2, cam, 48);
    float t0 = (float)CallC<double>(FUN_6037e0);
    CallThis(FUN_5f25c0, F(BYTE*, F(BYTE*, s, 488), 80), m);
    DWORD v[4];
    DWORD a, b;
    CallVirt(s, 96, m, v, &a, &b);
    if (g_q77_702e8c & 0x100)
        CallThis(FUN_5adc70, F(BYTE*, s, 464), F(int, s, 548));
    BYTE* c = F(BYTE*, s, 472);
    BYTE copy[136];
    memcpy(copy, c, 136);
    F(void*, copy, 0) = g_vr652538;
    BYTE* rc = F(BYTE*, F(BYTE*, s, 488), 76);
    if (F(int, rc, 16) == 15 && F(BYTE*, rc, 56) && F(int, F(BYTE*, rc, 56), 24) != -2)
        F(int, copy, 76) = F(int, F(BYTE*, rc, 56), 24);
    if (F(int, c, 76) >= 0) {
        CallThis(FUN_5eaf40, F(BYTE*, s, 468), F(BYTE*, s, 488), v[0], a, b, F(BYTE*, s, 464), copy);
        CallC(FUN_5e1a10, F(BYTE*, s, 480), F(int, copy, 76) + 1);
    } else
        CallC(FUN_5e1a10, F(BYTE*, s, 480), 0);
    BYTE* r = F(BYTE*, s, 464);
    F(int, r, 8480) = F(int, s, 2960);
    F(int, r, 8484) = F(int, s, 2964);
    F(int, r, 8488) = F(int, s, 2968);
    F(int, r, 8492) = F(int, s, 2972);
    c = F(BYTE*, s, 472);
    Fw133_V3 t;
    t.x = F(float, m, 36);
    t.y = F(float, m, 40);
    t.z = (float)((F(float, m, 44) - F(double, c, 96)) * F(double, c, 104) + F(double, c, 96));
    BYTE tm[48];
    CallC(FUN_5e19b0, F(BYTE*, s, 476), 0, CallC<BYTE*>(FUN_5abc40, tm, t));
    if (BYTE* o = F(BYTE*, s, 528))
        CallThis(FUN_5eb310, F(BYTE*, s, 468), F(BYTE*, s, 488), o, F(BYTE*, s, 472), 1, F(BYTE*, s, 464));
    if (BYTE* o = F(BYTE*, s, 528)) {
        CallThis(FUN_5ae0e0, F(BYTE*, s, 464), (BYTE*)g_q77_7025f0, (BYTE*)g_q77_7025f0);
        CallVirt(o, 56, F(int, g_q77_721750, 344) ? 1 : 5, cam2, cam, F(BYTE*, s, 464));
    }
    g_vr721744 = (float)(CallC<double>(FUN_6037e0) - t0);
}

} // extern "C"

// === drawing text in a bitmap font (FUN_5ea1b0)

KING_VAR(AsmLabel, s_ft6835c0, 6835c0)
extern "C" void FUN_5b6700(); KING_ALIAS("_FUN_5b6700", "@Window_RootSurface@8")
extern "C" void FUN_5b6710(); KING_ALIAS("_FUN_5b6710", "@Window_GetRect@12")
extern "C" void FUN_5ba3f0(); KING_ALIAS("_FUN_5ba3f0", "@Palette_Color@12")
extern "C" void FUN_5d3b10(); KING_ALIAS("_FUN_5d3b10", "_Video_BitDepth")
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")

extern "C" {

// [FUN_5ea1b0] draws text with font f on surface s, the baseline at (x, y), within width w (a newline goes
// back to x a line lower): glyphs blitted from the font's surface (+36) on a hardware surface (+116), else
// expanded from its 1-bit rows (+28) in color (raw, or the palette's match on an 8-bit surface)
void __cdecl Font_DrawText(BYTE* s, int x, int y, int w, BYTE* f, const BYTE* text, DWORD color, int raw)
{
    if (!f || !text)
        return;
    int x0 = x, w0 = w;
    int rowBytes = (F(int, f, 24) - 1) / 8 + 1;
    int glyphBytes = rowBytes * F(int, f, 20);
    int bpp = CallC<int>(FUN_5d3b10);
    DWORD c32 = color;
    if (!raw && F(WORD, s, 4) == 1 && bpp != 1) {
        if (!CallThis<BYTE*>(FUN_5b6700, s))
            return;
        c32 = CallThis<DWORD>(FUN_5ba3f0, CallThis<BYTE*>(FUN_5b6700, s), color);
    }
    int rect[4];
    CallThis(FUN_5b6710, s, rect);
    for (; *text; text++) {
        int c = *text;
        int first = F(int, f, 4), last = F(int, f, 8);
        int pre = 0, adv;
        if ((F(BYTE, f, 0) & 1) && c >= first && c <= last) {
            adv = F(BYTE*, f, 12)[c - first];
            pre = F(BYTE*, f, 16)[c - first];
        } else
            adv = F(int, f, 24);
        if (c >= first && c <= last) {
            x -= pre;
            int right = adv + x - 1;
            int h = F(int, f, 20);
            if (x >= 0 && right < F(WORD, s, 0) && y - h + 1 >= 0 && y < F(WORD, s, 2)) {
                if (c == 10) {
                    x = x0;
                    w = w0;
                    y += h;
                    adv = 0;
                } else {
                    if (adv >= w)
                        return;
                    if (F(int, s, 116)) {
                        BYTE* src = F(BYTE*, f, 36);
                        if (!src)
                            return;
                        int dx = x, dy = y;
                        if (F(int, s, 96)) {
                            dx = F(int, s, 100) + x;
                            dy = F(int, s, 104) + y;
                        }
                        int d[4] = {dx, dy - h, right, dy};
                        if (dx >= right)
                            d[2] = dx + 1;
                        DWORD fx[25] = {100};
                        fx[2] = 0xcc0020;
                        int sl = (c & 15) * F(int, f, 24), st = (c >> 4) * (h + 1) + 1;
                        int r[4] = {sl, st, sl + adv - 1, h + st};
                        if (sl >= r[2])
                            r[2] = sl + 1;
                        if (int hr = Com(g_7208fc, 20, d, src, r, 0x1008000, fx))
                            CallC(FUN_5d2ba0, CallC<const char*>(FUN_5d4c10, hr, (const char*)s_ft6835c0), 0);
                    } else {
                        const BYTE* g = F(BYTE*, f, 28) + (c - first) * glyphBytes;
                        BYTE* bits = F(BYTE*, s, 24 + F(int, s, 16) * 4);
                        if (!bits)
                            return;
                        int pitch = F(WORD, s, 6);
                        BYTE* row = bits + bpp * x + pitch * y;
                        const BYTE* gr = g + (h - 1) * rowBytes;
                        for (int i = h; i > 0; i--) {
                            unsigned v = (unsigned)gr[0] << 8 | gr[1];
                            BYTE* p = row;
                            for (int k = adv; k > 0; k--) {
                                if (v & 0x8000) {
                                    if (bpp == 1)
                                        *p = (BYTE)color;
                                    else if (bpp == 2)
                                        *(WORD*)p = (WORD)c32;
                                    else if (bpp == 4)
                                        *(DWORD*)p = c32;
                                }
                                v <<= 1;
                                p += bpp;
                            }
                            gr -= rowBytes;
                            row -= pitch;
                        }
                    }
                }
            }
        }
        x += adv;
        w += pre - adv;
    }
}

} // extern "C"

// === the framework's handlers and render passes (FUN_5de0a0)

extern "C" int __cdecl Weather_LogPos(void*, const BYTE* r, const BYTE* o);  // weather.cpp
extern "C" int __cdecl TreeGen_Draw(void* a0, BYTE* ctx, BYTE* o);  // media.cpp
extern "C" int __cdecl Water_Draw(void*, BYTE* r);
KING_VAR(AsmLabel, s_rp682f14, 682f14)
KING_VAR(AsmLabel, s_rp682f00, 682f00)
KING_VAR(AsmLabel, s_rp682ef0, 682ef0)
KING_VAR(AsmLabel, s_rp682ee0, 682ee0)
KING_VAR(AsmLabel, g_rp652518, 652518)
KING_VAR(DWORD, g_rp702e8c, 702e8c)
extern "C" void FUN_5d2540(); KING_ALIAS("_FUN_5d2540", "@NameHash_Add@16")
extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")

// a render pass object of kind k for world w
static void Rp_Pass(BYTE* w, int k)
{
    BYTE* o = (BYTE*)operator new(40);
    if (o) {
        CallThis(FUN_5d31e0, o, k, (const char*)s_rp682ee0);
        F(void*, o, 0) = g_rp652518;
        F(BYTE*, o, 32) = w;
        F(int, o, 36) = k;
    }
    CallThis(FUN_5d3570, w + 2992, o);
    CallVirt(o, 24, 0);
}

extern "C" {

// [FUN_5de0a0] registers three object name handlers (71e820) and resolves the pending names, then attaches the
// render passes of world w: kinds 400, 0, 20, 30, 10, with hardware flag 0x100 also 21, 31, 11, and 40 when +3044
void __fastcall World_SetupPasses(BYTE* w)
{
    CallThis(FUN_5d2540, (BYTE*)g_objectNames, (const char*)s_rp682f14, (void*)Weather_LogPos);
    CallThis(FUN_5d2540, (BYTE*)g_objectNames, (const char*)s_rp682f00, (void*)TreeGen_Draw);
    CallThis(FUN_5d2540, (BYTE*)g_objectNames, (const char*)s_rp682ef0, (void*)Water_Draw);
    CallThis(FUN_5d27c0, (BYTE*)g_objectNames);
    static const int base[] = {400, 0, 20, 30, 10};
    for (int k : base)
        Rp_Pass(w, k);
    if (g_rp702e8c & 0x100) {
        Rp_Pass(w, 21);
        Rp_Pass(w, 31);
        Rp_Pass(w, 11);
    }
    if (F(int, w, 3044))
        Rp_Pass(w, 40);
}

} // extern "C"

// === walking the scene with a probe point (FUN_5dbf00)

KING_VAR(AsmLabel, g_pw721298, 721298)
KING_VAR(void*, g_pw7212b0, 7212b0)
KING_VAR(void*, g_pw7216bc, 7216bc)
extern "C" void FUN_5ab950(); KING_ALIAS("_FUN_5ab950", "@Vec3_TransformPoint@12")
extern "C" void FUN_5ac2e0(); KING_ALIAS("_FUN_5ac2e0", "_Matrix_Inverse")
extern "C" void FUN_5f88d0(); KING_ALIAS("_FUN_5f88d0", "@Volume_Contains@12")
extern "C" void __cdecl Probe_Walk(BYTE* n);
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")

static void Pw_Children(BYTE* n, int k)
{
    for (BYTE* c = CallVirt<BYTE*>(n, 64, k); c; c = F(BYTE*, c, 8))
        Probe_Walk(c);
}

// the probe point moved into the frame of node f
static void Pw_IntoFrame(BYTE* f)
{
    BYTE m[48], inv[48];
    for (int i = 0; i < 4; i++)
        CallThis(FUN_4bd4d0, m + i * 12);
    memcpy(m, (BYTE*)g_fra7025f0, 48);
    CallThis(FUN_5f25c0, f, m);
    CallThis(FUN_5ab950, (BYTE*)g_pw721298, CallC<BYTE*>(FUN_5ac2e0, inv, m));
}

extern "C" {

// [FUN_5dbf00] walks the scene from node n with the probe point 721298: groups and switches into their children,
// distance LODs by the point (+56), frames (kind 4) and instances (kind 17) with the point in their local
// space; a leaf (kind 9) the point reaches (+64) goes to the callback 7216bc with its object (+48)
void __cdecl Probe_Walk(BYTE* n)
{
    if (!n)
        return;
    DWORD saved[3];
    switch (F(int, n, 16)) {
    case 4:
        memcpy(saved, (BYTE*)g_pw721298, 12);
        if (BYTE* f = F(BYTE*, n, 60))
            Pw_IntoFrame(f);
        Pw_Children(n, 0);
        memcpy((BYTE*)g_pw721298, saved, 12);
        break;
    case 5:
    case 15:
    case 21:
        Pw_Children(n, 0);
        break;
    case 7:
        Pw_Children(n, 0);
        Pw_Children(n, 1);
        break;
    case 8:
        Pw_Children(n, CallThis<int>(FUN_5f88d0, n + 56, (BYTE*)g_pw721298) ? 0 : 1);
        break;
    case 9:
        if (CallThis<int>(FUN_5f88d0, n + 64, (BYTE*)g_pw721298))
            ((void (__cdecl*)(BYTE*, void*))g_pw7216bc)(n + 48, g_pw7212b0);
        break;
    case 17: {
        if (F(BYTE, n, 40) & 8)
            break;
        memcpy(saved, (BYTE*)g_pw721298, 12);
        BYTE* t = F(BYTE*, n, 64);
        if (BYTE* f = F(BYTE*, n, 68))
            Pw_IntoFrame(f);
        Probe_Walk(t);
        memcpy((BYTE*)g_pw721298, saved, 12);
        break;
    }
    case 18: {
        BYTE* s = F(BYTE*, n, 60);
        Pw_Children(n, s ? F(int, s, 56) : F(int, n, 56));
        break;
    }
    }
}

} // extern "C"

// === reading a polygon (FUN_5e69a0)

extern "C" void FUN_5ab4d0(); KING_ALIAS("_FUN_5ab4d0", "_Vec3_Length")
extern "C" void FUN_5ab710(); KING_ALIAS("_FUN_5ab710", "@Vec3_AddAssign@16")
extern "C" void FUN_5ac6d0(); KING_ALIAS("_FUN_5ac6d0", "@Plane_FromPoints@44")
extern "C" void FUN_5cda50(); KING_ALIAS("_FUN_5cda50", "@Shade_Assign@12")
KING_VAR(AsmLabel, g_pg702650, 702650)
struct Pg_V3 { float x, y, z; };

extern "C" {

// [FUN_5e69a0] reads polygon s from file f: its points (+4, count +2), the plane of the first three (+32), the
// centre (+8) and the largest distance of a point from it (+24); 1
int __fastcall Polygon_Read(BYTE* s, void*, void* f)
{
    int n = CallC<int>(FUN_5ab140, f) & 0xFFFF;
    F(WORD, s, 2) = (WORD)n;
    BYTE* p = (BYTE*)operator new(n * 12);
    if (p)
        memset(p, 0, n * 12);
    F(BYTE*, s, 4) = p;
    for (int i = 0; i < F(WORD, s, 2); i++)
        CallThis(FUN_5ab790, F(BYTE*, s, 4) + i * 12, f);
    const Pg_V3* q = F(const Pg_V3*, s, 4);
    BYTE pl[16];
    CallThis(FUN_5cda50, s + 32, CallThis<BYTE*>(FUN_5ac6d0, pl, q[0], q[1], q[2]));
    Pg_V3* c = (Pg_V3*)(s + 8);
    memcpy(c, (BYTE*)g_pg702650, 12);
    for (int i = 0; i < F(WORD, s, 2); i++) {
        Pg_V3 t;
        CallThis(FUN_5ab710, c, &t, F(BYTE*, s, 4) + i * 12);
    }
    double k = F(WORD, s, 2);
    c->x = (float)(c->x / k);
    c->y = (float)(c->y / k);
    c->z = (float)(c->z / k);
    F(double, s, 24) = 0.0;
    for (int i = 0; i < F(WORD, s, 2); i++) {
        const Pg_V3* v = F(const Pg_V3*, s, 4) + i;
        float d[3] = {c->x - v->x, c->y - v->y, c->z - v->z};
        double l = CallC<double>(FUN_5ab4d0, d);
        if (l > F(double, s, 24))
            F(double, s, 24) = l;
    }
    return 1;
}

} // extern "C"

// === a frame as a position and angles (FUN_5de950)

extern "C" void FUN_5ab500(); KING_ALIAS("_FUN_5ab500", "_Vec3_ToAngles")
extern "C" void Fw_RotZ(); KING_ALIAS("_Fw_RotZ", "_Matrix_RotationZ")
extern "C" void Fw_SetThis(); KING_ALIAS("_Fw_SetThis", "@Vec3_SetThis@20")
KING_VAR(Pg_V3, g_fwAhead, 702620)
KING_VAR(Pg_V3, g_fwUp, 702630)
KING_VAR(double, g_fw64da00, 64da00)
KING_VAR(double, g_fw64b6e0, 64b6e0)
KING_VAR(double, g_fw64d900, 64d900)
KING_VAR(float, g_fw64c264, 64c264)

// v turned by the rotation part of m (12 floats)
static Pg_V3 Fw_Rotate(const float* m, const Pg_V3& v)
{
    Pg_V3 r;
    r.x = (float)((double)m[6] * v.z + (double)m[3] * v.y + (double)v.x * m[0]);
    r.y = (float)((double)m[7] * v.z + (double)m[4] * v.y + (double)m[1] * v.x);
    r.z = (float)((double)m[8] * v.z + (double)m[5] * v.y + (double)m[2] * v.x);
    return r;
}

// v moved by matrix r (12 floats)
static void Fw_Move(Pg_V3* o, const float* r, const Pg_V3& v)
{
    o->x = (float)((double)v.z * r[6] + (double)v.y * r[3] + (double)v.x * r[0] + r[9]);
    o->y = (float)((double)v.z * r[7] + (double)v.y * r[4] + (double)v.x * r[1] + r[10]);
    o->z = (float)((double)v.z * r[8] + (double)v.y * r[5] + (double)v.x * r[2] + r[11]);
}

extern "C" {

// [FUN_5de950] frame m (by value) as its position (x, y, z) and its angles: heading and pitch of where it looks
// (each less 64da00) and the roll of its up turned back by them
void __cdecl Matrix_ToAngles(Pg_V3 r0, Pg_V3 r1, Pg_V3 r2, Pg_V3 t, double* x, double* y, double* z, double* head,
                             double* pitch, double* roll)
{
    float m[12];
    memcpy(m, &r0, 12), memcpy(m + 3, &r1, 12), memcpy(m + 6, &r2, 12), memcpy(m + 9, &t, 12);
    *x = m[9];
    *y = m[10];
    *z = m[11];
    float az, po;
    CallC(FUN_5ab500, Fw_Rotate(m, g_fwAhead), &az, &po);
    *pitch = po - g_fw64da00;
    double h = az - g_fw64da00;
    *head = h;
    Pg_V3 u = Fw_Rotate(m, g_fwUp);
    float a[12], b[12];
    Pg_V3 w, v;
    Fw_Move(&w, CallC<float*>(Fw_RotZ, a, (float)-h), u);
    const float* rx = CallC<float*>(FUN_5abee0, b, (float)*pitch);
    Pg_V3 t2;
    Fw_Move(&t2, rx, w);
    CallThis(Fw_SetThis, &v, t2.x, t2.y, t2.z);
    if (v.z < g_fw64b6e0) {
        double r = x87_acos(v.z);
        *roll = r;
        if (v.x < g_fw64c264)
            *roll = -r;
    } else {
        double r = x87_asin(v.x);
        *roll = r;
        if (v.z < g_fw64c264)
            *roll = g_fw64d900 - r;
    }
}

} // extern "C"

// === the main frame window (FUN_5dd0d0)

extern "C" void Mf_FrameCtor(); KING_ALIAS("_Mf_FrameCtor", "@FrameWnd_Construct@4")
extern "C" void Mf_FrameCreate(); KING_ALIAS("_Mf_FrameCreate", "@FrameWnd_Create@40")
extern "C" void Mf_SetWindowPos(); KING_ALIAS("_Mf_SetWindowPos", "@Wnd_SetWindowPos@32")
extern "C" void Mf_LoadString(); KING_ALIAS("_Mf_LoadString", "@String_LoadString@12")
extern "C" void Mf_StringDtor(); KING_ALIAS("_Mf_StringDtor", "@String_Destruct@4")
extern "C" void Mf_ModuleState(); KING_ALIAS("_Mf_ModuleState", "_Afx_GetModuleState@0")
extern "C" void Mf_FrameExtra(); KING_ALIAS("_Mf_FrameExtra", "_Wnd_FrameExtra")
KING_VAR(AsmLabel, g_mfVtbl6523e4, 6523e4)
KING_VAR(const char*, g_mfEmptyString, 685fc0)
KING_VAR(AsmLabel, g_mfRectDefault, 72db68)
KING_VAR(AsmLabel, g_mfWndTop, 72da58)

extern "C" {

// [FUN_5dd0d0] constructor of the main frame window: full screen (a visible popup) with app flag +268 bit 10, else
// a window titled by string 130 with icon 130 sized for the picture (+256 x +260 scaled) plus its frame (unless
// +292 bit 1), shown or hidden as before
BYTE* __fastcall MainFrame_Construct(BYTE* s)
{
    CallThis(Mf_FrameCtor, s);
    F(void*, s, 0) = (BYTE*)g_mfVtbl6523e4;
    BYTE* app = g_fw721750;
    if (F(DWORD, app, 268) & 0x400) {
        CallThis(Mf_FrameCreate, s, (const char*)0, (const char*)0, 0x90000000u, (BYTE*)g_mfRectDefault, 0, 0, 0, 0);
        int h = CallC<int>(FUN_5d3f80) * F(int, g_fw721750, 260);
        int w = CallC<int>(FUN_5d3f70) * F(int, g_fw721750, 256);
        CallThis(Mf_SetWindowPos, s, (BYTE*)g_mfWndTop, 0, 0, w, h, 64);
        SetCursor(0);
        return s;
    }
    const char* title = g_mfEmptyString;
    CallThis(Mf_LoadString, &title, 130);
    CallThis(Mf_FrameCreate, s, (const char*)0, title, 0xCF0000u, (BYTE*)g_mfRectDefault, 0, 0, 0, 0);
    BYTE* ms = CallStd<BYTE*>(Mf_ModuleState);
    SetClassLongA(F(HWND, s, 28), -14, (LONG)LoadIconA(F(HINSTANCE, ms, 12), MAKEINTRESOURCEA(130)));
    int w = CallC<int>(FUN_5d3f70) * F(int, g_fw721750, 256);
    int h = CallC<int>(FUN_5d3f80) * F(int, g_fw721750, 260);
    if (!(F(BYTE, g_fw721750, 292) & 2)) {
        int e2 = CallC<int>(Mf_FrameExtra, 2);
        w = CallC<int>(Mf_FrameExtra, 1) + w + e2;
        int e4 = CallC<int>(Mf_FrameExtra, 4);
        h = CallC<int>(Mf_FrameExtra, 3) + h + e4;
    }
    DWORD f = F(DWORD, g_fw721750, 292);
    F(DWORD, g_fw721750, 292) = f & ~1u;
    CallThis(Mf_SetWindowPos, s, (BYTE*)g_mfWndTop, 0, 0, w, h, 128);
    F(DWORD, g_fw721750, 292) |= f & 1;
    CallThis(Mf_StringDtor, &title);
    return s;
}

} // extern "C"

// === a world frame (5e1750)

extern "C" void __fastcall Queue_Tick(BYTE* self, void*);
extern "C" void __cdecl Device_BeginScene();
extern "C" void __cdecl Device_WaitIdle();
extern "C" void __fastcall Surface_ClearRows(BYTE* self, void*);
extern "C" void __cdecl Device_ProbeMultitexture(BYTE* a, BYTE* b);
extern "C" void __cdecl Sound_Frame();
KING_VAR(double, g_wf71f808, 71f808)
KING_VAR(double, g_wf71f7f8, 71f7f8)
KING_VAR(double, g_wf720020, 720020)
KING_VAR(double, g_wf71f810, 71f810)
KING_VAR(double, g_wf64b658, 64b658)

extern "C" {

// [$L_5e1750] a frame of world s: the frame counters cleared (and the pending time +560 taken), the screen cleared
// and the scene begun; on the first frame the room's view surface set and the multitexture probed with its first
// two lit materials; the world's tasks run; the counters kept (times 64b658) at +496.., then the sound frame
// (with sounds, app +344) or a pending room change (+3024, vtbl+140) unless a load is under way
void __fastcall World_Frame(BYTE* s)
{
    g_wf71f808 = 0;
    g_wf71f7f8 = 0;
    g_wf720020 = 0;
    g_wf71f810 = 0;
    F(int, s, 624) = 0;
    F(int, s, 628) = 0;
    F(int, s, 3036) = 0;
    if (F(int, s, 552)) {
        F(double, s, 2968) = F(double, s, 560);
        F(int, s, 552) = 0;
    }
    Surface_ClearRows(F(BYTE*, s, 12), 0);
    Device_BeginScene();
    if (!F(int, s, 2992)) {
        BYTE* room = F(BYTE*, s, 468);
        CallThis(FUN_5b64e0, F(BYTE*, s, 12), F(BYTE**, room, 100)[F(int, room, 140)]);
        CallC(FUN_5e48f0);
        room = F(BYTE*, s, 468);
        BYTE* m2[2] = {0, 0};
        int k = 0;
        BYTE* m = F(BYTE*, room, 76);
        for (int i = 0; i < F(int, room, 72); i++, m += 124) {
            BYTE* t = F(BYTE*, m, 76);
            if (!(F(BYTE, m, 32) & 1) || !t || !(F(BYTE, t, 52) & 1))
                continue;
            if (!k) {
                m2[0] = m2[1] = m;
                k = 1;
            } else {
                m2[1] = m;
                if (F(int, t, 64) == 16)
                    break;
            }
        }
        Device_ProbeMultitexture(m2[0], m2[1]);
        Window_Show(s, 0, 0);
    }
    Queue_Tick(s + 2992, 0);
    Device_WaitIdle();
    F(double, s, 496) = g_wf71f808 * g_wf64b658;
    F(double, s, 504) = g_wf71f7f8 * g_wf64b658;
    F(double, s, 512) = g_wf720020 * g_wf64b658;
    F(double, s, 624) = g_wf71f810 * g_wf64b658;
    if (F(int, s, 3036)) {
        if (F(int, g_fw721750, 344))
            Sound_Frame();
        return;
    }
    if (F(int, s, 3024) && (!F(BYTE*, s, 484) || !F(int, F(BYTE*, s, 484), 20))) {
        CallVirt(s, 140);
        F(int, s, 3024) = 0;
    }
}

} // extern "C"

// === reading a TGA image with level chunks (FUN_5e8160)

extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
extern "C" void Tga_VideoFlags(); KING_ALIAS("_Tga_VideoFlags", "_Video_Get720898")
KING_VAR(char, s_tgaLoff, 683484)    // "LOFF"
KING_VAR(char, s_tgaLvls, 68347c)    // "LVLS"
KING_VAR(char, s_tgaLvlh, 6834a4)    // "LVLH"
KING_VAR(char, s_tgaLevl, 68349c)    // "LEVL"
KING_VAR(char, s_tgaLvmp, 683458)    // "LVMP"

// the next four bytes of f
static void Tga_Tag(FILE* f, char* tag)
{
    for (int i = 0; i < 4; i++)
        tag[i] = (char)fgetc(f);
}

static int Tga_Int(FILE* f) { return CallC<int>(FUN_5ab140, f); }
static unsigned Tga_Short(FILE* f) { return CallC<unsigned>(FUN_5ab0f0, f); }

// palette entry c (bytes +24, +25, +26 of the palette record) as a 16-bit pixel: 565 or 555
static WORD Tga_PalPixel(BYTE* img, int c, bool w565)
{
    BYTE* p = F(BYTE*, img, 16) + c * 3 + 24;
    if (w565)
        return (WORD)(((((p[0] & 0xF8) << 5) | (p[1] & 0xFC)) << 3) | (p[2] >> 3));
    return (WORD)(((((p[0] & 0xF8) << 5) | (p[1] & 0xF8)) << 2) | (p[2] >> 3));
}

extern "C" {

// [FUN_5e8160] reads the TGA image at offset off of f into img (pixels +8, levels +12, palette +16, bytes per
// pixel +24, flags +52, format +64): an image id holding a "LOFF" chunk points to "LVLS" mip levels or an
// "LVMP" level block (flag 16); then the colour-mapped (type 1) or true-colour (type 2) pixels, converted to the
// image's depth (flag 1); 0 on failure (7218bc = 3 for an unsupported depth)
int __cdecl Tga_Read(BYTE* img, FILE* f, long off)
{
    fseek(f, off, 0);
    int idLen = fgetc(f);
    int cmapType = fgetc(f);
    int type = fgetc(f);
    unsigned cmapFirst = Tga_Short(f);
    unsigned cmapLen = Tga_Short(f);
    int cmapBits = fgetc(f);
    Tga_Short(f);
    Tga_Short(f);
    unsigned width = Tga_Short(f);
    unsigned height = Tga_Short(f);
    int bpp = fgetc(f);
    int desc = fgetc(f);
    long hdrEnd = ftell(f);
    WORD& depth = F(WORD, img, 24);
    DWORD& flags = F(DWORD, img, 52);
    int t32 = 0;    // a stack slot shared with the 16-bit conversion's stale components
    char tag[4];
    if (idLen > 0) {
        flags &= ~16;
        Tga_Tag(f, tag);
        if (!strncmp(tag, &s_tgaLoff, 4)) {
            if (Tga_Int(f) != 4)
                return 0;
            fseek(f, Tga_Int(f) + off, 0);
            Tga_Tag(f, tag);
            if (!strncmp(tag, &s_tgaLvls, 4)) {
                int size = Tga_Int(f);
                long end = size + ftell(f);
                int levels = -1;
                while (end > ftell(f)) {
                    Tga_Tag(f, tag);
                    int len = Tga_Int(f);
                    long pos = ftell(f);
                    if (!strncmp(tag, &s_tgaLvlh, 4)) {
                        Tga_Int(f);
                        levels = Tga_Int(f);
                    } else if (!strncmp(tag, &s_tgaLevl, 4)) {
                        if (levels == -1)
                            return 0;
                        if (!Texture_ReadChunks(F(BYTE*, img, 12), f, depth, len, F(BYTE*, img, 16) + 24, bpp / 8))
                            return 0;
                    }
                    fseek(f, ((len + 3) & ~3) + pos, 0);
                }
                flags |= 16;
            } else if (!strncmp(tag, &s_tgaLvmp, 4)) {
                flags |= 16;
                int size = Tga_Int(f);
                Tga_Int(f);
                int side = Tga_Int(f);
                Tga_Int(f);
                int c = t32 = Tga_Int(f);
                if (depth == c)
                    fread(F(void*, img, 12), size - 16, 1, f);
                else {
                    int n = 0;
                    for (; side > 0; side >>= 1)
                        n += side * side;
                    for (int i = 0; i < n; i++) {
                        if (depth == 2) {
                            if (c != 1)
                                return 0;
                            int ci = fgetc(f);
                            if (ci < (int)cmapFirst || ci > (int)(cmapLen + cmapFirst - 1))
                                return 0;
                            F(WORD*, img, 12)[i] = Tga_PalPixel(img, ci, false);
                        } else if (depth == 4) {
                            if (c != 1)
                                return 0;
                            int ci = fgetc(f);
                            BYTE* p = F(BYTE*, img, 16) + ci * 3;
                            BYTE* o = F(BYTE*, img, 12) + i * 4;
                            o[0] = p[26];
                            o[1] = p[25];
                            o[2] = p[24];
                            o[3] = 0;
                        }
                    }
                }
            }
        }
    }
    fseek(f, hdrEnd + idLen, 0);
    if (cmapType) {
        if (cmapBits != 24)
            return 0;
        fseek(f, cmapLen * 3, 1);
    }
    bool down = (desc & 32) != 0;
    if (type == 1) {
        if (depth == 1) {
            for (unsigned y = 0; y < height; y++)
                fread(F(BYTE*, img, 8) + (down ? y : height - y - 1) * width, width, 1, f);
        } else if (depth == 2) {
            F(int, img, 64) = (CallC<BYTE>(Tga_VideoFlags) & 4) ? 3 : 2;
            for (unsigned y = 0; y < height; y++)
                for (unsigned x = 0; x < width; x++) {
                    unsigned ci = fgetc(f);
                    if (ci < cmapFirst || ci > cmapLen + cmapFirst - 1)
                        return 0;
                    unsigned i = (down ? y : height - y - 1) * width + x;
                    ((WORD*)F(BYTE*, img, 8))[i] = Tga_PalPixel(img, ci, F(int, img, 64) == 2);
                }
        } else if (depth == 3 || depth == 4) {
            for (unsigned y = 0; y < height; y++)
                for (unsigned x = 0; x < width; x++) {
                    unsigned ci = fgetc(f);
                    if (ci < cmapFirst || ci > cmapLen + cmapFirst - 1)
                        return 0;
                    unsigned i = (down ? y : height - y - 1) * width + x;
                    BYTE* p = F(BYTE*, img, 16) + ci * 3;
                    BYTE* o = F(BYTE*, img, 8) + depth * i;
                    o[0] = p[26];
                    o[1] = p[25];
                    o[2] = p[24];
                    if (depth == 4)
                        F(BYTE*, img, 8)[i * 4 + 3] = 255;
                }
        } else
            return 0;
    } else if (type == 2) {
        if (bpp == 32) {
            if (depth != 4) {
                g_7218bc = 3;
                return 0;
            }
            for (unsigned y = 0; y < height; y++)
                for (unsigned x = 0; x < width; x++) {
                    unsigned i = (down ? y : height - y - 1) * width + x;
                    for (int k = 0; k < 4; k++)
                        F(BYTE*, img, 8)[depth * i + k] = (BYTE)getc(f);
                }
        } else if (bpp == 24) {
            if (depth != 3 && depth != 4) {
                g_7218bc = 3;
                return 0;
            }
            for (unsigned y = 0; y < height; y++)
                for (unsigned x = 0; x < width; x++) {
                    unsigned i = (down ? y : height - y - 1) * width + x;
                    F(BYTE*, img, 8)[depth * i + 2] = (BYTE)getc(f);
                    F(BYTE*, img, 8)[depth * i + 1] = (BYTE)getc(f);
                    F(BYTE*, img, 8)[depth * i] = (BYTE)getc(f);
                    if (depth == 4)
                        F(BYTE*, img, 8)[i * 4 + 3] = 0;
                }
        } else if (bpp == 16) {
            if (depth == 2) {
                for (unsigned y = 0; y < height; y++)
                    for (unsigned x = 0; x < width; x++) {
                        WORD v = (WORD)Tga_Short(f);
                        ((WORD*)F(BYTE*, img, 8))[(down ? y : height - y - 1) * width + x] = v;
                    }
            } else if (depth == 3 || depth == 4) {
                // the components persist across pixels as in the original (an unknown format keeps the last)
                unsigned b = 0, g = cmapLen, r = t32, a = type;
                for (unsigned y = 0; y < height; y++)
                    for (unsigned x = 0; x < width; x++) {
                        unsigned i = (down ? y : height - y - 1) * width + x;
                        unsigned v = Tga_Short(f);
                        switch (F(int, img, 64)) {
                        case 2:
                            b = (v & 31) << 3, g = (v >> 3) & 252, r = (v >> 8) & 248;
                            break;
                        case 3:
                            b = (v & 31) << 3, g = (v >> 2) & 248, r = (v >> 7) & 248;
                            break;
                        case 5:
                            b = (v & 15) << 4, g = v & 240, r = (v >> 4) & 240, a = (v >> 8) & 240;
                            break;
                        }
                        F(BYTE*, img, 8)[depth * i + 2] = (BYTE)b;
                        F(BYTE*, img, 8)[depth * i + 1] = (BYTE)g;
                        F(BYTE*, img, 8)[depth * i] = (BYTE)r;
                        if (depth == 4)
                            F(BYTE*, img, 8)[i * 4 + 3] = (BYTE)a;
                    }
                F(int, img, 64) = depth == 4 ? 7 : 6;
            } else {
                g_7218bc = 3;
                return 0;
            }
        } else
            return 0;
    } else
        return 0;
    flags |= 1;
    return 1;
}

} // extern "C"

// === reading a face (FUN_5e58b0)

extern "C" int __cdecl atexit(void (__cdecl* f)());
KING_VAR(BYTE, g_fc7217b4, 7217b4)
KING_VAR(float, g_fc7217a8, 7217a8)
KING_VAR(float, g_fc7217ac, 7217ac)
KING_VAR(float, g_fc7217b0, 7217b0)
KING_VAR(int, g_fc72006c, 72006c)
KING_VAR(double, g_fc64b670, 64b670)
KING_VAR(double, g_fc64b6e0, 64b6e0)
KING_VAR(DWORD, g_fc702e8c, 702e8c)
extern "C" void Fc_RotX(); KING_ALIAS("_Fc_RotX", "_Matrix_RotationX")

static void __cdecl Fc_Nop() {}

extern "C" {

// [FUN_5e58b0] reads face s from file f with the model's points (28 bytes each) and normals (12): the flags (+0,
// stored ^ 1; +6 the texture layers), +28, +50, +48, the vertex count (+4) and its number (+2, from 72006c); per
// vertex its point index (+8), with flag 2 the layers' u and v (+12, +16), with flag 16 a normal (flag 1, flag
// 32 read, else the point's; kept as vectors +24 with 702e8c bit 6, else as light +20 from the fixed light
// direction 7217a8) or a read light (flag 32); then the plane of the first three points (+32); 1
int __fastcall Face_Read(BYTE* s, void*, void* f, BYTE* pts, BYTE* normals)
{
    Pg_V3 nrm = {0, 0, 0};
    if (!(g_fc7217b4 & 1)) {
        g_fc7217b4 |= 1;
        BYTE mx[48], mz[48];
        const float* a = CallC<const float*>(Fc_RotX, mx, 0x3F48F5DA);
        float v[3] = {a[9] + a[3], a[10] + a[4], a[11] + a[5]};
        const float* m = CallC<const float*>(Fw_RotZ, mz, 0x3F060A92);
        float y = v[2] * m[7] + v[1] * m[4] + v[0] * m[1] + m[10];
        g_fc7217b0 = v[2] * m[8] + v[1] * m[5] + v[0] * m[2] + m[11];
        g_fc7217a8 = v[2] * m[6] + v[1] * m[3] + v[0] * m[0] + m[9];
        g_fc7217ac = y;
        atexit(Fc_Nop);
    }
    DWORD fl = CallC<DWORD>(FUN_5ab140, f) ^ 1;
    F(WORD, s, 0) = (WORD)fl;
    F(WORD, s, 6) = (BYTE)(fl >> 8);
    F(float, s, 28) = CallC<float>(FUN_5ab190, f);
    F(WORD, s, 50) = (WORD)CallC<int>(FUN_5ab140, f);
    F(WORD, s, 48) = (WORD)CallC<int>(FUN_5ab140, f);
    F(WORD, s, 4) = (WORD)CallC<int>(FUN_5ab140, f);
    F(WORD, s, 2) = (WORD)g_fc72006c;
    g_fc72006c++;
    unsigned n = F(WORD, s, 4);
    F(void*, s, 8) = operator new(n * 2);
    if (F(BYTE, s, 0) & 2) {
        F(WORD, s, 6)++;
        F(void*, s, 12) = operator new(F(WORD, s, 6) * F(WORD, s, 4) * 4);
        F(void*, s, 16) = operator new(F(WORD, s, 6) * F(WORD, s, 4) * 4);
    }
    WORD w = F(WORD, s, 0);
    if (w & 16) {
        if ((w & 1) && (g_fc702e8c & 64)) {
            Pg_V3* p = (Pg_V3*)operator new(n * 12);
            if (p)
                memset(p, 0, n * 12);
            F(Pg_V3*, s, 24) = p;
        } else
            F(void*, s, 20) = operator new(F(WORD, s, 4) * 4);
    }
    for (int i = 0; i < F(WORD, s, 4); i++) {
        F(WORD*, s, 8)[i] = (WORD)CallC<int>(FUN_5ab140, f);
        if (F(BYTE, s, 0) & 2)
            for (int j = 0; j < F(WORD, s, 6); j++) {
                // as in the original: the layers' values land at i + 2 j
                F(float*, s, 12)[i + 2 * j] = CallC<float>(FUN_5ab190, f);
                F(float*, s, 16)[i + 2 * j] = CallC<float>(FUN_5ab190, f);
            }
        w = F(WORD, s, 0);
        if (!(w & 16))
            continue;
        if (w & 1) {
            if (w & 32)
                CallThis(FUN_5ab790, &nrm, f);
            else
                nrm = *(const Pg_V3*)(normals + F(WORD*, s, 8)[i] * 12);
            if (g_fc702e8c & 64)
                F(Pg_V3*, s, 24)[i] = nrm;
            else
                F(float*, s, 20)[i] = (float)((nrm.z * g_fc7217b0 + nrm.y * g_fc7217ac + g_fc7217a8 * nrm.x
                    + g_fc64b670) * g_fc64b6e0);
        } else if (w & 32)
            F(float*, s, 20)[i] = CallC<float>(FUN_5ab190, f);
    }
    const WORD* ix = F(const WORD*, s, 8);
    BYTE pl[16];
    CallThis(FUN_5cda50, s + 32, CallThis<BYTE*>(FUN_5ac6d0, pl, *(const Pg_V3*)(pts + ix[0] * 28),
        *(const Pg_V3*)(pts + ix[1] * 28), *(const Pg_V3*)(pts + ix[2] * 28)));
    return 1;
}

} // extern "C"

// === reading a flat face (FUN_5e6650)

extern "C" {

// [FUN_5e6650] reads flat face s from f: the flags (+0, +6 the layers), 4 skipped bytes, +34, +32, the vertex count
// (+4) and number (+2, from 72006c); per vertex x and y (+16, +20) and with flag 2 the layers' u and v (+8, +12, at
// i + 2 j as in the original); with flag 2 the texture scale +28 = |xy edge 0-1| / |uv edge 0-1|; 1
int __fastcall FlatFace_Read(BYTE* s, void*, FILE* f)
{
    DWORD fl = CallC<DWORD>(FUN_5ab140, f);
    F(WORD, s, 0) = (WORD)fl;
    F(WORD, s, 6) = (BYTE)(fl >> 8);
    DWORD skip;
    fread(&skip, 4, 1, f);
    F(WORD, s, 34) = (WORD)CallC<int>(FUN_5ab140, f);
    F(WORD, s, 32) = (WORD)CallC<int>(FUN_5ab140, f);
    F(WORD, s, 4) = (WORD)CallC<int>(FUN_5ab140, f);
    F(WORD, s, 2) = (WORD)g_fc72006c;
    g_fc72006c++;
    F(void*, s, 16) = operator new(F(WORD, s, 4) * 4);
    F(void*, s, 20) = operator new(F(WORD, s, 4) * 4);
    if (F(BYTE, s, 0) & 2) {
        F(WORD, s, 6)++;
        F(void*, s, 8) = operator new(F(WORD, s, 6) * F(WORD, s, 4) * 4);
        F(void*, s, 12) = operator new(F(WORD, s, 6) * F(WORD, s, 4) * 4);
    }
    for (int i = 0; i < F(WORD, s, 4); i++) {
        F(float*, s, 16)[i] = CallC<float>(FUN_5ab190, f);
        F(float*, s, 20)[i] = CallC<float>(FUN_5ab190, f);
        if (F(BYTE, s, 0) & 2)
            for (int j = 0; j < F(WORD, s, 6); j++) {
                F(float*, s, 8)[i + 2 * j] = CallC<float>(FUN_5ab190, f);
                F(float*, s, 12)[i + 2 * j] = CallC<float>(FUN_5ab190, f);
            }
    }
    if (F(BYTE, s, 0) & 2) {
        const float* u = F(float*, s, 8);
        const float* v = F(float*, s, 12);
        double du = (double)u[1] - u[0], dv = (double)v[1] - v[0];
        double l1 = x87_sqrt(dv * dv + du * du);
        F(float, s, 28) = (float)l1;
        const float* x = F(float*, s, 16);
        const float* y = F(float*, s, 20);
        double dy = (double)y[1] - y[0], dx = (double)x[1] - x[0];
        F(float, s, 28) = (float)(x87_sqrt(dx * dx + dy * dy) / l1);
    }
    return 1;
}

} // extern "C"

// === a camera following a frame with lag (FUN_5df3d0)

KING_VAR(double, g_cf720c80, 720c80)
KING_VAR(double, g_cf7212a8, 7212a8)
KING_VAR(double, g_cf720b58, 720b58)
KING_VAR(double, g_cf64d900, 64d900)
KING_VAR(double, g_cf64d908, 64d908)
KING_VAR(double, g_cf64d910, 64d910)
KING_VAR(double, g_cf682a20, 682a20)

// keeps *lag within lim of a
static void Cf_Clamp(double a, double* lag, double lim)
{
    if (a - *lag > lim)
        *lag = a - lim;
    if (-lim > a - *lag)
        *lag = lim + a;
}

extern "C" {

// [FUN_5df3d0] camera s follows frame m: its position (+2904..), the frame's angles (720c80, 7212a8, 720b58) and
// the lagging angles +2928 (the heading difference wrapped), +2936 and +2944 kept within 682a20 of them; +580 the
// frame's translation
void __fastcall Camera_Follow(BYTE* s, void*, const float* m)
{
    Pg_V3 r[4];
    memcpy(r, m, 48);
    CallC(FUN_5de950, r[0], r[1], r[2], r[3], (double*)(s + 2904), (double*)(s + 2912), (double*)(s + 2920),
        &g_cf720c80, &g_cf7212a8, &g_cf720b58);
    double lim = g_cf682a20;
    double d = g_cf720c80 - F(double, s, 2928);
    if (d > g_cf64d900)
        d -= g_cf64d908;
    if (d < g_cf64d910)
        d += g_cf64d908;
    if (d > lim)
        F(double, s, 2928) = g_cf720c80 - lim;
    if (d < -lim)
        F(double, s, 2928) = lim + g_cf720c80;
    Cf_Clamp(g_cf7212a8, (double*)(s + 2936), lim);
    Cf_Clamp(g_cf720b58, (double*)(s + 2944), lim);
    memcpy(s + 580, m + 9, 12);
}

} // extern "C"

// === a window over part of a parent's surface (FUN_5d8710)

extern "C" void Ww_SubSurface(); KING_ALIAS("_Ww_SubSurface", "@SubSurface_Construct@20")

extern "C" {

// [FUN_5d8710] window s (vtable 652168, state 3) on parent p's surface (+12): a 124-byte sub-surface over rectangle
// r, or over the whole surface (its size, state | 0xf0) when r is empty; flag 0x4000 passed on
BYTE* __fastcall Window_ConstructSub(BYTE* s, void*, BYTE* p, const int* r, int flags)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_fra652168;
    F(int, s, 52) = 0;
    F(int, s, 48) = 0;
    F(int, s, 16) = 3;
    int rc[4] = {r[0], r[1], r[2], r[3]};
    if (r[0] == 0 && r[2] == 0 && r[1] == 0)
    {
        BYTE* surf = F(BYTE*, p, 12);
        rc[0] = 0;
        rc[1] = 0;
        rc[2] = F(WORD, surf, 0);
        rc[3] = F(WORD, surf, 2);
        BYTE* n = (BYTE*)operator new(124);
        F(BYTE*, s, 12) = n ? CallThis<BYTE*>(Ww_SubSurface, n, F(BYTE*, p, 12), rc, flags & 0x4000) : 0;
        F(int, s, 16) |= 0xf0;
    }
    else
    {
        BYTE* n = (BYTE*)operator new(124);
        F(BYTE*, s, 12) = n ? CallThis<BYTE*>(Ww_SubSurface, n, F(BYTE*, p, 12), rc, flags & 0x4000) : 0;
    }
    F(int, s, 24) = 0;
    F(int, s, 28) = 0;
    return s;
}

} // extern "C"

// === queueing an event (a method at $L_5dbe10)

extern "C" {

// [$L_5dbe10] s's ring of 100 events (+56, read +456, write +460) takes v unless full
void __fastcall EventRing_Push(BYTE* s, void*, int v)
{
    int head = F(int, s, 456), tail = F(int, s, 460);
    if (tail == head - 1)
        return;
    if (tail == 99 && head == 0)
        return;
    F(int, s, 56 + tail * 4) = v;
    if (++F(int, s, 460) >= 100)
        F(int, s, 460) = 0;
}

} // extern "C"

// === the debug window's input (FUN_5e05c0)

extern "C" int __cdecl _mkdir(const char*);
extern "C" void __cdecl Sys_Log(const char* text, int value);
KING_VAR(BYTE*, g_di721750, 721750)
KING_VAR(int, g_di721758, 721758)                     // the screenshot number
KING_VAR(int, g_di72175c, 72175c)                     // the camera dump number
KING_VAR(int, g_di721760, 721760)
KING_VAR(int, g_di721764, 721764)
KING_VAR(float, g_di721744, 721744)
KING_VAR(float, g_di721210, 721210)
KING_VAR(float, g_di72174c, 72174c)
KING_VAR(float, g_di721748, 721748)
KING_VAR(int, g_di72006c, 72006c)
KING_VAR(int, g_di720070, 720070)
KING_VAR(int, g_di720074, 720074)
KING_VAR(int, g_di720078, 720078)
KING_VAR(int, g_di72007c, 72007c)
KING_VAR(int, g_di702e90, 702e90)
KING_VAR(int, g_di702e9c, 702e9c)
KING_VAR(int, g_di720720, 720720)
KING_VAR(char, g_diText, 7212bc)                      // the stats text
KING_VAR(char, g_diLine, 720e10)
KING_VAR(double, g_di64b5a0, 64b5a0)
KING_VAR(double, g_di64d0f0, 64d0f0)
KING_VAR(double, g_di64dd78, 64dd78)
KING_VAR(double, g_di64d338, 64d338)
KING_VAR(double, g_di64d378, 64d378)
KING_VAR(double, g_di652550, 652550)
KING_VAR(double, g_di64b730, 64b730)
KING_VAR(double, g_di64f3c0, 64f3c0)
KING_VAR(char, g_di6830dc, 6830dc)
KING_VAR(char, g_di6830d8, 6830d8)
KING_VAR(char, g_di6830c8, 6830c8)
KING_VAR(char, g_di6703e4, 6703e4)
KING_VAR(char, g_di67fb80, 67fb80)
KING_VAR(char, g_di682f34, 682f34)
KING_VAR(char, g_di6830c4, 6830c4)
KING_VAR(char, g_di682f24, 682f24)
KING_VAR(char, g_di6830b0, 6830b0)
KING_VAR(char, g_di68309c, 68309c)
KING_VAR(char, g_di683084, 683084)
KING_VAR(char, g_di68306c, 68306c)
KING_VAR(char, g_di683058, 683058)
KING_VAR(char, g_di683044, 683044)
KING_VAR(char, g_di683030, 683030)
KING_VAR(char, g_di68301c, 68301c)
KING_VAR(char, g_di683004, 683004)
KING_VAR(char, g_di682fe4, 682fe4)
KING_VAR(char, g_di682fd4, 682fd4)
KING_VAR(char, g_di682fc4, 682fc4)
KING_VAR(char, g_di682fb4, 682fb4)
KING_VAR(char, g_di682fa0, 682fa0)
KING_VAR(char, g_di682f94, 682f94)
KING_VAR(char, g_di682f84, 682f84)
KING_VAR(char, g_di682f78, 682f78)
KING_VAR(char, g_di682f64, 682f64)
KING_VAR(char, g_di682f58, 682f58)

// the next line of the stats text
static void Di_Add()
{
    strcat(&g_diText, &g_diLine);
}

// the mouse's offset from a 40 pixel dead zone around the middle of size n (window size w), 0 inside it
static int Di_Off(int p, int half, int w, int* side)
{
    *side = 0;
    if (p < half - 40)
    {
        *side = -1;
        return w / 2 - p - 40;
    }
    if (p > half + 40)
    {
        *side = 1;
        return p - w / 2 - 40;
    }
    return 0;
}

extern "C" {

// [FUN_5e05c0] debug window s's event ev: buttons 513 / 514 and 516 / 517 set and clear bits 1 and 2 of +2988,
// 519 cycles the mouse mode (+2984); mouse moves (512) turn the view (+2872 / +2880) by the offset from the middle
// (mode 1) or past a dead zone; keys: 8 toggles bit 8, 32 sets bit 4, 27 / 71 / 103 call +2856, 107 / 109 zoom,
// 113-115 set the mouse mode, 116 saves a screenshot, 117 shows the stats, 118 / 120 toggle flags, 119 dumps the
// camera
int __fastcall Dbg_OnInput(BYTE* s, void*, const short* ev)
{
    int t = ev[0];
    switch (t)
    {
    case 513:
        return F(int, s, 2988) |= 1;
    case 514:
        return F(int, s, 2988) &= ~1;
    case 516:
        return F(int, s, 2988) |= 2;
    case 517:
        return F(int, s, 2988) &= ~2;
    case 519:
    {
        int v = F(int, s, 2984) + 1;
        F(int, s, 2984) = v % 3;
        return v / 3;
    }
    case 512:
    {
        int x = ev[1], y = ev[2];
        BYTE* sf = F(BYTE*, s, 12);
        if (F(int, s, 2984) == 1)
        {
            int dx = (int)(F(WORD, sf, 0) >> 1) - x;
            F(double, s, 2872) = dx * g_di64b5a0 * g_di64d0f0;
            int dy = y - (int)(F(WORD, sf, 2) >> 1);
            F(double, s, 2880) = dy * g_di64d338;
            return (int)sf;
        }
        int side, half = F(WORD, sf, 0) >> 1;
        int dx = side = 0;
        if (x < half - 40 || x > half + 40)
            dx = Di_Off(x, half, CallVirt<int>(s, 20), &side);
        if (side < 0)
            F(double, s, 2872) = dx * g_di64b5a0 * g_di64d0f0;
        else if (side > 0)
            F(double, s, 2872) = dx * g_di64b5a0 * g_di64dd78;
        else
            F(double, s, 2872) = 0;
        half = F(WORD, sf, 2) >> 1;
        int dy = 0;
        side = 0;
        if (y < half - 40 || y > half + 40)
            dy = Di_Off(y, half, CallVirt<int>(s, 24), &side);
        if (side < 0)
            F(double, s, 2880) = dy * g_di64d338;
        else if (side > 0)
            F(double, s, 2880) = -(dy * g_di64d338);
        else
            F(double, s, 2880) = 0;
        return half;
    }
    case 256:
        break;
    default:
        return t - (t > 514 ? 519 : 513);
    }
    int k = ev[3];
    switch (k)
    {
    case 8:
        F(int, s, 2988) ^= 8;
        break;
    case 27:
    case 71:
    case 103:
        if (F(BYTE*, s, 2856))
            return CallVirt<int>(F(BYTE*, s, 2856), 36);
        break;
    case 32:
        return F(int, s, 2988) |= 4;
    case 107:
    case 109:
    {
        BYTE* v = F(BYTE*, s, 488);
        double z = k == 107 ? g_di64d378 : g_di652550;
        F(float, v, 88) = (float)(F(float, v, 88) * z);
        F(float, v, 84) = (float)(F(float, v, 84) * z);
        return (int)v;
    }
    case 113:
    case 114:
    case 115:
        F(int, s, 2984) = k - 113;
        break;
    case 116:
    {
        // a screenshot
        Clock_Start(s, 0);
        if (g_di721758 < 9999)
        {
            char dir[64], path[96];
            sprintf(dir, &g_di6830d8, &g_di6830dc);
            _mkdir(&g_di6830dc);
            sprintf(path, &g_di6830c8, dir, g_di721758);
            FILE* f = fopen(path, &g_di6703e4);
            Tga_Write(F(BYTE*, s, 12), f);
            fclose(f);
            g_di721758++;
        }
        Clock_Stop(s, 0);
        return 0;
    }
    case 117:
    {
        // the stats
        sprintf(&g_diText, &g_di6830b0, F(double, s, 2976) * g_di64b730);
        sprintf(&g_diLine, &g_di68309c, F(double, s, 496));
        Di_Add();
        sprintf(&g_diLine, &g_di683084, F(double, s, 504));
        Di_Add();
        sprintf(&g_diLine, &g_di68306c, F(double, s, 512));
        Di_Add();
        sprintf(&g_diLine, &g_di683058, (double)g_di721744);
        Di_Add();
        sprintf(&g_diLine, &g_di683044, (double)g_di721210);
        Di_Add();
        sprintf(&g_diLine, &g_di683030, (double)g_di72174c);
        Di_Add();
        sprintf(&g_diLine, &g_di68301c, (double)g_di721748);
        Di_Add();
        sprintf(&g_diLine, &g_di683004, F(double, s, 624));
        Di_Add();
        sprintf(&g_diLine, &g_di682fe4, g_di72006c, g_di720070, g_di720074);
        Di_Add();
        sprintf(&g_diLine, &g_di682fd4, g_di720078);
        Di_Add();
        sprintf(&g_diLine, &g_di682fc4, g_di702e90);
        Di_Add();
        sprintf(&g_diLine, &g_di682fb4, g_di72007c);
        Di_Add();
        sprintf(&g_diLine, &g_di682fa0, F(int, s, 2904), F(int, s, 2908), F(int, s, 2912), F(int, s, 2916),
                F(int, s, 2920), F(int, s, 2924));
        Di_Add();
        sprintf(&g_diLine, &g_di682f94, F(double, s, 2928) * g_di64f3c0);
        Di_Add();
        sprintf(&g_diLine, &g_di682f84, g_di720720);
        Di_Add();
        BYTE* o = F(BYTE*, s, 488);
        if (o)
            o = F(BYTE*, o, 76);
        sprintf(&g_diLine, &g_di682f78, F(int, o, 12));
        Di_Add();
        sprintf(&g_diLine, &g_di682f64, g_di702e9c);
        Di_Add();
        int fl = F(int, g_di721750, 268);
        if (fl & 0x400)
        {
            Sys_Log(&g_diText, fl);
            return 0;
        }
        return MessageBoxA(0, &g_diText, &g_di682f58, 0);
    }
    case 118:
        g_di721760 ^= 1;
        break;
    case 119:
    {
        // the camera's matrix
        Clock_Start(s, 0);
        FILE* f = fopen(&g_di682f34, &g_di67fb80);
        fprintf(f, &g_di6830c4, g_di72175c);
        float m[12];
        Camera_Get(s, 0, m);
        for (int r = 0; r < 4; r++)
            fprintf(f, &g_di682f24, (double)m[3 * r], (double)m[3 * r + 1], (double)m[3 * r + 2]);
        fclose(f);
        g_di72175c++;
        Clock_Stop(s, 0);
        return 0;
    }
    case 120:
        g_di721764 ^= 1;
        break;
    }
    return k - 8;
}

} // extern "C"

// === the camera kept out of the scene's walls (a method at $L_5ded40)

#include "vecmath.h"

extern "C" void Cc_NodeInit(); KING_ALIAS("_Cc_NodeInit", "@Node_Init@8")
extern "C" void Cc_World(); KING_ALIAS("_Cc_World", "@Node_GetWorld@12")
extern "C" void Cc_Apply(); KING_ALIAS("_Cc_Apply", "@Node_ApplyMatrix@16")
extern "C" void Cc_FromPose(); KING_ALIAS("_Cc_FromPose", "_Matrix_FromPose")
extern "C" void Cc_Point(); KING_ALIAS("_Cc_Point", "@Vec3_TransformPoint@12")
extern "C" void Cc_Dir(); KING_ALIAS("_Cc_Dir", "@Vec3_TransformDir@12")
extern "C" void Cc_Transform2(); KING_ALIAS("_Cc_Transform2", "@Matrix_Transform2@12")
KING_VAR(Matrix, g_cc7025f0, 7025f0)
KING_VAR(double, g_cc64d338, 64d338)
KING_VAR(float, g_cc64c304, 64c304)

extern "C" {

// [$L_5ded40] camera c (+616 its node, +488 the scene and its room +76): placed from its pose (+2904.., or from the
// node with flag 0x40000000), then moved up to 10 times out of what the room's vtbl+52 finds between the old place
// (+568) and the new (+580): pushed along the wall's normal by +632 * 64c304, or handed to vtbl+156 (the pose
// restored with +2984 == 1); a room change (bit 2) taken; then the hit given to its object and vtbl+152, and the
// pose's position read back
void __fastcall Camera_Collide(BYTE* c, void*)
{
    __declspec(align(8)) BYTE A[208], B[208], C[208];
    CallThis(Cc_NodeInit, A);
    CallThis(Cc_NodeInit, B);
    CallThis(Cc_NodeInit, C);
    Matrix M1 = g_cc7025f0, M2 = g_cc7025f0, M3 = g_cc7025f0;
    int ticks = 1000;
    if (!F(BYTE*, c, 488))
        return;
    float yaw = (float)F(double, c, 2928), pitch = (float)F(double, c, 2936), roll = (float)F(double, c, 2944);
    CallVirt(c, 176);
    CallThis(Cc_World, F(BYTE*, c, 616), &M1);
    F(Vec3, c, 568) = *(Vec3*)&M1.m[9];
    if (F(DWORD, c, 3020) & 0x40000000)
        M3 = M1;
    else
    {
        Matrix t;
        M3 = *CallC<const Matrix*>(Cc_FromPose, &t, F(double, c, 2904), F(double, c, 2912), F(double, c, 2920),
                                   F(double, c, 2952) + F(double, c, 2928), F(double, c, 2936), F(double, c, 2944));
    }
    CallThis(Cc_Apply, F(BYTE*, c, 616), &M3, 0);
    F(int, B, 100) = 0;
    F(int, B, 116) = 0;
    F(int, B, 120) = 0;
    DWORD r = 0;
    int it = 0;
    if (F(BYTE*, F(BYTE*, c, 488), 76))
    {
        for (; it < 10; it++)
        {
            CallThis(Cc_World, F(BYTE*, c, 616), &M2);
            Vec3 p = *(Vec3*)&M2.m[9];
            F(Vec3, c, 580) = p;
            memcpy(A + 4, &g_cc7025f0, 48);
            F(float, A, 0) = (float)F(double, c, 632);
            F(Vec3, A, 88) = p;
            F(Vec3, A, 76) = F(Vec3, c, 568);
            F(int, A, 116) = 0;
            F(int, A, 120) = 0;
            F(int, A, 100) = 0;
            BYTE* room = F(BYTE*, F(BYTE*, c, 488), 76);
            r = CallVirt<DWORD>(room, 52, F(int, c, 3028) ? 6 : 0, A);
            Matrix inv;
            Matrix_Inverse(&inv, (const Matrix*)(A + 4));
            CallThis(Cc_Point, A + 52, &inv);
            CallThis(Cc_Dir, A + 64, &inv);
            if (!F(int, c, 3028) || !(r & 1))
                break;
            bool slide = false;
            if (it)
            {
                Vec3 d, n;
                const Vec3& a = F(Vec3, c, 568);
                d.x = (float)((double)F(float, c, 580) - a.x);
                d.y = (float)((double)F(float, c, 584) - a.y);
                d.z = (float)((double)F(float, c, 588) - a.z);
                const Vec3* q = Vec3_Normalized(&n, &d);
                const Vec3& N = F(Vec3, A, 64);
                double dot = (double)q->z * N.z + (double)q->y * N.y + (double)N.x * q->x;
                slide = !(x87_fabs(dot) > g_cc64d338);
            }
            if (slide)
            {
                const Vec3& N = F(Vec3, A, 64);
                double nz = N.z * (double)g_cc64c304;
                float ny = (float)(N.y * (double)g_cc64c304);
                *(Vec3*)&M2.m[9] = *(Vec3*)&M1.m[9];
                float nx = (float)(N.x * (double)g_cc64c304);
                float s = (float)F(double, c, 632);
                Vec3 v;
                v.z = (float)(nz * s);
                v.y = (float)(ny * (double)s);
                v.x = (float)(nx * (double)s);
                Matrix T;
                CallThis(Cc_Transform2, &M2, Matrix_Translation(&T, v));
            }
            else
            {
                memcpy(B, A, 208);
                CallVirt(c, 156, &M2, &M1, A);
                if (F(int, c, 2984) == 1)
                {
                    F(double, c, 2928) = yaw;
                    F(double, c, 2944) = roll;
                    F(double, c, 2936) = pitch;
                }
            }
            CallThis(Cc_Apply, F(BYTE*, c, 616), &M2, 0);
        }
        if (it == 10)
        {
            *(Vec3*)&M2.m[9] = *(Vec3*)&M1.m[9];
            CallThis(Cc_Apply, F(BYTE*, c, 616), &M2, 0);
            r = 0;
        }
    }
    if (r & 2)
        F(int, F(BYTE*, c, 488), 76) = F(int, A, 104);
    if (BYTE* o = F(BYTE*, c, 532))
    {
        CallVirt(o, 36);
        CallVirt(F(BYTE*, F(BYTE*, c, 488), 76), 28, F(BYTE*, c, 532));
    }
    if (F(BYTE*, B, 116))
    {
        memcpy(C, B, 208);
        F(int*, C, 100) = &ticks;
        CallVirt(F(BYTE*, C, 116), 32, C);
    }
    CallVirt(c, 152, B);
    CallThis(Cc_World, F(BYTE*, c, 616), &M2);
    F(double, c, 2904) = M2.m[9];
    F(double, c, 2912) = M2.m[10];
    F(double, c, 2920) = M2.m[11];
    F(Vec3, c, 580) = *(Vec3*)&M2.m[9];
}

} // extern "C"

// === an empty record (a method at $L_5e6980)

extern "C" {

// [$L_5e6980] record s cleared: +4.. +16 and +32.. +44 (+20.. +28 kept)
BYTE* __fastcall Rec5e6980_Clear(BYTE* s, void*)
{
    F(int, s, 8) = 0;
    F(int, s, 12) = 0;
    F(int, s, 16) = 0;
    F(int, s, 32) = 0;
    F(int, s, 36) = 0;
    F(int, s, 40) = 0;
    F(int, s, 44) = 0;
    F(int, s, 4) = 0;
    return s;
}

} // extern "C"

// === a cluster's images, masks, sounds and items attached to one memory block ($L_5ee070 with its tails up to
//     $L_5ee1c3, a vtable entry)

extern "C" void Ca_TexSet(); KING_ALIAS("_Ca_TexSet", "@Texture_SetPixels@20")
extern "C" void Ca_TexSize(); KING_ALIAS("_Ca_TexSize", "@Texture_Size@8")
extern "C" void Ca_BmpSet(); KING_ALIAS("_Ca_BmpSet", "@Bitmap_SetPixels@16")
extern "C" void Ca_BmpBytes(); KING_ALIAS("_Ca_BmpBytes", "@Bitmap_Bytes@8")
extern "C" void Ca_WavSet(); KING_ALIAS("_Ca_WavSet", "@SoundFile_SetBuffer@12")
extern "C" void Ca_Fatal(); KING_ALIAS("_Ca_Fatal", "_Sys_Fatal")

static void Ca_Twice(const char* fmt, int k, const BYTE* s)
{
    char buf[84];
    sprintf(buf, fmt, k, F(const char*, s, 36));
    CallC(Ca_Fatal, buf, 0);
}

extern "C" {

// [$L_5ee070..$L_5ee1c3] cluster s (named +36) attached at p: its images (+44 / +68), masks (+52 / +72), sounds
// (+60 / +76) and items (+24 / +32, vtable +24) given their data one after another (all at p when p is 0); fatal
// when one already has data or the data passes the cluster's size (+16)
void __fastcall Cluster_Attach(BYTE* s, void*, BYTE* p)
{
    BYTE* at = p;
    for (int k = 0; k < F(int, s, 44); k++)
    {
        BYTE* t = F(BYTE**, s, 68)[k];
        if (!CallThis<int>(Ca_TexSet, t, at, 0, 0))
            Ca_Twice("Twice mapped image.Image %d in cluster %s.", k, s);
        if (p)
            at += CallThis<int>(Ca_TexSize, F(BYTE**, s, 68)[k]);
    }
    for (int k = 0; k < F(int, s, 52); k++)
    {
        if (!CallThis<int>(Ca_BmpSet, F(BYTE**, s, 72)[k], at, 0))
            Ca_Twice("Twice mapped mask.Mask %d in cluster %s.", k, s);
        if (p)
            at += CallThis<int>(Ca_BmpBytes, F(BYTE**, s, 72)[k]);
    }
    for (int k = 0; k < F(int, s, 60); k++)
    {
        if (!CallThis<int>(Ca_WavSet, F(BYTE**, s, 76)[k], at))
            Ca_Twice("Twice mapped wav.Wav %d in cluster %s.", k, s);
        if (p)
            at += F(int, F(BYTE**, s, 76)[k], 4);
    }
    for (int k = 0; k < F(int, s, 24); k++)
    {
        CallVirt(F(BYTE**, s, 32)[k], 24, at);
        if (p)
            at += F(int, F(BYTE**, s, 32)[k], 16);
    }
    if ((DWORD)at > (DWORD)(F(int, s, 16) + (int)p))
        CallC(Ca_Fatal, "Incorrect attach.", 0);
}

} // extern "C"

// === a resource table added by its two-letter name (FUN_5e4f60 with its tail $L_5e4fa1)

extern "C" void Rt_Fatal(); KING_ALIAS("_Rt_Fatal", "_Sys_Fatal")
extern "C" void Rt_Table(); KING_ALIAS("_Rt_Table", "@NamedTable_Construct@12")
extern "C" void Rt_Link(); KING_ALIAS("_Rt_Link", "@Node_Link@12")
KING_VAR(char, g_rtBadName, 68331c)

extern "C" {

// [FUN_5e4f60..$L_5e4fa1] resources s get a table named name (two letters, fatal else) in bucket (+1600) by the
// sum of its letters mod 100, linked after the bucket's last one unless the name is already there; 1
int __fastcall Resources_AddTable(BYTE* s, void*, const char* name)
{
    if (!name || strlen(name) != 2)
        CallC(Rt_Fatal, &g_rtBadName, name);
    if (!name)
        return 0;
    int sum = 0;
    for (const unsigned char* c = (const unsigned char*)name; *c; c++)
        sum += *c;
    int h = sum % 100;
    BYTE* t = (BYTE*)operator new(4020);
    if (t)
        t = CallThis<BYTE*>(Rt_Table, t, name);
    BYTE*& head = F(BYTE*, s, 1600 + 4 * h);
    if (!head)
    {
        head = t;
        return 1;
    }
    BYTE* e = head;
    while (strcmp(name, F(const char*, e, 4016)) && F(BYTE*, e, 8))
        e = F(BYTE*, e, 8);
    if (!strcmp(name, F(const char*, e, 4016)))
    {
        if (t)
            CallVirt(t, 0, 1);
    }
    else
        CallThis(Rt_Link, t, e);
    return 1;
}

} // extern "C"

// === a room's objects queued for deleting ($L_5d9a20 with its tail $L_5d9aa1, a node walk's callback)

extern "C" void Rq_Load(); KING_ALIAS("_Rq_Load", "@Resources_Load@12")
extern "C" void Rq_Link(); KING_ALIAS("_Rq_Link", "@Fw_LinkRoom@12")
extern "C" void Rq_Unlink(); KING_ALIAS("_Rq_Unlink", "@Node_Unlink@8")
extern "C" void Rq_Fatal(); KING_ALIAS("_Rq_Fatal", "_Sys_Fatal")

extern "C" {

// [$L_5d9a20..$L_5d9aa1] node of kind 14 with flag 2 and a module (+128): its room (module +36) loaded into the
// framework's resources (ctx +20, vtable +112), relinked, unlinked and queued in ctx's ring of 100 (+24, +424 the
// end, +428 the start; fatal when full); -1 to go on
int __cdecl Room_QueueDelete(BYTE* node, BYTE* ctx)
{
    if (F(int, node, 16) != 14 || !(F(BYTE, node, 40) & 2) || !F(BYTE*, node, 128))
        return -1;
    BYTE* room = F(BYTE*, F(BYTE*, node, 128), 36);
    if (BYTE* r = CallVirt<BYTE*>(F(BYTE*, ctx, 20), 112))
        CallThis(Rq_Load, r, F(int, room, 60));
    CallThis(Rq_Link, F(BYTE*, ctx, 20), room);
    CallThis(Rq_Unlink, room);
    F(BYTE*, ctx, 24 + 4 * F(int, ctx, 424)) = room;
    F(int, ctx, 424) = (F(int, ctx, 424) + 1) % 100;
    if (F(int, ctx, 424) == F(int, ctx, 428))
        CallC(Rq_Fatal, "DL Module to delete too many", 0);
    return -1;
}

} // extern "C"

// === the framework's observer cluster made (FUN_5daa30 with its tail $L_5dab43)

extern "C" void Ob_Sound(); KING_ALIAS("_Ob_Sound", "_Sound_Start")
extern "C" void Ob_Palette(); KING_ALIAS("_Ob_Palette", "@Palette_Construct@8")
extern "C" void Ob_Gray(); KING_ALIAS("_Ob_Gray", "@Palette_Gray@8")
extern "C" void Ob_Defaults(); KING_ALIAS("_Ob_Defaults", "@Palette_Defaults@8")
extern "C" void Ob_Fog(); KING_ALIAS("_Ob_Fog", "@Fog_BuildTables@16")
extern "C" void Ob_Dark(); KING_ALIAS("_Ob_Dark", "@Dark_BuildTables@12")
extern "C" void Ob_Fade(); KING_ALIAS("_Ob_Fade", "@Fade_BuildTables@16")
extern "C" void Ob_Cluster(); KING_ALIAS("_Ob_Cluster", "@Cluster_ConstructEmpty@36")
extern "C" void Ob_Resolve(); KING_ALIAS("_Ob_Resolve", "@NameHash_ResolvePending@8")
extern "C" void Ob_Fatal(); KING_ALIAS("_Ob_Fatal", "_Sys_Fatal")
extern "C" void Ob_NodeN(); KING_ALIAS("_Ob_NodeN", "@NodeN_Construct@20")
extern "C" void Ob_Apply(); KING_ALIAS("_Ob_Apply", "@Node_ApplyMatrix@16")
extern "C" void Ob_AddNode(); KING_ALIAS("_Ob_AddNode", "@Room_AddNode@12")
extern "C" void Ob_HashAdd(); KING_ALIAS("_Ob_HashAdd", "@NameHash_Add@16")
extern "C" void Ob_Push(); KING_ALIAS("_Ob_Push", "@Group_Push@12")
KING_VAR(BYTE*, g_ob721750, 721750)
KING_VAR(BYTE, g_ob71e820, 71e820)
KING_VAR(AsmLabel, g_ob7025f0, 7025f0)
KING_VAR(AsmLabel, g_ob64d7d0, 64d7d0)
KING_VAR(AsmLabel, g_ob64d7bc, 64d7bc)
KING_VAR(char, g_obName, 682c5c)

extern "C" {

// [FUN_5daa30..$L_5dab43] framework s's cluster (+468; a gray palette of its own when pal < 1), its "r_obs" node
// 10 below the origin with its view record (+120), named in the object names
BYTE* __fastcall Fw_MakeCluster(BYTE* s, void*, int pal, int items, int images, int masks, int sounds, int bufs,
                                int nodes)
{
    if (F(int, g_ob721750, 344))
        CallC(Ob_Sound);
    BYTE* own = 0;
    if (pal < 1)
    {
        BYTE* p = (BYTE*)operator new(816);
        if (p)
            p = CallThis<BYTE*>(Ob_Palette, p);
        own = p;
        CallThis(Ob_Gray, p);
        CallThis(Ob_Defaults, p);
        CallThis(Ob_Fog, p, 4, 8);
        CallThis(Ob_Dark, p, 32);
        CallThis(Ob_Fade, p, 32, 4);
        pal = 1;
    }
    BYTE* c = (BYTE*)operator new(180);
    if (c)
        c = CallThis<BYTE*>(Ob_Cluster, c, pal, items, images, masks, sounds, bufs, nodes);
    F(BYTE*, s, 468) = c;
    if (const char* err = CallThis<const char*>(Ob_Resolve, &g_ob71e820))
        CallC(Ob_Fatal, err, 0);
    BYTE* n = (BYTE*)operator new(112);
    if (n)
    {
        CallThis(Ob_NodeN, n, &g_obName, 6, 1);
        memset(n + 64, 0, 48);
        memcpy(n + 64, &g_ob7025f0, 48);
        F(void*, n, 0) = &g_ob64d7d0;
        F(int, n, 56) = 0;
        F(int, n, 60) = 0;
    }
    Vec3 at = {0, -10.0f, 0};
    Matrix t;
    CallThis(Ob_Apply, n, Matrix_Translation(&t, at), 1);
    CallThis(Ob_AddNode, F(BYTE*, s, 468), n);
    BYTE* o = (BYTE*)operator new(104);
    if (o)
    {
        F(int, o, 4) = 0;
        F(int, o, 8) = 0;
        F(void*, o, 0) = &g_ob64d7bc;
        F(int, o, 76) = 0;
        F(BYTE*, o, 80) = n;
        F(float, o, 88) = 1.0f;
        F(float, o, 84) = 1.0f;
        F(int, o, 96) = 0;
        F(int, o, 92) = 0;
        F(BYTE, o, 44) = 0;
        F(BYTE, o, 12) = 0;
        F(int, o, 100) = 0;
    }
    F(BYTE*, F(BYTE*, s, 468), 120) = o;
    CallThis(Ob_HashAdd, &g_ob71e820, &g_obName, n);
    if (own)
        CallThis(Ob_Push, F(BYTE*, s, 468), own);
    return F(BYTE*, s, 468);
}

} // extern "C"

// === a window resized with its child keeping its margins in proportion ($L_5da410, a vtable entry)

extern "C" void Wr_Next(); KING_ALIAS("_Wr_Next", "@Node_Next@8")
extern "C" void Wr_Remake(); KING_ALIAS("_Wr_Remake", "@Fw_RemakeSurface@24")
KING_VAR(double, g_wr64b670, 64b670)

// child c's margin (+44 / +48 by off) against window s's free space (size by vtable slot, less the end +52 / +56),
// as a ratio; 1000 when there is none
static double Wr_Ratio(BYTE* s, int slot, BYTE* c, int off)
{
    if (CallVirt<int>(s, slot) == F(int, c, off + 8))
        return 1000.0;
    double m = F(int, c, off);
    return x87_fabs(m / (double)(CallVirt<int>(s, slot) - F(int, c, off + 8)));
}

extern "C" {

// [$L_5da410] window s (its node +56, shown +60) remade at w x h: its child (+288, the last one) keeps its left /
// top margins (+44 / +48, the right / bottom ends +52 / +56) in the same ratio to the free space; the node's ends
// follow the new size
void __fastcall Wnd_Resize(BYTE* s, void*, int w, int h, int a3, int)
{
    double kx = 2.0, ky = 2.0;
    BYTE* n = F(BYTE*, s, 56);
    BYTE* c;
    if (n && F(int, s, 60) && (c = F(BYTE*, n, 288)) != 0 && !CallThis<int>(Wr_Next, c))
    {
        kx = Wr_Ratio(s, 20, c, 44);
        ky = Wr_Ratio(s, 24, c, 48);
    }
    CallThis(Wr_Remake, s, w, h, a3, 0);
    n = F(BYTE*, s, 56);
    if (!n || !F(int, s, 60))
        return;
    F(int, n, 52) = F(int, n, 44) + w;
    n = F(BYTE*, s, 56);
    F(int, n, 56) = F(int, n, 48) + h;
    n = F(BYTE*, s, 56);
    if ((F(BYTE, n, 32) & 4) || !(c = F(BYTE*, n, 288)) || CallThis<int>(Wr_Next, c))
        return;
    int vx = CallVirt<int>(s, 20) + (F(int, c, 44) - F(int, c, 52));
    int x = (int)((double)vx * (kx / (kx + g_wr64b670)));
    int vy = CallVirt<int>(s, 24) + (F(int, c, 48) - F(int, c, 56));
    int y = (int)((double)vy * (ky / (ky + g_wr64b670)));
    F(int, c, 52) += x - F(int, c, 44);
    F(int, c, 44) = x;
    F(int, c, 56) += y - F(int, c, 48);
    F(int, c, 48) = y;
}

} // extern "C"

// === the framework's renderer made again for a new surface (FUN_5dbbb0)

extern "C" void Rr_Surface(); KING_ALIAS("_Rr_Surface", "@Fw_NewSurface@24")
extern "C" void Rr_Nop(); KING_ALIAS("_Rr_Nop", "_Fw_Nop")
extern "C" void Rr_VecDtor(); KING_ALIAS("_Rr_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
extern "C" void Rr_New(); KING_ALIAS("_Rr_New", "@Renderer_Construct@12")
KING_VAR(AsmLabel, g_rr652280, 652280)

extern "C" {

// [FUN_5dbbb0] framework s's surface made (a, b, c, d), its renderer (+464) replaced by a new one keeping the old
// one's +8452
void __fastcall Fw_ResetRenderer(BYTE* s, void*, int a, int b, int c, int d)
{
    CallThis(Rr_Surface, s, a, b, c, d);
    BYTE* r = F(BYTE*, s, 464);
    int keep = F(int, r, 8452);
    if (r)
    {
        CallC(Rr_Nop, r + 7824);
        CallStd(Rr_VecDtor, r + 6224, 80, 20, (void*)Rr_Nop);
        operator delete(r);
    }
    BYTE* n = (BYTE*)operator new(20992);
    if (n)
    {
        CallThis(Rr_New, n, F(int, s, 12));
        F(BYTE*, n, 20976) = s;
        F(void*, n, 0) = &g_rr652280;
        F(BYTE*, n, 20984) = s;
    }
    F(BYTE*, s, 464) = n;
    F(int, n, 8452) = keep;
}

} // extern "C"

// === a window's bitmap blitted onto a DC ($L_5e27c0, a vtable entry)

extern "C" void Cd_New(); KING_ALIAS("_Cd_New", "@Cdc_Construct@4")
extern "C" void Cd_Free(); KING_ALIAS("_Cd_Free", "@Cdc_Destruct@4")
extern "C" void Cd_Attach(); KING_ALIAS("_Cd_Attach", "@Cdc_Attach@12")
extern "C" void Cd_Select(); KING_ALIAS("_Cd_Select", "_Cdc_SelectGdiObject@8")
extern "C" void Cd_MapMode(); KING_ALIAS("_Cd_MapMode", "@Cdc_SetMapMode@12")

extern "C" {

// [$L_5e27c0] window s's bitmap (+188, its handle +4) copied to CDC to at to's logical origin, the window's
// client size (+28 the window) mapped to to's mapping mode; 0 without a bitmap
int __fastcall Wnd_BlitTo(BYTE* s, void*, BYTE* to)
{
    __declspec(align(8)) BYTE dc[16];
    CallThis(Cd_New, dc);
    if (!F(void*, F(BYTE*, s, 188), 4))
    {
        CallThis(Cd_Free, dc);
        return 0;
    }
    HDC mem = CreateCompatibleDC(to ? F(HDC, to, 4) : 0);
    CallThis(Cd_Attach, dc, mem);
    void* bmp = F(BYTE*, s, 188) ? F(void*, F(BYTE*, s, 188), 4) : 0;
    CallStd(Cd_Select, F(HDC, dc, 4), bmp);
    CallThis(Cd_MapMode, dc, GetMapMode(F(HDC, to, 8)));
    RECT rc;
    GetClientRect(F(HWND, s, 28), &rc);
    SIZE sz = {rc.right - rc.left, rc.bottom - rc.top};
    DPtoLP(F(HDC, dc, 8), (POINT*)&sz, 1);
    POINT pt = {0, 0};
    DPtoLP(F(HDC, dc, 8), &pt, 1);
    BitBlt(F(HDC, to, 4), pt.x, pt.y, sz.cx, sz.cy, F(HDC, dc, 4), pt.x, pt.y, SRCCOPY);
    CallThis(Cd_Free, dc);
    return 1;
}

} // extern "C"

// === the resource file (*.rmp or its text form) read into an image (FUN_5ec0f0 with its tails $L_5ec1a4, $L_5ec1d7, $L_5ed091)

extern "C" void __cdecl Rm_SetDir(const char* dir); KING_ALIAS("_Rm_SetDir", "_Files_SetDir")
extern "C" void Rm_MmioReset(); KING_ALIAS("_Rm_MmioReset", "@Mmio_Reset@8")
extern "C" void Rm_MmioOpen(); KING_ALIAS("_Rm_MmioOpen", "@Mmio_Open@12")
extern "C" char* __cdecl Rm_ReadString(char* buf, void* f); KING_ALIAS("_Rm_ReadString", "_File_ReadString")
extern "C" char* __cdecl Rm_Token(char** cur); KING_ALIAS("_Rm_Token", "_NextToken")
extern "C" int __cdecl Rm_ReadInt(void* f); KING_ALIAS("_Rm_ReadInt", "_File_ReadInt")
extern "C" char* __cdecl Rm_fgets(char* s, int n, void* f); KING_ALIAS("_Rm_fgets", "_fgets")
extern "C" long __cdecl Rm_ftell(void* f); KING_ALIAS("_Rm_ftell", "_ftell")
extern "C" int __cdecl Rm_fseek(void* f, long off, int origin); KING_ALIAS("_Rm_fseek", "_fseek")
extern "C" void __cdecl Rm_SoundBuffers(int n); KING_ALIAS("_Rm_SoundBuffers", "_Sound_AllocBuffers")
extern "C" void* __cdecl Rm_SoundFromFile(void* f, int i, int size, int noload); KING_ALIAS("_Rm_SoundFromFile", "_Sound_CreateFromFile")
extern "C" void* __cdecl Rm_SoundCreate(const char* name, int i, int noload); KING_ALIAS("_Rm_SoundCreate", "_Sound_Create")
extern "C" void Rm_HashAdd(); KING_ALIAS("_Rm_HashAdd", "@NameHash_Add@16")
extern "C" void Rm_SoundCtor(); KING_ALIAS("_Rm_SoundCtor", "@Sound_Construct@20")
extern "C" void Rm_BitmapData(); KING_ALIAS("_Rm_BitmapData", "@Bitmap_ConstructData@20")
extern "C" void Rm_BitmapCtor(); KING_ALIAS("_Rm_BitmapCtor", "@Bitmap_Construct@12")
extern "C" void __cdecl Rm_MaskHeader(void* o, void* f, int size, void* map); KING_ALIAS("_Rm_MaskHeader", "_Mask_ReadHeader")
extern "C" void* __cdecl Rm_MaskFromFile(void* f, int size); KING_ALIAS("_Rm_MaskFromFile", "_Texture_ReadFile")
extern "C" void __cdecl Rm_MaskRead(void* o, const char* name); KING_ALIAS("_Rm_MaskRead", "_Data_Read5e94d0")
extern "C" void* __cdecl Rm_MaskLoad(const char* name); KING_ALIAS("_Rm_MaskLoad", "_Data_Read5e9360")
extern "C" void Rm_TexNamed(); KING_ALIAS("_Rm_TexNamed", "@Texture_ConstructNamed@20")
extern "C" void Rm_TexCtor(); KING_ALIAS("_Rm_TexCtor", "@Texture_Construct@12")
extern "C" void __cdecl Rm_TgaHeader(void* o, void* f, void* map); KING_ALIAS("_Rm_TgaHeader", "_Tga_ReadHeader")
extern "C" void* __cdecl Rm_ImageFromFile(void* f, const char* name, int flags); KING_ALIAS("_Rm_ImageFromFile", "_Image_FromFile")
extern "C" void __cdecl Rm_TgaRead(void* o, const char* name); KING_ALIAS("_Rm_TgaRead", "_Data_Read5e7930")
extern "C" void* __cdecl Rm_ImageLoad(const char* name); KING_ALIAS("_Rm_ImageLoad", "_Image_LoadFile")
extern "C" int __cdecl Rm_Material(char* text, BYTE* m); KING_ALIAS("_Rm_Material", "_Material_Read")
extern "C" void* __cdecl Rm_PalFromFile(void* f); KING_ALIAS("_Rm_PalFromFile", "_Palette_Load")
extern "C" void* __cdecl Rm_PalLoad(const char* name); KING_ALIAS("_Rm_PalLoad", "_Data_Read5e6e90")
extern "C" void __cdecl Rm_Fatal(const char* fmt, ...); KING_ALIAS("_Rm_Fatal", "_Sys_Fatal")

KING_VAR(char, g_rmDot671204, 671204)        // ".\\"
KING_VAR(char, g_rm68d1c0, 68d1c0)
KING_VAR(char, g_rmCannotOpen, 683774)       // "Cannot open *.rmp"
KING_VAR(char, g_rmColors, 68376c)           // "COLORS"
KING_VAR(char, g_rmEffects, 672998)          // "EFFECTS"
KING_VAR(char, g_rmSoundFiles, 683760)       // "SOUNDFILES"
KING_VAR(char, g_rmSounds, 683750)           // "SOUNDS"
KING_VAR(char, g_rmMaskFiles, 683744)        // "MASKFILES"
KING_VAR(char, g_rmTextureFiles, 683734)     // "TEXTUREFILES"
KING_VAR(char, g_rmMaterials, 683720)        // "MATERIALS"
KING_VAR(char, g_rmPaletteFiles, 6836f0)     // "PALETTEFILES"
KING_VAR(char, g_rmBackFiles, 6836e4)        // "BACKFILES"
KING_VAR(char, g_rmNoload, 683758)           // "noload"
KING_VAR(char, g_rmMemfix, 68372c)           // "memfix"
KING_VAR(char, g_rmBumpcoord, 683674)        // "bumpcoord"
KING_VAR(char, g_rmNumMaterials, 6836d4)     // "num materials"
KING_VAR(char, g_rmIncorrectMat, 68370c)     // "Incorrect material"
KING_VAR(char, g_rmMatRoad, 683700)          // "mat_road"
KING_VAR(char, g_rmIllegal, 6836c0)          // "illegal keyword %s"
KING_VAR(char, g_rmLine, 7220a4)             // the line buffer
KING_VAR(char, g_rmMatText, 722950)
KING_VAR(BYTE, g_rmNames, 71e820)            // the name hash

typedef void(__cdecl* Rm_Callback)();
extern "C" int __cdecl atoi(const char* s);

#define RM_EOF(f) FILE_EOF(f)

// a line of the file: from the mapped resource file (mode != 0) or by fgets
static void Rm_ReadLine(int mode, void* f, int n)
{
    if (mode)
        Rm_ReadString(&g_rmLine, f);
    else
        Rm_fgets(&g_rmLine, n, f);
}

static char* Rm_Dup(const char* s)
{
    char* d = (char*)operator new(strlen(s) + 1);
    strcpy(d, s);
    return d;
}

static void Rm_Add(const char* name, void* o) { CallThis(Rm_HashAdd, &g_rmNames, name, o); }

extern "C" {

// [FUN_5ec0f0..$L_5ed091] the image's resources from file f (kind 3: the .rmp at path mapped into memory, kind 2:
// the mmio file at path, else plain text lines): sections COLORS, EFFECTS (name, text), SOUNDFILES, SOUNDS,
// MASKFILES, TEXTUREFILES (noload / memfix / bumpcoord / a number), MATERIALS (must match the count at +72),
// PALETTEFILES and BACKFILES, each "KEYWORD n" and n named lines; every resource also goes into the name hash.
// At the end the materials get their textures (+56 -> +76), masks (+60 -> +72) and palette shade (+68).
// Returns 0, or the name of what failed (or the illegal keyword's message)
const char* __fastcall Image_ReadResources(BYTE* s, void*, const char* dir, void* f, int kind, const char* path)
{
    Rm_SetDir(strcmp(dir, &g_rmDot671204) == 0 ? &g_rm68d1c0 : dir);
    if (kind == 3)
    {
        F(DWORD, s, 152) |= 1;
        HANDLE h = CreateFileA(path, 0x80000000, 1, 0, 3, 0, 0);
        F(HANDLE, s, 44) = h;
        if (h == INVALID_HANDLE_VALUE)
            Rm_Fatal(&g_rmCannotOpen, 0);
        HANDLE m = CreateFileMappingA(F(HANDLE, s, 44), 0, 2, 0, 0, 0);
        F(HANDLE, s, 48) = m;
        F(void*, s, 40) = MapViewOfFile(m, 4, 0, 0, 0);
        kind = 2;
    }
    else
        F(DWORD, s, 152) &= ~1u;
    if (kind == 2)
    {
        F(void*, s, 36) = f;
        BYTE* mm = (BYTE*)operator new(131596);
        if (mm)
            mm = CallThis<BYTE*>(Rm_MmioReset, mm);
        F(BYTE*, s, 52) = mm;
        CallThis(Rm_MmioOpen, mm, path);
    }
    int mode = kind - 1;
    char* cur;

    for (;;)    // $L_5ec232: the next keyword
    {
        char* key;
        do
        {
            Rm_ReadLine(mode, f, 70);
            cur = &g_rmLine;
            key = Rm_Token(&cur);
        }
        while (!key && !RM_EOF(f));
        if (RM_EOF(f))
            break;
        if (F(Rm_Callback, s, 56))
            F(Rm_Callback, s, 56)();

        if (strcmp(key, &g_rmColors) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                if (Rm_Token(&cur))
                    i++;
            }
        }
        else if (strcmp(key, &g_rmEffects) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            if (n == 0)
                continue;
            F(int, s, 132) = n;
            F(char**, s, 124) = (char**)operator new(n * 4);
            F(char**, s, 128) = (char**)operator new(n * 4);
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                if (!name)
                    continue;
                F(char**, s, 124)[i] = Rm_Dup(name);
                F(char**, s, 128)[i] = cur ? Rm_Dup(cur) : 0;
                i++;
            }
        }
        else if (strcmp(key, &g_rmSoundFiles) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            if (n == 0)
                continue;
            Rm_SoundBuffers(n);
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                if (!name)
                    continue;
                char* t = Rm_Token(&cur);
                int noload = t && strcmp(t, &g_rmNoload) == 0;
                void* o;
                if (mode)
                {
                    int size = Rm_ReadInt(f);
                    long pos = Rm_ftell(f);
                    o = Rm_SoundFromFile(f, i, size, noload);
                    Rm_fseek(f, pos + size, 0);
                }
                else
                    o = Rm_SoundCreate(name, i, noload);
                i++;
                Rm_Add(name, o);
            }
        }
        else if (strcmp(key, &g_rmSounds) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            if (n == 0)
                continue;
            F(int, s, 112) = n;
            F(void**, s, 116) = (void**)operator new(n * 4);
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                if (!name)
                    continue;
                BYTE* o = (BYTE*)operator new(32);
                if (o)
                {
                    int k = atoi(Rm_Token(&cur)) - 1;
                    o = CallThis<BYTE*>(Rm_SoundCtor, o, name, k, 1);
                }
                F(void**, s, 116)[i] = o;
                Rm_Add(name, F(void**, s, 116)[i]);
                i++;
            }
        }
        else if (strcmp(key, &g_rmMaskFiles) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            if (n == 0)
                continue;
            F(int, s, 104) = n;
            F(BYTE**, s, 108) = (BYTE**)operator new(n * 4);
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                char* t = Rm_Token(&cur);
                BYTE** list = F(BYTE**, s, 108);
                if (mode)
                {
                    int size = Rm_ReadInt(f);
                    long pos = Rm_ftell(f);
                    if ((t && strcmp(t, &g_rmNoload) == 0) || (F(BYTE, s, 152) & 1))
                    {
                        bool noload = t && strcmp(t, &g_rmNoload) == 0;
                        BYTE* o = (BYTE*)operator new(40);
                        if (o)
                            o = CallThis<BYTE*>(Rm_BitmapData, o, f, pos, size);
                        F(BYTE**, s, 108)[i] = o;
                        if (F(BYTE, s, 152) & 1)
                            Rm_MaskHeader(F(BYTE**, s, 108)[i], f, size, F(void*, s, 40));
                        else
                        {
                            Rm_MaskHeader(F(BYTE**, s, 108)[i], f, size, 0);
                            F(DWORD, F(BYTE**, s, 108)[i], 28) |= 4;
                            F(DWORD, F(BYTE**, s, 108)[i], 28) &= ~1u;
                        }
                        (void)noload;
                    }
                    else
                        F(BYTE**, s, 108)[i] = (BYTE*)Rm_MaskFromFile(f, size);
                    Rm_fseek(f, pos + size, 0);
                }
                else if (t && strcmp(t, &g_rmNoload) == 0)
                {
                    BYTE* o = (BYTE*)operator new(40);
                    if (o)
                        o = CallThis<BYTE*>(Rm_BitmapCtor, o, name);
                    F(BYTE**, s, 108)[i] = o;
                    Rm_MaskRead(F(BYTE**, s, 108)[i], name);
                }
                else
                    F(BYTE**, s, 108)[i] = (BYTE*)Rm_MaskLoad(name);
                (void)list;
                Rm_Add(name, F(BYTE**, s, 108)[i]);
                if (name)
                    i++;
            }
        }
        else if (strcmp(key, &g_rmTextureFiles) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            F(int, s, 80) = n;
            if (n == 0)
                continue;
            F(BYTE**, s, 84) = (BYTE**)operator new(n * 4);
            for (int i = 0; i < n;)
            {
                if (i % 10 == 0 && F(Rm_Callback, s, 56))
                    F(Rm_Callback, s, 56)();
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                char* t = Rm_Token(&cur);
                if (mode)
                {
                    int size = Rm_ReadInt(f);
                    long pos = Rm_ftell(f);
                    if (t && strcmp(t, &g_rmNoload) == 0)
                    {
                        BYTE* o = (BYTE*)operator new(84);
                        if (o)
                            o = CallThis<BYTE*>(Rm_TexNamed, o, name, f, pos);
                        F(BYTE**, s, 84)[i] = o;
                        if (F(BYTE, s, 152) & 1)
                        {
                            Rm_TgaHeader(F(BYTE**, s, 84)[i], f, F(void*, s, 40));
                            F(DWORD, F(BYTE**, s, 84)[i], 52) &= ~1u;
                        }
                        else
                            Rm_TgaHeader(F(BYTE**, s, 84)[i], f, 0);
                        char* t3 = Rm_Token(&cur);
                        if (!t3 || strcmp(t3, &g_rmMemfix) != 0)
                            F(DWORD, F(BYTE**, s, 84)[i], 52) |= 0x400;
                        if (t3)
                            Rm_Token(&cur);
                        F(DWORD, F(BYTE**, s, 84)[i], 52) |= 4;
                    }
                    else
                    {
                        int flags = 0;
                        if (!t || strcmp(t, &g_rmMemfix) != 0)
                            flags = 1028;
                        flags |= 1;
                        if (F(BYTE, s, 152) & 1)
                        {
                            BYTE* o = (BYTE*)operator new(84);
                            if (o)
                                o = CallThis<BYTE*>(Rm_TexNamed, o, name, f, pos);
                            F(BYTE**, s, 84)[i] = o;
                            Rm_TgaHeader(F(BYTE**, s, 84)[i], f, F(void*, s, 40));
                            F(DWORD, F(BYTE**, s, 84)[i], 52) |= flags;
                        }
                        else
                            F(BYTE**, s, 84)[i] = (BYTE*)Rm_ImageFromFile(f, name, flags);
                        if (t)
                        {
                            if (strcmp(t, &g_rmBumpcoord) == 0)
                                F(int, F(BYTE**, s, 84)[i], 64) = 8;
                            else
                                F(int, F(BYTE**, s, 84)[i], 56) = atoi(t);
                        }
                    }
                    Rm_fseek(f, pos + size, 0);
                }
                else if (t && strcmp(t, &g_rmNoload) == 0)
                {
                    BYTE* o = (BYTE*)operator new(84);
                    if (o)
                        o = CallThis<BYTE*>(Rm_TexCtor, o, name);
                    F(BYTE**, s, 84)[i] = o;
                    Rm_TgaRead(F(BYTE**, s, 84)[i], name);
                }
                else
                {
                    F(BYTE**, s, 84)[i] = (BYTE*)Rm_ImageLoad(name);
                    if (t && strcmp(t, &g_rmBumpcoord) == 0)
                        F(int, F(BYTE**, s, 84)[i], 64) = 8;
                }
                Rm_Add(name, F(BYTE**, s, 84)[i]);
                if (!F(BYTE**, s, 84)[i])
                    return name;
                F(int, F(BYTE**, s, 84)[i], 80) = F(int, s, 52);
                if (name)
                    i++;
            }
        }
        else if (strcmp(key, &g_rmMaterials) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            if (n == 0)
                continue;
            if (n != F(int, s, 72))
                return &g_rmNumMaterials;
            int at = 0;
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                if (!name)
                    continue;
                for (int j = 0, off = 0; j < n; j++, off += 124)
                {
                    if (strcmp(name, (char*)F(BYTE*, s, 76) + off) != 0)
                        continue;
                    if (!cur)
                        Rm_Fatal(&g_rmIncorrectMat, name);
                    strcpy(&g_rmMatText, cur);
                    if (!Rm_Material(&g_rmMatText, F(BYTE*, s, 76) + off))
                        return name;
                    if (strncmp(name, &g_rmMatRoad, 8) == 0)
                    {
                        int c = (signed char)name[8];
                        if (c > 0)
                            c -= 47;
                        if (c >= 0 && c < 6)
                            F(int, s, 156 + c * 4) = j;
                    }
                    Rm_Add(name, F(BYTE*, s, 76) + at);
                }
                i++;
                at += 124;
            }
        }
        else if (strcmp(key, &g_rmPaletteFiles) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            F(int, s, 96) = n;
            if (n == 0)
                continue;
            F(void**, s, 100) = (void**)operator new(n * 4);
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                if (mode)
                {
                    int size = Rm_ReadInt(f);
                    long pos = Rm_ftell(f);
                    F(void**, s, 100)[i] = Rm_PalFromFile(f);
                    Rm_fseek(f, pos + size, 0);
                }
                else
                    F(void**, s, 100)[i] = Rm_PalLoad(name);
                Rm_Add(name, F(void**, s, 100)[i]);
                if (!F(void**, s, 100)[i])
                    return name;
                if (name)
                    i++;
            }
        }
        else if (strcmp(key, &g_rmBackFiles) == 0)
        {
            int n = atoi(Rm_Token(&cur));
            F(int, s, 88) = n;
            if (n == 0)
                continue;
            F(BYTE**, s, 92) = (BYTE**)operator new(n * 4);
            for (int i = 0; i < n;)
            {
                Rm_ReadLine(mode, f, 80);
                cur = &g_rmLine;
                char* name = Rm_Token(&cur);
                char* t = Rm_Token(&cur);
                if (mode)
                {
                    int size = Rm_ReadInt(f);
                    long pos = Rm_ftell(f);
                    if (t && strcmp(t, &g_rmNoload) == 0)
                    {
                        BYTE* o = (BYTE*)operator new(84);
                        if (o)
                            o = CallThis<BYTE*>(Rm_TexNamed, o, name, f, pos);
                        F(BYTE**, s, 92)[i] = o;
                        Rm_TgaHeader(F(BYTE**, s, 92)[i], f, 0);
                    }
                    else
                        F(BYTE**, s, 92)[i] = (BYTE*)Rm_ImageFromFile(f, 0, 0);
                    Rm_fseek(f, pos + size, 0);
                }
                else if (t && strcmp(t, &g_rmNoload) == 0)
                {
                    BYTE* o = (BYTE*)operator new(84);
                    if (o)
                        o = CallThis<BYTE*>(Rm_TexCtor, o, name);
                    F(BYTE**, s, 92)[i] = o;
                    Rm_TgaRead(F(BYTE**, s, 92)[i], name);
                }
                else
                    F(BYTE**, s, 92)[i] = (BYTE*)Rm_ImageLoad(name);
                Rm_Add(name, F(BYTE**, s, 92)[i]);
                if (!F(BYTE**, s, 92)[i])
                    return name;
                if (name)
                    i++;
            }
        }
        else
        {
            sprintf(&g_rmLine, &g_rmIllegal, key);
            return &g_rmLine;
        }
        if (RM_EOF(f))
            break;
    }

    // $L_5ed597: the materials' textures, masks and palette shades
    BYTE* pal = 0;
    if (F(BYTE**, s, 100) && *F(BYTE**, s, 100))
        pal = *F(BYTE**, s, 100) + 24;
    for (int k = 0, off = 0; k < F(int, s, 72); k++, off += 124)
    {
        BYTE* m = F(BYTE*, s, 76) + off;
        if (m[32] & 1)
            F(BYTE*, m, 76) = F(BYTE**, s, 84)[F(int, m, 56)];
        m = F(BYTE*, s, 76) + off;
        if (m[32] & 16)
            F(BYTE*, m, 72) = F(BYTE**, s, 108)[F(int, m, 60)];
        m = F(BYTE*, s, 76) + off;
        if (pal)
        {
            int idx = F(int, m, 40);
            DWORD b = pal[idx * 3];
            F(DWORD, m, 68) = (b << 8 | b) << 8 | b;
        }
    }
    return 0;
}

} // extern "C"

// === a mesh's draw data built from its faces (FUN_5f0b10 with its pieces $L_5f0d09, $L_5f0d3c, $L_5f0d41)

extern "C" void __fastcall Mb_FaceDestroy(BYTE* f); KING_ALIAS("@Mb_FaceDestroy@4", "@Fw5e57a0_Destroy@4")
extern "C" int __fastcall Mb_SetFlags(BYTE* s, void*, BYTE* nodes, int idx); KING_ALIAS("@Mb_SetFlags@16", "@MeshDraw_SetFlags@16")
extern "C" int __fastcall Mb_BuildVerts(BYTE* s, void*, BYTE* verts, BYTE* nodes, const BYTE* colors, int); KING_ALIAS("@Mb_BuildVerts@24", "@Mesh_BuildVertices@24")
KING_VAR(float, g_mb64c250, 64c250)
KING_VAR(float, g_mb64c458, 64c458)
KING_VAR(double, g_mb64eba8, 64eba8)

// the faces' corners (32 bytes each, three per face at the face's +src) copied into one array at s +dst, their
// layer coordinates (8 bytes, s +72 layers, from the face's +68) into s +76, and when lit (+80) the corner's own
// +24 pair followed by its layer pairs
static void Mb_Corners(BYTE* s, int dst, int src, int lit)
{
    F(WORD, s, 60) = (WORD)(F(WORD, s, 52) * 3);
    unsigned n3 = F(WORD, s, 60);
    BYTE* p = (BYTE*)operator new(n3 * 32);
    F(BYTE*, s, dst) = p ? p : 0;
    BYTE nl = F(BYTE, F(BYTE*, s, 56), 64);
    F(BYTE, s, 72) = nl;
    if (nl > 0)
        F(BYTE*, s, 76) = (BYTE*)operator new(nl * F(WORD, s, 52) * 24);
    if (lit)
        F(BYTE*, s, 80) = (BYTE*)operator new((F(BYTE, s, 72) + 1) * F(WORD, s, 52) * 24);
    int at = 0;
    for (int i = 0, fo = 0; i < F(WORD, s, 52); i++, fo += 88)
    {
        BYTE* c = F(BYTE*, F(BYTE*, s, 56) + fo, src);
        memcpy(F(BYTE*, s, dst) + at, c, 32);
        memcpy(F(BYTE*, s, dst) + at + 32, F(BYTE*, F(BYTE*, s, 56) + fo, src) + 32, 32);
        memcpy(F(BYTE*, s, dst) + at + 64, F(BYTE*, F(BYTE*, s, 56) + fo, src) + 64, 32);
        at += 96;
        int k0 = 3 * i, k1 = 3 * i + 1, k2 = 3 * i + 2;
        if (lit)
        {
            for (int k = 0; k < 3; k++)
            {
                int st = F(BYTE, s, 72) + 1;
                int kk = k == 0 ? k0 : k == 1 ? k1 : k2;
                BYTE* cc = F(BYTE*, F(BYTE*, s, 56) + fo, src);
                F(DWORD, F(BYTE*, s, 80), st * kk * 8) = F(DWORD, cc, 32 * k + 24);
                st = F(BYTE, s, 72) + 1;
                cc = F(BYTE*, F(BYTE*, s, 56) + fo, src);
                F(DWORD, F(BYTE*, s, 80), st * kk * 8 + 4) = F(DWORD, cc, 32 * k + 28);
            }
        }
        for (int j = 0; j < F(BYTE, s, 72); j++)
        {
            int l = F(BYTE, s, 72);
            BYTE* uv = F(BYTE*, F(BYTE*, s, 56) + fo, 68);
            F(DWORD, F(BYTE*, s, 76), (l * k0 + j) * 8) = F(DWORD, uv, j * 8);
            F(DWORD, F(BYTE*, s, 76), (l * k0 + j) * 8 + 4) = F(DWORD, uv, j * 8 + 4);
            F(DWORD, F(BYTE*, s, 76), (l * k1 + j) * 8) = F(DWORD, uv, (l + j) * 8);
            F(DWORD, F(BYTE*, s, 76), (l * k1 + j) * 8 + 4) = F(DWORD, uv, (l + j) * 8 + 4);
            F(DWORD, F(BYTE*, s, 76), (l * k2 + j) * 8) = F(DWORD, uv, (j + 2 * l) * 8);
            F(DWORD, F(BYTE*, s, 76), (l * k2 + j) * 8 + 4) = F(DWORD, uv, (j + 2 * l) * 8 + 4);
            if (lit)
            {
                const int ks[3] = { k0, k1, k2 };
                for (int k = 0; k < 3; k++)
                {
                    int m = F(BYTE, s, 72);
                    F(DWORD, F(BYTE*, s, 80), ((m + 1) * ks[k] + j) * 8 + 8) = F(DWORD, F(BYTE*, s, 76), (m * ks[k] + j) * 8);
                    m = F(BYTE, s, 72);
                    F(DWORD, F(BYTE*, s, 80), ((m + 1) * ks[k] + j) * 8 + 12) = F(DWORD, F(BYTE*, s, 76), (m * ks[k] + j) * 8 + 4);
                }
            }
        }
    }
    BYTE* faces = F(BYTE*, s, 56);
    if (faces)
    {
        int cnt = F(int, faces, -4);
        for (int k = cnt - 1; k >= 0; k--)
            Mb_FaceDestroy(faces + k * 88);
        operator delete(faces - 4);
    }
    F(BYTE*, s, 56) = 0;
}

extern "C" {

// [FUN_5f0b10..$L_5f0d41] mesh s (+52 its face count, +56 the faces of 88 bytes, +96 its kind, +98 the root
// node) prepared from model a (+16 the nodes of 124 bytes, +20 the colors, +4/+8/+12 the vertex lights): kind 3
// with lights packs each light's color scaled by the root's color and alpha into it; other kinds build each
// face's vertices. Then kind 2 copies the corners to +88 (from the face's +80), kind 1 to +84 (from +76), with
// the layer coordinates and lit pairs when a node on the root's chain has flags 2060, and frees the faces
void __fastcall Mesh_PrepareDraw(BYTE* s, void*, BYTE* a)
{
    BYTE* nodes = F(BYTE*, a, 16);
    BYTE* colors = F(BYTE*, a, 20);
    if (!F(WORD, s, 52))
        return;
    if (F(WORD, s, 96) == 3)
    {
        if (F(int, a, 4) == 3)
        {
            float alpha = 1.0f;
            BYTE* nd = nodes + F(WORD, s, 98) * 124;
            int ci = F(int, nd, 40) + 8;
            const BYTE* rgb = colors + ci * 2 + ci;
            DWORD fl = F(DWORD, nd, 32);
            if (fl & 32)
                alpha = (float)(g_mb64c250 - F(float, nd, 88));
            double r;
            float g, b;
            if (fl & 1)
            {
                r = g_mb64c250;
                b = 1.0f;
                g = 1.0f;
            }
            else
            {
                r = rgb[0] * g_mb64eba8;
                g = (float)(rgb[1] * g_mb64eba8);
                b = (float)(rgb[2] * g_mb64eba8);
            }
            if (F(int, a, 8) > 0)
            {
                DWORD hi = (DWORD)(int)(alpha * g_mb64c458) << 8;
                for (int i = 0, off = 0; i < F(int, a, 8); i++, off += 12)
                {
                    BYTE* v = F(BYTE*, a, 12) + off;
                    DWORD c = (DWORD)(int)(r * F(float, v, 0) * g_mb64c458) | hi;
                    c <<= 8;
                    c |= (DWORD)(int)(g * F(float, v, 0) * g_mb64c458);
                    c <<= 8;
                    c |= (DWORD)(int)(b * F(float, v, 0) * g_mb64c458);
                    F(DWORD, v, 0) = c;
                }
            }
        }
    }
    else
    {
        for (int i = 0, off = 0; i < F(WORD, s, 52); i++, off += 88)
            Mb_BuildVerts(F(BYTE*, s, 56) + off, 0, F(BYTE*, s, 48), nodes, colors, 0);
    }

    int lit = 0;
    for (BYTE* nd = nodes + F(WORD, s, 98) * 124; ; )
    {
        if (F(DWORD, nd, 32) & 2060)
            lit = 1;
        int next = F(int, nd, 44);
        if (next < 0)
            break;
        nd = nodes + next * 124;
        if (!nd)
            break;
    }
    if (F(WORD, s, 96) == 1)
        Mb_Corners(s, 84, 76, lit);
    else if (F(WORD, s, 96) == 2)
        Mb_Corners(s, 88, 80, lit);
    if (F(WORD, s, 96) != 0)
        Mb_SetFlags(s + 60, 0, nodes, F(WORD, s, 98));
}

} // extern "C"

// === the QUIT / RESTART panel ($L_5ddc50 with its tail $L_5ddd68)

extern "C" BYTE* __cdecl Qd_FontLoad(const char* name); KING_ALIAS("_Qd_FontLoad", "_Font_LoadFile")
extern "C" void __cdecl Qd_Exit(const char* msg); KING_ALIAS("_Qd_Exit", "_Sys_Exit")
extern "C" DWORD __fastcall Qd_StyleAssign(void* self, void*, const void* s); KING_ALIAS("@Qd_StyleAssign@12", "@Style_Assign@12")
extern "C" BYTE* __fastcall Qd_PanelCtor(BYTE* s, void*, const char* name, int a, BYTE* font, int b, int c); KING_ALIAS("@Qd_PanelCtor@28", "@Panel_Construct@28")
extern "C" BYTE* __fastcall Qd_ControlCtor(BYTE* s, void*, const char* name, int id); KING_ALIAS("@Qd_ControlCtor@16", "@Control_Construct@16")
extern "C" BYTE* __fastcall Qd_ButtonCtor(BYTE* s, void*, const char* text, int flags, int id); KING_ALIAS("@Qd_ButtonCtor@20", "@Button_ConstructId@20")
extern "C" void __fastcall Qd_AddChild(BYTE* self, void*, BYTE* child, BYTE* after); KING_ALIAS("@Qd_AddChild@16", "@Ctl_AddChild@16")
extern "C" BYTE* __fastcall Qd_NewChild(BYTE* self, void*, BYTE* w); KING_ALIAS("@Qd_NewChild@12", "@Fw_NewChild64@12")
KING_VAR(char, g_qdMenuSft, 682ed4)       // "menu.sft"
KING_VAR(char, g_qdMenuFon, 676220)       // "menu.fon"
KING_VAR(char, g_qdNoFont, 682eb8)        // "menu.sft(fon) is absent."
KING_VAR(char, g_qdQuit, 682eb0)          // "QUIT"
KING_VAR(char, g_qdRestart, 682ea8)       // "RESTART"
KING_VAR(char, g_qd68d1c0, 68d1c0)
KING_VAR(char, g_qd67663c, 67663c)
KING_VAR(BYTE, g_qdPanelVt, 6524d8)
KING_VAR(BYTE, g_qdMenuVt, 64dc50)
KING_VAR(int, g_qd720938, 720938)
KING_VAR(int, g_qd72093c, 72093c)
KING_VAR(int, g_qd720940, 720940)
KING_VAR(int, g_qd720944, 720944)
KING_VAR(BYTE*, g_qd721750, 721750)

// a control style (36 bytes) as its constructor leaves it
static void Qd_Style(DWORD* st)
{
    memset(st, 0, 36);
    ((float*)st)[6] = 0.5f;
}

static void Qd_Styles(BYTE* b, DWORD* s1, DWORD* s2, DWORD* s3, DWORD* s4)
{
    Qd_StyleAssign(b + 68, 0, s1);
    Qd_StyleAssign(b + 104, 0, s2);
    Qd_StyleAssign(b + 140, 0, s3);
    Qd_StyleAssign(b + 176, 0, s4);
}

extern "C" {

// [$L_5ddc50..$L_5ddd68] (app vtable) app s's quit panel: the menu font (+2864, loaded once), a panel with the
// buttons QUIT (4) and RESTART (3) centred in the app's window (vtable +20 width, +24 height), shown as +2716/+2856
void __fastcall App_ShowQuitPanel(BYTE* s, void*)
{
    DWORD st1[9], st2[9], st3[9], st5[9];
    Qd_Style(st2);
    Qd_Style(st1);
    Qd_Style(st3);
    Qd_Style(st5);
    if (F(BYTE*, s, 2864) == 0)
    {
        BYTE* f = Qd_FontLoad(&g_qdMenuSft);
        if (!f)
        {
            f = Qd_FontLoad(&g_qdMenuFon);
            if (!f)
                Qd_Exit(&g_qdNoFont);
        }
        F(BYTE*, s, 2864) = f;
    }
    st1[1] = 0;
    st1[0] = 2;
    st1[3] = 0;
    st1[2] = 4;
    Qd_StyleAssign(st2, 0, st1);
    st2[1] = 5;
    Qd_StyleAssign(st3, 0, st2);
    st3[3] = 4;
    st3[2] = 0;
    Qd_StyleAssign(st5, 0, st3);
    st5[1] = 0;

    BYTE* panel = (BYTE*)operator new(304);
    if (panel)
    {
        Qd_PanelCtor(panel, 0, &g_qd68d1c0, 0, F(BYTE*, s, 2864), 4, 0);
        F(void*, panel, 0) = &g_qdPanelVt;
        F(BYTE*, panel, 300) = s;
    }
    BYTE* menu = (BYTE*)operator new(296);
    if (menu)
    {
        Qd_ControlCtor(menu, 0, &g_qd67663c, 0);
        F(int, menu, 288) = 0;
        F(int, menu, 292) = 0;
        F(void*, menu, 0) = &g_qdMenuVt;
    }
    BYTE* b = (BYTE*)operator new(288);
    if (b)
        b = Qd_ButtonCtor(b, 0, &g_qdQuit, 1, 4);
    Qd_Styles(b, st1, st2, st5, st3);
    Qd_AddChild(menu, 0, b, 0);
    b = (BYTE*)operator new(288);
    if (b)
        b = Qd_ButtonCtor(b, 0, &g_qdRestart, 1, 3);
    Qd_Styles(b, st1, st2, st5, st3);
    Qd_AddChild(menu, 0, b, 0);
    CallVirt(menu, 20);
    int h = CallVirt<int>(s, 24);
    int dy = (h + (F(int, menu, 48) - F(int, menu, 56))) / 3;
    int w = CallVirt<int>(s, 20);
    int dx = (w + (F(int, menu, 44) - F(int, menu, 52))) / 2;
    F(int, menu, 44) += dx;
    F(int, menu, 48) += dy;
    F(int, menu, 52) += dx;
    F(int, menu, 56) += dy;
    Qd_AddChild(panel, 0, menu, 0);
    F(BYTE*, s, 2716) = panel;
    F(int, panel, 44) = g_qd720938;
    F(int, panel, 52) = g_qd720940;
    F(int, panel, 48) = g_qd72093c;
    F(int, panel, 56) = g_qd720944;
    F(BYTE*, s, 2856) = Qd_NewChild(g_qd721750, 0, F(BYTE*, s, 2716));
    CallVirt(F(BYTE*, s, 2856), 32);
}

} // extern "C"

// === the in-game Esc menu window ($L_511170 with its tail $L_5112b7)

extern "C" int* __cdecl Ow_RectBelow(int* out, void* window); KING_ALIAS("_Ow_RectBelow", "_Carat_WindowRectBelow")
extern "C" BYTE* __fastcall Ow_WindowCtor(BYTE* s, void*, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8); KING_ALIAS("@Ow_WindowCtor@40", "@ControlWindow_Construct@40")
extern "C" void __fastcall Ow_BoxInit(void* b, void*); KING_ALIAS("@Ow_BoxInit@8", "@Box_Init@8")
extern "C" BYTE* __cdecl Ow_FontLoad(const char* name); KING_ALIAS("_Ow_FontLoad", "_Font_LoadFile")
extern "C" void __cdecl Ow_Exit(const char* msg); KING_ALIAS("_Ow_Exit", "_Sys_Exit")
extern "C" DWORD __fastcall Ow_StyleAssign(void* self, void*, const void* s); KING_ALIAS("@Ow_StyleAssign@12", "@Style_Assign@12")
extern "C" BYTE* __cdecl Ow_NewObjRef(); KING_ALIAS("_Ow_NewObjRef", "_Shell_NewObjRef")
extern "C" int __cdecl Ow_Mode(); KING_ALIAS("_Ow_Mode", "_Game_568fd0")
extern "C" BYTE* __cdecl Ow_Find(const char* name); KING_ALIAS("_Ow_Find", "_Objects_Find")
extern "C" BYTE* __fastcall Ow_ScreenCtor(BYTE* s, void*, const char* name, int id); KING_ALIAS("@Ow_ScreenCtor@16", "@Screen_Construct@16")
extern "C" void __fastcall Ow_SetBoxes(BYTE* s, void*, void* a, void* b, void* c, void* d); KING_ALIAS("@Ow_SetBoxes@24", "@Screen_SetBoxes@24")
extern "C" BYTE* __cdecl Ow_Button(const char* name, int id); KING_ALIAS("_Ow_Button", "_Shell_ModeButtonC")
extern "C" void __fastcall Ow_AddChild(BYTE* self, void*, BYTE* child, BYTE* after); KING_ALIAS("@Ow_AddChild@16", "@Ctl_AddChild@16")
extern "C" void __fastcall Ow_LayoutRow(BYTE* self, void*, int gap); KING_ALIAS("@Ow_LayoutRow@12", "@Ctl_LayoutRow@12")
extern "C" void __fastcall Ow_RectOffset(void* r, void*, int dx, int dy); KING_ALIAS("@Ow_RectOffset@16", "@Rect_Offset@16")
extern "C" void __fastcall Ow_RectMoveTo(void* r, void*, int x, int y); KING_ALIAS("@Ow_RectMoveTo@16", "@Rect_MoveTo@16")
extern "C" BYTE* __fastcall Ow_WindowsAdd(BYTE* self, void*, BYTE* w); KING_ALIAS("@Ow_WindowsAdd@12", "@Windows_Add@12")
KING_VAR(void*, g_ow69684c, 69684c)
KING_VAR(int, g_ow6ced24, 6ced24)
KING_VAR(BYTE, g_owVt64ddc0, 64ddc0)
KING_VAR(BYTE, g_owVt64dd80, 64dd80)
KING_VAR(char, g_owMenuFon, 676220)
KING_VAR(char, g_owNoFont, 676820)
KING_VAR(BYTE, g_ow6ce3b8, 6ce3b8)
KING_VAR(BYTE, g_ow6ce408, 6ce408)
KING_VAR(BYTE, g_ow6ce988, 6ce988)
KING_VAR(BYTE, g_ow6ceb38, 6ceb38)
KING_VAR(BYTE, g_ow6ce3e0, 6ce3e0)
KING_VAR(char, g_owBack15, 676808)       // "m_15\\BackEscMain.txr"
KING_VAR(char, g_owBack16, 6767f0)       // "m_16\\BackEscMain.txr"
KING_VAR(BYTE*, g_ow6ce3fc, 6ce3fc)
KING_VAR(char, g_ow67663c, 67663c)
KING_VAR(char, g_owExitsys, 6767e8)
KING_VAR(char, g_owMain, 6767e0)
KING_VAR(char, g_owSound, 6767d8)
KING_VAR(char, g_owControl, 6767d0)
KING_VAR(char, g_ow911, 6767cc)
KING_VAR(char, g_owRestart, 6767c4)
KING_VAR(char, g_owLoadg, 6767bc)
KING_VAR(char, g_owSavg, 6767b4)
KING_VAR(BYTE*, g_ow6cec6c, 6cec6c)
KING_VAR(int, g_ow6ced0c, 6ced0c)
KING_VAR(int, g_ow6ced10, 6ced10)
KING_VAR(BYTE*, g_ow6ced6c, 6ced6c)
KING_VAR(BYTE, g_owWindows696750, 696750)

extern "C" {

// [$L_511170..$L_5112b7] (app vtable) app s's Esc menu: a control window below the main one, the menu font
// (+2864), the background (m_15 or m_16 by the game mode), a screen with the buttons exitsys, main, sound,
// control, 911, restart, loadg and savg (the last three disabled (+40) when the controls screen's +1316 is set)
// laid out in a row; shown as +2716 / +2856
void __fastcall App_ShowEscMenu(BYTE* s, void*)
{
    DWORD st1[9], st2[9], st3[9], box[9];
    int rect[4];
    BYTE* w = (BYTE*)operator new(376);
    if (w)
    {
        int* r = Ow_RectBelow(rect, g_ow69684c);
        Ow_WindowCtor(w, 0, (int)g_ow69684c, r[0], r[1], r[2], r[3], 0, g_ow6ced24, 0);
        F(void*, w, 0) = &g_owVt64ddc0;
        F(void*, w, 64) = &g_owVt64dd80;
        memset(st1, 0, sizeof st1);
        ((float*)st1)[6] = 0.5f;
        memset(st2, 0, sizeof st2);
        ((float*)st2)[6] = 0.5f;
        memset(st3, 0, sizeof st3);
        ((float*)st3)[6] = 0.5f;
        Ow_BoxInit(box, 0);
        BYTE* f = Ow_FontLoad(&g_owMenuFon);
        if (!f)
            Ow_Exit(&g_owNoFont);
        F(BYTE*, s, 2864) = f;
        Ow_StyleAssign(st1, 0, &g_ow6ce3b8);
        Ow_StyleAssign(st2, 0, &g_ow6ce408);
        Ow_StyleAssign(st3, 0, &g_ow6ce988);
        Ow_StyleAssign(box, 0, &g_ow6ceb38);
        BYTE* ref = Ow_NewObjRef();
        F(BYTE*, w, 52) = ref;
        F(BYTE*, w, 48) = ref;
        F(BYTE*, w, 368) = s;
        g_ow6ce3fc = Ow_Find(Ow_Mode() == 2 ? &g_owBack15 : &g_owBack16);

        BYTE* back = (BYTE*)operator new(296);
        if (back)
            back = Ow_ScreenCtor(back, 0, &g_ow67663c, 4);
        Ow_SetBoxes(back, 0, &g_ow6ce3e0, &g_ow6ce3e0, &g_ow6ce3e0, &g_ow6ce3e0);
        BYTE* row = (BYTE*)operator new(296);
        if (row)
            row = Ow_ScreenCtor(row, 0, &g_ow67663c, 0);
        Ow_SetBoxes(row, 0, &g_ow6ce3e0, &g_ow6ce3e0, &g_ow6ce3e0, &g_ow6ce3e0);
        Ow_AddChild(row, 0, Ow_Button(&g_owExitsys, 169), 0);
        BYTE* mainb = Ow_Button(&g_owMain, 163);
        Ow_AddChild(row, 0, mainb, 0);
        Ow_AddChild(row, 0, Ow_Button(&g_owSound, 164), 0);
        Ow_AddChild(row, 0, Ow_Button(&g_owControl, 165), 0);
        Ow_AddChild(row, 0, Ow_Button(&g_ow911, 166), 0);
        BYTE* b = Ow_Button(&g_owRestart, 168);
        Ow_AddChild(row, 0, b, 0);
        if (F(int, g_ow6cec6c, 1316) != 0)
            F(int, b, 40) = 1;
        b = Ow_Button(&g_owLoadg, 110);
        Ow_AddChild(row, 0, b, 0);
        if (F(int, g_ow6cec6c, 1316) != 0)
            F(int, b, 40) = 1;
        b = Ow_Button(&g_owSavg, 167);
        Ow_AddChild(row, 0, b, 0);
        if (F(int, g_ow6cec6c, 1316) != 0)
            F(int, b, 40) = 1;
        Ow_LayoutRow(row, 0, F(int, mainb, 48) - F(int, mainb, 56) + 45);
        Ow_AddChild(back, 0, row, 0);
        CallVirt(back, 20);
        Ow_RectOffset(row + 44, 0, 18, 454);
        Ow_RectMoveTo(back + 44, 0, g_ow6ced0c, g_ow6ced10);
        Ow_AddChild(w + 64, 0, back, 0);
        F(BYTE*, s, 2716) = w + 64;
    }
    g_ow6ced6c = w;
    F(BYTE*, s, 2856) = w;
    Ow_WindowsAdd(&g_owWindows696750, 0, w);
    CallVirt(F(BYTE*, s, 2856), 32);
}

} // extern "C"

// === a module (*.b3d with its resources and overlays) loaded (FUN_5dae90 with its pieces up to $L_5db637)

extern "C" BYTE* __fastcall Rl_ClusterCtor(BYTE* s, void*, const char* name); KING_ALIAS("@Rl_ClusterCtor@12", "@Cluster5ea880_Construct@12")
extern "C" void __fastcall Rl_NodeLink(BYTE* self, void*, BYTE* list); KING_ALIAS("@Rl_NodeLink@12", "@Node_Link@12")
extern "C" void __cdecl Rl_Fatal(const char* fmt, ...); KING_ALIAS("_Rl_Fatal", "_Sys_Fatal")
extern "C" void* __cdecl Rl_fopen(const char* name, const char* mode); KING_ALIAS("_Rl_fopen", "_fopen")
extern "C" int __cdecl Rl_fclose(void* f); KING_ALIAS("_Rl_fclose", "_fclose")
extern "C" int __fastcall Rl_ReadFile(BYTE* c, void*, void* f); KING_ALIAS("@Rl_ReadFile@12", "@Cluster_ReadFile@12")
extern "C" int __cdecl Rl_FreeMemory(); KING_ALIAS("_Rl_FreeMemory", "_Sys_FreeMemory")
extern "C" const char* __cdecl Rl_SoundStart(); KING_ALIAS("_Rl_SoundStart", "_Sound_Start")
extern "C" int __cdecl Rl_LoadAny(BYTE* img, const char* dir, char* path); KING_ALIAS("_Rl_LoadAny", "_Image_LoadAny")
extern "C" BYTE* __fastcall Rl_OverlaysLoad(BYTE* s, void*, const char* path, BYTE* o, int size); KING_ALIAS("@Rl_OverlaysLoad@20", "@Overlays_Load@20")
extern "C" int __fastcall Rl_TexCreate(BYTE* t, void*, unsigned flags); KING_ALIAS("@Rl_TexCreate@12", "@Texture_Create@12")
extern "C" void __fastcall Rl_RleTo16(BYTE* self, void*, BYTE* pal); KING_ALIAS("@Rl_RleTo16@12", "@Image_RleTo16@12")
extern "C" void __fastcall Rl_RoomEach(BYTE* self, void*, void* fn, void* arg); KING_ALIAS("@Rl_RoomEach@16", "@Room_Each@16")
extern "C" int __fastcall Rl_RleToSurface(BYTE* self, void*); KING_ALIAS("@Rl_RleToSurface@8", "@Image_RleToSurface@8")
extern "C" const char* __cdecl Rl_DdError(DWORD hr, const char* where); KING_ALIAS("_Rl_DdError", "_Dd_ErrorText")
extern "C" void __cdecl Rl_Log(const void* text, int value); KING_ALIAS("_Rl_Log", "_Sys_Log")
extern "C" void Rl_NodeDispatch(); KING_ALIAS("_Rl_NodeDispatch", "_Node_Dispatch")
typedef long(__stdcall* Rl_SetColorKey)(void* surf, DWORD flags, void* key);

KING_VAR(char, g_rlName720b60, 720b60)
KING_VAR(char, g_rlName720b61, 720b61)
KING_VAR(char*, g_rl7209cc, 7209cc)
KING_VAR(char*, g_rl7209d0, 7209d0)
KING_VAR(char, g_rlBase720bb4, 720bb4)
KING_VAR(char, g_rlPath721218, 721218)
KING_VAR(char, g_rlDot671204, 671204)        // ".\\"
KING_VAR(char, g_rlBadName, 682d80)          // "Incorrect b3d file name"
KING_VAR(char, g_rlTooLong, 682d68)          // "The path is too long."
KING_VAR(char, g_rlRb, 6703e8)               // "rb"
KING_VAR(char, g_rlNotFound, 682d58)         // " is not found"
KING_VAR(char, g_rlNoMemory, 682ca4)         // "Not enough memory\n"
KING_VAR(char, g_rlBadB3d, 682d48)           // "*.b3d is bad"
KING_VAR(char, g_rlNoPalMasks, 682d20)       // "Module has masks and has not palette"
KING_VAR(char, g_rlNoPalBase, 682d00)        // "Base module have not palette"
KING_VAR(char, g_rlCheckFormat, 682ce0)      // "check format(resolution) of "
KING_VAR(BYTE*, g_rl71e82c, 71e82c)
KING_VAR(BYTE*, g_rl721750, 721750)
KING_VAR(int, g_rl720904, 720904)
KING_VAR(int, g_rl7208c4, 7208c4)

static void Rl_SetDir(const char* dir)
{
    if (strcmp(dir, &g_rlDot671204) == 0)
        g_rlPath721218 = 0;
    else
        strcpy(&g_rlPath721218, dir);
}

extern "C" {

// [FUN_5dae90..$L_5db637] the module file name (quotes removed) loaded from dir into a new cluster linked into s
// +468: its *.b3d read, its image file (ext, or the b3d name) loaded, its overlays (*.ovl) when found, the
// materials' texture flags, the masks converted with the base palette, and with 16-bit textures (720904) each
// textured material's texture created and its color key set. Returns the cluster (0 when its image failed)
BYTE* __fastcall Module_Load(BYTE* s, void*, const char* dir, const char* name, const char* ext)
{
    strcpy(&g_rlName720b60, name);
    g_rl7209cc = &g_rlName720b60;
    if (g_rlName720b60 == '"')
    {
        g_rl7209cc = &g_rlName720b61;
        (&g_rlName720b60)[strlen(&g_rlName720b61)] = 0;
    }
    strcpy(&g_rlBase720bb4, g_rl7209cc);
    if (strlen(&g_rlBase720bb4) < 5)
        Rl_Fatal(&g_rlBadName, 0);
    (&g_rlBase720bb4)[strlen(&g_rlBase720bb4) - 4] = 0;
    BYTE* r = (BYTE*)operator new(180);
    r = r ? Rl_ClusterCtor(r, 0, &g_rlBase720bb4) : 0;
    BYTE* old = F(BYTE*, s, 468);
    if (!old)
        F(BYTE*, s, 468) = r;
    else if (F(BYTE*, old, 100))
        Rl_NodeLink(r, 0, old);
    else
    {
        F(BYTE*, s, 468) = r;
        Rl_NodeLink(old, 0, r);
    }
    if (strlen(g_rl7209cc) + strlen(dir) + 1 >= 128)
        Rl_Fatal(&g_rlTooLong, 0);
    Rl_SetDir(dir);
    strcat(&g_rlPath721218, g_rl7209cc);
    void* f = Rl_fopen(&g_rlPath721218, &g_rlRb);
    if (!f)
    {
        strcat(&g_rlPath721218, &g_rlNotFound);
        Rl_Fatal(&g_rlPath721218, 0);
    }
    if (F(Rm_Callback, s, 536))
        F(Rm_Callback, s, 536)();
    F(void*, r, 56) = F(void*, s, 536);
    g_rl71e82c = r;
    if (!Rl_ReadFile(r, 0, f))
        Rl_Fatal(Rl_FreeMemory() < 500000 ? &g_rlNoMemory : &g_rlBadB3d, 0);
    Rl_fclose(f);

    // the image file: ext from dir, else the b3d's own name, up to its '.'
    if (ext)
    {
        Rl_SetDir(dir);
        strcat(&g_rlPath721218, ext);
    }
    char* p = &g_rlPath721218;
    while (*p && *p != '.')
    {
        p++;
        g_rl7209d0 = p;
    }
    *p = '.';
    g_rl7209d0 = &g_rlPath721218;
    if (F(void*, g_rl721750, 344))
        Rl_SoundStart();
    int img = Rl_LoadAny(r, dir, g_rl7209d0);
    if (!img)
    {
        if (F(BYTE*, s, 468) == r)
            F(BYTE*, s, 468) = 0;
        if (r)
            CallVirt(r, 0, 1);
        r = 0;
    }

    // the overlays: the path's extension replaced by "ovl"
    p = &g_rlPath721218;
    g_rl7209d0 = p;
    if (*p)
    {
        do
        {
            p++;
            g_rl7209d0 = p;
        }
        while (*p != 0);
    }
    if (*p != '.')
    {
        while (p > &g_rlPath721218)
        {
            p--;
            g_rl7209d0 = p;
            if (*p == '.')
                break;
        }
    }
    if (*p == '.')
    {
        p++;
        g_rl7209d0 = p;
        *g_rl7209d0 = 'o';
        g_rl7209d0++;
        *g_rl7209d0 = 'v';
        g_rl7209d0++;
        *g_rl7209d0 = 'l';
        g_rl7209d0++;
        *g_rl7209d0 = 0;
        int keep = 1;
        if (img == 3 || (img == 2 && g_rl720904))
            keep = 0;
        BYTE* ov = (BYTE*)operator new(20);
        if (ov)
            ov = Rl_OverlaysLoad(ov, 0, &g_rlPath721218, r, keep);
        F(BYTE*, r, 136) = ov;
    }

    if (F(int, r, 88) && (F(DWORD, g_rl721750, 268) & 0x800))
        Rl_TexCreate(*F(BYTE**, r, 92), 0, 0);
    for (int i = 0, off = 0; i < F(int, r, 72); i++, off += 124)
    {
        BYTE* m = F(BYTE*, r, 76) + off;
        if (m[32] & 2)
            F(DWORD, F(BYTE**, r, 84)[F(int, m, 56)], 52) |= 8;
        m = F(BYTE*, r, 76) + off;
        if (m[32] & 64)
        {
            BYTE* t = F(BYTE**, r, 84)[F(int, m, 56)];
            if (!(F(DWORD, t, 52) & 8))
                F(DWORD, t, 52) |= 64;
        }
    }
    for (int i = 0; i < F(int, r, 104); i++)
    {
        if (!F(BYTE**, F(BYTE*, s, 468), 100))
            Rl_Fatal(&g_rlNoPalMasks, 0);
        Rl_RleTo16(F(BYTE**, r, 108)[i], 0, *F(BYTE**, F(BYTE*, s, 468), 100));
    }
    if (g_rl720904)
    {
        void* each[6] = { 0, 0, 0, 0, 0, 0 };
        if (!F(BYTE**, F(BYTE*, s, 468), 100))
            Rl_Fatal(&g_rlNoPalBase, 0);
        each[5] = *F(BYTE**, F(BYTE*, s, 468), 100);
        each[4] = F(void*, r, 76);
        Rl_RoomEach(r, 0, (void*)&Rl_NodeDispatch, each);
        for (int i = 0, off = 0; i < F(int, r, 72); i++, off += 124)
        {
            BYTE* m = F(BYTE*, r, 76) + off;
            if (!(F(DWORD, m, 32) & 1))
                continue;
            BYTE* t = F(BYTE*, m, 76);
            if (F(WORD, t, 26) == 0 && F(WORD, t, 20) != 1 && F(WORD, t, 22) != 1)
                Rl_Fatal(&g_rlCheckFormat, m);
            if (F(DWORD, m, 32) & 0x200)
                F(DWORD, t, 52) |= 0x100;
            DWORD fl = F(DWORD, t, 52);
            if (!(fl & 1) || (fl & 2))
                continue;
            if (!Rl_TexCreate(t, 0, fl >> 10 & 1))
                Rl_Log(m, i);
            if (F(int, t, 64) == 8)
                F(DWORD, m, 32) |= 0x80000;
            if ((F(DWORD, m, 32) & 0x80000) && !g_rl7208c4)
                F(int, t, 64) = 5;
            if (F(DWORD, t, 52) & 8)
            {
                const BYTE* c = *F(BYTE**, F(BYTE*, s, 468), 100) + (F(int, m, 40) + 8) * 3;
                DWORD key[2];
                DWORD v = ((DWORD)(c[0] & 0xF8) << 5 | (c[1] & 0xFFFFFFFC)) << 3 | (DWORD)c[2] >> 3;
                key[0] = v;
                key[1] = v;
                void* surf = F(void*, t, 72);
                long hr = (*(Rl_SetColorKey*)(*(BYTE**)surf + 116))(surf, 8, key);
                if (hr)
                    Rl_Log(Rl_DdError(hr, 0), 0);
            }
        }
        for (int i = 0; i < F(int, r, 104); i++)
        {
            BYTE* im = F(BYTE**, r, 108)[i];
            if (F(BYTE, im, 28) & 1)
                Rl_RleToSurface(im, 0);
        }
        for (int i = 0; i < F(int, r, 80); i++)
        {
            BYTE* t = F(BYTE**, r, 84)[i];
            DWORD fl = F(DWORD, t, 52);
            if ((fl & 1) && !(fl & 2))
                Rl_TexCreate(t, 0, ((fl & 1024) | 0x800) >> 10);
        }
    }
    void* probe = operator new(50000);
    if (!probe)
        Rl_Fatal(&g_rlNoMemory, 0);
    else
        operator delete(probe);
    g_rl71e82c = 0;
    return r;
}

} // extern "C"

// === the app's video mode, devices and main window set up (FUN_5e3bc0 with its tail $L_5e3c22)

ASM_PROC($L_5d7c20)    // tries a video mode (w, h, cres, flags, 3 outs)
extern "C" BYTE* __cdecl Dm_NewApp(); KING_ALIAS("_Dm_NewApp", "_Fw_NewApp384")
extern "C" void __cdecl Dm_DefaultMode(int* w, int* h, int* c, int* flags); KING_ALIAS("_Dm_DefaultMode", "_Dd_DefaultMode")
extern "C" void __cdecl Dm_TexFormat(int kind, int b); KING_ALIAS("_Dm_TexFormat", "_Dd_ChooseTextureFormat")
extern "C" int __cdecl Dm_ReadInt(const char* section, const char* key, int def, int* out, const char* file); KING_ALIAS("_Dm_ReadInt", "_Ini_ReadInt")
extern "C" int __cdecl Dm_GetDouble(const char* section, const char* key, double def, double* out, const char* file); KING_ALIAS("_Dm_GetDouble", "_Ini_GetDouble")
extern "C" int __cdecl Dm_SetMode(int w, int h, int bits, int windowed, int xs, int ys); KING_ALIAS("_Dm_SetMode", "_Video_SetMode")
extern "C" int __cdecl Dm_CreateDevices(DWORD flags); KING_ALIAS("_Dm_CreateDevices", "_Dd_CreateDevices")
extern "C" int __cdecl Dm_EffectZero(int); KING_ALIAS("_Dm_EffectZero", "_Effect_Zero")
extern "C" int __cdecl Dm_TimerCalibrate(int a); KING_ALIAS("_Dm_TimerCalibrate", "_Timer_Calibrate")
extern "C" void __fastcall Dm_SetMousePos(BYTE* self, void*, int x, int y); KING_ALIAS("@Dm_SetMousePos@16", "@CGameApp_SetMousePos@16")
extern "C" int __cdecl Dm_SoundOpen(); KING_ALIAS("_Dm_SoundOpen", "_Sound_Open")
extern "C" int __cdecl Dm_ScaleX(); KING_ALIAS("_Dm_ScaleX", "_Video_Get67fcf0")
extern "C" int __cdecl Dm_ScaleY(); KING_ALIAS("_Dm_ScaleY", "_Video_Get67fcf4")
extern "C" int __cdecl Dm_FrameExtra(int edge); KING_ALIAS("_Dm_FrameExtra", "_Wnd_FrameExtra")
extern "C" BYTE* __fastcall Dm_WindowCtor(BYTE* self, void*, int a, int b, int c, int d); KING_ALIAS("@Dm_WindowCtor@24", "@Fw5d8650_Construct@24")
extern "C" BYTE* __fastcall Dm_RootSurface(BYTE* self, void*); KING_ALIAS("@Dm_RootSurface@8", "@Window_RootSurface@8")
extern "C" void __fastcall Dm_PaletteApply(BYTE* self, void*); KING_ALIAS("@Dm_PaletteApply@8", "@Palette_Apply@8")
extern "C" int __cdecl Dm_TextureLevel(void); KING_ALIAS("_Dm_TextureLevel", "_Mode_TextureLevel")
extern "C" void __cdecl Dm_Set71e7d4(int v); KING_ALIAS("_Dm_Set71e7d4", "_Set71e7d4")
KING_VAR(int, g_dm721754, 721754)
KING_VAR(BYTE*, g_dm721750, 721750)
KING_VAR(double, g_dm652728, 652728)
KING_VAR(char, g_dmEnv, 6730a0)           // "ENV"
KING_VAR(char, g_dmXStretch, 68327c)      // "xstretch"
KING_VAR(char, g_dmYStretch, 683270)      // "ystretch"
KING_VAR(char, g_dmJoystick, 683260)      // "JOYSTICK"
KING_VAR(char, g_dmJ68326c, 68326c)
KING_VAR(char, g_dmJ68325c, 68325c)
KING_VAR(char, g_dmJ683258, 683258)
KING_VAR(char, g_dmJ683254, 683254)
KING_VAR(char, g_dmJ683250, 683250)
KING_VAR(char, g_dmJ68324c, 68324c)
KING_VAR(char, g_dmJ683248, 683248)
KING_VAR(char, g_dmJ683244, 683244)
KING_VAR(char, g_dmJ683240, 683240)
KING_VAR(char, g_dmJ68323c, 68323c)
KING_VAR(char, g_dmJ683238, 683238)
KING_VAR(char, g_dmJ683234, 683234)
KING_VAR(char, g_dmRender, 682e68)        // "RENDER"
KING_VAR(char, g_dmScaleTime, 683228)     // "scale_time"

extern "C" {

// [FUN_5e3bc0..$L_5e3c22] app s: its mode (+256 width, +260 height, +264 bits, +268 flags) tried, else the default
// mode with the flags it reports (0x100 kept only if reported, 0x200..0x2000 copied); the texture format, the
// video mode with the ini's stretch factors and the devices (once, unless 721754); the joystick ranges read; the
// mouse centred, the sound opened, the main window (+240..+252) sized to the screen less its frame, the ini's time
// scale (window vtable +20 less 100) into +368, the root palette and the texture level
void __fastcall App_SetupVideo(BYTE* s, void*)
{
    int m[4];
    Dm_NewApp();
    if (!CallC<int>($L_5d7c20, F(int, s, 256), F(int, s, 260), F(int, s, 264),
                    (F(DWORD, s, 268) & 0x1000) << 3 | (F(DWORD, s, 268) & 0x3F00), 0, 0, 0))
    {
        m[3] = F(DWORD, s, 268) & 0x7F00;
        m[0] = F(int, s, 256);
        m[1] = F(int, s, 260);
        m[2] = F(int, s, 264);
        Dm_DefaultMode(&m[0], &m[1], &m[2], &m[3]);
        F(int, s, 264) = m[2];
        F(int, s, 256) = m[0];
        F(int, s, 260) = m[1];
        DWORD fl = m[3];
        if (!(fl & 0x100))
            F(DWORD, s, 268) &= ~0x100u;
        static const DWORD bits[5] = { 0x400, 0x200, 0x800, 0x1000, 0x2000 };
        for (int i = 0; i < 5; i++)
        {
            if (fl & bits[i])
                F(DWORD, s, 268) |= bits[i];
            else
                F(DWORD, s, 268) &= ~bits[i];
        }
    }
    F(int, s, 284) = F(int, s, 256);
    F(int, s, 288) = F(int, s, 260);
    Dm_TexFormat(F(int, s, 264), -1);
    if (!g_dm721754)
    {
        int xs, ys;
        Dm_ReadInt(&g_dmEnv, &g_dmXStretch, 1, &xs, F(const char*, s, 280));
        if (xs < 1)
            xs = 1;
        Dm_ReadInt(&g_dmEnv, &g_dmYStretch, 1, &ys, F(const char*, s, 280));
        if (ys < 1)
            ys = 1;
        Dm_SetMode(F(int, s, 256), F(int, s, 260), F(int, s, 264), F(DWORD, s, 268) & 256, xs, ys);
        if (F(DWORD, g_dm721750, 268) & 0x200)
            Dm_CreateDevices(F(DWORD, g_dm721750, 268));
        Dm_EffectZero(0);
        Dm_EffectZero(0);
        Dm_TimerCalibrate(0);
        if (F(int, g_dm721750, 316))
        {
            double v[12];
            const char* keys[12] = { &g_dmJ68326c, &g_dmJ68325c, &g_dmJ683258, &g_dmJ683254, &g_dmJ683250,
                                     &g_dmJ68324c, &g_dmJ683248, &g_dmJ683244, &g_dmJ683240, &g_dmJ68323c,
                                     &g_dmJ683238, &g_dmJ683234 };
            for (int i = 0; i < 12; i++)
                Dm_GetDouble(&g_dmJoystick, keys[i], i < 4 ? 0.0 : i < 8 ? 0.5 : 1.0, &v[i], F(const char*, s, 280));
        }
    }
    Dm_SetMousePos(s, 0, F(int, s, 256) / 2, F(int, s, 260) / 2);
    if (F(int, s, 344) && Dm_SoundOpen())
        F(int, s, 344) = 0;
    int w = Dm_ScaleX() * F(int, g_dm721750, 256);
    int hh = F(int, g_dm721750, 260);
    int h = Dm_ScaleY() * hh;
    if (F(BYTE, g_dm721750, 292) & 2)
    {
        int a = Dm_FrameExtra(2);
        a += Dm_FrameExtra(1);
        w -= a;
        int b = Dm_FrameExtra(4);
        b += Dm_FrameExtra(3);
        h -= b;
    }
    BYTE* win = (BYTE*)operator new(56);
    if (win)
        win = Dm_WindowCtor(win, 0, w, h, F(int, s, 264), F(int, s, 268));
    F(BYTE*, s, 252) = win;
    F(BYTE*, s, 248) = win;
    F(BYTE*, s, 244) = win;
    F(BYTE*, s, 240) = win;
    int sc = CallVirt<int>(win, 20) - 100;
    Dm_GetDouble(&g_dmRender, &g_dmScaleTime, sc * g_dm652728, (double*)(s + 368), F(const char*, s, 280));
    BYTE* root = Dm_RootSurface(F(BYTE*, F(BYTE*, s, 252), 12), 0);
    if (root)
        Dm_PaletteApply(root, 0);
    Dm_Set71e7d4(Dm_TextureLevel() == 2);
}

} // extern "C"

// === the app's start (InitInstance, FUN_5e29b0 with its tail $L_5e2cb0)

extern "C" void Ia_GdiAttach(); KING_ALIAS("_Ia_GdiAttach", "@GdiObject_Attach@12")
extern "C" void Ia_DialogBkColor(); KING_ALIAS("_Ia_DialogBkColor", "@WinApp_SetDialogBkColor@16")
extern "C" void Ia_Enable3d(); KING_ALIAS("_Ia_Enable3d", "@WinApp_Enable3dControls@4")
extern "C" BYTE* __fastcall Ia_BitmapCtor(BYTE* self, void*); KING_ALIAS("@Ia_BitmapCtor@8", "@Obj654420_Construct@8")
extern "C" void __cdecl Ia_MemoryTouch(int n); KING_ALIAS("_Ia_MemoryTouch", "_Memory_Touch")
extern "C" const char* __cdecl Ia_DataPath(const char* path); KING_ALIAS("_Ia_DataPath", "_DataPath_Set")
extern "C" char* __cdecl Ia_Token(char** cur); KING_ALIAS("_Ia_Token", "_NextToken")
extern "C" void __fastcall Ia_SetupVideo(BYTE* s, void*); KING_ALIAS("@Ia_SetupVideo@8", "@App_SetupVideo@8")
extern "C" int __cdecl Ia_ScaleX(); KING_ALIAS("_Ia_ScaleX", "_Video_Get67fcf0")
extern "C" int __cdecl Ia_ScaleY(); KING_ALIAS("_Ia_ScaleY", "_Video_Get67fcf4")
extern "C" int __cdecl Ia_FrameExtra(int edge); KING_ALIAS("_Ia_FrameExtra", "_Wnd_FrameExtra")
extern "C" BYTE* __cdecl Ia_FontLoad(const char* name); KING_ALIAS("_Ia_FontLoad", "_Font_LoadFile")
KING_VAR(BYTE, g_iaSplashVt652570, 652570)
KING_VAR(BYTE, g_iaBitmapVt652564, 652564)
KING_VAR(char, g_iaTitleBmp, 68314c)       // "TITLEBMP"
KING_VAR(char, g_iaTitleBmpE, 683140)      // "TITLEBMPE"
KING_VAR(BYTE*, g_iaApp721750, 721750)
KING_VAR(char*, g_iaArgv720c04, 720c04)    // the exe path, then the command line's tokens (720c08..720c68)
KING_VAR(int, g_ia721778, 721778)
KING_VAR(char, g_iaMenuFon, 676220)

extern "C" {

// [FUN_5e29b0..$L_5e2cb0] (app vtable, InitInstance) app s: the title bitmap (TITLEBMP, or TITLEBMPE for language
// 1) centred in a splash frame, the data path and the exe name (.exe) into 720c04 and +276, the command line's
// tokens, the dialog colors and 3d controls, the video setup, the mouse captured or the cursor loaded, the main
// window (+384) centred on the screen, the command line handled (vtable +156), the splash deleted, the menu font
BOOL __fastcall App_InitInstance(BYTE* s, void*)
{
    char cmd[124];
    BYTE* splash = (BYTE*)operator new(192);
    if (splash)
    {
        CallThis(Mf_FrameCtor, splash);
        RECT rc = { 100, 100, 200, 300 };
        F(void*, splash, 0) = &g_iaSplashVt652570;
        CallThis(Mf_FrameCreate, splash, 0, 0, 0x80000000, &rc, 0, 0, 0, 0);
        BYTE* bmp = (BYTE*)operator new(8);
        if (bmp)
        {
            Ia_BitmapCtor(bmp, 0);
            F(void*, bmp, 0) = &g_iaBitmapVt652564;
        }
        F(BYTE*, splash, 188) = bmp;
        const char* res = F(int, g_iaApp721750, 320) == 1 ? &g_iaTitleBmp : &g_iaTitleBmpE;
        HBITMAP hb = LoadBitmapA(F(HINSTANCE, CallStd<BYTE*>(Mf_ModuleState), 12), res);
        CallThis(Ia_GdiAttach, bmp, hb);
        int sw = GetSystemMetrics(0);
        int sh = GetSystemMetrics(1);
        int bw = 100, bh = 200;
        BYTE* b = F(BYTE*, splash, 188);
        if (F(HGDIOBJ, b, 4))
        {
            BITMAP bm;
            GetObjectA(F(HGDIOBJ, b, 4), 24, &bm);
            bw = bm.bmWidth;
            bh = bm.bmHeight;
        }
        CallThis(Mf_SetWindowPos, splash, &g_mfWndTop, (sw - bw) / 2, (sh - bh) / 2, bw, bh, 64);
    }
    Ia_MemoryTouch(100);
    g_iaApp721750 = s;
    F(int, s, 392) = F(int, s, 104);
    const char* path = Ia_DataPath(F(const char*, s, 140));
    g_iaArgv720c04 = (char*)operator new(strlen(path) + 1);
    strcpy(g_iaArgv720c04, path);
    size_t n = strlen(path);
    g_iaArgv720c04[n - 3] = 'e';
    g_iaArgv720c04[n - 2] = 'x';
    g_iaArgv720c04[n - 1] = 'e';
    F(char*, g_iaApp721750, 276) = (char*)operator new(strlen(path) + 1);
    strcpy(F(char*, g_iaApp721750, 276), path);
    strcpy(cmd, F(const char*, s, 112));
    int argc = 1;
    char* cur = cmd;
    for (char** a = &g_iaArgv720c04 + 1; a < &g_iaArgv720c04 + 25; )
    {
        char* tok = Ia_Token(&cur);
        if (!tok)
            break;
        *a = (char*)operator new(strlen(tok) + 1);
        strcpy(*a, tok);
        a++;
        argc++;
    }
    CallThis(Ia_DialogBkColor, s, 0xC0C0C0, 0);
    CallThis(Ia_Enable3d, s);
    Ia_SetupVideo(s, 0);

    if (F(DWORD, g_iaApp721750, 268) & 0x400)
        SetCapture(F(HWND, F(BYTE*, g_iaApp721750, 384), 28));
    else
        F(HCURSOR, F(BYTE*, s, 384), 192) = LoadCursorA(F(HINSTANCE, s, 104), 0);
    BYTE* mw = F(BYTE*, s, 384);
    F(BYTE*, s, 28) = mw;
    F(HWND, s, 388) = F(HWND, mw, 28);
    int sw = GetSystemMetrics(0);
    int sh = GetSystemMetrics(1);
    int w = Ia_ScaleX() * F(int, g_iaApp721750, 256);
    int hh = F(int, g_iaApp721750, 260);
    int h = Ia_ScaleY() * hh;
    if (F(BYTE, g_iaApp721750, 292) & 2)
    {
        int a = Ia_FrameExtra(2);
        w = Ia_FrameExtra(1) + w + a;
        int b = Ia_FrameExtra(4);
        h = Ia_FrameExtra(3) + h + b;
    }
    DWORD fl = F(DWORD, g_iaApp721750, 292);
    DWORD bit = fl & 1;
    F(DWORD, g_iaApp721750, 292) = fl & ~1u;
    CallThis(Mf_SetWindowPos, F(BYTE*, s, 384), &g_mfWndTop, (sw - w) / 2, (sh - h) / 2, w, h, 64);
    F(DWORD, g_iaApp721750, 292) |= bit;
    CallVirt(s, 156, argc, &g_iaArgv720c04);
    if (splash)
        CallVirt(splash, 4, 1);
    F(int, s, 328) = 1;
    g_ia721778 = 1;
    F(BYTE*, s, 272) = Ia_FontLoad(&g_iaMenuFon);
    return 1;
}

} // extern "C"

// === a base module's image and overlays loaded (FUN_5db710 with its pieces up to $L_5db9b8)

KING_VAR(char, g_rlPath720d08, 720d08)
KING_VAR(char*, g_rl7212b4, 7212b4)

extern "C" {

// [FUN_5db710..$L_5db9b8] s +468 = a new cluster named name with only its image file (dir + name) loaded, its
// overlays (*.ovl) and the first texture created; returns it (0 when the image failed)
BYTE* __fastcall Module_LoadImage(BYTE* s, void*, const char* dir, const char* name)
{
    BYTE* r = (BYTE*)operator new(180);
    r = r ? Rl_ClusterCtor(r, 0, name) : 0;
    F(BYTE*, s, 468) = r;
    if (strlen(dir) + strlen(name) + 1 >= 128)
        Rl_Fatal(&g_rlTooLong, 0);
    if (strcmp(dir, &g_rlDot671204) == 0)
        g_rlPath720d08 = 0;
    else
        strcpy(&g_rlPath720d08, dir);
    strcat(&g_rlPath720d08, name);
    if (F(Rm_Callback, s, 536))
        F(Rm_Callback, s, 536)();
    if (F(void*, g_rl721750, 344))
        Rl_SoundStart();
    F(void*, F(BYTE*, s, 468), 56) = F(void*, s, 536);
    g_rl71e82c = F(BYTE*, s, 468);
    int img = Rl_LoadAny(F(BYTE*, s, 468), dir, &g_rlPath720d08);
    if (!img)
    {
        if (F(BYTE*, s, 468))
            CallVirt(F(BYTE*, s, 468), 0, 1);
        F(BYTE*, s, 468) = 0;
    }
    char* p = &g_rlPath720d08;
    g_rl7212b4 = p;
    if (*p)
    {
        do
        {
            p++;
            g_rl7212b4 = p;
        }
        while (*p != 0);
    }
    if (*p != '.')
    {
        while (p > &g_rlPath720d08)
        {
            p--;
            g_rl7212b4 = p;
            if (*p == '.')
                break;
        }
        if (*p != '.')
        {
            g_rl71e82c = 0;
            return F(BYTE*, s, 468);
        }
    }
    p++;
    g_rl7212b4 = p;
    *g_rl7212b4 = 'o';
    g_rl7212b4++;
    *g_rl7212b4 = 'v';
    g_rl7212b4++;
    *g_rl7212b4 = 'l';
    g_rl7212b4++;
    *g_rl7212b4 = 0;
    int keep = 1;
    if (img == 3 || (img == 2 && g_rl720904))
        keep = 0;
    BYTE* ov = (BYTE*)operator new(20);
    if (ov)
        ov = Rl_OverlaysLoad(ov, 0, &g_rlPath720d08, F(BYTE*, s, 468), keep);
    F(BYTE*, F(BYTE*, s, 468), 136) = ov;
    BYTE* m = F(BYTE*, s, 468);
    if (F(int, m, 88) && (F(DWORD, g_rl721750, 268) & 0x800))
        Rl_TexCreate(*F(BYTE**, m, 92), 0, 0);
    void* probe = operator new(50000);
    if (!probe)
        Rl_Fatal(&g_rlNoMemory, 0);
    else
        operator delete(probe);
    g_rl71e82c = 0;
    return F(BYTE*, s, 468);
}

} // extern "C"

// === the app's orbit camera ($L_5e10b0 with its pieces up to $L_5e169e)

extern "C" void __fastcall Cm_SetMousePos(BYTE* self, void*, int x, int y); KING_ALIAS("@Cm_SetMousePos@16", "@CGameApp_SetMousePos@16")
KING_VAR(BYTE*, g_cmApp721750, 721750)
KING_VAR(DWORD, g_cmPrev721780, 721780)
KING_VAR(float, g_cm720e0c, 720e0c)
KING_VAR(float, g_cm720d88, 720d88)
KING_VAR(float, g_cm721214, 721214)
KING_VAR(float, g_cm7212b8, 7212b8)
KING_VAR(float, g_cm721784, 721784)
KING_VAR(float, g_cm721788, 721788)
KING_VAR(float, g_cm682a28, 682a28)
KING_VAR(double, g_cm682a08, 682a08)
KING_VAR(double, g_cm682a10, 682a10)
KING_VAR(double, g_cm64b840, 64b840)
KING_VAR(double, g_cm64b6f0, 64b6f0)
KING_VAR(double, g_cm64b6e0, 64b6e0)
KING_VAR(double, g_cm64d338, 64d338)
KING_VAR(double, g_cm64b660, 64b660)
KING_VAR(double, g_cm64c958, 64c958)
KING_VAR(double, g_cm651388, 651388)
KING_VAR(double, g_cm64b678, 64b678)
KING_VAR(double, g_cm64b670, 64b670)
KING_VAR(double, g_cm652558, 652558)
KING_VAR(double, g_cm64d0f0, 64d0f0)
KING_VAR(double, g_cm64b7a8, 64b7a8)
KING_VAR(float, g_cm64c264, 64c264)
KING_VAR(float, g_cm64c938, 64c938)
KING_VAR(float, g_cm64c250, 64c250)
KING_VAR(float, g_cm64cab0, 64cab0)
KING_VAR(float, g_cm652560, 652560)
KING_VAR(float, g_cm64c320, 64c320)
KING_VAR(float, g_cm64c908, 64c908)
KING_VAR(float, g_cm64f3b8, 64f3b8)
KING_VAR(float, g_cm64c2f8, 64c2f8)
KING_VAR(float, g_cm64c268, 64c268)

#define SD(n) F(double, s, n)

// the zoom radius for window height H: r / ((zoom * k + H / 2) / H + c)
static double Cm_Zoom(BYTE* s)
{
    double h = F(int, g_cmApp721750, 260);
    return (SD(2880) * g_cm64b6f0 + h * g_cm64b6e0) / h + g_cm64d338;
}

static double Cm_Radius(BYTE* s)
{
    double a = SD(2904), b = SD(2912), c = SD(2920);
    return x87_sqrt((a * a + b * b) + c * c);
}

extern "C" {

// [$L_5e10b0..$L_5e169e] (app vtable) the orbit camera of app s: mode +2984 (1 the mouse orbit about the target,
// 2 the chase speed curve, 0 the free flight), state +2988 (1 dragging, 2 zooming, 4 reset, 8 reverse), angles
// +2928 / +2936, distance +2888, offset +2904..+2920 (doubles), the mouse moves +2872 / +2880 scaled by +2960
void __fastcall App_UpdateCamera(BYTE* s, void*)
{
    DWORD st = F(DWORD, s, 2988);
    if ((st & 4) || ((st & 1) && (st & 2)))
    {
        // $L_5e169e: reset
        memset(s + 2872, 0, 24);
        g_cm721214 = 0.0f;
        g_cm7212b8 = 0.0f;
        Cm_SetMousePos(s, 0, F(int, g_cmApp721750, 256) / 2, F(int, g_cmApp721750, 260) / 2);
        F(DWORD, s, 2988) &= ~4u;
        return;
    }
    DWORD drag = st & 1;
    int mode = F(int, s, 2984);
    if (mode == 1)
    {
        DWORD prev = g_cmPrev721780;
        DWORD was = prev & 1;
        if (!was && drag)
        {
            g_cm720e0c = (float)SD(2928);
            g_cm720d88 = (float)SD(2936);
            g_cm721214 = (float)(SD(2872) + SD(2872));
            g_cm7212b8 = (float)(SD(2880) * g_cm64b840);
        }
        if (!(prev & 2) && (F(BYTE, s, 2988) & 2))
            g_cm682a28 = (float)(Cm_Radius(s) / Cm_Zoom(s));
        if (was && !(F(BYTE, s, 2988) & 1))
        {
            g_cm721214 = (float)(SD(2872) + SD(2872) - g_cm721784);
            g_cm7212b8 = (float)(SD(2880) * g_cm64b840 - g_cm721788);
        }
        if (F(BYTE, s, 2988) & 1)
        {
            double x2 = SD(2872) + SD(2872);
            SD(2928) = (x2 - g_cm721214) + g_cm720e0c;
            SD(2936) = (SD(2880) * g_cm64b840 - g_cm7212b8) + g_cm720d88;
            g_cm721784 = (float)x2;
            g_cm721788 = (float)(SD(2880) * g_cm64b840);
        }
        else
        {
            SD(2928) = g_cm721214 * SD(2960) + SD(2928);
            SD(2936) = g_cm7212b8 * SD(2960) + SD(2936);
            if (!(SD(2936) <= g_cm682a08))
                SD(2936) = g_cm682a08;
            if (SD(2936) < g_cm682a10)
                SD(2936) = g_cm682a10;
        }
        double r = Cm_Radius(s);
        DWORD now = F(DWORD, s, 2988);
        if (now & 2)
            r = Cm_Zoom(s) * g_cm682a28;
        double a = SD(2928);
        SD(2904) = x87_cos(SD(2936)) * x87_sin(a) * r;
        SD(2912) = -(x87_cos(SD(2936)) * x87_cos(a) * r);
        SD(2920) = x87_sin(SD(2936)) * r;
        g_cmPrev721780 = now;
        return;
    }

    // $L_5e137e: the distance, eased while dragging or zooming
    double z = SD(2888);
    float k16 = (float)z;
    double v = z;
    if (drag)
    {
        if (!(z <= g_cm64c958))
        {
            v = x87_pow(g_cm64b660, SD(2960)) * k16;
            k16 = (float)v;
        }
        else if (!(z <= g_cm64c264))
        {
            v = 1.0;
            k16 = 1.0f;
        }
        else
        {
            v = k16 / x87_pow(g_cm64b660, SD(2960) + SD(2960));
            k16 = (float)v;
            if (!(v <= g_cm64c938))
            {
                v = g_cm64c250;
                k16 = (float)v;
            }
        }
    }
    bool clamp = true;
    if (st & 2)
    {
        if (v < g_cm651388)
            v = x87_pow(g_cm64b660, SD(2960)) * k16;
        else if (v < g_cm64c264)
        {
            v = g_cm64c938;
            clamp = false;
        }
        else
        {
            v = k16 / x87_pow(g_cm64b660, SD(2960) + SD(2960));
            if (v < g_cm64c250)
            {
                v = g_cm64c938;
                clamp = false;
            }
        }
    }
    if (clamp)
    {
        if (!(v <= g_cm64cab0))
            v = g_cm64cab0;
        else if (v < g_cm652560)
            v = g_cm652560;
    }
    SD(2888) = v;
    if (st & 8)
        v = -v;
    if (mode == 2)
    {
        double k = 1.0;
        double m = g_cm64b678;
        bool high = false;
        if (!(v < g_cm64c264) && v < g_cm64c320)
            k = v * g_cm64c908;
        else if (!(v <= g_cm64c320))
        {
            k = (g_cm64cab0 - (v - g_cm64c320)) * g_cm64f3b8;
            high = true;
        }
        if (high)
            m = v < g_cm64c2f8 ? v * g_cm64c268 : g_cm64b670;
        else if (!(v < g_cm64c2f8))
            m = g_cm64b670;
        double q = x87_sqrt(k) * SD(2872) * SD(2960) + SD(2928);
        SD(2928) = q;
        SD(2936) = SD(2880) * SD(2960) * m * g_cm64d338 + SD(2936);
        SD(2944) = m * SD(2872) * g_cm652558 * g_cm64d0f0 + SD(2944) * g_cm64b7a8;
        SD(2912) = x87_cos(q) * x87_cos(SD(2936)) * v * SD(2960) + SD(2912);
        SD(2904) = -(x87_sin(q) * x87_cos(SD(2936)) * v) * SD(2960) + SD(2904);
        SD(2920) = x87_sin(-SD(2936)) * v * SD(2960) + SD(2920);
    }
    else if (mode == 0)
    {
        double a = SD(2872) * SD(2960) + SD(2928);
        SD(2928) = a;
        double sa = x87_sin(a);
        double ca = x87_cos(a);
        SD(2912) = (v * ca + sa * SD(2896)) * SD(2960) + SD(2912);
        SD(2904) = (ca * SD(2896) - v * sa) * SD(2960) + SD(2904);
        SD(2920) = SD(2880) * SD(2960) + SD(2920);
    }
}

} // extern "C"

#undef SD
