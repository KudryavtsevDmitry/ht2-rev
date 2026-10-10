#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
BYTE* __fastcall MarineQueue_RemoveNode(BYTE*,void*,BYTE*);
BYTE* __fastcall Reference_RemoveNode(BYTE*,void*,BYTE*);
DWORD __fastcall MarineQueue_InsertNode(BYTE*,void*,BYTE*);
DWORD __fastcall Reference_InsertNode(BYTE*,void*,BYTE*);
DWORD __fastcall MarineQueue_InsertEdge(BYTE*,void*,BYTE*);
DWORD __fastcall Reference_InsertEdge(BYTE*,void*,BYTE*);
int _fltused=0;
}
static BYTE nodes[8][80], expected[8][80], list[12], expectedList[12];
static DWORD rebuild, expectedRebuild;
extern "C" DWORD __fastcall RebuildNodes(BYTE* self,void*) { rebuild=500; F(DWORD,self,8)=0; return 0x12345678; }
extern "C" DWORD __fastcall RebuildEdges(BYTE* self,void*) { rebuild=200; F(DWORD,self,8)=0; return 0x87654321; }
KING_ALIAS("FUN_60f390","@RebuildNodes@8")
KING_ALIAS("FUN_60f7f0","@RebuildEdges@8")
static void Setup(unsigned length,unsigned edge,DWORD score,DWORD count,unsigned roots)
{
    rebuild=0;
    for(unsigned i=0;i<8;i++) for(unsigned j=0;j<80;j++) nodes[i][j]=(BYTE)(i*80+j);
    unsigned base=edge?0:4;
    for(unsigned i=0;i<length;i++) {
        F(void*,nodes[i],base)=nodes[(i+1)%length]+base;
        F(void*,nodes[i],base+4)=nodes[(i+length-1)%length]+base;
        F(float,nodes[i],edge?72:28)=(float)(edge?i+1:length-i);
    }
    F(void*,nodes[7],base)=F(void*,nodes[7],base+4)=nodes[7]+base;
    F(DWORD,nodes[7],edge?72:28)=score;
    F(void*,list,0)=(roots&1)&&length?nodes[length/2]:0;
    F(void*,list,4)=(roots&2)&&length?nodes[0]:0;
    F(DWORD,list,8)=count;
}
static void Save()
{
    for(unsigned i=0;i<8;i++) for(unsigned j=0;j<80;j++) expected[i][j]=nodes[i][j];
    for(unsigned i=0;i<12;i++) expectedList[i]=list[i];
    expectedRebuild=rebuild;
}
static void Check(unsigned code)
{
    for(unsigned i=0;i<8;i++) for(unsigned j=0;j<80;j++) if(expected[i][j]!=nodes[i][j]) ExitProcess(code);
    for(unsigned i=0;i<12;i++) if(expectedList[i]!=list[i]) ExitProcess(code+1);
    if(expectedRebuild!=rebuild) ExitProcess(code+2);
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    const DWORD scores[]={0,0x80000000,0xbf800000,0x3f800000,0x40200000,0x40e00000,0x7f800000,0xff800000,0x7fc01234};
    const DWORD counts[]={0,198,199,498,499,0xffffffff};
    for(unsigned edge=0;edge<2;edge++) for(unsigned length=0;length<7;length++)
    for(unsigned value=0;value<9;value++) for(unsigned count=0;count<6;count++) {
        Setup(length,edge,scores[value],counts[count],length?3:0);
        DWORD reference=edge?Reference_InsertEdge(list,0,nodes[7]):Reference_InsertNode(list,0,nodes[7]); Save();
        Setup(length,edge,scores[value],counts[count],length?3:0);
        DWORD actual=edge?MarineQueue_InsertEdge(list,0,nodes[7]):MarineQueue_InsertNode(list,0,nodes[7]);
        Check(10);
        if(reference!=actual) ExitProcess(13);
    }
    for(unsigned length=1;length<7;length++) for(unsigned roots=0;roots<4;roots++) for(unsigned node=0;node<length;node++) {
        Setup(length,0,0,123,roots); BYTE* ref=Reference_RemoveNode(list,0,nodes[node]); Save();
        Setup(length,0,0,123,roots); BYTE* actual=MarineQueue_RemoveNode(list,0,nodes[node]);
        Check(20); if(ref!=actual) ExitProcess(23);
    }
    ExitProcess(0);
}
