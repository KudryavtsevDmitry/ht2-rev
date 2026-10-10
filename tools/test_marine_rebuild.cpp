#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
DWORD __fastcall MarineQueue_RebuildNodes(BYTE*,void*);
DWORD __fastcall MarineQueue_RebuildEdges(BYTE*,void*);
DWORD __fastcall Reference_RebuildNodes(BYTE*,void*);
DWORD __fastcall Reference_RebuildEdges(BYTE*,void*);
int _fltused=0;
}
static BYTE state[10][80], expected[10][80], list[12], expectedList[12];
static void Setup(unsigned a,unsigned b,unsigned edge,unsigned pattern)
{
    for(unsigned i=0;i<10;i++) for(unsigned j=0;j<80;j++) state[i][j]=(BYTE)(i*80+j);
    unsigned base=edge?0:4, scoreOffset=edge?72:28;
    for(unsigned side=0;side<2;side++) {
        unsigned length=side?b:a, start=side?5:0;
        for(unsigned i=0;i<length;i++) {
            BYTE* node=state[start+i];
            F(void*,node,base)=state[start+(i+1)%length]+base;
            F(void*,node,base+4)=state[start+(i+length-1)%length]+base;
            int rank=(int)(edge?i:length-i);
            F(float,node,scoreOffset)=(float)(rank*(pattern%3+1)+(side?(int)(pattern%7)-3:0));
            if(pattern==7) F(float,node,scoreOffset)=1.0f;
            if(pattern==8) F(DWORD,node,scoreOffset)=0;
            if(pattern==9) F(DWORD,node,scoreOffset)=0x80000000;
            if(pattern>=10 && i==(pattern-10)%5) F(DWORD,node,scoreOffset)=side?0x7fc01234:0x7f801234;
            if(pattern==16) F(DWORD,node,scoreOffset)=side?0xff800000:0x7f800000;
            if(pattern==17) F(DWORD,node,scoreOffset)=0x7fc01234;
        }
    }
    F(void*,list,0)=a?state[0]:0; F(void*,list,4)=b?state[5]:0;
    F(DWORD,list,8)=0xdeadbeef;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    for(unsigned edge=0;edge<2;edge++) for(unsigned a=0;a<6;a++) for(unsigned b=0;b<6;b++) for(unsigned pattern=0;pattern<18;pattern++) {
        Setup(a,b,edge,pattern);
        DWORD reference=edge?Reference_RebuildEdges(list,0):Reference_RebuildNodes(list,0);
        for(unsigned i=0;i<10;i++) for(unsigned j=0;j<80;j++) expected[i][j]=state[i][j];
        for(unsigned i=0;i<12;i++) expectedList[i]=list[i];
        Setup(a,b,edge,pattern);
        DWORD actual=edge?MarineQueue_RebuildEdges(list,0):MarineQueue_RebuildNodes(list,0);
        for(unsigned i=0;i<10;i++) for(unsigned j=0;j<80;j++) if(expected[i][j]!=state[i][j]) ExitProcess(1+edge);
        for(unsigned i=0;i<12;i++) if(expectedList[i]!=list[i]) ExitProcess(3+edge);
        if(a && b && reference!=actual) ExitProcess(5+edge);
    }
    ExitProcess(0);
}
