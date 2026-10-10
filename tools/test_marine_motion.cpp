#include "../build-test/marine-motion-globals.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
#define DECL(n,ret,args) extern "C" ret __fastcall n args; extern "C" ret __fastcall Reference_##n args;
DECL(MarineTask_UpdateMotion,void,(BYTE*,void*,const float*))
DECL(MarineTask_Frame,void,(BYTE*,void*,DWORD,DWORD))
DECL(MarineTransition_SetActive,DWORD,(BYTE*,void*,DWORD))
static BYTE state[8][256],application[1408],recorded[8000];
static volatile BYTE expected[8][256];
static DWORD vectorBits[3],trace[64],expectedTrace[64];
static unsigned variant,count;
static void Record(DWORD value) { trace[count++]=value; }
extern "C" unsigned __stdcall TestTime()
{ Record(1);if(variant&64) F(void*,state[0],0)=state[5];return 0xfedcba98; }
extern "C" void* TestTimePointer=(void*)TestTime;
extern "C" void __fastcall TestTick(BYTE* self,void*,DWORD time)
{
    Record(2);Record((DWORD)self);Record(time);
    if(variant&128) F(void*,state[0],0)=state[5];
}
extern "C" int __fastcall TestEmit(BYTE* self,void*)
{
    Record(3);Record((DWORD)self);
    if(variant&256) F(void*,self,4)=state[6];
    return 17;
}
extern "C" void __fastcall TestFinish(BYTE* self,void*,DWORD first,DWORD second)
{ Record(4);Record((DWORD)self);Record(first);Record(second); }
static void* table[16]={0,0,0,0,0,0,(void*)TestTick,0,0,0,0,0,0,0,0,(void*)TestFinish};
static void Print(const char* label,DWORD value)
{
    char line[80];unsigned n=0;while(*label) line[n++]=*label++;
    line[n++]='=';for(int i=7;i>=0;i--) line[n++]="0123456789abcdef"[(value>>(i*4))&15];
    line[n++]='\r';line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    variant=test;count=0;for(unsigned i=0;i<64;i++) trace[i]=0;
    for(unsigned i=0;i<8;i++) for(unsigned j=0;j<256;j++) state[i][j]=(BYTE)(test+i+j);
    g_marineApplicationState=application;g_marineRecordedState=recorded;
    F(DWORD,application,1300)=(test&1)?2:0;
    const double times[]={0,123.75,-123.75,4294967295.75,4294967296.75,1099511627776.75,-1099511627776.75};
    F(double,recorded,7968)=times[test%7];
    if(test%7==6) {F(DWORD,recorded,7968)=0x12345678;F(DWORD,recorded,7972)=0x7ff00000;}
    F(void*,state[0],0)=state[1];F(void*,state[1],84)=state[2];
    F(void*,state[5],84)=state[2];
    F(void*,state[2],0)=state[3];F(void*,state[3],112)=(test&2)?state[4]:0;
    F(void*,state[1],4)=state[6];F(void*,state[5],4)=state[7];
    F(void*,state[6],0)=F(void*,state[7],0)=table;
    for(unsigned i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);i++) *coefficients[i]=(float)((int)(test+i*3)%31-15)/8;
    g_mf_652f5c=-0.5f;g_mf_652f60=1.5f;g_mf_652f7c=2.0f;
    g_mf_652f58=0.3f;g_mf_653004=1.1f;g_mf_725db4=2.3f;
    g_mf_653000=0.02f;g_mf_64c338=0.6f;g_mf_64c350=1.5f;g_mf_64c250=1.0f;g_mf_652f80=0.9f;
    const DWORD values[]={0,0x80000000,0x3f000000,0x3fc00000,0xbf000000,0x3fa00000,0x7fc01234,0x7f801234,0x7f800000,0xff800000,0x00000001,0x7f7fffff};
    F(DWORD,state[2],28)=values[test%12];F(DWORD,state[2],32)=values[(test/12)%12];
    for(unsigned i=0;i<3;i++) vectorBits[i]=values[(test+i*5)%12];
    if(test>=256) {
        for(unsigned i=0;i<3;i++) { float v=(float)((int)(test*7+i*3)%61-30)/7;vectorBits[i]=*(DWORD*)&v; }
    }
    if(test==510 || test==511) for(unsigned i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);i++) *(DWORD*)coefficients[i]=(test==510)?0x7fc01234:0x7f801234;
    if(test&32) for(unsigned i=0;i<3;i++) F(DWORD,state[2],4+i*4)=vectorBits[i];
}
static DWORD Run(unsigned method,bool ref)
{
    if(method==0) {
        const float* position=(variant&32)?(float*)(state[2]+4):(float*)vectorBits;
        if(ref) Reference_MarineTask_UpdateMotion(state[0],0,position);else MarineTask_UpdateMotion(state[0],0,position);
    } else if(method==1) {
        if(ref) Reference_MarineTask_Frame(state[0],0,0x12345678,0x9abcdef0);else MarineTask_Frame(state[0],0,0x12345678,0x9abcdef0);
    } else return ref?Reference_MarineTransition_SetActive(state[4],0,variant):MarineTransition_SetActive(state[4],0,variant);
    return 0;
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD originalControl,control;__asm fnstcw originalControl
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;mode++) {
        control=(originalControl&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<3;method++) for(unsigned test=0;test<512;test++) {
            Setup(test);DWORD ref=Run(method,true);unsigned expectedCount=count;
            for(unsigned i=0;i<8;i++) for(unsigned j=0;j<256;j++) expected[i][j]=state[i][j];
            for(unsigned i=0;i<64;i++) expectedTrace[i]=trace[i];
            Setup(test);DWORD actual=Run(method,false);
            if(ref!=actual || count!=expectedCount) ExitProcess(1+method);
            for(unsigned i=0;i<8;i++) for(unsigned j=0;j<256;j++) if(expected[i][j]!=state[i][j]) {
                Print("mode",mode);Print("method",method);Print("test",test);Print("object",i);Print("byte",j);
                Print("expected",expected[i][j]);Print("actual",state[i][j]);ExitProcess(4);
            }
            for(unsigned i=0;i<64;i++) if(expectedTrace[i]!=trace[i]) {Print("method",method);Print("test",test);Print("trace",i);Print("expected",expectedTrace[i]);Print("actual",trace[i]);ExitProcess(5);}
        }
    }
    __asm fldcw originalControl
    ExitProcess(0);
}
