#include "../build-test/marineconstruct-globals.h"
#define DECL(prefix) \
extern "C" BYTE* __fastcall prefix##MarineGrid_ConstructBase(BYTE*,void*); \
extern "C" DWORD __fastcall prefix##MarineMesh_SetCellTriangles(BYTE*,void*,BYTE*,BYTE*); \
extern "C" DWORD __fastcall prefix##MarineMesh_ConstructGrid(BYTE*,void*,const DWORD*,float,float,DWORD);
DECL( /* actual */ )
DECL(Reference_)
static DWORD arena[65536],savedArena[65536],object[32],savedObject[32],center[3],first[16],second[16];
static DWORD used,trace,testCase,allocation,failAt;
static void Trace(DWORD v) {trace=(trace*16777619u)^v;}
extern "C" void* __cdecl TestAllocate(unsigned n)
{
    Trace(n); ++allocation; if(allocation==failAt) return 0;
    DWORD start=used; used+=(n+15)/16*4;
    if(used>65536) ExitProcess(99999);
    return arena+start;
}
extern "C" void __cdecl TestFree(void* p) {Trace((DWORD)p);}
extern "C" DWORD __stdcall TestClock() {Trace(77);return 0xfedcba98u;}
extern "C" DWORD (__stdcall* TestClockImport)()=TestClock;
extern "C" void __cdecl TestError(DWORD code,void*,void*,DWORD line,void*) {Trace(code);Trace(line);}
#pragma warning(disable:4035)
extern "C" __declspec(noinline) double __cdecl TestCeil(double x)
{
    WORD oldControl,newControl;
    __asm fnstcw oldControl
    newControl=(oldControl&~0xc00)|0x800;
    __asm fldcw newControl
    __asm fld x
    __asm frndint
    __asm fldcw oldControl
}
KING_ALIAS("??2@YAPAXI@Z","_TestAllocate")
KING_ALIAS("_malloc","_TestAllocate")
KING_ALIAS("_free","_TestFree")
KING_ALIAS("_ceil","_TestCeil")
KING_ALIAS("_FUN_60aad0","_TestError")
KING_ALIAS("__imp__timeGetTime@0","_TestClockImport")
extern "C" void __cdecl MarineGrid_BuildRandomDirections(DWORD seed) {Trace(seed);}
extern "C" int __fastcall MarineGreed_Allocate(BYTE* p,void*,unsigned vertices,unsigned cells)
{
    Trace(vertices);Trace(cells);
    F(void*,p,12)=TestAllocate(vertices*12);F(void*,p,8)=TestAllocate(cells*4); F(DWORD,p,48)=cells;return 1;
}
extern "C" void __fastcall MarineGreed_Init(BYTE* p,void*)
{
    F(BYTE*,p,32)=(BYTE*)TestAllocate(12);F(DWORD,F(BYTE*,p,32),0)=F(DWORD,F(BYTE*,p,32),4)=0;
    F(WORD,p,36)=F(WORD,p,38)=0;
}
extern "C" void __fastcall MarineGreed_Update(BYTE* p,void*) {Trace(80);Trace(F(DWORD,p,20));Trace(F(DWORD,p,24));}
extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE* list,void*,BYTE* t)
{
    Trace(F(DWORD,t,16));Trace(F(DWORD,t,20));Trace(F(DWORD,t,24));
    BYTE* last=F(BYTE*,list,4);
    if(!last) F(BYTE*,list,4)=t;
    else {
        BYTE* previous=F(BYTE*,last,8);
        F(BYTE*,t,4)=last+4; F(BYTE*,t,8)=previous;
        F(BYTE*,previous,0)=t+4; F(BYTE*,last,8)=t+4;
    }
    return 0;
}
extern "C" void __fastcall MarineGrid_Trim(BYTE*,void*,DWORD trim) {Trace(trim);}
extern "C" DWORD __fastcall GridAllocate(BYTE* p,void*,DWORD x,DWORD y) {Trace(x);Trace(y);F(DWORD,p,44)=x;F(DWORD,p,48)=y;return 1;}
extern "C" DWORD __fastcall GridCount(BYTE*,void*) {Trace(81);return 1024;}
extern "C" double __fastcall GridWeight(BYTE*,void*,const DWORD* a,const DWORD* b,const DWORD* c,BYTE*)
{return (double)(a[0]+b[1]+c[0]+testCase)/7.0;}
extern "C" void __fastcall GridCamera(BYTE* p,void*,BYTE* pos,BYTE* dir) {Trace(82);Trace(F(DWORD,pos,0));Trace(F(DWORD,dir,0));F(DWORD,p,4)=F(DWORD,pos,0);}
static void Setup(DWORD test)
{
    testCase=test;used=trace=allocation=0;failAt=0;
    for(unsigned i=0;i<65536;++i) arena[i]=0xcdcdcdcd;
    for(unsigned i=0;i<32;++i)object[i]=0xa5a5a5a5;
    for(unsigned i=0;i<16;++i){first[i]=0xdead0000+i;second[i]=0xbeef0000+i;g_constructGridVtable[i]=0;}
    center[0]=0x440c1000u+test*257;center[1]=0xc4812000u-test*123;center[2]=0;
    g_constructGridVtable[1]=(DWORD)GridCamera;g_constructGridVtable[3]=(DWORD)GridCount;
    g_constructGridVtable[5]=(DWORD)GridAllocate;g_constructGridVtable[9]=(DWORD)GridWeight;
    g_constructResolution=(test&32)?0.03125f:0.0625f;g_constructHalf=0.5f;
    g_constructNormalZ=0x3f000000+test;
}
static DWORD Run(unsigned method,bool reference)
{
    BYTE* p=(BYTE*)object;
    if(method==0){if(reference)Reference_MarineGrid_ConstructBase(p,0);else MarineGrid_ConstructBase(p,0);return (DWORD)p;}
    if(method==1) return reference?Reference_MarineMesh_SetCellTriangles(p,0,testCase&1?(BYTE*)first:0,testCase&2?(BYTE*)second:0):MarineMesh_SetCellTriangles(p,0,testCase&1?(BYTE*)first:0,testCase&2?(BYTE*)second:0);
    float width=(float)(1+testCase%7)*16.0f-0.125f;
    float height=(float)(1+(testCase/7)%7)*16.0f-0.375f;
    if(testCase&256) { if(testCase&128) height=0; else width=0; }
    // The cell allocation's null return is the constructor's explicit failure path.
    if((testCase&128)&&!(testCase&256))failAt=5;
    return reference?Reference_MarineMesh_ConstructGrid(p,0,center,width,height,testCase):MarineMesh_ConstructGrid(p,0,center,width,height,testCase);
}
static void Print(DWORD a,DWORD b,DWORD c)
{
    DWORD vals[]={a,b,c};char text[28];unsigned n=0;
    for(unsigned j=0;j<3;++j){for(int i=7;i>=0;--i)text[n++]="0123456789abcdef"[(vals[j]>>(i*4))&15];text[n++]=' ';}
    text[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,n,&written,0);
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    WORD original,control;__asm fnstcw original
    WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode){control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<3;++method)for(unsigned test=0;test<512;++test){
            Setup(test);DWORD expected=Run(method,true),expectedUsed=used,expectedTrace=trace;
            for(unsigned i=0;i<65536;++i)savedArena[i]=arena[i];
            for(unsigned i=0;i<32;++i)savedObject[i]=object[i];
            DWORD savedFirst=first[8],savedSecond=second[8];
            Setup(test);DWORD actual=Run(method,false);DWORD code=mode*10000+method*1000+test+1;
            if(actual!=expected||used!=expectedUsed||trace!=expectedTrace||first[8]!=savedFirst||second[8]!=savedSecond){Print(code,expectedTrace,trace);ExitProcess(code);}
            for(unsigned i=0;i<32;++i)if(object[i]!=savedObject[i]){Print(code,i,object[i]);ExitProcess(code);}
            for(unsigned i=0;i<65536;++i)if(arena[i]!=savedArena[i]){Print(code,i,arena[i]);Print(i,savedArena[i],arena[i]);ExitProcess(code);}
        }
    }
    const char message[]="Marine grid construction matches MASM in 4,608 allocation, topology, failure and full-state comparisons.\n";
    DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,0);ExitProcess(0);
}

