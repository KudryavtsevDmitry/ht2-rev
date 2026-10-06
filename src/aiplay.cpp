// aiplay.cpp - ai_play.cpp of the AI, rewritten from king.masm: the AI's
// players (the vehicles it drives): their ids, cargo values, and the
// counters of players per place.
//
// Building with  nmake ASM_AIPLAY=1  makes king.masm assemble the original
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

ASM_PROC(FUN_4cd730)    // the value of a vrId's vehicle

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
KING_ALIAS("_AI_Text", "FUN_40e270")
extern "C" void __cdecl AI_ScreenMessage(const char* text);
KING_ALIAS("_AI_ScreenMessage", "FUN_56ae60")
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
        AI_ScreenMessage(text);
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

ASM_PROC(FUN_423a30)    // RoadId::IsValid
ASM_PROC(FUN_423a60)    // NodeId::IsValid
ASM_PROC(FUN_40a500)    // PositionId::IsValid
extern "C" DVec3* __cdecl Road_Point(DVec3* out, PositionId pos);
KING_ALIAS("_Road_Point", "FUN_435bf0")
extern "C" int __cdecl Player_IsInvalid(void* player);
KING_ALIAS("_Player_IsInvalid", "FUN_4566c0")
extern "C" int __cdecl Vehicle_Exists(int vrId);
KING_ALIAS("_Vehicle_Exists", "FUN_4d8a40")
extern "C" void __cdecl Vehicle_Release(int vrId);
KING_ALIAS("_Vehicle_Release", "FUN_4d8ae0")
extern "C" void __cdecl Order_Destroy(void* order);
KING_ALIAS("_Order_Destroy", "FUN_50d190")
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
    if (!((CallThis<int>(FUN_423a30, &copy) || CallThis<int>(FUN_423a60, &copy.node))
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
    if (Player_IsInvalid(p)) {
        sprintf(g_playerText, "()");
        AiError(33, 100, s_playFile, s_playId, 2022, "Invalid id", "PlayerId", g_playerText);
    }
    PositionId copy = pos;
    if (!CallThis<int>(FUN_40a500, &copy))
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
KING_ALIAS("_Player_UpdateOrders", "FUN_478100")

// sends a player toward a position, as the waypoint with the id
extern "C" void __cdecl Player_GoTo(BYTE* p, PositionId target, int id)
{
    PositionId copy = target;
    if (!CallThis<int>(FUN_40a500, &copy))
        InvalidPosition(1971, &target);
    CheckSegment(1971, &target);
    PositionId* cur = (PositionId*)(p + 40);
    copy = *cur;
    if (copy.road == -1 && copy.node == -1 && x87_eq(copy.t, 0) && copy.segment == 0
        && copy.object == 0 && copy.m_24 == 0)
        AiError(0, 100, s_playFile, s_playId, 1973, "Internal error 92667", &g_aiEmpty, &g_aiEmpty);
    copy = *cur;
    if (!CallThis<int>(FUN_40a500, &copy))
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
