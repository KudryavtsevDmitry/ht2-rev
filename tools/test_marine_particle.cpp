#include "../src/king.h"
extern "C" {
int _fltused=0;
float g_mp_scale,g_mp_randomUnit,g_mp_angularFactor,g_mp_seconds;
}
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" void __fastcall MarineParticle_Start(BYTE*,void*,const DWORD*,const DWORD*);
extern "C" void __fastcall Reference_MarineParticle_Start(BYTE*,void*,const DWORD*,const DWORD*);
extern "C" void __fastcall MarineParticle_SetLifetime(BYTE*,void*,DWORD);
extern "C" void __fastcall Reference_MarineParticle_SetLifetime(BYTE*,void*,DWORD);
extern "C" void __fastcall MarineParticle_Update(BYTE*,void*,DWORD);
extern "C" void __fastcall Reference_MarineParticle_Update(BYTE*,void*,DWORD);
static DWORD object[20],position[3],velocity[3],clockValue,seed,testCase,calls,trace[4];
static volatile DWORD expected[20],expectedTrace[4];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
extern "C" DWORD __stdcall TestClock()
{
    trace[calls++]=0x111;
    if(testCase&32) {object[0]^=0x80000000;object[11]=0xfffffff0;}
    return clockValue;
}
extern "C" int __cdecl TestRand()
{
    trace[calls++]=0x222;
    if(testCase&64) {object[11]+=17;g_mp_scale=-g_mp_scale;}
    return (int)((testCase*131+calls*479)&32767);
}
static void Print(DWORD index,DWORD expectedValue,DWORD actualValue)
{
    char line[32];DWORD values[]={index,expectedValue,actualValue};unsigned n=0;
    for(unsigned j=0;j<3;++j) {for(int i=7;i>=0;--i) line[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];line[n++]=' ';}
    line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    seed=test+0x812d;testCase=test;calls=0;
    for(unsigned i=0;i<20;++i) object[i]=Next();
    for(unsigned i=0;i<3;++i) {position[i]=Next();velocity[i]=Next();}
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test<121) {
        for(unsigned i=0;i<20;++i) object[i]=special[(test+i)%11];
        for(unsigned i=0;i<3;++i) {position[i]=special[test%11];velocity[i]=special[(test/11)%11];}
    }
    clockValue=Next();
    const DWORD tickPairs[][2]={{0,0},{1,0},{0,1},{0,0xffffffff},{0xffffffff,0},{0x80000000,0},{0x7fffffff,0},{0x80000001,0},{3,0xfffffff0}};
    if(test<9) {clockValue=tickPairs[test][0];object[12]=tickPairs[test][1];}
    g_mp_scale=(float)((int)(test%37)-18)/16.0f;
    g_mp_randomUnit=1.0f/32768.0f;g_mp_angularFactor=2.0f;g_mp_seconds=0.001f;
    if(test<44) {
        *(DWORD*)&g_mp_scale=special[test%11];
        if(test&1) *(DWORD*)&g_mp_seconds=special[(test/11)%11];
    }
    for(unsigned i=0;i<4;++i) trace[i]=0;
}
static void Run(unsigned method,bool reference)
{
    const DWORD* p=(testCase&128)?object+((testCase>>8)%4)*3:position;
    const DWORD* v=(testCase&256)?object+((testCase>>6)%4)*3:velocity;
    if(method==0) {if(reference) Reference_MarineParticle_Start((BYTE*)object,0,p,v);else MarineParticle_Start((BYTE*)object,0,p,v);}
    if(method==1) {if(reference) Reference_MarineParticle_SetLifetime((BYTE*)object,0,clockValue);else MarineParticle_SetLifetime((BYTE*)object,0,clockValue);}
    if(method==2) {if(reference) Reference_MarineParticle_Update((BYTE*)object,0,clockValue);else MarineParticle_Update((BYTE*)object,0,clockValue);}
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<3;++method) for(unsigned test=0;test<1024;++test) {
            Setup(test);Run(method,true);
            for(unsigned i=0;i<20;++i) expected[i]=object[i];
            for(unsigned i=0;i<4;++i) expectedTrace[i]=trace[i];
            DWORD expectedCalls=calls,expectedScale=*(DWORD*)&g_mp_scale;
            Setup(test);Run(method,false);
            for(unsigned i=0;i<20;++i) if(expected[i]!=object[i]) {Print(i,expected[i],object[i]);ExitProcess(100000*mode+10000*method+test+1);}
            for(unsigned i=0;i<4;++i) if(expectedTrace[i]!=trace[i]) ExitProcess(400001);
            if(calls!=expectedCalls || expectedScale!=*(DWORD*)&g_mp_scale) ExitProcess(400002);
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
