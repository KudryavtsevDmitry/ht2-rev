#include "../build-test/marine-init-test-globals.h"
static volatile BYTE expected[40][64];
static void Setup(unsigned test)
{
    for(unsigned region=0;region<sizeof(regions)/sizeof(regions[0]);region++) {
        for(unsigned byte=0;byte<sizes[region];byte++) regions[region][byte]=(BYTE)(test+byte+region);
        if(sizes[region]==4) {
            float value=(float)((int)(test*7+region*3)%31-15)/8.0f;
            *(float*)regions[region]=value;
        }
    }
    if(test==62 || test==63) for(unsigned region=0;region<sizeof(regions)/sizeof(regions[0]);region++)
        if(sizes[region]==4) *(DWORD*)regions[region]=(test==62)?0x7fc01234:0x7f801234;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD originalControl, control;
    __asm fnstcw originalControl
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;mode++) {
        control=(originalControl & ~0x300) | precision[mode] | 0x3f;
        __asm fldcw control
        for(unsigned method=0;method<methodCount;method++) for(unsigned test=0;test<64;test++) {
            Setup(test); Run(method,true);
            for(unsigned region=0;region<sizeof(regions)/sizeof(regions[0]);region++)
                for(unsigned byte=0;byte<sizes[region];byte++) expected[region][byte]=regions[region][byte];
            Setup(test); Run(method,false);
            for(unsigned region=0;region<sizeof(regions)/sizeof(regions[0]);region++)
                for(unsigned byte=0;byte<sizes[region];byte++) if(expected[region][byte]!=regions[region][byte]) ExitProcess(10000*mode+100*method+test+1);
        }
    }
    __asm fldcw originalControl
    ExitProcess(0);
}
