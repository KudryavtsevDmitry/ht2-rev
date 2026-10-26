// core.cpp - the unit at 0x5cf180 of king.exe: intrusive list links, name
// hash tables, block pools, timed event queues, a memory-mapped file,
// millisecond timers and blit wrappers.

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
extern "C" void FUN_5c2800(); KING_ALIAS("_FUN_5c2800", "_Msavi_CreateStreamingBuffer")    // msavi: create the sound buffer
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
KING_VAR(DWORD, g_msaviBufSize, 702f64)
KING_VAR(DWORD, g_msaviLastPos, 702f68)
KING_VAR(DWORD, g_msaviPlayed, 702f6c)
KING_VAR(DWORD, g_msaviWritePos, 702f70)
KING_VAR(Dwords, g_msavi702f78, 702f78)
KING_VAR(int, g_702f94, 702f94)
KING_VAR(BYTE*, g_msavi702f98, 702f98)
KING_VAR(int, g_msavi702f9c, 702f9c)
KING_VAR(BYTE*, g_msaviBuffer, 702fa0)     // IDirectSoundBuffer
KING_VAR(BYTE*, g_msavi702fa4, 702fa4)
KING_VAR(BYTE*, g_msaviFormat, 702fa8)     // WAVEFORMATEX
KING_VAR(int, g_msaviReadCtx, 702fac)
KING_VAR(WaveRead, g_msaviRead, 702fb0)
KING_VAR(AsmLabel, s_msaviNoFormat, 67f5c4)
KING_VAR(AsmLabel, s_msaviNoRead, 67f5a0)
KING_VAR(AsmLabel, s_msaviNoBuffer, 67f578)
KING_VAR(AsmLabel, s_msaviLockFailed, 67f748)
KING_VAR(AsmLabel, s_msaviPosFailed, 67f7bc)
KING_VAR(AsmLabel, s_msaviStatusFailed, 67f7fc)
KING_VAR(AsmLabel, s_msaviFillFailed, 67f7e0)

KING_VAR(AsmLabel, g_vtbl651948, 651948)
KING_VAR(AsmLabel, g_vtbl651b00, 651b00)
KING_VAR(AsmLabel, g_vtbl651d14, 651d14)
KING_VAR(AsmLabel, g_vtbl651d64, 651d64)
KING_VAR(AsmLabel, g_vtbl651db0, 651db0)
KING_VAR(BYTE*, g_707f6c, 707f6c)
KING_VAR(int, g_70ebe8, 70ebe8)
KING_VAR(int, g_70ebec, 70ebec)
KING_VAR(int, g_70ebf0, 70ebf0)
KING_VAR(int, g_70ebf4, 70ebf4)
KING_VAR(int, g_70ebf8, 70ebf8)
KING_VAR(Dwords, g_7025f0, 7025f0)
KING_VAR(Dwords, g_67fa60, 67fa60)
KING_VAR(AsmLabel, s_treeShadow, 67f9ac)

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
KING_VAR(int, g_67faa0, 67faa0)
KING_VAR(int, g_71e800, 71e800)
KING_VAR(int, g_71e7d4, 71e7d4)
KING_VAR(int, g_71e7e4, 71e7e4)
KING_VAR(int, g_71e844, 71e844)
KING_VAR(AsmLabel, g_pool71e830, 71e830)
KING_VAR(AsmLabel, g_pool7200c0, 7200c0)
typedef int Ints[1];
KING_VAR(Ints, g_71e858, 71e858)
KING_VAR(AsmLabel, g_720080, 720080)
KING_VAR(AsmLabel, g_vtbl651eb4, 651eb4)
KING_VAR(BYTE*, g_723d44, 723d44)
KING_VAR(CRITICAL_SECTION, g_cs723b40, 723b40)
KING_VAR(AsmLabel, g_683c58, 683c58)

extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
KING_VAR(AsmLabel, s_dollars, 67fb20)          // "$$"

extern "C" {

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

static __forceinline BYTE* Table_Bucket(BYTE* self, int key)
{
    BYTE* e = F(BYTE*, self, (key % 10) * 32 + 8);
    while (e && F(int, e, 12) != key)
        e = F(BYTE*, e, 8);
    return e;
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

}

extern "C" {

// [FUN_5d2430] deletes the entries of this owner (+20)
void __fastcall NameHash_RemoveOwner(BYTE* self, void*, int owner)
{
    if (!g_71e844)
        return;
    for (int i = 0; i < F(int, self, 4); i++) {
        for (BYTE* e = F(BYTE**, self, 8)[i]; e;) {
            BYTE* next = F(BYTE*, e, 8);
            if (F(int, e, 20) == owner) {
                if (e == F(BYTE**, self, 8)[i])
                    F(BYTE**, self, 8)[i] = next;
                Node_Unlink(e, 0);
                CallVirt(e, 0, 1);
            }
            e = next;
        }
    }
    BYTE* head = F(BYTE*, self, 0);
    if (!head)
        return;
    for (BYTE* e = F(BYTE*, head, 8); e;) {
        BYTE* next = F(BYTE*, e, 8);
        if (F(int, e, 20) == owner) {
            Node_Unlink(e, 0);
            CallVirt(e, 0, 1);
        }
        e = next;
    }
    head = F(BYTE*, self, 0);
    if (F(int, head, 20) == owner) {
        BYTE* next = F(BYTE*, head, 8);
        Node_Unlink(head, 0);
        if (F(BYTE*, self, 0))
            CallVirt(F(BYTE*, self, 0), 0, 1);
        F(BYTE*, self, 0) = next;
    }
}

// [FUN_5d27c0] resolves the pending references; returns the first name that
// stays unresolved ("$$" names may stay)
const char* __fastcall NameHash_ResolvePending(BYTE* self, void*)
{
    BYTE* head = F(BYTE*, self, 0);
    if (!head)
        return 0;
    for (BYTE* e = F(BYTE*, head, 8); e;) {
        BYTE* next = F(BYTE*, e, 8);
        if (Hash_Lookup(self, 0, F(const char*, e, 12), F(int*, e, 16))) {
            Node_Unlink(e, 0);
            CallVirt(e, 0, 1);
        } else if (strncmp(F(const char*, e, 12), s_dollars, 2)) {
            return F(const char*, e, 12);
        }
        e = next;
    }
    head = F(BYTE*, self, 0);
    if (Hash_Lookup(self, 0, F(const char*, head, 12), F(int*, head, 16))) {
        BYTE* h = F(BYTE*, self, 0);
        if (!F(BYTE*, h, 8)) {
            if (h)
                CallVirt(h, 0, 1);
            F(BYTE*, self, 0) = 0;
            return 0;
        }
        F(BYTE*, self, 0) = F(BYTE*, h, 8);
        Node_Unlink(h, 0);
        CallVirt(h, 0, 1);
        return 0;
    }
    if (!strncmp(F(const char*, F(BYTE*, self, 0), 12), s_dollars, 2))
        return 0;
    return F(const char*, F(BYTE*, self, 0), 12);
}

// [FUN_5d3660] inserts an event by due time (+16, made absolute) and
// priority (+12 -> +20); drops an exact duplicate
void __fastcall Queue_Insert(BYTE* self, void*, BYTE* ev)
{
    if (!ev || !F(BYTE*, ev, 12))
        return;
    F(int, ev, 16) += F(int, self, 0);
    BYTE* n = F(BYTE*, self, 12);
    if (!n) {
        F(BYTE*, self, 12) = ev;
        return;
    }
    for (; n; n = CallThis<BYTE*>(FUN_5d3080, n))
        if (F(int, n, 12) == F(int, ev, 12) && F(int, n, 16) == F(int, ev, 16) && F(int, n, 20) == F(int, ev, 20)) {
            CallVirt(ev, 0, 1);
            return;
        }
    BYTE* p = F(BYTE*, self, 12);
    if (F(int, p, 16) > F(int, ev, 16) ||
        (F(int, p, 16) == F(int, ev, 16) && F(int, F(BYTE*, p, 12), 20) < F(int, F(BYTE*, ev, 12), 20))) {
        F(BYTE*, ev, 8) = p;
        F(BYTE*, p, 4) = ev;
        F(BYTE*, self, 12) = ev;
        return;
    }
    BYTE* q;
    while ((q = CallThis<BYTE*>(FUN_5d3080, p)) != 0 && F(int, q, 16) < F(int, ev, 16))
        p = q;
    while ((q = CallThis<BYTE*>(FUN_5d3080, p)) != 0 && F(int, q, 16) == F(int, ev, 16) &&
           F(int, F(BYTE*, q, 12), 20) > F(int, F(BYTE*, ev, 12), 20))
        p = q;
    Node_Link(ev, 0, p);
}

}

// ---------------------------------------------------------------------------
// Fatal errors and the event queues' holders (split procs: their code ran on
// past ENDP after the call to the fatal error function, which never returns).

extern "C" void FUN_5d2e50(); KING_ALIAS("_FUN_5d2e50", "_Table71e858_Get")    // a text of the resources
extern "C" void FUN_5d2190(); KING_ALIAS("_FUN_5d2190", "@Node_Link@12")
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
extern "C" void FUN_5d3090(); KING_ALIAS("_FUN_5d3090", "@Event_Construct@16")
extern "C" void FUN_5d30e0(); KING_ALIAS("_FUN_5d30e0", "@Event_Construct3@20")
extern "C" void FUN_5d3630(); KING_ALIAS("_FUN_5d3630", "@Queue_Log@8")
extern "C" void FUN_5d3660(); KING_ALIAS("_FUN_5d3660", "@Queue_Insert@12")
extern "C" void FUN_5d3a60(); KING_ALIAS("_FUN_5d3a60", "@Queue_Remove@12")
extern "C" void FUN_5d2b20(); KING_ALIAS("_FUN_5d2b20", "@Pool_Used@12")
extern "C" void FUN_5d3060(); KING_ALIAS("_FUN_5d3060", "_Pool7200c0_Alloc")    // event pool: allocate (size ignored)
extern "C" void FUN_5d3080(); KING_ALIAS("_FUN_5d3080", "@Node_Next@8")    // next sibling
extern "C" void FUN_5d2ba0(); KING_ALIAS("_FUN_5d2ba0", "_Sys_Log")    // log (text, value)
KING_VAR(AsmLabel, s_67fbc4, 67fbc4)
KING_VAR(AsmLabel, g_text71fc20, 71fc20)
KING_VAR(AsmLabel, s_67fbe0, 67fbe0)
KING_VAR(AsmLabel, g_text720028, 720028)
KING_VAR(AsmLabel, g_text720080, 720080)
KING_VAR(AsmLabel, s_67fbd0, 67fbd0)
KING_VAR(AsmLabel, g_text71f820, 71f820)
KING_VAR(AsmLabel, s_67fc94, 67fc94)
KING_VAR(AsmLabel, s_67fc78, 67fc78)
KING_VAR(AsmLabel, s_67fc54, 67fc54)
KING_VAR(AsmLabel, s_67fb40, 67fb40)
KING_VAR(AsmLabel, s_67fb24, 67fb24)
KING_VAR(AsmLabel, s_67fbfc, 67fbfc)
KING_VAR(AsmLabel, s_67fcb4, 67fcb4)
KING_VAR(AsmLabel, s_67fc3c, 67fc3c)
KING_VAR(int, g_720118, 720118)
extern "C" __declspec(noreturn) void __cdecl _exit(int code);
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg);

// [FUN_5d2ef0] ends the program: logs "<date> <time>" with the message, runs the fatal hook and exits
extern "C" void __cdecl Sys_Exit(const char* msg)
{
    SYSTEMTIME t;
    GetLocalTime(&t);
    sprintf(g_text720028, s_67fbe0, t.wDay, t.wMonth, t.wYear % 100, t.wHour, t.wMinute);
    sprintf(g_text71f820, s_67fbd0, (char*)g_text720080, (char*)g_text720028, msg);
    CallC(FUN_5d2ba0, (char*)g_text71f820, 0);
    g_pfnOnFatal();
    _exit(0);
}

// [FUN_5d2e70..$L_5d2e9e] the fatal error: formats (fmt, arg) after resource text 0 and ends the program
extern "C" void __cdecl Sys_Fatal(const char* fmt, const char* arg)
{
    const char* head = CallC<const char*>(FUN_5d2e50, 0);
    sprintf(g_text71fc20, s_67fbc4, head, fmt, arg);
    Sys_Exit(g_text71fc20);
}

// [FUN_5d3600..$L_5d3622] a pool failure: fatal, with the text of code 1 or of the others
extern "C" void __cdecl Pool_Fail(int code)
{
    if (code == 1)
        Sys_Fatal(s_67fc94, 0);
    Sys_Fatal(s_67fc78, 0);
}

// [FUN_5d3570..$L_5d358b] attaches a node (one without an owner) to this owner's list at +4
extern "C" void __fastcall Owner_Attach(BYTE* self, void*, BYTE* node)
{
    if (F(int, node, 12))
        Sys_Fatal(s_67fc54, 0);
    F(BYTE*, node, 12) = self;
    BYTE* head = F(BYTE*, self, 4);
    if (!head)
    {
        F(BYTE*, self, 4) = node;
        return;
    }
    CallThis(FUN_5d2190, node, head);
}

// [FUN_5d2ac0..$L_5d2b0d] frees an element of a fixed-size pool {size, count, base, used flags}; fatal when it isn't one
extern "C" void __fastcall Pool_FreeItem(BYTE* pool, void*, BYTE* p)
{
    unsigned i = (unsigned)(p - F(BYTE*, pool, 8)) / F(unsigned, pool, 0);
    if ((int)i < 0 || i >= F(unsigned, pool, 4))
        Sys_Fatal(s_67fb40, 0);
    if (!F(BYTE*, pool, 12)[i])
        Sys_Fatal(s_67fb24, 0);
    F(BYTE*, pool, 12)[i] = 0;
}

// posts an event built by make to the queue at +12 (fatal when the pool is out of events)
static int Queue_PostMade(BYTE* self, int arg, int kind)
{
    if (!F(void*, self, 12))
        return 0;
    void* m = CallC<void*>(FUN_5d3060, 24);
    BYTE* ev = 0;
    if (m)
        ev = kind < 0 ? CallThis<BYTE*>(FUN_5d3090, m, self, arg)
                      : CallThis<BYTE*>(FUN_5d30e0, m, self, arg, kind);
    if (!ev)
    {
        CallThis(FUN_5d3630, F(void*, self, 12));
        Sys_Fatal(s_67fbfc, 0);
    }
    return CallThis<int>(FUN_5d3660, F(void*, self, 12), ev);
}

// [FUN_5d32f0..$L_5d3341] posts an event (this, arg) to the queue at +12
extern "C" int __fastcall Queue_PostEvent(BYTE* self, void*, int arg)
{
    return Queue_PostMade(self, arg, -1);
}

// [FUN_5d3350..$L_5d33a3] posts a kind-1 event (this, arg) to the queue at +12
extern "C" int __fastcall Queue_PostEvent1(BYTE* self, void*, int arg)
{
    return Queue_PostMade(self, arg, 1);
}

// [FUN_5d33d0..$L_5d3423] posts a kind-2 event (this, arg) to the queue at +12
extern "C" int __fastcall Queue_PostEvent2(BYTE* self, void*, int arg)
{
    return Queue_PostMade(self, arg, 2);
}

// removes and deletes the children of list whose +12 is key
static void Queue_DropKey(BYTE* self, BYTE* list, int key)
{
    for (BYTE* n = list; n; )
    {
        BYTE* next = CallThis<BYTE*>(FUN_5d3080, n);
        if (F(int, n, 12) == key)
        {
            CallThis(FUN_5d3a60, self, n);
            if (n)
                CallVirt(n, 0, 1);
        }
        n = next;
    }
}

// [FUN_5d3790..$L_5d37af] deletes the queued events (both lists) of owner key; fatal for the running one
extern "C" void __fastcall Queue_RemoveKey(BYTE* self, void*, int key)
{
    if (g_720118 == key)
        Sys_Fatal(s_67fcb4, 0);
    Queue_DropKey(self, F(BYTE*, self, 12), key);
    Queue_DropKey(self, F(BYTE*, self, 8), key);
}

// unlinks and deletes every child of list
static void Owner_DeleteChildren(BYTE* list)
{
    for (BYTE* n = CallThis<BYTE*>(FUN_5d3080, list); n; n = CallThis<BYTE*>(FUN_5d3080, list))
    {
        CallThis(FUN_5d21b0, n);
        if (n)
            CallVirt(n, 0, 1);
    }
}

// [FUN_5d3460..$L_5d34fc] deletes the three lists at +12, +8 (a pooled one) and +4 with their children
extern "C" void __fastcall Owner_Clear(BYTE* self, void*)
{
    F(int, self, 0) = 0;
    BYTE* q = F(BYTE*, self, 12);
    if (q)
    {
        Owner_DeleteChildren(q);
        CallVirt(q, 0, 1);
        F(BYTE*, self, 12) = 0;
    }
    q = F(BYTE*, self, 8);
    if (q)
    {
        Owner_DeleteChildren(q);
        if (!CallThis<int>(FUN_5d2b20, g_pool7200c0, q))
            Sys_Fatal(s_67fc3c, 0);
        CallVirt(q, 0, 1);
        F(BYTE*, self, 8) = 0;
    }
    q = F(BYTE*, self, 4);
    if (q)
    {
        Owner_DeleteChildren(q);
        CallVirt(q, 0, 1);
        F(BYTE*, self, 4) = 0;
    }
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5d3fd0(); KING_ALIAS("_FUN_5d3fd0", "_Dd_Release")

// [FUN_5d2a10] frees an element of the pool at 71e830
extern "C" void __cdecl Pool71e830_Free(BYTE* p)
{
    Pool_FreeItem((BYTE*)g_pool71e830, 0, p);
}

// [FUN_5d2eb0] 5d3fd0(0)
extern "C" void __cdecl Core_5d2eb0()
{
    CallC(FUN_5d3fd0, 0);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5d35b0(); KING_ALIAS("_FUN_5d35b0", "@Owner_Detach@12")    // queue: drop a task (this, task)
KING_VAR(AsmLabel, s_67fcdc, 67fcdc)
KING_VAR(AsmLabel, s_67fcd0, 67fcd0)
KING_VAR(AsmLabel, g_text7200d4, 7200d4)
KING_VAR(BYTE*, g_coreTraceData, 71e814)
KING_VAR(int, g_coreTracePos, 71e818)
KING_VAR(int, g_coreTraceSize, 71e810)
extern "C" void __cdecl Pool_Fail(int code);

// appends a text to the trace buffer while it has room
static void Core_TraceText(const char* p)
{
    if (!g_coreTraceData || !*p)
        return;
    while (g_coreTracePos < g_coreTraceSize)
    {
        g_coreTraceData[g_coreTracePos] = *p++;
        g_coreTracePos++;
        if (!*p)
            break;
    }
}

// [FUN_5d38c0..$L_5d3a08] runs a due event of the queue: state 0 calls its task (traced when +20), 2 restarts it, others drop the task
extern "C" void __fastcall Queue_RunEvent(BYTE* self, void*, BYTE* ev)
{
    BYTE* task = F(BYTE*, ev, 12);
    if (!task)
        return;
    F(int, task, 24) = F(int, ev, 16);
    int st = F(int, ev, 20);
    if (st == 0)
    {
        g_720118 = (int)F(BYTE*, ev, 12);
        if (!Pool_Used((BYTE*)g_pool7200c0, 0, ev))
            Pool_Fail(0);
        if (F(int, self, 20))
        {
            BYTE* t = F(BYTE*, ev, 12);
            sprintf(g_text7200d4, s_67fcdc, F(int, t, 28), F(int, t, 20));
            Core_TraceText(g_text7200d4);
        }
        CallVirt(F(BYTE*, ev, 12), 20);
        if (F(int, self, 20))
            Core_TraceText(s_67fcd0);
        g_720118 = 0;
        if (!Pool_Used((BYTE*)g_pool7200c0, 0, ev))
            Pool_Fail(1);
    }
    else if (st == 2)
    {
        F(int, F(BYTE*, ev, 12), 16) = 0;
        CallVirt(F(BYTE*, ev, 12), 20);
    }
    else
    {
        CallThis(FUN_5d35b0, self, task);
        if (task)
            CallVirt(task, 0, 1);
        return;
    }
    F(int, F(BYTE*, ev, 12), 16)++;
    Queue_Remove(self, 0, ev);
    if (ev)
        CallVirt(ev, 0, 1);
}


// ---------------------------------------------------------------------------
extern "C" void FUN_5d3790(); KING_ALIAS("_FUN_5d3790", "@Queue_RemoveKey@12")
extern "C" void FUN_5d21b0(); KING_ALIAS("_FUN_5d21b0", "@Node_Unlink@8")
KING_VAR(AsmLabel, g_logName67fb5c, 67fb5c)
KING_VAR(AsmLabel, s_67fb80, 67fb80)
KING_VAR(AsmLabel, s_67fb84, 67fb84)
KING_VAR(FILE*, g_logFile720068, 720068)
extern "C" int __cdecl fprintf(FILE* f, const char* format, ...);

// [FUN_5d2ba0] appends (text, value) to the log file when it has a name
extern "C" void __cdecl Sys_Log(const char* text, int value)
{
    if (!g_logName67fb5c[0])
        return;
    g_logFile720068 = fopen(g_logName67fb5c, s_67fb80);
    fprintf(g_logFile720068, s_67fb84, text, value);
    fclose(g_logFile720068);
}

// [FUN_5d2f90] the free memory, always 1000000
extern "C" int __cdecl Sys_FreeMemory() { return 1000000; }

// [FUN_5d3080] the next sibling (+8)
extern "C" BYTE* __fastcall Node_Next(BYTE* self, void*) { return F(BYTE*, self, 8); }

// [FUN_5d35b0] detaches a task from this owner's list (+4 head)
extern "C" void __fastcall Owner_Detach(BYTE* self, void*, BYTE* node)
{
    CallThis(FUN_5d3790, self, node);
    if (node == F(BYTE*, self, 4))
    {
        F(BYTE*, self, 4) = F(BYTE*, node, 8);
        if (F(BYTE*, node, 8))
            F(BYTE*, F(BYTE*, node, 8), 4) = 0;
        F(BYTE*, node, 12) = 0;
        return;
    }
    CallThis(FUN_5d21b0, node);
    F(BYTE*, node, 12) = 0;
}


// ---------------------------------------------------------------------------
// Small virtual methods and static initializers.

extern "C" void FUN_5d3290(); KING_ALIAS("_FUN_5d3290", "@Object_Destroy@4")

static BYTE* Cor_DeleteAfter(BYTE* self, int flags, void (*dtor)())
{
    CallThis(dtor, self);
    if (flags & 1)
        operator delete(self);
    return self;
}
// [$L_5d31c0]
extern "C" BYTE* __fastcall Cor5d31c0_DeletingDestructor(BYTE* s, void*, int flags) { return Cor_DeleteAfter(s, flags, FUN_5d3290); }

extern "C" void FUN_5d2950(); KING_ALIAS("_FUN_5d2950", "@Cor5d2950_Destroy@4")
extern "C" void FUN_5d2a10(); KING_ALIAS("_FUN_5d2a10", "_Pool71e830_Free")
extern "C" void FUN_5ce630(); KING_ALIAS("_FUN_5ce630", "@Light_DestructBase@8")
extern "C" void FUN_5d3070(); KING_ALIAS("_FUN_5d3070", "_Pool7200c0_Free")

// [$L_5d2930] freed with FUN_5d2a10
extern "C" BYTE* __fastcall Cor5d2930_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5d2950, s);
    if (flags & 1)
        CallC(FUN_5d2a10, s);
    return s;
}
// [$L_5d30c0] freed with FUN_5d3070
extern "C" BYTE* __fastcall Cor5d30c0_DeletingDestructor(BYTE* s, void*, int flags)
{
    CallThis(FUN_5ce630, s);
    if (flags & 1)
        CallC(FUN_5d3070, s);
    return s;
}

extern "C" void FUN_5d33b0(); KING_ALIAS("_FUN_5d33b0", "@Handle_Release@8")
KING_VAR(AsmLabel, g_cor651ea0, 651ea0)
KING_VAR(AsmLabel, g_cor651ec8, 651ec8)
KING_VAR(AsmLabel, g_cor64d81c, 64d81c)

// [FUN_5d2950] (vtable 651ea0) frees +12
extern "C" void __fastcall Cor5d2950_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_cor651ea0;
    operator delete(F(void*, s, 12));
    F(void*, s, 0) = g_cor64d81c;
}
// [FUN_5d3290] the base object (vtable 651ec8): its links (FUN_5d33b0) and its name (+28)
extern "C" void __fastcall Object_Destroy(BYTE* s)
{
    F(void*, s, 0) = g_cor651ec8;
    CallThis(FUN_5d33b0, s);
    if (F(void*, s, 28))
        operator delete(F(void*, s, 28));
    F(void*, s, 0) = g_cor64d81c;
}

// ---------------------------------------------------------------------------
// Constructors and window resizing (exception frames dropped).

#include <string.h>

// a copy of the string t in new memory
static char* Cor_StrDup(const char* t)
{
    size_t n = strlen(t) + 1;
    char* d = (char*)operator new(n);
    memcpy(d, t, n);
    return d;
}
// [FUN_5d28a0] constructor (name): a named object (vtable 651ea0)
extern "C" BYTE* __fastcall Cor5d28a0_Construct(BYTE* s, void*, const char* name)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_cor651ea0;
    F(char*, s, 12) = Cor_StrDup(name);
    F(int, s, 16) = 0;
    F(int, s, 20) = 0;
    return s;
}
// [FUN_5d3110] constructor (name): the base object (vtable 651ec8), its name (+28) copied when given
extern "C" BYTE* __fastcall Object_Construct(BYTE* s, void*, const char* name)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_cor651ec8;
    F(int, s, 16) = 0;
    F(int, s, 20) = 0;
    F(int, s, 12) = 0;
    F(char*, s, 28) = name ? Cor_StrDup(name) : 0;
    return s;
}
// [FUN_5d31e0] constructor (kind, name): the same with the kind at +20
extern "C" BYTE* __fastcall Object_ConstructKind(BYTE* s, void*, int kind, const char* name)
{
    F(int, s, 4) = 0;
    F(int, s, 8) = 0;
    F(void*, s, 0) = g_cor651ec8;
    F(int, s, 20) = kind;
    F(int, s, 16) = 0;
    F(int, s, 12) = 0;
    F(char*, s, 28) = name ? Cor_StrDup(name) : 0;
    return s;
}

// ---------------------------------------------------------------------------
// Static object setup: a pointer table, slots of the static pools.

KING_VAR(AsmLabel, g_q46_71e820, 71e820)
extern "C" BYTE* __fastcall PtrTable500_Construct(BYTE* s, void*);    // media.cpp
KING_VAR(AsmLabel, g_q46_7200c0, 7200c0)
extern "C" BYTE* __fastcall Pool_AllocSlot(BYTE* s, void*);
// [FUN_5d2b50] constructs the static pointer table 71e820
extern "C" int __cdecl Static71e820_Construct(void)
{
    return (int)PtrTable500_Construct((BYTE*)(void*)g_q46_71e820, 0);
}
// [FUN_5d3060] a free slot of the static pool 7200c0
extern "C" int __cdecl Pool7200c0_Alloc(void)
{
    return (int)Pool_AllocSlot((BYTE*)(void*)g_q46_7200c0, 0);
}

// ---------------------------------------------------------------------------
// Small static setup and teardown procs, destructors and thunks.

KING_VAR(AsmLabel, g_q47_71e830, 71e830)
// [FUN_5d2a00] a free slot of the static pool 71e830
extern "C" int __cdecl Pool71e830_Alloc(void)
{
    return (int)Pool_AllocSlot((BYTE*)(void*)g_q47_71e830, 0);
}

// ---------------------------------------------------------------------------
// Small methods: vector storage frees, COM releases, static tables, CPU and sound checks.

KING_VAR(void*, g_q51_71e858, 71e858)
KING_VAR(int, g_q51_720890, 720890)
KING_VAR(int, g_q51_7208ac, 7208ac)
// [$L_5d2e20] frees the 1000 entries of the 71e858 table
extern "C" void __cdecl Table71e858_FreeAll(void)
{
    for (int k = 0; k < 1000; k++)
        if ((&g_q51_71e858)[k])
            operator delete((&g_q51_71e858)[k]);
}
// [FUN_5d3ab0] the texture level for the 720890 mode: 1, 2 or 3 (by 7208ac), 6, or 0
extern "C" int __cdecl Mode_TextureLevel(void)
{
    switch (g_q51_720890) {
    case 1: return 1;
    case 2: return g_q51_7208ac ? 2 : 3;
    case 3:
    case 4: return 6;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Small methods: pool slots, list teardown, loader thread, frame shutdown, message dispatch.

// [$L_5d2a70] a free slot of the pool s (element size +0, count +4, base +8, used bytes +12, next +16), or 0
extern "C" BYTE* __fastcall Pool_AllocSlot(BYTE* s, void*)
{
    unsigned i = F(unsigned, s, 16);
    unsigned n = F(unsigned, s, 4);
    BYTE* used = F(BYTE*, s, 12);
    int tries = (int)n;
    while (used[i]) {
        if (tries <= 0)
            return 0;
        i = (i + 1) % n;
        tries--;
    }
    used[i] = 1;
    F(unsigned, s, 16) = i;
    return F(int, s, 0) * i + F(BYTE*, s, 8);
}

// ---------------------------------------------------------------------------
// Polygon edges of the software span filler.

KING_VAR(int, g_q134_723a88, 723a88)
KING_VAR(int, g_q134_71e7dc, 71e7dc)
KING_VAR(int, g_q134_71e7d8, 71e7d8)
KING_VAR(AsmLabel, g_q134_70f3f0, 70f3f0)
// [FUN_5d02b0] walks the edge from a to b (fixed point x +12, y +16 with the row in the top bits) into the span
// table 70f3f0 (24-byte rows, the left or right side by 723a88 and the edge direction), widening the row range
// 71e7dc..71e7d8
extern "C" void __cdecl Span_Edge(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        F(int, t, row * 24) = F(int, p1, 12);
        if (row < g_q134_71e7dc)
            g_q134_71e7dc = row;
        if (row > g_q134_71e7d8)
            g_q134_71e7d8 = row;
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1023 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if (last > g_q134_71e7d8)
        g_q134_71e7d8 = last;
    int slope = dy ? (F(int, p2, 12) - F(int, p1, 12)) / dy : 0;
    int x = slope * f + F(int, p1, 12);
    F(int, t, row * 24) = x;
    for (row++; row <= last; row++) {
        x += slope << 10;
        F(int, t, row * 24) = x;
    }
}

// [FUN_5d0400] like 5d02b0 with a second value (+20, kept at +8 of the rows, at most 2^24 - 1 on the last row)
// interpolated along the edge
extern "C" void __cdecl Span_EdgeV(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        F(int, t, row * 24) = F(int, p1, 12);
        F(int, t, row * 24 + 8) = F(int, p1, 20);
        if (row < g_q134_71e7dc)
            g_q134_71e7dc = row;
        if (row > g_q134_71e7d8)
            g_q134_71e7d8 = row;
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1024 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if (last > g_q134_71e7d8)
        g_q134_71e7d8 = last;
    int x = F(int, p1, 12), v = F(int, p1, 20);
    int slope = 0, dv = 0;
    if (dy) {
        slope = (F(int, p2, 12) - x) / dy;
        dv = ((F(int, p2, 20) - v) << 6) / dy << 4;
        if (dy > 2048)
            v += ((f >> 4) * dv) >> 6;
        else
            v += ((dv >> 4) * f) >> 6;
        x += slope * f;
    }
    slope <<= 10;
    BYTE* r = t + row * 24;
    F(int, r, 0) = x;
    F(int, r, 8) = v;
    for (int k = row + 1; k <= F(int, p2, 16) >> 20; k++) {
        r += 24;
        x += slope;
        v += dv;
        F(int, r, 0) = x;
        F(int, r, 8) = v;
    }
    if (F(int, r, 8) >= 16777216)
        F(int, r, 8) = 16777215;
}

KING_VAR(AsmLabel, g_q136_67fa80, 67fa80)
// [FUN_5d05e0] like 5d02b0 with a value at +40 interpolated into +20 of the rows (dithered by the 67fa80 table
// after the first row)
extern "C" void __cdecl Span_EdgeD(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        F(int, t, row * 24) = F(int, p1, 12);
        F(int, t, row * 24 + 20) = F(int, p1, 40);
        if (row < g_q134_71e7dc)
            g_q134_71e7dc = row;
        if (row > g_q134_71e7d8)
            g_q134_71e7d8 = row;
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1023 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if ((F(int, p2, 16) >> 20) > g_q134_71e7d8)
        g_q134_71e7d8 = F(int, p2, 16) >> 20;
    int sx = 0, sv = 0;
    if (dy) {
        sx = (F(int, p2, 12) - F(int, p1, 12)) / dy;
        sv = (F(int, p2, 40) - F(int, p1, 40)) / dy;
    }
    int x = sx * f + F(int, p1, 12);
    int v = sv * f + F(int, p1, 40);
    sx <<= 10;
    sv <<= 10;
    BYTE* r = t + row * 24;
    F(int, r, 0) = x;
    F(int, r, 20) = v;
    const int* d = (const int*)g_q136_67fa80;
    for (int k = row + 1; k <= F(int, p2, 16) >> 20; k++) {
        x += sx;
        v += sv;
        r += 24;
        F(int, r, 0) = x;
        F(int, r, 20) = d[k & 7] + v;
    }
}

// [FUN_5d0790] 5d05e0 and 5d0400 together: +40 into +20 (dithered) and +20 into +8 (not moved by the first
// row's fraction)
extern "C" void __cdecl Span_EdgeDV(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        F(int, t, row * 24) = F(int, p1, 12);
        F(int, t, row * 24 + 20) = F(int, p1, 40);
        F(int, t, row * 24 + 8) = F(int, p1, 20);
        if (row < g_q134_71e7dc)
            g_q134_71e7dc = row;
        if (row > g_q134_71e7d8)
            g_q134_71e7d8 = row;
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1023 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if ((F(int, p2, 16) >> 20) > g_q134_71e7d8)
        g_q134_71e7d8 = F(int, p2, 16) >> 20;
    int sx = 0, dv = 0, sw = 0;
    if (dy) {
        sx = (F(int, p2, 12) - F(int, p1, 12)) / dy;
        if (dy > 2048)
            dv = ((F(int, p2, 20) - F(int, p1, 20)) << 6) / (dy >> 4);
        else
            dv = ((F(int, p2, 20) - F(int, p1, 20)) << 6) / dy << 4;
        sw = (F(int, p2, 40) - F(int, p1, 40)) / dy;
    }
    int x = sx * f + F(int, p1, 12);
    int w = sw * f + F(int, p1, 40);
    int v = F(int, p1, 20);
    sx <<= 10;
    sw <<= 10;
    BYTE* r = t + row * 24;
    F(int, r, 0) = x;
    F(int, r, 20) = w;
    F(int, r, 8) = v;
    const int* d = (const int*)g_q136_67fa80;
    for (int k = row + 1; k <= F(int, p2, 16) >> 20; k++) {
        w += sw;
        x += sx;
        r += 24;
        F(int, r, 0) = x;
        F(int, r, 20) = d[k & 7] + w;
        v += dv;
        F(int, r, 8) = v;
    }
}

// [FUN_5d0e40] like 5d02b0 with texture coordinates (+24, +28 into +12, +16 of the rows) and +20 copied to +8;
// a one-row edge does not widen the row range
extern "C" void __cdecl Span_EdgeUV(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        BYTE* r = t + row * 24;
        F(int, r, 0) = F(int, p1, 12);
        F(int, r, 12) = F(int, p1, 24);
        F(int, r, 16) = F(int, p1, 28);
        F(int, r, 8) = F(int, p1, 20);
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1023 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if ((F(int, p2, 16) >> 20) > g_q134_71e7d8)
        g_q134_71e7d8 = F(int, p2, 16) >> 20;
    int sx = 0, su = 0, sv = 0;
    if (dy & ~15) {
        sx = (F(int, p2, 12) - F(int, p1, 12)) / dy;
        su = ((F(int, p2, 24) - F(int, p1, 24)) << 6) / (dy >> 4);
        sv = ((F(int, p2, 28) - F(int, p1, 28)) << 6) / (dy >> 4);
    }
    int c = F(int, p1, 20);
    int x = sx * f + F(int, p1, 12);
    int u = (su >> 10) * f + F(int, p1, 24);
    int v = (sv >> 10) * f + F(int, p1, 28);
    sx <<= 10;
    BYTE* r = t + row * 24;
    F(int, r, 0) = x;
    F(int, r, 12) = u;
    F(int, r, 16) = v;
    F(int, r, 8) = c;
    for (int k = row + 1; k <= F(int, p2, 16) >> 20; k++) {
        r += 24;
        x += sx;
        u += su;
        v += sv;
        F(int, r, 0) = x;
        F(int, r, 12) = u;
        F(int, r, 16) = v;
        F(int, r, 8) = c;
    }
}

KING_VAR(void*, g_q156_71e838, 71e838)
KING_VAR(void*, g_q156_71e83c, 71e83c)
// [$L_5d29e0] static exit: frees the 71e838 and 71e83c blocks
extern "C" void __cdecl Core_Free71e838(void)
{
    operator delete(g_q156_71e838);
    operator delete(g_q156_71e83c);
}

// ---------------------------------------------------------------------------
// Static initializers: construct a global, register its teardown at exit.

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void q197_FUN_5d29b0(); KING_ALIAS("_q197_FUN_5d29b0", "_Pool71e830_Init")
extern "C" void q197_FUN_5d2b50(); KING_ALIAS("_q197_FUN_5d2b50", "_Static71e820_Construct")
extern "C" void q197_FUN_5d2c00(); KING_ALIAS("_q197_FUN_5d2c00", "_Messages_Load")
extern "C" void q197_FUN_5d3010(); KING_ALIAS("_q197_FUN_5d3010", "_Pool7200c0_Init")
extern "C" void q197_L_5d29e0(); KING_ALIAS("_q197_L_5d29e0", "_Core_Free71e838")
extern "C" void q197_L_5d2b70(); KING_ALIAS("_q197_L_5d2b70", "_Static71e820_Destroy")
extern "C" void q197_L_5d2e20(); KING_ALIAS("_q197_L_5d2e20", "_Table71e858_FreeAll")
extern "C" void q197_L_5d3040(); KING_ALIAS("_q197_L_5d3040", "_Pool7200c0_Release")
// [$L_5d29d0] registers the teardown at exit
extern "C" int __cdecl Static5d29a0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_5d29e0); }
// [$L_5d29a0] the static initializer: FUN_5d29b0, then the teardown at exit
extern "C" int __cdecl Static5d29a0_Init(void)
{
    CallC(q197_FUN_5d29b0);
    return Static5d29a0_AtExit();
}
// [$L_5d2b60] registers the teardown at exit
extern "C" int __cdecl Static5d2b40_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_5d2b70); }
// [$L_5d2b40] the static initializer: FUN_5d2b50, then the teardown at exit
extern "C" int __cdecl Static5d2b40_Init(void)
{
    CallC(q197_FUN_5d2b50);
    return Static5d2b40_AtExit();
}
// [$L_5d2e10] registers the teardown at exit
extern "C" int __cdecl Static5d2bf0_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_5d2e20); }
// [$L_5d2bf0] the static initializer: FUN_5d2c00, then the teardown at exit
extern "C" int __cdecl Static5d2bf0_Init(void)
{
    CallC(q197_FUN_5d2c00);
    return Static5d2bf0_AtExit();
}
// [$L_5d3030] registers the teardown at exit
extern "C" int __cdecl Static5d3000_AtExit(void) { return atexit((void(__cdecl*)(void))q197_L_5d3040); }
// [$L_5d3000] the static initializer: FUN_5d3010, then the teardown at exit
extern "C" int __cdecl Static5d3000_Init(void)
{
    CallC(q197_FUN_5d3010);
    return Static5d3000_AtExit();
}

extern "C" void FUN_5d2370(); KING_ALIAS("_FUN_5d2370", "@NameHash_Clear@8")
KING_VAR(char, g_q200_71e820, 71e820)
KING_VAR(void*, g_q200_71e828, 71e828)
KING_VAR(int, g_q200_71e844, 71e844)
KING_VAR(void*, g_q200_7200c8, 7200c8)
KING_VAR(void*, g_q200_7200cc, 7200cc)
// [$L_5d2b70] clears the name hash 71e820 and frees its table
extern "C" void __cdecl Static71e820_Destroy(void)
{
    CallThis(FUN_5d2370, &g_q200_71e820);
    operator delete(g_q200_71e828);
    g_q200_71e844 = 0;
}
// [$L_5d3040] frees the pool 7200c0's two blocks
extern "C" void __cdecl Pool7200c0_Release(void)
{
    operator delete(g_q200_7200c8);
    operator delete(g_q200_7200cc);
}

ASM_PROC(FUN_5fedbc)
ASM_PROC(FUN_5fef04)
extern "C" void FUN_5b94b0(); KING_ALIAS("_FUN_5b94b0", "@Rows_Get@12")
KING_VAR(BYTE*, g_q222_71e7ec, 71e7ec)
KING_VAR(int, g_q222_71e7f0, 71e7f0)
KING_VAR(int, g_q222_71e7f8, 71e7f8)
KING_VAR(int, g_q222_71e7bc, 71e7bc)
KING_VAR(BYTE*, g_q222_71e7e8, 71e7e8)
KING_VAR(int, g_q222_70f3d8, 70f3d8)
KING_VAR(BYTE*, g_q222_71e7c4, 71e7c4)
// [FUN_5d15f0] fills the rows 71e7dc..71e7d8 of the walked polygon (left edges in 70f3f0, right ones 31200 bytes
// on: x in 12.20 fixed point, z at +8) with colour c and its depth (z buffer 71e7e8, pitch 71e7f8 words; frame
// 71e7c4, pitch 71e7bc, 70f3d8 bytes a pixel) by 5fef04; with flag 1 the row table is chosen by k first
extern "C" void __cdecl Poly_FillZ(int flags, int c, int, float k)
{
    CallC(FUN_5fedbc, flags, 1);
    if (flags & 1) {
        BYTE* t = g_q222_71e7ec;
        g_q222_71e7f0 = CallThis<int>(FUN_5b94b0, t, (int)(F(int, t, 796) * k));
    }
    int y = g_q134_71e7dc;
    int zrow = g_q222_71e7f8 * y * 2;
    int row = y * g_q222_71e7bc;
    if (y > g_q134_71e7d8)
        return;
    for (int* e = (int*)((BYTE*)g_q134_70f3f0 + y * 24); y <= g_q134_71e7d8; y++, e += 6) {
        int l = e[0], r = e[7800];
        int x1 = r >> 20, x0 = l >> 20;
        int w = (r - l) >> 16;
        int dz = w < 16 ? 0 : ((e[7802] - e[2]) << 6) / w;
        int sub = (l >> 16) & 15;
        if (sub) {
            sub = 16 - sub;
            x0++;
        }
        if (x0 <= x1)
            CallC(FUN_5fef04, g_q222_70f3d8 * x0 + g_q222_71e7c4 + row, x1 - x0 + 1, 0, 0, c,
                  ((sub * dz) >> 6) + e[2], dz >> 2, g_q222_71e7e8 + x0 * 2 + zrow);
        zrow += g_q222_71e7f8 * 2;
        row += g_q222_71e7bc;
    }
}

ASM_PROC(FUN_5fe9b3)
ASM_PROC(FUN_5feace)
ASM_PROC(FUN_5fea6c)
ASM_PROC(FUN_5feafa)
ASM_PROC(FUN_5feb12)
KING_VAR(float, g_q223_64c458, 64c458)
// [FUN_5d1430] fills the rows 71e7dc..71e7d8 of the walked polygon (edges in 70f3f0 and 31200 bytes on, from the
// first pixel centre inside) with colour c; the mode is flags with the pixel size (2: 16 bits, 4: 32 bits) added:
// 0 plain bytes, 1 bytes through the row table chosen by k, 2 and 4 plain words / dwords, 3 words blended a through
// the table, 5 dwords blended a with k
extern "C" void __cdecl Poly_Fill(int flags, int c, int a, float k)
{
    if (g_q222_70f3d8 == 2)
        flags |= 2;
    else if (g_q222_70f3d8 == 4)
        flags |= 4;
    BYTE* row = g_q134_71e7dc * g_q222_71e7bc + g_q222_71e7c4;
    int table = 0;
    if ((flags & 1) && (g_q222_70f3d8 == 1 || g_q222_70f3d8 == 2)) {
        BYTE* t = g_q222_71e7ec;
        table = CallThis<int>(FUN_5b94b0, t, (int)(F(int, t, 796) * k));
    }
    int y = g_q134_71e7dc;
    if (y > g_q134_71e7d8)
        return;
    for (int* e = (int*)((BYTE*)g_q134_70f3f0 + y * 24 + 31200); y <= g_q134_71e7d8; y++, e += 6) {
        int l = e[-7800];
        int x0 = (l >> 20) + ((l & 0xfffff) != 0);
        int n = (e[0] >> 20) - x0 + 1;
        if (n > 0) {
            switch ((unsigned)flags) {
            case 0: memset(row + x0, (BYTE)c, n); break;
            case 1: CallC(FUN_5fe9b3, table, row + x0, c, n); break;
            case 2: CallC(FUN_5feace, row + x0 * 2, c, n); break;
            case 3: CallC(FUN_5fea6c, table, row + x0 * 2, a, n); break;
            case 4: CallC(FUN_5feafa, row + x0 * 4, c, n); break;
            case 5: CallC(FUN_5feb12, (int)(k * g_q223_64c458), row + x0 * 4, a, n); break;
            }
        }
        row += g_q222_71e7bc;
    }
}

// ---------------------------------------------------------------------------
// Software polygon drawing

extern "C" void FUN_5b6700(); KING_ALIAS("_FUN_5b6700", "@Window_RootSurface@8")
extern "C" void FUN_5ba010(); KING_ALIAS("_FUN_5ba010", "@Palette_Shade@16")
extern "C" void FUN_5ba070(); KING_ALIAS("_FUN_5ba070", "@Palette_Fog@16")
extern "C" void FUN_5ba170(); KING_ALIAS("_FUN_5ba170", "@Palette_FogColor@12")
extern "C" void FUN_5ba3f0(); KING_ALIAS("_FUN_5ba3f0", "@Palette_Color@12")
extern "C" void FUN_5ba5c0(); KING_ALIAS("_FUN_5ba5c0", "@Texture_MipWidth@12")
extern "C" void FUN_5ba5f0(); KING_ALIAS("_FUN_5ba5f0", "@Texture_MipPixels@12")
extern "C" void FUN_5cf630(); KING_ALIAS("_FUN_5cf630", "_Span_FillRows")
extern "C" void FUN_5d02b0(); KING_ALIAS("_FUN_5d02b0", "_Span_Edge")
extern "C" void FUN_5d0400(); KING_ALIAS("_FUN_5d0400", "_Span_EdgeV")
extern "C" void FUN_5d05e0(); KING_ALIAS("_FUN_5d05e0", "_Span_EdgeD")
extern "C" void FUN_5d0790(); KING_ALIAS("_FUN_5d0790", "_Span_EdgeDV")
extern "C" void FUN_5d0e40(); KING_ALIAS("_FUN_5d0e40", "_Span_EdgeUV")
extern "C" void FUN_5d1430(); KING_ALIAS("_FUN_5d1430", "_Poly_Fill")
extern "C" void FUN_5d15f0(); KING_ALIAS("_FUN_5d15f0", "_Poly_FillZ")
extern "C" void FUN_5cf480(); KING_ALIAS("_FUN_5cf480", "_Span_FillRowsU")    // shaded rows with z (fog, colour)
extern "C" void FUN_5cf920(); KING_ALIAS("_FUN_5cf920", "_Span_FillRowsRotated")    // textured rows, rotated (texture, mode)
extern "C" void FUN_5d0990(); KING_ALIAS("_FUN_5d0990", "_Span_EdgeUVP")    // edge with texture and shade
extern "C" void FUN_5d1030(); KING_ALIAS("_FUN_5d1030", "_Span_EdgeUVPersp")    // edge with texture
extern "C" void FUN_5d1740(); KING_ALIAS("_FUN_5d1740", "_Span_FillRowsUV")    // textured rows (texture, mode, step, level)
extern "C" void FUN_5d18d0(); KING_ALIAS("_FUN_5d18d0", "_Span_FillRowsUVZ")    // textured shaded rows (texture, mode, step)
extern "C" void FUN_5d1a80(); KING_ALIAS("_FUN_5d1a80", "_Spans_Fill")    // textured rows with z (texture, mode)
extern "C" void FUN_5d1c50(); KING_ALIAS("_FUN_5d1c50", "_Span_FillRowsAffine")    // textured rows, perspective (texture, mode)
extern "C" void FUN_5d1e50(); KING_ALIAS("_FUN_5d1e50", "_Span_FillRowsAffineFast")    // textured rows, perspective, transparent (texture, mode)
KING_VAR(int, g_spanFogColor71e7b0, 71e7b0)
KING_VAR(int, g_spanWidth71e7b4, 71e7b4)
KING_VAR(int, g_spanHeight71e7b8, 71e7b8)
KING_VAR(int, g_spanBpp70f3d8, 70f3d8)
KING_VAR(int, g_span71e7bc, 71e7bc)
KING_VAR(int, g_span71e7f8, 71e7f8)
KING_VAR(int, g_spanBits71e7c4, 71e7c4)
KING_VAR(WORD, g_spanPitch71e7f4, 71e7f4)
KING_VAR(int, g_span71e7e8, 71e7e8)
KING_VAR(BYTE*, g_spanSurface71e7ec, 71e7ec)
KING_VAR(int, g_texWidth70f3e4, 70f3e4)
KING_VAR(int, g_texPixels71e7c8, 71e7c8)
KING_VAR(WORD, g_tex71e7fc, 71e7fc)
KING_VAR(BYTE, g_texShift70f3dc, 70f3dc)
KING_VAR(int, g_texStep67fa7c, 67fa7c)
KING_VAR(int, g_span70f3e0, 70f3e0)
KING_VAR(int, g_span71e7d0, 71e7d0)
KING_VAR(DWORD, g_poly702e8c, 702e8c)

// the edges of the polygon of n vertices (44 bytes each), closing from the last to the first
static void Poly_Edges(void (*edge)(), BYTE* v, int n)
{
    int last = 0;
    for (int i = 0; i < n - 1; i++, last = i)
        CallC(edge, v + i * 44, v + (i + 1) * 44);
    CallC(edge, v + last * 44, v);
}

static bool Poly_Empty()
{
    return g_q134_71e7dc > g_q134_71e7d8;
}

extern "C" {

// [FUN_5cfb30] draws a polygon into the window's surface: flags 32 textured (16 perspective, 2 rotated 1 with
// z, 128 shaded), else 128 shaded or flat; fog (flag 64 on a 16-bit surface) and the z buffer as configured
void __cdecl Poly_Draw(DWORD flags, BYTE* win, int n, BYTE* v, BYTE* mat, int level)
{
    int idx = F(int, win, 16);
    g_spanBits71e7c4 = F(int, win, idx * 4 + 24);
    if (!g_spanBits71e7c4)
        return;
    g_spanPitch71e7f4 = F(WORD, win, idx * 4 + 56);
    g_span71e7e8 = F(int, win, 48);
    g_spanWidth71e7b4 = F(WORD, win, 0);
    g_spanHeight71e7b8 = F(WORD, win, 2);
    g_spanBpp70f3d8 = F(WORD, win, 4);
    g_span71e7bc = F(WORD, win, 6);
    g_span71e7f8 = F(WORD, win, 8);
    BYTE* s = CallThis<BYTE*>(FUN_5b6700, win);
    g_spanSurface71e7ec = s;
    if (!s)
        return;
    int fog = (F(int, s, 800) || g_spanBpp70f3d8 == 4) && (flags & 64) ? 4 : 0;
    if (F(DWORD, mat, 32) & 0x100)
        fog = 0;
    int z = (g_poly702e8c & 0x4000) && !(F(BYTE, mat, 32) & 128) ? 1 : 0;
    int two = (F(DWORD, mat, 32) >> 1) & 1;
    BYTE* tex = F(BYTE*, mat, 76);
    if (flags & 32) {
        g_texWidth70f3e4 = CallThis<int>(FUN_5ba5c0, tex, g_71e800);
        g_texPixels71e7c8 = CallThis<int>(FUN_5ba5f0, F(BYTE*, mat, 76), g_71e800);
        g_tex71e7fc = F(WORD, F(BYTE*, mat, 76), 28);
        g_texShift70f3dc = F(BYTE, F(BYTE*, mat, 76), 26) - (BYTE)g_71e800;
        g_texStep67fa7c = (g_spanWidth71e7b4 != 320) + 4;
        tex = F(BYTE*, mat, 76);
        if ((flags & 16) && !fog) {
            g_q134_71e7d8 = 0;
            g_q134_71e7dc = g_spanHeight71e7b8 - 1;
            Poly_Edges(FUN_5d0e40, v, n);
            if (!Poly_Empty())
                CallC(FUN_5d1e50, tex, two);
            return;
        }
        if ((flags & 2) && (flags & 1)) {
            g_q134_71e7dc = g_spanHeight71e7b8 - 1;
            g_q134_71e7d8 = 0;
            Poly_Edges(FUN_5d0e40, v, n);
            if (!Poly_Empty())
                CallC(FUN_5d1c50, tex, two | fog);
            return;
        }
        if (!(flags & 2) && (flags & 1)) {
            g_q134_71e7d8 = 0;
            g_q134_71e7dc = g_spanWidth71e7b4 - 1;
            for (int i = 0; i < n; i++) {
                BYTE* p = v + i * 44;
                int x = F(int, p, 12);
                F(int, p, 12) = F(int, p, 16);
                F(int, p, 16) = (g_spanWidth71e7b4 << 20) - x - 0x100000;
            }
            Poly_Edges(FUN_5d1030, v, n);
            if (!Poly_Empty())
                CallC(FUN_5cf920, tex, ((((F(DWORD, mat, 32) & 2) | 4) >> 1) | fog));
            return;
        }
        g_q134_71e7d8 = 0;
        g_q134_71e7dc = g_spanHeight71e7b8 - 1;
        if (flags & 2) {
            Poly_Edges(FUN_5d1030, v, n);
            if (!Poly_Empty())
                CallC(FUN_5d1a80, tex, two | fog);
            return;
        }
        g_span70f3e0 = F(int, CallThis<BYTE*>(FUN_5b6700, win), 16);
        if (flags & 128) {
            Poly_Edges(FUN_5d0990, v, n);
            if (!Poly_Empty())
                CallC(FUN_5d18d0, F(BYTE*, mat, 76), two | fog, g_texStep67fa7c);
        } else {
            Poly_Edges(FUN_5d1030, v, n);
            if (!Poly_Empty())
                CallC(FUN_5d1740, F(BYTE*, mat, 76), two | fog, g_texStep67fa7c, level);
        }
        return;
    }
    if (flags & 128) {
        if (fog)
            g_spanFogColor71e7b0 = CallThis<int>(FUN_5ba170, s, g_67faa0);
        int col = F(int, mat, 40);
        g_span71e7d0 = F(int, CallThis<BYTE*>(FUN_5b6700, win), 20);
        g_span70f3e0 = F(int, CallThis<BYTE*>(FUN_5b6700, win), 16);
        g_q134_71e7d8 = 0;
        g_q134_71e7dc = g_spanHeight71e7b8 - 1;
        Poly_Edges(z ? FUN_5d0790 : FUN_5d05e0, v, n);
        if (Poly_Empty())
            return;
        if (g_spanBpp70f3d8 != 1)
            col = CallThis<int>(FUN_5ba3f0, CallThis<BYTE*>(FUN_5b6700, win), col);
        CallC(z ? FUN_5cf480 : FUN_5cf630, fog, col);
        return;
    }
    g_q134_71e7d8 = 0;
    g_q134_71e7dc = g_spanHeight71e7b8 - 1;
    Poly_Edges(z ? FUN_5d0400 : FUN_5d02b0, v, n);
    if (Poly_Empty())
        return;
    int col = F(int, mat, 40);
    if (g_spanBpp70f3d8 != 1 && !(flags & 0x100))
        col = CallThis<int>(FUN_5ba3f0, g_spanSurface71e7ec, col);
    if ((g_poly702e8c & 64) && level > 10)
        col = CallThis<int>(FUN_5ba010, g_spanSurface71e7ec, col, level);
    if (fog)
        col = CallThis<int>(FUN_5ba070, g_spanSurface71e7ec, col, g_67faa0);
    int tr = (F(BYTE, mat, 32) & 32) ? 1 : 0;
    CallC(z ? FUN_5d15f0 : FUN_5d1430, tr, col, F(int, mat, 40), F(int, mat, 88));
}

}  // extern "C"

// === a textured, shaded polygon edge (FUN_5d0990)

extern "C" {

// [FUN_5d0990] like 5d0e40 with perspective: the texture coordinates (+24, +28 into +12, +16) follow the
// ratio of the interpolated +20 numerator to the depth (+20 into +8, at most 2^24 - 1 on the last row), the
// shade +40 goes dithered into +20; a coordinate changing by less than 15 stays constant
void __cdecl Span_EdgeUVP(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        BYTE* r = t + row * 24;
        F(int, r, 0) = F(int, p1, 12);
        F(int, r, 12) = F(int, p1, 24);
        F(int, r, 16) = F(int, p1, 28);
        F(int, r, 8) = F(int, p1, 20);
        F(int, r, 20) = F(int, p1, 40);
        if (row < g_q134_71e7dc)
            g_q134_71e7dc = row;
        if (row > g_q134_71e7d8)
            g_q134_71e7d8 = row;
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1024 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if ((F(int, p2, 16) >> 20) > g_q134_71e7d8)
        g_q134_71e7d8 = F(int, p2, 16) >> 20;
    int x = F(int, p1, 12), v = F(int, p1, 20);
    int num = 0, sx = 0, dnum = 0, dv = 0, sw = 0;
    if (dy) {
        sx = (F(int, p2, 12) - x) / dy;
        int v2 = F(int, p2, 20);
        if (dy > 2048) {
            int d = dy >> 4;
            dnum = (v2 << 6) / d;
            dv = ((v2 - v) << 6) / d;
            v += ((f >> 4) * dv) >> 6;
            num = (f >> 4) * dnum;
        } else {
            dnum = ((v2 << 6) / dy) << 4;
            dv = (((v2 - v) << 6) / dy) << 4;
            v += ((dv >> 4) * f) >> 6;
            num = (dnum >> 4) * f;
        }
        num >>= 6;
        sw = (F(int, p2, 40) - F(int, p1, 40)) / dy;
        x += sx * f;
    }
    int w = sw * f + F(int, p1, 40);
    sw <<= 10;
    int q = (num << 6) / (v >> 4);
    sx <<= 10;
    int u1 = F(int, p1, 24), t1 = F(int, p1, 28);
    int du = (F(int, p2, 24) - u1) >> 10;
    int dt = (F(int, p2, 28) - t1) >> 10;
    BYTE* r = t + row * 24;
    F(int, r, 12) = du * q + u1;
    F(int, r, 16) = dt * q + t1;
    F(int, r, 0) = x;
    F(int, r, 8) = v;
    F(int, r, 20) = w;
    bool fixU = (du < 0 ? -du : du) < 15;
    bool fixT = !fixU && (dt < 0 ? -dt : dt) < 15;
    const int* dith = (const int*)g_q136_67fa80;
    for (int k = row + 1; k <= F(int, p2, 16) >> 20; k++) {
        x += sx;
        v += dv;
        num += dnum;
        r += 24;
        q = (num << 6) / (v >> 4);
        F(int, r, 0) = x;
        F(int, r, 8) = v;
        F(int, r, 12) = fixU ? u1 : du * q + u1;
        F(int, r, 16) = fixT ? t1 : dt * q + t1;
        w += sw;
        F(int, r, 20) = dith[k & 7] + w;
    }
    if (F(int, r, 8) >= 16777216)
        F(int, r, 8) = 16777215;
}

} // extern "C"

// === a textured polygon edge with perspective (FUN_5d1030)

extern "C" {

// [FUN_5d1030] like 5d0990 without the shade: the texture coordinates (+24, +28 into +12, +16) follow the
// ratio of the interpolated +20 numerator to the depth (+20 into +8, at most 2^24 - 1 on the last row); a
// coordinate changing by less than 15 stays constant
void __cdecl Span_EdgeUVPersp(const BYTE* a, const BYTE* b)
{
    const BYTE *p1, *p2;
    int side = g_q134_723a88;
    if (F(int, a, 16) < F(int, b, 16)) {
        p1 = a;
        p2 = b;
        side = (side ^ 1) ? 0 : 1;
    } else {
        p1 = b;
        p2 = a;
        side ^= 1;
    }
    int y1 = F(int, p1, 16), y2 = F(int, p2, 16);
    int dy = (y2 - y1) >> 10;
    int frac = (y1 >> 10) & 1023;
    int row = y1 >> 20, last = y2 >> 20;
    BYTE* t = (BYTE*)g_q134_70f3f0 + side * 1300 * 24;
    if (row == last) {
        if (frac || !(y2 & 0xffc00))
            return;
        BYTE* r = t + row * 24;
        F(int, r, 0) = F(int, p1, 12);
        F(int, r, 12) = F(int, p1, 24);
        F(int, r, 16) = F(int, p1, 28);
        F(int, r, 8) = F(int, p1, 20);
        if (row < g_q134_71e7dc)
            g_q134_71e7dc = row;
        if (row > g_q134_71e7d8)
            g_q134_71e7d8 = row;
        return;
    }
    int f = frac;
    if (frac) {
        row++;
        f = 1024 - frac;
    }
    if (row < g_q134_71e7dc)
        g_q134_71e7dc = row;
    if ((F(int, p2, 16) >> 20) > g_q134_71e7d8)
        g_q134_71e7d8 = F(int, p2, 16) >> 20;
    int x = F(int, p1, 12), v = F(int, p1, 20);
    int num = 0, sx = 0, dnum = 0, dv = 0;
    if (dy) {
        sx = (F(int, p2, 12) - x) / dy;
        int v2 = F(int, p2, 20);
        if (dy > 2048) {
            int d = dy >> 4;
            dnum = (v2 << 6) / d;
            dv = ((v2 - v) << 6) / d;
            v += ((f >> 4) * dv) >> 6;
            num = (f >> 4) * dnum;
        } else {
            dnum = ((v2 << 6) / dy) << 4;
            dv = (((v2 - v) << 6) / dy) << 4;
            v += ((dv >> 4) * f) >> 6;
            num = (dnum >> 4) * f;
        }
        num >>= 6;
        x += sx * f;
    }
    int q = (num << 6) / (v >> 4);
    sx <<= 10;
    int u1 = F(int, p1, 24), t1 = F(int, p1, 28);
    int du = (F(int, p2, 24) - u1) >> 10;
    int dt = (F(int, p2, 28) - t1) >> 10;
    BYTE* r = t + row * 24;
    F(int, r, 12) = du * q + u1;
    F(int, r, 0) = x;
    F(int, r, 16) = dt * q + t1;
    F(int, r, 8) = v;
    bool fixU = (du < 0 ? -du : du) < 15;
    bool fixT = !fixU && (dt < 0 ? -dt : dt) < 15;
    for (int k = row + 1; k <= F(int, p2, 16) >> 20; k++) {
        v += dv;
        num += dnum;
        x += sx;
        r += 24;
        q = (num << 6) / (v >> 4);
        F(int, r, 0) = x;
        F(int, r, 8) = v;
        F(int, r, 12) = fixU ? u1 : du * q + u1;
        F(int, r, 16) = fixT ? t1 : dt * q + t1;
    }
    if (F(int, r, 8) >= 16777216)
        F(int, r, 8) = 16777215;
}

} // extern "C"

// === loading the message texts (FUN_5d2c00)

KING_VAR(char, s_mt67fba4, 67fba4)    // "Internal error, please con..."
KING_VAR(char, s_mt67fb98, 67fb98)    // "mess.txt"
KING_VAR(char, s_mt673398, 673398)    // "r"
extern "C" char* __cdecl fgets(char* s, int n, FILE* f);


static char* Mt_Dup(const char* s)
{
    char* p = (char*)operator new(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

extern "C" {

// [FUN_5d2c00] loads the 1000 message texts (71e858) from mess.txt: one quoted string per entry (lines without a
// quote skipped; \ddd octal escapes), the first three defaulting to the internal error text
void __cdecl Messages_Load()
{
    char** t = (char**)g_71e858;
    memset(t, 0, 4000);
    for (int i = 0; i < 3; i++)
        t[i] = Mt_Dup(&s_mt67fba4);
    FILE* f = fopen(&s_mt67fb98, &s_mt673398);
    if (!f)
        return;
    for (int i = 0; i < 1000; i++) {
        char line[120], text[108];
        int q;
        for (;;) {
            if FILE_EOF(f)
                goto next;
            fgets(line, 100, f);
            q = 0;
            while (line[q] && line[q] != '"')
                q++;
            if (line[q] == '"')
                break;
        }
        {
            int n = 0;
            for (char* p = line + q + 1; *p && *p != '"'; p++, n++) {
                if (*p == 92) {
                    int v = (p[1] << 6) + (((p[2] + 16) << 3) & 255) + p[3] - 48;
                    text[n] = (char)v;
                    p += 3;
                } else
                    text[n] = *p;
            }
            text[n] = 0;
            if (t[i])
                operator delete(t[i]);
            t[i] = Mt_Dup(text);
        }
    next:;
    }
}

} // extern "C"

// === textured span fill with u and v (FUN_5d1740)

extern "C" void FUN_5cf6f0(); KING_ALIAS("_FUN_5cf6f0", "_Span_DrawPerspective")
ASM_PROC(FUN_5feb87)
KING_VAR(int, g_q223_702e90, 702e90)
extern "C" void Sp_ShadeRow(); KING_ALIAS("_Sp_ShadeRow", "@Palette_ShadeRow@12")

// [FUN_5d1740] fills the spans of the rows 71e7dc..71e7d8 with u and v (+8.. +16 of the edges) by FUN_5cf6f0,
// counting the pixels (702e90); a shade above 50 with 702e8c bit 6 sets the fog colour (71e7b0) and mode bit 2
extern "C" void __cdecl Span_FillRowsUV(int unused, int mode, int a3, int shade)
{
    int y = g_q134_71e7dc;
    int row = y * g_span71e7bc;
    int row2 = g_span71e7f8 * y * 2;
    int alt = (g_poly702e8c >> 14) & 1;
    if (shade > 50 && (g_poly702e8c & 64)) {
        g_spanFogColor71e7b0 = CallThis<int>(Sp_ShadeRow, g_spanSurface71e7ec, shade);
        CallC(FUN_5feb87, mode | 4, alt);
    } else
        CallC(FUN_5feb87, mode, alt);
    for (y = g_q134_71e7dc; y <= g_q134_71e7d8; y++) {
        int* e = (int*)((BYTE*)g_q134_70f3f0 + y * 24);
        int dw = (e[7800] - e[0]) >> 16;
        int x = e[0] >> 20;
        int xr = e[7800] >> 20;
        int dv = 0, du = 0;
        if (dw >= 16) {
            du = (e[7802] << 6) / dw;
            dv = ((e[7802] - e[2]) << 6) / dw;
        }
        if (e[0] & 0xF0000)
            x++;
        if (x <= xr) {
            g_q223_702e90 += xr - x + 1;
            CallC(FUN_5cf6f0, x, xr, a3, e[7803], e[7804], e[3], e[4], dv, du, e[2], 0, g_spanBpp70f3d8 * x + row,
                mode, g_span71e7e8 + x * 2 + row2, 0, 0);
        }
        row2 += g_span71e7f8 * 2;
        row += g_span71e7bc;
    }
}

// === span fill with u, v and z (FUN_5d18d0)

// [FUN_5d18d0] like Span_FillRowsUV with z stepped too (the z buffer: 71e7e8, 71e7b4 a row), mode | 2; the
// subpixel test reuses the previous row's fraction (mode on the first row) as in the original
extern "C" void __cdecl Span_FillRowsUVZ(int unused, int mode, int a3)
{
    int y = g_q134_71e7dc;
    int row = y * g_span71e7bc;
    int row2 = g_spanWidth71e7b4 * y * 2;
    CallC(FUN_5feb87, mode | 2, (g_poly702e8c >> 14) & 1);
    int f = mode;
    for (y = g_q134_71e7dc; y <= g_q134_71e7d8; y++, row += g_span71e7bc, row2 += g_spanWidth71e7b4 * 2) {
        int* e = (int*)((BYTE*)g_q134_70f3f0 + y * 24);
        int l = e[0];
        int x = l >> 20;
        int xr = e[7800] >> 20;
        int dw = (e[7800] - l) >> 16;
        int z = e[5];
        if (f) {
            f = 16 - f;
            x++;
        }
        if (x > xr)
            continue;
        int du = 0, dv = 0, dz = 0;
        if (dw >= 16) {
            du = (e[7802] << 6) / dw;
            dv = ((e[7802] - e[2]) << 6) / dw;
            dz = (e[7805] - z) / (xr - x + 1);
        }
        f = (l >> 16) & 15;
        g_q223_702e90 += xr - x + 1;
        CallC(FUN_5cf6f0, x, xr, a3, e[7803], e[7804], e[3], e[4], dv, du, e[2], 0, g_spanBpp70f3d8 * x + row, mode,
            g_span71e7e8 + x * 2 + row2, z, dz);
    }
}

// === affine textured span fill (FUN_5d1c50)

ASM_PROC(FUN_5fee7c)
KING_VAR(int, g_q223_72007c, 72007c)

// fills the spans of the rows 71e7dc..71e7d8 with u and v (+12, +16 of the edges) stepped by the
// constant gradients of the middle row, counting pixels (702e90) and spans (72007c); mode bit 2 sets the fog
// colour (71e7b0) from each row's +8
static void Span_Affine(int mode, bool subpixel)
{
    int y = g_q134_71e7dc;
    int row = y * g_span71e7bc;
    int row2 = g_span71e7f8 * y * 2;
    CallC(FUN_5feb87, mode, (g_poly702e8c >> 14) & 1);
    int* m = (int*)((BYTE*)g_q134_70f3f0 + (g_q134_71e7dc + g_q134_71e7d8) / 2 * 24);
    int dw = (m[7800] - m[0]) >> 16;
    int du = 0, dv = 0;
    if (dw) {
        du = ((m[7803] - m[3]) << 4) / dw;
        dv = ((m[7804] - m[4]) << 4) / dw;
    }
    for (y = g_q134_71e7dc; y <= g_q134_71e7d8; y++, row += g_span71e7bc, row2 += g_span71e7f8 * 2) {
        int* e = (int*)((BYTE*)g_q134_70f3f0 + y * 24);
        int u = e[3], v = e[4];
        int xr = e[7800] >> 20;
        int x = e[0] >> 20;
        int frac = subpixel ? (e[0] >> 16) & 15 : 0;
        if (frac) {
            frac = 16 - frac;
            x++;
        }
        if (x > xr)
            continue;
        g_q223_702e90 += xr - x + 1;
        u += (frac * du) >> 4;
        v += (frac * dv) >> 4;
        if (mode & 4)
            g_spanFogColor71e7b0 = CallThis<int>(FUN_5ba170, g_spanSurface71e7ec, e[2]);
        CallC(FUN_5fee7c, g_spanBits71e7c4 + g_spanBpp70f3d8 * x + row, xr - x + 1, u, v, du, dv, e[2], 0,
            g_span71e7e8 + row2 + x * 2);
        g_q223_72007c++;
    }
}

// [FUN_5d1c50] the affine span fill with subpixel correction
extern "C" void __cdecl Span_FillRowsAffine(int unused, int mode) { Span_Affine(mode, true); }

// [FUN_5d1e50] Span_FillRowsAffine without the subpixel correction
extern "C" void __cdecl Span_FillRowsAffineFast(int unused, int mode) { Span_Affine(mode, false); }

// === filling a polygon's spans (FUN_5d1a80)

ASM_FUNC(Sf_Setup, FUN_5feb87)
ASM_FUNC(Sf_Span, FUN_5fee7c)
extern "C" void Sf_FogColor(); KING_ALIAS("_Sf_FogColor", "@Palette_FogColor@12")
KING_VAR(int, g_sf71e7dc, 71e7dc)    // the first row
KING_VAR(int, g_sf71e7d8, 71e7d8)    // the last row
KING_VAR(int, g_sf71e7bc, 71e7bc)    // the colour buffer's pitch
KING_VAR(int, g_sf71e7f8, 71e7f8)    // the depth buffer's pitch (in words)
KING_VAR(int, g_sf70f3d8, 70f3d8)    // bytes per pixel
KING_VAR(int, g_sf702e90, 702e90)    // pixels filled
KING_VAR(int, g_sf72007c, 72007c)    // spans filled
KING_VAR(int, g_sf71e7c4, 71e7c4)    // the colour buffer
KING_VAR(int, g_sf71e7e8, 71e7e8)    // the depth buffer
KING_VAR(BYTE*, g_sf71e7ec, 71e7ec)  // the palette
KING_VAR(int, g_sf71e7b0, 71e7b0)    // the fog colour
KING_VAR(unsigned, g_sf702e8c, 702e8c)
KING_VAR(BYTE, g_sf70f400, 70f400)   // the left edges, 24 bytes a row (x at -16, shade -8, u -4, v 0)

extern "C" {

// [FUN_5d1a80] fills the polygon's rows (71e7dc..71e7d8) between its left and right edges (16.16 fixed point,
// 31184 bytes apart) with FUN_5fee7c, stepping the texture coordinates across the span; with flag 4 the fog
// colour follows each row's shade
void __cdecl Spans_Fill(int unused, int flags)
{
    int off = g_sf71e7dc * g_sf71e7bc;
    int zoff = g_sf71e7f8 * g_sf71e7dc * 2;
    CallC(Sf_Setup, flags, (g_sf702e8c >> 14) & 1);
    int y = g_sf71e7dc;
    if (y > g_sf71e7d8)
        return;
    BYTE* e = &g_sf70f400 + y * 24;
    for (; y <= g_sf71e7d8; y++, e += 24, off += g_sf71e7bc, zoff += g_sf71e7f8 * 2) {
        int xr = F(int, e, 31184), xl = F(int, e, -16);
        int u = F(int, e, -4), v = F(int, e, 0);
        int last = xr >> 20, first = xl >> 20;
        int w = (xr - xl) >> 16;
        int du, dv;
        if (!w)
            du = dv = 0;
        else {
            du = ((F(int, e, 31196) - u) << 4) / w;
            dv = ((F(int, e, 31200) - v) << 4) / w;
        }
        int frac = (xl >> 16) & 15;
        if (frac) {
            frac = 16 - frac;
            first++;
        }
        if (first > last)
            continue;
        int at = g_sf70f3d8 * first + off;
        g_sf702e90 = g_sf702e90 + (last - first) + 1;
        u += (frac * du) >> 4;
        v += (frac * dv) >> 4;
        if (flags & 4)
            g_sf71e7b0 = CallThis<int>(Sf_FogColor, g_sf71e7ec, F(int, e, -8));
        CallC(Sf_Span, g_sf71e7c4 + at, last - first + 1, u, v, du, dv, F(int, e, -8), 0,
            g_sf71e7e8 + zoff + first * 2);
        g_sf72007c++;
    }
}

} // extern "C"
