#include "../src/king.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
struct MarineParticleMatrix {DWORD v[16];};
extern "C" {int _fltused=0;DWORD g_mpool_enabled;float g_mp_seconds;MarineParticleMatrix g_mpool_restoreMatrix;}
extern "C" void __fastcall MarineParticlePool_Render(BYTE*,void*,BYTE*,BYTE*,DWORD);
extern "C" void __fastcall Reference_MarineParticlePool_Render(BYTE*,void*,BYTE*,BYTE*,DWORD);
static DWORD pool[16],particles[2][120],geometry[2][32],owners[2][2],device[2],event[4],trace[1024];
static void* ownerTable[11];static void* deviceTables[2][26];
static DWORD seed,testCase,calls,clockValue,transformCalls,drawCalls;
static volatile DWORD expectedPool[16],expectedParticles[2][120],expectedTrace[1024];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static void Record(DWORD value) {trace[calls++]=value;}
extern "C" DWORD __stdcall TestClock()
{
    Record(1);Record(clockValue);
    if(testCase&64) {pool[4]=testCase%8;g_mpool_enabled=0;}
    return clockValue;
}
extern "C" double __fastcall TestHeight(BYTE* self,void*,BYTE* particle)
{
    Record(2);Record((DWORD)self);Record((DWORD)particle);
    for(unsigned i=0;i<3;++i) Record(F(DWORD,particle,i*4));
    float height=F(float,particle,8);
    if(testCase&128) {F(DWORD,particle,0)^=0x80000000;pool[0]=(DWORD)owners[1];}
    if(testCase&256) {pool[3]=(DWORD)particles[1];particles[1][13]=0x3f800000;}
    return height;
}
static DWORD Transform(DWORD tag,BYTE* self,DWORD state,const DWORD* matrix)
{
    Record(tag);Record((DWORD)self);Record(state);Record(matrix==g_mpool_restoreMatrix.v);
    for(unsigned i=0;i<16;++i) Record(matrix[i]);
    ++transformCalls;
    if(testCase&32) device[0]=(DWORD)deviceTables[1];
    return 0;
}
extern "C" DWORD __stdcall TestTransformA(BYTE* self,DWORD state,const DWORD* matrix) {return Transform(3,self,state,matrix);}
extern "C" DWORD __stdcall TestTransformB(BYTE* self,DWORD state,const DWORD* matrix) {return Transform(4,self,state,matrix);}
extern "C" void __fastcall TestBind(BYTE* self,void*,DWORD material,DWORD a,DWORD b,DWORD c)
{
    Record(5);Record((DWORD)self);Record(material);Record(a);Record(b);Record(c);
    if(testCase&16) {pool[2]=(DWORD)geometry[1];pool[6]=(testCase>>3)%8;}
    if(testCase&8) device[0]=(DWORD)deviceTables[1];
}
static DWORD State(DWORD tag,BYTE* self,DWORD state,DWORD value)
{
    Record(tag);Record((DWORD)self);Record(state);Record(value);
    if(testCase&4) device[0]=(DWORD)deviceTables[0];
    return 0;
}
extern "C" DWORD __stdcall TestStateA(BYTE* self,DWORD state,DWORD value) {return State(6,self,state,value);}
extern "C" DWORD __stdcall TestStateB(BYTE* self,DWORD state,DWORD value) {return State(7,self,state,value);}
static DWORD Draw(DWORD tag,BYTE* self,DWORD primitive,DWORD format,const DWORD* vertices,DWORD count,DWORD flags)
{
    Record(tag);Record((DWORD)self);Record(primitive);Record(format);Record((DWORD)vertices);Record(count);Record(flags);
    for(DWORD i=0;i<count*4;++i) Record(vertices[i]);
    ++drawCalls;
    if(testCase&512) pool[4]=2;
    return 0;
}
extern "C" DWORD __stdcall TestDrawA(BYTE* self,DWORD primitive,DWORD format,const DWORD* vertices,DWORD count,DWORD flags) {return Draw(8,self,primitive,format,vertices,count,flags);}
extern "C" DWORD __stdcall TestDrawB(BYTE* self,DWORD primitive,DWORD format,const DWORD* vertices,DWORD count,DWORD flags) {return Draw(9,self,primitive,format,vertices,count,flags);}
static void Print(DWORD index,DWORD expectedValue,DWORD actualValue)
{
    char line[32];DWORD values[]={index,expectedValue,actualValue};unsigned n=0;
    for(unsigned j=0;j<3;++j) {for(int i=7;i>=0;--i) line[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];line[n++]=' ';}
    line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    seed=test+0x286b;testCase=test;calls=transformCalls=drawCalls=0;
    for(unsigned i=0;i<16;++i) {pool[i]=Next();g_mpool_restoreMatrix.v[i]=Next();}
    for(unsigned b=0;b<2;++b) {
        for(unsigned i=0;i<120;++i) particles[b][i]=Next();
        for(unsigned i=0;i<32;++i) geometry[b][i]=Next();
        owners[b][0]=(DWORD)ownerTable;owners[b][1]=b;
    }
    for(unsigned i=0;i<1024;++i) trace[i]=0;
    pool[0]=(DWORD)owners[0];pool[2]=(DWORD)geometry[0];pool[3]=(DWORD)particles[0];pool[4]=test%8;pool[6]=(test>>3)%8;
    F(float,pool,28)=-1.0f;
    g_mpool_enabled=(test&1)?1:0;g_mp_seconds=0.001f;
    clockValue=(test&2)?0xfffffffe:test*113;
    device[0]=(DWORD)deviceTables[0];device[1]=0xabc;
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    for(unsigned b=0;b<2;++b) for(unsigned i=0;i<8;++i) {
        DWORD* p=particles[b]+i*15;
        for(unsigned j=0;j<15;++j) *(float*)(p+j)=(float)((int)((test+i+j)%29)-14)/8.0f;
        p[10]=clockValue+((i&1)?7:0xfffffff9);p[12]=clockValue-17;
        if(test<121) for(unsigned j=0;j<15;++j) if(j!=10 && j!=12) p[j]=special[(test+i+j)%11];
    }
    if(test<121) pool[7]=special[test%11];
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    ownerTable[10]=(void*)TestHeight;
    deviceTables[0][11]=(void*)TestTransformA;deviceTables[1][11]=(void*)TestTransformB;
    deviceTables[0][20]=(void*)TestStateA;deviceTables[1][20]=(void*)TestStateB;
    deviceTables[0][25]=(void*)TestDrawA;deviceTables[1][25]=(void*)TestDrawB;
    WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned test=0;test<1024;++test) {
            Setup(test);Reference_MarineParticlePool_Render((BYTE*)pool,0,(BYTE*)device,(BYTE*)event,test*131);
            DWORD expectedCalls=calls,expectedTransforms=transformCalls,expectedDraws=drawCalls,expectedDevice=device[0],expectedEnabled=g_mpool_enabled;
            for(unsigned i=0;i<16;++i) expectedPool[i]=pool[i];
            for(unsigned b=0;b<2;++b) for(unsigned i=0;i<120;++i) expectedParticles[b][i]=particles[b][i];
            for(unsigned i=0;i<1024;++i) expectedTrace[i]=trace[i];
            Setup(test);MarineParticlePool_Render((BYTE*)pool,0,(BYTE*)device,(BYTE*)event,test*131);
            if(calls!=expectedCalls || transformCalls!=expectedTransforms || drawCalls!=expectedDraws || device[0]!=expectedDevice || g_mpool_enabled!=expectedEnabled) ExitProcess(400001);
            for(unsigned i=0;i<16;++i) if(expectedPool[i]!=pool[i]) {Print(i,expectedPool[i],pool[i]);ExitProcess(10000*mode+test+1);}
            for(unsigned b=0;b<2;++b) for(unsigned i=0;i<120;++i) if(expectedParticles[b][i]!=particles[b][i]) {Print(1000+b*120+i,expectedParticles[b][i],particles[b][i]);ExitProcess(10000*mode+test+1);}
            for(unsigned i=0;i<1024;++i) if(expectedTrace[i]!=trace[i]) {Print(2000+i,expectedTrace[i],trace[i]);ExitProcess(10000*mode+test+1);}
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
