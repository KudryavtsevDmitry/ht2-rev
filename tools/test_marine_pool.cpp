#include "../src/king.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" {
int _fltused=0;float g_mp_scale,g_mp_randomUnit,g_mp_angularFactor,g_mp_seconds;
DWORD g_mpool_enabled;float g_mpool_angle,g_mpool_colorScale,g_mpool_half;
BYTE* g_marineApplicationState;
}
extern "C" void __fastcall MarineParticlePool_BuildGeometry(BYTE*,void*,DWORD,DWORD);
extern "C" void __fastcall Reference_MarineParticlePool_BuildGeometry(BYTE*,void*,DWORD,DWORD);
extern "C" BYTE* __fastcall MarineParticlePool_Construct(BYTE*,void*,DWORD,DWORD,DWORD,DWORD);
extern "C" BYTE* __fastcall Reference_MarineParticlePool_Construct(BYTE*,void*,DWORD,DWORD,DWORD,DWORD);
extern "C" DWORD __fastcall MarineParticlePool_Add(BYTE*,void*,const DWORD*,const DWORD*);
extern "C" DWORD __fastcall Reference_MarineParticlePool_Add(BYTE*,void*,const DWORD*,const DWORD*);
#define INDEX_DECL(name) extern "C" DWORD __fastcall name(BYTE*,void*,DWORD);
INDEX_DECL(MarineParticlePool_Remove) INDEX_DECL(Reference_MarineParticlePool_Remove)
extern "C" DWORD __fastcall MarineParticlePool_CanAdd(BYTE*,void*);
extern "C" DWORD __fastcall Reference_MarineParticlePool_CanAdd(BYTE*,void*);
#define SET_DECL(name) extern "C" void __fastcall name(BYTE*,void*,DWORD);
SET_DECL(MarineParticlePool_SetFloor) SET_DECL(Reference_MarineParticlePool_SetFloor)
SET_DECL(MarineParticlePool_SetLifetime) SET_DECL(Reference_MarineParticlePool_SetLifetime)
static DWORD pool[16],buffers[2][128],application[332],position[3],velocity[3],trace[128];
static DWORD seed,testCase,calls,allocations,randomCalls,randomState;
static volatile DWORD expectedPool[16],expectedBuffers[2][128],expectedTrace[128];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static void Record(DWORD a,DWORD b) {trace[calls++]=a;trace[calls++]=b;}
extern "C" void* __cdecl TestMalloc(size_t bytes)
{
    Record(1,bytes);return buffers[(allocations++)&1];
}
extern "C" DWORD __stdcall TestClock()
{
    Record(2,testCase*719+0xfffffff0u);
    if(testCase&64) pool[8]+=11;
    return testCase*719+0xfffffff0u;
}
extern "C" void __cdecl TestSrand(unsigned value)
{
    Record(3,value);randomState=value;
    if(testCase&128) pool[6]=testCase%8;
}
extern "C" int __cdecl TestRand()
{
    randomState=randomState*214013u+2531011u;
    DWORD value=(randomState>>16)&32767;
    if(testCase&16) value=(randomCalls&1)?32767:0;
    Record(4,value);++randomCalls;
    if(testCase&32) pool[2]=(DWORD)buffers[randomCalls&1];
    if(testCase&256) {pool[3]=(DWORD)buffers[randomCalls&1];pool[4]=testCase%4;pool[8]+=1;}
    return value;
}
static void Print(DWORD index,DWORD expectedValue,DWORD actualValue)
{
    char line[32];DWORD values[]={index,expectedValue,actualValue};unsigned n=0;
    for(unsigned j=0;j<3;++j) {for(int i=7;i>=0;--i) line[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];line[n++]=' ';}
    line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    seed=test+0x172b;testCase=test;calls=allocations=randomCalls=0;randomState=test+13;
    for(unsigned i=0;i<16;++i) pool[i]=Next();
    for(unsigned b=0;b<2;++b) for(unsigned i=0;i<128;++i) buffers[b][i]=Next();
    for(unsigned i=0;i<3;++i) {position[i]=Next();velocity[i]=Next();}
    for(unsigned i=0;i<128;++i) trace[i]=0;
    pool[1]=(test&1)?1:0;pool[2]=(DWORD)buffers[1];pool[3]=(DWORD)buffers[0];pool[4]=test%8;pool[5]=(test>>3)%8;pool[6]=test%8;
    g_marineApplicationState=(BYTE*)application;application[325]=test%3;
    g_mpool_enabled=(test&2)?0:1;
    g_mp_scale=0.75f;g_mp_randomUnit=1.0f/32768.0f;g_mp_angularFactor=2;g_mp_seconds=0.001f;
    g_mpool_angle=3.1415927f;g_mpool_colorScale=1.0f/65536.0f;g_mpool_half=0.5f;
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test<121) {*(DWORD*)&g_mp_randomUnit=special[test%11];*(DWORD*)&g_mpool_colorScale=special[(test/11)%11];}
}
static DWORD Run(unsigned method,bool reference,unsigned test)
{
    DWORD color=test*0x1827513u;
    if(method==0) {if(reference) Reference_MarineParticlePool_BuildGeometry((BYTE*)pool,0,test%8,color);else MarineParticlePool_BuildGeometry((BYTE*)pool,0,test%8,color);}
    if(method==1) return (DWORD)(reference?Reference_MarineParticlePool_Construct((BYTE*)pool,0,test%8,(test>>3)%8,color,test):MarineParticlePool_Construct((BYTE*)pool,0,test%8,(test>>3)%8,color,test));
    if(method==2) {const DWORD* p=(test&512)?buffers[0]:position;const DWORD* v=(test&512)?buffers[0]+3:velocity;return reference?Reference_MarineParticlePool_Add((BYTE*)pool,0,p,v):MarineParticlePool_Add((BYTE*)pool,0,p,v);}
    if(method==3) return reference?Reference_MarineParticlePool_Remove((BYTE*)pool,0,(test>>3)%9):MarineParticlePool_Remove((BYTE*)pool,0,(test>>3)%9);
    if(method==4) return reference?Reference_MarineParticlePool_CanAdd((BYTE*)pool,0):MarineParticlePool_CanAdd((BYTE*)pool,0);
    if(method==5) {if(reference) Reference_MarineParticlePool_SetFloor((BYTE*)pool,0,color);else MarineParticlePool_SetFloor((BYTE*)pool,0,color);}
    if(method==6) {if(reference) Reference_MarineParticlePool_SetLifetime((BYTE*)pool,0,color);else MarineParticlePool_SetLifetime((BYTE*)pool,0,color);}
    return 0;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<7;++method) for(unsigned test=0;test<1024;++test) {
            Setup(test);DWORD expectedReturn=Run(method,true,test),expectedCalls=calls;
            for(unsigned i=0;i<16;++i) expectedPool[i]=pool[i];
            for(unsigned b=0;b<2;++b) for(unsigned i=0;i<128;++i) expectedBuffers[b][i]=buffers[b][i];
            for(unsigned i=0;i<128;++i) expectedTrace[i]=trace[i];
            Setup(test);DWORD actualReturn=Run(method,false,test);
            if(expectedReturn!=actualReturn || expectedCalls!=calls) {Print(method,expectedReturn,actualReturn);ExitProcess(100000*mode+10000*method+test+1);}
            for(unsigned i=0;i<16;++i) if(expectedPool[i]!=pool[i]) {Print(i,expectedPool[i],pool[i]);ExitProcess(100000*mode+10000*method+test+1);}
            for(unsigned b=0;b<2;++b) for(unsigned i=0;i<128;++i) if(expectedBuffers[b][i]!=buffers[b][i]) {Print(1000+b*128+i,expectedBuffers[b][i],buffers[b][i]);ExitProcess(100000*mode+10000*method+test+1);}
            for(unsigned i=0;i<128;++i) if(expectedTrace[i]!=trace[i]) ExitProcess(400001);
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
