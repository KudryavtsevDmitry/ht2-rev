#include "../build-test/marinevertices-globals.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" void __fastcall MarineGrid_UpdateVertex(BYTE*,void*,const DWORD*,DWORD,DWORD);
extern "C" void __fastcall Reference_MarineGrid_UpdateVertex(BYTE*,void*,const DWORD*,DWORD,DWORD);
extern "C" DWORD __fastcall MarineGrid_EmitTriangle(BYTE*,void*,DWORD,DWORD,DWORD,BYTE*);
extern "C" DWORD __fastcall Reference_MarineGrid_EmitTriangle(BYTE*,void*,DWORD,DWORD,DWORD,BYTE*);
static DWORD object[64],vertices[2][768],indices[2][128],triangle[16],pools[2][16],point[2],seed,testCase;
static DWORD trace[1024],traceCount,addCount,canCount;
static DWORD Next(){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static DWORD Bits(float x){return *(DWORD*)&x;}
static void Trace(DWORD value){trace[traceCount++]=value;}
extern "C" DWORD __fastcall MarineParticlePool_CanAdd(BYTE* pool,void*)
{
    ++canCount;Trace(1);Trace((DWORD)pool);
    if(testCase&32){object[18]=Bits(1.25f);object[23]=Bits(3.5f);}
    return !(testCase&8);
}
extern "C" DWORD __fastcall MarineParticlePool_Add(BYTE* pool,void*,const DWORD* position,const DWORD* velocity)
{
    ++addCount;Trace(2);Trace((DWORD)pool);
    for(unsigned i=0;i<3;++i)Trace(position[i]);for(unsigned i=0;i<3;++i)Trace(velocity[i]);
    if(testCase&16){object[28]=(DWORD)pools[1];object[31]=(DWORD)vertices[1];object[35]=(DWORD)indices[1];}
    if(testCase&64){object[18]=Bits(0.75f);g_mv_normal.x=1.25f;g_mv_normal.y=2.5f;g_mv_normal.z=3.75f;}
    if(testCase&128) ((DWORD*)velocity)[2]=addCount;
    return addCount%3;
}
static void Setup(unsigned test)
{
    testCase=test;seed=test+0x8393;traceCount=addCount=canCount=0;
    for(unsigned i=0;i<64;++i)object[i]=Bits((float)((int)(Next()%83)-41)/8);
    for(unsigned i=0;i<2;++i){for(unsigned j=0;j<768;++j)vertices[i][j]=Next();for(unsigned j=0;j<128;++j)indices[i][j]=Next();for(unsigned j=0;j<16;++j)pools[i][j]=Next();}
    for(unsigned i=0;i<1024;++i)trace[i]=0;
    for(unsigned i=0;i<16;++i)triangle[i]=Next();
    for(unsigned i=0;i<sizeof(floatCoefficients)/sizeof(floatCoefficients[0]);++i)*floatCoefficients[i]=(float)((test+i*3)%47+1)/8;
    g_mv_one=1;g_mv_negativeOne=-1;g_mv_half=0.5f;g_mv_normalZ=-1;g_mv_epsilon=1e-5;g_mv_smallZ=0.01f;g_mv_uvScale=0.5f;
    g_mv_pending=10000;g_mv_foamHeight=-0.5f;g_angleScale=57.29578f;g_vertexZero=0;g_zeroVec.x=g_zeroVec.y=g_zeroVec.z=0;
    g_mv_flatNormal.x=0;g_mv_flatNormal.y=0;g_mv_flatNormal.z=1;
    g_mv_normal.x=1;g_mv_normal.y=2;g_mv_normal.z=3;
    point[0]=Next();point[1]=Next();object[10]=Next();
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test<121){
        for(unsigned i=0;i<sizeof(floatCoefficients)/sizeof(floatCoefficients[0]);++i)if(floatCoefficients[i]!=&g_mv_pending)*(DWORD*)floatCoefficients[i]=special[(test/11+i)%11];
        for(unsigned i=1;i<25;++i)if(i!=10)object[i]=special[(test+i)%11];
        g_mv_flatNormal.x=*(float*)&special[(test+1)%11];g_mv_flatNormal.y=*(float*)&special[(test+2)%11];g_mv_flatNormal.z=*(float*)&special[(test+3)%11];
    }
    for(unsigned i=0;i<720;++i){g_testSine[i]=(float)((int)((test+i)%31)-15)/16;g_testCosine[i]=(float)((int)((test+i*7)%31)-15)/16;}
    object[28]=(DWORD)pools[0];object[31]=(DWORD)vertices[0];object[32]=test%3;object[33]=(DWORD)indices[0];object[34]=test%3;object[35]=(DWORD)indices[0];
    triangle[9]=test%13;
    if(test&256)*(DWORD*)&g_mv_pending=0x7fc12345;
    for(unsigned i=1;i<8;++i){
        vertices[0][i*12]=(test&(1u<<(i-1)))?Bits(5.25f):Bits(g_mv_pending);
        if(test<11)vertices[0][i*12]=special[test];
        vertices[0][i*12+1]=Bits((float)((int)((test+i*7)%31)-15));vertices[0][i*12+2]=Bits((float)((int)((test+i*11)%31)-15));
    }
}
static DWORD Run(unsigned method,bool reference)
{
    BYTE* self=(BYTE*)object;const DWORD* coordinates=(testCase&256)?object+1:point;
    if(testCase&512)coordinates=(DWORD*)&g_mv_normal;
    DWORD a=(1u+testCase%7)|(testCase<<16),b=(testCase&32)?a:(1u+(testCase+2)%7)|(testCase<<16),c=(testCase&64)?b:(1u+(testCase+4)%7)|(testCase<<16);
    if(method==0){if(reference)Reference_MarineGrid_UpdateVertex(self,0,coordinates,a,triangle[9]);else MarineGrid_UpdateVertex(self,0,coordinates,a,triangle[9]);}
    else return reference?Reference_MarineGrid_EmitTriangle(self,0,a,b,c,(BYTE*)triangle):MarineGrid_EmitTriangle(self,0,a,b,c,(BYTE*)triangle);
    return 0;
}
static DWORD snapshot[4096];
static void Print(DWORD a,DWORD b,DWORD c){char text[28];DWORD args[]={a,b,c};unsigned n=0;for(unsigned j=0;j<3;++j){for(int i=7;i>=0;--i)text[n++]="0123456789abcdef"[(args[j]>>(i*4))&15];text[n++]=' ';}text[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,n,&written,0);}
static unsigned State(bool save)
{
    DWORD* regions[]={object,(DWORD*)vertices,(DWORD*)indices,triangle,(DWORD*)pools,point,trace,&traceCount,&addCount,&canCount,(DWORD*)&g_mv_normal};unsigned sizes[]={64,1536,256,16,32,2,1024,1,1,1,3};unsigned n=0;
    for(unsigned r=0;r<11;++r)for(unsigned i=0;i<sizes[r];++i){if(save)snapshot[n]=regions[r][i];else if(snapshot[n]!=regions[r][i]){Print(n,snapshot[n],regions[r][i]);return n+1;}++n;}
    for(unsigned i=0;i<sizeof(floatCoefficients)/sizeof(floatCoefficients[0]);++i){DWORD value=*(DWORD*)floatCoefficients[i];if(save)snapshot[n]=value;else if(snapshot[n]!=value){Print(n,snapshot[n],value);return n+1;}++n;}
    return 0;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode){control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<2;++method)for(unsigned test=0;test<1024;++test){Setup(test);DWORD expectedReturn=Run(method,true);State(true);Setup(test);DWORD actualReturn=Run(method,false);DWORD code=mode*100000+method*10000+test+1;if(expectedReturn!=actualReturn){Print(code,expectedReturn,actualReturn);ExitProcess(code);}
            if(State(false)){Print(code,method,test);ExitProcess(code);}}
    }
    const char message[]="Marine wave vertices and indexed output match MASM in 6,144 full-state and emission comparisons.\n";DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,0);ExitProcess(0);
}
