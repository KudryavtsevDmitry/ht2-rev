#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
BYTE* __fastcall MarineQueue_TopNode(BYTE*,void*);
BYTE* __fastcall MarineQueue_TopEdge(BYTE*,void*);
BYTE* __fastcall Reference_TopNode(BYTE*,void*);
BYTE* __fastcall Reference_TopEdge(BYTE*,void*);
DWORD __fastcall MarineQueue_SetNodePriority(BYTE*,void*,BYTE*,DWORD);
DWORD __fastcall MarineQueue_SetEdgePriority(BYTE*,void*,BYTE*,DWORD);
DWORD __fastcall Reference_SetNodePriority(BYTE*,void*,BYTE*,DWORD);
DWORD __fastcall Reference_SetEdgePriority(BYTE*,void*,BYTE*,DWORD);
int _fltused=0;
}
static BYTE list[12], first[80], last[80], expected[80];
static DWORD trace[8], expectedTrace[8];
static unsigned count;
extern "C" void __fastcall Remove(BYTE* self,void*,BYTE* item)
{
    trace[count++]=(DWORD)self; trace[count++]=(DWORD)item;
    trace[count++]=F(DWORD,item,28); trace[count++]=F(DWORD,item,72);
    --F(DWORD,self,8);
}
extern "C" void __fastcall Insert(BYTE* self,void*,BYTE* item)
{
    trace[count++]=(DWORD)self; trace[count++]=(DWORD)item;
    trace[count++]=F(DWORD,item,28); trace[count++]=F(DWORD,item,72);
}
KING_ALIAS("FUN_60f690","@Remove@12")
KING_ALIAS("FUN_6136f0","@Remove@12")
KING_ALIAS("FUN_60f520","@Insert@12")
KING_ALIAS("FUN_60f900","@Insert@12")
static const DWORD scores[]={0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc01234,0x7f801234};
static void Setup(DWORD seed)
{
    count=0;
    for(unsigned i=0;i<80;i++) first[i]=(BYTE)(i+seed);
    F(DWORD,list,8)=seed;
}
extern "C" void TestEntry()
{
    for(unsigned a=0;a<8;a++) for(unsigned b=0;b<8;b++) for(unsigned pointers=0;pointers<4;pointers++) {
        F(void*,list,0)=(pointers&1)?first:0; F(void*,list,4)=(pointers&2)?last:0;
        F(DWORD,first,28)=F(DWORD,first,72)=scores[a];
        F(DWORD,last,28)=F(DWORD,last,72)=scores[b];
        if(MarineQueue_TopNode(list,0)!=Reference_TopNode(list,0)) ExitProcess(1);
        if(MarineQueue_TopEdge(list,0)!=Reference_TopEdge(list,0)) ExitProcess(2);
    }
    const DWORD seeds[]={0,1,0xffffffff};
    for(unsigned seed=0;seed<3;seed++) for(unsigned value=0;value<8;value++) for(unsigned edge=0;edge<2;edge++) {
        Setup(seeds[seed]);
        DWORD result=edge?Reference_SetEdgePriority(list,0,first,scores[value]):Reference_SetNodePriority(list,0,first,scores[value]);
        DWORD expectedCount=F(DWORD,list,8);
        for(unsigned i=0;i<80;i++) expected[i]=first[i];
        for(unsigned i=0;i<8;i++) expectedTrace[i]=trace[i];
        Setup(seeds[seed]);
        DWORD actual=edge?MarineQueue_SetEdgePriority(list,0,first,scores[value]):MarineQueue_SetNodePriority(list,0,first,scores[value]);
        if(actual!=result || F(DWORD,list,8)!=expectedCount || count!=8) ExitProcess(3);
        for(unsigned i=0;i<80;i++) if(expected[i]!=first[i]) ExitProcess(4);
        for(unsigned i=0;i<8;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(5);
    }
    ExitProcess(0);
}
