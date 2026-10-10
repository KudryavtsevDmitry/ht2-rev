#include "../src/king.h"
extern "C" {int _fltused=0;float g_ms_recordType;}
extern "C" DWORD __fastcall MarineScratch_ResetCounts(BYTE*,void*);
extern "C" DWORD __fastcall Reference_MarineScratch_ResetCounts(BYTE*,void*);
extern "C" DWORD __fastcall MarineScratch_AppendPoint(BYTE*,void*,const DWORD*);
extern "C" DWORD __fastcall Reference_MarineScratch_AppendPoint(BYTE*,void*,const DWORD*);
#define RELEASE_DECL(name) extern "C" void __fastcall name(BYTE*,void*);
RELEASE_DECL(MarineScratch_ReleaseAuxiliary) RELEASE_DECL(Reference_MarineScratch_ReleaseAuxiliary)
RELEASE_DECL(MarineScratch_ReleasePoints) RELEASE_DECL(Reference_MarineScratch_ReleasePoints)
static DWORD mesh[64],buffer[256],point[2],trace[8],calls,testCase,seed;
static volatile DWORD expectedMesh[64],expectedBuffer[256],expectedTrace[8];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
extern "C" void __cdecl TestDelete(void* value)
{
    trace[calls++]=(DWORD)value;
    if(testCase&64) mesh[35]=(DWORD)(buffer+8);
    if(testCase&128) mesh[33]=(DWORD)(buffer+16);
    if(testCase&256) mesh[31]=(DWORD)(buffer+24);
}
static void Setup(unsigned test)
{
    seed=test+0x271c;testCase=test;calls=0;
    for(unsigned i=0;i<64;++i) mesh[i]=Next();
    for(unsigned i=0;i<256;++i) buffer[i]=Next();
    for(unsigned i=0;i<8;++i) trace[i]=0;
    point[0]=Next();point[1]=Next();
    mesh[30]=test%16;mesh[31]=(DWORD)buffer;
    mesh[33]=(test&1)?(DWORD)(buffer+1):0;mesh[35]=(test&2)?(DWORD)(buffer+2):0;
    const DWORD indices[]={0,1,0x7fffffff,0x80000000,0xffffffff,0x01000001,0x80000001};
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    *(DWORD*)&g_ms_recordType=special[test%11];
    if(test<49) {point[0]=indices[test%7];point[1]=indices[(test/7)%7];}
    if(test&128) {mesh[30]=0;mesh[31]=(DWORD)(mesh+28);}
}
static DWORD Run(unsigned method,bool reference,unsigned test)
{
    BYTE* self=(BYTE*)mesh;
    const DWORD* p=(test&32)?(DWORD*)mesh[31]+mesh[30]*12:point;
    if(test&512) p=mesh+30;
    if(method==0) return reference?Reference_MarineScratch_ResetCounts(self,0):MarineScratch_ResetCounts(self,0);
    if(method==1) return reference?Reference_MarineScratch_AppendPoint(self,0,p):MarineScratch_AppendPoint(self,0,p);
    if(method==2) {if(reference) Reference_MarineScratch_ReleaseAuxiliary(self,0);else MarineScratch_ReleaseAuxiliary(self,0);}
    if(method==3) {if(reference) Reference_MarineScratch_ReleasePoints(self,0);else MarineScratch_ReleasePoints(self,0);}
    return 0;
}
static void Print(DWORD index,DWORD expectedValue,DWORD actualValue)
{
    char line[32];DWORD values[]={index,expectedValue,actualValue};unsigned n=0;
    for(unsigned j=0;j<3;++j) {for(int i=7;i>=0;--i) line[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];line[n++]=' ';}
    line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<4;++method) for(unsigned test=0;test<1024;++test) {
            Setup(test);DWORD expectedReturn=Run(method,true,test),expectedCalls=calls;
            for(unsigned i=0;i<64;++i) expectedMesh[i]=mesh[i];
            for(unsigned i=0;i<256;++i) expectedBuffer[i]=buffer[i];
            for(unsigned i=0;i<8;++i) expectedTrace[i]=trace[i];
            Setup(test);DWORD actualReturn=Run(method,false,test);
            DWORD code=100000*mode+10000*method+test+1;
            if(expectedReturn!=actualReturn || expectedCalls!=calls) {Print(method,expectedReturn,actualReturn);ExitProcess(code);}
            for(unsigned i=0;i<64;++i) if(expectedMesh[i]!=mesh[i]) {Print(i,expectedMesh[i],mesh[i]);ExitProcess(code);}
            for(unsigned i=0;i<256;++i) if(expectedBuffer[i]!=buffer[i]) {Print(100+i,expectedBuffer[i],buffer[i]);ExitProcess(code);}
            for(unsigned i=0;i<8;++i) if(expectedTrace[i]!=trace[i]) ExitProcess(400001);
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
