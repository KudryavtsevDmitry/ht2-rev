#include "../src/king.h"
extern "C" int _fltused=0;
extern "C" DWORD* __cdecl MarineMatrix_Zero(DWORD*),*Reference_MarineMatrix_Zero(DWORD*);
extern "C" DWORD* __cdecl MarineMatrix_RotateZ(DWORD*,float),*Reference_MarineMatrix_RotateZ(DWORD*,float);
extern "C" DWORD* __cdecl MarineMatrix_Translate(DWORD*,DWORD,DWORD,DWORD),*Reference_MarineMatrix_Translate(DWORD*,DWORD,DWORD,DWORD);
extern "C" DWORD* __cdecl MarineMatrix_Multiply(DWORD*,const float*,const float*),*Reference_MarineMatrix_Multiply(DWORD*,const float*,const float*);
static DWORD data[3][16],seed,angle;
static volatile DWORD expected[3][16];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static void Print(DWORD index,DWORD expectedValue,DWORD actualValue)
{
    char line[32];DWORD values[]={index,expectedValue,actualValue};unsigned n=0;
    for(unsigned j=0;j<3;++j) {for(int i=7;i>=0;--i) line[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];line[n++]=' ';}
    line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    seed=test+0x43ab;
    for(unsigned b=0;b<3;++b) for(unsigned i=0;i<16;++i) data[b][i]=Next();
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000,0x40490fdb};
    angle=Next();
    if(test<144) {
        angle=special[test%12];
        for(unsigned i=0;i<16;++i) {data[0][i]=special[(test+i)%12];data[1][i]=special[(test/12+i)%12];}
    }
    if(test>=144 && test<160) {
        for(unsigned i=0;i<16;++i) {data[0][i]=(i%5)?0:0x3f800000;data[1][i]=(i%5)?0:0x3f800000;}
        data[0][test-144]=0x80000000;
    }
}
static DWORD* GetOut(unsigned alias) {return alias<3?data[alias]:(alias==3?data[0]+1:data[1]+3);}
static DWORD* Run(unsigned method,bool reference,unsigned alias)
{
    DWORD* out=GetOut(alias);
    if(method==0) return reference?Reference_MarineMatrix_Zero(out):MarineMatrix_Zero(out);
    if(method==1) return reference?Reference_MarineMatrix_RotateZ(out,*(float*)&angle):MarineMatrix_RotateZ(out,*(float*)&angle);
    if(method==2) return reference?Reference_MarineMatrix_Translate(out,data[0][0],data[0][1],data[0][2]):MarineMatrix_Translate(out,data[0][0],data[0][1],data[0][2]);
    return reference?Reference_MarineMatrix_Multiply(out,(float*)data[0],(float*)data[1]):MarineMatrix_Multiply(out,(float*)data[0],(float*)data[1]);
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<4;++method) for(unsigned alias=0;alias<5;++alias) for(unsigned test=0;test<1024;++test) {
            Setup(test);DWORD* result=Run(method,true,alias);
            if(result!=GetOut(alias)) ExitProcess(400001);
            for(unsigned b=0;b<3;++b) for(unsigned i=0;i<16;++i) expected[b][i]=data[b][i];
            Setup(test);if(Run(method,false,alias)!=result) ExitProcess(400002);
            for(unsigned b=0;b<3;++b) for(unsigned i=0;i<16;++i) if(expected[b][i]!=data[b][i]) {Print(b*16+i,expected[b][i],data[b][i]);ExitProcess(100000*mode+10000*method+1024*alias+test+1);}
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
