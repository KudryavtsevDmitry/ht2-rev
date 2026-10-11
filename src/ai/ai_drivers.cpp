// ai_drivers.cpp - ai_play.cpp of the AI, rewritten from king.masm: the AI's
// players (the vehicles it drives): their ids, cargo values, and the
// counters of players per place.
//
// Building with  nmake ASM_AI_DRIVERS=1  makes king.masm assemble the original
// procedures again.

#define _CRT_SECURE_NO_WARNINGS     // strcpy as in the game
#include <string.h>
#include "road.h"

static const char s_playId[] = "$Id: ai_play.cpp 2001/8/14 15:09:54";
static const char s_playFile[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\Ai_play.cpp";

// ---------------------------------------------------------------------------
// Cargo

struct CargoPrice {                 // 24 bytes
    BYTE    m_0[12];
    double* values;                 // +12, 3 doubles per kind
    BYTE    m_16[8];
};

struct CargoItem {                  // 16 bytes
    int    kind;
    int    m_4;
    double amount;
};

struct Cargo {
    int        m_0;
    int        type;                // +4, -1: none
    BYTE       m_8[8];
    CargoItem* items;               // +16, 19 of them
    BYTE       m_20[20];
    DWORD      flags;               // +40, 1 and 2: discounts
};

typedef double* TypeValues[1];
ASM_VAR(CargoPrice*, g_cargoPrices, $L_68cc08)
ASM_VAR(TypeValues*, g_cargoTypes, $L_68cfd8)   // per type, the base value at +48

extern "C" double __cdecl Cargo_ItemsValue(const Cargo* c)
{
    double sum = 0;
    const CargoItem* item = c->items;
    const CargoPrice* price = g_cargoPrices;
    for (int i = 0; i < 19; i++, item++, price++)
        sum = sum + price->values[item->kind * 3] * item->amount;
    return sum;
}

// the value of a player's cargo, 0 without one
extern "C" double __cdecl Player_CargoValue(BYTE* player)
{
    Cargo* c = *(Cargo**)(player + 508);
    if (!c || c->type == -1)
        return 0;
    double base = *(double*)((BYTE*)(*g_cargoTypes)[c->type] + 48);
    double v = base;
    if (c->flags & 1) {
        base = base * 0.65;
        v = base;
    }
    if (c->flags & 2)
        v = 0.45 * base;
    return Cargo_ItemsValue(c) + v;
}

extern "C" void FUN_4cd730(); KING_ALIAS("_FUN_4cd730", "_Vehicle_Value4cd730")    // the value of a vrId's vehicle
ASM_PROC(FUN_4754d0)
ASM_PROC(FUN_477b60)
ASM_VAR(DWORD, g_tagPLYX, $L_66fa60)
ASM_VAR(double, g_zero64b678, $L_64b678)
ASM_VAR(double, g_one64b670, $L_64b670)
ASM_VAR(double, g_66c720, $L_66c720)
ASM_VAR(double, g_64bc20, $L_64bc20)
ASM_VAR(double, g_four64b7c0, $L_64b7c0)
ASM_VAR(double, g_64bc28, $L_64bc28)
ASM_VAR(double, g_m1_64bc30, $L_64bc30)
extern "C" void FUN_4777b0(); KING_ALIAS("_FUN_4777b0", "_Player_FirstGoods")
extern "C" void FUN_477350(); KING_ALIAS("_FUN_477350", "_Driver_Load")
extern "C" void FUN_477bd0(); KING_ALIAS("_FUN_477bd0", "_Player_SetDeadline")
extern "C" void FUN_477c30(); KING_ALIAS("_FUN_477c30", "_Player_ClearDeadline")
extern "C" void FUN_477c60(); KING_ALIAS("_FUN_477c60", "_Driver_SetDeadline")
extern "C" void FUN_40a9f0(); KING_ALIAS("_FUN_40a9f0", "_AI_ListData")
extern "C" void FUN_4776b0(); KING_ALIAS("_FUN_4776b0", "_Player_Reset477600")
extern "C" void FUN_417660(); KING_ALIAS("_FUN_417660", "_Player_IsHuman")
ASM_VAR(BYTE*, g_68d4f4, $L_68d4f4)
extern "C" void FUN_490c90(); KING_ALIAS("_FUN_490c90", "_Save_BeginBlock")
extern "C" void FUN_490c40(); KING_ALIAS("_FUN_490c40", "_Save_ParseHeader")
extern "C" void FUN_490ce0(); KING_ALIAS("_FUN_490ce0", "_Save_EndBlock")
extern "C" void FUN_490c70(); KING_ALIAS("_FUN_490c70", "_Save_BlockEnd")
ASM_PROC(FUN_479aa0)
extern "C" void FUN_477880(); KING_ALIAS("_FUN_477880", "_Player_Driver")
ASM_PROC(FUN_471ec0)
ASM_PROC(FUN_472110)
extern "C" void FUN_478980(); KING_ALIAS("_FUN_478980", "@List1000_Erase@16")
ASM_VAR(BYTE**, g_68cf38, $L_68cf38)
ASM_VAR(DWORD, g_tagCGLT, $L_66f9d0)
ASM_VAR(DWORD, g_tagGILT, $L_66fa14)
typedef char AsmLabel[1];
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
ASM_VAR(AsmLabel, g_68d1c0, $L_68d1c0)

extern "C" double __cdecl Player_Profit(BYTE* player)
{
    int vrId = *(int*)(player + 76);
    double vehicle = vrId == -1 ? 0.0 : ((double (__cdecl*)(int))FUN_4cd730)(vrId);
    return Player_CargoValue(player) - vehicle * 5000.0;
}

extern "C" double __cdecl Player_CargoValueOf(BYTE* p) { return Player_CargoValue(p + 8); }
extern "C" double __cdecl Player_ProfitOf(BYTE* p) { return Player_Profit(p + 8); }

// copies the 304 bytes of a player's state block; -1 without one
extern "C" int __cdecl Player_GetState(BYTE* player, void* dest)
{
    BYTE* block = *(BYTE**)(player + 516);
    if (!block)
        return -1;
    memcpy(dest, *(void**)(block + 16), 304);
    return 0;
}

// ---------------------------------------------------------------------------
// Players

struct PlayerListNode {
    PlayerListNode* next;
    PlayerListNode* prev;
};
ASM_VAR(PlayerListNode*, g_players, $L_68b990)  // the list's head node

struct PlaceCount {                 // 56 bytes
    BYTE m_0[28];
    int  trucks;                    // +28, players of kinds 1, 2, 5, 6
    int  others;                    // +32
    BYTE m_36[20];
};
ASM_VAR(PlaceCount*, g_places, $L_68a958)
ASM_VAR(PlaceCount*, g_placesEnd, $L_68a95c)

struct Player {
    CharVec name;
    BYTE    m_12[64];
    int     vrId;                   // +76
    BYTE    m_80[4];
    int     kind;                   // +84
    BYTE    m_88[896];
    int     place;                  // +984, -1: none

    void SetVrId(int id);
    int* Counter(int at) { return kind == 1 || kind == 2 || kind == 6 || kind == 5 ? &g_places[at].trucks : &g_places[at].others; }
    void SetPlace(int at);
    PlaceCount* GetPlace();
};

void Player::SetVrId(int id)
{
    if (id != -1)
        for (PlayerListNode* n = g_players->next; n != g_players; n = n->next) {
            Player* p = (Player*)(n + 1);
            if (p && p != this && p->vrId == id)
                AiError(0, 1, s_playFile, s_playId, 398, "Duplicate vrId", Str(&name), Str(&p->name));
        }
    vrId = id;
}

void Player::SetPlace(int at)
{
    if (place == at)
        return;
    if (place != -1)
        --*Counter(place);
    place = at;
    if (at != -1)
        ++*Counter(at);
}

PlaceCount* Player::GetPlace()
{
    if (place < 0)
        return 0;
    if ((unsigned)(g_placesEnd - g_places) > (unsigned)place)
        return &g_places[place];
    return 0;
}

// ---------------------------------------------------------------------------
// Player state (offsets into the AI's player record)

#define FIELD(type, p, off) (*(type*)((BYTE*)(p) + (off)))

extern "C" double __cdecl AI_Now();
KING_ALIAS("_AI_Now", "FUN_401000")
extern "C" void __cdecl AI_SortTimes(double* first, double* last, int, int, int);
KING_ALIAS("_AI_SortTimes", "FUN_47be40")
extern "C" const char* __cdecl AI_Text(const char* key);
KING_ALIAS("_AI_Text", "_Message_Text")
extern "C" void __cdecl Messages_Add(const char* text);          // copter.cpp
ASM_VAR(int, g_dealersBusy, $L_68d154)

static const char s_playHeader[] = "C:\\Nek\\Vrappl\\Gi\\Htai\\ai_play.h";

// the goods vector of the player's current cargo (the one at +512 first)
static BYTE* ActiveCargo(BYTE* p)
{
    BYTE* c = FIELD(BYTE*, p, 512);
    if (c && FIELD(int, c, 4) != -1)
        return c + 28;
    c = FIELD(BYTE*, p, 508);
    if (c && FIELD(int, c, 4) != -1)
        return c + 28;
    AiError(0, 1, s_playHeader, s_playId, 740, "Internal error 9012", &g_aiEmpty, &g_aiEmpty);
    return p;                       // as in the game
}

extern "C" void* __cdecl Player_FirstGoods(BYTE* p)
{
    BYTE* v = ActiveCargo(p);
    if (FIELD(void*, v, 0) == FIELD(void*, v, 4))
        return 0;
    return FIELD(void*, ActiveCargo(p), 0);
}

static void SetFlag(BYTE* p, DWORD bit, bool on)
{
    if (on)
        FIELD(DWORD, p, 96) |= bit;
    else
        FIELD(DWORD, p, 96) &= ~bit;
}

// recomputes the summary flags at +96
extern "C" void __cdecl Player_UpdateFlags(BYTE* p)
{
    SetFlag(p, 0x80, FIELD(BYTE, p, 496) != 0);
    SetFlag(p, 0x800, FIELD(void*, p, 176) != g_players);
    SetFlag(p, 0x1000, FIELD(void*, p, 160) != FIELD(void*, p, 164));
    BYTE* state = FIELD(BYTE*, p, 516);
    SetFlag(p, 0x2000, (Player_FirstGoods(p + 8) || FIELD(void*, p, 24) != FIELD(void*, p, 28))
                       && state && FIELD(int, FIELD(BYTE*, state, 16), 128));
    SetFlag(p, 0x2000000, FIELD(int, p, 92) == 1 && (FIELD(BYTE, FIELD(BYTE*, p, 516), 40) & 1));
}

// drops the licences whose time is over; tells the player how many
extern "C" void __cdecl Player_ExpireLicences(BYTE* p)
{
    char text[128];
    double*& begin = FIELD(double*, p, 144);
    double*& end = FIELD(double*, p, 148);
    if (begin == end)
        return;
    AI_SortTimes(begin, end, 0, 0, 0);
    if (begin == end)
        return;
    int n = 0;
    do {
        double* e = end;
        if (!(AI_Now() > e[-1]))
            break;
        end--;
        n++;
    } while (begin != end);
    if (n > 0 && FIELD(int, p, 92) == 1) {
        sprintf(text, AI_Text("LICENCES_EXPIRED"), n);
        Messages_Add(text);
    }
}

// the time left on the longest licence; -1 without licences
extern "C" int __cdecl Player_LicenceTimeLeft(BYTE* p, double* left)
{
    *left = 0;
    double* begin = FIELD(double*, p, 144);
    if (begin == FIELD(double*, p, 148))
        return -1;
    AI_SortTimes(begin, FIELD(double*, p, 148), 0, 0, (int)left);
    double* end = FIELD(double*, p, 148);
    double t = end[-1] - AI_Now();
    *left = t;
    if (!(t >= 0))
        *left = 0;
    return 0;
}

static const double s_never = 1.7976931348623157e308;

// a deadline in t from now; DBL_MAX: none
extern "C" void __cdecl Player_SetDeadline(BYTE* p, double t)
{
    if (x87_eq(t, s_never)) {
        FIELD(double, p, 104) = t;
        FIELD(DWORD, p, 88) |= 4;
        return;
    }
    double at = AI_Now() + t;
    FIELD(DWORD, p, 88) |= 4;
    FIELD(double, p, 104) = at;
}

extern "C" void __cdecl Player_ClearDeadline(BYTE* p)
{
    DWORD* d = (DWORD*)(p + 104);
    d[0] = 0xffffffff;              // DBL_MAX
    d[1] = 0x7fefffff;
    FIELD(DWORD, p, 88) |= 4;
}

// ends the player's deal; a dealer (kind 9) can't be released
extern "C" void __cdecl Player_EndDeal(BYTE* p)
{
    if (FIELD(int, p, 84) == 9) {
        AiError(0, 200, s_playFile, s_playId, 1797, "Dealer released", &g_aiEmpty, &g_aiEmpty);
        return;
    }
    if ((FIELD(DWORD, p, 88) >> 2 & 1) && FIELD(int, p, 92) == 256)
        FIELD(int, FIELD(BYTE*, p, 964), 12)--;
    if ((FIELD(DWORD, p, 88) >> 2 & 1) && FIELD(int, p, 92) == 257)
        g_dealersBusy--;
    FIELD(DWORD, p, 88) &= 0xf7fffffb;
    FIELD(DWORD, p, 104) = 0;
    FIELD(DWORD, p, 108) = 0;
}

extern "C" void __cdecl Player_EndDealOf(BYTE* p) { Player_EndDeal(p + 8); }

// ---------------------------------------------------------------------------
// Waypoints: positions a player is to visit, kept sorted

struct Waypoint {                   // 40 bytes
    PositionId pos;
    int        id;                  // +32
    int        m_36;
};

struct WaypointVec {
    Waypoint* begin;
    Waypoint* end;
    Waypoint* capacityEnd;

    void Add(Waypoint w);
    bool RemoveId(int id);
    bool RemoveNode(int node);
    void Clear() { end = begin; }
};

ASM_PROC(FUN_479050)    // vector<Waypoint>::insert_aux(pos, x)
extern "C" void __cdecl Waypoints_Sort(Waypoint* first, Waypoint* last, int, int, int);
KING_ALIAS("_Waypoints_Sort", "FUN_47c0a0")

// the same node, or the same place on the same road
static bool SamePlace(const Waypoint* a, const Waypoint* b)
{
    if (a->pos.node == b->pos.node && a->pos.node != -1)
        return true;
    return a->pos.road == b->pos.road && a->pos.node == b->pos.node && x87_eq(a->pos.t, b->pos.t)
        && a->pos.segment == b->pos.segment && a->pos.object == b->pos.object;
}

// Adds a waypoint and sorts them; then moves the distinct ones to the front
// as std::unique would, but leaves the vector's end where it was.
void WaypointVec::Add(Waypoint w)
{
    if (end != capacityEnd) {
        if (end)
            *end = w;
        end++;
    } else
        CallThis(FUN_479050, this, end, &w);
    Waypoints_Sort(begin, end, 0, 0, 0);
    Waypoint* last = end;
    Waypoint* d = begin;
    if (d == last || d + 1 == last)
        return;
    for (Waypoint* s = d + 1; !SamePlace(d, s); d = s, s++)
        if (s + 1 == last)
            return;
    for (Waypoint* s = d + 1; s != last; s++)
        if (!SamePlace(d, s))
            *++d = *s;
}

bool WaypointVec::RemoveId(int id)
{
    Waypoint* d = begin;
    while (d != end && d->id != id)
        d++;
    if (d == end)
        return false;
    for (Waypoint* s = d + 1; s != end; s++)
        if (s->id != id)
            *d++ = *s;
    end -= end - d;
    return true;
}

bool WaypointVec::RemoveNode(int node)
{
    Waypoint* d = begin;
    while (d != end && d->pos.node != node)
        d++;
    if (d == end)
        return false;
    for (Waypoint* s = d + 1; s != end; s++)
        if (s->pos.node != node)
            *d++ = *s;
    end -= end - d;
    return true;
}

// ---------------------------------------------------------------------------
// A player's position

extern "C" int __fastcall RoadId_Valid(const int* id, void*);          // aimain.cpp
extern "C" int __fastcall NodeId_Valid(const int* id, void*);
extern "C" int __fastcall Position_Valid(const PositionId* p, void*);
extern "C" DVec3* __cdecl Road_Point(DVec3* out, PositionId pos);
KING_ALIAS("_Road_Point", "_Road_PointOf")
extern "C" int __cdecl Player_NotValid(BYTE* player);         // aimain.cpp
extern "C" int __cdecl Vehicle_Exists(int vrId);
KING_ALIAS("_Vehicle_Exists", "FUN_4d8a40")
extern "C" void __cdecl Vehicle_Release(int vrId);
KING_ALIAS("_Vehicle_Release", "FUN_4d8ae0")
extern "C" void __cdecl Order_Destroy(void* order);
KING_ALIAS("_Order_Destroy", "@Shell_Nop@8")
typedef char AiText[1];
ASM_VAR(AiText, g_positionText, $L_77e6e0)
ASM_VAR(AiText, g_playerText, $L_77e660)

static void InvalidPosition(int line, const PositionId* p)
{
    sprintf(g_positionText, "(%d %g %d %d %x)", p->road, p->t, p->segment, p->object, p->m_24);
    AiError(33, 100, s_playFile, s_playId, line, "Invalid id", "PositionId", g_positionText);
}

// a position's segment must exist on its road
static void CheckSegment(int line, const PositionId* p)
{
    if (p->road < 0 || (unsigned)p->road >= (unsigned)(g_roadEdgesEnd - g_roadEdges))
        return;
    RoadEdge* e = g_roadEdges[p->road];
    if (p->object < e->objectsEnd - e->objects && p->segment < e->objects[p->object]->SegmentCount())
        return;
    AiError(33, 100, s_playFile, s_playId, line, "Invalid Position Id", &g_aiEmpty, &g_aiEmpty);
}

struct PlayerBase {                 // the player record behind its id
    BYTE       m_0[16];
    void*      orders;              // +16, a vector of 12-byte orders
    void*      ordersEnd;
    BYTE       m_24[8];
    PositionId pos;                 // +32
    BYTE       m_64[240];
    DVec3      point;               // +304, where pos is

    void SetPosition(const PositionId* p);
};

void PlayerBase::SetPosition(const PositionId* p)
{
    PositionId copy = *p;
    if (!((RoadId_Valid(&copy.road, 0) || NodeId_Valid(&copy.node, 0))
          && !!(copy.t >= -0.0002) && !(copy.t > 1.0002) && copy.segment >= 0 && copy.object >= 0))
        InvalidPosition(633, p);
    CheckSegment(633, p);
    DVec3 pt;
    DVec3* q = Road_Point(&pt, *p);
    point.x = q->x;
    point.y = q->y;
    point.z = q->z;
    int node = pos.node;
    if (node != -1) {
        DVec3* at = (DVec3*)((BYTE*)g_roadNodes[node] + 48);
        double dx = at->x - point.x;
        double dy = at->y - point.y;
        double dz = at->z - point.z;
        if (!(x87_sqrt((dx * dx + dy * dy) + dz * dz) >= 0.001)) {
            pos = *p;
            pos.node = node;        // still at the node
            pos.m_28 = -1;
            return;
        }
    }
    pos = *p;
    pos.m_28 = -1;
}

// puts a player at a position: releases its vehicle, drops its orders and
// waypoints
extern "C" void __cdecl Player_Place(BYTE* p, PositionId pos)
{
    if (Player_NotValid(p)) {
        sprintf(g_playerText, "()");
        AiError(33, 100, s_playFile, s_playId, 2022, "Invalid id", "PlayerId", g_playerText);
    }
    PositionId copy = pos;
    if (!Position_Valid(&copy, 0))
        InvalidPosition(2023, &pos);
    CheckSegment(2023, &pos);
    int vrId = FIELD(int, p, 84);
    if (vrId != -1 && Vehicle_Exists(vrId) != 0)
        Vehicle_Release(FIELD(int, p, 84));
    PlayerBase* b = (PlayerBase*)(p + 8);
    BYTE* begin = (BYTE*)b->orders;
    BYTE* end = (BYTE*)b->ordersEnd;
    for (BYTE* o = begin; o != (BYTE*)b->ordersEnd; o += 12)
        Order_Destroy(o);
    b->ordersEnd = (BYTE*)b->ordersEnd - (end - begin) / 12 * 12;
    FIELD(BYTE, b, 864) = 1;
    ((WaypointVec*)(p + 72))->Clear();
    b->SetPosition(&pos);
}

extern "C" void __cdecl Player_UpdateOrders(BYTE* p);

// sends a player toward a position, as the waypoint with the id
extern "C" void __cdecl Player_GoTo(BYTE* p, PositionId target, int id)
{
    PositionId copy = target;
    if (!Position_Valid(&copy, 0))
        InvalidPosition(1971, &target);
    CheckSegment(1971, &target);
    PositionId* cur = (PositionId*)(p + 40);
    copy = *cur;
    if (copy.road == -1 && copy.node == -1 && x87_eq(copy.t, 0) && copy.segment == 0
        && copy.object == 0 && copy.m_24 == 0)
        AiError(0, 100, s_playFile, s_playId, 1973, "Internal error 92667", &g_aiEmpty, &g_aiEmpty);
    copy = *cur;
    if (!Position_Valid(&copy, 0))
        InvalidPosition(1974, cur);
    CheckSegment(1974, cur);
    Waypoint w;
    w.pos = target;
    w.id = id;
    w.m_36 = 0;
    ((WaypointVec*)(p + 72))->RemoveId(w.id);
    ((WaypointVec*)(p + 72))->Add(w);
    Player_UpdateOrders(p);
    if (FIELD(void*, p, 24) == FIELD(void*, p, 28)) {
        char text[128];
        sprintf(g_positionText, "(%d %g %d %d %x)", target.road, target.t, target.segment, target.object, target.m_24);
        strcpy(text, g_positionText);
        AiError(0, 100, s_playFile, s_playId, 1998, "Internal error 5676", text, &g_aiEmpty);
    }
}

// ---------------------------------------------------------------------------
// Entry points for king.masm.

extern "C" {
void __fastcall Player_SetVrId(Player* self, void*, int id) { self->SetVrId(id); }          // [FUN_473540]
void __fastcall Player_SetPlace(Player* self, void*, int at) { self->SetPlace(at); }        // [FUN_474160]
PlaceCount* __fastcall Player_GetPlace(Player* self, void*) { return self->GetPlace(); }    // [FUN_474210]
void __fastcall Waypoints_Add(WaypointVec* self, void*, Waypoint w) { self->Add(w); }       // [FUN_477d70]
void __fastcall Waypoints_Replace(WaypointVec* self, void*, Waypoint w) { self->RemoveId(w.id); self->Add(w); } // [FUN_477e90]
bool __fastcall Waypoints_RemoveId(WaypointVec* self, void*, int id) { return self->RemoveId(id); }       // [FUN_477ec0]
bool __fastcall Waypoints_RemoveNode(WaypointVec* self, void*, int node) { return self->RemoveNode(node); } // [FUN_477fa0]
void __fastcall Player_SetPosition(PlayerBase* self, void*, const PositionId* p) { self->SetPosition(p); } // [FUN_474290]
void __fastcall Waypoints_Clear(WaypointVec* self, void*) { self->Clear(); }                // [FUN_478080]
}

// ---------------------------------------------------------------------------
// medium-sized functions: the save writers, the logo of a player

extern "C" void FUN_46e460(); KING_ALIAS("_FUN_46e460", "@CharVec_InsertRange@20")    // vector<char>::insert(pos, first, last): the save stream
extern "C" void FUN_471b10(); KING_ALIAS("_FUN_471b10", "@Rec471b10_Save@12")    // writes a 16-byte record (stream)
ASM_PROC(FUN_470e10)    // writes an order (stream)
ASM_PROC(FUN_477600)
typedef DWORD Dwords[1];
ASM_VAR(Dwords, g_playRng, $L_68b9c8)          // the AI's subtractive generator
ASM_VAR(unsigned, g_playRngI, $L_68baa4)
ASM_VAR(unsigned, g_playRngJ, $L_68baa8)
ASM_VAR(DWORD*, g_logoUsed, $L_68cc88)          // per logo: bit 0 taken
ASM_VAR(int, g_logoRequests, $L_68d18c)

static void Put(BYTE* stream, const void* p, int n)
{
    CallThis(FUN_46e460, stream, FIELD(char*, stream, 4), p, (const char*)p + n);
}

static void Put4(BYTE* stream, DWORD v) { Put(stream, &v, 4); }

extern "C" void __fastcall Rec471bf0_Save(DWORD* self, void*, BYTE* stream)
{
    Put4(stream, self[6]);
    Put4(stream, self[1]);
    Put4(stream, self[0]);
    Put(stream, self + 2, 8);
    Put4(stream, self[4]);
    Put4(stream, self[5]);
    Put4(stream, self[7]);
    Put4(stream, self[8]);
}

// a list of players by name: the count, each name's length and bytes (0 for none)
extern "C" void __fastcall PlayerRefs_Save(BYTE** self, void*, BYTE* stream)
{
    Put4(stream, (self[1] - self[0]) >> 4);
    for (unsigned i = 0; i < (unsigned)((self[1] - self[0]) >> 4); i++) {
        BYTE* e = FIELD(BYTE*, self[0], i * 16);
        if (e == (BYTE*)g_players) {
            Put4(stream, 0);
            continue;
        }
        Put4(stream, FIELD(BYTE*, e, 12) - FIELD(BYTE*, e, 8));
        for (unsigned j = 0; j < (unsigned)(FIELD(BYTE*, e, 12) - FIELD(BYTE*, e, 8)); j++) {
            char c = FIELD(char*, e, 8)[j];
            Put(stream, &c, 1);
        }
    }
    Put4(stream, (DWORD)self[3]);
}

extern "C" void __fastcall Rec4722c0_Save(DWORD* self, void*, BYTE* stream)
{
    Put4(stream, self[1]);
    Put4(stream, self[1]);
    Put4(stream, self[0]);
    Put4(stream, (int)(self[5] - self[4]) >> 4);
    for (unsigned i = 0; i < (unsigned)((int)(self[5] - self[4]) >> 4); i++)
        CallThis(FUN_471b10, (BYTE*)self[4] + i * 16, stream);
    Put4(stream, (int)(self[8] - self[7]) / 144);
    for (unsigned i = 0; i < (unsigned)((int)(self[8] - self[7]) / 144); i++)
        CallThis(FUN_470e10, (BYTE*)self[7] + i * 144, stream);
    Put4(stream, self[10]);
}

extern "C" void __cdecl Doubles5_Save(BYTE* stream, const double* d)
{
    for (int i = 0; i < 5; i++)
        Put(stream, d + i, 8);
}

// a player's logo: a random free one of 8, else the first if free
extern "C" void __cdecl Player_AllocLogo(BYTE* p)
{
    g_logoRequests++;
    if (FIELD(int, p, 156) != -1)
        return;
    DWORD* used = g_logoUsed;
    int i = 0;
    for (; i < 100; i++) {
        unsigned a = (g_playRngI + 1) % 55;
        unsigned b = (g_playRngJ + 1) % 55;
        g_playRngI = a;
        g_playRngJ = b;
        g_playRng[a] -= g_playRng[b];
        unsigned r = g_playRng[g_playRngI] & 7;
        if (!(used[r] & 1)) {
            used[r] |= 1;
            FIELD(int, p, 156) = r;
            used = g_logoUsed;
            break;
        }
    }
    if (FIELD(int, p, 156) == -1)
        for (; i < 8; i++)
            if (!(used[0] & 1)) {
                used[0] |= 1;
                FIELD(int, p, 156) = 0;
                break;
            }
    if (FIELD(int, p, 156) == -1)
        AiError(0, 256, s_playFile, s_playId, 1318, "Can not allocate a logo", &g_aiEmpty, &g_aiEmpty);
}

// FUN_477600 on a player's waypoint records and on its orders' records
extern "C" void __cdecl Player_Reset477600(BYTE* p)
{
    for (BYTE* e = FIELD(BYTE*, p, 152); e != FIELD(BYTE*, p, 156); e += 16)
        CallC(FUN_477600, e);
    FIELD(BYTE*, p, 164) = FIELD(BYTE*, p, 152);
    CallC(FUN_477600, p + 168);
    for (int k = 508; k <= 512; k += 4) {
        BYTE* o = FIELD(BYTE*, p, k);
        if (!o)
            continue;
        for (BYTE* it = FIELD(BYTE*, o, 28); it != FIELD(BYTE*, FIELD(BYTE*, p, k), 32); it += 144)
            for (BYTE* e = FIELD(BYTE*, it, 120); e != FIELD(BYTE*, it, 124); e += 16)
                CallC(FUN_477600, e);
    }
}

// loads a road record from the save stream
extern "C" void FUN_471e90(); KING_ALIAS("_FUN_471e90", "@Rec471e90_Load@12")    // loads the vector at +120 (cursor)

static void Get(BYTE** cur, BYTE* self, int off, int n)
{
    memcpy(self + off, *cur, n);
    *cur += n;
}

extern "C" void __fastcall Rec4710a0_Load(BYTE* self, void*, BYTE** cur)
{
    static const signed char order[][2] = {
        {0, 4}, {12, 4}, {16, 4}, {48, 4}, {28, 4}, {24, 4}, {32, 8}, {40, 4}, {44, 4},
        {52, 4}, {56, 8}, {64, 8}, {96, 8}, {72, 8}, {80, 8}, {88, 8}, {104, 4}, {112, 8},
    };
    for (int i = 0; i < (int)(sizeof order / sizeof order[0]); i++)
        Get(cur, self, (unsigned char)order[i][0], order[i][1]);
    CallThis(FUN_471e90, self + 120, cur);
    Get(cur, self, 4, 4);
    Get(cur, self, 8, 4);
}

// --- save/load of the drivers' records: the save stream is a vector<char>
// appended with [FUN_46e460]; the load cursor is a BYTE* advanced by reads.

static void Save_Write(BYTE* s, const void* p, int n)
{
    CallThis(FUN_46e460, s, F(BYTE*, s, 4), (const BYTE*)p, (const BYTE*)p + n);
}

static void Load_Read(BYTE** cur, void* p, int n)
{
    memcpy(p, *cur, n);
    *cur += n;
}

struct LoadChunk {
    BYTE** cur;
    int a, b;
    DWORD tag;
};

static bool Load_Begin(LoadChunk* c, BYTE** cur, DWORD tag, int line, const char* err)
{
    c->cur = cur;
    CallC(FUN_490c40, cur, &c->tag, &c->a, &c->b);
    if (c->tag == tag)
        return true;
    AiError(0, 100, s_playFile, s_playId, line, err, (const char*)g_68d1c0, (const char*)g_68d1c0);
    return false;
}

static void Load_End(LoadChunk* c)
{
    CallC(FUN_490c70, c->cur, c->a, c->b);
}

extern "C" {

// [FUN_4718b0] saves a waypoint: kind (2 bytes), value, and its node's id (or -1)
void __fastcall Waypoint_Save(BYTE* self, void*, BYTE* s)
{
    WORD k = F(WORD, self, 8);
    Save_Write(s, &k, 2);
    DWORD v = F(DWORD, self, 0);
    Save_Write(s, &v, 4);
    BYTE* n = F(BYTE*, self, 4);
    int id = n ? F(int, n, 12) : -1;
    Save_Write(s, &id, 4);
}

// [FUN_471940] loads a waypoint
void __fastcall Waypoint_Load(BYTE* self, void*, BYTE** cur)
{
    Load_Read(cur, self + 8, 2);
    Load_Read(cur, self, 4);
    int id;
    Load_Read(cur, &id, 4);
    F(BYTE*, self, 4) = id == -1 ? 0 : g_68cf38[id];
    F(BYTE, self, 10) = 255;
}

// [FUN_471b10] saves a word and a double
void __fastcall Rec471b10_Save(BYTE* self, void*, BYTE* s)
{
    Save_Write(s, self, 4);
    Save_Write(s, self + 8, 8);
}

// [FUN_471b70]
void __fastcall Rec471b10_Load(BYTE* self, void*, BYTE** cur)
{
    Load_Read(cur, self, 4);
    Load_Read(cur, self + 8, 8);
}

// [FUN_471ba0] saves 8 bytes
void __fastcall Pair_Save(BYTE* self, void*, BYTE* s)
{
    Save_Write(s, self, 8);
}

// [FUN_471bd0]
void __fastcall Pair_Load(BYTE* self, void*, BYTE** cur)
{
    Load_Read(cur, self, 8);
}

// [FUN_471cf0]
void __fastcall Rec471cf0_Load(BYTE* self, void*, BYTE** cur)
{
    Load_Read(cur, self + 24, 4);
    Load_Read(cur, self + 4, 4);
    Load_Read(cur, self, 4);
    Load_Read(cur, self + 8, 8);
    Load_Read(cur, self + 16, 4);
    Load_Read(cur, self + 20, 4);
    Load_Read(cur, self + 28, 4);
    Load_Read(cur, self + 32, 4);
}

// [FUN_471e90]
void __fastcall Rec471e90_Load(BYTE* self, void*, BYTE** cur)
{
    CallC(FUN_479aa0, cur, self);
    Load_Read(cur, self + 12, 4);
}

// [FUN_472270]
void __cdecl Player_Call471ec0(int player)
{
    BYTE* p = CallC<BYTE*>(FUN_477880, player);
    if (p)
        CallC(FUN_471ec0, p + 120, player);
}

// [FUN_4722a0]
void __cdecl Player_Call472110(int player)
{
    BYTE* p = CallC<BYTE*>(FUN_477880, player);
    if (p)
        CallC(FUN_472110, p + 120);
}

// [FUN_4725b0]
void __fastcall Rec4725b0_Init(BYTE* self, void*)
{
    F(BYTE, self, 0) = 0;
    F(int, self, 8) = 0;
    F(int, self, 12) = 0;
    F(int, self, 16) = 0;
    F(int, self, 40) = 0;
    F(int, self, 48) = 0;
    F(int, self, 56) = 0;
    F(int, self, 44) = 0;
    F(int, self, 52) = 0;
    F(int, self, 60) = 0;
    F(BYTE, self, 64) = 0;
}

// [FUN_4725e0]
void __fastcall Rec4725e0_Init(BYTE* self, void*)
{
    F(BYTE, self, 1) = 0;
    F(int, self, 24) = 0;
    F(int, self, 28) = 0;
    F(int, self, 32) = 0;
}

// [FUN_4725f0] saves a CGLT chunk: +8, then +0..+7
void __fastcall Cglt_Save(BYTE* self, void*, BYTE* s)
{
    int pos;
    CallC(FUN_490c90, s, (const DWORD*)&g_tagCGLT, &pos);
    Save_Write(s, self + 8, 4);
    Save_Write(s, self, 8);
    CallC(FUN_490ce0, s, &pos);
}

// [FUN_4726a0]
void __fastcall Cglt_Load(BYTE* self, void*, BYTE** cur)
{
    LoadChunk c;
    if (Load_Begin(&c, cur, g_tagCGLT, 225, "Game load error 0678")) {
        Load_Read(cur, self + 8, 4);
        Load_Read(cur, self, 8);
    }
    Load_End(&c);
}

// [FUN_472780] saves a GILT chunk: two CGLT chunks and the fields
void __fastcall Gilt_Save(BYTE* self, void*, BYTE* s)
{
    int pos;
    CallC(FUN_490c90, s, (const DWORD*)&g_tagGILT, &pos);
    Cglt_Save(self + 8, 0, s);
    Cglt_Save(self + 24, 0, s);
    Save_Write(s, self, 1);
    Save_Write(s, self + 40, 8);
    Save_Write(s, self + 48, 8);
    Save_Write(s, self + 56, 8);
    Save_Write(s, self + 64, 1);
    Save_Write(s, self + 1, 1);
    CallC(FUN_490ce0, s, &pos);
}

// [FUN_4728c0]
void __fastcall Gilt_Load(BYTE* self, void*, BYTE** cur)
{
    LoadChunk c;
    if (Load_Begin(&c, cur, g_tagGILT, 257, "Game load error 0679")) {
        Cglt_Load(self + 8, 0, cur);
        Cglt_Load(self + 24, 0, cur);
        Load_Read(cur, self, 1);
        Load_Read(cur, self + 40, 8);
        Load_Read(cur, self + 48, 8);
        Load_Read(cur, self + 56, 8);
        Load_Read(cur, self + 64, 1);
        Load_Read(cur, self + 1, 1);
    }
    Load_End(&c);
}

// [FUN_4734d0]
int __cdecl Players_Call478980(int x)
{
    return CallThis<int>(FUN_478980, &g_players, &x, x);
}

// [FUN_474260] whether the player has a place with flag 1
int __cdecl Player_HasPlaceFlag1(Player* p)
{
    if (!Player_GetPlace(p, 0))
        return 0;
    return (F(BYTE, Player_GetPlace(p, 0), 16) & 1) ? 1 : 0;
}

// [FUN_474560] loads five doubles
void __cdecl Doubles5_Load(BYTE** cur, BYTE* out)
{
    Load_Read(cur, out, 40);
}

// [FUN_4745d0] saves three doubles
void __cdecl Doubles3_Save(BYTE* s, const BYTE* v)
{
    Save_Write(s, v, 8);
    Save_Write(s, v + 8, 8);
    Save_Write(s, v + 16, 8);
}

// [FUN_474650] saves a 4x3 double matrix
void __cdecl Matrix43_Save(BYTE* s, const BYTE* m)
{
    for (int i = 0; i < 12; i++)
        Save_Write(s, m + i * 8, 8);
}

// [FUN_4746e0]
void __cdecl Matrix43_Load(BYTE** cur, BYTE* m)
{
    Load_Read(cur, m, 96);
}

}

extern "C" {

// [FUN_4754b0] an empty waypoint
BYTE* __fastcall Waypoint_Init(BYTE* self, void*)
{
    F(int, self, 0) = -1;
    F(int, self, 4) = 0;
    F(WORD, self, 8) = 0;
    F(BYTE, self, 10) = 255;
    return self;
}

// [FUN_476890] saves a name: its length, then its bytes one by one (0 for none)
void __fastcall Name_Save(BYTE** self, void*, BYTE* s)
{
    BYTE* str = self[0];
    if (str == (BYTE*)g_players) {
        int z = 0;
        Save_Write(s, &z, 4);
        return;
    }
    int n = F(BYTE*, str, 12) - F(BYTE*, str, 8);
    Save_Write(s, &n, 4);
    for (unsigned i = 0; i < (unsigned)(F(BYTE*, str, 12) - F(BYTE*, str, 8)); i++) {
        BYTE c = F(BYTE*, str, 8)[i];
        Save_Write(s, &c, 1);
    }
}

// [FUN_4769e0] copies 19 16-byte entries into the object's table at +516
int __cdecl Vehicle_SetTable516(BYTE* o, const BYTE* src)
{
    if (!F(BYTE*, o, 516))
        return -1;
    for (int off = 0; off < 304; off += 16)
        memcpy(F(BYTE*, F(BYTE*, o, 516), 16) + off, src + off, 16);
    return 0;
}

// [FUN_476b50] releases the player's logo
void __cdecl Player_ReleaseLogo(BYTE* p)
{
    g_logoRequests--;
    if (F(int, p, 92) == 1)
        return;
    int logo = F(int, p, 156);
    if (logo != -1) {
        g_logoUsed[logo] &= ~1u;
        F(int, p, 156) = -1;
        return;
    }
    AiError(0, 256, s_playFile, s_playId, 1333, "Internal error 569", (const char*)g_68d1c0, (const char*)g_68d1c0);
}

// [FUN_4771a0] the next entry of the player's list at +164 (or the list head)
DWORD* __cdecl Player_NextEntry(DWORD* out, BYTE* p)
{
    BYTE* e = F(BYTE*, p, 172);
    if (e == F(BYTE*, p, 164)) {
        *out = (DWORD)g_players;
        return out;
    }
    F(BYTE*, p, 172) = e + 16;
    *out = F(DWORD, e, 0);
    return out;
}

// [FUN_4771e0] restarts the walk and returns the first entry
DWORD* __cdecl Player_FirstEntry(DWORD* out, BYTE* p)
{
    F(BYTE*, p, 172) = F(BYTE*, p, 160);
    return Player_NextEntry(out, p);
}

// [FUN_4772e0]
void __cdecl Driver_ClearFlags(BYTE* x)
{
    BYTE* o = F(BYTE*, x, 508);
    if (o)
        F(DWORD, o, 40) &= ~1u;
    F(DWORD, x, 88) &= 0xFDFFFFFF;
}

// [FUN_477300]
void __cdecl Player_ClearFlags(BYTE* p)
{
    Driver_ClearFlags(p + 8);
}

// [FUN_4773d0]
int __cdecl Player_Call477350(BYTE* p)
{
    return CallC<int>(FUN_477350, p + 8);
}

// [FUN_477880] the player's driver record
int __cdecl Player_Driver(BYTE* p)
{
    return CallC<int>(FUN_4777b0, p + 8);
}

// [FUN_477c10]
int __cdecl Player_Call477bd0(BYTE* p, int a, int b)
{
    return CallC<int>(FUN_477bd0, p + 8, a, b);
}

// [FUN_477c50]
int __cdecl Player_Call477c30(BYTE* p)
{
    return CallC<int>(FUN_477c30, p + 8);
}

// [FUN_477cb0]
int __cdecl Player_Call477c60(BYTE* p, int a, int b)
{
    return CallC<int>(FUN_477c60, p + 8, a, b);
}

// [FUN_4773e0] drops the last time entry (sorting the list first)
void __cdecl Player_PopTime(BYTE* p)
{
    BYTE* b = F(BYTE*, p, 144);
    if (b == F(BYTE*, p, 148))
        return;
    AI_SortTimes((double*)b, F(double*, p, 148), 0, 0, (int)p);
    F(BYTE*, p, 148) -= 8;
}

// [FUN_477780] calls [FUN_4776b0] on every player
void __cdecl Players_Each4776b0()
{
    BYTE* head = (BYTE*)g_players;
    for (BYTE* it = *(BYTE**)head; it != (BYTE*)g_players; it = *(BYTE**)it)
        CallC(FUN_4776b0, CallC<BYTE*>(FUN_40a9f0, it));
}

// [FUN_477b00] selects the current player
void __cdecl Players_SetCurrent(BYTE* p)
{
    if (p != (BYTE*)g_players && !CallC<bool>(FUN_417660, p)) {
        AiError(0, 1, s_playFile, s_playId, 1717, "Internal error 943", (const char*)g_68d1c0, (const char*)g_68d1c0);
        return;
    }
    g_68d4f4 = p;
}

// [FUN_4788c0] calls [FUN_477600] on every 16-byte entry of every 144-byte record
void __cdecl Route_Each477600(BYTE* x)
{
    BYTE* v = F(BYTE*, x, 20);
    if (!v)
        return;
    for (BYTE* e = F(BYTE*, v, 28); e != F(BYTE*, F(BYTE*, x, 20), 32); e += 144)
        for (BYTE* q = F(BYTE*, e, 120); q != F(BYTE*, e, 124); q += 16)
            CallC(FUN_477600, q);
}

}

extern "C" {

// [FUN_4758d0] saves the player: a PLYX chunk when only the basics changed
void __fastcall Player_SaveX(BYTE* self, void*, BYTE* s)
{
    if (F(BYTE, self, 864) || F(int, self, 868) != F(int, self, 88) || F(int, self, 872) != F(int, self, 92)) {
        CallThis(FUN_4754d0, self, s);
        return;
    }
    CallThis(FUN_46e460, s, F(BYTE*, s, 4), F(const BYTE*, self, 0), F(const BYTE*, self, 4));
    int pos;
    CallC(FUN_490c90, s, (const DWORD*)&g_tagPLYX, &pos);
    Save_Write(s, self + 88, 4);
    static const int words[] = {56, 36, 32};
    for (int i = 0; i < 3; i++)
        Save_Write(s, self + words[i], 2);
    float f = (float)F(double, self, 40);
    Save_Write(s, &f, 4);
    static const int words2[] = {48, 52, 60};
    for (int i = 0; i < 3; i++)
        Save_Write(s, self + words2[i], 2);
    f = (float)F(double, self, 328);
    Save_Write(s, &f, 4);
    f = (float)F(double, self, 496);
    Save_Write(s, &f, 4);
    Save_Write(s, self + 504, 1);
    Save_Write(s, self + 720, 1);
    Save_Write(s, self + 716, 1);
    CallC(FUN_490ce0, s, &pos);
}

// [FUN_4769c0] the table value (i, j)
double __cdecl Table68cc08_Get(int i, int j)
{
    const double* row = F(const double*, (BYTE*)g_cargoPrices + i * 24, 12);
    return row[j * 3];
}

// [FUN_477310] the driver's cargo weight factor
double __cdecl Driver_CargoFactor(BYTE* x)
{
    BYTE* o = F(BYTE*, x, 516);
    if (!o || !(F(BYTE, o, 40) & 1))
        return g_zero64b678;
    BYTE* t = ((BYTE**)g_cargoTypes)[F(int, o, 4)];
    return (g_one64b670 - g_66c720) * F(double, t, 48) * g_64bc20;
}

// [FUN_477350] the load the driver carries (scaled by the current race's factor)
double __cdecl Driver_Load(BYTE* x)
{
    BYTE* r;
    double f = 1.0;
    CallC(FUN_477b60, &r);
    if (r != (BYTE*)g_players)
        f = F(double, r, 768) * g_four64b7c0 + g_one64b670;
    BYTE* o = F(BYTE*, x, 508);
    if (!o || F(int, o, 4) == -1)
        return g_zero64b678;
    BYTE* t = ((BYTE**)g_cargoTypes)[F(int, o, 4)];
    return f * F(double, t, 48) * g_64bc28;
}

// [FUN_477c60] sets the deadline: t, or now + t (-1 keeps it as it is)
void __cdecl Driver_SetDeadline(BYTE* x, double t)
{
    if (!(t < g_m1_64bc30) && !(t > g_m1_64bc30)) {
        F(double, x, 104) = t;
    } else {
        F(double, x, 104) = AI_Now() + t;
    }
    F(DWORD, x, 88) |= 0x8000000;
}

}

ASM_PROC(FUN_4729e0)    // player constructor (kind)
ASM_PROC(FUN_4731e0)    // player destructor
extern "C" void FUN_478e50(); KING_ALIAS("_FUN_478e50", "@AiList_Insert@20")
extern "C" void FUN_426280(); KING_ALIAS("_FUN_426280", "@Int_Set@12")
extern "C" void FUN_4afc20(); KING_ALIAS("_FUN_4afc20", "@SchList_End@12")

extern "C" {

// [FUN_473420] appends a new player of a kind to the player list; its
// +104 is set to its own iterator; returns the player
BYTE* __cdecl Players_Append(int kind)
{
    BYTE p[1000];
    CallThis(FUN_4729e0, p, kind);
    void* end;
    CallThis(FUN_4afc20, &g_players, &end);
    void* out;
    CallThis(FUN_478e50, &g_players, &out, end, p);
    CallThis(FUN_4731e0, p);
    void** last = (void**)((void**)g_players)[1];
    int it;
    int* r = CallThis<int*>(FUN_426280, &it, last);
    BYTE* node = (BYTE*)((void**)g_players)[1];
    F(int, node, 104) = *r;
    return (BYTE*)((void**)g_players)[1] + 8;
}

}

ASM_PROC(FUN_4872f0)    // re-plans a player's route to a position (player, position by value)
ASM_PROC(FUN_5d2ba0)    // console: print (text, 0)
extern "C" const char* __cdecl Rec_Name8(BYTE* p);

static void PrintPosition(const PositionId* p)
{
    sprintf(g_positionText, "(%d %g %d %d %x)", p->road, p->t, p->segment, p->object, p->m_24);
    CallC(FUN_5d2ba0, g_positionText, 0);
}

static void Replan(BYTE* p, const DWORD* to)
{
    CallC(FUN_4872f0, p, to[0], to[1], to[2], to[3], to[4], to[5], to[6], to[7]);
}

extern "C" {

// [FUN_478100] re-plans the route to the last waypoint unless it is already
// the last node of the route; when that leaves no route, prints the player's
// name, the waypoint and its position
void __cdecl Player_UpdateOrders(BYTE* p)
{
    BYTE* wEnd = F(BYTE*, p, 76);
    if (F(BYTE*, p, 72) == wEnd)
        return;
    if (F(BYTE*, p, 24) != F(BYTE*, p, 28)) {
        int n = F(int, wEnd, -36);
        if (n != -1 && n == F(int, F(BYTE*, p, 28), -12))
            return;
    }
    Replan(p, (const DWORD*)(F(BYTE*, p, 76) - 40));
    if (F(BYTE*, p, 24) == F(BYTE*, p, 28))
        Replan(p, (const DWORD*)(F(BYTE*, p, 76) - 40));
    if (F(BYTE*, p, 24) != F(BYTE*, p, 28))
        return;
    CallC(FUN_5d2ba0, Rec_Name8(p), 0);
    PrintPosition((const PositionId*)(F(BYTE*, p, 76) - 40));
    PositionId here = *(const PositionId*)(p + 40);
    PrintPosition(&here);
}

}

extern "C" void FUN_4593b0(); KING_ALIAS("_FUN_4593b0", "_Vec12b_AllocCopyA@12")    // allocate n 12-byte records and copy [first, last) (stdcall)
ASM_PROC(FUN_47c650)    // copy 12-byte records (first, last, to)
extern "C" void FUN_47e3e0(); KING_ALIAS("_FUN_47e3e0", "_Stl_UninitCopy12")    // uninitialized copy of 12-byte records
extern "C" void FUN_484690(); KING_ALIAS("_FUN_484690", "_Road_OrientHops")    // orients the hops of a route
ASM_PROC(FUN_44b4a0)

extern "C" {

// [FUN_471990] assigns the hops (12-byte records) of src to the route at
// +16, orients them and marks the route changed (+864)
void __fastcall Player_SetHops(BYTE* self, void*, BYTE* const* src)
{
    BYTE** v = (BYTE**)(self + 16);
    if ((BYTE* const*)v != src) {
        BYTE* first = src[0];
        BYTE* last = src[1];
        unsigned n = (last - first) / 12;
        if (n > (unsigned)((v[2] - v[0]) / 12)) {
            BYTE* p = CallThis<BYTE*>(FUN_4593b0, v, n, first, last);
            int cap = (v[2] - v[0]) / 12;
            if (cap)
                CallC(FUN_44b4a0, v[0], cap * 12);
            v[0] = p;
            v[2] = p + n * 12;
        } else if ((unsigned)((v[1] - v[0]) / 12) >= n) {
            CallC(FUN_47c650, first, last, v[0], src, 0);
        } else {
            int have = (v[1] - v[0]) / 12;
            BYTE* d = v[0];
            BYTE* s = first;
            for (int i = 0; i < have; i++, s += 12, d += 12) {
                F(DWORD, d, 0) = F(DWORD, s, 0);
                F(DWORD, d, 4) = F(DWORD, s, 4);
                F(DWORD, d, 8) = F(DWORD, s, 8);
            }
            CallC(FUN_47e3e0, src[0] + have * 12, src[1], v[1], src);
        }
        v[1] = v[0] + n * 12;
    }
    CallC(FUN_484690, v);
    F(BYTE, self, 864) = 1;
}

}

extern "C" void FUN_461b00(); KING_ALIAS("_FUN_461b00", "@Rec41ce50_DeletingDestructor@12")    // 144-byte order record: destructor (flag)

// destroys the 144-byte records of the vector at +28 of r
static void Orders144_Clear(BYTE* r)
{
    BYTE* b = F(BYTE*, r, 28);
    BYTE* e = F(BYTE*, r, 32);
    for (BYTE* p = b; p != F(BYTE*, r, 32); p += 144)
        CallThis(FUN_461b00, p, 0);
    F(BYTE*, r, 32) = F(BYTE*, r, 32) - ((e - b) / 144) * 144;
}

extern "C" {

// [FUN_4778a0] clears the order lists of the player's two order sets
void __cdecl Player_ClearOrderLists(BYTE* p)
{
    if (F(BYTE*, p, 508))
        Orders144_Clear(F(BYTE*, p, 508));
    if (F(BYTE*, p, 512))
        Orders144_Clear(F(BYTE*, p, 512));
}

}

ASM_PROC(FUN_479370)    // vector of 40-byte records: assignment
extern "C" void FUN_47acd0(); KING_ALIAS("_FUN_47acd0", "_Alloc40_Copy@12")    // allocate n 40-byte records and copy (stdcall)
extern "C" void FUN_47e1a0(); KING_ALIAS("_FUN_47e1a0", "_Copy40B")    // copy 40-byte records
extern "C" void FUN_47d330(); KING_ALIAS("_FUN_47d330", "_Stl_UninitCopy40")    // uninitialized copy of 40-byte records

extern "C" {

// [FUN_4736e0] swaps two vectors of 40-byte records by copying
void __cdecl Recs40_Swap(BYTE** a, BYTE** b)
{
    BYTE* t[3] = {0, 0, 0};
    CallThis(FUN_479370, t, a);
    CallThis(FUN_479370, a, b);
    if ((BYTE**)t != b) {
        unsigned n = (t[1] - t[0]) / 40;
        if (n > (unsigned)((b[2] - b[0]) / 40)) {
            BYTE* p = CallThis<BYTE*>(FUN_47acd0, b, n, t[0], t[1]);
            int cap = (b[2] - b[0]) / 40;
            if (cap)
                CallC(FUN_44b4a0, b[0], cap * 40);
            b[0] = p;
            b[2] = p + n * 40;
        } else if ((unsigned)((b[1] - b[0]) / 40) >= n) {
            CallC(FUN_47e1a0, t[0], t[1], b[0], b, 0);
        } else {
            int have = (b[1] - b[0]) / 40;
            CallC(FUN_47e1a0, t[0], t[0] + have * 40, b[0], b, 0);
            CallC(FUN_47d330, t[0] + ((b[1] - b[0]) / 40) * 40, t[1], b[1], b);
        }
        b[1] = b[0] + n * 40;
    }
    int cap = (t[2] - t[0]) / 40;
    if (cap)
        CallC(FUN_44b4a0, t[0], cap * 40);
}

}
