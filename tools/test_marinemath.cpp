#include "../build-test/marinemath-globals.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
#define METHODS(prefix) METHODS_I(prefix)
#define METHODS_I(prefix) \
extern "C" double __fastcall prefix##MarineGrid_TriangleWeight(BYTE*,void*,const DWORD*,const DWORD*,const DWORD*,BYTE*); \
extern "C" double __fastcall prefix##MarineGrid_SurfaceWeight(BYTE*,void*,const DWORD*,const DWORD*,const DWORD*,DWORD,const DWORD*,const DWORD*,const DWORD*,DWORD); \
extern "C" void __fastcall prefix##MarineGrid_ReflectionUV(BYTE*,void*,float,float,float,const Vec3*,float*,float*); \
extern "C" double __fastcall prefix##MarineGrid_RippleHeight(BYTE*,void*,const float*); \
extern "C" void __fastcall prefix##MarineMaterial_UpdateFrame(BYTE*,void*,DWORD); \
extern "C" void __fastcall prefix##MarineGrid_SetTime(BYTE*,void*,DWORD); \
extern "C" void __cdecl prefix##MarineGrid_InitDiameter(); \
extern "C" void __cdecl prefix##MarineGrid_InitRandomDiameter();
METHODS(Reference_)
extern "C" double __fastcall MarineGrid_TriangleWeight(BYTE*,void*,const DWORD*,const DWORD*,const DWORD*,BYTE*);
extern "C" double __fastcall MarineGrid_SurfaceWeight(BYTE*,void*,const DWORD*,const DWORD*,const DWORD*,DWORD,const DWORD*,const DWORD*,const DWORD*,DWORD);
extern "C" void __fastcall MarineGrid_ReflectionUV(BYTE*,void*,float,float,float,const Vec3*,float*,float*);
extern "C" double __fastcall MarineGrid_RippleHeight(BYTE*,void*,const float*);
extern "C" void __fastcall MarineMaterial_UpdateFrame(BYTE*,void*,DWORD);
extern "C" void __fastcall MarineGrid_SetTime(BYTE*,void*,DWORD);
extern "C" void __cdecl MarineGrid_InitDiameter();
extern "C" void __cdecl MarineGrid_InitRandomDiameter();
static DWORD object[64],material[16],triangle[16],points[6][2],vector[6],output[16],vtable[8],seed,testCase,trace,now;
static DWORD Next(){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static DWORD Bits(float x){return *(DWORD*)&x;}
extern "C" DWORD __fastcall Depth(BYTE* self,void*)
{
    trace=1;
    if(testCase&32) F(float,self,4)=-F(float,self,4);
    return testCase%11;
}
static void Setup(unsigned test)
{
    testCase=test;seed=test+71231;trace=0;
    for(unsigned i=0;i<64;++i)object[i]=Bits((float)((int)(Next()%127)-63)/8);
    for(unsigned i=0;i<16;++i){material[i]=Next();triangle[i]=Next();output[i]=Next();}
    for(unsigned i=0;i<6;++i){points[i][0]=Next();points[i][1]=Next();vector[i]=Bits((float)((int)(Next()%97)-48)/4);}
    for(unsigned i=0;i<sizeof(floatCoefficients)/sizeof(floatCoefficients[0]);++i)*floatCoefficients[i]=(float)((int)((test+i*13)%41)-20)/8;
    g_mm_epsilon=1e-5;g_mm_unit=1;g_mm_zero=0;g_mm_one=1;g_mm_negativeOne=-1;g_mm_half=0.5f;g_mm_third=1.0f/3;g_mm_normalZ=-1;g_mm_backWeight=0.001f;g_mm_smallZ=0.01f;g_mm_uvScale=0.5f;
    g_angleScale=57.29578f;g_zeroVec.x=g_zeroVec.y=g_zeroVec.z=0;
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test<121) {
        for(unsigned i=0;i<6;++i)vector[i]=special[(test+i)%11];
        for(unsigned i=0;i<sizeof(floatCoefficients)/sizeof(floatCoefficients[0]);++i)
            if(floatCoefficients[i]!=&g_mm_zero&&floatCoefficients[i]!=&g_mm_one) *(DWORD*)floatCoefficients[i]=special[(test/11+i)%11];
        for(unsigned i=1;i<7;++i)object[i]=special[(test+i*3)%11];
    }
    for(unsigned i=0;i<720;++i) {g_testSine[i]=(float)((int)((test+i)%31)-15)/16;g_testCosine[i]=(float)((int)((test+i*7)%31)-15)/16;}
    object[0]=(DWORD)vtable;vtable[4]=(DWORD)Depth;triangle[6]=test%13;
    object[50]=(test&1)?(DWORD)material:0;
    material[0]=(test&2)?0:(test&128)?0x80000001u:(test%13+1);material[1]=Next();
    const DWORD intervals[]={0,1,17,0xfffffff9,0x7fffffff,0x80000001,0xffffffff};material[2]=intervals[(test/13)%7];
    const DWORD ticks[]={0,1,17,0x7fffffff,0x80000000,0xffffffff,0xfffffff0};
    now=ticks[test%7];object[10]=ticks[(test/7)%7];
    if(test&64){points[0][0]=0;points[0][1]=0;points[1][0]=1;points[1][1]=0;points[2][0]=0;points[2][1]=1;}
}
static unsigned __int64 Run(unsigned method,bool reference)
{
    BYTE* self=(BYTE*)object;union{double value;unsigned __int64 bits;} result;result.bits=0;
    const DWORD* a=(testCase&256)?object+1:points[0];
    const float* surface=(testCase&512)?(float*)(object+4):(float*)(vector+3);
    float* u=(float*)output;float* v=(float*)(output+1);
    if(testCase&128){u=(float*)(object+1);v=(float*)(object+2);}
    if(testCase&64){u=&g_mm_uvScale;v=&g_mm_half;}
    switch(method){
    case 0:result.value=reference?Reference_MarineGrid_TriangleWeight(self,0,a,points[1],points[2],(BYTE*)triangle):MarineGrid_TriangleWeight(self,0,a,points[1],points[2],(BYTE*)triangle);break;
    case 1:result.value=reference?Reference_MarineGrid_SurfaceWeight(self,0,a,points[1],points[2],123,points[3],points[4],points[5],testCase&1):MarineGrid_SurfaceWeight(self,0,a,points[1],points[2],123,points[3],points[4],points[5],testCase&1);break;
    case 2:if(reference)Reference_MarineGrid_ReflectionUV(self,0,*(float*)vector,*(float*)(vector+1),*(float*)(vector+2),(Vec3*)surface,u,v);else MarineGrid_ReflectionUV(self,0,*(float*)vector,*(float*)(vector+1),*(float*)(vector+2),(Vec3*)surface,u,v);break;
    case 3:result.value=reference?Reference_MarineGrid_RippleHeight(self,0,(float*)a):MarineGrid_RippleHeight(self,0,(float*)a);break;
    case 4:if(reference)Reference_MarineMaterial_UpdateFrame((BYTE*)material,0,now);else MarineMaterial_UpdateFrame((BYTE*)material,0,now);break;
    case 5:if(reference)Reference_MarineGrid_SetTime(self,0,now);else MarineGrid_SetTime(self,0,now);break;
    case 6:if(reference)Reference_MarineGrid_InitDiameter();else MarineGrid_InitDiameter();break;
    case 7:if(reference)Reference_MarineGrid_InitRandomDiameter();else MarineGrid_InitRandomDiameter();break;
    }
    return result.bits;
}
static DWORD snapshot[256];
static void Print(DWORD a,DWORD b,DWORD c){char text[28];DWORD args[]={a,b,c};unsigned n=0;for(unsigned j=0;j<3;++j){for(int i=7;i>=0;--i)text[n++]="0123456789abcdef"[(args[j]>>(i*4))&15];text[n++]=' ';}text[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,n,&written,0);}
static unsigned State(bool save)
{
    DWORD* regions[]={object,material,triangle,(DWORD*)points,vector,output,&trace};unsigned sizes[]={64,16,16,12,6,16,1};unsigned n=0;
    for(unsigned r=0;r<7;++r)for(unsigned i=0;i<sizes[r];++i){if(save)snapshot[n]=regions[r][i];else if(snapshot[n]!=regions[r][i]){Print(n,snapshot[n],regions[r][i]);return n+1;}++n;}
    for(unsigned i=0;i<sizeof(floatCoefficients)/sizeof(floatCoefficients[0]);++i){DWORD value=*(DWORD*)floatCoefficients[i];if(save)snapshot[n]=value;else if(snapshot[n]!=value){Print(n,snapshot[n],value);return n+1;}++n;}
    return 0;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode){control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<8;++method)for(unsigned test=0;test<1024;++test){
            Setup(test);unsigned __int64 expected=Run(method,true);State(true);
            Setup(test);unsigned __int64 actual=Run(method,false);DWORD code=mode*100000+method*10000+test+1;
            if(expected!=actual){Print(code,(DWORD)expected,(DWORD)actual);Print(0,(DWORD)(expected>>32),(DWORD)(actual>>32));ExitProcess(code);}
            if(State(false)){Print(code,method,test);ExitProcess(code);}
        }
    }
    const char message[]="Marine refinement, UV, ripple and frame math matches MASM in 24,576 comparisons.\n";DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,0);ExitProcess(0);
}
