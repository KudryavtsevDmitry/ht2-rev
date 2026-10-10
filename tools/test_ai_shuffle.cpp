#include "../src/king.h"
extern "C" {
DWORD g_aiShuffleState[55], g_aiShuffleIndex, g_aiShuffleOtherIndex;
void __cdecl AI_ShuffleState_Init();
void __cdecl Reference_Shuffle_Init();
}
extern "C" void* __cdecl memset(void*, int, size_t);
#pragma function(memset)
extern "C" void* __cdecl memset(void* dst, int value, size_t size)
{
    volatile BYTE* bytes = (BYTE*)dst;
    for (size_t i = 0; i < size; ++i) bytes[i] = (BYTE)value;
    return dst;
}
extern "C" void TestEntry()
{
    DWORD expected[55];
    Reference_Shuffle_Init();
    for (unsigned i = 0; i < 55; ++i) expected[i] = g_aiShuffleState[i];
    for (unsigned run = 0; run < 3; ++run) {
        for (unsigned i = 0; i < 55; ++i) g_aiShuffleState[i] = 0xdeadbeef + i + run;
        g_aiShuffleIndex = g_aiShuffleOtherIndex = 0xdeadbeef;
        AI_ShuffleState_Init();
        if (g_aiShuffleIndex != 0 || g_aiShuffleOtherIndex != 31) ExitProcess(1);
        for (unsigned i = 0; i < 55; ++i)
            if (g_aiShuffleState[i] != expected[i]) ExitProcess(2);
    }
    ExitProcess(0);
}
