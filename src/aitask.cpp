// aitask.cpp - AI_TASK.CPP of the AI, rewritten from king.masm: how the AI
// reaches the game: its cycle task, the switches of the game screen, screen
// messages and banners. The unit's static initializers (an STL random
// generator, the message buffer) are compiler-generated and stay assembly.
//
// Building with  nmake ASM_AITASK=1  makes king.masm assemble the original
// procedures again.

#include "ai.h"

ASM_PROC(FUN_42d6e0)    // the AI's cycle
ASM_PROC(FUN_5d9290)    // CMainView: module by name
ASM_PROC(FUN_5ef390)    // module: switch by name
ASM_PROC(FUN_5e1a10)    // set a switch
ASM_PROC(FUN_46ecd0)    // STL copy(first, last, result) for char*
ASM_PROC(FUN_44c220)    // vector<char>::insert(pos, first, last)
ASM_PROC(FUN_56ae90)    // King: screen message
ASM_PROC(FUN_56aed0)    // King: event banner

typedef CharVec* ModuleNames[];
ASM_VAR(ModuleNames*, g_aiModuleNames, $L_68d0c8)
ASM_VAR(CharVec, g_screenMessage, $L_68d7c0)   // text of the last displayScreenMessage
ASM_VAR(double, g_bannerValue, $L_68d7d0)      // shown with the next eventBanner
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
    CallC(FUN_46ecd0, end, end, begin, message, 0);
    g_screenMessage.end += begin - end;
    // g_screenMessage.insert(end(), message, message + strlen(message) + 1)
    CallThis(FUN_44c220, &g_screenMessage, g_screenMessage.end, message, message + strlen(message) + 1);
    CallC(FUN_56ae90, 9, g_screenMessage.begin, 0, 0);
}

void AI_AddToBanner(double value)
{
    g_bannerValue += value;
}

void eventBanner(int xEventId, char* message)
{
    StateTrace trace("eventBanner(int xEventId, char * message)");
    CallC(FUN_56aed0, xEventId, g_bannerValue, message);
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
