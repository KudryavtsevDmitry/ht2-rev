// sound_gate.cpp - D1gate.cpp of the Gi\Srmidp library, rewritten from
// king.masm: the DirectPlay network gate (service providers, sessions,
// message records with a GUID, data and name), the byte stream of network
// messages ('*' float, '-' int, 'x' blob fields), the DirectShow music
// player (state machine over IMediaControl/IMediaEvent/IBasicAudio), and
// the .ini helpers of the engine.

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

extern "C" void __cdecl Providers_Rebuild();
extern "C" void FUN_6016f0(); KING_ALIAS("_FUN_6016f0", "_Providers_Rebuild")    // rewinds the service provider list
extern "C" int __cdecl Gate_600b50(int a);
extern "C" void FUN_600b50(); KING_ALIAS("_FUN_600b50", "_Gate_600b50")    // DirectPlay error text (hr)
extern "C" void __cdecl Ai_Report(int a1, int level, int a3, int a4, int a5, int a6, int a7, int a8);
extern "C" int __cdecl Gate_Send(DWORD to, const BYTE* buf, int size);
extern "C" void FUN_601ff0(); KING_ALIAS("_FUN_601ff0", "_Gate_Send")
extern "C" int __cdecl Gate_Players(DWORD* out, BYTE** list);
extern "C" void FUN_602320(); KING_ALIAS("_FUN_602320", "_Gate_Players")
extern "C" int __cdecl Tsc_Calibrate(int);
typedef void (*VFn)();
extern "C" void __fastcall Gate_FreeBuffers(BYTE* self, int);
extern "C" void FUN_601470(); KING_ALIAS("_FUN_601470", "@Gate_FreeBuffers@8")
extern "C" void FUN_601c70(); KING_ALIAS("_FUN_601c70", "_Dp_Connect")
extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
extern "C" void __cdecl Ai_PoolFree(void* p, unsigned size);
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")
extern "C" void __fastcall CharVec_InsertRange(BYTE** self, void*, BYTE* pos, BYTE* first, BYTE* last);
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
extern "C" void FUN_46ecd0(); KING_ALIAS("_FUN_46ecd0", "_CopyBytes")
KING_VAR(DWORD*, g_providerIds, 723de8)
KING_VAR(AsmLabel, g_683ca0, 683ca0)
extern "C" AsmLabel g_enumProvider; KING_ALIAS("_g_enumProvider", "_Sg_EnumProvider@24")
KING_VAR(AsmLabel, g_vtbl652de0, 652de0)
KING_VAR(AsmLabel, g_clsidFilterGraph, 6554c0)
KING_VAR(AsmLabel, g_iidGraphBuilder, 6554d0)

KING_VAR(AsmLabel, g_guidIpx, 655340)
KING_VAR(AsmLabel, g_guidTcp, 655330)
KING_VAR(AsmLabel, g_guidSerial, 655320)
KING_VAR(AsmLabel, g_guidModem, 655310)
KING_VAR(AsmLabel, s_ipx, 68427c)
KING_VAR(AsmLabel, s_tcp, 684274)
KING_VAR(AsmLabel, s_serial, 684268)
KING_VAR(AsmLabel, s_modem, 684260)
KING_VAR(BYTE*, g_providersBegin, 723d60)     // vector of 32-byte records
KING_VAR(BYTE*, g_providersEnd, 723d64)
KING_VAR(DWORD, g_providerNext, 723dec)
KING_VAR(BYTE*, g_directPlay, 723ddc)          // IDirectPlay4
KING_VAR(DWORD, g_dpPlayer, 723de0)
KING_VAR(BYTE, g_comInitialized, 723df0)
KING_VAR(AsmLabel, g_clsidDirectPlay, 655350)
KING_VAR(AsmLabel, g_iidDirectPlay4, 655360)
KING_VAR(AsmLabel, g_68d1c0, 68d1c0)
KING_VAR(AsmLabel, s_noDirectPlay, 684280)
KING_VAR(AsmLabel, s_d1gateId, 683c78)
KING_VAR(AsmLabel, s_d1gateFile, 68422c)
KING_VAR(AsmLabel, g_dpMessages, 723d90)
KING_VAR(AsmLabel, g_playerName, 723df8)
KING_VAR(AsmLabel, g_playerName2, 723f88)
KING_VAR(DWORD, g_723d88, 723d88)
KING_VAR(AsmLabel, g_vtbl652dc8, 652dc8)
KING_VAR(AsmLabel, g_iidMediaEvent, 6554b0)
KING_VAR(AsmLabel, g_iidBasicAudio, 6554e0)
KING_VAR(AsmLabel, g_iidMediaControl, 6554f0)
KING_VAR(AsmLabel, s_N, 6761e8)                // "N", the missing value
KING_VAR(AsmLabel, s_ld, 6843ac)               // "%ld"
KING_VAR(AsmLabel, s_g, 671350)                // "%g"

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
    Providers_Rebuild();
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
        int text = Gate_600b50(hr);
        Ai_Report(2, 100, (int)((const char*)s_d1gateFile), (int)((const char*)s_d1gateId), 647, (int)((const char*)s_noDirectPlay), text, (int)((void*)g_68d1c0));
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
    return Gate_Send(0, (const BYTE*)a, b);
}

// [FUN_6021d0]
int __cdecl Dp_SendTo(int to, int a, int b)
{
    return Gate_Send(to, (const BYTE*)a, b);
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
    return Gate_Players((DWORD*)a, (BYTE**)((void*)g_dpMessages));
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
    return Tsc_Calibrate(a);
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

// --- message buffers (a vector<char> at +4), the provider list, music files

static void Buf_Write(BYTE* v, const void* p, int n)
{
    CharVec_InsertRange((BYTE**)v, 0, F(BYTE*, v, 4), (BYTE*)((const BYTE*)p), (BYTE*)((const BYTE*)p + n));
}

extern "C" {

// [FUN_6016f0] rebuilds the service provider list and its id table
void __cdecl Providers_Rebuild()
{
    if (g_providerIds)
        operator delete(g_providerIds);
    g_providerIds = 0;
    for (BYTE* e = g_providersBegin; e != g_providersEnd; e += 32)
        Gate_FreeBuffers(e, 0);
    g_providersEnd = g_providersBegin;
    Dp_Create();
    BYTE* dp = g_directPlay;
    CallStd(F(VFn, F(BYTE*, dp, 0), 140), dp, (void*)g_683ca0, (void*)g_enumProvider, 0, 0);
    int n = (g_providersEnd - g_providersBegin) >> 5;
    DWORD* ids = (DWORD*)operator new(n * 4 + 4);
    g_providerIds = ids;
    int i = 0;
    for (BYTE* e = g_providersBegin; e != g_providersEnd; e += 32)
        ids[i++] = F(DWORD, e, 24);
    ids[i] = 0;
}

// [FUN_601c10] calls [FUN_601c70] with provider i (-1 when out of range)
int __cdecl Providers_Call601c70(unsigned i, int a, int b, int c)
{
    if ((unsigned)((g_providersEnd - g_providersBegin) >> 5) <= i)
        return -1;
    struct Guid16 { DWORD d[4]; } g;
    memcpy(&g, g_providersBegin + i * 32, 16);
    return CallC<int>(FUN_601c70, g, a, b, c);
}

// [FUN_6026f0] a 16-byte list node holding *x
BYTE* __stdcall DpList_MakeNode(const DWORD* x)
{
    BYTE* n = (BYTE*)Stl_Allocate(16);
    if (n + 8)
        F(DWORD, n, 8) = *x;
    return n;
}

// [FUN_6026c0] inserts a node holding *x before pos; *out = the node
BYTE** __stdcall DpList_Insert(BYTE** out, BYTE* pos, const DWORD* x)
{
    BYTE* n = DpList_MakeNode(x);
    F(BYTE*, n, 0) = pos;
    F(BYTE*, n, 4) = F(BYTE*, pos, 4);
    F(BYTE*, F(BYTE*, pos, 4), 0) = n;
    F(BYTE*, pos, 4) = n;
    *out = n;
    return out;
}

// [FUN_602be0] a message: an empty buffer with the 8-byte header written
BYTE* __fastcall DpMessage_Construct(BYTE* self, void*)
{
    F(int, self, 4) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 20) = 0;
    F(int, self, 16) = 0;
    F(int, self, 24) = 0;
    F(void*, self, 0) = g_vtbl652de0;
    DWORD v = 8;
    Buf_Write(self + 4, &v, 4);
    v = 0;
    Buf_Write(self + 4, &v, 4);
    F(int, self, 24) = 8;
    return self;
}

// [FUN_602df0] empties the message and writes the header again
void __fastcall DpMessage_Reset(BYTE* self, void*)
{
    F(BYTE*, self, 8) = F(BYTE*, self, 4);
    F(int, self, 20) = 0;
    F(int, self, 16) = 0;
    F(int, self, 24) = 0;
    DWORD v = 8;
    Buf_Write(self + 4, &v, 4);
    v = 0;
    Buf_Write(self + 4, &v, 4);
    F(int, self, 24) = 8;
}

// [FUN_603170] replaces the message with n received bytes
void __fastcall DpMessage_Set(BYTE* self, void*, const BYTE* p, int n)
{
    BYTE* b = F(BYTE*, self, 4);
    BYTE* e = F(BYTE*, self, 8);
    Bytes_Copy(e, e, b);
    F(BYTE*, self, 8) = e + (b - e);
    F(int, self, 20) = 0;
    F(int, self, 24) = 0;
    Buf_Write(self + 4, p, n);
    F(int, self, 16) = CallVirt<int>(self, 0);
    F(int, self, 24) = 8;
    F(int, self, 16) = F(int, F(BYTE*, self, 4), 4);
}

// [FUN_603570] appends the 4 bytes of v to the buffer
int __cdecl Buf_WriteDword(int unused, BYTE* v, DWORD x)
{
    return CallThis<int>(FUN_46e460, v, F(BYTE*, v, 4), (const BYTE*)&x, (const BYTE*)&x + 4);
}

// vector<char>::insert(pos, x)
static void Bytes_InsertAt(BYTE** self, BYTE* pos, const BYTE* x)
{
    BYTE* last = self[1];
    if (last != self[2]) {
        if (last)
            *last = last[-1];
        self[1] = last + 1;
        BYTE v = *x;
        for (BYTE* p = last - 1; p > pos; p--)
            *p = p[-1];
        *pos = v;
        return;
    }
    unsigned n = last - self[0];
    unsigned cap = n ? n * 2 : 1;
    BYTE* p = (BYTE*)Stl_Allocate(cap);
    BYTE* q = CallC<BYTE*>(FUN_46ecd0, self[0], pos, p, 0);
    if (q)
        *q = *x;
    BYTE* e = CallC<BYTE*>(FUN_46ecd0, pos, self[1], q + 1, 0);
    if (self[2] != self[0])
        Ai_PoolFree(self[0], self[2] - self[0]);
    self[0] = p;
    self[1] = e;
    self[2] = p + cap;
}

// [FUN_6035c0]
void __fastcall Bytes_Insert(BYTE** self, void*, BYTE* pos, const BYTE* x)
{
    Bytes_InsertAt(self, pos, x);
}

// [FUN_603590] appends the byte c to the buffer
void __cdecl Buf_WriteByte(int unused, BYTE** v, BYTE c)
{
    BYTE* e = v[1];
    if (e != v[2]) {
        if (e)
            *e = c;
        v[1]++;
        return;
    }
    Bytes_InsertAt(v, e, &c);
}

// [FUN_6038d0] opens a music file with DirectShow (-1 on failure)
int __fastcall Music_Open(BYTE* self, void*, const char* path)
{
    Music_Close(self, 0);
    IUnknown** graph = (IUnknown**)(self + 8);
    if (CoCreateInstance(*(const CLSID*)g_clsidFilterGraph, 0, 1, *(const IID*)g_iidGraphBuilder, (void**)graph) < 0)
        return -1;
    IUnknown* ev;
    HRESULT r = (*graph)->QueryInterface(*(const IID*)g_iidMediaEvent, (void**)&ev);
    if (r >= 0) {
        r = CallStd<HRESULT>(F(VFn, F(BYTE*, ev, 0), 28), ev, self + 16);
        ev->Release();
    }
    if (r < 0) {
        (*graph)->Release();
        *graph = 0;
        return -1;
    }
    Music_QueryControl(self, 0);
    WCHAR wpath[260];
    MultiByteToWideChar(0, 0, path, -1, wpath, 260);
    IUnknown* g = *graph;
    if (CallStd<HRESULT>(F(VFn, F(BYTE*, g, 0), 52), g, wpath, 0) < 0) {
        F(int, self, 4) = 2;
        Music_Close(self, 0);
        return -1;
    }
    F(int, self, 0) = 1;
    return 0;
}

}

extern "C" void** __fastcall Vec_Construct(void** self, void*, int);
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")    // vector<char> constructor
extern "C" void __cdecl Net_FillSendBuffer(BYTE** v, const BYTE* data, int size);
extern "C" void FUN_600e60(); KING_ALIAS("_FUN_600e60", "_Net_FillSendBuffer")    // appends bytes to a vector<char> (vector, data, size)
extern "C" void Gate_SendBufferDtor(); KING_ALIAS("_Gate_SendBufferDtor", "_Static723d98_Destroy")
extern "C" int __cdecl atexit(void (__cdecl* fn)());
KING_VAR(BYTE, g_sendBufInit, 723d8c)
typedef BYTE* SendBuf[3];
KING_VAR(SendBuf, g_sendBuf, 723d98)
KING_VAR(int, g_sendRetries, 683cb8)

static int Dp_SendEx(DWORD to, BYTE* data, int size)
{
    typedef int (__stdcall* Fn)(BYTE*, DWORD, DWORD, DWORD, BYTE*, int, int, int, int, int);
    return (*(Fn*)(*(BYTE**)g_directPlay + 196))(g_directPlay, g_dpPlayer, to, 1537, data, size, 0, 0, 0, 0);
}

extern "C" {

// [FUN_601ff0] sends size bytes of buf to player to (guaranteed, async),
// retrying while DirectPlay is busy
int __cdecl Gate_Send(DWORD to, const BYTE* buf, int size)
{
    if (!buf)
        Ai_Report(2341, 100, (int)((const char*)s_d1gateFile), (int)((const char*)s_d1gateId), 1075, (int)("Invalid buffer"), (int)((void*)g_68d1c0), (int)((void*)g_68d1c0));
    if (!size)
        Ai_Report(2351, 100, (int)((const char*)s_d1gateFile), (int)((const char*)s_d1gateId), 1079, (int)("Invalid buffer size"), (int)((void*)g_68d1c0), (int)((void*)g_68d1c0));
    if (!(g_sendBufInit & 1)) {
        g_sendBufInit |= 1;
        char dummy;
        Vec_Construct((void**)g_sendBuf, 0, (int)&dummy);
        atexit((void (__cdecl*)())Gate_SendBufferDtor);
    }
    g_sendBuf[1] = g_sendBuf[0];
    Net_FillSendBuffer(g_sendBuf, buf, size);
    int hr = Dp_SendEx(to, g_sendBuf[0], g_sendBuf[1] - g_sendBuf[0]);
    for (int i = 0; hr == (int)0x8877000e && i < g_sendRetries; i++)
        hr = Dp_SendEx(to, g_sendBuf[0], g_sendBuf[1] - g_sendBuf[0]);
    if (hr != (int)0x8000000a && hr < 0) {
        int text = Gate_600b50(hr);
        Ai_Report(5, 100, (int)((const char*)s_d1gateFile), (int)((const char*)s_d1gateId), 1102, (int)("Cannot send message"), text, (int)((void*)g_68d1c0));
    }
    return 0;
}

}

extern "C" const char* __cdecl DxError_Name(DWORD c);
extern "C" void FUN_600660(); KING_ALIAS("_FUN_600660", "_DxError_Name")
extern "C" void FUN_602b60(); KING_ALIAS("_FUN_602b60", "@Blob_Set@16")
extern "C" void FUN_603b30(); KING_ALIAS("_FUN_603b30", "@Music_Close@8")
extern "C" BYTE** __fastcall Sg_ListConstruct(BYTE** self);
extern "C" void $L_602410(); KING_ALIAS("_$L_602410", "@Sg_ListConstruct@4")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")
KING_VAR(char, g_723d90, 723d90)
KING_VAR(char, g_723d60, 723d60)

// [FUN_600b50] forwards to FUN_600660
extern "C" int __cdecl Gate_600b50(int a)
{
    return (int)DxError_Name(a);
}

// [FUN_602bd0] empties blob s: Blob_Set(s, 0, 0)
extern "C" void __fastcall Gate_602bd0(BYTE* s, int)
{
    Blob_Set(s, 0, 0, 0);
}

// [FUN_6032f0] sets +24 to 8, returns 0
extern "C" int __fastcall Gate_Set24(BYTE* self, int)
{
    *(int*)(self + 24) = 8;
    return 0;
}

// [FUN_6023e0] destroys the global at 723d90
extern "C" void __cdecl Gate_Destroy723d90()
{
    Sg_ListConstruct((BYTE**)&g_723d90);
}

// [FUN_601530] constructs the global container at 723d60
extern "C" void __cdecl Gate_Init723d60()
{
    char alloc;
    Vec_Construct((void**)&g_723d60, 0, (int)&alloc);
}

// [FUN_601470] frees the buffers at +16 and +24
extern "C" void __fastcall Gate_FreeBuffers(BYTE* self, int)
{
    void* p = *(void**)(self + 16);
    if (p)
        operator delete(p);
    p = *(void**)(self + 24);
    if (p)
        operator delete(p);
}

// [FUN_603b60] shuts the music player down and uninitializes COM
extern "C" void __cdecl Gate_ShutdownCom()
{
    CallC(FUN_603b30);
    CoUninitialize();
}

// [FUN_6033f0] reads a type-7 message: id, a byte and six floats; 3 for another type
extern "C" int __fastcall Msg_ReadType7(BYTE* self, int, int* id, float* f, int* b)
{
    BYTE* m = F(BYTE*, self, 4) + F(int, self, 24);
    char type = m[0];
    *id = *(int*)(m + 1);
    if (type != 7)
        return 3;
    *b = (signed char)(F(BYTE*, self, 4)[F(int, self, 24) + 5]);
    for (int i = 0; i < 6; i++)
        f[i] = *(float*)(F(BYTE*, self, 4) + i * 4 + F(int, self, 24) + 6);
    return 0;
}

// the message header: size of the buffer at +0, message number at +4
static void Msg_Finish(BYTE* self)
{
    BYTE** v = (BYTE**)(self + 4);
    *(int*)v[0] = (int)(v[1] - v[0]);
    int n = ++F(int, self, 16);
    *(int*)(v[0] + 4) = n;
}

// [FUN_602fd0] writes message 7: id, a byte and six floats
extern "C" void __fastcall Msg_Write7(BYTE* self, int, int id, const float* f, BYTE b)
{
    BYTE** v = (BYTE**)(self + 4);
    Buf_WriteByte(0, v, 7);
    Buf_WriteDword(0, (BYTE*)v, id);
    Buf_WriteByte(0, v, b);
    for (int i = 0; i < 6; i++) {
        float x = f[i];
        CharVec_InsertRange(v, 0, v[1], (BYTE*)&x, (BYTE*)&x + 4);
    }
    Msg_Finish(self);
}

// [FUN_603050] writes message 42: id and a value as float
extern "C" void __fastcall Msg_Write42(BYTE* self, int, int id, double d)
{
    BYTE** v = (BYTE**)(self + 4);
    Buf_WriteByte(0, v, 42);
    Buf_WriteDword(0, (BYTE*)v, id);
    float x = (float)d;
    CharVec_InsertRange(v, 0, v[1], (BYTE*)&x, (BYTE*)&x + 4);
    Msg_Finish(self);
}

// [FUN_6030b0] writes message 45: id and a dword
extern "C" void __fastcall Msg_Write45(BYTE* self, int, int id, DWORD x)
{
    BYTE** v = (BYTE**)(self + 4);
    Buf_WriteByte(0, v, 45);
    Buf_WriteDword(0, (BYTE*)v, id);
    CharVec_InsertRange(v, 0, v[1], (BYTE*)&x, (BYTE*)&x + 4);
    Msg_Finish(self);
}

// [FUN_603110] writes message 120: id, a length and that many bytes
extern "C" void __fastcall Msg_Write120(BYTE* self, int, int id, const BYTE* p, int n)
{
    BYTE** v = (BYTE**)(self + 4);
    Buf_WriteByte(0, v, 120);
    Buf_WriteDword(0, (BYTE*)v, id);
    Buf_WriteDword(0, (BYTE*)v, n);
    CharVec_InsertRange(v, 0, v[1], (BYTE*)p, (BYTE*)(p + n));
    Msg_Finish(self);
}

extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
KING_VAR(char, vt_652de0, 652de0)

// [FUN_602ca0] frees the byte buffer at +4 (capacity at +12)
extern "C" void __fastcall MsgBuf_FreeStorage(BYTE* self, int)
{
    BYTE* b = F(BYTE*, self, 4);
    int n = F(BYTE*, self, 12) - b;
    if (n)
        Ai_PoolFree(b, n);
}

// [FUN_602e80] destructor (vtable 652de0): frees the byte buffer at +4
extern "C" void __fastcall MsgBuf_Destroy(BYTE* self, int)
{
    *(void**)self = &vt_652de0;
    MsgBuf_FreeStorage(self, 0);
}

// ---------------------------------------------------------------------------
extern "C" void FUN_600b50(); KING_ALIAS("_FUN_600b50", "_Gate_600b50")    // an error text
extern "C" void FUN_6016f0(); KING_ALIAS("_FUN_6016f0", "_Providers_Rebuild")
KING_VAR(BYTE, g_sg723df0, 723df0)
KING_VAR(AsmLabel, g_sg683ca0, 683ca0)
KING_VAR(AsmLabel, s_sg68d1c0, 68d1c0)
KING_VAR(AsmLabel, s_sg684320, 684320)
KING_VAR(AsmLabel, s_sg683c78, 683c78)
KING_VAR(AsmLabel, s_sg68422c, 68422c)

// [FUN_6021f0] starts the sound gate once with a name (up to 16 chars); -1 when COM fails
extern "C" int __cdecl Gate_Start(const char* name)
{
    if (g_sg723df0)
        return 0;
    HRESULT hr = CoInitialize(0);
    if (hr < 0)
    {
        Ai_Report(6, 100, (int)((char*)s_sg68422c), (int)((char*)s_sg683c78), 1134, (int)((char*)s_sg684320), (int)((const char*)Gate_600b50(hr)), (int)((char*)s_sg68d1c0));
        return -1;
    }
    if (name && strlen(name))
    {
        unsigned n = strlen(name);
        strncpy(g_sg683ca0, name, n < 16 ? n : 16);
    }
    Providers_Rebuild();
    g_sg723df0 = 1;
    return 0;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

KING_VAR(DWORD, g_sougat723d88, 723d88)

// [$L_601040]
extern "C" void __cdecl SougatInit_601040() { g_sougat723d88 = 0x0; }

extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")

// a byte stream over a char vector (+4) with a position (+8)
// [$L_602c80]
extern "C" int __fastcall Stream_Begin(BYTE* s) { return F(int*, s, 4)[0]; }
// [$L_602c90]
extern "C" int __fastcall Stream_End(BYTE* s) { return F(int*, s, 4)[1]; }

// [$L_600be0] writes n bytes of p at the position
extern "C" int __fastcall Stream_Write(BYTE* s, void*, BYTE* p, int n)
{
    BYTE* v = F(BYTE*, s, 4);
    CharVec_InsertRange((BYTE**)v, 0, F(BYTE*, v, 0) + F(int, s, 8), p, p + n);
    F(int, s, 8) += n;
    return n;
}

// [$L_600c10] seeks: 0 from the start, 1 from the position, 2 back from the end
extern "C" int __fastcall Stream_Seek(BYTE* s, void*, int off, int whence)
{
    if (whence == 1)
        return F(int, s, 8) += off;
    if (whence == 2) {
        BYTE* v = F(BYTE*, s, 4);
        return F(int, s, 8) = F(int, v, 4) - F(int, v, 0) - off;
    }
    if (whence == 0)
        return F(int, s, 8) = off;
    return -1;
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

KING_VAR(BYTE*, g_q51_723ddc, 723ddc)
KING_VAR(int, g_q51_723df4, 723df4)
typedef long(__stdcall* ComFn96)(void*, int, int);
// [$L_601800] unless c has bit 0: asks the 723ddc object (slot 96) for (a, 1); 1 on failure, else sets 723df4
extern "C" int __stdcall Gate_Enum601800(int a, int, int c, int)
{
    if (c & 1)
        return 0;
    BYTE* o = g_q51_723ddc;
    if (((ComFn96)(*(void***)o)[24])(o, a, 1) < 0)
        return 1;
    g_q51_723df4 = 1;
    return 0;
}
// [$L_601840] appends the 16-byte record r to the list (count at +0, at most 10 at +4); always 1
extern "C" int __stdcall Gate_AddRecord(const DWORD* r, DWORD* list, int)
{
    if (list[0] < 10) {
        DWORD* d = list + 1 + list[0] * 4;
        d[0] = r[0];
        d[1] = r[1];
        d[2] = r[2];
        d[3] = r[3];
        list[0]++;
    }
    return 1;
}

// [$L_600b90] reads up to n bytes from memory stream s (buffer +4: begin, end; position +8) into dst; the new
// position
extern "C" unsigned __fastcall MemStream_Read(BYTE* s, void*, void* dst, unsigned n)
{
    BYTE** b = F(BYTE**, s, 4);
    unsigned avail = (unsigned)(b[1] - b[0]) - F(unsigned, s, 8);
    if (n > avail)
        n = avail;
    memcpy(dst, b[0] + F(unsigned, s, 8), n);
    return F(unsigned, s, 8) += n;
}

extern "C" void __fastcall Providers_Destroy(BYTE** v, void*);
extern "C" void $L_602550(); KING_ALIAS("_$L_602550", "@Providers_Destroy@8")
extern "C" void __fastcall List16_Destroy(BYTE** s);
// [$L_601560] destroys the global container 723d60
extern "C" void __cdecl Gate_Destroy723d60(void) { Providers_Destroy((BYTE**)&g_723d60, 0); }
// [$L_601550] destroys it at exit
extern "C" int __cdecl Gate_723d60AtExit(void) { return atexit(Gate_Destroy723d60); }
// [$L_601520] the static initializer of 723d60
extern "C" int __cdecl Gate_723d60Init(void)
{
    Gate_Init723d60();
    return Gate_723d60AtExit();
}
// [$L_602400] the final destructor of 723d90
extern "C" void __cdecl Gate_Free723d90(void) { List16_Destroy((BYTE**)&g_723d90); }
// [$L_6023f0] runs it at exit
extern "C" int __cdecl Gate_723d90AtExit(void) { return atexit(Gate_Free723d90); }
// [$L_6023d0] the static initializer of 723d90
extern "C" int __cdecl Gate_723d90Init(void)
{
    Gate_Destroy723d90();
    return Gate_723d90AtExit();
}

typedef void* SgFreeLists[17];
KING_VAR(SgFreeLists, g_q196_68d1c0, 68d1c0)
KING_VAR(volatile LONG, g_q196_68d204, 68d204)
extern "C" void __fastcall Stl_Lock(volatile LONG* l, void*);
extern "C" void __cdecl free(void*);
// [$L_602550] destroys the vector of 32-byte provider records (each one's buffers freed) and returns its storage
// to the STL allocator
extern "C" void __fastcall Providers_Destroy(BYTE** v, void*)
{
    for (BYTE* e = v[0]; e != v[1]; e += 32)
        Gate_FreeBuffers(e, 0);
    BYTE* p = v[0];
    int n = (int)(v[2] - p) >> 5;
    if (!n)
        return;
    unsigned bytes = n << 5;
    if (bytes > 128) {
        free(p);
        return;
    }
    void** list = &g_q196_68d1c0[(bytes + 7) >> 3];
    Stl_Lock(&g_q196_68d204, 0);
    *(void**)p = *list;
    *list = p;
    g_q196_68d204 = 0;
}

// ---------------------------------------------------------------------------
// Thunks.

KING_VAR(char, g_q238_723d98, 723d98)
extern "C" int __fastcall q238_VecB_FreeStorage(BYTE* s, void* d); KING_ALIAS("@q238_VecB_FreeStorage@8", "@VecB_FreeStorage@8")
// [$L_6021a0] VecB_FreeStorage on the static object 723d98
extern "C" void __cdecl Static723d98_Destroy(void) { q238_VecB_FreeStorage((BYTE*)&g_q238_723d98, 0); }

// === a received network block (FUN_600c60)

extern "C" BYTE* __stdcall Bytes_Clone(unsigned n, const BYTE* first, const BYTE* last);
extern "C" BYTE* __cdecl Gz_MemStreamNew(BYTE* data, unsigned size);
extern "C" int __cdecl Gz_Decompress(void* in, void* out);
extern "C" int __cdecl Gz_StreamClose(BYTE** p);

// empties byte vector v, keeping room for at least 2048 bytes
static void Rb_Reset(BYTE** v)
{
    v[1] = v[0];
    if ((unsigned)(v[2] - v[0]) < 2048) {
        int size = (int)(v[1] - v[0]);
        BYTE* p = Bytes_Clone(2048, v[0], v[1]);
        if (int bytes = (int)(v[2] - v[0]))
            Ai_PoolFree(v[0], bytes);
        v[0] = p;
        v[1] = p + size;
        v[2] = p + 2048;
    }
}

extern "C" {

// [FUN_600c60] network block b (n bytes) for the receive buffer v: a CPSD block is a script call (its value and a
// new handler for v), anything else is copied into v
void __cdecl Net_OnBlock(BYTE** v, const BYTE* b, int n)
{
    bool call = !strncmp((const char*)b, "CPSD", 4);
    Rb_Reset(v);
    if (call) {
        void* r = (void*)Gz_MemStreamNew((BYTE*)(b + 4), n - 4);
        BYTE* h = (BYTE*)operator new(12);
        void* hv = h ? DpHandler_Construct(h, 0, (int)v) : 0;
        Gz_Decompress(r, hv);
        Gz_StreamClose((BYTE**)&r);
        Gz_StreamClose((BYTE**)&hv);
    } else
        CharVec_InsertRange(v, 0, v[1], (BYTE*)b, (BYTE*)(b + n));
}

} // extern "C"

// === receiving the next network message (FUN_601060)

extern "C" double __cdecl Tsc_Now();
KING_VAR(BYTE, g_rm723d48, 723d48)
KING_VAR(AsmLabel, g_rm723d70, 723d70)    // the receive buffer (a byte vector)
extern "C" AsmLabel g_rm601250; KING_ALIAS("_g_rm601250", "_Sg_Exit601250")    // its destructor
KING_VAR(AsmLabel, s_rm684250, 684250)
extern "C" int __cdecl atexit(void (__cdecl*)(void));

// fills message m: kind, sender, the time and the data
static void Rm_Set(BYTE* m, int kind, DWORD from, const void* p, DWORD n)
{
    F(DWORD, m, 12) = from;
    F(double, m, 16) = Tsc_Now();
    F(int, m, 0) = kind;
    Blob_Set(m, 0, p, n);
}

extern "C" {

// [FUN_601060] receives the next DirectPlay message into m: a player joining (1) or leaving (2), or another
// player's data (4, through Net_OnBlock); -1 when there is none, 0 otherwise (errors reported)
int __cdecl Net_Receive(BYTE* m)
{
    if (!g_directPlay)
        return -1;
    BYTE* buf = 0;
    DWORD size = 0;
    for (;;) {
        DWORD from = 0, to = 0;
        int hr = Com(g_directPlay, 100, &from, &to, 1, buf, &size);
        if (hr == (int)0x887700BE) {
            operator delete(buf);
            return -1;
        }
        if (hr == (int)0x8877001E) {
            operator delete(buf);
            buf = (BYTE*)operator new(size);
            continue;
        }
        if (hr < 0) {
            Ai_Report(1, 100, (int)((const char*)s_d1gateFile), (int)((const char*)s_d1gateId), 513, (int)((const char*)s_rm684250), Gate_600b50(hr), (int)((void*)g_68d1c0));
            break;
        }
        if (!from) {
            int t = F(int, buf, 0);
            if ((t == 3 || t == 5) && F(int, buf, 4) == 1) {
                Rm_Set(m, t == 3 ? 1 : 2, F(DWORD, buf, 8), 0, 0);
                break;
            }
            continue;
        }
        BYTE** v = (BYTE**)g_rm723d70;
        if (!(g_rm723d48 & 1)) {
            g_rm723d48 |= 1;
            char alloc;
            Vec_Construct((void**)v, 0, (int)&alloc);
            atexit((void (__cdecl*)(void))(void*)g_rm601250);
        }
        v[1] = v[0];
        Net_OnBlock(v, buf, size);
        Rm_Set(m, 4, from, v[0], (DWORD)(v[1] - v[0]));
        break;
    }
    operator delete(buf);
    return 0;
}

} // extern "C"

// === the lobby address (FUN_601890)

KING_VAR(AsmLabel, g_dpaidSP, 6552e0)        // DPAID_ServiceProvider
KING_VAR(AsmLabel, g_dpaidA, 6552d0)
KING_VAR(AsmLabel, g_dpaidB, 6552c0)
KING_VAR(AsmLabel, g_dpaidC, 6552b0)
KING_VAR(AsmLabel, g_dpaidPort, 6552a0)
KING_VAR(AsmLabel, s_lobbyDefault, 6842c8)
KING_VAR(AsmLabel, g_lobby723ec0, 723ec0)
KING_VAR(AsmLabel, g_lobbyPort, 724050)

struct Gate_Elem {
    BYTE  guid[16];
    DWORD size;
    void* data;
};

// one address element of type g for the string str
static void Gate_StrElem(Gate_Elem* e, const void* g, const char* str)
{
    memcpy(e->guid, g, 16);
    e->size = lstrlenA(str) + 1;
    e->data = (void*)str;
}

extern "C" {

// [FUN_601890] with lobby l builds the compound address of service provider sp from the address types it takes
// (the name, the two strings and the port; the default name when there is none and def): *out and *size the
// GlobalAlloc'ed address; 0 or the error
HRESULT __cdecl Gate_BuildAddress(BYTE* l, const BYTE* sp, void** out, DWORD* size, char def)
{
    DWORD types[41];
    memset(types, 0, sizeof types);
    HRESULT hr = CallVirt<HRESULT>(l, 24, (void*)Gate_AddRecord, sp, types, 0);
    if (hr < 0)
        return hr;
    Gate_Elem e[11];
    memcpy(e[0].guid, (BYTE*)g_dpaidSP, 16);
    e[0].size = 16;
    e[0].data = (void*)sp;
    DWORD n = 1, port;
    for (DWORD i = 0; i < types[0]; i++) {
        const BYTE* g = (const BYTE*)(types + 1 + i * 4);
        Gate_Elem* d = e + n;
        if (!memcmp(g, (BYTE*)g_dpaidA, 16)) {
            if (lstrlenA((char*)g_playerName) > 0)
                Gate_StrElem(d, (BYTE*)g_dpaidA, (char*)g_playerName);
            else if (def)
                Gate_StrElem(d, (BYTE*)g_dpaidA, (char*)s_lobbyDefault);
            else
                continue;
        } else if (!memcmp(g, (BYTE*)g_dpaidB, 16))
            Gate_StrElem(d, (BYTE*)g_dpaidB, (char*)g_lobby723ec0);
        else if (!memcmp(g, (BYTE*)g_dpaidC, 16))
            Gate_StrElem(d, (BYTE*)g_dpaidC, (char*)g_playerName2);
        else if (!memcmp(g, (BYTE*)g_dpaidPort, 16)) {
            port = atoi((char*)g_lobbyPort);
            if (!(WORD)port)
                continue;
            memcpy(d->guid, (BYTE*)g_dpaidPort, 16);
            d->size = 2;
            d->data = &port;
        } else
            continue;
        n++;
    }
    DWORD sz;
    hr = CallVirt<HRESULT>(l, 56, e, n, (void*)0, &sz);
    if (hr != (HRESULT)0x8877001E)
        return hr;
    void* buf = GlobalLock(GlobalAlloc(66, sz));
    if (!buf)
        return (HRESULT)0x8007000E;
    hr = CallVirt<HRESULT>(l, 56, e, n, buf, &sz);
    if (hr >= 0) {
        *out = buf;
        *size = sz;
        return 0;
    }
    GlobalUnlock(GlobalHandle(buf));
    GlobalFree(GlobalHandle(buf));
    return hr;
}

} // extern "C"

// === connecting to a session (FUN_601c70)

KING_VAR(BYTE, g_dpHosting, 723dd8)
KING_VAR(AsmLabel, g_clsidLobby, 6552f0)
KING_VAR(AsmLabel, g_iidLobby, 655300)
KING_VAR(AsmLabel, s_dpNoPlayer, 6842d0)
extern "C" void $L_601800(); KING_ALIAS("_$L_601800", "_Gate_Enum601800@16")    // the session enumeration callback

#define DP_CALL(o, off, ...) CallStd<int>(F(VFn, F(BYTE*, (o), 0), (off)), (o), __VA_ARGS__)
#define DP_CALL0(o, off) CallStd<int>(F(VFn, F(BYTE*, (o), 0), (off)), (o))

// closes and releases the DirectPlay object
static void Dp_Drop()
{
    DP_CALL0(g_directPlay, 16);
    DP_CALL0(g_directPlay, 8);
    g_directPlay = 0;
}

extern "C" {

// [FUN_601c70] connects through service provider sp (a GUID by value) and hosts (host: up to max players, session
// name) or joins the session found; 0, 1 when there is none to join, 3 when it fails to connect, -1 when no player
int __cdecl Dp_Connect(DWORD sp0, DWORD sp1, DWORD sp2, DWORD sp3, int host, int max, int name)
{
    char h = host != 0;
    g_dpHosting = h;
    Providers_Rebuild();
    DWORD sp[4] = {sp0, sp1, sp2, sp3};
    BYTE* e = g_providersBegin;
    while (e != g_providersEnd && memcmp(e, sp, 16))
        e += 32;
    if (e == g_providersEnd)
        return 3;
    Dp_Create();
    BYTE* lobby = 0;
    CoCreateInstance(*(const CLSID*)(BYTE*)g_clsidLobby, 0, 1, *(const IID*)(BYTE*)g_iidLobby, (void**)&lobby);
    void* addr = 0;
    DWORD size = 0;
    Gate_BuildAddress(lobby, (const BYTE*)&sp0, &addr, &size, h);
    DP_CALL0(lobby, 8);
    int hr = DP_CALL(g_directPlay, 152, addr, 0);
    if (hr == (int)0x88770005) {
        Dp_Drop();
        Dp_Create();
        hr = DP_CALL(g_directPlay, 152, addr, 0);
    }
    GlobalUnlock(GlobalHandle(addr));
    GlobalFree(GlobalHandle(addr));
    if (hr < 0) {
        Dp_Drop();
        return 3;
    }
    DWORD desc[20];
    memset(desc, 0, sizeof desc);
    desc[0] = 80;
    memcpy(desc + 2, (BYTE*)g_683ca0, 16);
    memcpy(desc + 6, (BYTE*)g_683ca0, 16);
    if (host) {
        desc[1] = 8260;
        desc[10] = max;
        desc[12] = name;
        do
            hr = DP_CALL(g_directPlay, 96, desc, 130);
        while (hr == (int)0x8877015E);
        if (hr < 0) {
            Dp_Drop();
            return 3;
        }
    } else {
        g_q51_723df4 = 0;
        do
            hr = DP_CALL(g_directPlay, 52, desc, 0, (void*)$L_601800, 0, 129);
        while (hr == (int)0x8877015E);
        if (hr < 0) {
            Dp_Drop();
            return 3;
        }
        if (!g_q51_723df4) {
            Dp_Drop();
            return 1;
        }
    }
    hr = DP_CALL(g_directPlay, 24, &g_dpPlayer, 0, 0, 0, 0, 0);
    g_723d88 = 0;
    if (hr < 0) {
        Dp_Drop();
        int text = CallC<int>(FUN_600b50, hr, (void*)g_68d1c0);
        Ai_Report(1, 100, (int)((const char*)s_d1gateFile), (int)((const char*)s_d1gateId), 1038, (int)((const char*)s_dpNoPlayer), text, (int)((void*)g_68d1c0));
        return -1;
    }
    g_723d88 = g_dpPlayer;
    return 0;
}

} // extern "C"

// === inserting a DirectPlay record (FUN_6027b0)

extern "C" BYTE* __cdecl DpRecords_UninitCopy(BYTE* first, BYTE* last, BYTE* dest, BYTE*);
extern "C" void FUN_602ad0(); KING_ALIAS("_FUN_602ad0", "_DpRecords_UninitCopy")    // uninitialized copy of records (first, last, dest, -)

extern "C" {

// [FUN_6027b0] vector<32-byte DirectPlay record> s: inserts a copy of x before pos, growing it to twice its size
void __fastcall DpRecords_InsertAux(BYTE** s, void*, BYTE* pos, const BYTE* x)
{
    if (s[1] != s[2]) {
        BYTE* end = s[1];
        if (end)
            DpRecord_Copy(end, 0, end - 32);
        s[1] += 32;
        BYTE tmp[32];
        DpRecord_Copy(tmp, 0, x);
        BYTE* src = s[1] - 64;
        BYTE* dst = s[1] - 32;
        for (int n = (int)(src - pos) >> 5; n > 0; n--) {
            src -= 32;
            dst -= 32;
            DpRecord_Assign(dst, 0, src);
        }
        DpRecord_Assign(pos, 0, tmp);
        Gate_FreeBuffers(tmp, 0);
        return;
    }
    unsigned n = (unsigned)(s[1] - s[0]) >> 5;
    unsigned cap = n ? n * 2 : 1;
    BYTE* buf = cap ? (BYTE*)Stl_Allocate(cap * 32) : 0;
    BYTE* p = DpRecords_UninitCopy(s[0], pos, buf, pos);
    if (p)
        DpRecord_Copy(p, 0, x);
    BYTE* q = DpRecords_UninitCopy(pos, s[1], p + 32, p);
    for (BYTE* e = s[0]; e != s[1]; e += 32)
        Gate_FreeBuffers(e, 0);
    if (unsigned bytes = (unsigned)(s[2] - s[0]) >> 5 << 5)
        Ai_PoolFree(s[0], bytes);
    s[0] = buf;
    s[1] = q;
    s[2] = buf + cap * 32;
}

} // extern "C"

// === DirectPlay / DirectSound error names (FUN_600660)

KING_VAR(char, g_dpe683ce4, 683ce4)
KING_VAR(char, g_dpe684208, 684208)
KING_VAR(char, g_dpe684218, 684218)
KING_VAR(char, g_dpe6841f4, 6841f4)
KING_VAR(char, g_dpe6841e4, 6841e4)
KING_VAR(char, g_dpe6841d0, 6841d0)
KING_VAR(char, g_dpe68418c, 68418c)
KING_VAR(char, g_dpe6841a0, 6841a0)
KING_VAR(char, g_dpe6841bc, 6841bc)
KING_VAR(char, g_dpe684178, 684178)
KING_VAR(char, g_dpe684160, 684160)
KING_VAR(char, g_dpe68414c, 68414c)
KING_VAR(char, g_dpe684134, 684134)
KING_VAR(char, g_dpe68411c, 68411c)
KING_VAR(char, g_dpe684104, 684104)
KING_VAR(char, g_dpe6840e8, 6840e8)
KING_VAR(char, g_dpe6840d8, 6840d8)
KING_VAR(char, g_dpe6840c4, 6840c4)
KING_VAR(char, g_dpe6840b0, 6840b0)
KING_VAR(char, g_dpe68409c, 68409c)
KING_VAR(char, g_dpe684088, 684088)
KING_VAR(char, g_dpe684078, 684078)
KING_VAR(char, g_dpe684064, 684064)
KING_VAR(char, g_dpe684050, 684050)
KING_VAR(char, g_dpe684038, 684038)
KING_VAR(char, g_dpe684028, 684028)
KING_VAR(char, g_dpe684014, 684014)
KING_VAR(char, g_dpe684000, 684000)
KING_VAR(char, g_dpe683ff0, 683ff0)
KING_VAR(char, g_dpe683fdc, 683fdc)
KING_VAR(char, g_dpe683fd0, 683fd0)
KING_VAR(char, g_dpe683fbc, 683fbc)
KING_VAR(char, g_dpe683fa0, 683fa0)
KING_VAR(char, g_dpe683f8c, 683f8c)
KING_VAR(char, g_dpe683f78, 683f78)
KING_VAR(char, g_dpe683f64, 683f64)
KING_VAR(char, g_dpe683f50, 683f50)
KING_VAR(char, g_dpe683f38, 683f38)
KING_VAR(char, g_dpe683f24, 683f24)
KING_VAR(char, g_dpe683f0c, 683f0c)
KING_VAR(char, g_dpe683ef4, 683ef4)
KING_VAR(char, g_dpe683ee0, 683ee0)
KING_VAR(char, g_dpe683ec8, 683ec8)
KING_VAR(char, g_dpe683e94, 683e94)
KING_VAR(char, g_dpe683ea8, 683ea8)
KING_VAR(char, g_dpe683eb8, 683eb8)
KING_VAR(char, g_dpe683e7c, 683e7c)
KING_VAR(char, g_dpe683e38, 683e38)
KING_VAR(char, g_dpe683e50, 683e50)
KING_VAR(char, g_dpe683e64, 683e64)
KING_VAR(char, g_dpe683e20, 683e20)
KING_VAR(char, g_dpe683dd4, 683dd4)
KING_VAR(char, g_dpe683df0, 683df0)
KING_VAR(char, g_dpe683e04, 683e04)
KING_VAR(char, g_dpe683dbc, 683dbc)
KING_VAR(char, g_dpe683d78, 683d78)
KING_VAR(char, g_dpe683d8c, 683d8c)
KING_VAR(char, g_dpe683da8, 683da8)
KING_VAR(char, g_dpe683d60, 683d60)
KING_VAR(char, g_dpe683d0c, 683d0c)
KING_VAR(char, g_dpe683d20, 683d20)
KING_VAR(char, g_dpe683d40, 683d40)
KING_VAR(char, g_dpe683cf8, 683cf8)
KING_VAR(char, g_dpe683cbc, 683cbc)
KING_VAR(char, g_dpe683cd0, 683cd0)
KING_VAR(char, g_dpeFmt683cec, 683cec)
KING_VAR(char, g_dpeBuf723d7c, 723d7c)

extern "C" {

// [FUN_600660] the name of DirectX result code c (DPERR_..., DSERR_..., S_OK), else "[0x%08X]" in a static buffer
const char* __cdecl DxError_Name(DWORD c)
{
    switch (c)
    {
    case 0x00000000: return &g_dpe683ce4;
    case 0x8000000a: return &g_dpe684208;
    case 0x80004001: return &g_dpe684218;
    case 0x80004002: return &g_dpe6841f4;
    case 0x80004005: return &g_dpe6841e4;
    case 0x8007000e: return &g_dpe6841d0;
    case 0x80070057: return &g_dpe68418c;
    case 0x88770005: return &g_dpe6841a0;
    case 0x8877000a: return &g_dpe6841bc;
    case 0x88770014: return &g_dpe684178;
    case 0x8877001e: return &g_dpe684160;
    case 0x88770028: return &g_dpe68414c;
    case 0x88770032: return &g_dpe684134;
    case 0x8877003c: return &g_dpe68411c;
    case 0x88770046: return &g_dpe684104;
    case 0x88770050: return &g_dpe6840e8;
    case 0x8877005a: return &g_dpe6840d8;
    case 0x88770078: return &g_dpe6840c4;
    case 0x88770082: return &g_dpe6840b0;
    case 0x88770096: return &g_dpe68409c;
    case 0x8877009b: return &g_dpe684088;
    case 0x887700a0: return &g_dpe684078;
    case 0x887700aa: return &g_dpe684064;
    case 0x887700be: return &g_dpe684050;
    case 0x887700c8: return &g_dpe684038;
    case 0x887700d2: return &g_dpe684028;
    case 0x887700dc: return &g_dpe684014;
    case 0x887700e6: return &g_dpe684000;
    case 0x887700f0: return &g_dpe683ff0;
    case 0x887700fa: return &g_dpe683fdc;
    case 0x8877010e: return &g_dpe683fd0;
    case 0x88770118: return &g_dpe683fbc;
    case 0x88770122: return &g_dpe683fa0;
    case 0x8877012c: return &g_dpe683f8c;
    case 0x88770136: return &g_dpe683f78;
    case 0x88770140: return &g_dpe683f64;
    case 0x8877014a: return &g_dpe683f50;
    case 0x88770154: return &g_dpe683f38;
    case 0x8877015e: return &g_dpe683f24;
    case 0x88770168: return &g_dpe683f0c;
    case 0x88770172: return &g_dpe683ef4;
    case 0x8877017c: return &g_dpe683ee0;
    case 0x88770186: return &g_dpe683ec8;
    case 0x88770190: return &g_dpe683e94;
    case 0x8877019a: return &g_dpe683ea8;
    case 0x887701a4: return &g_dpe683eb8;
    case 0x887703e8: return &g_dpe683e7c;
    case 0x887703f2: return &g_dpe683e38;
    case 0x887703fc: return &g_dpe683e50;
    case 0x88770406: return &g_dpe683e64;
    case 0x88770410: return &g_dpe683e20;
    case 0x8877041a: return &g_dpe683dd4;
    case 0x8877042e: return &g_dpe683df0;
    case 0x88770438: return &g_dpe683e04;
    case 0x88770442: return &g_dpe683dbc;
    case 0x8877044c: return &g_dpe683d78;
    case 0x887707d0: return &g_dpe683d8c;
    case 0x887707da: return &g_dpe683da8;
    case 0x887707e4: return &g_dpe683d60;
    case 0x887707ee: return &g_dpe683d0c;
    case 0x887707f8: return &g_dpe683d20;
    case 0x88770802: return &g_dpe683d40;
    case 0x8877080c: return &g_dpe683cf8;
    case 0x88770816: return &g_dpe683cbc;
    case 0x88770820: return &g_dpe683cd0;
    }
    wsprintfA(&g_dpeBuf723d7c, &g_dpeFmt683cec, c);
    return &g_dpeBuf723d7c;
}

} // extern "C"

// === a message's record cursor (FUN_6031e0, FUN_603300)

extern "C" {

// [FUN_6031e0] message s (data +4..+8, cursor +24): the type of the record at the cursor into *type; 0 for a
// known one (7, 42, 45, 120), 3 unknown, 1 at the end, 4 past it, 5 shorter than its header (vtable +4)
int __fastcall Msg_NextType(BYTE* s, void*, int* type)
{
    int head = CallVirt<int>(s, 4);
    int len = F(int, s, 8) - F(int, s, 4);
    if (len < head)
        return 5;
    int at = F(int, s, 24);
    if (at == len)
        return 1;
    if (!(at < len))
        return 4;
    int t = F(BYTE*, s, 4)[at];
    *type = t;
    return (t == 7 || t == 42 || t == 45 || t == 120) ? 0 : 3;
}

// [FUN_603300] message s's cursor moved past its record (7: 30 bytes, 42 / 45: 9, 120: 9 and its length) and
// the next record's type checked as Msg_NextType
int __fastcall Msg_SkipRecord(BYTE* s, void*)
{
    int at = F(int, s, 24);
    const BYTE* p = F(BYTE*, s, 4) + at;
    int t = *p;
    if (t == 7)
        F(int, s, 24) = at + 30;
    else if (t == 42 || t == 45)
        F(int, s, 24) = at + 9;
    else if (t == 120)
        F(int, s, 24) = *(const int*)(p + 5) + at + 9;
    return Msg_NextType(s, 0, &t);
}

} // extern "C"

// ---------------------------------------------------------------------------
// Batch 32: the network send buffer

extern "C" void Ng32_CopyBytes(); KING_ALIAS("_Ng32_CopyBytes", "_CopyBytes")
extern "C" int __cdecl Gz_Compress(void* in, void* out);
KING_VAR(BYTE, ng32_pack723de4, 723de4)
KING_VAR(char, ng32_tag683cb0, 683cb0)
KING_VAR(char, ng32_tagEnd683cb4, 683cb4)

// empties v and makes sure it holds 2048 bytes
static void Ng32_Reset(BYTE** v)
{
    BYTE* end = v[1];
    BYTE* begin = v[0];
    v[1] = v[1] + (begin - end);
    if ((unsigned)(v[2] - v[0]) < 2048) {
        BYTE* b = v[0];
        int n = (int)(v[1] - b);
        BYTE* nb = Bytes_Clone(2048u, b, v[1]);
        if (v[2] - v[0])
            Ai_PoolFree(v[0], (unsigned)(v[2] - v[0]));
        v[0] = nb;
        v[1] = nb + n;
        v[2] = nb + 2048;
    }
}

extern "C" {

// [FUN_600e60] puts the bytes (data, size) into the send buffer v: with packing on (723de4), the packed script
// value of them behind the 4-byte tag, kept when it is smaller than the bytes; else the bytes themselves
void __cdecl Net_FillSendBuffer(BYTE** v, const BYTE* data, int size)
{
    if (ng32_pack723de4) {
        CallC(Ng32_CopyBytes, v[1], v[1], v[0], size, 0);
        Ng32_Reset(v);
        void* r = (void*)Gz_MemStreamNew((BYTE*)data, size);
        BYTE* h = (BYTE*)operator new(12);
        void* hv = h ? DpHandler_Construct(h, 0, (int)v) : 0;
        Gz_Compress(r, hv);
        Gz_StreamClose((BYTE**)&r);
        Gz_StreamClose((BYTE**)&hv);
        CharVec_InsertRange(v, 0, v[0], (BYTE*)((const char*)&ng32_tag683cb0), (BYTE*)((const char*)&ng32_tagEnd683cb4));
        unsigned len = (unsigned)(v[1] - v[0]);
        if ((double)(__int64)len / size < 1.0)
            return;
    }
    Ng32_Reset(v);
    CharVec_InsertRange(v, 0, v[1], (BYTE*)data, (BYTE*)(data + size));
}

}

// [$L_601250] at exit: frees the record vector at 723d70
extern "C" void __fastcall VecB_FreeStorage(BYTE** v, int);
KING_VAR(BYTE, sg_723d70, 723d70)
extern "C" void __cdecl Sg_Exit601250() { VecB_FreeStorage((BYTE**)&sg_723d70, 0); }

// [$L_601260] clears +16 and +24
extern "C" BYTE* __fastcall Sg_Clear601260(BYTE* s)
{
    F(int, s, 16) = 0;
    F(int, s, 24) = 0;
    return s;
}

// [$L_601630] the service provider enumeration callback: every provider
// except the one with the GUID at 655320 is appended to the records at 723d60
// (guid, connection data, its size, name, flags). Returns TRUE.
KING_VAR(DWORD, sg_655320, 655320)
KING_VAR(BYTE*, sg_723d64, 723d64)          // end
KING_VAR(BYTE*, sg_723d68, 723d68)          // capacity end
KING_VAR(BYTE, sg_723d60, 723d60)
void __fastcall DpRecords_InsertAux(BYTE** s, void*, BYTE* pos, const BYTE* x);
extern "C" BOOL __stdcall Sg_EnumProvider(const DWORD* guid, const void* conn, DWORD size, const BYTE* name,
                                          int flags, void*)
{
    if (memcmp(guid, &sg_655320, 16) != 0) {
        BYTE rec[32];
        DpRecord_Construct(rec, 0, guid, conn, size, F(const char*, name, 8), flags);
        BYTE* end = sg_723d64;
        if (end != sg_723d68) {
            if (end)
                DpRecord_Copy(end, 0, rec);
            sg_723d64 = end + 32;
        } else {
            DpRecords_InsertAux((BYTE**)&sg_723d60, 0, end, rec);
        }
        Gate_FreeBuffers(rec, 0);
    }
    return TRUE;
}

// [$L_6022c0] the player enumeration callback: appends the player id to the
// list given as context. Returns TRUE.
extern "C" BYTE** __fastcall SchList_End(BYTE** self, void*, BYTE** out);
extern "C" BOOL __stdcall Sg_EnumPlayer(DWORD id, DWORD, const void*, DWORD, BYTE** list)
{
    BYTE* end;
    BYTE* n;
    SchList_End(list, 0, &end);
    DpList_Insert(&n, end, &id);
    return TRUE;
}

// [FUN_602320] the session's players: refills list from IDirectPlay4::
// EnumPlayers and takes the first into out; -1 (reported) without a
// session or when the enumeration fails
extern "C" BYTE** __fastcall SchList_Begin(BYTE** self, void*, BYTE** out);
extern "C" BYTE** __fastcall SchList_EraseRange(BYTE* self, void*, BYTE** out, BYTE* first, BYTE* last);
KING_VAR(BYTE, sg_68d1c0, 68d1c0)
KING_VAR(AsmLabel, sg_s684354, 684354)
KING_VAR(AsmLabel, sg_s684340, 684340)
KING_VAR(AsmLabel, sg_s683c78, 683c78)
KING_VAR(AsmLabel, sg_s68422c, 68422c)
typedef long (__stdcall* Sg_EnumPlayersFn)(void*, const void*, void*, void*, DWORD);
extern "C" int __cdecl Gate_Players(DWORD* out, BYTE** list)
{
    int line;
    const char* msg;
    long hr;
    if (!g_directPlay) {
        hr = 0;
        line = 1209;
        msg = sg_s684354;
    } else {
        BYTE* end = *list;
        BYTE* b;
        BYTE* junk;
        SchList_Begin(list, 0, &b);
        SchList_EraseRange((BYTE*)list, 0, &junk, b, end);
        hr = ((Sg_EnumPlayersFn)(*(void***)g_directPlay)[12])(g_directPlay, g_683ca0, (void*)Sg_EnumPlayer, list, 16);
        if (hr >= 0) {
            list[1] = *(BYTE**)*list;
            return MsgList_Take(out, (BYTE*)list);
        }
        line = 1216;
        msg = sg_s684340;
    }
    int text = Gate_600b50(hr);
    Ai_Report(8, 100, (int)((const char*)sg_s68422c), (int)((const char*)sg_s683c78), line, (int)msg, text, (int)&sg_68d1c0);
    return -1;
}

// [$L_602410] constructs an empty list (a 16-byte head node pointing at itself)
extern "C" BYTE* __cdecl Stl_Alloc16();
extern "C" BYTE** __fastcall Sg_ListConstruct(BYTE** self)
{
    BYTE* n = Stl_Alloc16();
    *self = n;
    F(BYTE*, n, 0) = n;
    F(BYTE*, *self, 4) = *self;
    return self;
}

// [FUN_602ad0] uninitialized copy of 32-byte records from [first, last) to dest
extern "C" BYTE* __cdecl DpRecords_UninitCopy(BYTE* first, BYTE* last, BYTE* dest, BYTE*)
{
    for (; first != last; first += 32, dest += 32)
        if (dest)
            DpRecord_Copy(dest, 0, first);
    return dest;
}
