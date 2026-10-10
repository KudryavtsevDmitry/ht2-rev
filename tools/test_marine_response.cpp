#include "../build-test/marine-response-globals.h"
extern "C" double __cdecl MarineAngle_Sine(float),Reference_MarineAngle_Sine(float),MarineAngle_Cosine(float),Reference_MarineAngle_Cosine(float);
extern "C" void __fastcall MarineMesh_CreateMotionResponse(BYTE*,void*,DWORD,DWORD,DWORD);
extern "C" void __fastcall Reference_MarineMesh_CreateMotionResponse(BYTE*,void*,DWORD,DWORD,DWORD);
static DWORD object[32],owners[2][32],response[12],transitions[2][4],seed,testCase,allocCalls,allocSize,input[3];
static volatile DWORD expectedObject[32],expectedOwners[2][32],expectedResponse[12],expectedTransitions[2][4],expectedCoefficients[18];
static DWORD Next() {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
extern "C" void* __cdecl TestAllocate(size_t bytes)
{
    ++allocCalls;allocSize=bytes;
    if(testCase&64) {object[1]=(DWORD)owners[1];g_mc_6530ac+=0.25f;}
    return response;
}
static void Setup(unsigned test)
{
    seed=test+0x378a;testCase=test;allocCalls=allocSize=0;
    for(unsigned i=0;i<32;++i) {object[i]=Next();for(unsigned j=0;j<2;++j) owners[j][i]=Next();}
    for(unsigned i=0;i<12;++i) response[i]=Next();
    for(unsigned i=0;i<4;++i) for(unsigned j=0;j<2;++j) transitions[j][i]=Next();
    object[1]=(DWORD)owners[0];
    for(unsigned j=0;j<2;++j) owners[j][28]=(test&128)?0:(DWORD)transitions[j];
    for(unsigned i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);++i) *coefficients[i]=(float)((int)((test+i*7)%37)-18)/8.0f;
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    for(unsigned i=0;i<3;++i) input[i]=Next();
    if(test<121) {
        for(unsigned i=0;i<3;++i) input[i]=special[(test+i)%11];
        for(unsigned i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);++i) *(DWORD*)coefficients[i]=special[(test/11+i)%11];
    }
    for(unsigned i=0;i<720;++i) {
        DWORD sine=Next(),cosine=Next();
        if(test<121) {sine=special[(test+i)%11];cosine=special[(test/11+i)%11];}
        ((DWORD*)&g_testSine)[i]=sine;((DWORD*)&g_testCosine)[i]=cosine;
    }
}
static unsigned __int64 Run(unsigned method,bool reference)
{
    union {double value;unsigned __int64 bits;} result;
    if(method==0) {result.value=reference?Reference_MarineAngle_Sine(*(float*)input):MarineAngle_Sine(*(float*)input);return result.bits;}
    if(method==1) {result.value=reference?Reference_MarineAngle_Cosine(*(float*)input):MarineAngle_Cosine(*(float*)input);return result.bits;}
    if(reference) Reference_MarineMesh_CreateMotionResponse((BYTE*)object,0,input[0],input[1],input[2]);
    else MarineMesh_CreateMotionResponse((BYTE*)object,0,input[0],input[1],input[2]);
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
        for(unsigned method=0;method<3;++method) for(unsigned test=0;test<1024;++test) {
            Setup(test);unsigned __int64 expectedReturn=Run(method,true);DWORD expectedCalls=allocCalls,expectedSize=allocSize;
            for(unsigned i=0;i<32;++i) {expectedObject[i]=object[i];for(unsigned j=0;j<2;++j) expectedOwners[j][i]=owners[j][i];}
            for(unsigned i=0;i<12;++i) expectedResponse[i]=response[i];
            for(unsigned i=0;i<4;++i) for(unsigned j=0;j<2;++j) expectedTransitions[j][i]=transitions[j][i];
            for(unsigned i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);++i) expectedCoefficients[i]=*(DWORD*)coefficients[i];
            Setup(test);unsigned __int64 actualReturn=Run(method,false);
            DWORD code=100000*mode+10000*method+test+1;
            if(expectedReturn!=actualReturn) {Print(method,(DWORD)expectedReturn,(DWORD)actualReturn);Print(100,(DWORD)(expectedReturn>>32),(DWORD)(actualReturn>>32));ExitProcess(code);}
            if(allocCalls!=expectedCalls || allocSize!=expectedSize) ExitProcess(400001);
            for(unsigned i=0;i<32;++i) {
                if(expectedObject[i]!=object[i]) {Print(i,expectedObject[i],object[i]);ExitProcess(code);}
                for(unsigned j=0;j<2;++j) if(expectedOwners[j][i]!=owners[j][i]) {Print(100+j*32+i,expectedOwners[j][i],owners[j][i]);ExitProcess(code);}
            }
            for(unsigned i=0;i<12;++i) if(expectedResponse[i]!=response[i]) {Print(200+i,expectedResponse[i],response[i]);ExitProcess(code);}
            for(unsigned i=0;i<4;++i) for(unsigned j=0;j<2;++j) if(expectedTransitions[j][i]!=transitions[j][i]) ExitProcess(400002);
            for(unsigned i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);++i) if(expectedCoefficients[i]!=*(DWORD*)coefficients[i]) ExitProcess(400003);
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
