// core.cpp - the unit at 0x5cf180 of king.masm: intrusive list links, name
// hash tables, block pools, timed event queues, a memory-mapped file,
// millisecond timers and blit wrappers.
//
// Building with  nmake ASM_CORE=1  makes king.masm assemble the original
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
extern "C" void FUN_5c36e0(); KING_ALIAS("_FUN_5c36e0", "@Entry_Apply@16")
ASM_PROC(FUN_6021f0)
extern "C" void FUN_602510(); KING_ALIAS("_FUN_602510", "_Dp_SetPlayerName")
extern "C" void FUN_6022a0(); KING_ALIAS("_FUN_6022a0", "_Dp_Shutdown")
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

extern "C" int __cdecl strncmp(const char* a, const char* b, size_t n);
ASM_VAR(AsmLabel, s_dollars, $L_67fb20)          // "$$"

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
