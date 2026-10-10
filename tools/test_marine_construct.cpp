#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
int _fltused=0;
char g_marineNodeBaseVtable[1],g_marineTaskVtable[1],g_marineTaskSecondaryVtable[1],g_marineTaskFile[1],g_marineErrorFormat[1],g_marineDate[1];
char g_marineNodeName[]="MarineNode",g_marineCommonLibrary[]="common",g_marineRainNodeName[]="rain_node",g_marineZeroName[]="zero";
double g_marineTaskRadiusSquared;
BYTE* g_marineRecordedState;
DWORD g_mt_frameValue;
float g_mt_axisX,g_mt_axisY,g_mt_axisZ;
}
extern "C" BYTE* __fastcall MarineTask_Construct(BYTE*,void*,BYTE*);
extern "C" BYTE* __fastcall Reference_MarineTask_Construct(BYTE*,void*,BYTE*);
static BYTE task[256],world[2][3072],library[2][128],node[96],materials[2][5][124],pool[2][4];
static float matrix[12],alternate[12];
static volatile BYTE expectedTask[256],expectedNode[96],expectedPool[2][4];
static DWORD trace[256],expectedTrace[256];
static unsigned count,variant;
static void Record(DWORD v) { trace[count++]=v; }
static void Vector(const DWORD* p) { for(unsigned i=0;i<3;i++) Record(p[i]); }
void* operator new(size_t size)
{
    Record(1);Record(size);F(DWORD,task,116)=99;F(DWORD,task,132)=13;
    if(variant&256) g_mt_frameValue=variant+1;
    return (variant&1)?0:pool[0];
}
extern "C" BYTE* __fastcall TestBase(BYTE* self,void*,const char* name,DWORD value)
{ Record(2);Record((DWORD)self);Record((DWORD)name);Record(value);F(DWORD,self,40)=123;return self; }
extern "C" BYTE* __fastcall TestSecondary(BYTE* self,void*,const char* name)
{ Record(3);Record((DWORD)self);Record((DWORD)name);Record(F(DWORD,task,0));Record(F(DWORD,task,72));return self; }
extern "C" void __fastcall TestRegister(BYTE* self,void*,BYTE* secondary)
{ Record(4);Record((DWORD)self);Record((DWORD)secondary);Record(F(DWORD,task,0));Record(F(DWORD,secondary,0)); }
extern "C" void __fastcall TestSecondaryEnable(BYTE* self,void*,DWORD enabled)
{ Record(5);Record((DWORD)self);Record(enabled); }
extern "C" BYTE* __fastcall TestMeshCreate(BYTE* self,void*,DWORD x,DWORD y,DWORD z,DWORD a,DWORD b,DWORD c,DWORD d)
{
    Record(6);Record((DWORD)self);Record(x);Record(y);Record(z);Record(a);Record(b);Record(c);Record(d);
    F(DWORD,self,0)=0x11223344;return (variant&16)?pool[1]:self;
}
extern "C" float* __fastcall TestMatrix(BYTE* self,void*,float* out)
{ Record(7);Record((DWORD)self);for(unsigned i=0;i<12;i++) *(DWORD*)(out+i)=*(DWORD*)(matrix+i);return (variant&8)?alternate:out; }
extern "C" DWORD __fastcall TestRebuild(BYTE* self,void*,const DWORD* position,const DWORD* direction)
{
    Record(8);Record((DWORD)self);Vector(position);Vector(direction);
    if(variant&4) F(void*,task,112)=world[1];
    return 0;
}
extern "C" BYTE* __fastcall TestLibrary(BYTE* self,void*,const char* name)
{ Record(9);Record((DWORD)self);Record((DWORD)name);return library[self==world[1]?1:0]; }
extern "C" BYTE* __fastcall TestFindNode(BYTE* self,void*,const char* name)
{ Record(10);Record((DWORD)self);Record((DWORD)name);return node; }
extern "C" void __fastcall TestAttach(BYTE* self,void*,BYTE* source)
{ Record(11);Record((DWORD)self);Record((DWORD)source);F(DWORD,source,36)=variant*7;F(DWORD,source,44)=0xabcdef00; }
extern "C" void TestError(DWORD code,const char* format,const char* file,DWORD line,const char* date)
{
    Record(12);Record(code);Record((DWORD)format);Record((DWORD)file);Record(line);Record((DWORD)date);
    if(variant&128) F(void*,task,108)=pool[1];
}
extern "C" BYTE* __fastcall TestMode(BYTE* self,void*,DWORD material)
{ Record(13);Record((DWORD)self);Record(material);return self; }
static void Print(const char* label,DWORD value)
{
    char line[80];unsigned n=0;while(*label) line[n++]=*label++;
    line[n++]='=';for(int i=7;i>=0;i--) line[n++]="0123456789abcdef"[(value>>(i*4))&15];
    line[n++]='\r';line[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),line,n,&written,0);
}
static void Setup(unsigned test)
{
    variant=test;count=0;for(unsigned i=0;i<256;i++) trace[i]=0;
    for(unsigned i=0;i<256;i++) task[i]=(BYTE)(test+i);
    for(unsigned i=0;i<96;i++) node[i]=(BYTE)(test+i);
    for(unsigned i=0;i<2;i++) {
        for(unsigned j=0;j<4;j++) pool[i][j]=(BYTE)(test+i+j);
        F(DWORD,world[i],524)=(test&2)^(i?2:0);
        F(DWORD,library[i],72)=(test&64)?0:5;F(void*,library[i],76)=materials[i];
        for(unsigned j=0;j<5;j++) {
            char* name=(char*)materials[i][j];name[0]='n';name[1]='o';name[2]=0;
            if(j==(test+i)%6) {name[0]='z';name[1]='e';name[2]='r';name[3]='o';name[4]=0;}
        }
    }
    g_marineRecordedState=0;g_mt_frameValue=0x55667788;
    g_mt_axisX=(float)((int)(test%13)-6)/7;g_mt_axisY=(float)((int)(test%11)-5)/9;g_mt_axisZ=(float)((int)(test%7)-3)/8;
    for(unsigned i=0;i<12;i++) {matrix[i]=(float)((int)((test+i*7)%47)-23)/6;alternate[i]=(float)((int)((test+i*5)%31)-15)/8;}
    g_marineTaskRadiusSquared=(double)((int)(test%31)-5)/3;
    const DWORD special[]={0,0x80000000,0x00000001,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000};
    if(test>=480) for(unsigned i=0;i<12;i++) *(DWORD*)(matrix+i)=*(DWORD*)(alternate+i)=special[(test+i)%8];
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    WORD originalControl,control;__asm fnstcw originalControl
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;mode++) {
        control=(originalControl&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned test=0;test<512;test++) {
            Setup(test);BYTE* ref=Reference_MarineTask_Construct(task,0,world[0]);unsigned expectedCount=count;
            BYTE* expectedRecorded=g_marineRecordedState;DWORD expectedFrame=g_mt_frameValue;
            for(unsigned i=0;i<256;i++) expectedTask[i]=task[i];
            for(unsigned i=0;i<96;i++) expectedNode[i]=node[i];
            for(unsigned i=0;i<2;i++) for(unsigned j=0;j<4;j++) expectedPool[i][j]=pool[i][j];
            for(unsigned i=0;i<256;i++) expectedTrace[i]=trace[i];
            Setup(test);BYTE* actual=MarineTask_Construct(task,0,world[0]);
            if(ref!=actual || count!=expectedCount || expectedRecorded!=g_marineRecordedState || expectedFrame!=g_mt_frameValue) {Print("test",test);Print("expected count",expectedCount);Print("actual count",count);ExitProcess(1);}
            for(unsigned i=0;i<256;i++) if(expectedTask[i]!=task[i]) {Print("mode",mode);Print("test",test);Print("byte",i);Print("expected",expectedTask[i]);Print("actual",task[i]);ExitProcess(2);}
            for(unsigned i=0;i<96;i++) if(expectedNode[i]!=node[i]) ExitProcess(3);
            for(unsigned i=0;i<2;i++) for(unsigned j=0;j<4;j++) if(expectedPool[i][j]!=pool[i][j]) ExitProcess(4);
            for(unsigned i=0;i<256;i++) if(expectedTrace[i]!=trace[i]) {Print("mode",mode);Print("test",test);Print("trace",i);Print("expected",expectedTrace[i]);Print("actual",trace[i]);ExitProcess(5);}
        }
    }
    __asm fldcw originalControl
    ExitProcess(0);
}
