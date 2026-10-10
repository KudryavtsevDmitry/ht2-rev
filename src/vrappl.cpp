// vrappl.cpp - small functions of the engine library units after the window
// controls (0x5bfe60 on): list counters, the AVI sound stream (msavi),
// keyed object tables, the music player, text logs, tree particles and
// tree shadows.
//
// Building with  nmake ASM_VRAPPL=1  makes king.masm assemble the original
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
ASM_PROC(FUN_5d2ba0)    // log (text, value)
ASM_PROC(FUN_5c2800)    // msavi: create the sound buffer
ASM_PROC(FUN_5c36e0)
ASM_PROC(FUN_6021f0)
ASM_PROC(FUN_602510)
ASM_PROC(FUN_6022a0)
ASM_PROC(FUN_602e80)
extern "C" void FUN_5ab160(); KING_ALIAS("_FUN_5ab160", "_File_WriteInt")  // write an int to a file (value, file)
ASM_PROC(FUN_5c3f20)
ASM_PROC(FUN_5f19b0)

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
ASM_PROC(FUN_5d2ac0)
ASM_PROC(FUN_6037e0)    // time in seconds, returns double
ASM_PROC(FUN_5d35b0)
ASM_PROC(FUN_5d3790)
ASM_PROC(FUN_5d3080)    // next sibling
ASM_PROC(FUN_5d38c0)
ASM_PROC(FUN_404420)
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

// [FUN_5cf3f0]
void __fastcall Obj_Notify104(BYTE* self, void*)
{
    BYTE* o = F(BYTE*, self, 96);
    if (o)
        CallVirt(o, 24, self + 104);
}

// [FUN_5cfab0]
int __cdecl Blit_WithKey(int a, int b, int c, int d, int e, int f, int key)
{
    g_67faa0 = key;
    return CallC<int>(FUN_5cfb30, a, b, c, d, e, f);
}

// [FUN_5cfae0]
int __cdecl Blit_WithAlpha(int a, int b, int c, int d, int e, int f, int alpha)
{
    g_71e800 = alpha;
    int r = CallC<int>(FUN_5cfb30, a, b, c, d, e, f);
    g_71e800 = 0;
    return r;
}

// [FUN_5cfb20]
void __cdecl Set71e7d4(int v)
{
    g_71e7d4 = v;
}

// [FUN_5d2030]
int __cdecl Get71e7e4()
{
    return g_71e7e4;
}

// [FUN_5d2100] a zeroed shared memory file
void __fastcall MapFile_Open(BYTE* self, void*, const char* name, DWORD size)
{
    if (F(BYTE, self, 20))
        return;
    F(DWORD, self, 8) = size;
    HANDLE h = CreateFileA(name, 0xC0000000, 1, 0, 2, 0x90000000, 0);
    F(HANDLE, self, 0) = h;
    if (h == INVALID_HANDLE_VALUE)
        return;
    HANDLE m = CreateFileMappingA(h, 0, 4, 0, F(DWORD, self, 8), 0);
    F(HANDLE, self, 4) = m;
    BYTE* v = (BYTE*)MapViewOfFile(m, 0xF001F, 0, 0, 0);
    F(int, self, 16) = 0;
    F(BYTE*, self, 12) = v;
    if (v)
        memset(v, 0, F(DWORD, self, 8));
    F(BYTE, self, 20) = 1;
}

// [FUN_5d2190] links self after list
void __fastcall Node_Link(BYTE* self, void*, BYTE* list)
{
    F(BYTE*, self, 4) = list;
    BYTE* n = F(BYTE*, list, 8);
    F(BYTE*, self, 8) = n;
    if (n)
        F(BYTE*, n, 4) = self;
    F(BYTE*, list, 8) = self;
}

// [FUN_5d21b0]
void __fastcall Node_Unlink(BYTE* self, void*)
{
    BYTE* p = F(BYTE*, self, 4);
    if (p)
        F(BYTE*, p, 8) = F(BYTE*, self, 8);
    BYTE* n = F(BYTE*, self, 8);
    if (n)
        F(BYTE*, n, 4) = F(BYTE*, self, 4);
    F(BYTE*, self, 4) = 0;
    F(BYTE*, self, 8) = 0;
}

// [FUN_5d2370]
void __fastcall NameHash_Clear(BYTE* self, void*)
{
    if (!g_71e844)
        return;
    for (int i = 0; i < F(int, self, 4); i++) {
        if (!F(BYTE**, self, 8)[i])
            continue;
        BYTE* n;
        while ((n = F(BYTE*, F(BYTE**, self, 8)[i], 8)) != 0) {
            Node_Unlink(n, 0);
            CallVirt(n, 0, 1);
        }
        BYTE* b = F(BYTE**, self, 8)[i];
        if (b)
            CallVirt(b, 0, 1);
        F(BYTE**, self, 8)[i] = 0;
    }
    if (F(BYTE*, self, 0)) {
        BYTE* n;
        while ((n = F(BYTE*, F(BYTE*, self, 0), 8)) != 0) {
            Node_Unlink(n, 0);
            CallVirt(n, 0, 1);
        }
        if (F(BYTE*, self, 0))
            CallVirt(F(BYTE*, self, 0), 0, 1);
        F(BYTE*, self, 0) = 0;
    }
}

// [FUN_5d2500]
unsigned __fastcall Hash_Name(BYTE* self, void*, const BYTE* s)
{
    unsigned h = 0;
    if (!s)
        return 0;
    while (*s)
        h += *s++;
    return h % 500;
}

// [FUN_5d2720]
int __fastcall Hash_Lookup(BYTE* self, void*, const char* name, int* out)
{
    if (!name) {
        *out = 0;
        return 0;
    }
    for (BYTE* e = F(BYTE**, self, 8)[Hash_Name(self, 0, (const BYTE*)name)]; e; e = F(BYTE*, e, 8)) {
        const char* a = name;
        const char* b = F(const char*, e, 12);
        while (*a == *b && *a) {
            a++;
            b++;
        }
        if (*a == *b) {
            *out = F(int, e, 16);
            return 1;
        }
    }
    *out = 0;
    return 0;
}

// [FUN_5d2a20] count blocks of size bytes and a used flag for each
BYTE* __fastcall Pool_Init(BYTE* self, void*, DWORD size, DWORD count)
{
    F(DWORD, self, 0) = size;
    F(DWORD, self, 4) = count;
    F(void*, self, 8) = operator new(size * count);
    F(BYTE*, self, 12) = (BYTE*)operator new(count);
    for (DWORD i = 0; i < count; i++)
        F(BYTE*, self, 12)[i] = 0;
    F(int, self, 16) = 0;
    return self;
}

// [FUN_5d29b0]
void __cdecl Pool71e830_Init()
{
    Pool_Init((BYTE*)g_pool71e830, 0, 24, 60000);
}

// [FUN_5d2b20]
DWORD __fastcall Pool_Used(BYTE* self, void*, BYTE* p)
{
    return F(BYTE*, self, 12)[(DWORD)(p - F(BYTE*, self, 8)) / F(DWORD, self, 0)];
}

// [FUN_5d2e50]
int __cdecl Table71e858_Get(int i)
{
    if (i >= 1000 || i < 0)
        return 0;
    return g_71e858[i];
}

// [FUN_5d2ec0]
void __cdecl Set720080(const char* s)
{
    char* d = g_720080;
    while ((*d++ = *s++) != 0) {
    }
}

// [FUN_5d2fa0]
void __cdecl Time_NowMs(double* out)
{
    *out = CallC<double>(FUN_6037e0) * 1000.0;
}

// [FUN_5d2fc0] *last becomes the time since *last (not negative), added to *acc
void __cdecl Time_Delta(double* last, double* acc)
{
    double t = CallC<double>(FUN_6037e0) * 1000.0 - *last;
    if (!(t >= 0.0f))
        t = 0.0f;
    *acc = t + *acc;
    *last = t;
}

// [FUN_5d3010]
void __cdecl Pool7200c0_Init()
{
    Pool_Init((BYTE*)g_pool7200c0, 0, 24, 500);
}

// [FUN_5d3070]
int __cdecl Pool7200c0_Free(void* p)
{
    return CallThis<int>(FUN_5d2ac0, g_pool7200c0, p);
}

// [FUN_5d3090]
BYTE* __fastcall Event_Construct(BYTE* self, void*, int a, int b)
{
    F(int, self, 12) = a;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl651eb4;
    F(int, self, 16) = b;
    F(int, self, 20) = 0;
    return self;
}

// [FUN_5d30e0]
BYTE* __fastcall Event_Construct3(BYTE* self, void*, int a, int b, int c)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl651eb4;
    F(int, self, 12) = a;
    F(int, self, 16) = b;
    F(int, self, 20) = c;
    return self;
}

// [FUN_5d33b0]
void __fastcall Handle_Release(BYTE* self, void*)
{
    if (F(BYTE*, self, 12))
        CallThis(FUN_5d35b0, F(BYTE*, self, 12), self);
    F(int, self, 12) = 0;
}

// [FUN_5d3430]
void __fastcall Handle_Call3790(BYTE* self, void*)
{
    if (F(BYTE*, self, 12))
        CallThis(FUN_5d3790, F(BYTE*, self, 12), self);
}

// [FUN_5d3820] how many queued events have +12 == key
int __fastcall Queue_Count(BYTE* self, void*, int key)
{
    int n = 0;
    for (BYTE* e = F(BYTE*, self, 12); e; e = CallThis<BYTE*>(FUN_5d3080, e))
        if (F(int, e, 12) == key)
            n++;
    return n;
}

// [FUN_5d3440]
int __fastcall Handle_Count(BYTE* self, void*)
{
    BYTE* q = F(BYTE*, self, 12);
    return q ? Queue_Count(q, 0, (int)self) : 0;
}

// [FUN_5d3630]
void __fastcall Queue_Log(BYTE* self, void*)
{
    for (BYTE* e = F(BYTE*, self, 12); e; e = CallThis<BYTE*>(FUN_5d3080, e))
        Log(F(const char*, F(BYTE*, e, 12), 28), F(int, e, 20));
}

// [FUN_5d38a0] the clock in ms
int __fastcall Queue_NowMs(BYTE* self, void*)
{
    double* t = F(double*, self, 24);
    if (!t)
        return 0;
    return (int)(*t * 1000.0);
}

// [FUN_5d3850] runs the due events of both queues
void __fastcall Queue_Tick(BYTE* self, void*)
{
    BYTE* e;
    while ((e = F(BYTE*, self, 12)) != 0 && F(int, e, 16) <= F(int, self, 0))
        CallThis(FUN_5d38c0, self, e);
    while ((e = F(BYTE*, self, 8)) != 0 && F(int, e, 16) <= Queue_NowMs(self, 0))
        CallThis(FUN_5d38c0, self, e);
    F(int, self, 0) = F(int, self, 0) + 1;
}

// [FUN_5d3a60]
void __fastcall Queue_Remove(BYTE* self, void*, BYTE* e)
{
    if (e == F(BYTE*, self, 12)) {
        F(BYTE*, self, 12) = CallThis<BYTE*>(FUN_5d3080, e);
    } else if (e == F(BYTE*, self, 8)) {
        F(BYTE*, self, 8) = CallThis<BYTE*>(FUN_5d3080, e);
    } else {
        Node_Unlink(e, 0);
        return;
    }
    BYTE* n = F(BYTE*, e, 8);
    if (n)
        F(int, n, 4) = 0;
}

// [FUN_5ffb20]
int __cdecl Obj723d44_Delete()
{
    if (g_723d44)
        CallVirt(g_723d44, 0, 1);
    g_723d44 = 0;
    return 0;
}

// [FUN_5ffc30]
int __cdecl Set683c58(DWORD lo, DWORD hi)
{
    EnterCriticalSection(&g_cs723b40);
    ((DWORD*)g_683c58)[0] = lo;
    ((DWORD*)g_683c58)[1] = hi;
    LeaveCriticalSection(&g_cs723b40);
    return 0;
}

// [FUN_600540]
BYTE* __fastcall Obj600540_Delete(BYTE* self, void*, int flags)
{
    CallThis(FUN_404420, self);
    if (flags & 1)
        operator delete(self);
    return self;
}

}
