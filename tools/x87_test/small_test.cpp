// small_test.cpp - differential test of the player lookup (Player_FromId) and the rope compare (Rope_Compare)
// against their original machine code (small_asm.cpp). run.ps1 pastes the C++ functions from src/ai into
// small_gen.cpp, which this file includes; the callees are stubs that log their calls.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

extern "C" unsigned char KingRdata[0x30000];
extern "C" unsigned char KingData[0x20000];
extern "C" unsigned char KingBss[0xA0000];
unsigned char KingRdata[0x30000];
unsigned char KingData[0x20000];
unsigned char KingBss[0xA0000];

#define KING_RDATA_BASE 0x64b000
#define KING_DATA_BASE 0x66c000
#define KING_BSS_BASE 0x68a820
#define KING_VAR(type, name, addr) static type& name = *(type*)(KingBss + (0x##addr - KING_BSS_BASE));
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

static std::vector<BYTE> g_log;
static void L(const void* p, size_t n) { g_log.insert(g_log.end(), (const BYTE*)p, (const BYTE*)p + n); }
static void Lid(int id) { L(&id, 4); }
static void Lp(const void* p) { L(&p, 4); }

// --- the AI list and its stubs
struct AiListNode {
    AiListNode* next;
    AiListNode* prev;
    BYTE m_8[84];
    int kind;
    DWORD flags;
};

extern "C" {
void** __cdecl AiList_FindActive(void** out, void* first, void* last)
{
    Lid(1); Lp(first); Lp(last);
    AiListNode* n = (AiListNode*)first;
    for (; n != last; n = n->next)
        if (n->kind == 1 && !(n->flags & 0x100))
            break;
    *out = n;
    return out;
}
void** __fastcall SchList_End(void** self, void*, void** out) { Lid(2); *out = *self; return out; }
void** __fastcall SchList_Begin(void** self, void*, void** out) { Lid(3); *out = *(void**)*self; return out; }
void* __cdecl Range_CallE(void** out, void* f, void* l, int) { AiList_FindActive(out, f, l); return out; }
int* __fastcall Int_Set(int* s, void*, int a) { *s = a; return s; }
}
// the C++ calls the list functions through these labels (different bodies, or the linker folds them into one)
extern "C" void FUN_4afc20() { Lid(-2); }
extern "C" void FUN_4afbf0() { Lid(-3); }

template <class R = int, class... A> static R CallThis(void (*f)(), void* self, A... a)
{
    if (f == FUN_4afc20)
        return (R)(INT_PTR)SchList_End((void**)self, 0, (void**)a...);
    return (R)(INT_PTR)SchList_Begin((void**)self, 0, (void**)a...);
}

// --- ropes and their stubs
struct RopeIt60 { int pos, root, d2, d3; char* cur; char* end; int d6[9]; };
static void Lit(const void* it)   // the fields an iterator is built with (the others are left over)
{
    const int* d = (const int*)it;
    L(d, 8);
    L(d + 4, 4);
}
static int Result(int id)
{
    return (int)((g_log.size() * 2654435761u + id) % 3) - 1;
}

extern "C" {
int __cdecl LexCompare(const signed char* a, const signed char* aEnd, const signed char* b, const signed char* bEnd)
{
    Lid(10); Lp(a); Lp(aEnd); Lp(b); Lp(bEnd);
    return Result(10);
}
int __cdecl Chars_CompareRope(const char* f, const char* l, RopeIt60 a, RopeIt60 b)
{
    Lid(11); Lp(f); Lp(l); Lit(&a); Lit(&b);
    return Result(11);
}
int __cdecl Rope_CompareChars(RopeIt60 a, RopeIt60 b, const char* f, const char* l)
{
    Lid(12); Lit(&a); Lit(&b); Lp(f); Lp(l);
    return Result(12);
}
int __cdecl Rope_CompareRope(RopeIt60 a, RopeIt60 b, RopeIt60 c, RopeIt60 d)
{
    Lid(13); Lit(&a); Lit(&b); Lit(&c); Lit(&d);
    return Result(13);
}
DWORD* __fastcall Rec60_Copy(DWORD* s, void*, const DWORD* src)
{
    if (src[4]) {
        memcpy(s, src, 60);
        return s;
    }
    s[0] = src[0];
    s[1] = src[1];
    s[4] = 0;
    return s;
}
static RopeIt60 Copy(const RopeIt60& s)
{
    RopeIt60 r;
    memset(&r, 0xCC, sizeof r);
    Rec60_Copy((DWORD*)&r, 0, (const DWORD*)&s);
    return r;
}
// the original forwarders: copies of the records into the call
int __cdecl Rec60_Call4667d0(int a, int b, RopeIt60 x, RopeIt60 y) { return Chars_CompareRope((const char*)a, (const char*)b, Copy(x), Copy(y)); }
int __cdecl Rec60_Call4668d0(RopeIt60 x, RopeIt60 y, int a, int b) { return Rope_CompareChars(Copy(x), Copy(y), (const char*)a, (const char*)b); }
int __cdecl Rec60_Call466a00(RopeIt60 a, RopeIt60 b, RopeIt60 c, RopeIt60 d) { return Rope_CompareRope(Copy(a), Copy(b), Copy(c), Copy(d)); }
}

#include "small_gen.cpp"

extern "C" void Player_FromId477b60();
extern "C" void Rope_Compare45a7d0();

static unsigned g_seed;
static unsigned Rnd() { g_seed = g_seed * 1664525u + 1013904223u; return g_seed >> 8; }

static AiListNode g_nodes[12];
static AiListNode** const g_list = (AiListNode**)(KingBss + (0x68b990 - 0x68a820));
static BYTE** const g_current = (BYTE**)(KingBss + (0x68d4f4 - 0x68a820));

static void MakeList(unsigned seed)
{
    g_seed = seed * 2654435761u + 7;
    int n = Rnd() % 10;   // nodes besides the head
    for (int i = 0; i <= n; i++) {
        g_nodes[i].next = &g_nodes[i == n ? 0 : i + 1];
        g_nodes[i].prev = &g_nodes[i == 0 ? n : i - 1];
        g_nodes[i].kind = Rnd() % 3;
        g_nodes[i].flags = Rnd() % 2 ? 0x100 : Rnd() % 0x400;
    }
    *g_list = &g_nodes[0];
    *g_current = (BYTE*)0x12345678;
}

static int TestPlayer(unsigned seed)
{
    int io1 = (int)Rnd(), io2;
    MakeList(seed);
    io1 = (int)Rnd();
    io2 = io1;
    g_log.clear();
    int* r1;
    __asm {
        lea eax, io1
        push eax
        call Player_FromId477b60
        add esp, 4
        mov r1, eax
    }
    std::vector<BYTE> log1 = g_log;
    BYTE* cur1 = *g_current;
    MakeList(seed);
    g_log.clear();
    int* r2 = Player_FromId(&io2);
    if ((r1 == &io1) != (r2 == &io2) || io1 != io2 || cur1 != *g_current || log1 != g_log) {
        printf("player seed %u: io %08x vs %08x, current %p vs %p, log %s\n", seed, io1, io2, cur1, *g_current,
               log1 == g_log ? "same" : "differs");
        return 1;
    }
    return 0;
}

static BYTE g_ropes[2][24];
static char g_chars[2][16];

static BYTE* MakeRope(int k)
{
    if (Rnd() % 5 == 0)
        return 0;
    BYTE* r = g_ropes[k];
    memset(r, 0xEE, 24);
    F(int, r, 0) = Rnd() % 16;
    F(BYTE, r, 8) = Rnd() % 2 ? 0 : (BYTE)(1 + Rnd() % 3);
    F(char*, r, 20) = g_chars[k];
    return r;
}

static int TestRope(unsigned seed)
{
    g_seed = seed * 2654435761u + 99;
    BYTE* a = MakeRope(0);
    BYTE* b = MakeRope(1);
    g_log.clear();
    int r1;
    __asm {
        push b
        push a
        call Rope_Compare45a7d0
        add esp, 8
        mov r1, eax
    }
    std::vector<BYTE> log1 = g_log;
    g_log.clear();
    int r2 = Rope_Compare(a, b);
    if (r1 != r2 || log1 != g_log) {
        printf("rope seed %u: %d vs %d, log %s\n", seed, r1, r2, log1 == g_log ? "same" : "differs");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv)
{
    unsigned cases = argc > 1 ? atoi(argv[1]) : 3000;
    int fails = 0;
    for (unsigned s = 0; s < cases && fails < 10; s++) {
        fails += TestPlayer(s);
        fails += TestRope(s);
    }
    printf("small: %u cases each of the player lookup and the rope compare: %d failures\n", cases, fails);
    return fails != 0;
}
