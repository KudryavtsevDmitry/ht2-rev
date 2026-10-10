#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" { int _fltused=0;float g_mt_axisX,g_mt_axisY,g_mt_axisZ;BYTE* g_mt_motionState;DWORD g_mt_frameValue; }
#define DECL(n,ret,args) extern "C" ret __fastcall n args; extern "C" ret __fastcall Reference_##n args;
DECL(MarineTask_DeferredBounds,void,(BYTE*,void*))
DECL(MarineTask_DispatchUpdate,DWORD,(BYTE*,void*,DWORD,BYTE*))
static BYTE task[256],matrixSource[600],motion[560],owner[2][256],event[256];
static float matrix[12],alternate[12];
static DWORD trace[128],expectedTrace[128];
static volatile BYTE expected[256];
static unsigned count,variant,method;
static void Record(DWORD v) { trace[count++]=v; }
static void Vector(const float* v) { for(unsigned i=0;i<3;i++) Record(*(DWORD*)(v+i)); }
extern "C" float* __fastcall TestMatrix(BYTE* self,void*,float* out)
{
    Record(1);Record((DWORD)self);for(unsigned i=0;i<12;i++) *(DWORD*)(out+i)=*(DWORD*)(matrix+i);
    if(variant&512) F(DWORD,task,128)=1;
    return (variant&64)?alternate:out;
}
extern "C" void __fastcall TestEnable(BYTE* self,void*,DWORD enabled)
{ Record(2);Record((DWORD)self);Record(enabled); }
extern "C" DWORD __fastcall TestQuery(BYTE* self,void*,const float* bounds)
{
    Record(3);Record((DWORD)self);Vector(bounds);Record(*(DWORD*)(bounds+3));
    if(variant&16) F(DWORD,task,116)=F(DWORD,task,116)?0:1;
    return (variant&2)?3:0;
}
extern "C" void __fastcall TestAdjust(BYTE* self,void*,const float* position,const float* direction,BYTE* object)
{
    Record(4);Record((DWORD)self);Vector(position);Vector(direction);Record((DWORD)object);
    if(variant&128) { F(DWORD,task,120)=1;F(DWORD,task,124)=1;F(void*,task,108)=owner[1]; }
}
extern "C" int __fastcall TestBounds(BYTE* self,void*,const DWORD* position,const DWORD* direction)
{
    Record(5);Record((DWORD)self);Vector((float*)position);Vector((float*)direction);
    if(variant&256) {
        if(method==0) F(DWORD,task,56)=19;
        else F(void*,task,108)=owner[1];
    }
    return 1;
}
extern "C" void __fastcall TestMotion(BYTE* self,void*,const float* value)
{
    Record(6);Record((DWORD)self);Vector(value);
    if(variant&512) { F(void*,task,108)=owner[0];F(DWORD,task,124)=1; }
}
extern "C" void __fastcall TestFrame(BYTE* self,void*,DWORD first,DWORD second)
{ Record(7);Record((DWORD)self);Record(first);Record(second); }
static void Print(const char* label,DWORD value)
{
    char line[80];unsigned n=0;while(*label) line[n++]=*label++;
    line[n++]='=';for(int i=7;i>=0;i--) line[n++]="0123456789abcdef"[(value>>(i*4))&15];
    line[n++]='\r';line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    variant=test;count=0;for(unsigned i=0;i<128;i++) trace[i]=0;
    for(unsigned i=0;i<256;i++) task[i]=(BYTE)(test+i);
    F(void*,task,32)=owner[0];F(void*,task,36)=matrixSource;
    F(void*,task,108)=owner[0];F(void*,task,112)=matrixSource;
    F(DWORD,task,116)=test&1;F(DWORD,task,120)=test&4;F(DWORD,task,124)=test&8;F(DWORD,task,128)=test&32;
    F(void*,matrixSource,464)=(test&4)?event:0;
    const DWORD counters[]={0,1,19,20,21,0x7fffffff,0x80000000,0xffffffff};F(DWORD,task,56)=counters[test%8];
    for(unsigned i=0;i<4;i++) F(float,task,136+i*4)=(float)(test+i)/7;
    g_mt_motionState=motion;g_mt_frameValue=0xaabbccdd;
    g_mt_axisX=(float)((int)(test%13)-6)/7;g_mt_axisY=(float)((int)(test%11)-5)/9;g_mt_axisZ=(float)((int)(test%7)-3)/8;
    F(float,motion,532)=(float)((int)(test%53)-26)/3;
    for(unsigned i=0;i<12;i++) { matrix[i]=(float)((int)((test+i*7)%47)-23)/6;alternate[i]=(float)((int)((test+i*5)%31)-15)/8; }
    const DWORD special[]={0,0x80000000,0x00000001,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test>=960) {
        for(unsigned i=0;i<12;i++) *(DWORD*)(matrix+i)=*(DWORD*)(alternate+i)=special[(test+i)%8];
        *(DWORD*)&g_mt_axisX=special[test%8];*(DWORD*)&g_mt_axisY=special[(test+1)%8];*(DWORD*)&g_mt_axisZ=special[(test+2)%8];
    }
    if(test>=900 && test<916) g_mt_axisX=g_mt_axisY=g_mt_axisZ=0;
}
static DWORD Run(bool ref)
{
    if(method==0) {if(ref) Reference_MarineTask_DeferredBounds(task,0);else MarineTask_DeferredBounds(task,0);return 0;}
    return ref?Reference_MarineTask_DispatchUpdate(task,0,0x1234,event):MarineTask_DispatchUpdate(task,0,0x1234,event);
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD originalControl,control;__asm fnstcw originalControl
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;mode++) {
        control=(originalControl&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(method=0;method<2;method++) for(unsigned test=0;test<1024;test++) {
            Setup(test);DWORD ref=Run(true);unsigned expectedCount=count;
            for(unsigned i=0;i<256;i++) expected[i]=task[i];
            for(unsigned i=0;i<128;i++) expectedTrace[i]=trace[i];
            Setup(test);DWORD actual=Run(false);
            if(ref!=actual || count!=expectedCount) {Print("method",method);Print("test",test);Print("expected return",ref);Print("actual return",actual);Print("expected calls",expectedCount);Print("actual calls",count);ExitProcess(1);}
            for(unsigned i=0;i<256;i++) if(expected[i]!=task[i]) {Print("method",method);Print("test",test);Print("byte",i);Print("expected",expected[i]);Print("actual",task[i]);ExitProcess(2);}
            for(unsigned i=0;i<128;i++) if(expectedTrace[i]!=trace[i]) {Print("mode",mode);Print("method",method);Print("test",test);Print("trace",i);Print("expected",expectedTrace[i]);Print("actual",trace[i]);ExitProcess(3);}
        }
    }
    __asm fldcw originalControl
    ExitProcess(0);
}
