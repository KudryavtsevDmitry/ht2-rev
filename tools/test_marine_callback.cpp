#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
#define TWO(n) extern "C" DWORD __fastcall MarineTask_##n(BYTE*,void*,DWORD,const DWORD*); extern "C" DWORD __fastcall Reference_##n(BYTE*,void*,DWORD,const DWORD*);
#define BOUNDS(n) extern "C" DWORD __fastcall MarineTask_##n(BYTE*,void*,const DWORD*,const DWORD*); extern "C" DWORD __fastcall Reference_##n(BYTE*,void*,const DWORD*,const DWORD*);
#define ONE(n) extern "C" DWORD __fastcall MarineTask_##n(BYTE*,void*,const DWORD*); extern "C" DWORD __fastcall Reference_##n(BYTE*,void*,const DWORD*);
TWO(InitPoints) TWO(AllocatePoints) TWO(SetPoints) TWO(RequestPoints)
BOUNDS(SetBounds) BOUNDS(RebuildBounds) ONE(ApplyPoint) ONE(ForwardPoint) ONE(ForwardPointCopy)
extern "C" void __fastcall MarineTask_ReleaseMesh(BYTE*,void*);
extern "C" void __fastcall Reference_ReleaseMesh(BYTE*,void*);
static BYTE state[7][256], pool[56];
static volatile BYTE expected[7][256], expectedPool[56];
static DWORD trace[128], expectedTrace[128], values[24];
static unsigned count, variant;
static void Record(DWORD value) { trace[count++]=value; }
extern "C" int _fltused=0;
void* operator new(size_t size) { Record(1); Record(size); return (variant&8)?0:pool; }
extern "C" void __fastcall TestUpdate(BYTE* mesh,void*) { Record(2); Record((DWORD)mesh); if(variant&2) F(void*,state[1],0)=state[5]; }
extern "C" DWORD __fastcall TestPrune(BYTE* mesh,void*) { Record(3); Record((DWORD)mesh); return 0xabcdef01; }
KING_ALIAS("FUN_60d780","@TestPrune@8")
KING_ALIAS("@MarineGreed_Update@8","@TestUpdate@8")
extern "C" void __fastcall Bounds(BYTE* sink,void*,const DWORD* a,const DWORD* b)
{
    Record(4); Record((DWORD)sink);
    for(unsigned i=0;i<3;i++) { Record(a[i]); Record(b[i]); }
    if(variant&1) F(void*,state[1],0)=state[4];
}
extern "C" DWORD __fastcall Point(BYTE* sink,void*,const DWORD* point)
{
    Record(5); Record((DWORD)sink);
    for(unsigned i=0;i<3;i++) Record(point[i]);
    return 0x76543210;
}
extern "C" void __fastcall DeleteMesh(BYTE* mesh,void*,DWORD flag) { Record(6); Record((DWORD)mesh); Record(flag); }
static void* sinkTable[11], *meshTable[1];
static void Setup(unsigned test)
{
    variant=test; count=0;
    for(unsigned i=0;i<128;i++) trace[i]=0;
    for(unsigned i=0;i<7;i++) for(unsigned j=0;j<256;j++) state[i][j]=(BYTE)(i*13+j+test);
    for(unsigned i=0;i<56;i++) pool[i]=(BYTE)(i+test);
    sinkTable[1]=(void*)Bounds; sinkTable[10]=(void*)Point; meshTable[0]=(void*)DeleteMesh;
    F(void*,state[0],108)=state[1]; F(void*,state[1],0)=state[2];
    F(void*,state[3],0)=sinkTable;
    for(unsigned i=2;i<=5;i++) if(i!=3) { F(void*,state[i],0)=meshTable; F(void*,state[i],4)=state[3]; }
    for(unsigned i=0;i<24;i++) values[i]=0x3f800000+i+test;
    const DWORD unusual[]={0x7f801234,0xff800000,0x80000000,0x7fc01234};
    if(test&4) for(unsigned i=0;i<3;i++) values[i]=unusual[(test+i)%4];
}
static DWORD Run(unsigned method,bool reference)
{
    unsigned size=variant%11;
    switch(method) {
    case 0: return reference?Reference_InitPoints(state[6],0,size,values):MarineTask_InitPoints(state[6],0,size,values);
    case 1: return reference?Reference_AllocatePoints(state[3],0,size,values):MarineTask_AllocatePoints(state[3],0,size,values);
    case 2: return reference?Reference_SetPoints(state[1],0,size,values):MarineTask_SetPoints(state[1],0,size,values);
    case 3: return reference?Reference_RequestPoints(state[0],0,size,values):MarineTask_RequestPoints(state[0],0,size,values);
    case 4: return reference?Reference_SetBounds(state[1],0,values,values+3):MarineTask_SetBounds(state[1],0,values,values+3);
    case 5: return reference?Reference_RebuildBounds(state[1],0,values,values+3):MarineTask_RebuildBounds(state[1],0,values,values+3);
    case 6: return reference?Reference_ApplyPoint(state[1],0,values):MarineTask_ApplyPoint(state[1],0,values);
    case 7: return reference?Reference_ForwardPoint(state[0],0,values):MarineTask_ForwardPoint(state[0],0,values);
    case 8: return reference?Reference_ForwardPointCopy(state[0],0,values):MarineTask_ForwardPointCopy(state[0],0,values);
    default:
        if(variant&1) F(void*,state[1],0)=0;
        if(reference) Reference_ReleaseMesh(state[1],0); else MarineTask_ReleaseMesh(state[1],0);
        return 0;
    }
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    for(unsigned method=0;method<10;method++) for(unsigned test=0;test<32;test++) {
        Setup(test); DWORD result=Run(method,true); unsigned expectedCount=count;
        for(unsigned i=0;i<7;i++) for(unsigned j=0;j<256;j++) expected[i][j]=state[i][j];
        for(unsigned i=0;i<56;i++) expectedPool[i]=pool[i];
        for(unsigned i=0;i<128;i++) expectedTrace[i]=trace[i];
        Setup(test); DWORD actual=Run(method,false);
        if(result!=actual || expectedCount!=count) ExitProcess(1+method);
        for(unsigned i=0;i<7;i++) for(unsigned j=0;j<256;j++) if(expected[i][j]!=state[i][j]) ExitProcess(20+method);
        for(unsigned i=0;i<56;i++) if(expectedPool[i]!=pool[i]) ExitProcess(40+method);
        for(unsigned i=0;i<128;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(60+method);
    }
    ExitProcess(0);
}
