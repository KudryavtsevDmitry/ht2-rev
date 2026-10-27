// copter_test.cpp - differential test of the helicopter step: the original machine code (Copter_Step563600,
// copter_asm.cpp) against the C++ translation (Copter_StepC, generated with branch counters by run.ps1) on
// random helicopters. The callees are stubs that log their calls (with the bytes behind their pointer
// arguments) and act deterministically; the logs and all memory are compared.
#include <windows.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <vector>

extern "C" unsigned char KingRdata[0x30000];
extern "C" unsigned char KingData[0x20000];
extern "C" unsigned char KingBss[0xA0000];
unsigned char KingRdata[0x30000];
unsigned char KingData[0x20000];
unsigned char KingBss[0xA0000];

extern "C" void Copter_Step563600();
extern "C" DWORD __fastcall Copter_StepC(DWORD ecx, DWORD edx);
extern "C" unsigned g_cov[];
extern "C" unsigned g_ncov;

// the old runtime's _ftol (asm_crt.asm), which the game also uses for the compiler's __ftol2
extern "C" __declspec(naked) void _ftol()
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -12
        wait
        fnstcw word ptr [ebp-2]
        wait
        mov ax, word ptr [ebp-2]
        or ah, 12
        mov word ptr [ebp-4], ax
        fldcw word ptr [ebp-4]
        fistp qword ptr [ebp-12]
        fldcw word ptr [ebp-2]
        mov eax, dword ptr [ebp-12]
        mov edx, dword ptr [ebp-8]
        leave
        ret
    }
}
#pragma comment(linker, "/alternatename:__ftol2=__ftol")
#pragma comment(linker, "/alternatename:__ftol2_sse=__ftol")

static std::vector<BYTE> g_log;
static int g_trace;
static void L(const void* p, size_t n) { g_log.insert(g_log.end(), (const BYTE*)p, (const BYTE*)p + n); }
static void Lid(int id) { if (g_trace) printf("%d ", id); L(&id, 4); }
static void Lv(const void* v) { L(v, 12); }
static void Lf(float f) { L(&f, 4); }
static void Li(int i) { L(&i, 4); }

enum { OBJ = 8192 };
static BYTE g_copter[OBJ];
static float g_dt;
static unsigned g_rand;
static unsigned Hash(const void* p, size_t n)
{
    unsigned h = 2166136261u;
    for (size_t i = 0; i < n; i++)
        h = (h ^ ((const BYTE*)p)[i]) * 16777619u;
    return h;
}
static float HashF(unsigned h) { return (float)((int)(h % 20001) - 10000) / 1000.0f; }

extern "C" {
double __cdecl Game_FrameTime() { Lid(1); return g_dt; }
float __cdecl Angle_F2Pi(float x, const char* name)
{
    Lid(2); Lf(x); L(&name, 4);
    return x - floorf(x / 6.2831855f) * 6.2831855f;
}
int __cdecl Copter_PlaneCircle(const float* a, const float* n, const float* c, const float* axis, float d,
                               const float* v, float r, float* at, float* angle, float* dir)
{
    Lid(3); Lv(a); Lv(n); Lv(c); Lv(axis); Lf(d); Lv(v); Lf(r);
    unsigned h = Hash(g_log.data() + g_log.size() - 80, 80);
    for (int i = 0; i < 3; i++) {
        at[i] = a[i] + HashF(h + i) * 0.1f;
        dir[i] = HashF(h * 7 + i) * 0.1f;
    }
    *angle = HashF(h * 13);
    return (h >> 7) % 3 != 0;
}
float* __cdecl Matrix_RotationZ(float* out, float angle)
{
    Lid(4); Lf(angle);
    float c = cosf(angle), s = sinf(angle);
    float m[12] = {c, s, 0, -s, c, 0, 0, 0, 1, 0, 0, 0};
    memcpy(out, m, 48);
    return out;
}
// (a, b: ids; c: a float; d, e: the segment's ends, vectors on the caller's stack)
static int Query(int id, int a, int b, int c, int d, int e, float* point, float* normal)
{
    Lid(id); Li(a); Li(b); Li(c); Lv((const void*)d); Lv((const void*)e);
    unsigned h = Hash(g_log.data() + g_log.size() - 36, 36);
    for (int i = 0; i < 3; i++) {
        point[i] = HashF(h + i);
        normal[i] = HashF(h * 3 + i) * 0.1f;
    }
    normal[2] = HashF(h * 5) * 0.1f;   // -1..1: steep and flat ground
    return (h >> 5) % 4 != 0;
}
int __cdecl Query_4e7261(int a, int b, int c, int d, int e, float* point, float* normal) { return Query(5, a, b, c, d, e, point, normal); }
int __cdecl Query_4e7566(int a, int b, int c, int d, int e, float* point, float* normal) { return Query(6, a, b, c, d, e, point, normal); }
double __cdecl Vec3_Length(const float* v)
{
    Lid(7); Lv(v);
    return sqrt((double)v[0] * v[0] + (double)v[1] * v[1] + (double)v[2] * v[2]);
}
float* __cdecl Vec3_Normalized(float* o, const float* v)
{
    Lid(8); Lv(v);
    double l = sqrt((double)v[0] * v[0] + (double)v[1] * v[1] + (double)v[2] * v[2]);
    float r[3] = {v[0], v[1], v[2]};
    if (l > 0)
        for (int i = 0; i < 3; i++)
            r[i] = (float)(v[i] / l);
    memcpy(o, r, 12);
    return o;
}
static jmp_buf g_fatal;   // World_Fatal ends the run, as it ends the game
void __cdecl World_Fatal(const char* msg)
{
    Lid(9); L(&msg, 4);
    longjmp(g_fatal, 1);
}
int __cdecl rand()
{
    Lid(10);
    g_rand = g_rand * 214013u + 2531011u;
    return (g_rand >> 16) & 0x7FFF;
}
float __fastcall Copter_SideDrift(BYTE* s, void*, const float* p)
{
    Lid(11); Lv(p); Li((int)(s - g_copter));
    return p[0] * *(float*)(s + 76) + p[1] * *(float*)(s + 104) + p[2] * *(float*)(s + 52);
}
float* __fastcall Vec3_AddAssign(float* self, void*, float* ret, const float* v)
{
    Lid(12); Lv(self); Lv(v);
    for (int i = 0; i < 3; i++)
        self[i] = (float)((double)v[i] + self[i]);
    memcpy(ret, self, 12);
    return ret;
}
}

// --- random helicopters
static unsigned g_seed;
static unsigned Rnd() { g_seed = g_seed * 1664525u + 1013904223u; return g_seed >> 8; }
static float g_scale = 1.0f;
static void Fill(BYTE* p, size_t n)
{
    for (size_t i = 0; i + 4 <= n; i += 4) {
        unsigned k = Rnd() % 1000;
        if (k < 40)
            *(int*)(p + i) = 0;
        else if (k < 70)
            *(int*)(p + i) = Rnd() % 16;
        else if (k < 72)
            *(DWORD*)(p + i) = 0x7FC00000;
        else if (k < 100)
            *(float*)(p + i) = (float)((int)(Rnd() % 2001) - 1000) * 1e-4f * g_scale;
        else
            *(float*)(p + i) = (float)((int)(Rnd() % 20001) - 10000) / 1000.0f * g_scale;
    }
}

static BYTE g_rdataOrig[sizeof KingRdata];
static BYTE g_dataOrig[sizeof KingData];

static void LoadImage(const char* exe)
{
    FILE* f = fopen(exe, "rb");
    if (!f) {
        printf("cannot open %s\n", exe);
        exit(1);
    }
    static BYTE img[4 << 20];
    size_t n = fread(img, 1, sizeof img, f);
    fclose(f);
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(img + ((IMAGE_DOS_HEADER*)img)->e_lfanew);
    IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    DWORD base = nt->OptionalHeader.ImageBase;
    struct { DWORD va; BYTE* dst; DWORD size; } parts[2] = {{0x64b000, g_rdataOrig, sizeof g_rdataOrig},
                                                           {0x66c000, g_dataOrig, sizeof g_dataOrig}};
    for (int p = 0; p < 2; p++)
        for (DWORD k = 0; k < parts[p].size; k++) {
            DWORD rva = parts[p].va + k - base;
            for (int s = 0; s < nt->FileHeader.NumberOfSections; s++)
                if (rva >= sec[s].VirtualAddress && rva < sec[s].VirtualAddress + sec[s].SizeOfRawData &&
                    sec[s].PointerToRawData + rva - sec[s].VirtualAddress < n)
                    parts[p].dst[k] = img[sec[s].PointerToRawData + rva - sec[s].VirtualAddress];
        }
}

static void MakeWorld(unsigned seed)
{
    g_seed = seed * 2654435761u + 4242;
    memcpy(KingRdata, g_rdataOrig, sizeof KingRdata);
    memcpy(KingData, g_dataOrig, sizeof KingData);
    g_scale = 1.0f;
    Fill(KingBss, sizeof KingBss);
    static const float scales[6] = {0.01f, 0.1f, 1.0f, 1.0f, 10.0f, 100.0f};
    g_scale = scales[Rnd() % 6];
    Fill(g_copter, OBJ);
    g_scale = 1.0f;
    // the fields the step reads as integers: the state (+3236) and counters/flags
    *(int*)(g_copter + 3236) = Rnd() % 13;
    static const int ints[6] = {3208, 3432, 3440, 3460, 3464, 3484};
    for (int i = 0; i < 6; i++)
        *(int*)(g_copter + ints[i]) = (int)(Rnd() % 6) - 1;
    static const float dts[5] = {0.0f, 0.001f, 0.02f, 0.05f, 0.2f};
    g_dt = dts[Rnd() % 5];
    g_rand = Rnd();
}

struct Result {
    DWORD ret;
    std::vector<BYTE> log, mem;
};

static void Snapshot(Result& r)
{
    r.mem.assign(g_copter, g_copter + OBJ);
    r.mem.insert(r.mem.end(), KingBss, KingBss + sizeof KingBss);
    r.mem.insert(r.mem.end(), KingData, KingData + sizeof KingData);
    r.log = g_log;
}

static DWORD RunAsm()
{
    DWORD r;
    BYTE* self = g_copter;
    __asm {
        mov ecx, self
        call Copter_Step563600
        mov r, eax
    }
    return r;
}

static int Compare(const Result& a, const Result& b, unsigned seed, const char* mode)
{
    // the step is void: its caller (FUN_5687b0) ignores EAX, which holds what a float-returning callee left there
    size_t n = a.log.size() < b.log.size() ? a.log.size() : b.log.size();
    for (size_t i = 0; i < n; i++)
        if (a.log[i] != b.log[i]) {
            printf("seed %u %s: call log differs at byte %u (of %u / %u)\n", seed, mode, (unsigned)i,
                   (unsigned)a.log.size(), (unsigned)b.log.size());
            return 1;
        }
    if (a.log.size() != b.log.size()) {
        printf("seed %u %s: call log length %u vs %u\n", seed, mode, (unsigned)a.log.size(), (unsigned)b.log.size());
        return 1;
    }
    for (size_t i = 0; i < a.mem.size(); i++)
        if (a.mem[i] != b.mem[i]) {
            printf("seed %u %s: memory differs at %u (%s offset %u)\n", seed, mode, (unsigned)i,
                   i < OBJ ? "copter" : i < OBJ + sizeof KingBss ? "bss" : "data",
                   (unsigned)(i < OBJ ? i : i < OBJ + sizeof KingBss ? i - OBJ : i - OBJ - sizeof KingBss));
            return 1;
        }
    return 0;
}

int main(int argc, char** argv)
{
    unsigned cases = argc > 1 ? atoi(argv[1]) : 1000;
    LoadImage(argc > 2 ? argv[2] : "king_original.exe");
    g_trace = getenv("HT_TRACE") != 0;
    setvbuf(stdout, 0, _IONBF, 0);
    const unsigned modes[2] = {_PC_53, _PC_24};
    const char* names[2] = {"pc53", "pc24"};
    int fails = 0;
    Result a, b;
    unsigned first = argc > 3 ? atoi(argv[3]) : 0;   // the first seed (to rerun one world)
    for (unsigned seed = first; seed < first + cases && fails < 10; seed++)
        for (int m = 0; m < 2; m++) {
            unsigned cw;
            _controlfp_s(&cw, modes[m], _MCW_PC);
            MakeWorld(seed);
            g_log.clear();
            if (g_trace) printf("\nasm: ");
            if (!setjmp(g_fatal))
                a.ret = RunAsm();
            Snapshot(a);
            MakeWorld(seed);
            g_log.clear();
            if (g_trace) printf("\nC:   ");
            if (!setjmp(g_fatal))
                b.ret = Copter_StepC((DWORD)g_copter, 0);
            Snapshot(b);
            _controlfp_s(&cw, _PC_53, _MCW_PC);
            fails += Compare(a, b, seed, names[m]);
        }
    unsigned hit = 0;
    for (unsigned i = 0; i < g_ncov; i++)
        hit += g_cov[i] != 0;
    printf("copter: %u cases x 2 precisions: %d failures; branch directions covered %u of %u\n", cases, fails, hit,
           g_ncov);
    for (unsigned i = 0; i < g_ncov; i++)
        if (!g_cov[i])
            printf("  not covered: branch %u %s\n", i / 2, i % 2 ? "taken" : "not taken");
    return fails != 0;
}
