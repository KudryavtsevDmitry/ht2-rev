// test_deque_abi.ps1 extracts the allocator from scheval.cpp and links it
// here, exercising the replacement with the assembly caller's arguments.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" void __stdcall Deque_AllocPages(int** first, int** last);
extern "C" void* __cdecl malloc(unsigned size)
{
    return HeapAlloc(GetProcessHeap(), 0, size);
}
extern "C" void __cdecl FUN_4581a0() { ExitProcess(2); }

extern "C" void __cdecl TestEntry()
{
    for (int count = 0; count <= 3; ++count) {
        int* pages[4] = {0, 0, 0, (int*)0x12345678};
        int** first = pages;
        int** last = pages + count;
        unsigned before, after;
        __asm {
            mov before, esp
            push last
            push first
            call Deque_AllocPages
            mov after, esp
            mov esp, before
        }
        if (before != after) ExitProcess(3);
        for (int i = 0; i < count; ++i) {
            if (!pages[i]) ExitProcess(4);
            pages[i][0] = i;
            pages[i][127] = i + 1; // Each page holds 512 bytes.
            if (!HeapFree(GetProcessHeap(), 0, pages[i])) ExitProcess(5);
        }
        if (pages[3] != (int*)0x12345678) ExitProcess(6);
        if (count < 3 && pages[count]) ExitProcess(7);
    }
    ExitProcess(0);
}
