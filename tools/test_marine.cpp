#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
char g_marineDate[1], g_marineErrorFormat[1], g_marineFile[1], g_marineAssertFormat[1];
char g_marineHeader[1], g_marineNeighbor44[1], g_marineNeighbor52[1], g_marineNeighbor48[1];
void __fastcall MarineGreed_RemoveNode(BYTE*, void*, BYTE*);
void __fastcall Reference_RemoveNode(BYTE*, void*, BYTE*);
int __fastcall MarineGreed_RefreshPriorities(BYTE*,void*);
int __fastcall Reference_RefreshPriorities(BYTE*,void*);
int __fastcall MarineGreed_Refine(BYTE*,void*);
int __fastcall Reference_Refine(BYTE*,void*);
int __fastcall MarineGreed_Emit(BYTE*,void*);
int __fastcall Reference_Emit(BYTE*,void*);
DWORD g_marineRefinementEnabled, g_marineRefinementLimit;

}
static BYTE state[24][96], expected[24][96];
static DWORD trace[256], expectedTrace[256];
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
static unsigned tick, desired;
extern "C" unsigned __stdcall TestTime() { return tick++; }
extern "C" void* TestTimePointer=(void*)TestTime;
extern "C" void __fastcall SetPriority(BYTE* list,void*,BYTE* item,float score) { Record(11,list); Record(12,item); Record(13,(void*)F(DWORD,&score,0)); }
extern "C" BYTE* __fastcall TopNode(BYTE* list,void*) { Record(14,list); return state[1]; }
extern "C" BYTE* __fastcall TopEdge(BYTE* list,void*) { Record(15,list); return state[3]; }
extern "C" int __fastcall EmptyEdges(BYTE* list,void*) { Record(16,list); return !g_marineRefinementEnabled; }
extern "C" void SplitEdge(BYTE* edge,BYTE* self) { Record(17,edge); Record(18,self); }
extern "C" void MergeGroup(BYTE* group,BYTE* self) { Record(19,group); Record(20,self); ++F(WORD,self,38); }
extern "C" unsigned __fastcall Desired(BYTE*,void*) { Record(21,(void*)desired); return desired; }
extern "C" double __fastcall NodeScore(BYTE*,void*,DWORD a,DWORD b,DWORD c,BYTE* point) { Record(a,point); Record(b,(void*)c); return 1.25; }
extern "C" double __fastcall EdgeScore(BYTE*,void*,DWORD a,DWORD b,DWORD c,BYTE* p,DWORD d,DWORD e,DWORD f,BYTE* q) { Record(a,p); Record(b,(void*)c); Record(d,q); Record(e,(void*)f); return 2.5; }
extern "C" void __fastcall Begin(BYTE* sink,void*) { Record(22,sink); }
extern "C" void __fastcall Vertex(BYTE*,void*,DWORD v) { Record(23,(void*)v); }
extern "C" void __fastcall Triangle(BYTE*,void*,WORD a,WORD b,WORD c,BYTE* n) { Record(a,n); Record(b,(void*)(unsigned)c); }
KING_ALIAS("FUN_60f4f0","@SetPriority@16")
KING_ALIAS("FUN_60f8d0","@SetPriority@16")
KING_ALIAS("FUN_60f7b0","@TopNode@8")
KING_ALIAS("FUN_60f9d0","@TopEdge@8")
KING_ALIAS("FUN_6137d0","@EmptyEdges@8")
KING_ALIAS("FUN_606760","_SplitEdge")
KING_ALIAS("FUN_605900","_MergeGroup")
static void* sinkTable[15];
static void* sink=sinkTable;
extern "C" int _fltused=0;
static void* vtable[] = { (void*)NodeDelete };
static void Setup(unsigned test)
{
    for (unsigned i=0;i<24;i++) for(unsigned j=0;j<96;j++) state[i][j]=0;
    for(unsigned i=0;i<256;i++) trace[i]=0;
    count=0; tick=0xfffffffe; desired=8;
    g_marineRefinementEnabled=(test%5)!=0; g_marineRefinementLimit=test%3;
    sinkTable[2]=(void*)Desired; sinkTable[7]=(void*)Begin; sinkTable[8]=(void*)EdgeScore;
    sinkTable[9]=(void*)NodeScore; sinkTable[13]=(void*)Vertex; sinkTable[14]=(void*)Triangle;
    BYTE* self=state[0]; BYTE* node=state[1]; BYTE* group=state[2]; BYTE* edge=state[3];
    F(void*,self,4)=&sink;
    F(float,node,28)=(test%4)?1.0f:0.0f; F(float,edge,72)=(test%4)?0.0f:1.0f;
    if(test%7==0) F(DWORD,node,28)=0x7fc00000;
    F(void*,node,16)=state[17]; F(void*,node,20)=state[18]; F(void*,node,24)=state[19];
    for(unsigned i=7;i<=10;i+=3) { F(void*,state[i],16)=state[17]; F(void*,state[i],20)=state[18]; F(void*,state[i],24)=state[19]; }
    F(WORD,state[17],8)=31; F(WORD,state[18],8)=47; F(WORD,state[19],8)=63;
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
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    for(unsigned test=0;test<384;test++) {
        for(unsigned method=0;method<4;method++) {
        Setup(test);
        if(method==3) { F(void*,state[4],0)=state[1]; F(void*,state[4],4)=0; F(WORD,state[0],36)=2; F(void*,state[0],12)=state[23]; F(DWORD,state[23],0)=123; F(DWORD,state[23],4)=456; F(DWORD,state[0],24)=test&1; F(DWORD,state[0],20)=test&2; }
        int referenceResult=1;
        if(method==0) Reference_RemoveNode(state[0],0,state[1]);
        if(method==1) referenceResult=Reference_RefreshPriorities(state[0],0);
        if(method==2) referenceResult=Reference_Refine(state[0],0);
        if(method==3) referenceResult=Reference_Emit(state[0],0);
        unsigned referenceCount=count;
        for(unsigned i=0;i<24;i++) for(unsigned j=0;j<96;j++) expected[i][j]=state[i][j];
        for(unsigned i=0;i<256;i++) expectedTrace[i]=trace[i];
        Setup(test);
        if(method==3) { F(void*,state[4],0)=state[1]; F(void*,state[4],4)=0; F(WORD,state[0],36)=2; F(void*,state[0],12)=state[23]; F(DWORD,state[23],0)=123; F(DWORD,state[23],4)=456; F(DWORD,state[0],24)=test&1; F(DWORD,state[0],20)=test&2; }
        int actualResult=1;
        if(method==0) MarineGreed_RemoveNode(state[0],0,state[1]);
        if(method==1) actualResult=MarineGreed_RefreshPriorities(state[0],0);
        if(method==2) actualResult=MarineGreed_Refine(state[0],0);
        if(method==3) actualResult=MarineGreed_Emit(state[0],0);
        if(actualResult!=referenceResult) ExitProcess(4000+test);
        if(count!=referenceCount) ExitProcess(1000+test);
        for(unsigned i=0;i<24;i++) for(unsigned j=0;j<96;j++) if(expected[i][j]!=state[i][j]) ExitProcess(2000+test);
        for(unsigned i=0;i<256;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(3000+test);
    }
    }
    ExitProcess(0);
}
