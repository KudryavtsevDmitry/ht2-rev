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
ASM_PROC(FUN_470900)
extern "C" void FUN_44ac60(); KING_ALIAS("_FUN_44ac60", "_Vec4b_AllocCopy@12")    // vector<pointer>: a new buffer (n, first, last)
ASM_PROC(FUN_44b4a0)    // the AI's allocator: free (p, size)
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
ASM_PROC(FUN_46e460)
ASM_PROC(FUN_475b20)
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
ASM_PROC(FUN_4810e0)
ASM_PROC(FUN_481790)
ASM_PROC(FUN_482320)
typedef char AsmLabel[1];
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
