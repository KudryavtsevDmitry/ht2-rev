#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
int _fltused=0;
float g_mq_zero,g_mq_one,g_mq_half,g_mq_delay,g_mq_offsetScale,g_mq_height,g_mq_lowerHeight;
DWORD g_mt_frameValue;
}
#define DECL(n,ret,args) extern "C" ret __fastcall n args; extern "C" ret __fastcall Reference_##n args;
DECL(MarineTask_DrawPending,void,(BYTE*,void*,const float*,const float*,BYTE*))
DECL(MarineCamera_HorizontalAngle,double,(BYTE*,void*))
DECL(MarineCamera_VerticalAngle,double,(BYTE*,void*))
static BYTE task[256],world[3072],event[8600],camera[96],device[2][4];
static DWORD position[3],direction[3],trace[128],expectedTrace[128];
static volatile BYTE expectedTask[256];
static unsigned count,variant;
static void Record(DWORD v) { trace[count++]=v; }
extern "C" void __fastcall TestBind(BYTE* self,void*,DWORD material,DWORD enable,DWORD a,DWORD b)
{
    Record(1);Record((DWORD)self);Record(material);Record(enable);Record(a);Record(b);
    if(variant&128) g_mt_frameValue=(DWORD)device[1];
}
extern "C" DWORD __stdcall TestState(BYTE* self,DWORD state,DWORD value)
{
    Record(2);Record((DWORD)self);Record(state);Record(value);
    if(variant&256) g_mt_frameValue=(DWORD)device[0];
    return 0;
}
extern "C" DWORD __stdcall TestDraw(BYTE* self,DWORD kind,DWORD format,const DWORD* data,DWORD number,DWORD flags)
{
    Record(3);Record((DWORD)self);Record(kind);Record(format);Record(number);Record(flags);
    for(unsigned i=0;i<32;i++) Record(data[i]);
    return 0;
}
static void* table[26];
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
    F(void*,task,112)=world;F(void*,world,488)=camera;
    F(DWORD,task,116)=test&1;F(DWORD,task,152)=0x12345678;
    table[20]=(void*)TestState;table[25]=(void*)TestDraw;
    F(void*,device[0],0)=F(void*,device[1],0)=table;g_mt_frameValue=(DWORD)device[0];
    const DWORD values[]={0,0x80000000,0xbf800000,0x3e800000,0x3f800000,0x40a00000,0x41200000,0x7fc01234,0x7f801234,0x7f800000,0xff800000,0x00000001};
    F(DWORD,task,156)=values[test%12];
    F(double,world,2960)=(double)((int)(test%17)-5)/8;
    F(double,event,8472)=(double)((int)(test%53)-4)/7;
    F(float,camera,88)=(float)((int)(test%37)-18)/4;
    F(float,camera,84)=(float)((int)(test%31)-15)/5;
    g_mq_zero=0;g_mq_one=1;g_mq_half=0.5f;
    g_mq_delay=5;g_mq_offsetScale=0.65f;g_mq_height=250;g_mq_lowerHeight=10;
    for(unsigned i=0;i<3;i++) {
        float p=(float)((int)((test+i*7)%47)-23)/6,d=(float)((int)((test+i*5)%31)-15)/8;
        position[i]=*(DWORD*)&p;direction[i]=*(DWORD*)&d;
    }
    if(test&32) g_mq_delay=(float)((int)(test%5)-2);
    if(test>=900) {
        F(DWORD,camera,88)=values[(test+3)%12];F(DWORD,camera,84)=values[(test+5)%12];
        for(unsigned i=0;i<3;i++) {position[i]=values[(test+i)%12];direction[i]=values[(test+i+4)%12];}
    }
    if(test==1022 || test==1023) {
        const DWORD nan=(test==1022)?0x7fc01234:0x7f801234;
        *(DWORD*)&g_mq_half=*(DWORD*)&g_mq_offsetScale=*(DWORD*)&g_mq_height=*(DWORD*)&g_mq_lowerHeight=nan;
    }
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD originalControl,control;__asm fnstcw originalControl
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;mode++) {
        control=(originalControl&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<3;method++) for(unsigned test=0;test<1024;test++) {
            Setup(test);double ref=0;
            if(method==0) Reference_MarineTask_DrawPending(task,0,(float*)position,(float*)direction,event);
            else if(method==1) ref=Reference_MarineCamera_HorizontalAngle(camera,0);
            else ref=Reference_MarineCamera_VerticalAngle(camera,0);
            unsigned expectedCount=count;DWORD expectedDevice=g_mt_frameValue;
            for(unsigned i=0;i<256;i++) expectedTask[i]=task[i];
            for(unsigned i=0;i<128;i++) expectedTrace[i]=trace[i];
            Setup(test);double actual=0;
            if(method==0) MarineTask_DrawPending(task,0,(float*)position,(float*)direction,event);
            else if(method==1) actual=MarineCamera_HorizontalAngle(camera,0);
            else actual=MarineCamera_VerticalAngle(camera,0);
            if(count!=expectedCount || expectedDevice!=g_mt_frameValue) {Print("method",method);Print("test",test);ExitProcess(1);}
            for(unsigned i=0;i<2;i++) if(F(DWORD,&ref,i*4)!=F(DWORD,&actual,i*4)) {Print("mode",mode);Print("method",method);Print("test",test);Print("expected",F(DWORD,&ref,i*4));Print("actual",F(DWORD,&actual,i*4));ExitProcess(2);}
            for(unsigned i=0;i<256;i++) if(expectedTask[i]!=task[i]) {Print("mode",mode);Print("test",test);Print("byte",i);Print("expected",expectedTask[i]);Print("actual",task[i]);ExitProcess(3);}
            for(unsigned i=0;i<128;i++) if(expectedTrace[i]!=trace[i]) {Print("mode",mode);Print("test",test);Print("trace",i);Print("expected",expectedTrace[i]);Print("actual",trace[i]);ExitProcess(4);}
        }
    }
    __asm fldcw originalControl
    ExitProcess(0);
}
