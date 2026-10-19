// media.cpp - the unit at 0x5bfe60 of king.masm: list counters, the AVI
// sound stream (msavi), keyed object tables, the music player, record
// files, tree branches and tree shadows.
//
// Building with  nmake ASM_MEDIA=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"
#include <oleauto.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
typedef DWORD Dwords[1];
extern "C" int __cdecl rand();

extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")    // unlink from its list
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")    // link into list (thiscall node, list)
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // log (text, value)
extern "C" void FUN_5d3460(); KING_ALIAS("_FUN_5d3460", "@Owner_Clear@8")
extern "C" void FUN_5c4410(); KING_ALIAS("_FUN_5c4410", "@Media_Destroy@4")
extern "C" void FUN_5cc930(); KING_ALIAS("_FUN_5cc930", "@Trees_Construct@8")
ASM_PROC(FUN_5f4a90)
extern "C" void FUN_5ef660(); KING_ALIAS("_FUN_5ef660", "@Sce5ef660_Destroy@4")
extern "C" void FUN_4011e0(); KING_ALIAS("_FUN_4011e0", "_Array_ForEach@16")
extern "C" void FUN_4bd4d0(); KING_ALIAS("_FUN_4bd4d0", "@Vec3_ZeroThis@8")
extern "C" void FUN_5ce930(); KING_ALIAS("_FUN_5ce930", "@Med5ce930_Destroy@4")
ASM_VAR(AsmLabel, g_vtbl6518e4, $L_6518e4)
ASM_VAR(AsmLabel, g_vtbl64d81c, $L_64d81c)
ASM_VAR(AsmLabel, g_vtbl651dec, $L_651dec)
ASM_VAR(BYTE*, g_70ebe4, $L_70ebe4)
ASM_VAR(int, g_707f94, $L_707f94)
ASM_VAR(int, g_707f98, $L_707f98)
ASM_VAR(WORD, g_quadIndex, $L_707f9c)
ASM_PROC(FUN_5c2800)    // msavi: create the sound buffer
extern "C" void FUN_5c36e0(); KING_ALIAS("_FUN_5c36e0", "@Entry_Apply@16")
extern "C" void FUN_6021f0(); KING_ALIAS("_FUN_6021f0", "_Gate_Start")
extern "C" void FUN_602510(); KING_ALIAS("_FUN_602510", "_Dp_SetPlayerName")
extern "C" void FUN_6022a0(); KING_ALIAS("_FUN_6022a0", "_Dp_Shutdown")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_5ab160(); KING_ALIAS("_FUN_5ab160", "_File_WriteInt")  // write an int to a file (value, file)
extern "C" void FUN_5c3f20(); KING_ALIAS("_FUN_5c3f20", "@Media_Construct@12")
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")

// msavi
typedef void(__cdecl* WaveRead)(DWORD size, BYTE* buf, DWORD* got, int ctx);
ASM_VAR(DWORD, g_msaviBufSize, $L_702f64)
ASM_VAR(DWORD, g_msaviLastPos, $L_702f68)
ASM_VAR(DWORD, g_msaviPlayed, $L_702f6c)
ASM_VAR(DWORD, g_msaviWritePos, $L_702f70)
ASM_VAR(Dwords, g_msavi702f78, $L_702f78)
ASM_VAR(int, g_702f94, $L_702f94)
ASM_VAR(BYTE*, g_msavi702f98, $L_702f98)
ASM_VAR(int, g_msavi702f9c, $L_702f9c)
ASM_VAR(BYTE*, g_msaviBuffer, $L_702fa0)     // IDirectSoundBuffer
ASM_VAR(BYTE*, g_msavi702fa4, $L_702fa4)
ASM_VAR(BYTE*, g_msaviFormat, $L_702fa8)     // WAVEFORMATEX
ASM_VAR(int, g_msaviReadCtx, $L_702fac)
ASM_VAR(WaveRead, g_msaviRead, $L_702fb0)
ASM_VAR(AsmLabel, s_msaviNoFormat, $L_67f5c4)
ASM_VAR(AsmLabel, s_msaviNoRead, $L_67f5a0)
ASM_VAR(AsmLabel, s_msaviNoBuffer, $L_67f578)
ASM_VAR(AsmLabel, s_msaviLockFailed, $L_67f748)
ASM_VAR(AsmLabel, s_msaviPosFailed, $L_67f7bc)
ASM_VAR(AsmLabel, s_msaviStatusFailed, $L_67f7fc)
ASM_VAR(AsmLabel, s_msaviFillFailed, $L_67f7e0)

ASM_VAR(AsmLabel, g_vtbl651948, $L_651948)
ASM_VAR(AsmLabel, g_vtbl651b00, $L_651b00)
ASM_VAR(AsmLabel, g_vtbl651d14, $L_651d14)
ASM_VAR(AsmLabel, g_vtbl651d64, $L_651d64)
ASM_VAR(AsmLabel, g_vtbl651db0, $L_651db0)
ASM_VAR(BYTE*, g_707f6c, $L_707f6c)
ASM_VAR(int, g_70ebe8, $L_70ebe8)
ASM_VAR(int, g_70ebec, $L_70ebec)
ASM_VAR(int, g_70ebf0, $L_70ebf0)
ASM_VAR(int, g_70ebf4, $L_70ebf4)
ASM_VAR(int, g_70ebf8, $L_70ebf8)
ASM_VAR(Dwords, g_7025f0, $L_7025f0)
ASM_VAR(Dwords, g_67fa60, $L_67fa60)
ASM_VAR(AsmLabel, s_treeShadow, $L_67f9ac)

#define E_FAIL_ ((int)0x80004005)

static __forceinline void Log(const char* s, int v)
{
    CallC(FUN_5d2ba0, s, v);
}

static __forceinline void Release(BYTE* com)
{
    (*(void(__stdcall**)(BYTE*))(*(BYTE**)com + 8))(com);
}

template <class R = int, class... A>
static __forceinline R Com(BYTE* com, unsigned off, A... args)
{
    return (*(R(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

ASM_PROC(FUN_5cfb30)
extern "C" void FUN_5d2ac0(); KING_ALIAS("_FUN_5d2ac0", "@Pool_FreeItem@12")
extern "C" void FUN_6037e0(); KING_ALIAS("_FUN_6037e0", "_Tsc_Now")    // time in seconds, returns double
extern "C" void FUN_5d35b0(); KING_ALIAS("_FUN_5d35b0", "@Owner_Detach@12")
extern "C" void FUN_5d3790(); KING_ALIAS("_FUN_5d3790", "@Queue_RemoveKey@12")
extern "C" void FUN_5d3080(); KING_ALIAS("_FUN_5d3080", "@Node_Next@8")    // next sibling
extern "C" void FUN_5d38c0(); KING_ALIAS("_FUN_5d38c0", "@Queue_RunEvent@12")
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8")
ASM_VAR(int, g_67faa0, $L_67faa0)
ASM_VAR(int, g_71e800, $L_71e800)
ASM_VAR(int, g_71e7d4, $L_71e7d4)
ASM_VAR(int, g_71e7e4, $L_71e7e4)
ASM_VAR(int, g_71e844, $L_71e844)
ASM_VAR(AsmLabel, g_pool71e830, $L_71e830)
ASM_VAR(AsmLabel, g_pool7200c0, $L_7200c0)
typedef int Ints[1];
ASM_VAR(Ints, g_71e858, $L_71e858)
ASM_VAR(AsmLabel, g_720080, $L_720080)
ASM_VAR(AsmLabel, g_vtbl651eb4, $L_651eb4)
ASM_VAR(BYTE*, g_723d44, $L_723d44)
ASM_VAR(CRITICAL_SECTION, g_cs723b40, $L_723b40)
ASM_VAR(AsmLabel, g_683c58, $L_683c58)

extern "C" long __cdecl ftell(FILE* f);
extern "C" int __cdecl fseek(FILE* f, long off, int origin);
extern "C" size_t __cdecl fwrite(const void* p, size_t size, size_t n, FILE* f);
extern "C" void __fastcall Node_ApplyMatrix(BYTE* self, void*, const void* m, int mode);   // scene.cpp
extern "C" void __cdecl Video_SetPalette(const BYTE* rgb, DWORD first, DWORD count);    // video_mode.cpp
ASM_PROC(FUN_5be8c0)    // lays out list rows (thiscall on the drop part)
ASM_FUNC(List_RowCallback, $L_5c0700)
ASM_PROC(FUN_5c06d0)    // visible rows of a list
ASM_PROC(FUN_6303c4)    // ICDecompress
ASM_PROC(FUN_6303b8)    // ICSendMessage
extern "C" void FUN_5c29d0(); KING_ALIAS("_FUN_5c29d0", "_Media_Start")
ASM_PROC(FUN_5c1d40)    // draws the video frame
ASM_PROC(FUN_5c34a0)
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_602df0(); KING_ALIAS("_FUN_602df0", "@DpMessage_Reset@8")
ASM_PROC(FUN_5c49b0)    // next record time (forward)
extern "C" void FUN_5ab140(); KING_ALIAS("_FUN_5ab140", "_File_ReadInt")    // reads an int from a file
ASM_PROC(FUN_5c4d30)
ASM_VAR(DWORD, g_msaviBlock, $L_702f74)
ASM_VAR(AsmLabel, s_aviReadError, $L_67f418)
ASM_VAR(AsmLabel, s_msaviProgressFailed, $L_67f79c)
ASM_VAR(AsmLabel, s_msaviNotifyLockFailed, $L_67f770)
typedef void(__cdecl* EntrySetter)(BYTE*, const DWORD*, int);
extern "C" void __fastcall Node_Unlink(BYTE* self, void*);   // core.cpp
extern "C" DWORD __fastcall Style_Assign(DWORD* self, void*, const DWORD* s);   // controls.cpp

extern "C" {

// [FUN_5c0020]
int __fastcall List_DrawAt(BYTE* self, void*, int a)
{
    return CallVirt<int>(self, 28, a, F(int, self, 44), F(int, self, 48), 1);
}

// [FUN_5c08f0] position of the item at +336
int __fastcall List_IndexOf336(BYTE* self, void*)
{
    int n = 0;
    BYTE* it = F(BYTE*, self, 296);
    if (it)
        for (BYTE* want = F(BYTE*, self, 336); it != want; n++) {
            it = F(BYTE*, it, 8);
            if (!it)
                break;
        }
    return n;
}

// [FUN_5c0c00] position of the item at +300
int __fastcall List_IndexOf300(BYTE* self, void*)
{
    int n = 0;
    BYTE* it = F(BYTE*, self, 296);
    if (it)
        for (BYTE* want = F(BYTE*, self, 300); it != want; n++) {
            it = F(BYTE*, it, 8);
            if (!it)
                break;
        }
    return n;
}

// [FUN_5c0cf0]
void __fastcall List_Clear(BYTE* self, void*)
{
    BYTE* head = F(BYTE*, self, 296);
    if (!head)
        return;
    BYTE* it;
    while ((it = F(BYTE*, head, 8)) != 0) {
        CallThis(FUN_5d21b0, it);
        CallVirt(it, 0, 1);
    }
    CallVirt(head, 0, 1);
    F(int, self, 336) = 0;
    F(int, self, 300) = 0;
    F(int, self, 296) = 0;
    F(int, self, 328) = 0;
    F(WORD, F(BYTE*, self, 340), 296) = 0;
    F(WORD, self, 332) = 0;
}

// [FUN_5c0e90] position of the item with this name, -1 if none
int __fastcall List_FindName(BYTE* self, void*, const char* name)
{
    int n = 0;
    for (BYTE* it = F(BYTE*, self, 296); it; it = F(BYTE*, it, 8), n++) {
        const char* a = name;
        const char* b = F(const char*, it, 12);
        while (*a == *b && *a) {
            a++;
            b++;
        }
        if (*a == *b)
            return n;
    }
    return -1;
}

// [FUN_5c1260]
void __fastcall Avi_Init(BYTE* self, void*)
{
    F(int, self, 12) = 0;
    F(int, self, 8) = 0;
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 36) = 0;
    F(int, self, 40) = 0;
    F(int, self, 2204) = 0;
    F(int, self, 2200) = 0;
    F(int, self, 2176) = 0;
    F(int, self, 2208) = 0;
    F(BYTE*, self, 2172) = self + 1148;
}

// [FUN_5c1990]
BSTR __cdecl AnsiToBstr(const char* s, int len)
{
    int n = MultiByteToWideChar(0, 0, s, len, 0, 0) - 1;
    BSTR b = SysAllocStringLen(0, n);
    if (b)
        MultiByteToWideChar(0, 0, s, -1, b, n);
    return b;
}

// [FUN_5c2450]
int __fastcall Avi_Start(BYTE* self, void*)
{
    int st = F(int, self, 2208);
    if (st == 0)
        return 2;
    if (st != 1) {
        F(int, self, 2208) = 1;
        F(int, self, 2180) = 0;
        F(int, self, 2188) = 0;
        F(int, self, 2184) = 0;
        F(int, self, 2192) = 0;
    }
    return 0;
}

// [FUN_5c26f0]
int __cdecl Msavi_Free()
{
    if (g_msavi702fa4) {
        Release(g_msavi702fa4);
        g_msavi702fa4 = 0;
    }
    if (g_msaviBuffer) {
        Release(g_msaviBuffer);
        g_msaviBuffer = 0;
    }
    if (g_msavi702f9c && g_msavi702f98) {
        Release(g_msavi702f98);
        g_msavi702f98 = 0;
    }
    return 0;
}

// [FUN_5c2750]
int __cdecl Msavi_Open(BYTE* format, WaveRead read, int ctx)
{
    if (!format) {
        Log(s_msaviNoFormat, 0);
        return E_FAIL_;
    }
    if (!read) {
        Log(s_msaviNoRead, 0);
        return E_FAIL_;
    }
    g_msaviRead = read;
    g_msaviReadCtx = ctx;
    g_msaviFormat = format;
    if (g_msavi702fa4) {
        Release(g_msavi702fa4);
        g_msavi702fa4 = 0;
    }
    if (g_msaviBuffer) {
        Release(g_msaviBuffer);
        g_msaviBuffer = 0;
    }
    g_msavi702f78[0] = 0;
    g_msavi702f78[1] = 0;
    int hr = CallC<int>(FUN_5c2800);
    if (hr < 0) {
        Log(s_msaviNoBuffer, hr);
        return hr;
    }
    return 0;
}

// [FUN_5c2b10] reads the stream into buf, the rest is silence
int __cdecl Msavi_Read(BYTE* buf, DWORD size)
{
    DWORD got = 0;
    if (!buf || !g_msaviRead)
        return E_FAIL_;
    g_msaviRead(size, buf, &got, g_msaviReadCtx);
    if (got < size) {
        BYTE fill = F(WORD, g_msaviFormat, 14) == 8 ? 0x80 : 0;
        memset(buf + got, fill, size - got);
    }
    return 0;
}

// [FUN_5c2a50] fills the whole buffer
int __cdecl Msavi_FillBuffer()
{
    BYTE* p = 0;
    DWORD n;
    if (!g_msaviBuffer)
        return E_FAIL_;
    g_msaviWritePos = 0;
    g_msaviPlayed = 0;
    g_msaviLastPos = 0;
    Com(g_msaviBuffer, 52, 0);
    int hr = Com(g_msaviBuffer, 44, 0, g_msaviBufSize, &p, &n, 0, 0, 0);
    if (hr < 0) {
        Log(s_msaviLockFailed, hr);
        return hr;
    }
    Msavi_Read(p, n);
    Com(g_msaviBuffer, 76, p, n, 0, 0);
    g_msaviWritePos = n % g_msaviBufSize;
    return 0;
}

// [FUN_5c2c70] adds the bytes played since the last call
int __cdecl Msavi_UpdatePlayed()
{
    DWORD pos;
    if (!g_msaviBuffer)
        return E_FAIL_;
    int hr = Com(g_msaviBuffer, 16, &pos, 0);
    if (hr < 0) {
        Log(s_msaviPosFailed, hr);
        return hr;
    }
    DWORD d = pos < g_msaviLastPos ? g_msaviBufSize - g_msaviLastPos + pos : pos - g_msaviLastPos;
    g_msaviLastPos = pos;
    g_msaviPlayed += d;
    return 0;
}

// [FUN_5c2ce0] restores a lost looping buffer and refills it
int __cdecl Msavi_Restore()
{
    DWORD st;
    if (!g_msaviBuffer)
        return E_FAIL_;
    int hr = Com(g_msaviBuffer, 36, &st);
    if (hr < 0) {
        Log(s_msaviStatusFailed, hr);
        return hr;
    }
    if (st & 2) {
        do {
            if (Com(g_msaviBuffer, 80) == (int)0x88780096)
                Sleep(10);
        } while (Com(g_msaviBuffer, 80));
        hr = Msavi_FillBuffer();
        if (hr < 0) {
            Log(s_msaviFillFailed, hr);
            return hr;
        }
    }
    return 0;
}

// [FUN_5c2f00]
void __cdecl Set702f94()
{
    g_702f94 = 1;
}

// Keyed tables: 10 lists of 32 bytes (head at +8), key % 10 picks one.
// An entry: vtbl, +4/+8 links, +12 key, +16 type, +20...

// [FUN_5c2fe0]
void __fastcall Table_Clear(BYTE* self, void*)
{
    for (int k = 0; k < 10; k++) {
        BYTE* it = F(BYTE*, self, 8 + k * 32);
        while (it) {
            BYTE* next = F(BYTE*, it, 8);
            CallThis(FUN_5d21b0, it);
            if (it)
                CallVirt(it, 0, 1);
            it = next;
        }
    }
}

// [FUN_5c3130]
void __fastcall Table_Link(BYTE* self, void*, BYTE* e)
{
    CallThis(FUN_5d2190, e, self + (F(DWORD, e, 12) % 10) * 32);
}

static BYTE* NewEntry()
{
    BYTE* e = (BYTE*)operator new(32);
    if (e) {
        F(int, e, 4) = 0;
        F(int, e, 8) = 0;
        F(void*, e, 0) = g_vtbl651948;
    }
    return e;
}

// [FUN_5c3020]
BYTE* __fastcall Table_Add(BYTE* self, void*, int key, int type, int v)
{
    BYTE* e = NewEntry();
    if (!e)
        return 0;
    F(int, e, 12) = key;
    F(int, e, 16) = type;
    F(int, e, 20) = v;
    Table_Link(self, 0, e);
    return e;
}

// [FUN_5c3070]
BYTE* __fastcall Table_Add6(BYTE* self, void*, int key, int a, int b)
{
    BYTE* e = NewEntry();
    if (!e)
        return 0;
    F(int, e, 12) = key;
    F(int, e, 16) = 6;
    F(int, e, 20) = a;
    F(int, e, 24) = b;
    Table_Link(self, 0, e);
    return e;
}

// [FUN_5c30d0]
BYTE* __fastcall Table_Add5(BYTE* self, void*, int key, int a, int c, int b)
{
    BYTE* e = NewEntry();
    if (!e)
        return 0;
    F(int, e, 12) = key;
    F(int, e, 20) = a;
    F(int, e, 16) = 5;
    F(int, e, 28) = c;
    F(int, e, 24) = b;
    Table_Link(self, 0, e);
    return e;
}

static __forceinline BYTE* Table_Bucket(BYTE* self, int key)
{
    BYTE* e = F(BYTE*, self, (key % 10) * 32 + 8);
    while (e && F(int, e, 12) != key)
        e = F(BYTE*, e, 8);
    return e;
}

// [FUN_5c3160]
BYTE* __fastcall Table_Find(BYTE* self, void*, int key)
{
    return Table_Bucket(self, key);
}

// [FUN_5c31a0]
int __fastcall Table_Set(BYTE* self, void*, int key, int type, int a, int b)
{
    BYTE* e = Table_Bucket(self, key);
    if (!e)
        return 0;
    int t = F(int, e, 16);
    if (t != type) {
        if (type == 3) {
            if (t != 0 && t != 2)
                return 0;
        } else if (type != 6 || t != 5) {
            return 0;
        }
    }
    CallThis(FUN_5c36e0, e, a, b);
    return 1;
}

// [FUN_5c3220]
int __fastcall Table_Value(BYTE* self, void*, int key)
{
    BYTE* e = Table_Bucket(self, key);
    return e ? F(int, e, 20) : 0;
}

// [FUN_5c32a0]
BYTE* __fastcall List_Add(BYTE* self, void*, int key, int type, int v)
{
    BYTE* e = NewEntry();
    if (e) {
        F(int, e, 12) = key;
        F(int, e, 16) = type;
        F(int, e, 20) = v;
    }
    CallThis(FUN_5d2190, e, self);
    return e;
}

// [FUN_5c32f0]
BYTE* __fastcall List_Add6(BYTE* self, void*, int key, int a, int b)
{
    BYTE* e = NewEntry();
    if (e) {
        F(int, e, 12) = key;
        F(int, e, 16) = 6;
        F(int, e, 20) = a;
        F(int, e, 24) = b;
    }
    CallThis(FUN_5d2190, e, self);
    return e;
}

// [FUN_5c3340]
BYTE* __fastcall List_Add5(BYTE* self, void*, int key, int a, int c, int b)
{
    BYTE* e = NewEntry();
    if (e) {
        F(int, e, 12) = key;
        F(int, e, 20) = a;
        F(int, e, 16) = 5;
        F(int, e, 28) = c;
        F(int, e, 24) = b;
    }
    CallThis(FUN_5d2190, e, self);
    return e;
}

// [FUN_5c33a0]
void __fastcall List_DeleteAll(BYTE* self, void*)
{
    BYTE* it;
    while ((it = F(BYTE*, self, 8)) != 0) {
        CallThis(FUN_5d21b0, it);
        CallVirt(it, 0, 1);
    }
}

// [FUN_5c39e0] starts the music track (at most 7 characters of its name)
void __fastcall Music_Play(BYTE* self, void*, const char* name, int volume, int unused)
{
    if (F(int, self, 416))
        return;
    char buf[12];
    int n = 0;
    while (name[n] && n < 7) {
        buf[n] = name[n];
        n++;
    }
    if (n < 12)
        memset(buf + n, 0, 12 - n);
    F(int, self, 432) = 0;
    F(int, self, 436) = 0;
    F(int, self, 440) = 1;
    CallC(FUN_6021f0, buf);
    F(int, self, 416) = 1;
    if (volume)
        CallC(FUN_602510, volume, 0);
}

// [FUN_5c3a80]
void __fastcall Music_Stop(BYTE* self, void*)
{
    if (F(int, self, 416)) {
        Sleep(1000);
        CallC(FUN_6022a0);
        F(int, self, 416) = 0;
        F(int, self, 428) = 0;
        F(int, self, 420) = 0;
    }
}

// [FUN_5c40b0] keeps up to 999 objects, deletes the rest
void __fastcall Pool_Keep(BYTE* self, void*, BYTE* p)
{
    if (!p)
        return;
    int n = F(int, self, 7920);
    if (n >= 999) {
        CallThis(FUN_602e80, p);
        operator delete(p);
        return;
    }
    F(BYTE*, self, 3920 + n * 4) = p;
    F(int, self, 7920) = F(int, self, 7920) + 1;
}

// [FUN_5c4340]
void __fastcall Rec_Close(BYTE* self, void*)
{
    FILE* f = F(FILE*, self, 7928);
    if (!f)
        return;
    if (F(int, self, 7936)) {
        CallC(FUN_5ab160, 0xFFFFFF00, f);
        CallC(FUN_5ab160, 4, F(FILE*, self, 7928));
        CallC(FUN_5ab160, F(int, self, 7940), F(FILE*, self, 7928));
        CallC(FUN_5ab160, 4, F(FILE*, self, 7928));
        F(int, self, 7936) = 0;
    }
    fclose(F(FILE*, self, 7928));
    F(FILE*, self, 7928) = 0;
}

// [FUN_5c43c0]
void __fastcall Rec_Mark(BYTE* self, void*)
{
    FILE* f = F(FILE*, self, 7928);
    if (f && F(int, self, 7936)) {
        CallC(FUN_5ab160, 0xFFFFF000, f);
        CallC(FUN_5ab160, 0, F(FILE*, self, 7928));
        CallC(FUN_5ab160, 0, F(FILE*, self, 7928));
    }
}

// [FUN_5c5310]
BYTE* __fastcall Timeline_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_5c3f20, self, a);
    F(void*, self, 0) = g_vtbl651b00;
    F(int, self, 8368) = F(int, g_707f6c, 432) * 10000 + 1000;
    return self;
}

// [FUN_5c53b0]
void __fastcall Timeline_Reset(BYTE* self, void*)
{
    F(int, self, 8368) = F(int, g_707f6c, 432) * 10000 + 1000;
}

// [FUN_5c5c00]
void __fastcall Timeline_Clear(BYTE* self, void*, int t)
{
    if (t <= 1000000)
        F(BYTE, self, 8372 + (t - F(int, self, 8368)) / 10) = 0;
}

// [FUN_5c82d0]
int __cdecl Ratio_Over15000(float a, float b)
{
    return (double)a / b > 15000.0;
}

// [FUN_5cb990]
void __cdecl Set70ebf4(int v)
{
    g_70ebf4 = v;
}

// [FUN_5cb9a0]
void __cdecl Set70ebe8(int a, int b)
{
    g_70ebe8 = a;
    g_70ebec = b;
}

// [FUN_5cb9c0]
void __cdecl Set70ebf0(int v)
{
    g_70ebf0 = v;
}

// [FUN_5cb9d0]
DWORD* __fastcall Rec24_Set(DWORD* self, void*, const DWORD* v, DWORD a, DWORD b, DWORD c)
{
    self[0] = v[0];
    self[1] = v[1];
    self[2] = v[2];
    self[3] = a;
    self[4] = b;
    self[5] = c;
    return self;
}

// [FUN_5cba00] a branch of a tree, random length and angle
BYTE* __fastcall Branch_Construct(BYTE* self, void*, BYTE* parent, int v)
{
    F(void*, self, 0) = g_vtbl651d14;
    F(BYTE*, self, 16) = parent;
    F(int, self, 8) = 0;
    F(int, self, 4) = 0;
    F(int, self, 12) = 0;
    F(float, self, 20) = 0.75f;
    int r = rand();
    F(float, self, 24) = 1.0f;
    F(float, self, 28) = (float)((double)r * 3.0518509447574615e-05f);
    r = rand();
    F(float, self, 36) = (float)((double)r * 3.051851081181667e-06f);
    if (!F(BYTE*, self, 16)) {
        r = rand();
        F(int, self, 32) = v;
        F(float, self, 24) = (float)((double)r * 1.5259254723787308e-05f + 1.0f);
        return self;
    }
    r = rand();
    F(int, self, 32) = v;
    F(float, self, 24) =
        (float)(F(float, F(BYTE*, self, 16), 24) - (double)r * 3.0518509447574615e-05f * F(float, self, 20));
    return self;
}

// [FUN_5cbac0]
void __fastcall Branch_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl651d14;
    static const int offs[3] = {8, 4, 12};
    for (int k = 0; k < 3; k++) {
        BYTE* c = F(BYTE*, self, offs[k]);
        if (c) {
            Branch_Destroy(c, 0);
            operator delete(c);
            F(BYTE*, self, offs[k]) = 0;
        }
    }
    F(int, self, 16) = 0;
}

// [FUN_5cbc30]
double __fastcall Branch_Sway(BYTE* self, void*, float a)
{
    return x87_cos(a) * 0.1f + F(float, self, 28);
}

// [FUN_5cbc50]
float __fastcall Branch_Value(BYTE* self, void*)
{
    return F(float, self, 32);
}

// [FUN_5cbc60]
DWORD* __fastcall Leaf_Init(DWORD* self, void*, const DWORD* m, DWORD a, DWORD b)
{
    for (int k = 0; k < 12; k++)
        self[k] = 0;
    for (int k = 0; k < 12; k++)
        self[k] = g_7025f0[k];
    for (int k = 0; k < 12; k++)
        self[k] = m[k];
    self[12] = a;
    self[13] = b;
    return self;
}

// [FUN_5cc620] appends a leaf (at most 100)
void __fastcall Leaves_Add(BYTE* self, void*, const DWORD* m, DWORD b)
{
    if (F(int, self, 4) == 100 || !F(BYTE*, self, 0))
        return;
    DWORD leaf[14];
    Leaf_Init(leaf, 0, m, F(DWORD, self, 12), b);
    DWORD* d = (DWORD*)(F(BYTE*, self, 0) + F(int, self, 4) * 56);
    for (int k = 0; k < 14; k++)
        d[k] = leaf[k];
    F(int, self, 4) = F(int, self, 4) + 1;
}

// [FUN_5cca30]
void __fastcall Tree_Free(BYTE* self, void*)
{
    BYTE* b = F(BYTE*, self, 36);
    if (b) {
        Branch_Destroy(b, 0);
        operator delete(b);
    }
    if (F(void*, self, 0))
        operator delete(F(void*, self, 0));
}

// [FUN_5cda40]
DWORD* __fastcall Vec4_Zero(DWORD* self, void*)
{
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    self[3] = 0;
    return self;
}

// [FUN_5cda50]
DWORD __fastcall Shade_Assign(DWORD* self, void*, const DWORD* s)
{
    self[0] = s[0];
    self[1] = s[1];
    self[2] = s[2];
    self[3] = s[3];
    return s[3];
}

// [FUN_5cda80] shadow of a circle (x, y, -, r) at point p: 0 outside,
// growing to 1 towards the centre
double __fastcall Shadow_Circle(const float* self, void*, const float* p)
{
    double r2 = (double)self[3] * self[3];
    double dy = (double)self[1] - p[1];
    double dx = (double)self[0] - p[0];
    double d2 = dx * dx + dy * dy;
    if (!(r2 >= d2))
        return 0.0f;
    double v = (r2 - d2) / (0.25f * r2);
    if (v > 1.0f)
        return 1.0f;
    return v;
}

// [FUN_5cdaf0]
BYTE* __fastcall Shadows_Construct(BYTE* self, void*)
{
    for (int k = 0; k < 10; k++)
        Vec4_Zero((DWORD*)(self + k * 16), 0);
    F(int, self, 160) = 0;
    return self;
}

// [FUN_5cdb20] the shade at p of up to 10 circles
float __fastcall Shadows_At(BYTE* self, void*, const float* p)
{
    float acc = 0;
    for (unsigned i = 0; i < F(unsigned, self, 160); i++)
        acc = (float)(0.3f * Shadow_Circle((float*)(self + i * 16), 0, p) + acc);
    if (acc > 1.0f)
        return 1.0f;
    return acc;
}

// [FUN_5cdba0]
void __fastcall Shadows_Add(BYTE* self, void*, const DWORD* c)
{
    int n = F(int, self, 160);
    if (n != 10) {
        Shade_Assign((DWORD*)(self + n * 16), 0, c);
        F(int, self, 160) = F(int, self, 160) + 1;
    }
}

// [FUN_5cdbd0]
void __fastcall Shadows_SetOwner(BYTE* self, void*, int owner)
{
    if (owner != F(int, self, 164)) {
        F(int, self, 160) = 0;
        F(int, self, 164) = owner;
    }
}

// [FUN_5cdc00]
int __cdecl Get70ebf8()
{
    return g_70ebf8;
}

// [FUN_5cdc10]
BYTE* __fastcall TreeShadow_Construct(BYTE* self, void*)
{
    CallThis(FUN_5f19b0, self, (const char*)s_treeShadow, 5, 1);
    static const int offs[] = {56, 60, 172, 176, 180, 184, 188, 192, 196, 200, 204, 212, 216, 220};
    for (int k = 0; k < 14; k++)
        F(int, self, offs[k]) = 0;
    F(void*, self, 0) = g_vtbl651d64;
    F(BYTE, self, 40) |= 1;
    return self;
}

// [FUN_5ce540]
void __cdecl Set67fa60(DWORD lo, DWORD hi)
{
    g_67fa60[0] = lo;
    g_67fa60[1] = hi;
}

// [FUN_5ce560]
BYTE* __fastcall Light_Construct(BYTE* self, void*, int a, int b)
{
    F(int, self, 16) = a;
    F(int, self, 20) = b;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl651db0;
    F(int, self, 24) = 0;
    return self;
}

}

extern "C" {

// [FUN_5c07c0] lays out the list's drop part under a header of height h
void __fastcall List_Layout(BYTE* self, void*, int h)
{
    BYTE* c = F(BYTE*, self, 340);
    Style_Assign((DWORD*)(c + 68), 0, (DWORD*)(self + 68));
    Style_Assign((DWORD*)(c + 104), 0, (DWORD*)(self + 104));
    Style_Assign((DWORD*)(c + 140), 0, (DWORD*)(self + 140));
    Style_Assign((DWORD*)(c + 176), 0, (DWORD*)(self + 176));
    CallThis(FUN_5be8c0, F(BYTE*, self, 340), 0, self + 332, (void*)List_RowCallback, self,
            F(int, self, 344) + F(int, self, 308) + h * 2, h, 0, 0);
    int d = F(int, self, 52) - F(int, self, 44) - h;
    int dy = 2;
    BYTE* font = F(BYTE*, self, 16);
    if (font && F(char*, self, 12) && *F(char*, self, 12))
        dy = F(int, font, 20) + 2;
    int* r = (int*)(F(BYTE*, self, 340) + 44);
    r[0] += d;
    r[2] += d;
    r[1] += dy;
    r[3] += dy;
    int s = F(int, self, 348);
    F(int, self, 312) += s;
    F(int, self, 320) += s - h;
}

// [FUN_5c0c20] selects item i and scrolls it into view
void __fastcall List_Select(BYTE* self, void*, int i)
{
    if (i < 0 || i >= F(int, self, 328))
        return;
    BYTE* head = F(BYTE*, self, 296);
    BYTE* c = head;
    for (int k = i; c && F(BYTE*, c, 8) && k > 0; k--)
        c = F(BYTE*, c, 8);
    F(BYTE*, self, 300) = c;
    int top = 0;
    for (BYTE* t = head; t && F(BYTE*, t, 8) && t != F(BYTE*, self, 336); t = F(BYTE*, t, 8))
        top++;
    if (i < top) {
        top = i;
        F(BYTE*, self, 336) = F(BYTE*, self, 300);
    }
    while (i >= CallThis<int>(FUN_5c06d0, self) + top) {
        top++;
        F(BYTE*, self, 336) = F(BYTE*, F(BYTE*, self, 336), 8);
    }
    if (F(int, self, 328) - CallThis<int>(FUN_5c06d0, self) <= 0)
        F(WORD, self, 332) = 0;
    else
        F(WORD, self, 332) = (WORD)top;
}

// [FUN_5c0d60] removes the item of this name
void __fastcall List_RemoveName(BYTE* self, void*, const char* name)
{
    BYTE* n = F(BYTE*, self, 296);
    for (; n; n = F(BYTE*, n, 8))
        if (!strcmp(F(const char*, n, 12), name))
            break;
    if (n) {
        if (n == F(BYTE*, self, 300))
            F(BYTE*, self, 300) = F(BYTE*, n, 8) ? F(BYTE*, n, 8) : F(BYTE*, n, 4);
        if (n == F(BYTE*, self, 336))
            F(BYTE*, self, 336) = F(BYTE*, n, 8) ? F(BYTE*, n, 8) : F(BYTE*, n, 4);
        if (n == F(BYTE*, self, 296)) {
            BYTE* next = F(BYTE*, n, 8);
            F(BYTE*, self, 296) = next;
            if (next)
                F(BYTE*, next, 4) = 0;
        } else {
            Node_Unlink(n, 0);
        }
        CallVirt(n, 0, 1);
        F(int, self, 328) = F(int, self, 328) - 1;
        BYTE* c = F(BYTE*, self, 340);
        F(short, c, 296) = F(short, c, 296) - 1;
        if (F(short, F(BYTE*, self, 340), 296) < 0)
            F(short, F(BYTE*, self, 340), 296) = 0;
    }
    short v = F(short, F(BYTE*, self, 340), 296);
    if (v < F(short, self, 332))
        F(short, self, 332) = v;
}

// [FUN_5c19f0] reads and decompresses video frame i
int __fastcall Avi_ReadFrame(BYTE* self, void*, int i)
{
    DWORD* e = (DWORD*)(F(BYTE*, self, 0) + i * 16);
    DWORD off = e[0], size = e[1];
    LONG hi = 0;
    DWORD got;
    if (SetFilePointer(F(HANDLE, self, 36), off, &hi, 0) != off)
        return 1;
    if (!ReadFile(F(HANDLE, self, 36), F(void*, self, 2200), size, &got, 0) || got != size)
        return 1;
    F(DWORD, self, 64) = size;
    F(int, self, 1128) = F(int, self, 1116) * F(int, self, 1112) * 3;
    if (CallC<int>(FUN_6303c4, F(int, self, 2176), 0, self + 44, F(void*, self, 2200), self + 1108,
                   F(void*, self, 2204)))
        return 1;
    if (F(WORD, self, 1122) == 8) {
        if (CallStd<int>(FUN_6303b8, F(int, self, 2176), 16414, self + 44, self + 1108))
            return 1;
        if (F(WORD, self, 1122) == 8 && F(DWORD, self, 1140)) {
            CallStd<int>(FUN_6303b8, F(int, self, 2176), 16413, self + 1108, 0);
            Video_SetPalette(F(BYTE*, self, 2172), 0, F(DWORD, self, 1140));
        }
    }
    return 0;
}

// [FUN_5c1b20] reads *len bytes of the audio stream into buf
int __fastcall Avi_ReadAudio(BYTE* self, void*, BYTE* buf, DWORD* len)
{
    int st = F(int, self, 2208);
    if (st == 0 || st == 1) {
        *len = 0;
        return E_FAIL_;
    }
    if (F(int, self, 2184) >= F(int, self, 12)) {
        *len = 0;
        return 0;
    }
    DWORD total = 0;
    LONG hi = 0;
    DWORD got = 0;
    while (*len) {
        DWORD* e = (DWORD*)(F(BYTE*, self, 4) + F(int, self, 2184) * 16);
        DWORD pos = F(DWORD, self, 2188);
        DWORD left = e[1] - pos;
        DWORD start = e[0];
        if (left > *len) {
            if (SetFilePointer(F(HANDLE, self, 40), start + pos, &hi, 0) != F(DWORD, self, 2188) + start ||
                !ReadFile(F(HANDLE, self, 40), buf, *len, &got, 0) || got != *len) {
                *len = got + total;
                Log(s_aviReadError, 0);
                return E_FAIL_;
            }
            total += got;
            F(DWORD, self, 2188) += got;
            *len = 0;
        } else {
            if (SetFilePointer(F(HANDLE, self, 40), start + pos, &hi, 0) != F(DWORD, self, 2188) + start ||
                !ReadFile(F(HANDLE, self, 40), buf, left, &got, 0) || got != left) {
                *len = got + total;
                Log(s_aviReadError, 1);
                return E_FAIL_;
            }
            *len -= got;
            total += got;
            buf += got;
            int n = F(int, self, 2184) + 1;
            F(DWORD, self, 2188) = 0;
            F(int, self, 2184) = n;
            if (n >= F(int, self, 12)) {
                if (F(BYTE, self, 2240) & 2)
                    F(int, self, 2184) = 0;
                *len = total;
                return 0;
            }
        }
    }
    *len = total;
    return 0;
}

// [FUN_5c2340] advances the video to the current time, then draws
int __fastcall Avi_Update(BYTE* self, void*, int a, int b, int c, int d, int e)
{
    int st = F(int, self, 2208);
    if (st == 0)
        return 2;
    if (st == 1) {
        F(int, self, 2208) = 2;
        CallC(FUN_5c29d0);
        F(double, self, 2216) = CallC<double>(FUN_6037e0);
    }
    int t = (int)((CallC<double>(FUN_6037e0) - F(double, self, 2216)) * F(double, self, 2224) * 0.001);
    int n = F(int, self, 2180);
    if (t > n + 10) {
        F(int, self, 2180) = n + 10;
        if (n + 10 < F(int, self, 8)) {
            F(int, self, 2180) = n + 11;
            Avi_ReadFrame(self, 0, n + 10);
        }
    } else if (n < t) {
        do {
            F(int, self, 2180) = n + 1;
            Avi_ReadFrame(self, 0, n);
            n = F(int, self, 2180);
        } while (n < F(int, self, 8) && n < t);
    }
    if (F(int, self, 2180) >= F(int, self, 8)) {
        F(int, self, 2208) = 1;
        F(int, self, 2180) = 0;
        F(int, self, 2188) = 0;
        F(int, self, 2184) = 0;
        F(int, self, 2192) = 0;
        return 1;
    }
    CallThis(FUN_5c1d40, self, a, b, c, d, e);
    return 0;
}

// [FUN_5c2ba0] refills the played part of the stream buffer
int __cdecl Msavi_HandleNotification()
{
    BYTE* p = 0;
    DWORD n;
    int hr = Msavi_UpdatePlayed();
    if (hr < 0) {
        Log(s_msaviProgressFailed, hr);
        return hr;
    }
    if (!g_msaviBuffer)
        return E_FAIL_;
    hr = Com(g_msaviBuffer, 44, g_msaviWritePos, g_msaviBlock, &p, &n, 0, 0, 0);
    if (hr < 0) {
        Log(s_msaviNotifyLockFailed, hr);
        return hr;
    }
    Msavi_Read(p, n);
    Com(g_msaviBuffer, 76, p, n, 0, 0);
    g_msaviWritePos = (g_msaviWritePos + n) % g_msaviBufSize;
    return 0;
}

// [FUN_5c33d0] sends and records the frame's network messages
void __fastcall Net_FlushFrame(BYTE* self, void*)
{
    BYTE* rec = F(BYTE*, self, 32);
    if (!F(int, rec, 7948))
        return;
    BYTE* buf = rec + 8336;
    for (BYTE* n = F(BYTE*, self, 8); n; n = F(BYTE*, n, 8))
        CallThis(FUN_5c34a0, n, buf);
    if (F(int, g_707f6c, 416))
        CallC(FUN_6021b0, F(BYTE*, buf, 4), F(BYTE*, buf, 8) - F(BYTE*, buf, 4));
    rec = F(BYTE*, self, 32);
    if (F(int, rec, 7936) && F(FILE*, rec, 7928)) {
        CallC(FUN_5ab160, -1, F(FILE*, rec, 7928));
        CallC(FUN_5ab160, F(BYTE*, buf, 8) - F(BYTE*, buf, 4), F(FILE*, F(BYTE*, self, 32), 7928));
        fwrite(F(BYTE*, buf, 4), F(BYTE*, buf, 8) - F(BYTE*, buf, 4), 1, F(FILE*, F(BYTE*, self, 32), 7928));
        CallC(FUN_5ab160, F(BYTE*, buf, 8) - F(BYTE*, buf, 4), F(FILE*, F(BYTE*, self, 32), 7928));
    }
    CallThis(FUN_602df0, buf);
}

// [FUN_5c36e0] stores a value into the entry's target by its type
int __fastcall Entry_Apply(BYTE* self, void*, const DWORD* src, DWORD size)
{
    BYTE* t = F(BYTE*, self, 20);
    if (!t || !src)
        return 0;
    switch (F(int, self, 16)) {
    case 1:
        Node_ApplyMatrix(t, 0, src, 0);
        return 1;
    case 0: {
        int v = (int)src[0];
        if (v < CallVirt<int>(t, 60) && v >= 0)
            F(int, t, 56) = v;
        return 1;
    }
    case 3:
        *(DWORD*)t = src[0];
        return 1;
    case 4:
        *(double*)t = *(const double*)src;
        return 1;
    case 2:
        CallVirt(t, 28, 0);
        F(int, self, 20) = 0;
        return 1;
    case 5:
        F(EntrySetter, self, 28)(t, src, F(int, self, 24));
        return 1;
    case 6: {
        DWORD n = F(DWORD, self, 24);
        if (n > size)
            n = size;
        memcpy(t, src, n);
        return 1;
    }
    }
    return 1;
}

// [FUN_5c4c50] the record position nearest before time t; the time after
// it in *next
int __fastcall Rec_SeekTime(BYTE* self, void*, int t, int* next)
{
    FILE* f = F(FILE*, self, 7928);
    long pos = ftell(f);
    int r = CallThis<int>(FUN_5c49b0, self, 1);
    if (r == -1)
        r = CallThis<int>(FUN_5c49b0, self, 0);
    int dir = r >= t ? -1 : 1;
    int cur = r;
    for (;;) {
        long p = ftell(f);
        if (dir == 1) {
            int r2 = CallThis<int>(FUN_5c49b0, self, 1);
            if (r2 == -1) {
                *next = cur;
                return cur;
            }
            if (r2 > t) {
                *next = r2;
                fseek(f, pos, 0);
                return cur;
            }
            pos = p;
            cur = r2;
        } else {
            int r2 = CallThis<int>(FUN_5c49b0, self, 0);
            if (r2 == -1) {
                *next = cur;
                return cur;
            }
            if (r2 < t) {
                *next = cur;
                return r2;
            }
            pos = p;
            cur = r2;
        }
    }
}

// [FUN_5c4e20] reads the record header (skipping other blocks)
void __fastcall Rec_ReadHeader(BYTE* self, void*, int unused, int b)
{
    FILE* f = F(FILE*, self, 7928);
    long start = ftell(f);
    int v = CallC<int>(FUN_5ab140, f);
    while (v != -16) {
        if (v == -256) {
            fseek(f, start, 0);
            return;
        }
        if (v == -1) {
            CallThis(FUN_5c4d30, self, start, b);
        } else {
            int n = CallC<int>(FUN_5ab140, f) + 4;
            fseek(f, ftell(f) + n, 0);
        }
        v = CallC<int>(FUN_5ab140, f);
    }
    if (CallC<int>(FUN_5ab140, f) == 4) {
        F(int, self, 7940) = CallC<int>(FUN_5ab140, f) != 0;
        CallC<int>(FUN_5ab140, f);
        return;
    }
    F(int, self, 7940) = CallC<int>(FUN_5ab140, f);
    CallC<int>(FUN_5ab140, f);
    CallC<int>(FUN_5ab140, f);
}

}

// --- constructors/destructors with exception frames (the frames only rethrow)

extern "C" {

// [FUN_5c10f0] a named list item's destructor
void __fastcall NamedItem_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl6518e4;
    operator delete(F(void*, self, 12));
    F(void*, self, 0) = g_vtbl64d81c;
}

// [FUN_5c1140] appends an item named name to the list and selects nothing new
void __fastcall List_AddNamed(BYTE* self, void*, const char* name)
{
    BYTE* it = (BYTE*)operator new(16);
    if (it) {
        F(int, it, 4) = 0;
        F(int, it, 8) = 0;
        F(void*, it, 0) = g_vtbl6518e4;
        char* p = (char*)operator new(strlen(name) + 1);
        F(char*, it, 12) = p;
        strcpy(p, name);
    }
    BYTE* c = F(BYTE*, self, 296);
    if (!c) {
        F(BYTE*, self, 336) = it;
        F(BYTE*, self, 300) = it;
        F(BYTE*, self, 296) = it;
    } else {
        while (F(BYTE*, c, 8))
            c = F(BYTE*, c, 8);
        CallThis(FUN_5d2190, it, c);
        F(BYTE*, self, 300) = it;
    }
    F(int, self, 328)++;
    if (F(int, self, 328) > CallThis<int>(FUN_5c06d0, self))
        F(short, F(BYTE*, self, 340), 296)++;
    List_Select(self, 0, 0);
}

// [FUN_5c53e0]
void __fastcall Media5c53e0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl651b00;
    CallThis(FUN_5d3460, self + 2992);
    CallThis(FUN_5c4410, self);
}

// [FUN_5cc930] the trees: parameters, a root branch and 100 56-byte leaves
BYTE* __fastcall Trees_Construct(BYTE* self, void*)
{
    F(float, self, 16) = 10.0f;
    F(float, self, 20) = 45.0f;
    F(float, self, 12) = 4.0f;
    F(float, self, 24) = 0.5f;
    F(DWORD, self, 28) = 0x3f733333;
    F(int, self, 32) = 4;
    F(int, self, 4) = 0;
    F(int, self, 40) = 0;
    BYTE* o = (BYTE*)operator new(40);
    F(BYTE*, self, 36) = o ? Branch_Construct(o, 0, 0, 0x3f800000) : 0;
    BYTE* a = (BYTE*)operator new(5600);
    if (a) {
        BYTE* p = a;
        for (int i = 0; i < 100; i++, p += 56) {
            CallStd(FUN_4011e0, p, 12, 4, FUN_4bd4d0);
            memcpy(p, g_7025f0, 48);
        }
    }
    F(BYTE*, self, 0) = a;
    return self;
}

// [FUN_5c6400] creates the trees and shadows; fills the quad index table
int __cdecl Effects_InitTrees(int a, int b)
{
    BYTE* o = (BYTE*)operator new(44);
    g_70ebe4 = o ? Trees_Construct(o, 0) : 0;
    g_707f94 = a;
    g_707f98 = b;
    WORD* q = &g_quadIndex;
    for (int i = 0; i < 100; i++, q += 6) {
        WORD v = (WORD)(i * 4);
        q[0] = v;
        q[1] = v + 1;
        q[2] = v + 2;
        q[3] = v;
        q[4] = v + 2;
        q[5] = v + 3;
    }
    if (!g_70ebf8) {
        BYTE* s = (BYTE*)operator new(168);
        g_70ebf8 = s ? (int)Shadows_Construct(s, 0) : 0;
    }
    return g_70ebf8;
}

// [FUN_5c64f0]
void __cdecl Effects_FreeTrees()
{
    BYTE* t = g_70ebe4;
    if (t) {
        Tree_Free(t, 0);
        operator delete(t);
        g_70ebe4 = 0;
    }
    if (g_70ebf8) {
        BYTE* s = (BYTE*)operator new(168);
        if (s)
            Shadows_Construct(s, 0);
        g_70ebf8 = 0;
    }
    g_707f94 = 0;
    g_707f98 = 0;
}

// [FUN_5c8780]
void __fastcall Media5c8780_Destroy(BYTE* self, void*)
{
    CallThis(FUN_5f4a90, self + 64);
    if (F(void*, self, 60))
        operator delete(F(void*, self, 60));
    CallThis(FUN_5ef660, self);
}

// [FUN_5ceae0]
void __fastcall Media5ceae0_Destroy(BYTE* self, void*)
{
    F(void*, self, 0) = g_vtbl651dec;
    operator delete(F(void*, self, 172));
    CallThis(FUN_5ce930, self);
}

}

// ---------------------------------------------------------------------------
extern "C" void FUN_5ef540(); KING_ALIAS("_FUN_5ef540", "_Table124_Find")
ASM_VAR(AsmLabel, s_67f8e0, $L_67f8e0)
ASM_VAR(AsmLabel, s_67ef70, $L_67ef70)
ASM_VAR(AsmLabel, s_67ef78, $L_67ef78)
ASM_VAR(AsmLabel, s_67f958, $L_67f958)
ASM_VAR(AsmLabel, g_name70dd90, $L_70dd90)
ASM_VAR(AsmLabel, g_name70de78, $L_70de78)
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_5c5bc0..$L_5c5be8] takes a free 10-byte slot of the 1000 at +8368 (used flags at +8372); fatal when none is left
extern "C" BYTE* __fastcall Media_TakeSlot(BYTE* self, void*)
{
    for (int i = 0; i < 1000; i++)
        if (!F(BYTE, self, 8372 + i))
        {
            F(BYTE, self, 8372 + i) = 1;
            return F(BYTE*, self, 8368) + i * 10;
        }
    Sys_Fatal(s_67f8e0, 0);
    return (BYTE*)-1;
}

// looks up the formatted name in the table of obj+36; fatal when it isn't there
static BYTE* Media_FindName(BYTE* obj, char* name)
{
    int found;
    char text[248];
    BYTE* r = CallC<BYTE*>(FUN_5ef540, name, F(int, obj, 36), &found);
    if (!r)
    {
        sprintf(text, s_67f958, name, F(int, F(BYTE*, obj, 36), 60));
        Sys_Fatal(text, 0);
    }
    return r;
}

// [FUN_5c84a0..$L_5c8505] the entry of obj's table named by format 67ef70 with a
extern "C" BYTE* __cdecl Media_FindFmt1(BYTE* obj, int a)
{
    sprintf(g_name70dd90, s_67ef70, a);
    return Media_FindName(obj, g_name70dd90);
}

// [FUN_5c8520..$L_5c858a] the entry of obj's table named by format 67ef78 with (a, b)
extern "C" BYTE* __cdecl Media_FindFmt2(BYTE* obj, int a, int b)
{
    sprintf(g_name70de78, s_67ef78, a, b);
    return Media_FindName(obj, g_name70de78);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5c2fe0(); KING_ALIAS("_FUN_5c2fe0", "@Table_Clear@8")
extern "C" void FUN_5c33a0(); KING_ALIAS("_FUN_5c33a0", "@List_DeleteAll@8")
extern "C" void FUN_5c53b0(); KING_ALIAS("_FUN_5c53b0", "@Timeline_Reset@8")
ASM_PROC(FUN_5e02f0)
extern "C" void FUN_5c4940(); KING_ALIAS("_FUN_5c4940", "@Media_ClearList@8")
ASM_PROC(FUN_5cc6b0)

// [FUN_5cda30] +4
extern "C" int __fastcall Media_Get4(BYTE* self, void*) { return F(int, self, 4); }

// [FUN_5c4910] clears the table (+7976), the list (+8300) and then 5e02f0
extern "C" void __fastcall Media_Clear5c4910(BYTE* self, void*)
{
    CallThis(FUN_5c2fe0, self + 7976);
    CallThis(FUN_5c33a0, self + 8300);
    CallThis(FUN_5e02f0, self);
}

// [FUN_5c5380] resets the timeline, frees all 1000 slots and runs 5c4940
extern "C" void __fastcall Media_Reset5c5380(BYTE* self, void*)
{
    CallThis(FUN_5c53b0, self);
    memset(self + 8372, 0, 1000);
    CallThis(FUN_5c4940, self);
}

// [FUN_5cc680] 5cc6b0(0, b, obj) (passing this on), then obj's vtbl+8(1.0)
extern "C" void __fastcall Media_Call5cc680(void* self, void*, int a, int b, BYTE* obj)
{
    CallThis(FUN_5cc6b0, self, 0, b, obj);
    CallVirt(obj, 8, 1.0f);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
ASM_PROC(FUN_5e03c0)
ASM_VAR(AsmLabel, g_vt64d81c, $L_64d81c)

// [FUN_5c4940] deletes the 3920 list (count +7920) from the end, resets the state and runs 5e03c0
extern "C" int __fastcall Media_ClearList(BYTE* self, void*)
{
    while (F(int, self, 7920) > 0)
    {
        int n = --F(int, self, 7920);
        BYTE* p = F(BYTE*, self, 3920 + n * 4);
        if (p)
        {
            CallThis(FUN_602e80, p);
            operator delete(p);
        }
    }
    F(int, self, 7968) = 0;
    F(int, self, 7944) = 1;
    F(int, self, 7948) = 1;
    F(int, self, 7972) = 0;
    return CallThis<int>(FUN_5e03c0, self);
}

// [FUN_5c5440] returns 0 (19 arguments ignored)
extern "C" int __stdcall Media_Zero19(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { return 0; }

// [FUN_5ce630] the base destructor body: sets the vtable
extern "C" void __fastcall Light_DestructBase(BYTE* self, void*) { F(void*, self, 0) = g_vt64d81c; }

// ---------------------------------------------------------------------------
extern "C" void FUN_5c3020(); KING_ALIAS("_FUN_5c3020", "@Table_Add@20")
extern "C" void FUN_5c30d0(); KING_ALIAS("_FUN_5c30d0", "@Table_Add5@24")
ASM_PROC($L_5c6120)
extern "C" void FUN_5c2ce0(); KING_ALIAS("_FUN_5c2ce0", "_Msavi_Restore")
extern "C" void FUN_5c2a50(); KING_ALIAS("_FUN_5c2a50", "_Msavi_FillBuffer")
ASM_VAR(BYTE*, g_md702fa0, $L_702fa0)
ASM_VAR(AsmLabel, s_md67f728, $L_67f728)
ASM_VAR(AsmLabel, s_md67f70c, $L_67f70c)
ASM_VAR(AsmLabel, s_md67f6e8, $L_67f6e8)
extern "C" void __cdecl Sys_Log(const char* text, int value);

// [FUN_5c6080] registers the three controls of this item in the owner's table (+7976) from index n
extern "C" void __fastcall Item_Register(BYTE* self, void*, BYTE* owner, int n)
{
    BYTE* t = owner + 7976;
    F(int, self, 368) = n + 1;
    F(int, self, 364) = n;
    F(int, self, 360) = CallThis<int>(FUN_5c3020, t, n, 2, self);
    int k = F(int, self, 368);
    F(int, self, 368) = k + 1;
    F(int, self, 352) = CallThis<int>(FUN_5c3020, t, k, 1, F(int, self, 324));
    k = F(int, self, 368);
    F(int, self, 368) = k + 1;
    F(int, self, 356) = CallThis<int>(FUN_5c30d0, t, k, self, $L_5c6120, 64);
    F(DWORD, self, 208) |= 5;
    F(int, self, 372) = 1;
}

// [FUN_5c29d0] starts the media player (702fa0): 5c2ce0, 5c2a50, then its method +48 (0, 0, 1); a failure is logged and returned
extern "C" long __cdecl Media_Start()
{
    if (!g_md702fa0)
        return (long)0x80004005;
    long r = CallC<long>(FUN_5c2ce0);
    if (r < 0)
    {
        Sys_Log(s_md67f728, r);
        return r;
    }
    r = CallC<long>(FUN_5c2a50);
    if (r < 0)
    {
        Sys_Log(s_md67f70c, r);
        return r;
    }
    r = ((long(__stdcall*)(BYTE*, int, int, int))(*(void***)g_md702fa0)[12])(g_md702fa0, 0, 0, 1);
    if (r < 0)
    {
        Sys_Log(s_md67f6e8, r);
        return r;
    }
    return 0;
}


// ---------------------------------------------------------------------------
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_5ab160(); KING_ALIAS("_FUN_5ab160", "_File_WriteInt")    // writes an int to a file (v, file)
extern "C" void FUN_5c2450(); KING_ALIAS("_FUN_5c2450", "@Avi_Start@8")
ASM_PROC(FUN_6303b8)
ASM_PROC(FUN_6303b2)
extern "C" void FUN_5c2f00(); KING_ALIAS("_FUN_5c2f00", "_Set702f94")
ASM_VAR(BYTE*, g_md707f6c, $L_707f6c)
ASM_VAR(int, g_md7208b8, $L_7208b8)
extern "C" size_t __cdecl fwrite(const void* buf, size_t size, size_t n, FILE* f);

// [FUN_5c3630] records a block (rec +4 .. +8): to 6021b0 when the player is set, and to the record file (+7928, when +7936) as -1, length, bytes, length
extern "C" void __fastcall Media_RecordBlock(BYTE* self, void*, BYTE* rec)
{
    if (!F(int, self, 7948))
        return;
    BYTE* p = F(BYTE*, rec, 4);
    int n = F(BYTE*, rec, 8) - p;
    if (F(int, g_md707f6c, 416))
        CallC(FUN_6021b0, p, n);
    FILE* f = F(FILE*, self, 7928);
    if (!f || !F(int, self, 7936))
        return;
    CallC(FUN_5ab160, -1, f);
    CallC(FUN_5ab160, F(BYTE*, rec, 8) - F(BYTE*, rec, 4), F(FILE*, self, 7928));
    fwrite(F(BYTE*, rec, 4), F(BYTE*, rec, 8) - F(BYTE*, rec, 4), 1, F(FILE*, self, 7928));
    CallC(FUN_5ab160, F(BYTE*, rec, 8) - F(BYTE*, rec, 4), F(FILE*, self, 7928));
}

// [FUN_5c12a0] closes the media player: frees its buffers, events and the device (+2176)
extern "C" void __fastcall Media_Close(BYTE* self, void*)
{
    CallThis(FUN_5c2450, self);
    if (F(void*, self, 4)) { operator delete(F(void*, self, 4)); F(void*, self, 4) = 0; }
    if (F(void*, self, 0)) { operator delete(F(void*, self, 0)); F(void*, self, 0) = 0; }
    if (F(void*, self, 2200)) { operator delete(F(void*, self, 2200)); F(void*, self, 2200) = 0; }
    if (F(void*, self, 2204)) { operator delete(F(void*, self, 2204)); F(void*, self, 2204) = 0; }
    if (F(HANDLE, self, 36)) { CloseHandle(F(HANDLE, self, 36)); F(HANDLE, self, 36) = 0; }
    if (F(HANDLE, self, 40)) { CloseHandle(F(HANDLE, self, 40)); F(HANDLE, self, 40) = 0; }
    if (F(int, self, 2176))
    {
        CallStd(FUN_6303b8, F(int, self, 2176), 16398, 0, 0);
        CallStd(FUN_6303b2, F(int, self, 2176));
        F(int, self, 2176) = 0;
    }
    F(int, self, 2208) = 0;
    g_md7208b8 = 0;
    CallC(FUN_5c2f00);
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_5c10f0(); KING_ALIAS("_FUN_5c10f0", "@NamedItem_Destroy@8")
extern "C" void FUN_5ce630(); KING_ALIAS("_FUN_5ce630", "@Light_DestructBase@8")
extern "C" void FUN_5c3990(); KING_ALIAS("_FUN_5c3990", "@Med5c3990_Destroy@4")
extern "C" void FUN_5c4410(); KING_ALIAS("_FUN_5c4410", "@Media_Destroy@4")
extern "C" void FUN_5c53e0(); KING_ALIAS("_FUN_5c53e0", "@Media5c53e0_Destroy@8")
extern "C" void FUN_5c8780(); KING_ALIAS("_FUN_5c8780", "@Media5c8780_Destroy@8")
ASM_PROC(FUN_5cdcb0)
extern "C" void FUN_5ce5b0(); KING_ALIAS("_FUN_5ce5b0", "@Med5ce5b0_Destroy@4")
extern "C" void FUN_5ce930(); KING_ALIAS("_FUN_5ce930", "@Med5ce930_Destroy@4")
extern "C" void FUN_5ceae0(); KING_ALIAS("_FUN_5ceae0", "@Media5ceae0_Destroy@8")
ASM_PROC(FUN_5cf170)
extern "C" void FUN_5da8a0(); KING_ALIAS("_FUN_5da8a0", "@Fw_UnloadRoom@8")
extern "C" void FUN_5c2fe0(); KING_ALIAS("_FUN_5c2fe0", "@Table_Clear@8")
extern "C" void FUN_5c33a0(); KING_ALIAS("_FUN_5c33a0", "@List_DeleteAll@8")
ASM_PROC(FUN_5dc230)
ASM_PROC(FUN_5d4c10)

static BYTE* Med_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5c10d0]
extern "C" BYTE* __fastcall Med5c10d0_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5c10f0); }
// [$L_5c2f60]
extern "C" BYTE* __fastcall Med5c2f60_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5ce630); }
// [$L_5c3970]
extern "C" BYTE* __fastcall Med5c3970_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5c3990); }
// [$L_5c4090]
extern "C" BYTE* __fastcall Med5c4090_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5c4410); }
// [$L_5c5350]
extern "C" BYTE* __fastcall Med5c5350_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5c53e0); }
// [$L_5c8760]
extern "C" BYTE* __fastcall Med5c8760_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5c8780); }
// [$L_5cdc90]
extern "C" BYTE* __fastcall Med5cdc90_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5cdcb0); }
// [$L_5ce590]
extern "C" BYTE* __fastcall Med5ce590_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5ce5b0); }
// [$L_5ce850]
extern "C" BYTE* __fastcall Med5ce850_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5ce930); }
// [$L_5ceac0]
extern "C" BYTE* __fastcall Med5ceac0_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5ceae0); }
// [$L_5cf150]
extern "C" BYTE* __fastcall Med5cf150_DeletingDestructor(BYTE* s, void*, int flags) { return Med_DeleteAfter(s, flags, FUN_5cf170); }

// static initializers of constants

// [$L_5c63e0]
extern "C" void __cdecl MedInit_5c63e0() {  }
ASM_VAR(AsmLabel, s_med671204, $L_671204)
ASM_VAR(AsmLabel, g_med64d81c, $L_64d81c)

// [$L_5c3960]
extern "C" void __fastcall Med_Nothing8(BYTE*, void*, int, int) {}
// [$L_5ce840] takes dt off the time at +16
extern "C" void __fastcall Med_SubTime(BYTE* s, void*, double dt) { F(double, s, 16) = F(double, s, 16) - dt; }
// [$L_5d2300] forwards to vtbl+8
extern "C" int __fastcall Med_Forward8(BYTE* s, void*, int a) { return CallVirt<int>(s, 8, a); }

// [$L_5c4310] unloads the room, then the two players at +7976 and +8300
extern "C" void __fastcall Med_Unload(BYTE* s)
{
    CallThis(FUN_5da8a0, s);
    F(int, s, 488) = 0;
    CallThis(FUN_5c2fe0, s + 7976);
    CallThis(FUN_5c33a0, s + 8300);
}

// [$L_5c4100] vtbl+188 with the translated text
extern "C" int __fastcall Med_5c4100(BYTE* s, void*, int a)
{
    const char* t = CallThis<const char*>(FUN_5dc230, s, (const char*)s_med671204);
    return CallVirt<int>(s, 188, t, a, 0);
}

// [$L_5ce6b0] vtbl+24 (a) of each item of the chain at +24 (next +8)
extern "C" void __fastcall Med_ChainCall24(BYTE* s, void*, int a)
{
    for (BYTE* o = F(BYTE*, s, 24); o; o = F(BYTE*, o, 8))
        CallVirt(o, 24, a);
}

// [$L_5d22c0] a new 12-byte object (vtable 64d81c) given a through its vtbl+12
extern "C" BYTE* __fastcall Med_NewHolder(BYTE*, void*, int a)
{
    BYTE* o = (BYTE*)operator new(12);
    if (o) {
        F(int, o, 4) = 0;
        F(int, o, 8) = 0;
        F(void*, o, 0) = g_med64d81c;
    }
    CallVirt(o, 12, a);
    return o;
}

// [$L_5ce990] the surface of a (+48) through its vtbl+116 (1, 66, s+60, +36, 0); a result goes to FUN_5d4c10
extern "C" void __fastcall Med_5ce990(BYTE* s, void*, BYTE* a)
{
    BYTE* o = F(BYTE*, a, 48);
    void* fn = *(void**)(F(BYTE*, o, 0) + 116);
    int r = ((int(__stdcall*)(BYTE*, int, int, BYTE*, int, int))fn)(o, 1, 66, s + 60, F(int, s, 36), 0);
    if (r)
        CallC(FUN_5d4c10, r, 0);
}

extern "C" void FUN_5ab110(); KING_ALIAS("_FUN_5ab110", "_File_WriteShort")
extern "C" void Med_fwrite(); KING_ALIAS("_Med_fwrite", "_fwrite")

// [$L_5d21e0] writes the chunk tag 65534, then +8 and +4
extern "C" void __fastcall Med_Write5d21e0(BYTE* s, void*, void* f)
{
    CallC(FUN_5ab110, 65534, f);
    int v = F(int, s, 8);
    CallC(Med_fwrite, &v, 4, 1, f);
    int w = F(int, s, 4);
    CallC(Med_fwrite, &w, 4, 1, f);
}

// ---------------------------------------------------------------------------
// Sound channels, list boxes and their readers.

extern "C" void FUN_5cba00(); KING_ALIAS("_FUN_5cba00", "@Branch_Construct@16")
extern "C" void FUN_5f36d0(); KING_ALIAS("_FUN_5f36d0", "@Obj_DrawTested@16")
ASM_PROC(FUN_5b1020)
extern "C" void FUN_5cf3f0(); KING_ALIAS("_FUN_5cf3f0", "@Obj_Notify104@8")
extern "C" void FUN_5c0c00(); KING_ALIAS("_FUN_5c0c00", "@List_IndexOf300@8")
extern "C" void FUN_5c0c20(); KING_ALIAS("_FUN_5c0c20", "@List_Select@12")
extern "C" void FUN_5ab0f0(); KING_ALIAS("_FUN_5ab0f0", "_File_ReadShort")
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
extern "C" void FUN_5d32f0(); KING_ALIAS("_FUN_5d32f0", "@Queue_PostEvent@12")
extern "C" void Med_printf(); KING_ALIAS("_Med_printf", "_printf")
extern "C" void Med_exit(); KING_ALIAS("_Med_exit", "_exit")
extern "C" void Med_fread(); KING_ALIAS("_Med_fread", "_fread")
ASM_VAR(float, g_med64c264, $L_64c264)
ASM_VAR(double, g_med64b678, $L_64b678)
ASM_VAR(AsmLabel, s_med67fad0, $L_67fad0)

// the channel at +off, made (40 bytes, FUN_5cba00 (self, a)) on first use while the volume (+24) is not negative
static BYTE* Med_Channel(BYTE* s, int off, int a)
{
    if (!(F(float, s, 24) >= g_med64c264))
        return 0;
    if (F(BYTE*, s, off))
        return F(BYTE*, s, off);
    BYTE* o = (BYTE*)operator new(40);
    if (!o) {
        F(BYTE*, s, off) = 0;
        return 0;
    }
    return F(BYTE*, s, off) = CallThis<BYTE*>(FUN_5cba00, o, s, a);
}
// [$L_5cbb30]
extern "C" BYTE* __fastcall Med_Channel8(BYTE* s, void*, int a) { return Med_Channel(s, 8, a); }
// [$L_5cbb80]
extern "C" BYTE* __fastcall Med_Channel4(BYTE* s, void*, int a) { return Med_Channel(s, 4, a); }
// [$L_5cbbd0] the same at +12 with full volume
extern "C" BYTE* __fastcall Med_Channel12(BYTE* s, void*, int a)
{
    if (!(F(float, s, 24) >= g_med64c264))
        return 0;
    if (F(BYTE*, s, 12))
        return F(BYTE*, s, 12);
    BYTE* o = (BYTE*)operator new(40);
    F(BYTE*, s, 12) = o ? CallThis<BYTE*>(FUN_5cba00, o, s, a) : 0;
    F(DWORD, F(BYTE*, s, 12), 24) = 0x3f800000;
    return F(BYTE*, s, 12);
}

// [$L_5cf310]
extern "C" int __fastcall Med_5cf310(BYTE* s, void*, int a, BYTE* b)
{
    if (!F(int, s, 64))
        return 0;
    F(int, s, 32) = 0;
    int r = CallThis<int>(FUN_5f36d0, s, a, b);
    if (!r)
        return r;
    CallThis(FUN_5b1020, b, F(int, s, 124), 0, 0, 0);
    CallThis(FUN_5cf3f0, s - 32);
    return 1;
}

// [$L_5cf410] the sound (+96) at the time of +100; once it has played out it is deleted and vtbl+28 (1)
extern "C" void __fastcall Med_5cf410(BYTE* s)
{
    BYTE* o = F(BYTE*, s, 96);
    if (!o)
        return;
    CallVirt(o, 20, F(double, F(BYTE*, s, 100), 2960));
    o = F(BYTE*, s, 96);
    if (F(double, o, 16) > g_med64b678) {
        CallThis(FUN_5d32f0, s, 1);
        return;
    }
    if (o)
        CallVirt(o, 0, 1);
    F(int, s, 96) = 0;
    CallVirt(s, 28, 1);
}

// [$L_5ce640] the sounds of the list (+24, next +8) at time (a, b); played-out ones are removed
extern "C" void __fastcall Med_5ce640(BYTE* s, void*, int a, int b)
{
    for (BYTE* o = F(BYTE*, s, 24); o;) {
        BYTE* next = F(BYTE*, o, 8);
        CallVirt(o, 20, a, b);
        if (!(F(double, o, 16) > g_med64b678)) {
            if (F(BYTE*, s, 24) == o)
                F(BYTE*, s, 24) = next;
            CallThis(FUN_5d21b0, o);
            if (o)
                CallVirt(o, 0, 1);
        }
        o = next;
    }
    if (!F(BYTE*, s, 24)) {
        F(DWORD, s, 16) = 0;
        F(DWORD, s, 20) = 0xbff00000;
    }
}

// [$L_5c0aa0] a list box: Enter gives its command, up and down move the selection
extern "C" int __fastcall Med_ListKey(BYTE* s, void*, int key, int up)
{
    if (up)
        return key;
    if (key == 13)
        return F(int, s, 28) | 0x8000;
    if (key == 38) {
        CallThis(FUN_5c0c20, s, CallThis<int>(FUN_5c0c00, s) - 1);
        return 32806;
    }
    if (key == 40) {
        CallThis(FUN_5c0c20, s, CallThis<int>(FUN_5c0c00, s) + 1);
        return 32808;
    }
    return key;
}

// [$L_5d2230] reads the chunk (tag 65534, then the flags +8 and +4)
extern "C" void __fastcall Med_Read5d2230(BYTE* s, void*, void* f)
{
    if (CallC<int>(FUN_5ab0f0, f) != 65534) {
        CallC(Med_printf, (const char*)s_med67fad0);
        CallC(Med_exit, 0);
    }
    int v;
    CallC(Med_fread, &v, 4, 1, f);
    F(int, s, 8) = v ? 1 : 0;
    CallC(Med_fread, &v, 4, 1, f);
    F(int, s, 4) = v ? 1 : 0;
}

// ---------------------------------------------------------------------------
// More virtual methods.

// [$L_5c0b00] a list box: the scroll bar (+340) takes its events, a click in the rows selects the row (+300)
extern "C" int __fastcall Med_ListMouse(BYTE* s, void*, int msg, int x, int y)
{
    BYTE* sb = F(BYTE*, s, 340);
    if (CallVirt<int>(sb, 24, x - F(int, s, 44), y - F(int, s, 48)))
        return CallVirt<int>(F(BYTE*, s, 340), 32, msg, x - F(int, s, 44), y - F(int, s, 48));
    int px = x - (F(int, s, 44) + F(int, s, 312));
    int py = y - (F(int, s, 48) + F(int, s, 316));
    if (px < 0 || py < 0)
        return msg;
    if (px > F(int, s, 320) - F(int, s, 312))
        return msg;
    if (py > F(int, s, 324) - F(int, s, 316))
        return msg;
    int row = py / (F(int, F(BYTE*, s, 16), 20) + F(int, s, 344));
    for (BYTE* p = F(BYTE*, s, 336); p; p = F(BYTE*, p, 8), row--)
        if (row <= 0) {
            F(BYTE*, s, 300) = p;
            break;
        }
    return F(int, s, 28) | 0x8000;
}

extern "C" void FUN_5c3a80(); KING_ALIAS("_FUN_5c3a80", "@Music_Stop@8")
extern "C" void FUN_5e3010(); KING_ALIAS("_FUN_5e3010", "@CGameApp_Destruct@8")
extern "C" void Med_VecDtor(); KING_ALIAS("_Med_VecDtor", "??_M@YGXPAXIHP6EX0@Z@Z")
ASM_VAR(AsmLabel, g_med65195c, $L_65195c)
ASM_VAR(AsmLabel, g_med651dcc, $L_651dcc)

// [FUN_5c3260]
extern "C" void __fastcall Med5c3260_Destroy(BYTE* s)
{
    CallThis(FUN_5c33a0, s);
    F(void*, s, 0) = g_med64d81c;
}
// [FUN_5c3990] (vtable 65195c)
extern "C" void __fastcall Med5c3990_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_med65195c;
    CallThis(FUN_5c3a80, s);
    CallThis(FUN_5e3010, s);
}
// [FUN_5c2f80] the table, then the ten 32-byte channels
extern "C" void __fastcall Med5c2f80_Destroy(BYTE* s)
{
    CallThis(FUN_5c2fe0, s);
    CallStd(Med_VecDtor, s, 32, 10, FUN_5ce630);
}
// [FUN_5ce930] (vtable 651dcc) frees +40 and +56
extern "C" void __fastcall Med5ce930_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_med651dcc;
    operator delete(F(void*, s, 40));
    operator delete(F(void*, s, 56));
    F(void*, s, 0) = g_med64d81c;
}

// ---------------------------------------------------------------------------
// Constructors and destructors (exception frames dropped).

extern "C" void FUN_5d33b0(); KING_ALIAS("_FUN_5d33b0", "@Handle_Release@8")
extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
extern "C" void FUN_5ef780(); KING_ALIAS("_FUN_5ef780", "@Node_Remove@8")
extern "C" void FUN_5c4340(); KING_ALIAS("_FUN_5c4340", "@Rec_Close@8")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
extern "C" void FUN_5c3260(); KING_ALIAS("_FUN_5c3260", "@Med5c3260_Destroy@4")
extern "C" void FUN_5c2f80(); KING_ALIAS("_FUN_5c2f80", "@Med5c2f80_Destroy@4")
extern "C" void FUN_5ddb40(); KING_ALIAS("_FUN_5ddb40", "@MainView_Destroy@4")
extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")
ASM_VAR(AsmLabel, g_med651e80, $L_651e80)
ASM_VAR(AsmLabel, g_med651e34, $L_651e34)
ASM_VAR(AsmLabel, g_med651db0, $L_651db0)
ASM_VAR(AsmLabel, g_med651a20, $L_651a20)
ASM_VAR(BYTE*, g_med707f6c, $L_707f6c)

// [FUN_5cf370] (vtables 651e80, 651e34 at +32)
extern "C" void __fastcall Med5cf370_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_med651e80;
    F(void*, s, 32) = g_med651e34;
    CallThis(FUN_5ef780, s + 32);
    CallThis(FUN_5d33b0, s);
    CallThis(FUN_5f1a70, s ? s + 32 : 0);
    CallThis(FUN_5d3290, s);
}
// [FUN_5ce5b0] a sound list (vtable 651db0) deletes its sounds (+24, next +8)
extern "C" void __fastcall Med5ce5b0_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_med651db0;
    for (BYTE* c = F(BYTE*, s, 24); c;) {
        BYTE* next = F(BYTE*, c, 8);
        if (c)
            CallVirt(c, 0, 1);
        c = next;
    }
    F(void*, s, 0) = g_med64d81c;
}
// [FUN_5c4410] the media player (vtable 651a20)
extern "C" void __fastcall Media_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_med651a20;
    if (F(int, g_med707f6c, 416))
        operator delete(F(void*, s, 7924));
    CallThis(FUN_5c4340, s);
    CallThis(FUN_602e80, s + 8336);
    CallThis(FUN_5c3260, s + 8300);
    CallThis(FUN_5c2f80, s + 7976);
    CallThis(FUN_5ddb40, s);
}

// ---------------------------------------------------------------------------
// Constructors, destructors and setup (exception frames dropped).

ASM_PROC(FUN_5dc4d0)
ASM_PROC(FUN_5c2f10)
extern "C" void FUN_602be0(); KING_ALIAS("_FUN_602be0", "@DpMessage_Construct@8")
extern "C" void FUN_5e3150(); KING_ALIAS("_FUN_5e3150", "@CGameApp_Construct@16")
extern "C" void FUN_6021f0(); KING_ALIAS("_FUN_6021f0", "_Gate_Start")
#include <string.h>
ASM_VAR(AsmLabel, g_med651948, $L_651948)
ASM_VAR(AsmLabel, s_med6730a0, $L_6730a0)
ASM_VAR(AsmLabel, s_med67f888, $L_67f888)
ASM_VAR(AsmLabel, s_med67f890, $L_67f890)
ASM_VAR(AsmLabel, s_med67f87c, $L_67f87c)
ASM_VAR(AsmLabel, s_med6729a8, $L_6729a8)
ASM_VAR(AsmLabel, s_med672994, $L_672994)
ASM_VAR(BYTE*, g_med707f70, $L_707f70)
ASM_VAR(BYTE*, g_med67fb7c, $L_67fb7c)
ASM_VAR(AsmLabel, g_medHook5c37e0, $L_5c37e0)

// [FUN_5c3f20] constructor (a): the media player (vtable 651a20); with sound (707f6c +416) its device name from the ini
extern "C" BYTE* __fastcall Media_Construct(BYTE* s, void*, int a)
{
    CallThis(FUN_5dc4d0, s, a);
    CallThis(FUN_5c2f10, s + 7976, s);
    F(int, s, 8304) = 0;
    F(int, s, 8308) = 0;
    F(void*, s, 8300) = g_med651948;
    F(int, s, 8312) = -1;
    F(int, s, 8316) = -1;
    F(int, s, 8320) = 0;
    F(BYTE*, s, 8332) = s;
    CallThis(FUN_602be0, s + 8336);
    F(int, s, 7960) = 0;
    F(void*, s, 0) = g_med651a20;
    F(int, s, 7924) = 0;
    F(int, s, 7928) = 0;
    F(int, s, 7936) = 0;
    F(int, s, 7932) = 0;
    F(DWORD, s, 7964) = 0x3ff00000;
    F(int, s, 7944) = 1;
    F(int, s, 7948) = 1;
    F(int, s, 7952) = 0;
    F(int, s, 7920) = 0;
    if (F(int, g_med707f6c, 416)) {
        char buf[64];
        GetPrivateProfileStringA((const char*)s_med6730a0, (const char*)s_med67f888, (const char*)s_med67f890, buf, 50,
                                 F(const char*, s, 640));
        size_t n = strlen(buf) + 1;
        char* t = (char*)operator new(n);
        F(char*, s, 7924) = t;
        memcpy(t, buf, n);
    }
    return s;
}

// [FUN_5c3800] constructor (a, b): the sound system (vtable 65195c, also 707f6c); network test mode from the ini
extern "C" BYTE* __fastcall SoundSystem_Construct(BYTE* s, void*, int a, int b)
{
    CallThis(FUN_5e3150, s, a, b);
    F(int, s, 408) = 0;
    F(int, s, 412) = 0;
    F(void*, s, 0) = g_med65195c;
    g_med707f6c = s;
    g_med707f70 = g_med67fb7c;
    g_med67fb7c = (BYTE*)g_medHook5c37e0;
    char buf[80];
    GetPrivateProfileStringA((const char*)s_med6730a0, (const char*)s_med67f87c, (const char*)s_med6729a8, buf, 16,
                             F(const char*, s, 280));
    F(int, s, 416) = strcmp(buf, (const char*)s_med672994) == 0 ? 1 : 0;
    if (F(int, s, 416)) {
        char t[12] = "nettest";
        if (CallC<int>(FUN_6021f0, t))
            F(int, s, 416) = 0;
    }
    F(int, s, 440) = 1;
    F(int, s, 420) = 0;
    F(int, s, 432) = 0;
    F(int, s, 436) = 0;
    F(int, s, 444) = 0;
    F(int, s, 424) = 0;
    F(int, s, 428) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Scroll bar construction and chat (exception frames dropped).

extern "C" void FUN_6021d0(); KING_ALIAS("_FUN_6021d0", "_Dp_SendTo")
extern "C" void FUN_603110(); KING_ALIAS("_FUN_603110", "@Msg_Write120@20")
extern "C" void FUN_602e80(); KING_ALIAS("_FUN_602e80", "@MsgBuf_Destroy@8")
typedef BYTE (__cdecl* MedChatFn)(const char*, int);

// [FUN_5c5450] (-, text): in a network game (707f6c +420 == 2) the chat text (at most 64 characters) is sent
extern "C" int __fastcall Media_SendChat(BYTE* s, void*, int, const char* text)
{
    if (F(int, g_med707f6c, 420) != 2)
        return 1;
    BYTE pkt[32];
    CallThis(FUN_602be0, pkt);
    size_t n = strlen(text);
    if (n > 64) {
        CallThis(FUN_602e80, pkt);
        return 0;
    }
    int id = F(int, g_med707f6c, 440);
    BYTE d[80];
    MedChatFn fn = F(MedChatFn, s, 444);
    d[1] = fn ? fn(text, id) : 255;
    F(int, g_med707f6c, 440)++;
    d[0] = (BYTE)id;
    memcpy(d + 2, text, n + 1);
    CallThis(FUN_603110, pkt, 901, d, (int)n + 3);
    CallC(FUN_6021d0, F(int, s, 408), F(BYTE*, pkt, 4), F(BYTE*, pkt, 8) - F(BYTE*, pkt, 4));
    CallThis(FUN_602e80, pkt);
    return 1;
}

// ---------------------------------------------------------------------------
// Constructors and surfaces (exception frames dropped).

extern "C" void FUN_5d31e0(); KING_ALIAS("_FUN_5d31e0", "@Object_ConstructKind@16")
extern "C" void FUN_5f19b0(); KING_ALIAS("_FUN_5f19b0", "@NodeN_Construct@20")
extern "C" void FUN_5d3570(); KING_ALIAS("_FUN_5d3570", "@Owner_Attach@12")
ASM_VAR(AsmLabel, s_q20_67fa68, $L_67fa68)
ASM_VAR(AsmLabel, g_q20_68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, g_q20_64c26c, $L_64c26c)
ASM_VAR(AsmLabel, g_q20_651e80, $L_651e80)
ASM_VAR(AsmLabel, g_q20_651e34, $L_651e34)
ASM_VAR(AsmLabel, g_q20_7025f0, $L_7025f0)
ASM_VAR(int, g_q20_720908, $L_720908)

// [FUN_5cf180] constructor (world, name, owner): a media object (vtables 651e80, 651e34 at +32) registered with
// its owner (or the world's +524), its entry (+156) the world room list item called name
extern "C" BYTE* __fastcall MediaObj_Construct(BYTE* s, void*, BYTE* w, const char* name, BYTE* owner)
{
    CallThis(FUN_5d31e0, s, 100, (const char*)s_q20_67fa68);
    CallThis(FUN_5f19b0, s + 32, (BYTE*)g_q20_68d1c0, 5, 1);
    F(void*, s, 32) = g_q20_64c26c;
    F(int, s, 88) = 0;
    F(int, s, 92) = 0;
    memcpy(s + 104, g_q20_7025f0, 48);
    F(int, s, 152) = 0;
    F(void*, s, 0) = g_q20_651e80;
    F(void*, s, 32) = g_q20_651e34;
    BYTE* o = owner ? owner : F(BYTE*, w, 524);
    if (o)
        CallVirt(o, 20, 0, s ? s + 32 : 0);
    F(BYTE*, s, 100) = w;
    CallThis(FUN_5d3570, w + 2992, s);
    BYTE* l = F(BYTE*, w, 468);
    int n = F(int, l, 72), i = 0;
    for (; i < n; i++)
        if (!strcmp(name, (const char*)(F(BYTE*, l, 76) + i * 124)))
            break;
    F(BYTE*, s, 156) = i < n ? F(BYTE*, l, 76) + i * 124 : 0;
    F(int, s, 96) = 0;
    F(int, s, 152) = g_q20_720908;
    return s;
}
