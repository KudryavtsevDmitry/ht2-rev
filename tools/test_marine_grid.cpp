#include "../src/king.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" {int _fltused=0;float g_mg_xLimit,g_mg_yLimit;DWORD g_mg_width,g_mg_height;}
#define MAP_DECL(name) extern "C" double __fastcall name(BYTE*,void*,const DWORD*);
MAP_DECL(MarineGrid_MapX) MAP_DECL(MarineGrid_MapY) MAP_DECL(Reference_MarineGrid_MapX) MAP_DECL(Reference_MarineGrid_MapY)
#define UNMAP_DECL(name) extern "C" void __fastcall name(BYTE*,void*,const DWORD*,float*,float*);
UNMAP_DECL(MarineGrid_Unmap) UNMAP_DECL(Reference_MarineGrid_Unmap)
#define OUT_DECL(name) extern "C" DWORD __fastcall name(BYTE*,void*,const DWORD*);
OUT_DECL(MarineGrid_Outside) OUT_DECL(Reference_MarineGrid_Outside)
#define TRI_DECL(name) extern "C" DWORD __fastcall name(BYTE*,void*,BYTE*);
TRI_DECL(MarineGrid_ClassifyTriangle) TRI_DECL(Reference_MarineGrid_ClassifyTriangle)
extern "C" DWORD __cdecl MarineGrid_Width(),Reference_MarineGrid_Width(),MarineGrid_Height(),Reference_MarineGrid_Height();
static DWORD grid[64],wrapper[8],triangle[8],points[3][2],output[2],seed;
static volatile DWORD expectedGrid[64],expectedPoints[3][2],expectedOutput[2];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static void Print(DWORD index,DWORD expectedValue,DWORD actualValue)
{
    char line[32];DWORD values[]={index,expectedValue,actualValue};unsigned n=0;
    for(unsigned j=0;j<3;++j) {for(int i=7;i>=0;--i) line[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];line[n++]=' ';}
    line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    seed=test+0x538b;
    for(unsigned i=0;i<64;++i) grid[i]=Next();
    for(unsigned i=0;i<8;++i) {wrapper[i]=Next();triangle[i]=Next();}
    for(unsigned i=0;i<3;++i) {points[i][0]=Next();points[i][1]=Next();triangle[4+i]=(DWORD)points[i];}
    wrapper[1]=(DWORD)grid;output[0]=Next();output[1]=Next();
    g_mg_width=Next();g_mg_height=Next();
    g_mg_xLimit=1.0f;g_mg_yLimit=1.0f;
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    const DWORD indices[]={0,1,0x7fffffff,0x80000000,0xffffffff,0x01000001,0x80000001};
    if(test<121) {
        for(unsigned i=0;i<64;++i) grid[i]=special[(test+i)%11];
        for(unsigned i=0;i<3;++i) {points[i][0]=indices[(test+i)%7];points[i][1]=indices[(test/7+i)%7];}
        *(DWORD*)&g_mg_xLimit=special[test%11];*(DWORD*)&g_mg_yLimit=special[(test/11)%11];
    }
    if(test>=1016) {
        F(float,grid,192)=F(float,grid,196)=1.0f;F(float,grid,176)=F(float,grid,188)=0;
        for(unsigned i=0;i<3;++i) {points[i][0]=(test&(1<<i))?2:0;points[i][1]=0;}
    }
}
static unsigned __int64 Run(unsigned method,bool reference,unsigned test)
{
    union {double value;unsigned __int64 bits;} result;
    const DWORD* p=(test&128)?grid+((test>>8)%4)*3:points[0];
    if(method==0) {result.value=reference?Reference_MarineGrid_MapX((BYTE*)grid,0,p):MarineGrid_MapX((BYTE*)grid,0,p);return result.bits;}
    if(method==1) {result.value=reference?Reference_MarineGrid_MapY((BYTE*)grid,0,p):MarineGrid_MapY((BYTE*)grid,0,p);return result.bits;}
    if(method==2) {
        float* x=(float*)output;float* y=(float*)(output+1);
        if(test&256) {x=(float*)(points[0]+1);y=(float*)points[0];}
        if(test&512) {x=(float*)(grid+39);y=(float*)(grid+26);}
        if(reference) Reference_MarineGrid_Unmap((BYTE*)grid,0,p,x,y);else MarineGrid_Unmap((BYTE*)grid,0,p,x,y);
        return 0;
    }
    if(method==3) return reference?Reference_MarineGrid_Outside((BYTE*)wrapper,0,p):MarineGrid_Outside((BYTE*)wrapper,0,p);
    if(method==4) return reference?Reference_MarineGrid_ClassifyTriangle((BYTE*)wrapper,0,(BYTE*)triangle):MarineGrid_ClassifyTriangle((BYTE*)wrapper,0,(BYTE*)triangle);
    if(method==5) return reference?Reference_MarineGrid_Width():MarineGrid_Width();
    return reference?Reference_MarineGrid_Height():MarineGrid_Height();
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<7;++method) for(unsigned test=0;test<1024;++test) {
            Setup(test);unsigned __int64 expectedReturn=Run(method,true,test);
            for(unsigned i=0;i<64;++i) expectedGrid[i]=grid[i];
            for(unsigned i=0;i<3;++i) for(unsigned j=0;j<2;++j) expectedPoints[i][j]=points[i][j];
            expectedOutput[0]=output[0];expectedOutput[1]=output[1];
            Setup(test);unsigned __int64 actualReturn=Run(method,false,test);
            if(expectedReturn!=actualReturn) {Print(method,(DWORD)expectedReturn,(DWORD)actualReturn);ExitProcess(100000*mode+10000*method+test+1);}
            for(unsigned i=0;i<64;++i) if(expectedGrid[i]!=grid[i]) {Print(i,expectedGrid[i],grid[i]);ExitProcess(100000*mode+10000*method+test+1);}
            for(unsigned i=0;i<3;++i) for(unsigned j=0;j<2;++j) if(expectedPoints[i][j]!=points[i][j]) ExitProcess(400001);
            if(expectedOutput[0]!=output[0] || expectedOutput[1]!=output[1]) ExitProcess(400002);
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
