#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" DWORD g_marineStartDelay=0;
#define ONE(n) extern "C" DWORD __fastcall MarineTask_##n(BYTE*,void*,DWORD); extern "C" DWORD __fastcall Reference_##n(BYTE*,void*,DWORD);
#define ZERO(n) extern "C" DWORD __fastcall MarineTask_##n(BYTE*,void*); extern "C" DWORD __fastcall Reference_##n(BYTE*,void*);
#define TWO(n) extern "C" DWORD __fastcall MarineTask_##n(BYTE*,void*,DWORD,DWORD); extern "C" DWORD __fastcall Reference_##n(BYTE*,void*,DWORD,DWORD);
ONE(SetMode) ZERO(GetMode) ONE(Activate) ONE(Deactivate) ONE(SetPendingValue) ZERO(ClearRegistration) TWO(SetFirstBounds) TWO(SetSecondBounds)
static BYTE state[6][3200];
static volatile BYTE expected[6][3200];
static void Setup(unsigned test)
{
    for(unsigned i=0;i<6;i++) for(unsigned j=0;j<3200;j++) state[i][j]=(BYTE)(i+j+test);
    F(void*,state[0],0)=state[3]; F(void*,state[3],4)=state[4];
    F(void*,state[0],112)=state[1]; F(void*,state[0],16)=state[2];
    F(DWORD,state[0],116)=test%4;
    const int loads[]={-1,0,5,6,0x7fffffff};
    F(int,state[1],2992)=loads[test%5];
    g_marineStartDelay=0x3f800000+test;
}
static DWORD Run(unsigned method,unsigned test,bool reference)
{
    DWORD a=(test%3)?0:0xffffffff, b=0xfedcba98+test;
    switch(method) {
    case 0: return reference?Reference_SetMode(state[0],0,a):MarineTask_SetMode(state[0],0,a);
    case 1: return reference?Reference_GetMode(state[0],0):MarineTask_GetMode(state[0],0);
    case 2: return reference?Reference_Activate(state[0],0,a):MarineTask_Activate(state[0],0,a);
    case 3: return reference?Reference_Deactivate(state[0],0,a):MarineTask_Deactivate(state[0],0,a);
    case 4: return reference?Reference_SetPendingValue(state[0],0,b):MarineTask_SetPendingValue(state[0],0,b);
    case 5: return reference?Reference_ClearRegistration(state[0],0):MarineTask_ClearRegistration(state[0],0);
    case 6: return reference?Reference_SetFirstBounds(state[0],0,a,b):MarineTask_SetFirstBounds(state[0],0,a,b);
    default: return reference?Reference_SetSecondBounds(state[0],0,a,b):MarineTask_SetSecondBounds(state[0],0,a,b);
    }
}
extern "C" void TestEntry()
{
    for(unsigned method=0;method<8;method++) for(unsigned test=0;test<128;test++) {
        Setup(test); DWORD reference=Run(method,test,true);
        for(unsigned i=0;i<6;i++) for(unsigned j=0;j<3200;j++) expected[i][j]=state[i][j];
        Setup(test); DWORD actual=Run(method,test,false);
        if(reference!=actual) ExitProcess(1+method);
        for(unsigned i=0;i<6;i++) for(unsigned j=0;j<3200;j++) if(expected[i][j]!=state[i][j]) ExitProcess(20+method);
    }
    ExitProcess(0);
}
