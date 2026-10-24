// ai_saveload.cpp - ai_svld.cpp of the AI, rewritten from king.masm: helpers
// of its save files.
//
// Building with  nmake ASM_AI_SAVELOAD=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include <objbase.h>
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)
extern "C" void FUN_470900(); KING_ALIAS("_FUN_470900", "@CharVec_Construct@12")
extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector<pointer>: a new buffer (n, first, last)
extern "C" void FUN_44b4a0(); KING_ALIAS("_FUN_44b4a0", "_Ai_PoolFree")    // the AI's allocator: free (p, size)
extern "C" void FUN_4471f0(); KING_ALIAS("_FUN_4471f0", "@CharVec_Erase@16")
extern "C" void FUN_47aad0(); KING_ALIAS("_FUN_47aad0", "@Bytes_Reserve@12")
extern "C" void FUN_47ab20(); KING_ALIAS("_FUN_47ab20", "@CharVec_PushBack@12")
extern "C" void FUN_410df0(); KING_ALIAS("_FUN_410df0", "@CharVec_Str@8")
extern "C" void FUN_407320(); KING_ALIAS("_FUN_407320", "@CharVec_Equals@12")
extern "C" void FUN_483c10(); KING_ALIAS("_FUN_483c10", "_AI_UpdateSwitch")
extern "C" void FUN_490c40(); KING_ALIAS("_FUN_490c40", "_Save_ParseHeader")
extern "C" void FUN_490c70(); KING_ALIAS("_FUN_490c70", "_Save_BlockEnd")
extern "C" void FUN_473420(); KING_ALIAS("_FUN_473420", "_Players_Append")
extern "C" void FUN_474720(); KING_ALIAS("_FUN_474720", "@Player_Load@12")
extern "C" void FUN_480650(); KING_ALIAS("_FUN_480650", "_Save_RebaseTime")
extern "C" void FUN_422390(); KING_ALIAS("_FUN_422390", "_Player_Register")
extern "C" void FUN_420000(); KING_ALIAS("_FUN_420000", "_Rec_4015a0Both")
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
extern "C" void FUN_475b20(); KING_ALIAS("_FUN_475b20", "@Player_Save@12")
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
extern "C" void FUN_4810e0(); KING_ALIAS("_FUN_4810e0", "_Save_Town")
extern "C" void FUN_481790(); KING_ALIAS("_FUN_481790", "_Save_Present")
extern "C" void FUN_482320(); KING_ALIAS("_FUN_482320", "_Save_Ambush")
typedef char AsmLabel[1];
ASM_VAR(AsmLabel, g_tagAMBU, $L_66fe74)
ASM_VAR(AsmLabel, g_tagTOWN, $L_66fdf4)
ASM_VAR(AsmLabel, g_tagTNDE, $L_66fdec)
ASM_VAR(AsmLabel, s_svldId, $L_66fcb4)
ASM_VAR(AsmLabel, s_svldFile, $L_66fd70)
ASM_VAR(AsmLabel, g_tagPLYR, $L_66fddc)
ASM_VAR(AsmLabel, g_tagPLRS, $L_66fde4)
ASM_VAR(AsmLabel, g_tagTWNS, $L_66fe28)
ASM_VAR(AsmLabel, g_tagPRSS, $L_66fe48)
ASM_VAR(AsmLabel, g_tagROAD, $L_66fe50)
ASM_VAR(AsmLabel, g_tagRDDD, $L_66fe6c)
ASM_VAR(AsmLabel, g_tagAMBS, $L_66fe90)
ASM_VAR(AsmLabel, g_tagOBST, $L_66fe98)
ASM_VAR(AsmLabel, g_tagOBSS, $L_66feb8)
ASM_VAR(BYTE*, g_68cf6c, $L_68cf6c)
ASM_VAR(BYTE**, g_68cf38, $L_68cf38)
ASM_VAR(BYTE**, g_68cf3c, $L_68cf3c)
ASM_VAR(BYTE**, g_68a970, $L_68a970)
ASM_VAR(BYTE**, g_68a974, $L_68a974)
ASM_VAR(BYTE**, g_68cc18, $L_68cc18)
ASM_VAR(BYTE**, g_68cc1c, $L_68cc1c)

ASM_VAR(BYTE*, g_68ccb8, $L_68ccb8)             // 104-byte records
ASM_VAR(double, g_bonusFactor, $L_66f878)       // 1.5
ASM_VAR(double, g_68d6c8, $L_68d6c8)

struct E12 { DWORD a, b, key; };

extern "C" BYTE* __fastcall SvRec480550_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_470900, self, a);
    for (int i = 16; i <= 48; i += 4)
        F(int, self, i) = 0;
    return self;
}

extern "C" BYTE* __fastcall SvRec480580_Construct(BYTE* self, void*, int a)
{
    CallThis(FUN_470900, self, a);
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
        CallC(FUN_44b4a0, self[0], cap * 4);
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
ASM_VAR(PtrList, g_ordrTowns, $L_68cec0)
ASM_VAR(PtrList, g_ordrVehicles, $L_68cfd8)
ASM_VAR(void*, g_ordrPlayers, $L_68b990)
ASM_VAR(char, g_ordrEmpty, $L_68d1c0)
ASM_VAR(double, g_priceDirect, $L_66f880)       // 1.2
ASM_VAR(double, g_priceUrgent, $L_66f888)       // 1.5
ASM_VAR(double, g_priceSpecial, $L_66f890)      // 4.0

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
    CallThis(FUN_46e460, s, F(BYTE*, s, 4), (const BYTE*)p, (const BYTE*)p + n);
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
    CallThis(FUN_475b20, p, s);
    Chunk_Close(&c);
}

// [FUN_481040] saves the players (PLRS)
void __cdecl Save_Players(BYTE* s)
{
    SaveChunk c;
    Chunk_Open(&c, s, (const DWORD*)g_tagPLRS);
    BYTE* head = (BYTE*)g_ordrPlayers;
    for (BYTE* it = *(BYTE**)head; it != (BYTE*)g_ordrPlayers; it = *(BYTE**)it)
        Save_Player(s, CallC<BYTE*>(FUN_40a9f0, it));
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
        CallC(FUN_481790, s, F(BYTE*, n, 8));
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
        BYTE* p = CallC<BYTE*>(FUN_40a9f0, it);
        if (!F(BYTE, p, 792)) {
            int id;
            CallThis(FUN_426280, &id, it);
            CallC(FUN_420000, id);
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
    CallC(FUN_490c40, cur, &c->tag, &c->a, &c->b);
    return c->tag == *(const DWORD*)tag;
}

static void LoadChunk_Close(LoadChunkS* c)
{
    CallC(FUN_490c70, c->cur, c->a, c->b);
}

extern "C" {

// [FUN_480ed0] loads a player (PLYR) and fixes its saved pointers
void __cdecl Load_Player(BYTE** cur)
{
    LoadChunkS c;
    if (LoadChunk_Open(&c, cur, g_tagPLYR)) {
        BYTE* p = CallC<BYTE*>(FUN_473420, &g_ordrEmpty);
        CallThis(FUN_474720, p, cur);
        CallC(FUN_480650, p + 784);
        CallC(FUN_480650, p + 104);
        for (BYTE* e = F(BYTE*, p, 136); e != F(BYTE*, p, 140); e += 8)
            CallC(FUN_480650, e);
        for (int k = 508; k <= 512; k += 4) {
            BYTE* r = F(BYTE*, p, k);
            if (!r)
                continue;
            for (BYTE* e = F(BYTE*, r, 28); e != F(BYTE*, F(BYTE*, p, k), 32); e += 144) {
                CallC(FUN_480650, e + 72);
                CallC(FUN_480650, e + 80);
            }
        }
        CallC(FUN_422390, &cur, p);
    }
    LoadChunk_Close(&c);
}

}

// reads a name (length, then its bytes) into the vector<char> v (+0 begin)
static void Load_NameInto(BYTE** cur, BYTE** v)
{
    int n = F(int, *cur, 0);
    *cur += 4;
    CallThis(FUN_4471f0, v, v[0], v[1]);
    CallThis(FUN_47aad0, v, n);
    for (int i = n; i > 0; i--) {
        BYTE c = **cur;
        (*cur)++;
        CallThis(FUN_47ab20, v, &c);
    }
}

static void NameVec_Free(BYTE** v)
{
    if (v[2] != v[0])
        CallC(FUN_44b4a0, v[0], v[2] - v[0]);
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

ASM_VAR(AsmLabel, s_aiStream66ff3c, $L_66ff3c)
ASM_VAR(AsmLabel, s_errOpen66ff58, $L_66ff58)
ASM_VAR(AsmLabel, s_errRead66ff40, $L_66ff40)
ASM_VAR(char, g_empty68d1c0s, $L_68d1c0)
ASM_PROC(FUN_482a70)    // reads the AI state from a buffer
extern "C" void FUN_477780(); KING_ALIAS("_FUN_477780", "_Players_Each4776b0")
extern "C" void FUN_478910(); KING_ALIAS("_FUN_478910", "_Routes_Each477600")
extern "C" void FUN_478950(); KING_ALIAS("_FUN_478950", "_Ambushes_Each477600")
extern "C" void FUN_482d20(); KING_ALIAS("_FUN_482d20", "_Load_RelinkPlayers")
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
    CallC(FUN_482a70, p);
    operator delete(p);
    CallC(FUN_477780);
    CallC(FUN_478910);
    CallC(FUN_478950);
    CallC(FUN_482d20);
    CallC(FUN_483320);
}

// ---------------------------------------------------------------------------
// Relinking after a load

extern "C" void FUN_4175c0(); KING_ALIAS("_FUN_4175c0", "_Player_MarkListed")
extern "C" void FUN_417770(); KING_ALIAS("_FUN_417770", "_Player_Retire")
extern "C" void FUN_419490(); KING_ALIAS("_FUN_419490", "@Market_Get@8")
extern "C" void FUN_448a60(); KING_ALIAS("_FUN_448a60", "@Vec36_At@12")
ASM_VAR(BYTE**, g_towns68cc98, $L_68cc98)
ASM_VAR(BYTE**, g_towns68cc9c, $L_68cc9c)
ASM_VAR(BYTE**, g_68cf00, $L_68cf00)
ASM_VAR(BYTE, g_count68ced0, $L_68ced0)         // three 16-byte records, a count at +4
ASM_VAR(BYTE, g_count68cee0, $L_68cee0)
ASM_VAR(BYTE, g_count68cef0, $L_68cef0)
ASM_VAR(int, g_68d14c, $L_68d14c)
ASM_VAR(int, g_68d18c, $L_68d18c)
ASM_VAR(int, g_68a968, $L_68a968)
ASM_VAR(int, g_68d01c, $L_68d01c)
ASM_VAR(int, g_68cfe4, $L_68cfe4)
ASM_VAR(int, g_68d0e4, $L_68d0e4)
ASM_VAR(int, g_68cf34, $L_68cf34)
ASM_VAR(char, g_msg77e460, $L_77e460)
ASM_VAR(AsmLabel, s_fmt66e4d0, $L_66e4d0)
ASM_VAR(AsmLabel, s_err66cbf4, $L_66cbf4)
ASM_VAR(AsmLabel, s_err66fedc, $L_66fedc)
ASM_VAR(AsmLabel, s_err66fec8, $L_66fec8)
ASM_VAR(AsmLabel, s_err66fee8, $L_66fee8)

static int Relink_Id(BYTE* it)
{
    int id;
    CallThis(FUN_426280, &id, it);
    return id;
}

// the town of a player of state 256/257 counts it, and the player's slot there gets its id and the flag
static int Relink_Slot(BYTE* it, int line, DWORD flag)
{
    BYTE* p = CallC<BYTE*>(FUN_40a9f0, it);
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
    BYTE* e = CallThis<BYTE*>(FUN_448a60, CallThis<BYTE*>(FUN_419490, &t) + 28, k);
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
            CallC(FUN_4175c0, Relink_Id(it));
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
            CallC(FUN_417770, Relink_Id(it));
        BYTE* p = CallC<BYTE*>(FUN_40a9f0, it);
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

ASM_VAR(AsmLabel, g_tagPRES, $L_66fe40)
ASM_VAR(AsmLabel, g_tag66fe38, $L_66fe38)
ASM_VAR(AsmLabel, g_tag66fe30, $L_66fe30)
ASM_VAR(void*, g_pr68d11c, $L_68d11c)
extern "C" void FUN_4133a0(); KING_ALIAS("_FUN_4133a0", "@Position_InitNone@8")
extern "C" void FUN_41bd30(); KING_ALIAS("_FUN_41bd30", "_Presents_Append")
extern "C" void FUN_41d300(); KING_ALIAS("_FUN_41d300", "@Int_Copy@12")
extern "C" void FUN_4488e0(); KING_ALIAS("_FUN_4488e0", "@Vec16_FreeStorage@8")
extern "C" void FUN_466400(); KING_ALIAS("_FUN_466400", "_UninitFill16")
extern "C" void FUN_467180(); KING_ALIAS("_FUN_467180", "@Vec16_Reserve@16")
ASM_PROC(FUN_472410)
extern "C" void FUN_478ac0(); KING_ALIAS("_FUN_478ac0", "@Orders144_Destroy@8")
extern "C" void FUN_481740(); KING_ALIAS("_FUN_481740", "_Save_Read3Doubles")
extern "C" void FUN_487730(); KING_ALIAS("_FUN_487730", "@Vec_Construct@12")

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
        CallThis(FUN_478ac0, old + 28);
        CallThis(FUN_4488e0, old + 16);
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
        CallThis(FUN_487730, r + 28, alloc + 1);
        F(DWORD, r, 40) = 0;
    }
    F(BYTE*, e, 20) = r;
    CallThis(FUN_472410, r, cur);
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
        CallC(FUN_481740, cur, e + 32);
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
        CallC(FUN_480650, e + 96);
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
                CallC(FUN_480650, o + 72);
                CallC(FUN_480650, o + 80);
            }
        int it, copy;
        CallC(FUN_41bd30, &it, e);
        CallThis(FUN_41d300, &copy, &it);
        ((void (__cdecl*)(int, int))g_pr68d11c)(copy, 1);
    }
    LoadChunk_Close(&c);
}

} // extern "C"

// === saving a present (FUN_481790)

extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")
extern "C" void FUN_4722c0(); KING_ALIAS("_FUN_4722c0", "@Rec4722c0_Save@12")
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
    CallThis(FUN_46e460, v, v[1], (const BYTE*)p, (const BYTE*)p + n);
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
    CallC(FUN_4816d0, v, p + 32);
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
        CallThis(FUN_4722c0, o, v);
        Sp_End(&b);
    }
    Sp_End(&outer);
}

} // extern "C"
