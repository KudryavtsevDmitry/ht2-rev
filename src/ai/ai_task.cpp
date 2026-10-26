// ai_task.cpp - AI_TASK.CPP of the AI, rewritten from king.masm: how the AI
// reaches the game: its cycle task, the switches of the game screen, screen
// messages and banners. The unit's static initializers (an STL random
// generator, the message buffer) are compiler-generated and stay assembly.
//
// Building with  nmake ASM_AI_TASK=1  makes king.masm assemble the original
// procedures again.

#include "ai.h"

extern "C" void FUN_42d6e0(); KING_ALIAS("_FUN_42d6e0", "_Ai_Tick")    // the AI's cycle
extern "C" void FUN_5d9290(); KING_ALIAS("_FUN_5d9290", "@Scenes_Find@12")    // CMainView: module by name
extern "C" void FUN_5ef390(); KING_ALIAS("_FUN_5ef390", "@Room_FindObject@12")    // module: switch by name
extern "C" void FUN_5e1a10(); KING_ALIAS("_FUN_5e1a10", "_Switch_Select")    // set a switch
extern "C" char* __cdecl CopyBytes(const char* first, const char* last, char* to);   // aiordr.cpp
extern "C" void FUN_44c220(); KING_ALIAS("_FUN_44c220", "@CharVec_InsertRange44c220@20")    // vector<char>::insert(pos, first, last)
extern "C" void __cdecl Notice_Show(int level, const char* text, int a, int b);              // copter.cpp
extern "C" void __cdecl Notice_Banner(int eventId, double value, const char* text);       // copter.cpp

typedef CharVec* ModuleNames[];
KING_BSS(ModuleNames*, g_aiModuleNames, 0x68d0c8)
KING_BSS(CharVec, g_screenMessage, 0x68d7c0)   // text of the last displayScreenMessage
KING_BSS(double, g_bannerValue, 0x68d7d0)      // shown with the next eventBanner
ASM_VAR(void* const, g_vtblXCycleTask, $L_64bdf8)

// The task that runs the AI's cycle, vtable [$L_64bdf8].
struct XCycleTask : Task {
    void Run();
};

void XCycleTask::Run()
{
    CallThis(FUN_42d6e0, this);
    FUN_5d32f0_(1);
}

void AI_StartXCycleTask()
{
    XCycleTask* task = (XCycleTask*)operator new(sizeof(XCycleTask));
    if (task) {
        task->Construct("doXCycle");
        task->vtbl = (void**)&g_vtblXCycleTask;
    }
    g_pWnd6cec6c->m_1356->m_objects.Add(task);
    task->v_24(1);
}

// the game clock in seconds, 0 without a game screen
double AI_Time()
{
    if (g_pWnd6cec6c && g_pWnd6cec6c->m_1356)
        return g_pWnd6cec6c->m_1356->m_2968;
    return 0.0;
}

void* AI_483a50()
{
    return (BYTE*)g_pWnd6cec6c->m_1356 + 8336;
}

// Sets a switch of a module of the game screen.
void updateSwitch(const char* moduleName, const char* switchName, int v)
{
    StateTrace trace("updateSwitch(char const * moduleName, char const * switchName, int v)");
    if (!moduleName || !switchName || !strlen(moduleName) || !strlen(switchName))
        return;
    void* module = CallThis<void*>(FUN_5d9290, g_pMainView, moduleName);
    if (!module)
        return;
    void* sw = CallThis<void*>(FUN_5ef390, module, switchName);
    if (sw)
        CallC(FUN_5e1a10, sw, v);
}

void AI_UpdateSwitch(const AiSwitch* s)
{
    if (s->module == -1)
        return;
    const CharVec* module = (*g_aiModuleNames)[s->module];
    updateSwitch(module->begin != module->end ? module->begin : NULL, s->name, s->value != 0);
}

void displayScreenMessage(const char* message)
{
    StateTrace trace("displayScreenMessage(char const * message)");
    // g_screenMessage.erase(begin(), end()); the copy's fourth argument is
    // an empty iterator tag, passed with whatever ECX held
    char* begin = g_screenMessage.begin;
    char* end = g_screenMessage.end;
    CopyBytes(end, end, begin);
    g_screenMessage.end += begin - end;
    // g_screenMessage.insert(end(), message, message + strlen(message) + 1)
    CallThis(FUN_44c220, &g_screenMessage, g_screenMessage.end, message, message + strlen(message) + 1);
    Notice_Show(9, g_screenMessage.begin, 0, 0);
}

void AI_AddToBanner(double value)
{
    g_bannerValue += value;
}

void eventBanner(int xEventId, char* message)
{
    StateTrace trace("eventBanner(int xEventId, char * message)");
    Notice_Banner(xEventId, g_bannerValue, message);
    g_bannerValue = 0;
}

// ---------------------------------------------------------------------------
// Entry points for king.masm. The cdecl functions declared in ai.h replace
// their labels themselves: AI_StartXCycleTask [FUN_483990], AI_Time
// [FUN_483a20], AI_483a50 [FUN_483a50], updateSwitch [FUN_483a70],
// AI_UpdateSwitch [FUN_483c10], displayScreenMessage [FUN_483cb0],
// AI_AddToBanner [FUN_483e60], eventBanner [FUN_483e80].

extern "C" void __fastcall XCycleTask_Run(XCycleTask* self, void*)  // [$L_483970]
{
    self->Run();
}

// Game-owned static state of AI_TASK.CPP. Unsigned subtraction preserves
// the original generator's modulo-2^32 recurrence without signed overflow.
typedef DWORD AiShuffleState[55];
KING_BSS(AiShuffleState, g_aiShuffleState, 0x68d6d8)
KING_BSS(DWORD, g_aiShuffleIndex, 0x68d7b4)
KING_BSS(DWORD, g_aiShuffleOtherIndex, 0x68d7b8)
extern "C" void FUN_404420(); KING_ALIAS("_FUN_404420", "@VecB_FreeStorage@8") // shared vector<char> destructor
extern "C" int __cdecl atexit(void (__cdecl* callback)());

extern "C" void __cdecl AI_ShuffleState_Init()
{
    for (unsigned i = 0; i < 55; ++i)
        g_aiShuffleState[i] = 0;
    DWORD seed = 161803398, current = 1;
    g_aiShuffleState[54] = seed;
    for (unsigned offset = 0; offset < 1134; offset += 21) {
        unsigned slot = offset % 55;
        seed -= current;
        g_aiShuffleState[slot] = current;
        current = seed;
        seed = g_aiShuffleState[slot];
    }
    for (unsigned pass = 0; pass < 4; ++pass)
        for (unsigned i = 0; i < 55; ++i)
            g_aiShuffleState[i] -= g_aiShuffleState[(i + 31) % 55];
    g_aiShuffleIndex = 0;
    g_aiShuffleOtherIndex = 31;
}

extern "C" CharVec* __cdecl AI_MessageBuffer_Construct()
{
    g_screenMessage.begin = 0;
    g_screenMessage.end = 0;
    g_screenMessage.capacityEnd = 0;
    return &g_screenMessage;
}

extern "C" void __cdecl AI_MessageBuffer_Destroy()
{
    CallThis(FUN_404420, &g_screenMessage);
}

extern "C" int __cdecl AI_MessageBuffer_RegisterDestructor()
{
    return atexit(AI_MessageBuffer_Destroy);
}

extern "C" int __cdecl AI_MessageBuffer_Init()
{
    AI_MessageBuffer_Construct();
    return AI_MessageBuffer_RegisterDestructor();
}
