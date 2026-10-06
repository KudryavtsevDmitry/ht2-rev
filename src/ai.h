// ai.h - the AI (C:\Nek\Vrappl\Gi\Htai\): declarations shared by the C++
// code of its units.
#pragma once

#include "king.h"
#include "x87.h"

// The AI's crash trace: a function writes "\r\nSTART:" and its signature
// into state.dump when it is entered, "\r\nCOMPL:" and the signature when it
// returns. Most units call the guard out of line ([FUN_401870],
// [FUN_4018b0]); AI_TASK.CPP has it inline, as written here.
class StateTrace {
public:
    explicit StateTrace(const char* signature) : m_signature(signature), m_dump(&g_stateDump)
    {
        Write("\r\nSTART:");
    }
    ~StateTrace() { Write("\r\nCOMPL:"); }

private:
    void Write(const char* prefix)      // 8 characters
    {
        if (g_stateDump.data && g_stateDump.pos + 8 < g_stateDump.size) {
            char* p = g_stateDump.data + g_stateDump.pos;
            ((DWORD*)p)[0] = ((const DWORD*)prefix)[0];
            ((DWORD*)p)[1] = ((const DWORD*)prefix)[1];
            g_stateDump.pos += 8;
        }
        if (g_stateDump.data && *m_signature) {
            const char* s = m_signature;
            do {
                if (g_stateDump.pos >= g_stateDump.size)
                    break;
                g_stateDump.data[g_stateDump.pos++] = *s++;
            } while (*s);
        }
    }

    const char* m_signature;
    StateDump*  m_dump;
};

// vector<char> of the SGI-style STL the AI uses
struct CharVec {
    char* begin;
    char* end;
    char* capacityEnd;
};

// A scheduled object of the engine, kept in an ObjectContainer (32 bytes).
ASM_PROC(FUN_5d3110)    // Task::Task(const char* name)
ASM_PROC(FUN_5d32f0)
struct Task {
    void** vtbl;
    BYTE   m_4[28];

    void Construct(const char* name) { CallThis(FUN_5d3110, this, name); }
    void FUN_5d32f0_(int a) { CallThis(FUN_5d32f0, this, a); }
    void v_24(int a) { CallVirt(this, 24, a); }
};

// AI_TASK.CPP (aitask.cpp)
struct AiSwitch {
    int         module;         // index into g_aiModuleNames, -1: none
    const char* name;           // +4
    BYTE        m_8[8];
    bool        value;          // +16
};

extern "C" {
double __cdecl AI_Time();
void* __cdecl AI_483a50();
void __cdecl updateSwitch(const char* moduleName, const char* switchName, int v);
void __cdecl AI_UpdateSwitch(const AiSwitch* s);
void __cdecl displayScreenMessage(const char* message);
void __cdecl AI_AddToBanner(double value);
void __cdecl eventBanner(int xEventId, char* message);
void __cdecl AI_StartXCycleTask();
}
