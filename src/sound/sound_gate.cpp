// sound_gate.cpp - D1gate.cpp of the Gi\Srmidp library, rewritten from
// king.masm: the DirectPlay network gate (service providers, sessions,
// message records with a GUID, data and name), the byte stream of network
// messages ('*' float, '-' int, 'x' blob fields), the DirectShow music
// player (state machine over IMediaControl/IMediaEvent/IBasicAudio), and
// the .ini helpers of the engine.
//
// Building with  nmake ASM_SOUND_GATE=1  makes king.masm assemble the
// original procedures again.

#include "king.h"
#include "x87.h"
#include <objbase.h>

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

typedef char AsmLabel[1];
extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
extern "C" char* __cdecl strncpy(char* d, const char* s, size_t n);
extern "C" long __cdecl strtol(const char* s, char** end, int base);
extern "C" long __cdecl atol(const char* s);
extern "C" double __cdecl atof(const char* s);

ASM_PROC(FUN_6016f0)    // rewinds the service provider list
ASM_PROC(FUN_600b50)    // DirectPlay error text (hr)
ASM_PROC(FUN_401010)    // fatal error report
ASM_PROC(FUN_601ff0)
ASM_PROC(FUN_602320)
ASM_PROC(FUN_603f30)

ASM_VAR(AsmLabel, g_guidIpx, $L_655340)
ASM_VAR(AsmLabel, g_guidTcp, $L_655330)
ASM_VAR(AsmLabel, g_guidSerial, $L_655320)
ASM_VAR(AsmLabel, g_guidModem, $L_655310)
ASM_VAR(AsmLabel, s_ipx, $L_68427c)
ASM_VAR(AsmLabel, s_tcp, $L_684274)
ASM_VAR(AsmLabel, s_serial, $L_684268)
ASM_VAR(AsmLabel, s_modem, $L_684260)
ASM_VAR(BYTE*, g_providersBegin, $L_723d60)     // vector of 32-byte records
ASM_VAR(BYTE*, g_providersEnd, $L_723d64)
ASM_VAR(DWORD, g_providerNext, $L_723dec)
ASM_VAR(BYTE*, g_directPlay, $L_723ddc)          // IDirectPlay4
ASM_VAR(DWORD, g_dpPlayer, $L_723de0)
ASM_VAR(BYTE, g_comInitialized, $L_723df0)
ASM_VAR(AsmLabel, g_clsidDirectPlay, $L_655350)
ASM_VAR(AsmLabel, g_iidDirectPlay4, $L_655360)
ASM_VAR(AsmLabel, g_68d1c0, $L_68d1c0)
ASM_VAR(AsmLabel, s_noDirectPlay, $L_684280)
ASM_VAR(AsmLabel, s_d1gateId, $L_683c78)
ASM_VAR(AsmLabel, s_d1gateFile, $L_68422c)
ASM_VAR(AsmLabel, g_dpMessages, $L_723d90)
ASM_VAR(AsmLabel, g_playerName, $L_723df8)
ASM_VAR(AsmLabel, g_playerName2, $L_723f88)
ASM_VAR(DWORD, g_723d88, $L_723d88)
ASM_VAR(AsmLabel, g_vtbl652dc8, $L_652dc8)
ASM_VAR(AsmLabel, g_iidMediaEvent, $L_6554b0)
ASM_VAR(AsmLabel, g_iidBasicAudio, $L_6554e0)
ASM_VAR(AsmLabel, g_iidMediaControl, $L_6554f0)
ASM_VAR(AsmLabel, s_N, $L_6761e8)                // "N", the missing value
ASM_VAR(AsmLabel, s_ld, $L_6843ac)               // "%ld"
ASM_VAR(AsmLabel, s_g, $L_671350)                // "%g"

static __forceinline void Release(BYTE* com)
{
    (*(void(__stdcall**)(BYTE*))(*(BYTE**)com + 8))(com);
}

template <class... A>
static __forceinline int Com(BYTE* com, unsigned off, A... args)
{
    return (*(int(__stdcall**)(BYTE*, A...))(*(BYTE**)com + off))(com, args...);
}

static char* StrDup(const char* s)
{
    size_t n = strlen(s) + 1;
    char* d = (char*)operator new(n);
    memcpy(d, s, n);
    return d;
}

static X87 double x87_log10(double x)
{
    __asm { fldlg2 }
    __asm { fld x }
    __asm { fyl2x }
}

extern "C" {

// [FUN_600b70]
BYTE* __fastcall DpHandler_Construct(BYTE* self, void*, int owner)
{
    F(int, self, 4) = owner;
    F(int, self, 8) = 0;
    F(void*, self, 0) = g_vtbl652dc8;
    return self;
}

// A record: +0 GUID, +16 data, +20 its size, +24 name, +28 flags.

// [FUN_601270]
BYTE* __fastcall DpRecord_Construct(BYTE* self, void*, const DWORD* guid, const void* data, DWORD size,
                                    const char* name, int flags)
{
    for (int k = 0; k < 4; k++)
        F(DWORD, self, k * 4) = guid[k];
    F(void*, self, 16) = operator new(size);
    if (size)
        memcpy(F(void*, self, 16), data, size);
    F(DWORD, self, 20) = size;
    F(char*, self, 24) = StrDup(name);
    F(int, self, 28) = flags;
    return self;
}

static void DpRecord_CopyFrom(BYTE* self, const BYTE* s)
{
    for (int k = 0; k < 4; k++)
        F(DWORD, self, k * 4) = F(DWORD, s, k * 4);
    F(void*, self, 16) = operator new(F(DWORD, s, 20));
    if (F(DWORD, s, 20))
        memcpy(F(void*, self, 16), F(void*, s, 16), F(DWORD, s, 20));
    F(DWORD, self, 20) = F(DWORD, s, 20);
    F(char*, self, 24) = StrDup(F(const char*, s, 24));
    F(int, self, 28) = F(int, s, 28);
}

// [FUN_601310]
BYTE* __fastcall DpRecord_Copy(BYTE* self, void*, const BYTE* s)
{
    DpRecord_CopyFrom(self, s);
    return self;
}

// [FUN_6013b0]
BYTE* __fastcall DpRecord_Assign(BYTE* self, void*, const BYTE* s)
{
    if (F(void*, self, 16))
        operator delete(F(void*, self, 16));
    if (F(void*, self, 24))
        operator delete(F(void*, self, 24));
    DpRecord_CopyFrom(self, s);
    return self;
}

// [FUN_6014a0] the name of a service provider
const char* __fastcall DpRecord_ProviderName(BYTE* self, void*)
{
    if (!memcmp(self, g_guidIpx, 16))
        return s_ipx;
    if (!memcmp(self, g_guidTcp, 16))
        return s_tcp;
    if (!memcmp(self, g_guidSerial, 16))
        return s_serial;
    if (!memcmp(self, g_guidModem, 16))
        return s_modem;
    return F(const char*, self, 24);
}

// [FUN_601570] the next service provider into *out, -1 after the last
int __cdecl Dp_NextProvider(BYTE* out)
{
    if (g_providerNext >= (DWORD)((g_providersEnd - g_providersBegin) >> 5))
        return -1;
    DpRecord_Assign(out, 0, g_providersBegin + g_providerNext * 32);
    g_providerNext++;
    return 0;
}

// [FUN_6015b0]
int __cdecl Dp_FirstProvider(BYTE* out)
{
    CallC(FUN_6016f0);
    g_providerNext = 0;
    return Dp_NextProvider(out);
}

// [FUN_6015d0]
void __cdecl Dp_Create()
{
    if (g_directPlay)
        return;
    int hr = CoCreateInstance(*(const CLSID*)g_clsidDirectPlay, 0, 1, *(const IID*)g_iidDirectPlay4,
                              (void**)&g_directPlay);
    if (hr < 0) {
        int text = CallC<int>(FUN_600b50, hr);
        CallC(FUN_401010, 2, 100, (const char*)s_d1gateFile, (const char*)s_d1gateId, 647,
              (const char*)s_noDirectPlay, text, (void*)g_68d1c0);
    }
}

// [FUN_601fa0]
int __cdecl Dp_Release()
{
    if (g_directPlay) {
        if (g_dpPlayer)
            Com(g_directPlay, 36, g_dpPlayer);
        g_dpPlayer = 0;
        Com(g_directPlay, 16);
        Release(g_directPlay);
        g_directPlay = 0;
    }
    return 0;
}

// [FUN_6021b0]
int __cdecl Dp_Send(int a, int b)
{
    return CallC<int>(FUN_601ff0, 0, a, b);
}

// [FUN_6021d0]
int __cdecl Dp_SendTo(int to, int a, int b)
{
    return CallC<int>(FUN_601ff0, to, a, b);
}

// [FUN_6022a0]
int __cdecl Dp_Shutdown()
{
    Dp_Release();
    if (g_comInitialized) {
        CoUninitialize();
        g_comInitialized = 0;
    }
    return 0;
}

// [FUN_6022f0] takes the next received message of the list
int __cdecl MsgList_Take(DWORD* out, BYTE* list)
{
    BYTE* cur = F(BYTE*, list, 4);
    if (cur == F(BYTE*, list, 0)) {
        *out = 0;
        return -1;
    }
    *out = F(DWORD, cur, 8);
    F(BYTE*, list, 4) = *(BYTE**)F(BYTE*, list, 4);
    return 0;
}

// [FUN_6024d0]
int __cdecl Dp_Receive(int a)
{
    return CallC<int>(FUN_602320, a, (void*)g_dpMessages);
}

// [FUN_6024f0]
int __cdecl Dp_TakeMessage(DWORD* out)
{
    return MsgList_Take(out, (BYTE*)g_dpMessages);
}

// [FUN_602510]
int __cdecl Dp_SetPlayerName(const char* s)
{
    strncpy(g_playerName, s, 200);
    strncpy(g_playerName2, s, 200);
    return 0;
}

// [FUN_602540]
int __cdecl Dp_Get723d88(DWORD* out)
{
    *out = g_723d88;
    return 0;
}

// [FUN_602b60]
void __fastcall Blob_Set(BYTE* self, void*, const void* data, DWORD size)
{
    if (F(void*, self, 4)) {
        operator delete(F(void*, self, 4));
        F(void*, self, 4) = 0;
    }
    F(DWORD, self, 8) = size;
    if (data && size) {
        F(void*, self, 4) = operator new(size);
        memcpy(F(void*, self, 4), data, size);
    }
}

// [FUN_602bc0]
BYTE* __fastcall Stream_Construct(BYTE* self, void*)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 0) = 0;
    return self;
}

// Message stream fields at +4 (buffer) + +24 (read position): a type byte,
// the field id, then the value.

// [FUN_603460]
int __fastcall Stream_ReadFloat(BYTE* self, void*, DWORD* id, float* out)
{
    BYTE* p = F(BYTE*, self, 4) + F(int, self, 24);
    *id = *(DWORD*)(p + 1);
    if (p[0] != '*')
        return 3;
    *out = *(float*)(F(BYTE*, self, 4) + F(int, self, 24) + 5);
    return 0;
}

// [FUN_6034a0]
int __fastcall Stream_ReadInt(BYTE* self, void*, DWORD* id, DWORD* out)
{
    BYTE* p = F(BYTE*, self, 4) + F(int, self, 24);
    *id = *(DWORD*)(p + 1);
    if (p[0] != '-')
        return 3;
    *out = *(DWORD*)(F(BYTE*, self, 4) + F(int, self, 24) + 5);
    return 0;
}

// [FUN_6034e0]
int __fastcall Stream_ReadBlob(BYTE* self, void*, DWORD* id, void* buf, DWORD* size)
{
    BYTE* p = F(BYTE*, self, 4) + F(int, self, 24);
    *id = *(DWORD*)(p + 1);
    if (p[0] != 'x')
        return 3;
    DWORD n = *(DWORD*)(F(BYTE*, self, 4) + F(int, self, 24) + 5);
    *size = n;
    memcpy(buf, F(BYTE*, self, 4) + F(int, self, 24) + 9, n);
    return 0;
}

// [FUN_603540] copy of the bytes [first, last) forwards, returns the end
BYTE* __cdecl Bytes_Copy(const BYTE* first, const BYTE* last, BYTE* dest)
{
    for (int n = last - first; n > 0; n--)
        *dest++ = *first++;
    return dest;
}

// [FUN_6037f0]
int __cdecl Timer_Calibrate(int a)
{
    return CallC<int>(FUN_603f30, a);
}

// The music player: +0 state (1 stopped, 2 paused, 3 running), +4 error,
// +8 the filter graph, +12 its IMediaControl.

// [FUN_603a00]
int __fastcall Music_StopGraph(BYTE* self, void*)
{
    int st = F(int, self, 0);
    if ((st == 3 || st == 2) && F(BYTE*, self, 12) && Com(F(BYTE*, self, 12), 36) >= 0) {
        F(int, self, 0) = 1;
        return 0;
    }
    return -1;
}

// [FUN_603810] polls the graph's state and events
int __fastcall Music_Poll(BYTE* self, void*)
{
    if (F(int, self, 0) != 3)
        return F(int, self, 0);
    BYTE* control = F(BYTE*, self, 12);
    if (!control)
        return 3;
    int st = 0;
    if (Com(control, 40, 0, &st) < 0)
        return 0;
    if (st == 0)
        F(int, self, 0) = 1;
    else if (st == 1)
        F(int, self, 0) = 2;
    BYTE* events;
    if (Com(F(BYTE*, self, 8), 0, (const void*)g_iidMediaEvent, &events) < 0)
        return F(int, self, 0);
    int code, p;
    if (Com(events, 32, &code, &p, &code, 0) >= 0 && (code == 1 || code == 2 || code == 3))
        Music_StopGraph(self, 0);
    Release(events);
    return F(int, self, 0);
}

// [FUN_6039c0]
int __fastcall Music_Run(BYTE* self, void*, int unused)
{
    int st = F(int, self, 0);
    if ((st == 1 || st == 2) && F(BYTE*, self, 12)) {
        if (Com(F(BYTE*, self, 12), 28) >= 0) {
            F(int, self, 0) = 3;
            return 0;
        }
        F(int, self, 4) = 3;
    }
    return -1;
}

// [FUN_603a90] 0..1 to hundredths of decibels
int __cdecl Music_VolumeToDb(double v)
{
    if (!(v > 0.0))
        return -10000;
    if (v > 1.0)
        return 0;
    return (int)(x87_log10(v) * 2000.0);
}

// [FUN_603a30]
int __fastcall Music_SetVolume(BYTE* self, void*, double v)
{
    BYTE* graph = F(BYTE*, self, 8);
    BYTE* audio;
    if (!graph || Com(graph, 0, (const void*)g_iidBasicAudio, &audio) < 0)
        return -1;
    int hr = Com(audio, 28, Music_VolumeToDb(v));
    Release(audio);
    return hr < 0 ? -1 : 0;
}

// [FUN_603ad0]
int __fastcall Music_QueryControl(BYTE* self, void*)
{
    BYTE* graph = F(BYTE*, self, 8);
    if (!graph)
        return 0;
    return Com(graph, 0, (const void*)g_iidMediaControl, self + 12);
}

// [FUN_603af0]
void __fastcall Music_ReleaseControl(BYTE* self, void*)
{
    if (F(BYTE*, self, 12))
        Release(F(BYTE*, self, 12));
    F(BYTE*, self, 12) = 0;
}

// [FUN_603b10]
BYTE* __fastcall Music_Construct(BYTE* self, void*)
{
    F(int, self, 0) = 0;
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    CoInitialize(0);
    return self;
}

// [FUN_603b30]
void __fastcall Music_Close(BYTE* self, void*)
{
    Music_ReleaseControl(self, 0);
    if (F(BYTE*, self, 8))
        Release(F(BYTE*, self, 8));
    F(int, self, 8) = 0;
    F(int, self, 16) = 0;
    F(int, self, 0) = 0;
}

// .ini values; "N" (or an empty value) means missing: then the default.

static int Ini_Get(const char* section, const char* key, const char* file, char* buf)
{
    GetPrivateProfileStringA(section, key, s_N, buf, 96, file);
    return strlen(buf) && strncmp(buf, s_N, 1);
}

// [FUN_603b70]
int __cdecl Ini_ReadInt(const char* section, const char* key, int def, int* out, const char* file)
{
    char buf[96];
    if (Ini_Get(section, key, file, buf))
        *out = strtol(buf, 0, 0);
    else
        *out = def;
    return 0;
}

// [FUN_603bf0]
int __cdecl Ini_ReadShort(const char* section, const char* key, short def, short* out, const char* file)
{
    char buf[96];
    if (Ini_Get(section, key, file, buf))
        *out = (short)atol(buf);
    else
        *out = def;
    return 0;
}

// [FUN_603c70]
BOOL __cdecl Ini_WriteInt(const char* section, const char* key, long v, const char* file)
{
    char buf[96];
    sprintf(buf, s_ld, v);
    return WritePrivateProfileStringA(section, key, buf, file);
}

}
