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
extern "C" void FUN_5f4a90(); KING_ALIAS("_FUN_5f4a90", "@Value_Free@4")
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

extern "C" void FUN_5cfb30(); KING_ALIAS("_FUN_5cfb30", "_Poly_Draw")
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
extern "C" void FUN_5be8c0(); KING_ALIAS("_FUN_5be8c0", "@ScrollBar_Layout@40")    // lays out list rows (thiscall on the drop part)
ASM_FUNC(List_RowCallback, $L_5c0700)
extern "C" void FUN_5c06d0(); KING_ALIAS("_FUN_5c06d0", "@List_VisibleRows@4")    // visible rows of a list
ASM_PROC(FUN_6303c4)    // ICDecompress
ASM_PROC(FUN_6303b8)    // ICSendMessage
extern "C" void FUN_5c29d0(); KING_ALIAS("_FUN_5c29d0", "_Media_Start")
extern "C" void FUN_5c1d40(); KING_ALIAS("_FUN_5c1d40", "@Video_DrawFrame@28")    // draws the video frame
ASM_PROC(FUN_5c34a0)
extern "C" void FUN_6021b0(); KING_ALIAS("_FUN_6021b0", "_Dp_Send")
extern "C" void FUN_602df0(); KING_ALIAS("_FUN_602df0", "@DpMessage_Reset@8")
extern "C" void FUN_5c49b0(); KING_ALIAS("_FUN_5c49b0", "@Rec_Step@12")    // next record time (forward)
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
extern "C" void FUN_5e02f0(); KING_ALIAS("_FUN_5e02f0", "@View_PlaceCamera@4")
extern "C" void FUN_5c4940(); KING_ALIAS("_FUN_5c4940", "@Media_ClearList@8")
extern "C" void FUN_5cc6b0(); KING_ALIAS("_FUN_5cc6b0", "@Tree_Grow@20")

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
extern "C" void FUN_5e03c0(); KING_ALIAS("_FUN_5e03c0", "@World_Reset@4")
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
extern "C" void $L_5c6120(); KING_ALIAS("_$L_5c6120", "_View_ShowNamed")
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
extern "C" void FUN_5cdcb0(); KING_ALIAS("_FUN_5cdcb0", "@Med5cdcb0_Destruct@8")
extern "C" void FUN_5ce5b0(); KING_ALIAS("_FUN_5ce5b0", "@Med5ce5b0_Destroy@4")
extern "C" void FUN_5ce930(); KING_ALIAS("_FUN_5ce930", "@Med5ce930_Destroy@4")
extern "C" void FUN_5ceae0(); KING_ALIAS("_FUN_5ceae0", "@Media5ceae0_Destroy@8")
extern "C" void FUN_5cf170(); KING_ALIAS("_FUN_5cf170", "@Thunk5cf170@4")
extern "C" void FUN_5da8a0(); KING_ALIAS("_FUN_5da8a0", "@Fw_UnloadRoom@8")
extern "C" void FUN_5c2fe0(); KING_ALIAS("_FUN_5c2fe0", "@Table_Clear@8")
extern "C" void FUN_5c33a0(); KING_ALIAS("_FUN_5c33a0", "@List_DeleteAll@8")
extern "C" void FUN_5dc230(); KING_ALIAS("_FUN_5dc230", "@App_Path@12")
extern "C" void FUN_5d4c10(); KING_ALIAS("_FUN_5d4c10", "_Dd_ErrorText")

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

extern "C" void FUN_5dc4d0(); KING_ALIAS("_FUN_5dc4d0", "@MainView_Construct@12")
extern "C" void FUN_5c2f10(); KING_ALIAS("_FUN_5c2f10", "@Lights5c2f10_Construct@12")
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
extern "C" void __cdecl Media_ExitHook(void);

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
    g_med67fb7c = (BYTE*)Media_ExitHook;
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

// ---------------------------------------------------------------------------
// Network item destructor, sorted list box insert (exception frames dropped).

extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
extern "C" void FUN_6030b0(); KING_ALIAS("_FUN_6030b0", "@Msg_Write45@16")
extern "C" void FUN_5c3630(); KING_ALIAS("_FUN_5c3630", "@Media_RecordBlock@12")
extern "C" void FUN_5c5c00(); KING_ALIAS("_FUN_5c5c00", "@Timeline_Clear@12")
extern "C" void FUN_5c40b0(); KING_ALIAS("_FUN_5c40b0", "@Pool_Keep@12")
extern "C" void FUN_5a47f0(); KING_ALIAS("_FUN_5a47f0", "@Wea5a47f0_Destroy@4")
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")
extern "C" void FUN_5c06d0(); KING_ALIAS("_FUN_5c06d0", "@List_VisibleRows@4")
extern "C" void FUN_5c0c20(); KING_ALIAS("_FUN_5c0c20", "@List_Select@12")
ASM_VAR(AsmLabel, g_q23_651c34, $L_651c34)
ASM_VAR(AsmLabel, g_q23_651be8, $L_651be8)
ASM_VAR(AsmLabel, g_q23_6518e4, $L_6518e4)

// [FUN_5c6210] destructor: a networked item (vtables 651c34, 651be8 at +256): its three parts (+352..360) deleted;
// unless remote (+372) in a network game its removal (904) is announced
extern "C" void __fastcall NetItem_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_q23_651c34;
    F(void*, s, 256) = g_q23_651be8;
    for (int o = 352; o <= 360; o += 4) {
        if (!F(BYTE*, s, o))
            continue;
        CallThis(FUN_5d21b0, F(BYTE*, s, o));
        if (F(BYTE*, s, o))
            CallVirt(F(BYTE*, s, o), 0, 1);
    }
    if (!F(int, s, 372) && (F(int, g_med707f6c, 416) || F(int, F(BYTE*, s, 36), 7928))) {
        BYTE a[32];
        CallThis(FUN_602be0, a);
        CallThis(FUN_6030b0, a, F(int, s, 364), 0);
        CallThis(FUN_5c3630, F(BYTE*, s, 36), a);
        CallThis(FUN_5c5c00, F(BYTE*, s, 36), F(int, s, 364));
        BYTE* m = (BYTE*)operator new(28);
        BYTE* p = m ? CallThis<BYTE*>(FUN_602be0, m) : 0;
        BYTE d[8];
        F(int, d, 0) = F(int, s, 364);
        d[4] = 0;
        CallThis(FUN_603110, p, 904, d, 5);
        CallThis(FUN_5c40b0, F(BYTE*, s, 36), p);
        CallThis(FUN_602e80, a);
    }
    CallThis(FUN_5a47f0, s);
}

// [FUN_5c0f00] (text): a list box line (16 bytes, vtable 6518e4, its text +12) inserted in order into the list
// (+296 first, +300 current, +336); the scroll range follows
extern "C" void __fastcall ListBox_AddSorted(BYTE* s, void*, const char* text)
{
    BYTE* n = (BYTE*)operator new(16);
    if (n) {
        F(int, n, 4) = 0;
        F(int, n, 8) = 0;
        F(void*, n, 0) = g_q23_6518e4;
        size_t len = strlen(text) + 1;
        char* t = (char*)operator new(len);
        F(char*, n, 12) = t;
        memcpy(t, text, len);
    }
    BYTE* e = F(BYTE*, s, 296);
    if (!e) {
        F(BYTE*, s, 336) = n;
        F(BYTE*, s, 300) = n;
        F(BYTE*, s, 296) = n;
    } else {
        while (F(BYTE*, e, 8) && strcmp(text, F(const char*, e, 12)) > 0)
            e = F(BYTE*, e, 8);
        if (strcmp(text, F(const char*, e, 12)) > 0) {
            CallThis(FUN_5d2190, n, e);
        } else if (!F(BYTE*, e, 4)) {
            F(BYTE*, s, 336) = n;
            F(BYTE*, s, 300) = n;
            F(BYTE*, s, 296) = n;
            F(BYTE*, n, 8) = e;
            F(BYTE*, e, 4) = n;
        } else {
            CallThis(FUN_5d2190, n, F(BYTE*, e, 4));
        }
        F(BYTE*, s, 300) = n;
    }
    F(int, s, 328)++;
    if (F(int, s, 328) > CallThis<int>(FUN_5c06d0, s))
        F(WORD, F(BYTE*, s, 340), 296)++;
    CallThis(FUN_5c0c20, s, 0);
}

// ---------------------------------------------------------------------------
// Scalar deleting destructors: the destructor, then the memory when flag 1 is set.

extern "C" void FUN_5c6210(); KING_ALIAS("_FUN_5c6210", "@NetItem_Destroy@4")
// [$L_5c6060]
extern "C" BYTE* __fastcall Dtor5c6060_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5c6210, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

extern "C" void FUN_5cf370(); KING_ALIAS("_FUN_5cf370", "@Med5cf370_Destroy@4")
// [$L_5cf2f0]
extern "C" BYTE* __fastcall Dtor5cf2f0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5cf370, s);
    if (flags & 1)
        operator delete(s);
    return s;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

ASM_VAR(AsmLabel, g_q46_71e808, $L_71e808)
extern "C" AsmLabel g_q46_5d2080; KING_ALIAS("_g_q46_5d2080", "@MappedFile_Init@4")
// [FUN_5d2050] constructs the static 71e808
extern "C" int __cdecl Static71e808_Construct(void)
{
    return CallThis<int>((void (*)())(void*)g_q46_5d2080, (void*)g_q46_71e808);
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_5f1a70(); KING_ALIAS("_FUN_5f1a70", "@Node5f1a70_Destroy@4")
ASM_VAR(AsmLabel, g_q47_651d64, $L_651d64)
// [FUN_5cdcb0] destructor: sets +40 bit 0, the 651d64 vtable, then the node
extern "C" void __fastcall Med5cdcb0_Destruct(BYTE* s, void*)
{
    F(BYTE, s, 40) |= 1;
    F(void*, s, 0) = g_q47_651d64;
    CallThis(FUN_5f1a70, s);
}

// ---------------------------------------------------------------------------
// Small setup procs: static vectors, a light array, the race start, a stream read.

extern "C" void FUN_5ce630(); KING_ALIAS("_FUN_5ce630", "@Light_DestructBase@8")
extern "C" void Med_VecCtor(); KING_ALIAS("_Med_VecCtor", "??_L@YGXPAXIHP6EX0@Z1@Z")
extern "C" AsmLabel g_q48_5c2f40; KING_ALIAS("_g_q48_5c2f40", "@MedRec_Init@4")
// [FUN_5c2f10] constructor (x): ten 32-byte lights, x at +320
extern "C" BYTE* __fastcall Lights5c2f10_Construct(BYTE* s, void*, int x)
{
    CallStd(Med_VecCtor, s, 32, 10, (BYTE*)g_q48_5c2f40, FUN_5ce630);
    F(int, s, 320) = x;
    return s;
}

// ---------------------------------------------------------------------------
// Small methods and static helpers: guarded static construction, releases, frees.

typedef ULONG(__stdcall* ComRelease)(void*);
// [$L_5c2490] releases the COM object at +0
extern "C" void __fastcall ComPtr_Release(BYTE* s, void*)
{
    void* p = F(void*, s, 0);
    if (p)
        ((ComRelease)(*(void***)p)[2])(p);
}

// ---------------------------------------------------------------------------
// Small methods: table constructors and clears, list teardown, cursor handling, flag maps.

ASM_VAR(int, g_q52_71e844, $L_71e844)
// [$L_5d2310] constructor: an empty table of 500 pointers (+8, capacity +4), marks 71e844
extern "C" BYTE* __fastcall PtrTable500_Construct(BYTE* s, void*)
{
    F(int, s, 12) = 0;
    F(int, s, 4) = 500;
    F(int, s, 0) = 0;
    F(void**, s, 8) = (void**)operator new(2000);
    for (int k = 0; k < F(int, s, 4); k++)
        F(void**, s, 8)[k] = 0;
    g_q52_71e844 = 1;
    return s;
}

// ---------------------------------------------------------------------------
// Small methods: pool slots, list teardown, loader thread, frame shutdown, message dispatch.

extern "C" void FUN_5ef2b0(); KING_ALIAS("_FUN_5ef2b0", "_Scene_FindObject")
// [$L_5c6120] shows the scene object named name in the view o (resets +256 by slot 36, then slot 40 with the object
// and the text after the name)
extern "C" void __cdecl View_ShowNamed(BYTE* o, const char* name)
{
    int obj = CallC<int>(FUN_5ef2b0, 0, name);
    const char* rest = name + strlen(name) + 1;
    if (obj && rest) {
        CallVirt(o + 256, 36);
        CallVirt(o, 40, obj, rest);
    }
}

// ---------------------------------------------------------------------------
// The media thread start.

extern "C" void FUN_5c24a0(); KING_ALIAS("_FUN_5c24a0", "_Msavi_OpenSound")
extern "C" void Mfc_AfxBeginThread(); KING_ALIAS("_Mfc_AfxBeginThread", "?AfxBeginThread@@YGPAVCWinThread@@P6AIPAX@Z0HIKPAU_SECURITY_ATTRIBUTES@@@Z")
ASM_VAR(int, g_q54_702f60, $L_702f60)
ASM_VAR(HANDLE, g_q54_702f58, $L_702f58)
ASM_VAR(HANDLE, g_q54_702f5c, $L_702f5c)
ASM_VAR(int, g_q54_702f98, $L_702f98)
ASM_VAR(int, g_q54_702f9c, $L_702f9c)
ASM_VAR(int, g_q54_702f94, $L_702f94)
extern "C" unsigned __cdecl Media_ThreadProc(void*);
ASM_VAR(AsmLabel, s_q54_67f854, $L_67f854)
// [FUN_5c2e60] starts the media thread for the window w: COM, two events; without a device d it opens one
// (logs and gives up on failure), then the 5c2d70 thread
extern "C" void __cdecl Media_StartThread(int w, int d)
{
    CoInitialize(0);
    g_q54_702f60 = w;
    g_q54_702f58 = CreateEventA(0, 0, 0, 0);
    g_q54_702f5c = CreateEventA(0, 0, 0, 0);
    if (!d) {
        g_q54_702f9c = 1;
        if (CallC<int>(FUN_5c24a0, g_q54_702f60) < 0) {
            CallC(FUN_5d2ba0, (const char*)s_q54_67f854, 0);
            return;
        }
    } else {
        g_q54_702f98 = d;
        g_q54_702f9c = 0;
    }
    CallStd(Mfc_AfxBeginThread, (void*)Media_ThreadProc, 0, 1, 4096, 0, 0);
    g_q54_702f94 = 0;
}

// ---------------------------------------------------------------------------
// Polygon span fill.

ASM_PROC(FUN_5fedbc)
ASM_PROC(FUN_5fef04)
ASM_VAR(int, g_q56_71e7dc, $L_71e7dc)
ASM_VAR(int, g_q56_71e7d8, $L_71e7d8)
ASM_VAR(int, g_q56_71e7bc, $L_71e7bc)
ASM_VAR(int, g_q56_71e7c4, $L_71e7c4)
ASM_VAR(int, g_q56_70f3d8, $L_70f3d8)
ASM_VAR(int, g_q56_70f3f0, $L_70f3f0)
// [FUN_5cf630] fills the spans of the rows 71e7dc..71e7d8 from the edge tables (70f3f0 left, 31200 bytes on the
// right, 24 bytes a row: x in 12.20 fixed point, z at +20) with the color c; mode | 2 set first
extern "C" void __cdecl Span_FillRows(int mode, int c)
{
    CallC(FUN_5fedbc, mode | 2, 0);
    int y = g_q56_71e7dc;
    int row = y * g_q56_71e7bc;
    if (y > g_q56_71e7d8)
        return;
    int* e = &g_q56_70f3f0 + y * 6;
    for (;;) {
        int l = e[0];
        int r = e[7800] >> 20;
        int z = e[5];
        int x = l >> 20;
        if (l & 0xf0000)
            x++;
        if (x <= r) {
            int n = r - x + 1;
            CallC(FUN_5fef04, g_q56_70f3d8 * x + g_q56_71e7c4 + row, n, z, (e[7805] - z) / n, c, 0, 0, 0);
        }
        y++;
        e += 6;
        row += g_q56_71e7bc;
        if (y > g_q56_71e7d8)
            break;
    }
}

// ---------------------------------------------------------------------------
// The media thread.

extern "C" void FUN_5c2ba0(); KING_ALIAS("_FUN_5c2ba0", "_Msavi_HandleNotification")
extern "C" void FUN_5c26f0(); KING_ALIAS("_FUN_5c26f0", "_Msavi_Free")
ASM_VAR(AsmLabel, s_q58_67f830, $L_67f830)
ASM_VAR(AsmLabel, s_q58_67f818, $L_67f818)
// [$L_5c2d70] the media thread: waits on the two events (702f58, 702f5c) and the messages; the first event runs
// the movie notifications (a failure other than E_FAIL is logged) and ends the thread, as does the 702f94 flag;
// frees the movie and closes the events
extern "C" unsigned __cdecl Media_ThreadProc(void*)
{
    g_q54_702f94 = 0;
    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(2, &g_q54_702f58, FALSE, INFINITE, QS_ALLINPUT);
        if (r == 0) {
            long hr = CallC<long>(FUN_5c2ba0);
            if (hr < 0) {
                if (hr != (long)0x80004005) {
                    CallC(FUN_5d2ba0, (const char*)s_q58_67f830, hr);
                    CallC(FUN_5d2ba0, (const char*)s_q58_67f818, 0);
                }
                g_q54_702f94 = 1;
                break;
            }
        } else if (r == 2) {
            MSG m;
            while (PeekMessageA(&m, 0, 0, 0, PM_REMOVE)) {
                TranslateMessage(&m);
                DispatchMessageA(&m);
            }
        }
        if (g_q54_702f94)
            break;
    }
    CallC(FUN_5c26f0);
    CloseHandle(g_q54_702f58);
    CloseHandle(g_q54_702f5c);
    return 1;
}

// ---------------------------------------------------------------------------
// A mapped file.

// [$L_5d20a0] closes it: the view (+12), the mapping (+4) and the file (+0); +20 cleared
extern "C" void __fastcall MappedFile_Close(BYTE* s)
{
    if (F(void*, s, 12))
        UnmapViewOfFile(F(void*, s, 12));
    HANDLE m = F(HANDLE, s, 4);
    F(void*, s, 12) = 0;
    if (m != INVALID_HANDLE_VALUE)
        CloseHandle(m);
    HANDLE f = F(HANDLE, s, 0);
    F(HANDLE, s, 4) = INVALID_HANDLE_VALUE;
    if (f != INVALID_HANDLE_VALUE)
        CloseHandle(f);
    F(HANDLE, s, 0) = INVALID_HANDLE_VALUE;
    F(BYTE, s, 20) = 0;
}

// ---------------------------------------------------------------------------
// List rows and edit field keys.

// [FUN_5c06d0] the rows a list shows: its height (+308) over the font's line height (+16's +20) and the spacing
// (+344); 0 without a font
extern "C" int __fastcall List_VisibleRows(BYTE* s)
{
    BYTE* f = F(BYTE*, s, 16);
    if (!f)
        return 0;
    int r = F(int, s, 308) / (F(int, f, 20) + F(int, s, 344));
    return r >= 0 ? r : 0;
}

// [$L_5c0040] a key of an edit field (text +288 of +292 bytes, cursor +296; +32 bit 31 = select all): with
// type 0 backspace, delete, left and right edit the text and enter gives the field's command (+28), handled keys
// with bit 15; else a character 32..255 is inserted (replacing all of a selected text)
extern "C" int __fastcall Edit_Key(BYTE* s, void*, int key, int type)
{
    char* t = F(char*, s, 288);
    if (!t)
        return key;
    if (type) {
        if (F(unsigned, s, 32) & 0x80000000) {
            F(unsigned, s, 32) &= 0x7fffffff;
            F(int, s, 296) = 0;
            *t = 0;
        }
        if (key >= 32 && key <= 255 && F(int, s, 296) < F(int, s, 292) - 2) {
            char* p = F(char*, s, 288) + F(int, s, 296);
            if (!*p)
                p[1] = 0;
            F(char*, s, 288)[F(int, s, 296)] = (char)key;
            F(int, s, 296)++;
        }
        return key;
    }
    int i;
    switch (key) {
    case 8:
        F(unsigned, s, 32) &= 0x7fffffff;
        if (F(int, s, 296) <= 0)
            break;
        i = --F(int, s, 296);
        if (t[i])
            do {
                t[i] = t[i + 1];
                i++;
            } while (F(char*, s, 288)[i]);
        break;
    case 46:
        F(unsigned, s, 32) &= 0x7fffffff;
        i = F(int, s, 296);
        if (t[i])
            do {
                t[i] = t[i + 1];
                i++;
            } while (F(char*, s, 288)[i]);
        break;
    case 37:
        F(unsigned, s, 32) &= 0x7fffffff;
        if (F(int, s, 296) > 0)
            F(int, s, 296)--;
        break;
    case 39:
        F(unsigned, s, 32) &= 0x7fffffff;
        if (t[F(int, s, 296)])
            F(int, s, 296)++;
        break;
    case 13:
        key = F(int, s, 28);
        F(unsigned, s, 32) &= 0x7fffffff;
        break;
    case 190:
        break;
    default:
        return key;
    }
    return key | 0x8000;
}

// ---------------------------------------------------------------------------
// Drawing a list.

extern "C" void FUN_5ea1b0(); KING_ALIAS("_FUN_5ea1b0", "_Font_DrawText")
extern "C" void FUN_5be1d0(); KING_ALIAS("_FUN_5be1d0", "@Ctl_DrawAll@24")
extern "C" void __cdecl Ctl_DrawFrame(BYTE* dst, const int* r, DWORD flags, int color, int a5, int light, int dark,
                                      int dx, int dy, int fill, BYTE* bmp, int pattern);    // controls.cpp
extern "C" int __fastcall List_VisibleRows(BYTE* s);
// [$L_5c0920] draws list s at x, y on dst: the controls, the frame (+312..+324 in the style of +104), the rows
// from the first shown (+336) while they fit (+308), the chosen one (+300) in its font (+20) and colour (+108);
// the scroll bar (+340) is hidden (+40 = 1) when all rows fit
extern "C" int __fastcall List_Draw(BYTE* s, void*, BYTE* dst, int x, int y, int a4)
{
    int line = F(int, F(BYTE*, s, 16), 20);
    int top = F(int, s, 48) + line + y + 2;
    CallThis(FUN_5be1d0, s, dst, x, y, a4);
    int r[4];
    r[0] = F(int, s, 312) + F(int, s, 44) + x;
    r[1] = F(int, s, 316) + F(int, s, 48) + y;
    r[2] = F(int, s, 320) + F(int, s, 44) + x;
    r[3] = F(int, s, 324) + F(int, s, 48) + y;
    Ctl_DrawFrame(dst, r, F(DWORD, s, 32), F(int, s, 104), F(int, s, 108), F(int, s, 112), F(int, s, 116),
                  F(int, s, 120), F(int, s, 124), F(int, s, 128), F(BYTE*, s, 132), F(int, s, 136));
    int dy = line;
    for (BYTE* e = F(BYTE*, s, 336); e && dy < F(int, s, 308); e = F(BYTE*, e, 8)) {
        bool sel = e == F(BYTE*, s, 300);
        CallC(FUN_5ea1b0, dst, r[0] + 2, top + dy, F(int, s, 320) - F(int, s, 312) - 2,
              sel ? F(BYTE*, s, 20) : F(BYTE*, s, 16), F(const char*, e, 12), sel ? F(int, s, 108) : F(int, s, 72), 0);
        dy += F(int, F(BYTE*, s, 16), 20) + F(int, s, 344);
    }
    F(int, F(BYTE*, s, 340), 40) = F(int, s, 328) > List_VisibleRows(s) ? 0 : 1;
    return 0;
}

extern "C" void FUN_5ce870(); KING_ALIAS("_FUN_5ce870", "@Mesh_ScaleColors@24")
// [$L_5ceb40] sets up mesh s with n vertices (5ce870) and keeps a copy of its first n index words (+40) at +172
extern "C" void __fastcall Mesh5ceb40_Setup(BYTE* s, void*, int n, int a2, int a3, int a4)
{
    CallThis(FUN_5ce870, s, n, a2, a3, a4);
    F(int, s, 176) = n;
    void* p = operator new(n * 4);
    F(void*, s, 172) = p;
    memcpy(p, F(void*, s, 40), n * 4);
}

extern "C" void FUN_5c3a80(); KING_ALIAS("_FUN_5c3a80", "@Music_Stop@8")
ASM_VAR(BYTE*, g_q128_707f6c, $L_707f6c)
// [$L_5c37e0] exit hook: stops the music, then runs the previous hook (707f70)
extern "C" void __cdecl Media_ExitHook(void)
{
    CallThis(FUN_5c3a80, g_q128_707f6c);
    ((void(__cdecl*)())g_med707f70)();
}

extern "C" void FUN_5c1b20(); KING_ALIAS("_FUN_5c1b20", "@Avi_ReadAudio@16")
ASM_VAR(int, g_q156_702f54, $L_702f54)
// [$L_5c1360] audio stream callback: reads up to n bytes from stream s into buf, stores the count in *got and
// adds it to 702f54
extern "C" int __cdecl Avi_AudioRead(int n, void* buf, int* got, BYTE* s)
{
    int v = n;
    int r = CallThis<int>(FUN_5c1b20, s, buf, &v);
    *got = v;
    g_q156_702f54 += v;
    return r;
}

// [FUN_5ce870] fills the +36 vertex colours of mesh s (+40) from the n colours (repeated), their red, green and
// blue scaled by k (alpha kept)
extern "C" void __fastcall Mesh_ScaleColors(BYTE* s, void*, int n, const DWORD* colors, double k)
{
    for (int i = 0; i < F(int, s, 36); i++) {
        DWORD c = colors[i % n];
        int r = (int)((double)(__int64)((c >> 16) & 255) * k);
        int g = (int)((double)(__int64)((c >> 8) & 255) * k);
        int b = (int)((double)(__int64)(c & 255) * k);
        F(DWORD*, s, 40)[i] = (c & 0xff000000) | (((r << 8) | g) << 8 | b);
    }
}

// ---------------------------------------------------------------------------
// The random tables of particles.

extern "C" int __cdecl rand(void);
ASM_VAR(double, g_q166_64b958, $L_64b958)
ASM_VAR(double, g_q166_64b670, $L_64b670)
ASM_VAR(AsmLabel, g_q166_70ef1c, $L_70ef1c)
ASM_VAR(AsmLabel, g_q166_70f0ac, $L_70f0ac)
ASM_VAR(AsmLabel, g_q166_70f23c, $L_70f23c)
ASM_VAR(AsmLabel, g_q166_70ebfc, $L_70ebfc)
ASM_VAR(AsmLabel, g_q166_70ed88, $L_70ed88)
ASM_VAR(int, g_q166_70f3d0, $L_70f3d0)
// [FUN_5cf070] fills the 100 random particle directions (70ef1c, 70f0ac, 70f23c: -1..1), colours (70ebfc) and
// sizes (70ed8c) once (70f3d0)
extern "C" void __cdecl Particles_InitRandom(void)
{
    for (int i = 0; i < 100; i++) {
        ((float*)g_q166_70ef1c)[i] = (float)((rand() % 100) * g_q166_64b958 - g_q166_64b670);
        ((float*)g_q166_70f0ac)[i] = (float)((rand() % 100) * g_q166_64b958 - g_q166_64b670);
        ((float*)g_q166_70f23c)[i] = (float)((rand() % 100) * g_q166_64b958 - g_q166_64b670);
        ((DWORD*)g_q166_70ebfc)[i] = ((rand() % 2) ? 4290773008u : 0u) + 4290822368u;
        ((DWORD*)g_q166_70ed88)[i + 1] = ((rand() % 2) ? 14160u : 0u) + 4294953070u;
    }
    g_q166_70f3d0 = 1;
}

extern "C" void FUN_5ab710(); KING_ALIAS("_FUN_5ab710", "@Vec3_AddAssign@16")
ASM_VAR(double, g_q167_64fa38, $L_64fa38)
ASM_VAR(double, g_q167_67fa60, $L_67fa60)
// [$L_5ceba0] moves particle cloud s by dt: every particle (+56, +36 of them) and the cloud's position (+24) by
// the velocity (+160..+168), the vertical speed slowed by 64fa38, the colours faded by 67fa60, the life (+16)
// shortened
extern "C" void __fastcall Particles_Move(BYTE* s, void*, double dt)
{
    for (int i = 0; i < F(int, s, 36); i++) {
        float* p = (float*)(F(BYTE*, s, 56) + i * 12);
        p[0] = (float)(F(float, s, 160) * dt + p[0]);
        p[1] = (float)(F(float, s, 164) * dt + p[1]);
        p[2] = (float)(F(float, s, 168) * dt + p[2]);
    }
    float v[3] = {(float)(dt * F(float, s, 160)), (float)(dt * F(float, s, 164)), (float)(dt * F(float, s, 168))};
    float out[3];
    CallThis(FUN_5ab710, s + 24, out, v);
    F(float, s, 168) = (float)(F(float, s, 168) - dt * g_q167_64fa38);
    Mesh_ScaleColors(s, 0, F(int, s, 176), F(const DWORD*, s, 172), g_q167_67fa60);
    F(double, s, 16) = F(double, s, 16) - dt;
}

// ---------------------------------------------------------------------------
// Sound effects of objects.

ASM_VAR(void*, g_q168_70ebe8, $L_70ebe8)
ASM_VAR(int, g_q168_70ebf4, $L_70ebf4)
ASM_VAR(BYTE*, g_q168_707f94, $L_707f94)
ASM_VAR(BYTE*, g_q168_707f98, $L_707f98)
ASM_VAR(float, g_q168_64c258, $L_64c258)
ASM_VAR(float, g_q168_64c330, $L_64c330)
ASM_VAR(float, g_q168_651cec, $L_651cec)
ASM_VAR(float, g_q168_64c25c, $L_64c25c)
ASM_VAR(float, g_q168_65170c, $L_65170c)
ASM_VAR(float, g_q168_64c250, $L_64c250)
ASM_VAR(AsmLabel, s_q168_67f994, $L_67f994)
// [FUN_5c87f0] plays sound kind k (0-6: one of the two sound objects 707f94/707f98 and its volume scale) at
// object a's position (+20, 4 floats) through the 70ebe8 hook; logs and gives 0 for an unknown kind or sound
extern "C" int __cdecl Sound_PlayAt(BYTE* a, unsigned k)
{
    if (!g_q168_70ebe8 || !g_q168_70ebf4)
        return 0;
    float scale = 0;
    BYTE* o = 0;
    bool ok = true;
    switch (k) {
    case 0: scale = g_q168_64c258; o = g_q168_707f98; break;
    case 1: scale = g_q168_64c330; o = g_q168_707f94; break;
    case 2: scale = g_q168_651cec; o = g_q168_707f94; break;
    case 3: scale = g_q168_64c25c; o = g_q168_707f94; break;
    case 4: scale = g_q168_64c25c; o = g_q168_707f98; break;
    case 5: scale = g_q168_65170c; o = g_q168_707f94; break;
    case 6: scale = g_q168_64c250; o = g_q168_707f94; break;
    default: ok = false;
    }
    if (ok && o) {
        F(DWORD, o, 32) |= 0x40200;
        float v[4];
        memcpy(v, a + 20, 16);
        return ((int(__cdecl*)(BYTE*, BYTE*, float*, float))g_q168_70ebe8)(a, o, v, v[3] * scale);
    }
    char buf[256];
    sprintf(buf, (const char*)s_q168_67f994);
    CallC(FUN_5d2ba0, buf, 0);
    return 0;
}

ASM_VAR(float, g_q188_64c348, $L_64c348)
ASM_VAR(float, g_q188_651c94, $L_651c94)
ASM_VAR(float, g_q188_64d494, $L_64d494)
ASM_VAR(float, g_q188_64c264, $L_64c264)
ASM_VAR(float, g_q188_651c90, $L_651c90)
ASM_VAR(float, g_q188_651c8c, $L_651c8c)
ASM_VAR(float, g_q188_64c350, $L_64c350)
ASM_VAR(float, g_q188_64c258, $L_64c258)
ASM_VAR(BYTE*, g_q188_7025b8, $L_7025b8)
ASM_VAR(float, g_q188_651c88, $L_651c88)
ASM_VAR(float, g_q188_651c84, $L_651c84)
// [FUN_5c8010] integrates the swing x, v, w of a hanging object over dt in steps of at most 64c348 (w limited by
// lim scaled by 64d494, damped harder past it), stiffness k and mass m (with the 7025b8 world's gravity); m + dt
extern "C" double __cdecl Swing_Step(float* x, float* v, float* w, float k, float m, float a6, float dt)
{
    int n = 1;
    double step = dt;
    if (dt > g_q188_64c348) {
        n = (int)(dt * g_q188_651c94);
        step = g_q188_64c348;
        if (n <= 0)
            return a6 + dt;
    }
    float lim = (float)(k * g_q188_64d494);
    for (; n; n--) {
        double y = step * *v + *w;
        double c;
        if (!(lim >= x87_fabs(y))) {
            *w = y > g_q188_64c264 ? lim : (float)(k * g_q188_651c90);
            c = g_q188_64c350;
        } else {
            *w = (float)y;
            c = g_q188_64c258;
        }
        double z = step * *x + *v;
        *v = (float)z;
        *x = (float)(k * *w * g_q188_651c8c - z * c);
        double f = g_q188_7025b8 ? k * F(float, g_q188_7025b8, 532) * k * m * g_q188_651c88
                                 : k * k * m * g_q188_651c84;
        *x = (float)(f + *x);
    }
    return a6 + dt;
}

ASM_VAR(float, g_q189_651c64, $L_651c64)
ASM_VAR(float, g_q189_651c9c, $L_651c9c)
ASM_VAR(float, g_q189_651c98, $L_651c98)
// [FUN_5c8130] the four swing values from a and b, rotated over p0..p3 by the quarter (t scaled by 651c64) they
// fall into
extern "C" void __cdecl Swing_Quarters(float t, float a, float b, float* p0, float* p1, float* p2, float* p3)
{
    float c = g_q189_651c64;
    int i = (int)(c * t) % 4;
    if ((unsigned)i > 3)
        return;
    float* p[4] = {p0, p1, p2, p3};
    *p[(4 - i) & 3] = (float)(c * a * g_q188_64d494);
    *p[(5 - i) & 3] = (float)(c * b * g_q189_651c9c);
    *p[(6 - i) & 3] = (float)(c * a * g_q189_651c98);
    *p[(7 - i) & 3] = (float)(c * b * g_q188_651c90);
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q198_Dtor5c6060_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor5c6060_DeletingDestructor@12", "@Dtor5c6060_DeletingDestructor@12")
extern "C" int __fastcall q198_Dtor5cf2f0_DeletingDestructor(BYTE* s, void* d, int a0); KING_ALIAS("@q198_Dtor5cf2f0_DeletingDestructor@12", "@Dtor5cf2f0_DeletingDestructor@12")
extern "C" int __fastcall q198_MappedFile_Close(BYTE* s); KING_ALIAS("@q198_MappedFile_Close@4", "@MappedFile_Close@4")
// [$L_5c63b0] forwards to Dtor5c6060_DeletingDestructor for the object 256 bytes before
extern "C" int __fastcall Thunk5c63b0(BYTE* s, void* d, int a0) { return q198_Dtor5c6060_DeletingDestructor(s - 256, d, a0); }
// [$L_5cf470] forwards to Dtor5cf2f0_DeletingDestructor for the object 32 bytes before
extern "C" int __fastcall Thunk5cf470(BYTE* s, void* d, int a0) { return q198_Dtor5cf2f0_DeletingDestructor(s - 32, d, a0); }

extern "C" __declspec(dllimport) void __stdcall SysFreeString(wchar_t*);
// [$L_5c19e0] frees the BSTR *s
extern "C" void __fastcall Bstr_Free(wchar_t** s) { SysFreeString(*s); }
// [$L_5c6ab0] nothing to do
extern "C" void __fastcall Med5c6ab0_Nop(BYTE*) {}
// [$L_5c77f0] nothing to do
extern "C" void __fastcall Med5c77f0_Nop(BYTE*) {}

ASM_VAR(char, g_q200_651948, $L_651948)
// [$L_5c2f40] an empty 32-byte media record (vtable 651948, no ids)
extern "C" BYTE* __fastcall MedRec_Init(BYTE* s)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = &g_q200_651948;
    F(int, s, 12) = -1;
    F(int, s, 16) = -1;
    F(int, s, 20) = 0;
    return s;
}
// [$L_5d2080] an empty mapped file: no handles
extern "C" BYTE* __fastcall MappedFile_Init(BYTE* s)
{
    F(int, s, 0) = -1;
    F(int, s, 4) = -1;
    F(int, s, 8) = 0;
    F(int, s, 12) = 0;
    F(int, s, 16) = 0;
    F(BYTE, s, 20) = 0;
    return s;
}

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q238_Fw_SetupRoom(BYTE* s); KING_ALIAS("@q238_Fw_SetupRoom@4", "@Fw_SetupRoom@4")
extern "C" int __fastcall q238_MappedFile_Close(BYTE* s); KING_ALIAS("@q238_MappedFile_Close@4", "@MappedFile_Close@4")
// [FUN_5c5370] forwards to Fw_SetupRoom
extern "C" int __fastcall Thunk5c5370(BYTE* s) { return q238_Fw_SetupRoom(s); }

// ---------------------------------------------------------------------------
// Thunks.

extern "C" int __fastcall q240_MappedFile_Close(BYTE* s); KING_ALIAS("@q240_MappedFile_Close@4", "@MappedFile_Close@4")
extern "C" int __fastcall q240_Med5ce5b0_Destroy(BYTE* s); KING_ALIAS("@q240_Med5ce5b0_Destroy@4", "@Med5ce5b0_Destroy@4")
// [FUN_5cf170] forwards to Med5ce5b0_Destroy
extern "C" int __fastcall Thunk5cf170(BYTE* s) { return q240_Med5ce5b0_Destroy(s); }

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q241_L_5d2070(); KING_ALIAS("_q241_L_5d2070", "$L_5d2070")
// [$L_5d2060] registers 5d2070 at exit
extern "C" int __cdecl Static5d2060_AtExit(void) { return atexit((void(__cdecl*)(void))q241_L_5d2070); }

extern "C" void FUN_5abc40(); KING_ALIAS("_FUN_5abc40", "_Matrix_Translation")
extern "C" void FUN_5cc680(); KING_ALIAS("_FUN_5cc680", "@Media_Call5cc680@20")
ASM_VAR(float, g_q243_702630, $L_702630)
ASM_VAR(float, g_q243_702634, $L_702634)
ASM_VAR(float, g_q243_702638, $L_702638)
ASM_VAR(float, g_q243_64c748, $L_64c748)
ASM_VAR(float, g_q243_64c330, $L_64c330)
struct Med243_V3 { float x, y, z; };
// [FUN_5cca60] starts particle source s for p at (x, y, z) moved by half of 702630: the translation matrix of that
// point, p's rate (+32) from t over the source's count (+32), the source's length (+12) from t
extern "C" void __fastcall Particles_StartAt(BYTE* s, void*, BYTE* p, float x, float y, float z, int, int, float t)
{
    double hx = g_q243_702630 * g_q168_64c258;
    float hy = (float)(g_q243_702634 * g_q168_64c258);
    float hz = (float)(g_q243_702638 * g_q168_64c258);
    Med243_V3 v = {(float)(x + hx), (float)(hy + y), (float)(hz + z)};
    BYTE m[48];
    CallC(FUN_5abc40, m, v);
    F(float, p, 32) = (float)(t * g_q243_64c748 / (F(int, s, 32) + 1));
    F(float, s, 12) = (float)(t * g_q243_64c330);
    CallThis(FUN_5cc680, s, 0, m, p);
}

ASM_PROC(FUN_5ce6e0)
ASM_VAR(double, g_q244_651e10, $L_651e10)
ASM_VAR(AsmLabel, g_q244_70ebfc, $L_70ebfc)
ASM_VAR(AsmLabel, g_q244_70ef1c, $L_70ef1c)
ASM_VAR(AsmLabel, g_q244_70f0ac, $L_70f0ac)
ASM_VAR(AsmLabel, g_q244_70f23c, $L_70f23c)
ASM_VAR(char, g_q244_651dec, $L_651dec)
// [FUN_5ce9c0] constructs a particle stream s of n (at most 100) particles from (x, y, z) moved a random 0..9
// steps of 651e10 along d, with the shared tables 70ebfc.. and spread 0.1; its direction kept at +160 (vtable 651dec)
extern "C" BYTE* __fastcall Stream_Construct(BYTE* s, void*, int n, float x, float y, float z, float dx, float dy,
                                             float dz, int a, int b)
{
    if (n > 100)
        n = 100;
    double k = rand() % 10 * g_q244_651e10;
    float ox = (float)(dx * k), oy = (float)(dy * k);
    Med243_V3 v = {(float)(x + ox), (float)(oy + y), (float)(dz * k + z)};
    CallThis(FUN_5ce6e0, s, a, b, v, n, (void*)g_q244_70ebfc, (void*)g_q244_70ef1c, (void*)g_q244_70f0ac,
             (void*)g_q244_70f23c, 0.1);
    F(int, s, 160) = 0;
    F(int, s, 164) = 0;
    F(int, s, 168) = 0;
    F(float, s, 160) = dx;
    F(float, s, 164) = dy;
    F(int, s, 172) = 0;
    F(int, s, 176) = 0;
    F(void*, s, 0) = &g_q244_651dec;
    F(float, s, 168) = dz;
    return s;
}

// === drawing a video frame (FUN_5c1d40)

ASM_VAR(AsmLabel, s_vf67f42c, $L_67f42c)
extern "C" void FUN_5d3ab0(); KING_ALIAS("_FUN_5d3ab0", "_Mode_TextureLevel")    // the texture level: 2 for 565, 3 for 555 surfaces

static WORD Vf_565(DWORD v)
{
    return (WORD)((((v >> 10) & 63) | (v >> 19) << 6) << 5 | (v >> 3) & 31);
}

static WORD Vf_555(DWORD v)
{
    return (WORD)((((v >> 19) & 31) << 5 | (v >> 11) & 31) << 5 | (v >> 3) & 31);
}

extern "C" {

// [FUN_5c1d40] draws the decoded frame (+2204, +1112 x +1116 pixels of +1122 bits, bottom-up) on the DirectDraw
// surface at (x, y) in w x h, clipped; scaled when +2240 bit 0 asks for it and the sizes differ; 16-bit
// surfaces get 565 or 555 pixels, 8-bit frames are copied
void __fastcall Video_DrawFrame(BYTE* self, void*, BYTE* surf, int x, int y, int w, int h)
{
    DWORD desc[31];
    memset(desc, 0, sizeof desc);
    desc[0] = 124;
    desc[1] = 8387054;
    typedef HRESULT(__stdcall * LockFn)(BYTE*, void*, void*, DWORD, void*);
    typedef HRESULT(__stdcall * UnlockFn)(BYTE*, void*);
    if ((*(LockFn*)(*(BYTE**)surf + 100))(surf, 0, desc, 33, 0)) {
        CallC(FUN_5d2ba0, (const char*)s_vf67f42c, 0);
        return;
    }
    int width = desc[3], height = desc[2], pitch = desc[4];
    BYTE* bits = (BYTE*)desc[9];
    unsigned srcW = F(DWORD, self, 1112), srcH = F(DWORD, self, 1116);
    if (x >= width || y >= height || x + w == 0 || y + h == 0)
        return;
    if (x < 0) {
        w = x + w;
        x = 0;
    }
    if (y < 0) {
        h = y + h;
        y = 0;
    }
    if ((unsigned)(x + w) > (unsigned)width)
        w = width - x;
    if ((unsigned)(y + h) > (unsigned)height)
        h = height - y;
    unsigned bpp = F(WORD, self, 1122) >> 3;
    BYTE* src = F(BYTE*, self, 2204);
    BYTE* dst = bits + pitch * y + x * 2;
    if ((F(BYTE, self, 2240) & 1) && (srcW != (unsigned)w || srcH != (unsigned)h)) {
        int mode = CallC<int>(FUN_5d3ab0);
        if (mode == 2 || (mode = CallC<int>(FUN_5d3ab0)) == 3) {
            unsigned acc = 0;
            for (unsigned r = (unsigned)h; r; r--, acc += srcH, dst += pitch) {
                unsigned row = (srcH - acc / (unsigned)h - 1) * srcW;
                WORD* d = (WORD*)dst;
                unsigned col = 0;
                for (unsigned c = (unsigned)w; c; c--, col += srcW) {
                    DWORD v = F(DWORD, src, (col / (unsigned)w + row) * bpp);
                    *d++ = mode == 2 ? Vf_565(v) : Vf_555(v);
                }
            }
        }
    } else {
        int mode = CallC<int>(FUN_5d3ab0);
        if (mode == 2 || (mode = CallC<int>(FUN_5d3ab0)) == 3) {
            if (h) {
                int step = -(int)(bpp * srcW);
                int start = (srcH - 1) * bpp * srcW;
                for (unsigned r = (unsigned)h; r; r--, start += step, dst += pitch) {
                    WORD* d = (WORD*)dst;
                    int o = start;
                    for (unsigned c = (unsigned)w; c; c--, o += bpp) {
                        DWORD v = F(DWORD, src, o);
                        *d++ = mode == 2 ? Vf_565(v) : Vf_555(v);
                    }
                }
            }
        } else if (bpp == 1 && h) {
            int o = (srcH - 1) * srcW;
            for (unsigned r = (unsigned)h; r; r--, o -= srcW, dst += pitch)
                memcpy(dst, src + o, w);
        }
    }
    (*(UnlockFn*)(*(BYTE**)surf + 128))(surf, 0);
}

} // extern "C"

// === opening a video file (FUN_5c13a0)

ASM_VAR(AsmLabel, g_avClsid651938, $L_651938)
ASM_VAR(AsmLabel, g_avIid67f408, $L_67f408)
ASM_VAR(AsmLabel, s_av67f3f8, $L_67f3f8)
ASM_VAR(AsmLabel, s_av67f3e4, $L_67f3e4)
ASM_VAR(int, g_av7208b8, $L_7208b8)
ASM_VAR(BYTE*, g_av720920, $L_720920)
ASM_VAR(int, g_av702f54, $L_702f54)

extern "C" void FUN_5c12a0(); KING_ALIAS("_FUN_5c12a0", "@Media_Close@8")
extern "C" void FUN_5c1990(); KING_ALIAS("_FUN_5c1990", "_AnsiToBstr")    // a BSTR of an ANSI string (text, length)
ASM_PROC(FUN_6303be)    // ICOpen
ASM_PROC(FUN_6303b8)    // ICSendMessage
extern "C" void FUN_5c2e60(); KING_ALIAS("_FUN_5c2e60", "_Media_StartThread")
extern "C" void FUN_5c2750(); KING_ALIAS("_FUN_5c2750", "_Msavi_Open")

// a COM method (stdcall, this pushed) at vtbl+off of o
template <class R = HRESULT, class... A>
static R Av_Com(void* o, int off, A... args)
{
    return ((R(__stdcall*)(void*, A...))*(void**)(*(BYTE**)o + off))(o, args...);
}

// the index records of a stream: count records of 16 bytes, zeroed at +0 and +4
static BYTE* Av_Records(int count)
{
    BYTE* r = (BYTE*)operator new(count * 16);
    if (r)
        for (int i = 0; i < count; i++) {
            F(int, r, i * 16 + 4) = 0;
            F(int, r, i * 16) = 0;
        }
    return r;
}

extern "C" {

// [FUN_5c13a0] opens the video file path (thiscall): its reader object, the video stream format (+44), the
// decompressor (+2176) to bits per pixel bits, the frame index (+0), the frame buffers (+2200, +2204), the rate
// (+2224), the audio stream (+16, index +4) and the playing thread; 0 when done, else an error code or 1
int __fastcall Video_Open(BYTE* self, void*, const char* path, int bits, int scale, int thread)
{
    CallThis(FUN_5c12a0, self);
    F(int, self, 2240) = scale;
    BYTE* obj = 0;
    HRESULT hr = CoCreateInstance(*(const IID*)(char*)g_avClsid651938, 0, 7, *(const IID*)(char*)g_avIid67f408, (void**)&obj);
    if (hr) {
        if (obj)
            Av_Com<ULONG>(obj, 8);
        return hr;
    }
    HANDLE h = CreateFileA(path, 0x80000000, 1, 0, 3, 0, 0);
    F(HANDLE, self, 36) = h;
    if (!h) {
        if (obj)
            Av_Com<ULONG>(obj, 8);
        return 1;
    }
    void* name = CallC<void*>(FUN_5c1990, path, -1);
    hr = Av_Com(obj, 28, name);
    if (hr) {
        SysFreeString((BSTR)name);
        if (obj)
            Av_Com<ULONG>(obj, 8);
        return hr;
    }
    int out = 0;
    Av_Com(obj, 36, &out);
    if (out != 1)
        goto fail;
    {
        Av_Com(obj, 52, 0, self + 8);
        DWORD fileInfo[14];
        memset(fileInfo, 0, sizeof fileInfo);
        Av_Com(obj, 84, fileInfo, 56, &out);
        DWORD info[16];
        memset(info, 0, sizeof info);
        Av_Com(obj, 44, 0, info, 64, &out, &out, &out);
        if (info[2] & 0x10000)
            CallC(FUN_5d2ba0, (const char*)s_av67f3f8, 0);
        memset(self + 1108, 0, 40);
        Av_Com(obj, 68, 0, self + 44, 40, &out);
        memcpy(self + 1108, self + 44, 40);
        void* ic = CallStd<void*>(FUN_6303be, 0x63646976, F(DWORD, self, 60), 2);    // 'vidc'
        F(void*, self, 2176) = ic;
        if (!ic) {
            SysFreeString((BSTR)name);
            if (obj)
                Av_Com<ULONG>(obj, 8);
            return 1;
        }
        F(WORD, self, 1122) = (WORD)bits;
        F(int, self, 1124) = 0;
        F(BYTE*, self, 2172) = self + 1108 + F(int, self, 1108);
        if (CallStd<int>(FUN_6303b8, ic, 16395, self + 44, self + 1108)) {
            CallC(FUN_5d2ba0, (const char*)s_av67f3e4, 0);
            goto fail;
        }
        if (CallStd<int>(FUN_6303b8, F(void*, self, 2176), 16396, self + 44, self + 1108))
            goto fail;
        int n = F(int, self, 8);
        F(BYTE*, self, 0) = Av_Records(n);
        for (int i = 0; i < F(int, self, 8); i++) {
            int a, b, c;
            int key = 0;
            HRESULT r = Av_Com(obj, 88, 0, i, &c, &a, &b, &key);
            BYTE* rec = F(BYTE*, self, 0) + i * 16;
            F(int, rec, 0) = a;
            F(int, rec, 4) = b;
            F(int, rec, 8) = c;
            F(BYTE, rec, 12) = (BYTE)key;
            if (r)
                break;
        }
        F(int, self, 2180) = 0;
        F(void*, self, 2200) = operator new(F(int, self, 52) * F(int, self, 48) * 4);
        F(void*, self, 2204) = operator new(F(int, self, 1116) * F(int, self, 1112) * 4);
        double rate = (double)(__int64)(DWORD)info[6] / (int)info[5];
        if (bits == 8)
            g_av7208b8 = 1;
        F(int, self, 2208) = 1;
        F(double, self, 2224) = rate;
        int streams;
        Av_Com(obj, 40, &streams);
        if (streams <= 0) {
            SysFreeString((BSTR)name);
            if (obj)
                Av_Com<ULONG>(obj, 8);
            return 0;
        }
        CallC(FUN_5c2e60, g_av720920, thread);
        F(HANDLE, self, 40) = CreateFileA(path, 0x80000000, 1, 0, 3, 0, 0);
        DWORD info2[16];
        Av_Com(obj, 48, 0, info2, 64, &out, &out);
        Av_Com(obj, 56, 0, self + 12);
        Av_Com(obj, 72, 0, self + 16, 18, &out);
        g_av702f54 = 0;
        CallC(FUN_5c2750, self + 16, (void*)Avi_AudioRead, self);
        F(double, self, 2232) = (double)(__int64)(DWORD)(F(WORD, self, 30) * F(int, self, 20));
        int n2 = F(int, self, 12);
        F(BYTE*, self, 4) = Av_Records(n2);
        for (int i = 0; i < F(int, self, 12); i++) {
            int a, b, c;
            HRESULT r = Av_Com(obj, 64, 0, i, &c, &a, &b);
            BYTE* rec = F(BYTE*, self, 4) + i * 16;
            F(int, rec, 0) = a;
            F(int, rec, 4) = b;
            F(int, rec, 8) = c;
            if (r)
                break;
        }
        F(int, self, 2192) = 0;
        F(int, self, 2196) = 0;
        F(int, self, 2188) = 0;
        F(int, self, 2184) = 0;
        SysFreeString((BSTR)name);
        if (obj)
            Av_Com<ULONG>(obj, 8);
        return 0;
    }
fail:
    SysFreeString((BSTR)name);
    if (obj)
        Av_Com<ULONG>(obj, 8);
    return 1;
}

} // extern "C"

// === waiting for the network music (FUN_5c3c50)

extern "C" void FUN_602bc0(); KING_ALIAS("_FUN_602bc0", "@Stream_Construct@8")
extern "C" void FUN_602bd0(); KING_ALIAS("_FUN_602bd0", "_Gate_602bd0")
extern "C" void FUN_600b60(); KING_ALIAS("_FUN_600b60", "_Tsc_Now")
extern "C" void FUN_6016f0(); KING_ALIAS("_FUN_6016f0", "_Providers_Rebuild")
extern "C" void FUN_601c10(); KING_ALIAS("_FUN_601c10", "_Providers_Call601c70")
extern "C" void FUN_603170(); KING_ALIAS("_FUN_603170", "@DpMessage_Set@16")
extern "C" void FUN_6032f0(); KING_ALIAS("_FUN_6032f0", "@Gate_Set24@8")
extern "C" void FUN_6034e0(); KING_ALIAS("_FUN_6034e0", "@Stream_ReadBlob@20")
extern "C" void FUN_601060(); KING_ALIAS("_FUN_601060", "_Net_Receive")    // the next received message (stream); 0 when there is one
ASM_PROC(FUN_6031e0)    // a message's next record type (out); 0 when there is one
ASM_PROC(FUN_603300)    // skips to the next record; 0 when there is one
ASM_VAR(double, g_nm707f88, $L_707f88)
ASM_VAR(double, g_nm64b5f0, $L_64b5f0)
ASM_VAR(double, g_nm64b730, $L_64b730)
ASM_VAR(AsmLabel, g_nm67f880, $L_67f880)

extern "C" {

// [FUN_5c3c50] music m waiting (+416) for the network's track (+420 not yet set): opens the session (+428; one
// try per 64b5f0 seconds), asks for track name (message 902, every 64b730 seconds) and reads the replies: a
// 901 record gives the track (+432, +436; 255 stops the music), its name into out and the time (+412)
void __fastcall Music_WaitNetwork(BYTE* m, void*, const char* name, char* out)
{
    BYTE s[40];
    CallThis(FUN_602bc0, s);
    F(int, s, 12) = 0;
    F(double, s, 16) = CallC<double>(FUN_600b60);
    if (F(int, m, 416) && !F(int, m, 420)) {
        if (!F(int, m, 428)) {
            double t = CallC<double>(FUN_6037e0);
            double lim = g_nm707f88 + g_nm64b5f0;
            if (CallC<double>(FUN_6037e0) > lim) {
                g_nm707f88 = t;
                CallC(FUN_6016f0);
                if (CallC<int>(FUN_601c10, F(int, m, 424), 0, 8, g_nm67f880))
                    goto done;
                F(int, m, 428) = 1;
            }
        }
        if (F(int, m, 428)) {
            double t = CallC<double>(FUN_6037e0);
            double lim = g_nm707f88 + g_nm64b730;
            if (CallC<double>(FUN_6037e0) > lim) {
                BYTE q[32];
                CallThis(FUN_602be0, q);
                CallThis(FUN_603110, q, 902, name, (int)strlen(name) + 1);
                CallC(FUN_6021b0, F(BYTE*, q, 4), F(BYTE*, q, 8) - F(BYTE*, q, 4));
                g_nm707f88 = t;
                CallThis(FUN_602e80, q);
            }
            BYTE r[32];
            CallThis(FUN_602be0, r);
            for (int more = CallC<int>(FUN_601060, s); !more; more = CallC<int>(FUN_601060, s)) {
                if (F(int, s, 0) != 4)
                    continue;
                CallThis(FUN_603170, r, F(BYTE*, s, 4), F(int, s, 8));
                if (CallThis<int>(FUN_6032f0, r))
                    continue;
                DWORD type, id, size;
                BYTE blob[256];
                bool found = false;
                while (!CallThis<int>(FUN_6031e0, r, &type)) {
                    if (type == 120 && !CallThis<int>(FUN_6034e0, r, &id, blob, &size) && id == 901) {
                        found = true;
                        break;
                    }
                    if (CallThis<int>(FUN_603300, r))
                        break;
                }
                if (!found)
                    continue;
                if (blob[1] == 255)
                    Music_Stop(m, 0);
                else {
                    F(int, m, 436) = blob[1];
                    F(int, m, 432) = blob[0];
                    F(int, m, 420) = 1;
                    F(int, m, 412) = F(int, s, 12);
                    strcpy(out, (const char*)blob + 2);
                }
                break;
            }
            CallThis(FUN_602e80, r);
        }
    }
done:
    CallThis(FUN_602bd0, s);
}

} // extern "C"

// === network object messages (FUN_5c5910)

extern "C" void FUN_5c3160(); KING_ALIAS("_FUN_5c3160", "@Table_Find@12")

struct Om_Matrix { DWORD d[12]; };      // a matrix passed by value
typedef void (__fastcall* Om_Create)(BYTE* self, void*, char* name, char* a, char* b, DWORD x, float f, DWORD y,
                                     Om_Matrix m, DWORD id);

// the object (+20) of entry key in s's table (+7976) when it is of kind 2 (+16); 0 when there is no such entry,
// else 1 with *o set
static int Om_Find(BYTE* s, DWORD key, BYTE** o)
{
    BYTE* e = CallThis<BYTE*>(FUN_5c3160, s + 7976, key);
    if (!e || F(int, e, 16) != 2)
        return 0;
    *o = F(BYTE*, e, 20);
    return 1;
}

extern "C" {

// [FUN_5c5910] network message id for media s: with the object table (+7932) messages 900 and 903 mark a known
// object (+160 its key) as shown (+296 bit 0) and 904 shows or hides one (+4); otherwise 900 / 903 create the
// object (names at +0, +24, +48, +140, +130, +150, the matrix +72, the key +160) by the vtable entries 224 / 228
int __fastcall Media_ObjectMessage(BYTE* s, void*, int id, BYTE* msg, int)
{
    if (F(int, s, 7932)) {
        BYTE* o;
        if (id == 900 || id == 903) {
            if (Om_Find(s, F(DWORD, msg, 160), &o)) {
                if (o)
                    F(BYTE, o, 296) |= 1;
                return 1;
            }
        } else if (id == 904) {
            if (!Om_Find(s, F(DWORD, msg, 0), &o))
                return 0;
            if (o) {
                if (F(BYTE, msg, 4))
                    F(BYTE, o, 296) &= ~1;
                else
                    F(BYTE, o, 296) |= 1;
            }
            return 1;
        }
    }
    if (id != 900 && id != 903)
        return 0;
    char a[32], b[32], c[32];
    strcpy(a, (const char*)msg);
    strcpy(b, (const char*)msg + 24);
    strcpy(c, (const char*)msg + 48);
    Om_Matrix m;
    memcpy(&m, msg + 72, 48);
    Om_Create fn = (*(Om_Create**)s)[id == 900 ? 56 : 57];
    fn(s, 0, a, b, c, F(DWORD, msg, 140), F(float, msg, 130), F(DWORD, msg, 150), m, F(DWORD, msg, 160));
    return 1;
}

} // extern "C"

// === stepping through the records of a recording (FUN_5c49b0)

static int Rs_Int(FILE* f) { return CallC<int>(FUN_5ab140, f); }

// the time of a record's header: a packed word (kind 4: the low 20 bits to +7940, the rest in seconds) or the
// next two ints (+7940, the time)
static int Rs_Time(BYTE* s, FILE* f, int kind)
{
    if (kind == 4) {
        int v = Rs_Int(f);
        F(int, s, 7940) = v & 0xFFFFF;
        return (v >> 20) * 1000;
    }
    F(int, s, 7940) = Rs_Int(f);
    return Rs_Int(f);
}

extern "C" {

// [FUN_5c49b0] moves recording s (file +7928) to the next (forward) or previous record, running the events
// (-1 blocks) on the way; the record's time, or -1 at the end (-256 forward, -4096 backward) with the position
// kept
int __fastcall Rec_Step(BYTE* s, void*, int forward)
{
    FILE* f = F(FILE*, s, 7928);
    long pos = ftell(f);
    if (forward) {
        for (int t = Rs_Int(f); t != -16; t = Rs_Int(f)) {
            if (t == -256) {
                fseek(f, pos, 0);
                return -1;
            }
            if (t == -1)
                CallThis(FUN_5c4d30, s, 0, 0);
            else {
                int n = Rs_Int(f) + 4;
                fseek(f, ftell(f) + n, 0);
            }
        }
        int kind = Rs_Int(f);
        int r = Rs_Time(s, f, kind);
        Rs_Int(f);
        return r;
    }
    fseek(f, ftell(f) - 4, 0);
    int r;
    long p;
    if (Rs_Int(f) == 4) {
        fseek(f, ftell(f) - 8, 0);
        r = Rs_Time(s, f, 4);
        p = ftell(f) - 16;
    } else {
        fseek(f, ftell(f) - 12, 0);
        r = Rs_Time(s, f, 0);
        p = ftell(f) - 20;
    }
    fseek(f, p, 0);
    int back = Rs_Int(f) + 12;
    fseek(f, ftell(f) - back, 0);
    for (int t = Rs_Int(f); t != -16; t = Rs_Int(f)) {
        if (t == -4096) {
            fseek(f, ftell(f) + back - 4, 0);
            return -1;
        }
        if (t == -1) {
            long q = ftell(f);
            CallThis(FUN_5c4d30, s, 0, 0);
            fseek(f, q - 8, 0);
        } else
            fseek(f, ftell(f) - 8, 0);
        back = Rs_Int(f) + 12;
        fseek(f, ftell(f) - back, 0);
    }
    fseek(f, ftell(f) + back - 4, 0);
    return r;
}

} // extern "C"

// === growing a tree's branches (FUN_5cc6b0)

extern "C" void FUN_5abfd0(); KING_ALIAS("_FUN_5abfd0", "_Matrix_RotationY")
extern "C" void FUN_5ac0c0(); KING_ALIAS("_FUN_5ac0c0", "_Matrix_RotationZ")
extern "C" void FUN_5aba40(); KING_ALIAS("_FUN_5aba40", "_Matrix_Multiply")
ASM_VAR(float, g_tb64c900, $L_64c900)
ASM_VAR(float, g_tb651d24, $L_651d24)
ASM_VAR(float, g_tb651d20, $L_651d20)

// the matrix of a child branch: moved along z by len, turned about y by a * ky and about z by b, then m
static BYTE* Tb_Child(BYTE* out, const BYTE* m, float len, float a, float ky, float b)
{
    Med243_V3 v = {0, 0, len};
    BYTE t[48], ry[48], rz[48], m1[48], m2[48];
    CallC(FUN_5abc40, t, v);
    CallC(FUN_5abfd0, ry, (float)(a * ky));
    CallC(FUN_5ac0c0, rz, b);
    CallC(FUN_5aba40, m1, rz, ry);
    CallC(FUN_5aba40, m2, m1, t);
    return CallC<BYTE*>(FUN_5aba40, out, m2, m);
}

extern "C" {

// [FUN_5cc6b0] grows tree s from branch b at depth with matrix m: below the depth limit (+32) its two children
// (b's vtable 0 and 4 by the length b->value * +28) are turned apart by the sway (+40, +16) and the spread (+24,
// +20); at the limit, or without a branch, a leaf
void __fastcall Tree_Grow(BYTE* s, void*, int depth, const BYTE* m, BYTE* b)
{
    if (depth >= F(int, s, 32) || !b) {
        Leaves_Add(s, 0, (const DWORD*)m, b && depth >= F(int, s, 32) ? F(DWORD, b, 36) : 0);
        return;
    }
    float len = (float)(Branch_Value(b, 0) * F(float, s, 28));
    float a = (float)(F(float, s, 24) * g_tb64c900 + Branch_Sway(b, 0, F(float, s, 40)) * F(float, s, 16));
    float z = (float)(F(float, s, 20) * g_tb651d24);
    BYTE c[48];
    Tb_Child(c, m, len, a, g_tb651d24, z);
    Tree_Grow(s, 0, depth + 1, c, CallVirt<BYTE*>(b, 0, len));
    float a2 = (float)((g_q168_64c250 - F(float, s, 24)) * g_tb64c900 +
                       (g_q168_64c250 - Branch_Sway(b, 0, F(float, s, 40))) * F(float, s, 16));
    float z2 = (float)(F(float, s, 20) * g_tb651d24);
    Tb_Child(c, m, len, a2, g_tb651d20, z2);
    Tree_Grow(s, 0, depth + 1, c, CallVirt<BYTE*>(b, 4, len));
}

} // extern "C"

// === creating a network object (FUN_5c5c30)

ASM_VAR(int, g_no67f878, $L_67f878)
ASM_VAR(AsmLabel, g_no5c5ea0, $L_5c5ea0)    // its message handler
extern "C" char* __cdecl strncpy(char* d, const char* s, size_t n);

extern "C" {

// [FUN_5c5c30] registers object s with media m under a key (+364: from the preset id +344 or a free slot) and its
// handler entries (+352, +356); records its creation message id (o's names, the third name from c for 900,
// else s+268, x, f, the matrix, s+344, the key) and keeps a 904 message showing it
void __fastcall NetObject_Create(BYTE* s, void*, BYTE* m, BYTE* o, BYTE* c, DWORD x, float f, int, Om_Matrix mt,
                                 int, int id)
{
    F(int, s, 372) = 0;
    F(int, s, 364) = 0;
    F(int, s, 368) = 0;
    F(int, s, 352) = 0;
    F(int, s, 356) = 0;
    F(int, s, 360) = 0;
    if (g_no67f878 != -1) {
        F(int, s, 344) = g_no67f878;
        g_no67f878 = -1;
    }
    if (!F(int, g_707f6c, 416) && !F(int, m, 7928))
        return;
    if (F(int, s, 344) == -1)
        F(int, s, 364) = (int)Media_TakeSlot(m, 0);
    else
        F(int, s, 364) = (F(int, s, 344) * 5 + 500000) * 2;
    int k = F(int, s, 364) + 1;
    F(int, s, 368) = k + 1;
    F(BYTE*, s, 352) = List_Add(m + 8300, 0, k, 1, F(int, s, 324));
    k = F(int, s, 368);
    F(int, s, 368) = k + 1;
    F(BYTE*, s, 356) = List_Add5(m + 8300, 0, k, (int)s, (int)(void*)g_no5c5ea0, 20);
    BYTE msg[32];
    CallThis(FUN_602be0, msg);
    BYTE d[256];
    memset(d, 0, sizeof d);     // the original sent the stack bytes between the fields
    strncpy((char*)d, F(const char*, F(BYTE*, o, 36), 60), 24);
    strncpy((char*)d + 24, F(const char*, o, 12), 24);
    strncpy((char*)d + 48, id == 900 ? F(const char*, c, 12) : F(const char*, s, 268), 24);
    memcpy(d + 72, &mt, 48);
    F(int, d, 150) = F(int, s, 344);
    F(DWORD, d, 140) = x;
    F(float, d, 130) = f;
    F(int, d, 160) = F(int, s, 364);
    CallThis(FUN_603110, msg, id, d, 250);
    Media_RecordBlock(m, 0, msg);
    BYTE* q = (BYTE*)operator new(28);
    if (q)
        q = CallThis<BYTE*>(FUN_602be0, q);
    BYTE e[8];
    F(int, e, 0) = F(int, s, 364);
    e[4] = 1;
    CallThis(FUN_603110, q, 904, e, 5);
    Pool_Keep(m, 0, q);
    CallThis(FUN_602e80, msg);
}

} // extern "C"

// === opening DirectSound for the movie player (FUN_5c24a0)

extern "C" void* g_impDirectSoundCreate; KING_ALIAS("_g_impDirectSoundCreate", "__imp__DirectSoundCreate@12")
ASM_VAR(DWORD, g_ds702f74, $L_702f74)
ASM_VAR(AsmLabel, s_ds67f534, $L_67f534)
ASM_VAR(AsmLabel, s_ds67f500, $L_67f500)
ASM_VAR(AsmLabel, s_ds67f4d8, $L_67f4d8)
ASM_VAR(AsmLabel, s_ds67f4c0, $L_67f4c0)
ASM_VAR(AsmLabel, s_ds67f468, $L_67f468)
ASM_VAR(AsmLabel, s_ds67f44c, $L_67f44c)

#pragma pack(push, 2)
struct Ms_WaveFormat { WORD wFormatTag, nChannels; DWORD nSamplesPerSec, nAvgBytesPerSec; WORD nBlockAlign, wBitsPerSample, cbSize; };
#pragma pack(pop)

extern "C" {

// [FUN_5c24a0] opens DirectSound (702f98) for window w with the primary buffer at 22050 Hz in the best format the
// card supports (16-bit stereo, 8-bit stereo or mono); 0 or the error, logged
int __cdecl Msavi_OpenSound(HWND w)
{
    memset((BYTE*)g_msavi702f78, 0, 24);
    BYTE* prim = 0;
    g_msaviBufSize = 0;
    g_ds702f74 = 0;
    g_msaviWritePos = 0;
    g_msaviPlayed = 0;
    g_msaviLastPos = 0;
    int hr = ((int(__stdcall*)(void*, BYTE**, void*))g_impDirectSoundCreate)(0, &g_msavi702f98, 0);
    if (hr < 0) {
        g_msavi702f98 = 0;
        CallC(FUN_5d2ba0, s_ds67f534, hr);
        return hr;
    }
    hr = Com(g_msavi702f98, 24, w, 2);
    if (hr < 0) {
        CallC(FUN_5d2ba0, s_ds67f500, hr);
        return hr;
    }
    DWORD d[9];
    memset(d, 0, sizeof d);
    d[0] = 36;
    d[1] = 1;
    hr = Com(g_msavi702f98, 12, d, &prim, 0);
    if (hr < 0) {
        CallC(FUN_5d2ba0, s_ds67f4d8, hr);
        return hr;
    }
    DWORD caps[24];
    caps[0] = 96;
    if (int r = Com(g_msavi702f98, 16, caps)) {
        CallC(FUN_5d2ba0, s_ds67f4c0, r);
        return (int)0x80004005;
    }
    WORD ch, bits;
    BYTE f = (BYTE)caps[1];
    if (f & 8) {
        ch = 2;
        bits = 16;
    } else if (f & 2) {
        ch = 2;
        bits = 8;
    } else if (f & 1) {
        ch = 1;
        bits = 8;
    } else {
        CallC(FUN_5d2ba0, s_ds67f44c, 0);
        return (int)0x80004005;
    }
    Ms_WaveFormat wf;
    memset(&wf, 0, sizeof wf);
    wf.wFormatTag = 1;
    wf.nChannels = ch;
    wf.nSamplesPerSec = 22050;
    wf.wBitsPerSample = bits;
    wf.nBlockAlign = (WORD)((bits >> 3) * ch);
    wf.nAvgBytesPerSec = wf.nBlockAlign * 22050;
    hr = Com(prim, 56, &wf);
    if (hr < 0) {
        char text[256];
        sprintf(text, s_ds67f468, bits, ch);
        CallC(FUN_5d2ba0, text, hr);
        return hr;
    }
    if (prim)
        Com(prim, 8);
    return 0;
}

} // extern "C"

// === the four rays of a marker (FUN_5c7110)

ASM_PROC(FUN_5c6ac0)    // draws a ray (a, p, q, d, ...)
extern "C" void Mk_Normalized(); KING_ALIAS("_Mk_Normalized", "_Vec3_Normalized")
ASM_VAR(Med243_V3, g_mkAhead, $L_702620)
ASM_VAR(Med243_V3, g_mkSide, $L_702640)

extern "C" {

// [FUN_5c7110] draws the four rays of marker a from p to q (each with c, d, e): ahead and sideways (702620,
// 702640 scaled by 64c330), and the two diagonals between them at half length
void __cdecl Marker_DrawRays(int a, Med243_V3 p, Med243_V3 q, int c, int d, int e)
{
    const Med243_V3& f = g_mkAhead;
    const Med243_V3& sd = g_mkSide;
    double k = g_q168_64c330;
    Med243_V3 v = {(float)(f.x * k), (float)(f.y * k), (float)(f.z * k)};
    CallC(FUN_5c6ac0, a, p, q, v, c, d, e);
    Med243_V3 w = {(float)(sd.x * k), (float)(sd.y * k), (float)(sd.z * k)};
    CallC(FUN_5c6ac0, a, p, q, w, c, d, e);
    Med243_V3 t = {(float)((double)sd.x + f.x), (float)((double)sd.y + f.y), (float)((double)sd.z + f.z)}, n;
    const Med243_V3* u = CallC<const Med243_V3*>(Mk_Normalized, &n, &t);
    Med243_V3 h = {(float)(u->x * (double)g_q168_64c258), (float)(u->y * (double)g_q168_64c258),
                   (float)(u->z * (double)g_q168_64c258)};
    CallC(FUN_5c6ac0, a, p, q, h, c, d, e);
    float nx = -sd.x, ny = -sd.y, nz = -sd.z;
    Med243_V3 t2 = {(float)((double)nx + f.x), (float)((double)ny + f.y), (float)((double)nz + f.z)};
    u = CallC<const Med243_V3*>(Mk_Normalized, &n, &t2);
    Med243_V3 h2 = {(float)(u->x * (double)g_q168_64c258), (float)(u->y * (double)g_q168_64c258),
                    (float)(u->z * (double)g_q168_64c258)};
    CallC(FUN_5c6ac0, a, p, q, h2, c, d, e);
}

} // extern "C"

// === attaching a shadow to a node (FUN_5cdcd0)

extern "C" void Sh2_sprintf(); KING_ALIAS("_Sh2_sprintf", "_sprintf")
ASM_VAR(AsmLabel, g_shBuf1, $L_70d8b8)
ASM_VAR(AsmLabel, g_shBuf2, $L_70de98)
ASM_VAR(AsmLabel, s_shUnknown, $L_67fa54)
ASM_VAR(AsmLabel, s_shNoMat, $L_67fa18)
ASM_VAR(AsmLabel, s_shNoNode, $L_67f9b8)
typedef int(__cdecl* Sh2_Ground)(int tree, const float* p, float* normal, float* dir);

// logs fmt with the module of node n and its point (+20) into buf
static void Sh2_Report(char* buf, const char* fmt, BYTE* n)
{
    const char* name = F(const char*, F(BYTE*, n, 36), 60);
    if (!name)
        name = (const char*)s_shUnknown;
    const float* p = (const float*)(n + 20);
    CallC(Sh2_sprintf, buf, fmt, name, (double)p[0], (double)p[1], (double)p[2]);
    CallC(FUN_5d2ba0, buf, 0);
}

extern "C" {

// [FUN_5cdcd0] puts shadow s on node n (its tree +44) under material mat at p with size a: placed through the ground
// callback 70ebec (normal +212, direction +196, then the axes +172 and +184 across it); 1, or 0 with a report
int __fastcall Shadow_Attach(BYTE* s, void*, BYTE* n, BYTE* mat, const float* p, int a)
{
    if (!n)
        return 0;
    if (!F(BYTE*, n, 44)) {
        Sh2_Report((char*)(BYTE*)g_shBuf2, (const char*)s_shNoNode, n);
        return 0;
    }
    if (!mat) {
        Sh2_Report((char*)(BYTE*)g_shBuf1, (const char*)s_shNoMat, n);
        return 0;
    }
    CallVirt(F(BYTE*, n, 44), 20, 0, s);
    F(BYTE*, s, 164) = n;
    F(int, s, 208) = a;
    F(BYTE*, s, 64) = mat;
    memcpy(s + 20, p, 12);
    F(int, s, 32) = a;
    Sh2_Ground fn = (Sh2_Ground)g_70ebec;
    if (!fn)
        return 0;
    float* d = (float*)(s + 196);
    if (!fn(F(int, n, 44), p, (float*)(s + 212), d))
        return 0;
    const Med243_V3& f = g_mkAhead;
    float* x = (float*)(s + 172);
    x[0] = (float)((double)f.z * d[1] - (double)f.y * d[2]);
    x[1] = (float)((double)f.x * d[2] - (double)f.z * d[0]);
    x[2] = (float)((double)f.y * d[0] - (double)f.x * d[1]);
    float* y = (float*)(s + 184);
    float y0 = (float)((double)d[1] * x[2] - (double)d[2] * x[1]);
    float y1 = (float)((double)d[2] * x[0] - (double)d[0] * x[2]);
    float y2 = (float)((double)d[0] * x[1] - (double)d[1] * x[0]);
    y[0] = y0, y[1] = y1, y[2] = y2;
    F(BYTE, s, 40) &= ~1;
    return 1;
}

} // extern "C"
