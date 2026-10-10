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
ASM_PROC(FUN_604b40)
ASM_PROC(FUN_604410)    // play a table entry (a, b)
ASM_PROC(FUN_6041b0)    // stop sound id

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
