#include "../src/king.h"
extern "C" int _fltused=0;
extern "C" DWORD* __cdecl MarineGeometry_Interpolate(DWORD*,const DWORD*,const DWORD*,float);
extern "C" DWORD* __cdecl Reference_MarineGeometry_Interpolate(DWORD*,const DWORD*,const DWORD*,float);
extern "C" BYTE* __fastcall MarineQueue_Init(BYTE*,void*);
extern "C" BYTE* __fastcall Reference_MarineQueue_Init(BYTE*,void*);
static DWORD data[3][10],fractionBits,seed;
static volatile DWORD expected[3][10];
static BYTE queue[32];
static volatile BYTE expectedQueue[32];
static DWORD Next() { seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed; }
static void Print(const char* label,DWORD value)
{
    char line[80];unsigned n=0;
    while(*label) line[n++]=*label++;
    line[n++]='=';
    for(int i=7;i>=0;i--) line[n++]="0123456789abcdef"[(value>>(i*4))&15];
    line[n++]='\r';line[n++]='\n';DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    seed=test+0x57ab;
    for(unsigned i=0;i<3;i++) for(unsigned j=0;j<10;j++) {
        DWORD bits=Next();
        data[i][j]=(bits&0x807fffff)|(((bits>>23)%250+2)<<23);
    }
    const DWORD fractions[]={0,0x80000000,0x3dcccccd,0x3f000000,0x3f800000,0x40000000,0xbf800000,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    fractionBits=fractions[test%11];
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,0x00000001,0x007fffff,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test<121) {
        for(unsigned j=0;j<10;j++) { data[0][j]=special[test%11];data[1][j]=special[(test/11)%11]; }
    }
    if(test>=121 && test<132) {
        for(unsigned i=0;i<2;i++) for(unsigned j=3;j<6;j++) data[i][j]=0;
        data[0][3]=0x3f800000;data[1][3]=0xbf800000;
    }
    // Include NaN payloads in the four attributes copied through FLD/FSTP.
    if(test&16) for(unsigned j=6;j<10;j++) data[1][j]=special[(test+j)%11];
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD originalControl,control;
    __asm fnstcw originalControl
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;mode++) {
        control=(originalControl&~0x300)|precision[mode]|0x3f;
        __asm fldcw control
        for(unsigned test=0;test<256;test++) {
            for(unsigned i=0;i<32;i++) queue[i]=(BYTE)(i+test);
            BYTE* ref=Reference_MarineQueue_Init(queue,0);
            for(unsigned i=0;i<32;i++) expectedQueue[i]=queue[i];
            for(unsigned i=0;i<32;i++) queue[i]=(BYTE)(i+test);
            if(MarineQueue_Init(queue,0)!=ref) ExitProcess(1);
            for(unsigned i=0;i<32;i++) if(queue[i]!=expectedQueue[i]) ExitProcess(2);
        }
        for(unsigned alias=0;alias<3;alias++) for(unsigned test=0;test<1024;test++) {
            Setup(test);
            DWORD* ref=Reference_MarineGeometry_Interpolate(data[alias],data[0],data[1],*(float*)&fractionBits);
            for(unsigned i=0;i<3;i++) for(unsigned j=0;j<10;j++) expected[i][j]=data[i][j];
            Setup(test);
            DWORD* actual=MarineGeometry_Interpolate(data[alias],data[0],data[1],*(float*)&fractionBits);
            if(actual!=ref) ExitProcess(3);
            for(unsigned i=0;i<3;i++) for(unsigned j=0;j<10;j++) if(expected[i][j]!=data[i][j]) {
                Print("mode",mode);Print("alias",alias);Print("test",test);Print("buffer",i);Print("word",j);
                Print("expected",expected[i][j]);Print("actual",data[i][j]);
                ExitProcess(4);
            }
        }
    }
    __asm fldcw originalControl
    ExitProcess(0);
}
