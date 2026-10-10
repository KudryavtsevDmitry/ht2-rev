#include "../src/king.h"
#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))
extern "C" void* __cdecl memset(void*, int, size_t);
#pragma function(memset)
extern "C" void* __cdecl memset(void* dst, int value, size_t size)
{
    volatile BYTE* bytes = (BYTE*)dst;
    for (size_t i = 0; i < size; ++i) bytes[i] = (BYTE)value;
    return dst;
}
extern "C" BYTE** __stdcall ValueList_Erase(BYTE**, BYTE*);
extern "C" BYTE** __fastcall SchInputList_EraseRange(void*, void*, BYTE**, BYTE*, BYTE*);
extern "C" BYTE** __fastcall Reference_EraseOne(void*, void*, BYTE**, BYTE*);
extern "C" BYTE** __fastcall Reference_EraseRange(void*, void*, BYTE**, BYTE*, BYTE*);
static BYTE nodes[5][20], objects[2][16], snapshot[sizeof(nodes) + sizeof(objects)];
static unsigned freed, destroyed;
static BYTE* returned;
extern "C" void __cdecl TestFree(void* node, int size)
{
    if (size != 20) ExitProcess(3);
    ++freed;
    volatile BYTE* bytes = (BYTE*)node;
    for (int i = 0; i < size; ++i) bytes[i] = 0xdd;
}
extern "C" void __fastcall TestDelete(BYTE* self, void*, int flags)
{
    if (flags != 1 || F(int, self, 4) != 0) ExitProcess(4);
    ++destroyed;
    F(int, self, 8) = 1;
}
static void* vtable[4] = { 0, 0, 0, (void*)TestDelete };
static void Reset()
{
    freed = destroyed = 0;
    for (unsigned i = 0; i < sizeof(nodes); ++i) ((BYTE*)nodes)[i] = 0;
    for (unsigned i = 0; i < sizeof(objects); ++i) ((BYTE*)objects)[i] = 0;
    for (unsigned i = 0; i < 5; ++i) {
        F(BYTE*, nodes[i], 0) = nodes[(i + 1) % 5];
        F(BYTE*, nodes[i], 4) = nodes[(i + 4) % 5];
    }
    F(void*, objects[0], 0) = F(void*, objects[1], 0) = vtable;
    F(int, objects[0], 4) = 2;
    F(int, objects[1], 4) = 1;
    F(BYTE*, nodes[1], 8) = F(BYTE*, nodes[2], 8) = objects[0];
    F(BYTE*, nodes[3], 8) = objects[1];
    F(DWORD, nodes[4], 8) = 1; // Immediate values must never be dereferenced.
    F(DWORD, nodes[4], 16) = 0x10000;
}
static void Run(unsigned mode, bool reference)
{
    BYTE** result;
    if (mode == 1)
        result = reference ? Reference_EraseOne(0, 0, &returned, nodes[2])
                           : ValueList_Erase(&returned, nodes[2]);
    else {
        BYTE* first = nodes[mode == 3 ? 4 : 1];
        BYTE* last = nodes[mode == 3 ? 0 : mode == 2 ? 4 : 1];
        result = reference ? Reference_EraseRange(0, 0, &returned, first, last)
                           : SchInputList_EraseRange(0, 0, &returned, first, last);
    }
    if (result != &returned) ExitProcess(5);
}
extern "C" void TestEntry()
{
    for (unsigned mode = 0; mode < 4; ++mode) {
        Reset();
        Run(mode, false);
        unsigned expectedFree = freed, expectedDestroyed = destroyed;
        BYTE* expectedReturn = returned;
        for (unsigned i = 0; i < sizeof(nodes); ++i) snapshot[i] = ((BYTE*)nodes)[i];
        for (unsigned i = 0; i < sizeof(objects); ++i) snapshot[sizeof(nodes) + i] = ((BYTE*)objects)[i];
        Reset();
        Run(mode, true);
        if (freed != expectedFree || destroyed != expectedDestroyed || returned != expectedReturn) ExitProcess(1);
        for (unsigned i = 0; i < sizeof(nodes); ++i)
            if (((BYTE*)nodes)[i] != snapshot[i]) ExitProcess(2);
        for (unsigned i = 0; i < sizeof(objects); ++i)
            if (((BYTE*)objects)[i] != snapshot[sizeof(nodes) + i]) ExitProcess(2);
    }
    ExitProcess(0);
}
