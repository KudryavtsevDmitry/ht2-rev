// ai_saveload.cpp - ai_svld.cpp of the AI, rewritten from king.masm: helpers
// of its save files.
//
// Building with  nmake ASM_AI_SAVELOAD=1  makes king.masm assemble the original
// procedures again.

#include "king.h"
#include "x87.h"

#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)
extern "C" void FUN_470900(); KING_ALIAS("_FUN_470900", "@CharVec_Construct@12")
extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector<pointer>: a new buffer (n, first, last)
ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
extern "C" void FUN_4471f0(); KING_ALIAS("_FUN_4471f0", "@CharVec_Erase@16")
extern "C" void FUN_47aad0(); KING_ALIAS("_FUN_47aad0", "@Bytes_Reserve@12")
extern "C" void FUN_47ab20(); KING_ALIAS("_FUN_47ab20", "@CharVec_PushBack@12")
extern "C" void FUN_410df0(); KING_ALIAS("_FUN_410df0", "@CharVec_Str@8")
extern "C" void FUN_407320(); KING_ALIAS("_FUN_407320", "@CharVec_Equals@12")
extern "C" void FUN_483c10(); KING_ALIAS("_FUN_483c10", "_AI_UpdateSwitch")
extern "C" void FUN_490c40(); KING_ALIAS("_FUN_490c40", "_Save_ParseHeader")
extern "C" void FUN_490c70(); KING_ALIAS("_FUN_490c70", "_Save_BlockEnd")
extern "C" void FUN_473420(); KING_ALIAS("_FUN_473420", "_Players_Append")
ASM_PROC(FUN_474720)
extern "C" void FUN_480650(); KING_ALIAS("_FUN_480650", "_Save_RebaseTime")
extern "C" void FUN_422390(); KING_ALIAS("_FUN_422390", "_Player_Register")
ASM_PROC(FUN_420000)
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
ASM_PROC(FUN_46e460)
ASM_PROC(FUN_475b20)
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
extern "C" void FUN_4810e0(); KING_ALIAS("_FUN_4810e0", "_Save_Town")
ASM_PROC(FUN_481790)
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
KING_ALIAS("_AiError", "FUN_401010")
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
