// directsound.cpp - the unit at 0x603fd0 of king.masm (from the Gi\Srmidp
// sound library): the DirectSound mixer: a priority-ordered list of playing
// sounds under a critical section, the sound table (24-byte entries at
// [$L_724164]), RIFF chunk lookup in wave files, and volume conversion
// between 0..1 and DirectSound's hundredths of decibels.
//
// Building with  nmake ASM_DIRECTSOUND=1  makes king.masm assemble the
// original procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];

ASM_PROC(FUN_6047d0)
ASM_VAR(AsmLabel, g_vtbl652e20, $L_652e20)
ASM_PROC(FUN_604b40)
ASM_PROC(FUN_604410)    // play a table entry (a, b)
extern "C" void FUN_6041b0(); KING_ALIAS("_FUN_6041b0", "_Play_Stop")    // stop sound id

ASM_VAR(int, g_72411c, $L_72411c)
ASM_VAR(int, g_724120, $L_724120)
ASM_VAR(BYTE*, g_playHead, $L_724124)       // playing sounds, highest priority first
ASM_VAR(BYTE*, g_playTail, $L_724128)
ASM_VAR(CRITICAL_SECTION, g_playLock, $L_724148)
ASM_VAR(BYTE*, g_soundTable, $L_724164)     // 24-byte entries
ASM_VAR(DWORD, g_soundCount, $L_724168)     // low word
ASM_VAR(int, g_mixerUsers, $L_724188)
ASM_VAR(BYTE*, g_dsBuffer, $L_724190)       // COM objects
ASM_VAR(BYTE*, g_dsPrimary, $L_724194)
ASM_VAR(HMODULE, g_dsLibrary, $L_72419c)
ASM_VAR(int, g_7241a0, $L_7241a0)
ASM_VAR(HWND, g_mixerWindow, $L_7241a4)
ASM_VAR(int, g_mixerState, $L_6843ec)
ASM_VAR(float, g_mixerVolume, $L_6843fc)
ASM_VAR(int, g_mixerOpenFlag, $L_684400)
ASM_VAR(int, g_mixerResult, $L_684404)
ASM_VAR(AsmLabel, g_vtbl652df8, $L_652df8)

static __forceinline void Release(BYTE* com)
{
    (*(void(__stdcall**)(BYTE*))(*(BYTE**)com + 8))(com);
}

static X87 double x87_log10(double x)
{
    __asm { fldlg2 }
    __asm { fld x }
    __asm { fyl2x }
}

extern "C" void FUN_6055e0(); KING_ALIAS("_FUN_6055e0", "_Play_UpdateAll")
ASM_VAR(BYTE*, g_streams, $L_72412c)

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

extern "C" int __cdecl Volume_ToDb(float v);
extern "C" double __cdecl Volume_FromDb(int db);

extern "C" {

// [FUN_604a60]
void __cdecl Mixer_Lock()
{
    EnterCriticalSection(&g_playLock);
}

// [FUN_604a70]
void __cdecl Mixer_Unlock()
{
    LeaveCriticalSection(&g_playLock);
}

// [FUN_603fd0]
int __cdecl Mixer_Set724120(int a, int b)
{
    g_724120 = a;
    g_72411c = b;
    return 0;
}

// [FUN_603ff0]
void __cdecl Mixer_Clear724120()
{
    g_72411c = 0;
    g_724120 = 0;
}

// [FUN_604130] inserts the sound before the first one of lower priority (+36)
void __fastcall Play_Insert(BYTE* self, void*)
{
    BYTE* n = g_playHead;
    BYTE* prev = 0;
    Mixer_Lock();
    for (; n; prev = n, n = F(BYTE*, n, 16)) {
        if (F(WORD, n, 36) < F(WORD, self, 36)) {
            F(BYTE*, self, 20) = prev;
            F(BYTE*, self, 16) = n;
            if (prev)
                F(BYTE*, prev, 16) = self;
            else
                g_playHead = self;
            F(BYTE*, n, 20) = self;
            Mixer_Unlock();
            return;
        }
    }
    F(BYTE*, self, 16) = 0;
    F(BYTE*, self, 20) = prev;
    g_playTail = self;
    if (prev)
        F(BYTE*, prev, 16) = self;
    else
        g_playHead = self;
    Mixer_Unlock();
}

// [FUN_604000]
BYTE* __fastcall Play_Construct(BYTE* self, void*, int idx, int a, int b, int c, WORD priority, int d)
{
    F(int, self, 52) = idx;
    F(void*, self, 0) = g_vtbl652df8;
    F(DWORD, self, 4) = F(DWORD, g_soundTable, idx * 24);
    F(DWORD, self, 8) = F(DWORD, g_soundTable, idx * 24 + 4);
    F(int, self, 28) = b;
    F(int, self, 32) = c;
    F(int, self, 24) = a;
    F(int, self, 40) = d & 255;
    F(WORD, self, 36) = priority;
    F(int, self, 12) = 0;
    F(int, self, 44) = 11025;
    F(int, self, 48) = 11025;
    Play_Insert(self, 0);
    return self;
}

static void Play_Unlink(BYTE* n)
{
    if (n == g_playTail)
        g_playTail = F(BYTE*, n, 20);
    else
        F(BYTE*, F(BYTE*, n, 16), 20) = F(BYTE*, n, 20);
    if (n == g_playHead)
        g_playHead = F(BYTE*, n, 16);
    else
        F(BYTE*, F(BYTE*, n, 20), 16) = F(BYTE*, n, 16);
}

// [FUN_604230]
void __fastcall Play_Delete(BYTE* self, void*)
{
    Mixer_Lock();
    Play_Unlink(self);
    Mixer_Unlock();
    if (self)
        CallVirt(self, 0, 1);
}

// [FUN_604290]
void __cdecl Play_StopAll()
{
    Mixer_Lock();
    while (g_playHead) {
        BYTE* n = g_playHead;
        CallVirt(n, 24);
        g_playHead = F(BYTE*, g_playHead, 16);
        if (n)
            CallVirt(n, 0, 1);
    }
    Mixer_Unlock();
    g_playTail = 0;
}

// [FUN_6042e0]
BYTE* __cdecl Play_Find(int id)
{
    BYTE* n = g_playHead;
    while (n && F(int, n, 24) != id)
        n = F(BYTE*, n, 16);
    return n;
}

// [FUN_604300] stops every playing instance of table entry idx
void __cdecl Play_StopEntry(int idx)
{
    BYTE* n = g_playHead;
    Mixer_Lock();
    for (; n; n = F(BYTE*, n, 16)) {
        if (F(DWORD, n, 4) == F(DWORD, g_soundTable, idx * 24)) {
            CallVirt(n, 24);
            Play_Unlink(n);
            if (n)
                CallVirt(n, 0, 1);
        }
    }
    Mixer_Unlock();
}

// [FUN_604590]
void __fastcall Wave_Free(BYTE* self, void*)
{
    BYTE* p = F(BYTE*, self, 20);
    if (p && (p < F(BYTE*, self, 8) || p > F(BYTE*, self, 8) + F(DWORD, self, 16)))
        operator delete(p);
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 12) = 0;
    F(int, self, 8) = 0;
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
}

// [FUN_6045d0] moves the read position past the chunk (word aligned)
bool __fastcall Wave_SkipChunk(BYTE* self, void*, const DWORD* ck)
{
    if (!ck)
        return false;
    DWORD end = ck[3] + ck[1];
    if (end > F(DWORD, self, 16))
        return false;
    F(BYTE*, self, 12) = F(BYTE*, self, 8) + ((end + 1) & ~1u);
    return true;
}

// [FUN_604600] finds the chunk ck[0] (for RIFF also the form type ck[2])
// inside parent, or in the whole file: its size in ck[1], data offset in ck[3]
bool __fastcall Wave_FindChunk(BYTE* self, void*, const DWORD* parent, DWORD* ck)
{
    BYTE* base = F(BYTE*, self, 8);
    DWORD left;
    if (!parent) {
        left = F(DWORD, self, 16);
        F(BYTE*, self, 12) = base;
    } else {
        F(BYTE*, self, 12) = base + parent[3];
        left = parent[1];
    }
    if (left < 4)
        return false;
    for (;;) {
        BYTE* p = F(BYTE*, self, 12);
        if (*(DWORD*)p == ck[0])
            break;
        left -= 2;
        F(BYTE*, self, 12) = p + 2;
        if (left < 4)
            return false;
    }
    BYTE* p = F(BYTE*, self, 12);
    left -= 8;
    F(BYTE*, self, 12) = p + 4;
    DWORD size = *(DWORD*)(p + 4);
    F(BYTE*, self, 12) = p + 8;
    if (ck[0] == 0x46464952) {     // 'RIFF'
        if (*(DWORD*)(p + 8) != ck[2])
            return false;
        F(BYTE*, self, 12) = p + 12;
        left -= 4;
        size -= 4;
    }
    if (size > left || left < 4)
        return false;
    ck[1] = size;
    ck[3] = F(BYTE*, self, 12) - base;
    return true;
}

// [FUN_6046c0] 0..1 to hundredths of decibels
int __cdecl Volume_ToDb(float v)
{
    if (!(v > 0.0f))
        return -10000;
    return (int)(x87_log10(v) * 2000.0);
}

// [FUN_6046f0] hundredths of decibels to 0..1
double __cdecl Volume_FromDb(int db)
{
    if (db < -10000)
        return 0.0f;
    if (db >= 0)
        return 1.0f;
    return x87_pow(10.0, db * 0.0005);
}

// [FUN_604790]
int __cdecl Mixer_Open(int a, HWND w)
{
    g_mixerWindow = w;
    if (g_mixerResult < 0 || g_mixerOpenFlag != 1) {
        g_mixerOpenFlag = 1;
        g_mixerResult = CallC<int>(FUN_6047d0, w);
    }
    return g_mixerResult;
}

// [FUN_6049f0]
void __cdecl Mixer_Unload()
{
    if (g_dsPrimary)
        Release(g_dsPrimary);
    if (g_dsBuffer)
        Release(g_dsBuffer);
    g_dsBuffer = 0;
    g_dsPrimary = 0;
    if (g_dsLibrary)
        FreeLibrary(g_dsLibrary);
    g_7241a0 = 0;
    g_dsLibrary = 0;
}

// [FUN_604af0]
int __cdecl Mixer_Start()
{
    if (g_mixerResult)
        return g_mixerResult;
    if (g_mixerUsers)
        return 0;
    InitializeCriticalSection(&g_playLock);
    g_mixerState = 2;
    int r = CallC<int>(FUN_604b40);
    g_mixerResult = r;
    g_mixerUsers++;
    if (!r)
        g_mixerState = 1;
    return r;
}

// [FUN_604d30]
void __cdecl Mixer_ReleaseBuffers()
{
    g_mixerState = 2;
    if (g_dsPrimary)
        Release(g_dsPrimary);
    if (g_dsBuffer)
        Release(g_dsBuffer);
    g_dsBuffer = 0;
    g_dsPrimary = 0;
}

// [FUN_604e80]
void __cdecl Mixer_SetState1()
{
    g_mixerState = 1;
}

// [FUN_604e90]
int __cdecl Mixer_Load(int idx, int a, int b)
{
    if (idx >= (int)(g_soundCount & 0xFFFF))
        return 0;
    return CallThis<int>(FUN_604410, g_soundTable + idx * 24, a, b) & 255;
}

// [FUN_604fe0]
void __cdecl Channel_Stop(int id)
{
    if (id == -1)
        Play_StopAll();
    else
        CallC(FUN_6041b0, id);
}

// [FUN_605000]
void __cdecl Channel_Call8(int id, int a, int b, int c)
{
    BYTE* n = Play_Find(id);
    if (n)
        CallVirt(n, 8, a, b, c);
}

// [FUN_605030]
void __cdecl Channel_SetFrequency(int id, int f)
{
    BYTE* n = Play_Find(id);
    if (n)
        CallVirt(n, 12, f);
}

// [FUN_605090]
void __cdecl Mixer_Stop()
{
    if (g_mixerUsers > 0) {
        Mixer_ReleaseBuffers();
        DeleteCriticalSection(&g_playLock);
        g_mixerUsers--;
    }
}

// [FUN_6050b0]
void __cdecl Mixer_SetVolume(float v)
{
    if (g_dsPrimary) {
        BYTE* vt = *(BYTE**)g_dsPrimary;
        int db = Volume_ToDb(v);
        (*(int(__stdcall**)(BYTE*, int))(vt + 60))(g_dsPrimary, db);
    }
    g_mixerVolume = v;
}

}

extern "C" {

// The functions below sit after the start of grd.cpp in the program, but
// belong to the mixer.

// [FUN_6050f0] the mixer volume (0..1)
double __cdecl Mixer_GetVolume()
{
    float v = g_mixerVolume;
    int db;
    if (g_dsPrimary && Com(g_dsPrimary, 24, &db) == 0)
        return Volume_FromDb(db);
    return v;
}

// [FUN_605140] a DirectSound buffer of size bytes for the format
int __cdecl Mixer_CreateBuffer(BYTE* ds, void* format, BYTE** out, DWORD size)
{
    DWORD desc[9];
    for (int k = 0; k < 9; k++)
        desc[k] = 0;
    desc[2] = size;
    desc[4] = (DWORD)format;
    desc[0] = 36;
    desc[1] = 224;
    if (Com(ds, 12, desc, out, 0) == 0)
        return 1;
    *out = 0;
    return 0;
}

// [FUN_6051b0] deletes the streams whose buffers stopped playing
void __cdecl Streams_Collect()
{
    BYTE* s = g_streams;
    while (s) {
        BYTE* buf = F(BYTE*, s, 24);
        DWORD st;
        if (buf) {
            Com(buf, 36, &st);
            if (!(st & 1)) {
                CallVirt(s, 12);
                BYTE* next = F(BYTE*, s, 8);
                if (s)
                    CallVirt(s, 0, 1);
                s = next;
                continue;
            }
        }
        s = F(BYTE*, s, 8);
    }
    CallC(FUN_6055e0);
}

// [FUN_6052c0] volume and pan in hundredths of decibels from the two
// channel volumes (+28, +32)
void __fastcall Play_UpdateVolume(BYTE* self, void*)
{
    int a = Volume_ToDb(F(float, self, 28));
    int b = Volume_ToDb(F(float, self, 32));
    int v = a > b ? a : b;
    int pan = b - a;
    F(int, self, 68) = v;
    F(int, self, 64) = pan;
    if (v < -10000)
        F(int, self, 68) = -10000;
    if (pan < -10000)
        F(int, self, 64) = -10000;
    if (F(int, self, 64) > 10000)
        F(int, self, 64) = 10000;
}

// [FUN_605490] the highest priority sound without a buffer above p
BYTE* __cdecl Play_BestWaiting(WORD p)
{
    BYTE* best = 0;
    for (BYTE* n = g_playHead; n; n = F(BYTE*, n, 16))
        if (!F(int, n, 56) && F(WORD, n, 36) > p) {
            best = n;
            p = F(WORD, n, 36);
        }
    return best;
}

// [FUN_6054c0] the lowest priority sound with a buffer below p
BYTE* __cdecl Play_WorstPlaying(WORD p)
{
    BYTE* worst = 0;
    for (BYTE* n = g_playHead; n; n = F(BYTE*, n, 16))
        if (F(int, n, 56) && F(WORD, n, 36) < p) {
            worst = n;
            p = F(WORD, n, 36);
        }
    return worst;
}

// [FUN_6054f0] gives the buffer to a better sound, or takes one
void __fastcall Play_Arbitrate(BYTE* self, void*)
{
    BYTE* buf = F(BYTE*, self, 56);
    if (buf) {
        if (Play_BestWaiting(F(WORD, self, 36)))
            Com(buf, 72);
        return;
    }
    BYTE* w = Play_WorstPlaying(F(WORD, self, 36));
    if (w)
        CallVirt(w, 20);
}

}

extern "C" {

// [FUN_605200] a streamed play: the play base, then the stream positions
BYTE* __fastcall StreamPlay_Construct(BYTE* self, void*, int idx, int a, int b, int c, int priority, int d)
{
    Play_Construct(self, 0, idx, a, b, c, (WORD)priority, d);
    F(void*, self, 0) = g_vtbl652e20;
    int len = F(int, F(BYTE*, g_soundTable + idx * 24, 20), 4);
    F(int, self, 56) = 0;
    F(int, self, 60) = 0;
    F(int, self, 44) = len;
    F(int, self, 48) = len;
    Play_UpdateVolume(self, 0);
    return self;
}

}

typedef void (__cdecl* PlayDoneFn)(int);
ASM_VAR(PlayDoneFn, g_playDoneCb724178, $L_724178)
ASM_PROC(FUN_605310)

// [FUN_6055e0] updates all sounds, deleting finished ones, then gives free buffers to waiting sounds
extern "C" void __cdecl Play_UpdateAll()
{
    BYTE* n = g_playHead;
    while (n) {
        if (!CallVirt<int>(n, 28)) {
            if (g_playDoneCb724178)
                g_playDoneCb724178(F(int, n, 24));
            CallVirt(n, 24);
            if (!F(int, n, 40)) {
                BYTE* next = F(BYTE*, n, 16);
                Play_Delete(n, 0);
                n = next;
                continue;
            }
        }
        n = F(BYTE*, n, 16);
    }
    while (BYTE* p = Play_BestWaiting(0xffff))
        CallThis(FUN_605310, p);
}

// ---------------------------------------------------------------------------
extern "C" void FUN_604300(); KING_ALIAS("_FUN_604300", "_Play_StopEntry")
ASM_PROC(FUN_604390)
extern "C" void FUN_604590(); KING_ALIAS("_FUN_604590", "@Wave_Free@8")
ASM_VAR(DWORD, g_ds724168, $L_724168)
ASM_VAR(BYTE*, g_ds724164, $L_724164)
ASM_VAR(AsmLabel, g_vt652df8, $L_652df8)

// [FUN_604ed0] stops and frees sound i (24-byte records at 724164) when it is in use
extern "C" void __cdecl Snd_Free(int i)
{
    if (i >= (int)(g_ds724168 & 0xffff) || !F(int, g_ds724164, i * 24))
        return;
    CallC(FUN_604300, i);
    CallC(FUN_604390, i);
    CallThis(FUN_604590, g_ds724164 + i * 24);
}

// [FUN_604120] the base destructor body: sets the vtable
extern "C" void __fastcall Play_DestructBase(BYTE* self, void*) { F(void*, self, 0) = g_vt652df8; }


// ---------------------------------------------------------------------------
extern "C" void FUN_6042e0(); KING_ALIAS("_FUN_6042e0", "_Play_Find")
extern "C" void FUN_604a60(); KING_ALIAS("_FUN_604a60", "_Mixer_Lock")
extern "C" void FUN_604a70(); KING_ALIAS("_FUN_604a70", "_Mixer_Unlock")
ASM_VAR(BYTE*, g_ds724128, $L_724128)
ASM_VAR(BYTE*, g_ds724124, $L_724124)

// [FUN_6041b0] stops a playing sound by id, unlinks it (+16 prev, +20 next; list 724124..724128) and deletes it
extern "C" void __cdecl Play_Stop(int id)
{
    BYTE* p = CallC<BYTE*>(FUN_6042e0, id);
    if (!p)
        return;
    CallC(FUN_604a60);
    F(int, p, 40) = 0;
    CallVirt(p, 24);
    if (p != g_ds724128)
        F(BYTE*, F(BYTE*, p, 16), 20) = F(BYTE*, p, 20);
    else
        g_ds724128 = F(BYTE*, p, 20);
    if (p != g_ds724124)
        F(BYTE*, F(BYTE*, p, 20), 16) = F(BYTE*, p, 16);
    else
        g_ds724124 = F(BYTE*, p, 16);
    CallC(FUN_604a70);
    CallVirt(p, 0, 1);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_604230(); KING_ALIAS("_FUN_604230", "@Play_Delete@8")
extern "C" void FUN_605200(); KING_ALIAS("_FUN_605200", "@StreamPlay_Construct@32")
extern "C" void FUN_6051b0(); KING_ALIAS("_FUN_6051b0", "_Streams_Collect")
ASM_VAR(AsmLabel, g_ds724130, $L_724130)    // the wave format

template <class R = long, class... A>
static R Ds_ComCall(void* p, int off, A... a)
{
    return ((R(__stdcall*)(void*, A...))(*(void***)p)[off / 4])(p, a...);
}

// [FUN_604c80] sets the cooperative level of a DirectSound for a window and creates its primary buffer in format 724130; *size = its bytes; 0 on failure
extern "C" int __cdecl Ds_CreatePrimary(void* ds, void** buf, DWORD* size, HWND wnd)
{
    DWORD desc[9] = {};
    desc[0] = 36;
    desc[1] = 129;
    if (!Ds_ComCall(ds, 24, wnd, 2) && !Ds_ComCall(ds, 12, desc, buf, (void*)0)
        && !Ds_ComCall(*buf, 56, (void*)g_ds724130))
    {
        DWORD caps[5];
        caps[0] = 20;
        Ds_ComCall(*buf, 12, caps);
        *size = caps[2];
        return 1;
    }
    *buf = 0;
    *size = 0;
    return 0;
}

// [FUN_604f20] starts a sound of record i with an id (restarting a stopped one), its object (72 bytes) made by 605200; 0 when it can't
extern "C" int __cdecl Snd_Start(int i, int id, int a3, int a4, int a5, int a6)
{
    if (i < (int)(g_ds724168 & 0xffff) && id != -1 && F(int, g_ds724164, i * 24))
    {
        BYTE* p = CallC<BYTE*>(FUN_6042e0, id);
        if (p)
        {
            if (CallVirt<int>(p, 28))
                goto fail;
            CallVirt(p, 24);
            if (F(int, p, 40))
                goto fail;
            CallThis(FUN_604230, p);
        }
        BYTE* o = (BYTE*)operator new(72);
        if (o)
            o = CallThis<BYTE*>(FUN_605200, o, i, id, a3, a4, a5, a6);
        return CallVirt<int>(o, 4);
    }
fail:
    CallC(FUN_6051b0);
    return 0;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_604120(); KING_ALIAS("_FUN_604120", "@Play_DestructBase@8")
extern "C" void FUN_6052b0(); KING_ALIAS("_FUN_6052b0", "@Play6052b0_Destruct@8")
ASM_VAR(float, g_dir652e40, $L_652e40)
ASM_VAR(float, g_dir7241ac, $L_7241ac)

static BYTE* Dir_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_604100]
extern "C" BYTE* __fastcall Dir604100_DeletingDestructor(BYTE* s, void*, int flags) { return Dir_DeleteAfter(s, flags, FUN_604120); }
// [$L_605290]
extern "C" BYTE* __fastcall Dir605290_DeletingDestructor(BYTE* s, void*, int flags) { return Dir_DeleteAfter(s, flags, FUN_6052b0); }
// [$L_605750]
extern "C" void __cdecl DirInit_605750() { g_dir7241ac = (float)((double)g_dir652e40 + (double)g_dir652e40); }

extern "C" void FUN_604a60(); KING_ALIAS("_FUN_604a60", "_Mixer_Lock")
extern "C" void FUN_6051b0(); KING_ALIAS("_FUN_6051b0", "_Streams_Collect")
ASM_PROC(FUN_605310)
extern "C" void FUN_604a70(); KING_ALIAS("_FUN_604a70", "_Mixer_Unlock")
extern "C" void FUN_6052c0(); KING_ALIAS("_FUN_6052c0", "@Play_UpdateVolume@8")
extern "C" void FUN_6054f0(); KING_ALIAS("_FUN_6054f0", "@Play_Arbitrate@8")
ASM_VAR(int, g_dir6843ec, $L_6843ec)

// a COM method of the buffer o (vtable slot at byte offset off)
template <class R = int, class... A>
static R Dir_Com(void* o, unsigned off, A... args)
{
    void* fn = *(void**)(*(char**)o + off);
    return ((R(__stdcall*)(void*, A...))fn)(o, args...);
}

// [$L_604080] format (+28, +32, +36 word)
extern "C" void __fastcall Dir_SetFormat(BYTE* s, void*, int a, int b, WORD w)
{
    F(int, s, 28) = a;
    F(int, s, 32) = b;
    F(WORD, s, 36) = w;
}

// [$L_6040e0] the frequency (+44) in thousandths of the base (+48)
extern "C" unsigned __fastcall Dir_Rate(BYTE* s) { return F(unsigned, s, 44) * 1000 / F(unsigned, s, 48); }

// [$L_6040a0] the frequency from a rate in thousandths, kept within 100..100000
extern "C" void __fastcall Dir_SetRate(BYTE* s, void*, unsigned rate)
{
    F(unsigned, s, 44) = F(unsigned, s, 48) * rate / 1000;
    if (F(unsigned, s, 44) > 100000)
        F(unsigned, s, 44) = 100000;
    if (F(unsigned, s, 44) < 100)
        F(unsigned, s, 44) = 100;
}

// [$L_6056c0] the buffer's frequency read back (vtbl+32 into +44), as a rate
extern "C" unsigned __fastcall Dir_ReadRate(BYTE* s)
{
    void* b = F(void*, s, 56);
    if (b)
        Dir_Com(b, 32, s + 44);
    return F(unsigned, s, 44) * 1000 / F(unsigned, s, 48);
}

// [$L_6055a0] the buffer's vtbl+72
extern "C" void __fastcall Dir_Buffer72(BYTE* s)
{
    void* b = F(void*, s, 56);
    if (b)
        Dir_Com(b, 72);
}

// [$L_605580] releases the buffer
extern "C" void __fastcall Dir_ReleaseBuffer(BYTE* s)
{
    void* b = F(void*, s, 56);
    if (b)
        Dir_Com(b, 8);
    F(int, s, 60) = 0;
    F(int, s, 56) = 0;
}

// [$L_6055b0] 1 while playing (status bit 0) or without a buffer
extern "C" int __fastcall Dir_IsPlaying(BYTE* s)
{
    void* b = F(void*, s, 56);
    if (!b)
        return 1;
    DWORD st;
    Dir_Com(b, 36, &st);
    return st & 1;
}

// [$L_605460]
extern "C" int __fastcall Dir_605460(BYTE* s)
{
    CallThis(FUN_604a60, s);
    CallC(FUN_6051b0);
    int r = CallThis<int>(FUN_605310, s);
    if (r)
        g_dir6843ec = 3;
    CallC(FUN_604a70);
    return r;
}

// [$L_605530] a new format; the buffer gets volume (+64) and pan (+68) again
extern "C" int __fastcall Dir_Reformat(BYTE* s, void*, int a, int b, WORD w)
{
    F(int, s, 32) = b;
    F(int, s, 28) = a;
    F(WORD, s, 36) = w;
    CallThis(FUN_6052c0, s);
    if (F(void*, s, 56)) {
        Dir_Com(F(void*, s, 56), 64, F(int, s, 64));
        Dir_Com(F(void*, s, 56), 60, F(int, s, 68));
    }
    return CallThis<int>(FUN_6054f0, s);
}

// ---------------------------------------------------------------------------
// More virtual methods and static initializers.

// [$L_605650] the rate in thousandths (1000 keeps the base) to +44 and the buffer (vtbl+68)
extern "C" int __fastcall Dir_SetRateBuffer(BYTE* s, void*, unsigned rate)
{
    if (rate == 1000) {
        F(int, s, 44) = F(int, s, 48);
        if (F(void*, s, 56))
            return Dir_Com(F(void*, s, 56), 68, 0);
        return (int)F(void*, s, 56);
    }
    F(unsigned, s, 44) = F(unsigned, s, 48) * rate / 1000;
    if (F(unsigned, s, 44) > 100000)
        F(unsigned, s, 44) = 100000;
    if (F(unsigned, s, 44) < 100)
        F(unsigned, s, 44) = 100;
    if (F(void*, s, 56))
        return Dir_Com(F(void*, s, 56), 68, F(int, s, 44));
    return 0;
}

// ---------------------------------------------------------------------------
// Destructor thunks: the vtable, then the base destructor; static object cleanups.

extern "C" void FUN_604120(); KING_ALIAS("_FUN_604120", "@Play_DestructBase@8")
ASM_VAR(AsmLabel, g_q46_652e20, $L_652e20)
// [FUN_6052b0] destructor: the 652e20 vtable, then the base
extern "C" void __fastcall Play6052b0_Destruct(BYTE* s, void*)
{
    F(void*, s, 0) = g_q46_652e20;
    CallThis(FUN_604120, s);
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

extern "C" void FUN_604290(); KING_ALIAS("_FUN_604290", "_Play_StopAll")
extern "C" void FUN_604a60(); KING_ALIAS("_FUN_604a60", "_Mixer_Lock")
extern "C" unsigned __int64 __rdtsc(void);
extern "C" void __cpuid(int info[4], int leaf);
extern "C" unsigned int __readeflags(void);
extern "C" void __writeeflags(unsigned int v);
#pragma intrinsic(__rdtsc, __cpuid, __readeflags, __writeeflags)
ASM_VAR(BYTE*, g_q47_72412c, $L_72412c)
// [$L_6043d0] under the mixer lock, stops (slot 12) and deletes each sound of the 72412c list
extern "C" void __cdecl Play_DeleteAll(void)
{
    CallC(FUN_604a60);
    while (g_q47_72412c) {
        CallVirt(g_q47_72412c, 12);
        if (!g_q47_72412c)
            break;
        CallVirt(g_q47_72412c, 0, 1);
    }
    Mixer_Unlock();
}
// [FUN_604e70] stops all sounds, then deletes them
extern "C" void __cdecl Play_StopAndDeleteAll(void)
{
    CallC(FUN_604290);
    Play_DeleteAll();
}
// [FUN_6056f0] the EFLAGS bits that changed on toggling bit 21: nonzero when CPUID is available
extern "C" unsigned __cdecl Cpu_HasCpuid(void)
{
    unsigned o = (unsigned)__readeflags();
    __writeeflags(o ^ 0x200000);
    return (unsigned)__readeflags() ^ o;
}
// [FUN_60573b] the time stamp counter (after a serialising CPUID) into t
extern "C" void __cdecl Cpu_ReadTsc(unsigned* t)
{
    int r[4];
    __cpuid(r, 0);
    unsigned __int64 v = __rdtsc();
    t[0] = (unsigned)v;
    t[1] = (unsigned)(v >> 32);
}

// ---------------------------------------------------------------------------
// Small setup procs: static vectors, a light array, the race start, a stream read.

ASM_PROC(FUN_604e10)
extern "C" void FUN_603ff0(); KING_ALIAS("_FUN_603ff0", "_Mixer_Clear724120")
extern "C" void FUN_605090(); KING_ALIAS("_FUN_605090", "_Mixer_Stop")
extern "C" void FUN_6050b0(); KING_ALIAS("_FUN_6050b0", "_Mixer_SetVolume")
ASM_VAR(int, g_q48_72418c, $L_72418c)
ASM_VAR(int, g_q48_6843fc, $L_6843fc)
// [FUN_605050] shuts the mixer down: 604e10, clears it, sets the 72418c volume keeping 6843fc, stops it
extern "C" void __cdecl Mixer_Shutdown(void)
{
    CallC(FUN_604e10);
    CallC(FUN_603ff0);
    int keep = g_q48_6843fc;
    CallC(FUN_6050b0, g_q48_72418c);
    g_q48_6843fc = keep;
    CallC(FUN_605090);
}
