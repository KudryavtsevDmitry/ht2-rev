// ai_saveload.cpp - ai_svld.cpp of the AI, rewritten from king.masm: helpers
// of its save files.

#include "king.h"
#include <objbase.h>
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "_Ai_Clock")
extern "C" char** __fastcall CharVec_Construct(char** self, void*, const char* s);
extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector<pointer>: a new buffer (n, first, last)
extern "C" void __cdecl Ai_PoolFree(void* p, unsigned size);
extern "C" BYTE* __fastcall CharVec_Erase(void* s, void*, BYTE* first, BYTE* last);
extern "C" void __fastcall Bytes_Reserve(BYTE** self, void*, unsigned n);
extern "C" void FUN_47ab20(); KING_ALIAS("_FUN_47ab20", "@CharVec_PushBack@12")
extern "C" void FUN_410df0(); KING_ALIAS("_FUN_410df0", "@CharVec_Str@8")
extern "C" void FUN_407320(); KING_ALIAS("_FUN_407320", "@CharVec_Equals@12")
extern "C" void FUN_483c10(); KING_ALIAS("_FUN_483c10", "_AI_UpdateSwitch")
extern "C" int __cdecl Save_ParseHeader(int** cur, int* id, int** data, int* size);
extern "C" int __cdecl Save_BlockEnd(BYTE** out, BYTE* data, int size);
extern "C" BYTE* __cdecl Players_Append(int kind);
extern "C" void __fastcall Player_Load(BYTE* self, void*, BYTE** cur);
extern "C" void __cdecl Save_RebaseTime(double* t);
extern "C" DWORD* __cdecl Player_Register(DWORD* out, BYTE* p);
extern "C" void __cdecl Rec_4015a0Both(int id);
extern "C" int* __fastcall Int_Set(int* s, void*, int a);
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
extern "C" void __fastcall CharVec_InsertRange(BYTE** self, void*, BYTE* pos, BYTE* first, BYTE* last);
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
extern "C" void __fastcall Player_Save(BYTE* self, void*, BYTE* s);
extern "C" BYTE* __cdecl AI_ListData(BYTE* node);
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
extern "C" void FUN_4810e0(); KING_ALIAS("_FUN_4810e0", "_Save_Town")
extern "C" void __cdecl Save_Present(BYTE** v, BYTE* p);
extern "C" void FUN_481790(); KING_ALIAS("_FUN_481790", "_Save_Present")
extern "C" void FUN_482320(); KING_ALIAS("_FUN_482320", "_Save_Ambush")
typedef char AsmLabel[1];
KING_VAR(AsmLabel, g_tagAMBU, 66fe74)
KING_VAR(AsmLabel, g_tagTOWN, 66fdf4)
KING_VAR(AsmLabel, g_tagTNDE, 66fdec)
KING_VAR(AsmLabel, s_svldId, 66fcb4)
KING_VAR(AsmLabel, s_svldFile, 66fd70)
KING_VAR(AsmLabel, g_tagPLYR, 66fddc)
KING_VAR(AsmLabel, g_tagPLRS, 66fde4)
KING_VAR(AsmLabel, g_tagTWNS, 66fe28)
KING_VAR(AsmLabel, g_tagPRSS, 66fe48)
KING_VAR(AsmLabel, g_tagROAD, 66fe50)
KING_VAR(AsmLabel, g_tagRDDD, 66fe6c)
KING_VAR(AsmLabel, g_tagAMBS, 66fe90)
KING_VAR(AsmLabel, g_tagOBST, 66fe98)
KING_VAR(AsmLabel, g_tagOBSS, 66feb8)
KING_VAR(BYTE*, g_68cf6c, 68cf6c)
KING_VAR(BYTE**, g_68cf38, 68cf38)
KING_VAR(BYTE**, g_68cf3c, 68cf3c)
KING_VAR(BYTE**, g_68a970, 68a970)
KING_VAR(BYTE**, g_68a974, 68a974)
KING_VAR(BYTE**, g_68cc18, 68cc18)
KING_VAR(BYTE**, g_68cc1c, 68cc1c)

KING_VAR(BYTE*, g_68ccb8, 68ccb8)             // 104-byte records
KING_VAR(double, g_bonusFactor, 66f878)       // 1.5
KING_VAR(double, g_68d6c8, 68d6c8)

struct E12 { DWORD a, b, key; };

extern "C" BYTE* __fastcall SvRec480550_Construct(BYTE* self, void*, int a)
{
    CharVec_Construct((char**)self, 0, (const char*)a);
    for (int i = 16; i <= 48; i += 4)
        F(int, self, i) = 0;
    return self;
}

extern "C" BYTE* __fastcall SvRec480580_Construct(BYTE* self, void*, int a)
{
    CharVec_Construct((char**)self, 0, (const char*)a);
    F(int, self, 16) = 0;
    F(int, self, 20) = 0;
    return self;
}

// a saved time moved to the current clock (DBL_MAX stays)
extern "C" void __cdecl Save_RebaseTime(double* t)
{
    if (F(int, t, 0) == -1 && F(DWORD, t, 4) == 0x7fefffff)
        return;
    *t = AI_Now() - g_68d6c8 + *t;
}

// reads three doubles at *cursor
extern "C" void __cdecl Save_Read3Doubles(BYTE** cursor, DWORD* out)
{
    out[0] = F(DWORD, *cursor, 0);
    out[1] = F(DWORD, *cursor, 4);
    *cursor += 8;
    out[2] = F(DWORD, *cursor, 0);
    out[3] = F(DWORD, *cursor, 4);
    *cursor += 8;
    out[4] = F(DWORD, *cursor, 0);
    out[5] = F(DWORD, *cursor, 4);
    *cursor += 8;
}

// vector<pointer>::reserve
extern "C" void __fastcall PtrVec_Reserve(BYTE** self, void*, unsigned n)
{
    BYTE* first = self[0];
    if ((unsigned)((self[2] - first) >> 2) >= n)
        return;
    BYTE* last = self[1];
    int size = (int)(last - first) >> 2;
    BYTE* p = CallThis<BYTE*>(FUN_44ac60, self, n, first, last);
    int cap = (int)(self[2] - self[0]) >> 2;
    if (cap)
        Ai_PoolFree(self[0], cap * 4);
    self[0] = p;
    self[1] = p + size * 4;
    self[2] = p + n * 4;
}

// the first of the strings (vector<char>*) equal to name
extern "C" char*** __cdecl Strings_Find(char*** first, char*** last, const char* name)
{
    for (; first != last; first++) {
        char* s = (*first)[0];
        if (s != (*first)[1] && s && strcmp(s, name) == 0)
            break;
    }
    return first;
}

// ---------------------------------------------------------------------------
// medium-sized functions: an order's price and margin, the order constructor

struct OrdrPos { int road, node; double t; int segment, object, m_24, m_28; };   // PositionId

extern "C" void __cdecl AiError(int kind, int code, const char* file, const char* id, int line,
                                const char* msg, const char* a, const char* b);
KING_ALIAS("_AiError", "_Ai_Report")
extern "C" int* __cdecl Town_Id(int* out, BYTE* town);                                 // aimain.cpp
extern "C" void __cdecl Road_PositionCostBest(int side, OrdrPos from, OrdrPos to, double* cost);   // aitrce.cpp
extern "C" double __cdecl Order_Value(const int* o);
typedef void* PtrList[1];
KING_VAR(PtrList, g_ordrTowns, 68cec0)
KING_VAR(PtrList, g_ordrVehicles, 68cfd8)
KING_VAR(void*, g_ordrPlayers, 68b990)
KING_VAR(char, g_ordrEmpty, 68d1c0)
KING_VAR(double, g_priceDirect, 66f880)       // 1.2
KING_VAR(double, g_priceUrgent, 66f888)       // 1.5
KING_VAR(double, g_priceSpecial, 66f890)      // 4.0

static const char s_ordrId[] = "$Id: ai_ordr.cpp 2001/9/10 18:14:44";
static const char s_ordrFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_ordr.cpp";

static BYTE* Town(int i) { return (BYTE*)((void**)g_ordrTowns[0])[i]; }

// the unload node of the order's town (reported when missing)
static int UnloadNode(int* o, int line)
{
    int node;
    Town_Id(&node, Town(o[3]));
    if (node == -1)
        AiError(45, 100, s_ordrFile, s_ordrId, line, "Unload node not present for town",
                F(const char*, Town(o[3]), 12), &g_ordrEmpty);
    return node;
}

// the cost of the road from the order's position (+24) to that node
static double UnloadCost(int* o, int node)
{
    OrdrPos to = {-1, node, 0.0, 0, 0, 0, -1};
    double cost = 0;
    Road_PositionCostBest(0, F(OrdrPos, o, 24), to, &cost);
    return cost;
}

// --- saving: chunks are opened with [FUN_490c90] (stream, tag, &pos) and
// closed with [FUN_490ce0] (stream, &pos); bytes are appended with [FUN_46e460].

struct SaveChunk {
    BYTE* s;
    int pos;
};

static void Chunk_Open(SaveChunk* c, BYTE* s, const DWORD* tag)
{
    c->s = s;
    CallC(FUN_490c90, s, tag, &c->pos);
}

static void Chunk_Close(SaveChunk* c)
{
    CallC(FUN_490ce0, c->s, &c->pos);
}

static void Save_Bytes(BYTE* s, const void* p, int n)
{
    CharVec_InsertRange((BYTE**)s, 0, F(BYTE*, s, 4), (BYTE*)((const BYTE*)p), (BYTE*)((const BYTE*)p + n));
}

// the name (a vector<char> at +0): its length, then its bytes one by one
static void Save_Name(BYTE* s, const BYTE* rec)
{
    int n = F(const BYTE*, rec, 4) - F(const BYTE*, rec, 0);
    Save_Bytes(s, &n, 4);
    for (unsigned i = 0; i < (unsigned)(F(const BYTE*, rec, 4) - F(const BYTE*, rec, 0)); i++) {
        BYTE c = F(const BYTE*, rec, 0)[i];
        Save_Bytes(s, &c, 1);
    }
}

static void Save_EachPtr(BYTE* s, const DWORD* tag, BYTE** b, BYTE** const* e, void (*fn)())
{
    SaveChunk c;
    Chunk_Open(&c, s, tag);
    for (BYTE** it = b; it != *e; it++)
        CallC(fn, s, *it);
    Chunk_Close(&c);
}

extern "C" {

// [FUN_480e60] saves a player (PLYR)
void __cdecl Save_Player(BYTE* s, BYTE* p)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagPLYR);
    Player_Save(p, 0, s);
    Chunk_Close(&c);
}

// [FUN_481040] saves the players (PLRS)
void __cdecl Save_Players(BYTE* s)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagPLRS);
    BYTE* head = (BYTE*)g_ordrPlayers;
    for (BYTE* it = *(BYTE**)head; it != (BYTE*)g_ordrPlayers; it = *(BYTE**)it)
        Save_Player(s, AI_ListData(it));
    Chunk_Close(&c);
}

// [FUN_481640] saves the towns (TWNS)
void __cdecl Save_Towns(BYTE* s)
{
    BYTE*** v = (BYTE***)&g_ordrTowns;
    Save_EachPtr(s, (const DWORD*)g_tagTWNS, v[0], (BYTE** const*)&v[1], FUN_4810e0);
}

// [FUN_481f50] saves the routes (PRSS)
void __cdecl Save_Routes(BYTE* s)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagPRSS);
    for (BYTE* n = *(BYTE**)g_68cf6c; n != g_68cf6c; n = *(BYTE**)n)
        Save_Present((BYTE**)s, F(BYTE*, n, 8));
    Chunk_Close(&c);
}

// [FUN_481fe0] saves a road (ROAD): name and +16
void __cdecl Save_Road(BYTE* s, BYTE* road)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagROAD);
    Save_Name(s, road);
    Save_Bytes(s, road + 16, 4);
    Chunk_Close(&c);
}

// [FUN_482290] saves the roads (RDDD)
void __cdecl Save_Roads(BYTE* s)
{
    Save_EachPtr(s, (const DWORD*)g_tagRDDD, g_68cf38, &g_68cf3c, (void (*)())Save_Road);
}

// [FUN_482680] saves the ambushes (AMBS)
void __cdecl Save_Ambushes(BYTE* s)
{
    Save_EachPtr(s, (const DWORD*)g_tagAMBS, g_68a970, &g_68a974, FUN_482320);
}

// [FUN_482710] saves an obstacle (OBST): name and the byte at +28
void __cdecl Save_Obstacle(BYTE* s, BYTE* o)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagOBST);
    Save_Name(s, o);
    Save_Bytes(s, o + 28, 1);
    Chunk_Close(&c);
}

// [FUN_4829e0] saves the obstacles (OBSS)
void __cdecl Save_Obstacles(BYTE* s)
{
    Save_EachPtr(s, (const DWORD*)g_tagOBSS, g_68cc18, &g_68cc1c, (void (*)())Save_Obstacle);
}

}

extern "C" {

// [FUN_4816d0] saves three doubles
void __cdecl Save_Doubles3(BYTE* s, const double* v)
{
    for (int i = 0; i < 3; i++) {
        double d = v[i];
        Save_Bytes(s, &d, 8);
    }
}

// [FUN_482320] saves an ambush (AMBU): its name, two flags and its player's name
void __cdecl Save_Ambush(BYTE* s, BYTE* a)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagAMBU);
    Save_Name(s, a);
    Save_Bytes(s, a + 240, 1);
    Save_Bytes(s, a + 28, 1);
    BYTE* p = F(BYTE*, a, 124);
    if (p == (BYTE*)g_ordrPlayers) {
        int z = 0;
        Save_Bytes(s, &z, 4);
    } else {
        Save_Name(s, p + 8);
    }
    Chunk_Close(&c);
}

// [FUN_483320] after loading: re-registers every player that is not marked (+792)
void __cdecl Load_FixPlayers()
{
    BYTE* head = (BYTE*)g_ordrPlayers;
    for (BYTE* it = *(BYTE**)head; it != (BYTE*)g_ordrPlayers; it = *(BYTE**)it) {
        BYTE* p = AI_ListData(it);
        if (!F(BYTE, p, 792)) {
            int id;
            Int_Set(&id, 0, (int)it);
            Rec_4015a0Both(id);
        } else {
            AiError(0, 100, (const char*)s_svldFile, (const char*)s_svldId, 741, "Internal error 08457",
                    &g_ordrEmpty, &g_ordrEmpty);
        }
    }
}

}

struct LoadChunkS {
    BYTE** cur;
    int a, b;
    DWORD tag;
};

static bool LoadChunk_Open(LoadChunkS* c, BYTE** cur, const void* tag)
{
    c->cur = cur;
    Save_ParseHeader((int**)cur, (int*)&c->tag, (int**)&c->a, &c->b);
    return c->tag == *(const DWORD*)tag;
}

static void LoadChunk_Close(LoadChunkS* c)
{
    Save_BlockEnd(c->cur, (BYTE*)c->a, c->b);
}

extern "C" {

// [FUN_480ed0] loads a player (PLYR) and fixes its saved pointers
void __cdecl Load_Player(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagPLYR)) {
        BYTE* p = Players_Append((int)&g_ordrEmpty);
        Player_Load(p, 0, cur);
        Save_RebaseTime((double*)(p + 784));
        Save_RebaseTime((double*)(p + 104));
        for (BYTE* e = F(BYTE*, p, 136); e != F(BYTE*, p, 140); e += 8)
            Save_RebaseTime((double*)e);
        for (int k = 508; k <= 512; k += 4) {
            BYTE* r = F(BYTE*, p, k);
            if (!r)
                continue;
            for (BYTE* e = F(BYTE*, r, 28); e != F(BYTE*, F(BYTE*, p, k), 32); e += 144) {
                Save_RebaseTime((double*)(e + 72));
                Save_RebaseTime((double*)(e + 80));
            }
        }
        Player_Register((DWORD*)&cur, p);
    }
    LoadChunk_Close(&c);
}

}

// reads a name (length, then its bytes) into the vector<char> v (+0 begin)
static void Load_NameInto(BYTE** cur, BYTE** v)
{
    int n = F(int, *cur, 0);
    *cur += 4;
    CharVec_Erase(v, 0, v[0], v[1]);
    Bytes_Reserve(v, 0, n);
    for (int i = n; i > 0; i--) {
        BYTE c = **cur;
        (*cur)++;
        CallThis(FUN_47ab20, v, &c);
    }
}

static void NameVec_Free(BYTE** v)
{
    if (v[2] != v[0])
        Ai_PoolFree(v[0], v[2] - v[0]);
}

// the first record of [first, last) whose name ([FUN_410df0]) is name
static BYTE** FindByName(BYTE** first, BYTE** last, const char* name)
{
    for (; first != last; first++) {
        const char* s = CallThis<const char*>(FUN_410df0, *first);
        if (s && !strcmp(s, name))
            break;
    }
    return first;
}

extern "C" {

// [FUN_4820c0] loads a road (ROAD): finds it by name and sets +16
void __cdecl Load_Road(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagROAD)) {
        BYTE* v[3] = {0, 0, 0};
        Load_NameInto(cur, v);
        BYTE** it = FindByName(g_68cf38, g_68cf3c, (const char*)v[0]);
        if (it == g_68cf3c)
            AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, 418, "Can not find road", (const char*)v[0],
                    &g_ordrEmpty);
        int x = F(int, *cur, 0);
        *cur += 4;
        F(int, *it, 16) = x;
        NameVec_Free(v);
    }
    LoadChunk_Close(&c);
}

// [FUN_4824a0] loads an ambush (AMBU): its two flags and its player's name
void __cdecl Load_Ambush(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagAMBU)) {
        BYTE* v[3] = {0, 0, 0};
        Load_NameInto(cur, v);
        BYTE* name = v[0];
        BYTE** it = g_68a970;
        for (; it != g_68a974; it++)
            if (CallThis<bool>(FUN_407320, &name, *it))
                break;
        if (it == g_68a974)
            AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, 464, "Can not find ambush", (const char*)v[1],
                    &g_ordrEmpty);
        BYTE* a = *it;
        F(BYTE, a, 240) = **cur;
        (*cur)++;
        F(BYTE, a, 28) = **cur;
        (*cur)++;
        Load_NameInto(cur, (BYTE**)(a + 128));
        NameVec_Free(v);
    }
    LoadChunk_Close(&c);
}

// [FUN_4827f0] loads an obstacle (OBST): its flag, then [FUN_483c10]
void __cdecl Load_Obstacle(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagOBST)) {
        BYTE* v[3] = {0, 0, 0};
        Load_NameInto(cur, v);
        BYTE** it = FindByName(g_68cc18, g_68cc1c, (const char*)v[0]);
        if (it == g_68cc1c)
            AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, 503, "Can not find obstacle", (const char*)v[0],
                    &g_ordrEmpty);
        BYTE* o = *it;
        F(BYTE, o, 28) = **cur;
        (*cur)++;
        CallC(FUN_483c10, o ? o + 12 : 0);
        NameVec_Free(v);
    }
    LoadChunk_Close(&c);
}

}

extern "C" {

// [FUN_4810e0] saves a town (TOWN): its name and a TNDE chunk per node of
// each of its 16-byte entries (two node slots at +8 and +12)
void __cdecl Save_Town(BYTE* s, BYTE* town)
{
    SaveChunk outer;
    Chunk_Open(&outer, s, (const DWORD*)g_tagTOWN);
    Save_Name(s, town);
    int idx = 0;
    for (BYTE* e = F(BYTE*, town, 28); e != F(BYTE*, town, 32); e += 16, idx++) {
        for (int k = 0; k < 2; k++) {
            BYTE* node = F(BYTE*, e, 8 + k * 4);
            if (!node)
                continue;
            SaveChunk c;
            Chunk_Open(&c, s, (const DWORD*)g_tagTNDE);
            Save_Bytes(s, &idx, 4);
            Save_Bytes(s, node + 0, 4);
            Save_Bytes(s, node + 4, 4);
            Save_Bytes(s, node + 12, 4);
            Save_Bytes(s, node + 16, 8);
            Save_Bytes(s, node + 24, 8);
            DWORD stale[2] = {(DWORD)outer.s, (DWORD)outer.pos};   // as the original: its own chunk record
            Save_Bytes(s, stale, 8);
            Save_Bytes(s, node + 8, 4);
            Chunk_Close(&c);
        }
    }
    Chunk_Close(&outer);
}

}

extern "C" void* __cdecl Stl_Allocate(unsigned bytes);
extern "C" void* __cdecl memmove(void* d, const void* s, size_t n);

extern "C" {

// [FUN_4836f0] n bytes from the allocator holding a copy of [first, last)
BYTE* __stdcall Bytes_Clone(unsigned n, const BYTE* first, const BYTE* last)
{
    BYTE* p = n ? (BYTE*)Stl_Allocate(n) : 0;
    memmove(p, first, last - first);
    return p;
}

}

KING_VAR(AsmLabel, s_aiStream66ff3c, 66ff3c)
KING_VAR(AsmLabel, s_errOpen66ff58, 66ff58)
KING_VAR(AsmLabel, s_errRead66ff40, 66ff40)
KING_VAR(char, g_empty68d1c0s, 68d1c0)
extern "C" void __cdecl Ai_LoadState(BYTE* p);
extern "C" void FUN_482a70(); KING_ALIAS("_FUN_482a70", "_Ai_LoadState")    // reads the AI state from a buffer
extern "C" void __cdecl Players_Each4776b0();
extern "C" void __cdecl Routes_Each477600();
extern "C" void __cdecl Ambushes_Each477600();
extern "C" void __cdecl Load_RelinkPlayers();
extern "C" void FUN_482d20(); KING_ALIAS("_FUN_482d20", "_Load_RelinkPlayers")
extern "C" void __cdecl Load_FixPlayers();
extern "C" void FUN_483320(); KING_ALIAS("_FUN_483320", "_Load_FixPlayers")

// [FUN_4835d0] loads the AI state from its stream in a saved game, then fixes the links
extern "C" void __cdecl AI_LoadFromStorage(IStorage* stg)
{
    WCHAR name[260];
    MultiByteToWideChar(0, 0, s_aiStream66ff3c, -1, name, 260);
    IStream* s;
    if (stg->OpenStream(name, 0, 16, 0, &s) < 0)
        AiError(0, 1, (const char*)s_svldFile, (const char*)s_svldId, 794, s_errOpen66ff58, &g_empty68d1c0s, &g_empty68d1c0s);
    STATSTG st;
    s->Stat(&st, 1);
    BYTE* p = (BYTE*)operator new(st.cbSize.LowPart);
    ULONG read = 0;
    if (s->Read(p, st.cbSize.LowPart, &read) < 0)
        AiError(0, 1, (const char*)s_svldFile, (const char*)s_svldId, 802, s_errRead66ff40, &g_empty68d1c0s, &g_empty68d1c0s);
    s->Release();
    Ai_LoadState(p);
    operator delete(p);
    Players_Each4776b0();
    Routes_Each477600();
    Ambushes_Each477600();
    Load_RelinkPlayers();
    Load_FixPlayers();
}

// ---------------------------------------------------------------------------
// Relinking after a load

extern "C" void __cdecl Player_MarkListed(BYTE* id);
extern "C" void __cdecl Player_Retire(BYTE* id);
extern "C" void* __fastcall Market_Get(const int* s, void*);
extern "C" BYTE* __fastcall Vec36_At(void* s, void*, int i);
KING_VAR(BYTE**, g_towns68cc98, 68cc98)
KING_VAR(BYTE**, g_towns68cc9c, 68cc9c)
KING_VAR(BYTE**, g_68cf00, 68cf00)
KING_VAR(BYTE, g_count68ced0, 68ced0)         // three 16-byte records, a count at +4
KING_VAR(BYTE, g_count68cee0, 68cee0)
KING_VAR(BYTE, g_count68cef0, 68cef0)
KING_VAR(int, g_68d14c, 68d14c)
KING_VAR(int, g_68d18c, 68d18c)
KING_VAR(int, g_68a968, 68a968)
KING_VAR(int, g_68d01c, 68d01c)
KING_VAR(int, g_68cfe4, 68cfe4)
KING_VAR(int, g_68d0e4, 68d0e4)
KING_VAR(int, g_68cf34, 68cf34)
KING_VAR(char, g_msg77e460, 77e460)
KING_VAR(AsmLabel, s_fmt66e4d0, 66e4d0)
KING_VAR(AsmLabel, s_err66cbf4, 66cbf4)
KING_VAR(AsmLabel, s_err66fedc, 66fedc)
KING_VAR(AsmLabel, s_err66fec8, 66fec8)
KING_VAR(AsmLabel, s_err66fee8, 66fee8)

static int Relink_Id(BYTE* it)
{
    int id;
    Int_Set(&id, 0, (int)it);
    return id;
}

// the town of a player of state 256/257 counts it, and the player's slot there gets its id and the flag
static int Relink_Slot(BYTE* it, int line, DWORD flag)
{
    BYTE* p = AI_ListData(it);
    F(int, g_towns68cc98[F(int, p, 112)], 40)++;
    int t = F(int, CallC<BYTE*>(FUN_40a9f0, it), 112);
    if (t == -1)
        AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, line, (const char*)s_err66fee8,
                &g_ordrEmpty, &g_ordrEmpty);
    if (t < 0 || t >= g_towns68cc9c - g_towns68cc98) {
        sprintf(&g_msg77e460, (const char*)s_fmt66e4d0, t);
        AiError(33, 100, (const char*)s_svldFile, (const char*)s_svldId, line + 1, (const char*)s_err66cbf4,
                (const char*)s_err66fedc, &g_msg77e460);
    }
    BYTE* v = g_towns68cc98[t] + 28;
    int n = (F(BYTE*, v, 4) - F(BYTE*, v, 0)) / 36;
    int k = F(int, CallC<BYTE*>(FUN_40a9f0, it), 116);
    if (!((unsigned)n > (unsigned)k))
        AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, line + 3, (const char*)s_err66fec8,
                &g_ordrEmpty, &g_ordrEmpty);
    int id = Relink_Id(it);
    k = F(int, CallC<BYTE*>(FUN_40a9f0, it), 116);
    BYTE* e = Vec36_At((BYTE*)Market_Get(&t, 0) + 28, 0, k);
    F(int, e, 8) = id;
    k = F(int, CallC<BYTE*>(FUN_40a9f0, it), 116);
    F(DWORD, F(BYTE*, g_towns68cc98[t], 28) + k * 36, 4) |= flag;
    return t;
}

extern "C" {

// [FUN_482d20] after loading: links the players back into their towns' slots, sorts the hired drivers into the
// three cargo-class counts, retires players of state 5, and counts the AI's objects by kind
void __cdecl Load_RelinkPlayers()
{
    for (BYTE* r = &g_count68ced0 + 4; r < &g_count68ced0 + 52; r += 16)
        F(int, r, 0) = 0;
    g_68d14c = 0;
    g_68d18c = 0;
    for (BYTE* it = *(BYTE**)g_ordrPlayers; it != (BYTE*)g_ordrPlayers; it = *(BYTE**)it) {
        if (F(BYTE, CallC<BYTE*>(FUN_40a9f0, it), 488))
            Player_MarkListed((BYTE*)(Relink_Id(it)));
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 92) == 256) {
            int t = Relink_Slot(it, 653, 1);
            int k = F(int, CallC<BYTE*>(FUN_40a9f0, it), 116);
            BYTE f = F(BYTE, F(BYTE*, g_towns68cc98[t], 28) + k * 36, 4);
            BYTE* cls = (f & 8) ? &g_count68cef0 : (f & 32) ? &g_count68cee0 : &g_count68ced0;
            F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 964) = cls;
            F(int, F(BYTE*, CallC<BYTE*>(FUN_40a9f0, it), 964), 4)++;
        }
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 92) == 257) {
            g_68d14c++;
            Relink_Slot(it, 676, 2);
        }
        if (F(int, CallC<BYTE*>(FUN_40a9f0, it), 92) == 5)
            Player_Retire((BYTE*)(Relink_Id(it)));
        BYTE* p = AI_ListData(it);
        if (F(int, p, 152) != F(int, p, 156))
            g_68d18c++;
    }
    g_68a968 = 0;
    g_68d01c = 0;
    g_68cfe4 = 0;
    g_68d0e4 = 0;
    g_68cf34 = 0;
    for (BYTE* n = *(BYTE**)g_68cf6c; n != g_68cf6c; n = *(BYTE**)n) {
        BYTE* o = F(BYTE*, n, 8);
        if (F(int, o, 24) != -1)
            F(DWORD, g_68cf00[F(int, o, 24)], 12) |= 16;
        switch (F(DWORD, F(BYTE*, n, 8), 8) & 240) {
        case 16: g_68a968++; break;
        case 32: g_68d01c++; break;
        case 64: g_68cfe4++; break;
        case 96: g_68d0e4++; break;
        case 128: g_68cf34++; break;
        }
    }
}

}  // extern "C"

// === loading a present (FUN_481b20)

KING_VAR(AsmLabel, g_tagPRES, 66fe40)
KING_VAR(AsmLabel, g_tag66fe38, 66fe38)
KING_VAR(AsmLabel, g_tag66fe30, 66fe30)
KING_VAR(void*, g_pr68d11c, 68d11c)
extern "C" void FUN_4133a0(); KING_ALIAS("_FUN_4133a0", "@Position_InitNone@8")
extern "C" int* __cdecl Presents_Append(int* out, int v);
extern "C" int* __fastcall Int_Copy(int* s, void*, const int* src);
extern "C" void __fastcall Vec16_FreeStorage(BYTE** v, int);
extern "C" void FUN_466400(); KING_ALIAS("_FUN_466400", "_UninitFill16")
extern "C" void FUN_467180(); KING_ALIAS("_FUN_467180", "@Vec16_Reserve@16")
extern "C" void __fastcall DriverRec_Load(BYTE* s, void*, DWORD** pp);
extern "C" void __fastcall Orders144_Destroy(BYTE** self, void*);
extern "C" void __cdecl Save_Read3Doubles(BYTE** cursor, DWORD* out);
extern "C" void** __fastcall Vec_Construct(void** self, void*, int);

static DWORD Pr_32(BYTE** c) { DWORD v = F(DWORD, *c, 0); *c += 4; return v; }
static void Pr_64(BYTE** c, BYTE* to) { F(DWORD, to, 0) = F(DWORD, *c, 0); F(DWORD, to, 4) = F(DWORD, *c, 4); *c += 8; }

// the 32-byte record of a 66fe38 block
static BYTE* Pr_Record32(BYTE** cur)
{
    BYTE* r = (BYTE*)operator new(32);
    if (r) {
        for (int k = 0; k < 16; k += 4)
            F(int, r, k) = -1;
        for (int k = 16; k < 32; k += 4)
            F(int, r, k) = 0;
    }
    F(DWORD, r, 0) = Pr_32(cur);
    F(DWORD, r, 4) = Pr_32(cur);
    F(DWORD, r, 12) = Pr_32(cur);
    Pr_64(cur, r + 16);
    F(DWORD, r, 24) = F(DWORD, *cur, 0);
    F(DWORD, r, 28) = F(DWORD, *cur, 4);
    *cur += 16;
    F(DWORD, r, 8) = Pr_32(cur);
    return r;
}

// the 48-byte record of a 66fe30 block (the old one at e+20 destroyed)
static void Pr_Record48(BYTE* e, BYTE** cur)
{
    if (BYTE* old = F(BYTE*, e, 20)) {
        Orders144_Destroy((BYTE**)(old + 28), 0);
        Vec16_FreeStorage((BYTE**)(old + 16), 0);
        operator delete(old);
    }
    BYTE* r = (BYTE*)operator new(48);
    if (r) {
        F(DWORD, r, 8) = 0;
        F(int, r, 0) = -1;
        F(int, r, 4) = -1;
        F(DWORD, r, 12) = 1072693248;
        DWORD t[4] = {0, 0, 0, 1072693248};
        BYTE alloc[2];
        CallThis(FUN_467180, r + 16, 19, alloc);
        F(BYTE*, r, 20) = CallC<BYTE*>(FUN_466400, F(BYTE*, r, 16), 19, t, r);
        Vec_Construct((void**)(r + 28), 0, (int)(alloc + 1));
        F(DWORD, r, 40) = 0;
    }
    F(BYTE*, e, 20) = r;
    DriverRec_Load(r, 0, (DWORD**)cur);
}

extern "C" {

// [FUN_481b20] loads a present (PRES): its 112-byte record with the sub-blocks 66fe38 (+88) and 66fe30 (+20,
// whose orders' times are rebased), adds it to the presents and hands it to the handler 68d11c
void __cdecl Load_Present(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagPRES)) {
        BYTE* e = (BYTE*)operator new(112);
        if (e) {
            F(int, e, 0) = -1;
            F(DWORD, e, 4) = 0x12345678;
            F(int, e, 8) = 0;
            F(int, e, 12) = -1;
            F(int, e, 16) = -1;
            F(int, e, 20) = 0;
            F(int, e, 24) = -1;
            CallThis(FUN_4133a0, e + 56);
            F(int, e, 88) = 0;
            F(DWORD, e, 96) = 0xffffffff;
            F(DWORD, e, 100) = 0x7fefffff;
            F(BYTE, e, 104) = 0;
            F(int, e, 108) = -1;
        }
        F(DWORD, e, 8) = Pr_32(cur);
        F(DWORD, e, 12) = Pr_32(cur);
        F(DWORD, e, 16) = Pr_32(cur);
        F(DWORD, e, 24) = Pr_32(cur);
        F(DWORD, e, 0) = Pr_32(cur);
        Save_Read3Doubles(cur, (DWORD*)(e + 32));
        F(DWORD, e, 80) = Pr_32(cur);
        F(DWORD, e, 60) = Pr_32(cur);
        F(DWORD, e, 56) = Pr_32(cur);
        Pr_64(cur, e + 64);
        F(DWORD, e, 72) = Pr_32(cur);
        F(DWORD, e, 76) = Pr_32(cur);
        F(DWORD, e, 84) = Pr_32(cur);
        Pr_64(cur, e + 96);
        F(BYTE, e, 104) = **cur;
        *cur += 1;
        F(DWORD, e, 108) = Pr_32(cur);
        Save_RebaseTime((double*)(e + 96));
        while (*c.cur != (BYTE*)(c.a + c.b)) {
            LoadChunkS s;
            LoadChunk_Open(&s, cur, g_tag66fe38);
            if (s.tag == *(const DWORD*)g_tag66fe38)
                F(BYTE*, e, 88) = Pr_Record32(cur);
            else if (s.tag == *(const DWORD*)g_tag66fe30)
                Pr_Record48(e, cur);
            LoadChunk_Close(&s);
        }
        if (BYTE* r = F(BYTE*, e, 20))
            for (BYTE* o = F(BYTE*, r, 28); o != F(BYTE*, F(BYTE*, e, 20), 32); o += 144) {
                Save_RebaseTime((double*)(o + 72));
                Save_RebaseTime((double*)(o + 80));
            }
        int it, copy;
        Presents_Append(&it, (int)e);
        Int_Copy(&copy, 0, &it);
        ((void (__cdecl*)(int, int))g_pr68d11c)(copy, 1);
    }
    LoadChunk_Close(&c);
}

} // extern "C"

// === saving a present (FUN_481790)

extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
extern "C" void __fastcall Rec4722c0_Save(DWORD* self, void*, BYTE* stream);
extern "C" void __cdecl Save_Doubles3(BYTE* s, const double* v);
extern "C" void FUN_4816d0(); KING_ALIAS("_FUN_4816d0", "_Save_Doubles3")
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")

struct Sp_Block { BYTE** v; int mark; };

static void Sp_Begin(Sp_Block* b, BYTE** v, const AsmLabel& tag)
{
    b->v = v;
    CallC(FUN_490c90, v, (const char*)tag, &b->mark);
}

static void Sp_End(Sp_Block* b) { CallC(FUN_490ce0, b->v, &b->mark); }

static void Sp_Put(BYTE** v, const void* p, int n)
{
    CharVec_InsertRange(v, 0, v[1], (BYTE*)((const BYTE*)p), (BYTE*)((const BYTE*)p + n));
}

extern "C" {

// [FUN_481790] writes present p as a PRES block of the save data v: the record (the layout Load_Present
// reads), its 66fe38 sub-record (+88; 8 bytes of the block's own bookkeeping in the middle, which the loader
// skips) and its 66fe30 orders (+20)
void __cdecl Save_Present(BYTE** v, BYTE* p)
{
    Sp_Block outer;
    Sp_Begin(&outer, v, g_tagPRES);
    static const int first[] = {8, 12, 16, 24, 0};
    for (int k : first)
        Sp_Put(v, p + k, 4);
    Save_Doubles3((BYTE*)v, (const double*)(p + 32));
    Sp_Put(v, p + 80, 4);
    Sp_Put(v, p + 60, 4);
    Sp_Put(v, p + 56, 4);
    Sp_Put(v, p + 64, 8);
    Sp_Put(v, p + 72, 4);
    Sp_Put(v, p + 76, 4);
    Sp_Put(v, p + 84, 4);
    Sp_Put(v, p + 96, 8);
    Sp_Put(v, p + 104, 1);
    Sp_Put(v, p + 108, 4);
    if (BYTE* r = F(BYTE*, p, 88)) {
        Sp_Block b;
        Sp_Begin(&b, v, g_tag66fe38);
        Sp_Put(v, r + 0, 4);
        Sp_Put(v, r + 4, 4);
        Sp_Put(v, r + 12, 4);
        Sp_Put(v, r + 16, 8);
        Sp_Put(v, r + 24, 8);
        Sp_Put(v, &outer, 8);
        Sp_Put(v, r + 8, 4);
        Sp_End(&b);
    }
    if (BYTE* o = F(BYTE*, p, 20)) {
        Sp_Block b;
        Sp_Begin(&b, v, g_tag66fe30);
        Rec4722c0_Save((DWORD*)o, 0, (BYTE*)v);
        Sp_End(&b);
    }
    Sp_End(&outer);
}

} // extern "C"

// === loading a town (FUN_481330)

extern "C" BYTE* __fastcall CharVec_Clear(void* s, void*);
extern "C" void __cdecl Sys_Log(const char* text, int value);
KING_VAR(AsmLabel, s_svld66fe14, 66fe14)
KING_VAR(AsmLabel, s_svld66fdfc, 66fdfc)
KING_VAR(AsmLabel, s_svld66d5ac, 66d5ac)

extern "C" {

// [FUN_481330] loads a town (TOWN): finds it by name, then reads each TNDE chunk into a new 32-byte node for its
// entry (index; +0, +4, +12, +16, +24, then a skipped stale record, +8) and puts it into the entry's first free
// node slot (+8 or +12)
void __cdecl Load_Town(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagTOWN)) {
        BYTE* v[3];
        char alloc;
        Vec_Construct((void**)v, 0, (int)&alloc);
        int n = F(int, *cur, 0);
        *cur += 4;
        CharVec_Clear(v, 0);
        Bytes_Reserve(v, 0, n);
        for (; n; n--) {
            BYTE ch = **cur;
            (*cur)++;
            CallThis(FUN_47ab20, v, &ch);
        }
        char*** it = Strings_Find((char***)g_ordrTowns[0], (char***)g_ordrTowns[1], (const char*)v[0]);
        if (it == (char***)g_ordrTowns[1])
            AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, 244, s_svld66fe14, (const char*)v[0],
                    &g_ordrEmpty);
        while (*c.cur != (BYTE*)(c.a + c.b)) {
            LoadChunkS e;
            if (LoadChunk_Open(&e, cur, g_tagTNDE)) {
                int idx = F(int, *cur, 0);
                *cur += 4;
                DWORD* node = (DWORD*)operator new(32);
                node[0] = node[1] = node[2] = node[3] = 0xFFFFFFFF;
                node[4] = node[5] = node[6] = node[7] = 0;
                node[0] = F(DWORD, *cur, 0);
                node[1] = F(DWORD, *cur, 4);
                node[3] = F(DWORD, *cur, 8);
                node[4] = F(DWORD, *cur, 12);
                node[5] = F(DWORD, *cur, 16);
                node[6] = F(DWORD, *cur, 20);
                node[7] = F(DWORD, *cur, 24);
                node[2] = F(DWORD, *cur, 40);
                *cur += 44;
                if (node[0] == 0xFFFFFFFF) {
                    AiError(0, 200, (const char*)s_svldFile, (const char*)s_svldId, 256, s_svld66fdfc, &g_ordrEmpty,
                            &g_ordrEmpty);
                    Sys_Log(s_svld66d5ac, node[2]);
                    Sys_Log(s_svld66d5ac, node[1]);
                }
                Save_RebaseTime((double*)(node + 4));
                BYTE* entry = F(BYTE*, *it, 28) + idx * 16;
                if (!F(DWORD*, entry, 8))
                    F(DWORD*, entry, 8) = node;
                else
                    F(DWORD*, entry, 12) = node;
            }
            LoadChunk_Close(&e);
        }
        NameVec_Free(v);
    }
    LoadChunk_Close(&c);
}

} // extern "C"

// === loading the AI state (FUN_482a70)

KING_VAR(AsmLabel, g_tagAll66fec0, 66fec0)
KING_VAR(AsmLabel, g_tagGlob66fd68, 66fd68)
extern "C" void FUN_480b00(); KING_ALIAS("_FUN_480b00", "_Ai_LoadGlobals")

// a loader of one record of a list chunk
typedef void (__cdecl* Ld_Fn)(BYTE** cur);

extern "C" {

// [FUN_482a70] loads the AI state from the save data p: inside the outer chunk the globals and the lists of
// players (PLRS), towns (TWNS), presents (PRSS), roads (RDDD), ambushes (AMBS) and obstacles (OBSS)
void __cdecl Ai_LoadState(BYTE* p)
{
    static const struct { const char* tag; Ld_Fn fn; } k_lists[] = {
        {g_tagPLRS, Load_Player}, {g_tagTWNS, Load_Town}, {g_tagPRSS, Load_Present},
        {g_tagRDDD, Load_Road}, {g_tagAMBS, Load_Ambush}, {g_tagOBSS, Load_Obstacle},
    };
    BYTE** cur = &p;
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagAll66fec0))
        while (*c.cur != (BYTE*)(c.a + c.b)) {
            LoadChunkS e;
            LoadChunk_Open(&e, cur, g_tagGlob66fd68);
            if (e.tag == *(const DWORD*)g_tagGlob66fd68)
                CallC(FUN_480b00, &e, cur);
            else
                for (const auto& l : k_lists)
                    if (e.tag == *(const DWORD*)l.tag) {
                        while (*e.cur != (BYTE*)(e.a + e.b))
                            l.fn(cur);
                        break;
                    }
            LoadChunk_Close(&e);
        }
    LoadChunk_Close(&c);
}

} // extern "C"

// === saving the AI state (FUN_4833a0)

extern "C" void __cdecl Ai_SaveGlobals(BYTE** v);
KING_VAR(AsmLabel, s_sv66ff24, 66ff24)
KING_VAR(AsmLabel, s_sv66ff14, 66ff14)

extern "C" {

// [FUN_4833a0] saves the AI state into its stream of saved game stg: in one outer chunk the globals, players,
// towns, routes, roads, ambushes and obstacles, built in a 256 KB buffer
void __cdecl AI_SaveToStorage(IStorage* stg)
{
    WCHAR name[260];
    MultiByteToWideChar(0, 0, s_aiStream66ff3c, -1, name, 260);
    IStream* s;
    if (stg->CreateStream(name, 17, 0, 0, &s) < 0)
        AiError(0, 1, (const char*)s_svldFile, (const char*)s_svldId, 756, s_sv66ff24, &g_empty68d1c0s, &g_empty68d1c0s);
    BYTE* v[3] = {0, 0, 0};
    BYTE* p = Bytes_Clone(262144, 0, 0);
    v[0] = v[1] = p;
    v[2] = p + 262144;
    int mark;
    CallC(FUN_490c90, v, (const char*)g_tagAll66fec0, &mark);
    Ai_SaveGlobals(v);
    Save_Players((BYTE*)v);
    Save_Towns((BYTE*)v);
    Save_Routes((BYTE*)v);
    Save_Roads((BYTE*)v);
    Save_Ambushes((BYTE*)v);
    Save_Obstacles((BYTE*)v);
    CallC(FUN_490ce0, v, &mark);
    ULONG written = 0;
    if (s->Write(v[0], (ULONG)(v[1] - v[0]), &written) < 0)
        AiError(0, 256, (const char*)s_svldFile, (const char*)s_svldId, 775, s_sv66ff14, &g_empty68d1c0s,
                &g_empty68d1c0s);
    s->Release();
    if (v[2] - v[0])
        Ai_PoolFree(v[0], (int)(v[2] - v[0]));
}

} // extern "C"
