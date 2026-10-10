#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
char g_marineDate[1], g_marineErrorFormat[1], g_marineFile[1], g_marineAssertFormat[1];
char g_marineHeader[1], g_marineNeighbor44[1], g_marineNeighbor52[1], g_marineNeighbor48[1];
void __fastcall MarineGreed_RemoveNode(BYTE*, void*, BYTE*);
void __fastcall Reference_RemoveNode(BYTE*, void*, BYTE*);
}
static BYTE state[24][64], expected[24][64];
static DWORD trace[64], expectedTrace[64];
static unsigned count;
static void Record(DWORD tag, const void* pointer) { trace[count++] = tag; trace[count++] = (DWORD)pointer; }
extern "C" void __fastcall QueueRemove(BYTE* self, void*, BYTE* node) { Record(1,self); Record(2,node); }
extern "C" void __fastcall EdgeRemove(BYTE* self, void*, BYTE* edge) { Record(3,self); Record(4,edge); }
extern "C" void __fastcall EdgeDestroy(BYTE* self, void*) { Record(5,self); }
extern "C" void __fastcall GroupDestroy(BYTE* self, void*) { Record(6,self); }
extern "C" void __fastcall NodeDelete(BYTE* self, void*, int flag) { Record(7,self); Record(8,(void*)flag); }
extern "C" void TestFree(void* self) { Record(9,self); }
extern "C" void TestError(unsigned code, const char*, const char*, int line, const char*) { Record(code,(void*)line); }
extern "C" void TestAssert(int type, const char*, const char* expression, const char*, int line, const char*) { Record(type,expression); Record(10,(void*)line); }
KING_ALIAS("FUN_60f690","@QueueRemove@12")
KING_ALIAS("FUN_6136f0","@EdgeRemove@12")
KING_ALIAS("FUN_606750","@EdgeDestroy@8")
KING_ALIAS("FUN_6069d0","@GroupDestroy@8")
KING_ALIAS("FUN_60aad0","_TestError")
KING_ALIAS("FUN_60aa60","_TestAssert")
static void* vtable[] = { (void*)NodeDelete };
static void Setup(unsigned test)
{
    for (unsigned i=0;i<24;i++) for(unsigned j=0;j<64;j++) state[i][j]=0;
    for(unsigned i=0;i<64;i++) trace[i]=0;
    count=0;
    BYTE* self=state[0]; BYTE* node=state[1]; BYTE* group=state[2]; BYTE* edge=state[3];
    F(void*,node,0)=vtable; F(void*,node,32)=group;
    F(DWORD,group,8)=(test&1)?0:1;
    F(void*,self,32)=state[4]; F(void*,self,28)=state[5];
    F(WORD,self,38)=7; F(WORD,self,52)=9;
    F(void*,self,40)=node; F(void*,self,44)=edge;
    F(void*,state[4],0)=state[6]; F(void*,state[4],4)=state[7];
    F(void*,state[5],0)=state[8]; F(void*,state[5],4)=state[9];
    unsigned sentinel=(test/2)%3;
    F(void*,node,4)=state[10]+4; F(void*,edge,0)=state[11];
    if(sentinel==0) { F(void*,state[6],8)=node+4; F(void*,state[8],4)=edge; }
    if(sentinel==1) { F(void*,state[7],8)=node+4; F(void*,state[9],4)=edge; }
    unsigned neighbors=(test/6)%4;
    const unsigned slots[]={44,52,48};
    const unsigned preferred[]={44,48,52};
    const unsigned alternate[]={48,44,44};
    const unsigned finalSlot[]={52,44,44};
    for(unsigned i=0;i<3;i++) {
        if(neighbors==0) continue;
        F(void*,node,slots[i])=state[12+i];
        unsigned slot=neighbors==1?preferred[i]:neighbors==2?alternate[i]:finalSlot[i];
        if(!(test&128)) F(void*,state[12+i],slot)=node;
    }
    if(test&32) {
        F(void*,node,56)=edge;
        F(void*,edge,8)=state[15];
        if(test&64) F(void*,edge,12)=state[16];
        for(unsigned i=0;i<4;i++) {
            F(void*,state[15+i/2],(i%2)*4)=state[17+i];
            F(void*,state[17+i],12)=state[21+i%3];
            F(void*,state[21+i%3],56)=edge;
        }
    }
    if(test&16) { F(void*,self,40)=state[10]; F(void*,self,44)=state[11]; }
}
extern "C" void* __cdecl memset(void*,int,size_t);
#pragma function(memset)
extern "C" void* __cdecl memset(void* p,int v,size_t n) { volatile BYTE* b=(BYTE*)p; for(size_t i=0;i<n;i++) b[i]=(BYTE)v; return p; }
extern "C" void TestEntry()
{
    for(unsigned test=0;test<384;test++) {
        Setup(test); Reference_RemoveNode(state[0],0,state[1]);
        unsigned referenceCount=count;
        for(unsigned i=0;i<24;i++) for(unsigned j=0;j<64;j++) expected[i][j]=state[i][j];
        for(unsigned i=0;i<64;i++) expectedTrace[i]=trace[i];
        Setup(test); MarineGreed_RemoveNode(state[0],0,state[1]);
        if(count!=referenceCount) ExitProcess(1000+test);
        for(unsigned i=0;i<24;i++) for(unsigned j=0;j<64;j++) if(expected[i][j]!=state[i][j]) ExitProcess(2000+test);
        for(unsigned i=0;i<64;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(3000+test);
    }
    ExitProcess(0);
}
