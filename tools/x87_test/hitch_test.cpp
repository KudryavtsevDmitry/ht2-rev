// Differential test of the hitch solver: the original machine code (Hitch_Solve558f20, naked inline asm)
// against the C++ translation (Hitch_SolveC) on random trucks and trailers. Every callee is a stub that logs
// its call (with the bytes its pointer arguments point to) and acts deterministically, so both sides see the
// same world as long as they make the same calls; the logs, the return value and all memory are compared.
#include <windows.h>
#include <float.h>
#include <math.h>
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

extern "C" void Hitch_Solve558f20();
extern "C" DWORD __fastcall Hitch_SolveC(DWORD ecx, DWORD edx);
extern "C" unsigned g_cov[];
extern "C" unsigned g_ncov;

// --- the call log
static std::vector<BYTE> g_log;
static void L(const void* p, size_t n) { g_log.insert(g_log.end(), (const BYTE*)p, (const BYTE*)p + n); }
static int g_trace;
static void Lid(int id) { if (g_trace) printf("%d ", id); L(&id, 4); }
static void Lv(const void* v) { L(v, 12); }
static void Lf(float f) { L(&f, 4); }

// --- objects
enum { OBJ = 24576, BODY = 16384 };
static BYTE g_mem[4][OBJ];   // truck, trailer, truck body, trailer body
static BYTE* T = g_mem[0];
static BYTE* R = g_mem[1];
static BYTE* BT = g_mem[2];
static BYTE* BR = g_mem[3];
static int g_counter;

static void Lobj(const void* p)   // which object a self pointer is
{
    int k = -1;
    for (int i = 0; i < 4; i++)
        if ((BYTE*)p >= g_mem[i] && (BYTE*)p < g_mem[i] + OBJ)
            k = i * 100000 + (int)((BYTE*)p - g_mem[i]);
    Lid(k);
}

#define FV(p, off) ((float*)((BYTE*)(p) + (off)))

extern "C" {
float* __cdecl Vec3_Add(float* o, const float* a, const float* b)
{
    Lid(1); Lv(a); Lv(b);
    float r[3] = {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
    memcpy(o, r, 12);
    return o;
}
float* __cdecl Vec3_Cross(float* o, const float* a, const float* b)
{
    Lid(2); Lv(a); Lv(b);
    float r[3] = {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
    memcpy(o, r, 12);
    return o;
}
float* __cdecl Vec3_DivComponents(float* o, const float* a, const float* b)
{
    Lid(3); Lv(a); Lv(b);
    float r[3] = {a[0] / b[0], a[1] / b[1], a[2] / b[2]};
    memcpy(o, r, 12);
    return o;
}
float* __cdecl Vec3_DivScalar(float* o, const float* v, float k)
{
    Lid(4); Lv(v); Lf(k);
    float r[3] = {v[0] / k, v[1] / k, v[2] / k};
    memcpy(o, r, 12);
    return o;
}
double __cdecl Vec3_Length(const float* v)
{
    Lid(5); Lv(v);
    return sqrt((double)v[0] * v[0] + (double)v[1] * v[1] + (double)v[2] * v[2]);
}
float* __cdecl Vec3_Negated(float* o, const float* v)
{
    Lid(6); Lv(v);
    float r[3] = {-v[0], -v[1], -v[2]};
    memcpy(o, r, 12);
    return o;
}
float* __cdecl Vec3_Normalized(float* o, const float* v)
{
    Lid(7); Lv(v);
    double l = sqrt((double)v[0] * v[0] + (double)v[1] * v[1] + (double)v[2] * v[2]);
    float r[3] = {v[0], v[1], v[2]};
    if (l > 0) {
        r[0] = (float)(v[0] / l);
        r[1] = (float)(v[1] / l);
        r[2] = (float)(v[2] / l);
    }
    memcpy(o, r, 12);
    return o;
}
float* __cdecl Vec3_RotateBy(float* o, const float* v, const float* m)
{
    Lid(8); Lv(v); L(m, 48);
    float r[3];
    for (int i = 0; i < 3; i++)
        r[i] = v[0] * m[i] + v[1] * m[3 + i] + v[2] * m[6 + i];
    memcpy(o, r, 12);
    return o;
}
float* __cdecl Vec3_Scaled(float* o, const float* v, float s)
{
    Lid(9); Lv(v); Lf(s);
    float r[3] = {v[0] * s, v[1] * s, v[2] * s};
    memcpy(o, r, 12);
    return o;
}
void __fastcall Body_FrameIntegrate(BYTE* s, void*, float dt)
{
    Lid(10); Lobj(s); Lf(dt);
    for (int i = 0; i < 3; i++)
        *FV(s, 9012 + i * 4) += *FV(s, 9048 + i * 4) * dt;
}
float __fastcall Body_Get9508(BYTE* s, void*)
{
    Lid(11); Lobj(s);
    return *FV(s, 9508);
}
static float* GetVec(int id, BYTE* s, float* o, int off)
{
    Lid(id); Lobj(s);
    memcpy(o, s + off, 12);
    return o;
}
float* __fastcall Body_GetVec9012(BYTE* s, void*, float* o) { return GetVec(12, s, o, 9012); }
float* __fastcall Body_GetVec9024(BYTE* s, void*, float* o) { return GetVec(13, s, o, 9024); }
float* __fastcall Body_GetVec9048(BYTE* s, void*, float* o) { return GetVec(14, s, o, 9048); }
float* __fastcall Body_GetVec9084(BYTE* s, void*, float* o) { return GetVec(15, s, o, 9084); }
float* __fastcall Body_GetVec9144(BYTE* s, void*, float* o) { return GetVec(16, s, o, 9144); }
void __fastcall Body_Integrate(BYTE* s, void*, const float* f, const float* t)
{
    Lid(17); Lobj(s); Lv(f); Lv(t);
    for (int i = 0; i < 3; i++) {
        *FV(s, 9048 + i * 4) += f[i] * 0.01f;
        *FV(s, 9084 + i * 4) += t[i] * 0.02f;
    }
}
float* __fastcall Body_Matrix2274(BYTE* s, void*, float* o)
{
    Lid(18); Lobj(s);
    memcpy(o, s + 0x2274, 48);
    return o;
}
float* __fastcall Body_Matrix22a4(BYTE* s, void*, float* o)
{
    Lid(19); Lobj(s);
    memcpy(o, s + 0x22a4, 48);
    return o;
}
void __fastcall Body_SetSpin(BYTE* s, void*, float x, float y, float z)
{
    Lid(20); Lobj(s); Lf(x); Lf(y); Lf(z);
    *FV(s, 9084) = x;
    *FV(s, 9088) = y;
    *FV(s, 9092) = z;
}
void __fastcall Body_SetVec9012(BYTE* s, void*, int a, int b, int c)
{
    Lid(21); Lobj(s); L(&a, 4); L(&b, 4); L(&c, 4);
    *(int*)(s + 9012) = a;
    *(int*)(s + 9016) = b;
    *(int*)(s + 9020) = c;
}
static int g_steps;   // StateStep calls in this run
int __fastcall Body_StateStep(BYTE* s)
{
    Lid(22); Lobj(s);
    *FV(s, 9500) += 1.0f;
    int& left = *(int*)(s + 9600);   // steps this body still takes (set by MakeWorld)
    if (left <= 0 || ++g_steps > 64)
        return 0;
    left--;
    if ((*(DWORD*)(s + 9604) >> (20 + left)) & 1)   // sometimes the body leaves its state (+13124)
        *(int*)(s + 13124) = 1;
    return (int)((*(DWORD*)(s + 9604) >> (left * 2)) & 3) - 1;   // -1, 0, 1 or 2
}
void __fastcall Car_PlaceWithTrailer(BYTE* s, void*, const BYTE* a, const BYTE* b)
{
    Lid(23); Lobj(s); L(a, 48); L(b, 48);
}
int __fastcall Engine_AddContact(BYTE* s, void*, const float* f, const float* p, const float* q)
{
    Lid(24); Lobj(s); Lv(f); Lv(p); Lv(q);
    return ++g_counter;
}
int __fastcall Engine_AddPush(BYTE* s, void*, const float* a, const float* b)
{
    Lid(25); Lobj(s); Lv(a); Lv(b);
    return ++g_counter;
}
int __fastcall Hitch_Align(BYTE* s, void*, float* m1, float* m2)
{
    Lid(26); Lobj(s); L(m1, 48); L(m2, 48);
    return (*(DWORD*)(s + 21700) >> 5) & 1;
}
double __fastcall Trailer_SaddleLoad(BYTE* s, void*, const float* dir)
{
    Lid(27); Lobj(s); Lv(dir);
    return dir[0] * *FV(s, 9200) + dir[1] * *FV(s, 9204) + dir[2] * *FV(s, 9208);
}
float* __fastcall Vec3_AddAssign(float* self, void*, float* ret, const float* v)
{
    Lid(28); Lv(self); Lv(v);
    for (int i = 0; i < 3; i++)
        self[i] = (float)((double)v[i] + self[i]);
    memcpy(ret, self, 12);
    return ret;
}
float* __fastcall Vec3_MulAssign(float* self, void*, float* ret, float s)
{
    Lid(29); Lv(self); Lf(s);
    for (int i = 0; i < 3; i++)
        self[i] = (float)((double)s * self[i]);
    memcpy(ret, self, 12);
    return ret;
}
float* __fastcall Vec3_SetThis(float* self, void*, float x, float y, float z)
{
    Lid(30); Lf(x); Lf(y); Lf(z);
    self[0] = x;
    self[1] = y;
    self[2] = z;
    return self;
}
float* __fastcall Vec3_ZeroThis(float* self, void*)
{
    Lid(31);
    self[0] = self[1] = self[2] = 0;
    return self;
}
}

// the hitch solver 67a4ac: (mc, mt, 6 vectors, 4 matrices, 2 vectors) in, 4 vectors out
static void __cdecl HjStub(int mc, int mt, float* a0, float* a1, float* a2, float* a3, float* a4, float* a5,
                           float* m0, float* m1, float* m2, float* m3, float* v0, float* v1, float* o0, float* o1,
                           float* o2, float* o3)
{
    Lid(40); L(&mc, 4); L(&mt, 4);
    float* in[8] = {a0, a1, a2, a3, a4, a5, v0, v1};
    for (int i = 0; i < 8; i++)
        Lv(in[i]);
    float* ms[4] = {m0, m1, m2, m3};
    for (int i = 0; i < 4; i++)
        L(ms[i], 48);
    float* out[4] = {o0, o1, o2, o3};
    for (int i = 0; i < 4; i++)
        for (int k = 0; k < 3; k++)
            out[i][k] = (in[i][k] - in[i + 4][k]) * 0.25f + m0[i * 3 % 12 + k] * 0.125f;
}

// --- random worlds
static unsigned g_seed;
static unsigned Rnd() { g_seed = g_seed * 1664525u + 1013904223u; return g_seed >> 8; }
static float RndF()
{
    unsigned k = Rnd() % 1000;
    if (k == 0)
        return 0.0f;
    if (k == 1) {
        DWORD n = 0x7FC00000;
        return *(float*)&n;
    }
    if (k < 20)
        return (float)((int)(Rnd() % 2001) - 1000) * 100.0f;
    if (k < 60)
        return (float)((int)(Rnd() % 2001) - 1000) * 1e-6f;
    return (float)((int)(Rnd() % 20001) - 10000) / 1000.0f;
}
static float g_scale = 1.0f;   // magnitude of this world's floats
static void Fill(BYTE* p, size_t n)
{
    for (size_t i = 0; i + 4 <= n; i += 4)
        *(float*)(p + i) = RndF() * g_scale;
}

static BYTE g_rdataOrig[0x30000];

static void LoadRdata(const char* exe)
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
    for (DWORD k = 0; k < sizeof g_rdataOrig; k++) {
        DWORD va = 0x64b000 + k - base;
        for (int s = 0; s < nt->FileHeader.NumberOfSections; s++)
            if (va >= sec[s].VirtualAddress && va < sec[s].VirtualAddress + sec[s].SizeOfRawData &&
                sec[s].PointerToRawData + va - sec[s].VirtualAddress < n)
                g_rdataOrig[k] = img[sec[s].PointerToRawData + va - sec[s].VirtualAddress];
    }
}

static void MakeWorld(unsigned seed)
{
    g_seed = seed * 2654435761u + 12345;
    memcpy(KingRdata, g_rdataOrig, sizeof KingRdata);
    static const float scales[6] = {0.01f, 0.1f, 1.0f, 1.0f, 10.0f, 100.0f};
    g_scale = 1.0f;
    Fill(KingBss, sizeof KingBss);
    Fill(KingData, sizeof KingData);
    *(DWORD*)(KingData + 58540) = (DWORD)HjStub;
    *(int*)(KingBss + 412328) = Rnd() % 2;
    g_scale = scales[Rnd() % 6];
    for (int i = 0; i < 4; i++)
        Fill(g_mem[i], OBJ);
    g_scale = 1.0f;
    *(int*)(T + 10420) = Rnd() % 3;
    *(int*)(R + 10420) = Rnd() % 3;
    *(BYTE**)(T + 21600) = BT;
    *(BYTE**)(R + 21600) = BR;
    *(BYTE**)(T + 21880) = R;
    *(int*)(T + 21892) = (int)(Rnd() % 4) - 1;
    *(int*)(R + 21892) = (int)(Rnd() % 4) - 1;
    *(int*)(BT + 13124) = Rnd() % 2;
    *(int*)(BR + 13124) = Rnd() % 2;
    for (int i = 2; i < 4; i++) {
        *(int*)(g_mem[i] + 9600) = Rnd() % 6;
        *(DWORD*)(g_mem[i] + 9604) = Rnd() * 7919u;
    }
    // some worlds near rest: small velocities and spins, unit-ish matrices
    if (Rnd() % 3 == 0)
        for (int i = 2; i < 4; i++)
            for (int k = 0; k < 9; k++) {
                *FV(g_mem[i], 9048 + k * 4) *= 0.001f;
                *FV(g_mem[i], 9084 + k * 4) *= 0.001f;
            }
}

struct Result {
    DWORD ret;
    std::vector<BYTE> log, mem;
};

static void Snapshot(Result& r)
{
    r.mem.clear();
    for (int i = 0; i < 4; i++)
        r.mem.insert(r.mem.end(), g_mem[i], g_mem[i] + OBJ);
    r.mem.insert(r.mem.end(), KingBss, KingBss + sizeof KingBss);
    r.mem.insert(r.mem.end(), KingData, KingData + sizeof KingData);
    r.log = g_log;
}

static DWORD RunAsm()
{
    DWORD r;
    BYTE* self = T;
    __asm {
        mov ecx, self
        call Hitch_Solve558f20
        mov r, eax
    }
    return r;
}

static int Compare(const Result& a, const Result& b, unsigned seed, const char* mode)
{
    if (a.ret != b.ret) {
        printf("seed %u %s: return %u vs %u\n", seed, mode, a.ret, b.ret);
        return 1;
    }
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
            printf("seed %u %s: memory differs at %u (block %u offset %u)\n", seed, mode, (unsigned)i,
                   (unsigned)(i / OBJ), (unsigned)(i % OBJ));
            return 1;
        }
    return 0;
}

int main(int argc, char** argv)
{
    unsigned cases = argc > 1 ? atoi(argv[1]) : 1000;
    g_trace = getenv("HT_TRACE") != 0;
    setvbuf(stdout, 0, _IONBF, 0);
    LoadRdata(argc > 2 ? argv[2] : "king_original.exe");
    const unsigned modes[2] = {_PC_53, _PC_24};
    const char* names[2] = {"pc53", "pc24"};
    int fails = 0;
    Result a, b;
    for (unsigned seed = 0; seed < cases && fails < 10; seed++)
        for (int m = 0; m < 2; m++) {
            unsigned cw;
            _controlfp_s(&cw, modes[m], _MCW_PC);
            MakeWorld(seed);
            g_log.clear();
            g_counter = 0;
            g_steps = 0;
            if (g_trace) printf("\nasm: ");
            a.ret = RunAsm();
            Snapshot(a);
            MakeWorld(seed);
            g_log.clear();
            g_counter = 0;
            g_steps = 0;
            if (g_trace) printf("\nC:   ");
            fflush(stdout);
            b.ret = Hitch_SolveC((DWORD)T, 0);
            Snapshot(b);
            _controlfp_s(&cw, _PC_53, _MCW_PC);
            fails += Compare(a, b, seed, names[m]);
        }
    unsigned hit = 0;
    for (unsigned i = 0; i < g_ncov; i++)
        hit += g_cov[i] != 0;
    printf("%u cases x 2 precisions: %d failures; branch directions covered %u of %u\n", cases, fails, hit, g_ncov);
    for (unsigned i = 0; i < g_ncov; i++)
        if (!g_cov[i])
            printf("  not covered: branch %u %s\n", i / 2, i % 2 ? "taken" : "not taken");
    return fails != 0;
}
