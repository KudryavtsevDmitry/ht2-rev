#include "../src/king.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" int _fltused=0;
extern "C" float g_mq_zero=0;
extern "C" DWORD __fastcall MarineQueue_SetDeferredPriority(BYTE*,void*,BYTE*,DWORD);
extern "C" DWORD __fastcall Reference_MarineQueue_SetDeferredPriority(BYTE*,void*,BYTE*,DWORD);
extern "C" DWORD __fastcall MarineQueue_InsertDeferredNode(BYTE*,void*,BYTE*);
extern "C" DWORD __fastcall Reference_MarineQueue_InsertDeferredNode(BYTE*,void*,BYTE*);
extern "C" BYTE* __fastcall MarineQueue_TopDeferredNode(BYTE*,void*);
extern "C" BYTE* __fastcall Reference_MarineQueue_TopDeferredNode(BYTE*,void*);
static DWORD nodes[12][28],list[3],score;
static volatile DWORD expectedNodes[12][28],expectedList[3];
static void Setup(unsigned a,unsigned b,unsigned pattern,unsigned variant)
{
    for(unsigned i=0;i<12;++i) {
        for(unsigned j=0;j<28;++j) nodes[i][j]=i*117+j*91+pattern;
        nodes[i][1]=nodes[i][2]=(DWORD)(nodes[i]+1);
    }
    for(unsigned side=0;side<2;++side) {
        unsigned length=side?b:a,start=side?5:0;
        for(unsigned i=0;i<length;++i) {
            DWORD* node=nodes[start+i];
            node[1]=(DWORD)(nodes[start+(i+1)%length]+1);node[2]=(DWORD)(nodes[start+(i+length-1)%length]+1);
            *(float*)(node+7)=(float)((length-i)*(pattern%3+1)+(side?(int)(pattern%7)-3:0));
            if(pattern==7) node[7]=0x3f800000;
            if(pattern==8) node[7]=0;
            if(pattern==9) node[7]=0x80000000;
            if(pattern>=10 && i==(pattern-10)%5) node[7]=side?0x7fc01234:0x7f801234;
            if(pattern==16) node[7]=side?0xff800000:0x7f800000;
            if(pattern==17) node[7]=0x7fc01234;
        }
    }
    list[0]=a?(DWORD)nodes[0]:0;list[1]=b?(DWORD)nodes[5]:0;
    const DWORD counts[]={0,498,499,500,0xffffffff};list[2]=counts[variant];
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc01234,0x7f801234};
    if(pattern<8) score=special[pattern];else {float value=(float)((int)pattern-10);score=*(DWORD*)&value;}
    nodes[10][7]=score;
}
static DWORD Run(unsigned method,bool reference,unsigned a,unsigned b,unsigned variant)
{
    BYTE* self=(BYTE*)list;BYTE* node=(BYTE*)nodes[(a+b+variant)%11];
    if(method==0) return reference?Reference_MarineQueue_SetDeferredPriority(self,0,node,score):MarineQueue_SetDeferredPriority(self,0,node,score);
    if(method==1) return reference?Reference_MarineQueue_InsertDeferredNode(self,0,(BYTE*)nodes[10]):MarineQueue_InsertDeferredNode(self,0,(BYTE*)nodes[10]);
    return (DWORD)(reference?Reference_MarineQueue_TopDeferredNode(self,0):MarineQueue_TopDeferredNode(self,0));
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
        for(unsigned method=0;method<3;++method) for(unsigned a=0;a<6;++a) for(unsigned b=0;b<6;++b) for(unsigned pattern=0;pattern<18;++pattern) for(unsigned variant=0;variant<5;++variant) {
            Setup(a,b,pattern,variant);DWORD expectedReturn=Run(method,true,a,b,variant);
            for(unsigned i=0;i<3;++i) expectedList[i]=list[i];
            for(unsigned i=0;i<12;++i) for(unsigned j=0;j<28;++j) expectedNodes[i][j]=nodes[i][j];
            Setup(a,b,pattern,variant);DWORD actualReturn=Run(method,false,a,b,variant);
            DWORD code=100000*mode+10000*method+a*1000+b*100+pattern*5+variant+1;
            if(expectedReturn!=actualReturn) {Print(method,expectedReturn,actualReturn);ExitProcess(code);}
            for(unsigned i=0;i<3;++i) if(expectedList[i]!=list[i]) {Print(i,expectedList[i],list[i]);ExitProcess(code);}
            for(unsigned i=0;i<12;++i) for(unsigned j=0;j<28;++j) if(expectedNodes[i][j]!=nodes[i][j]) {Print(100+i*28+j,expectedNodes[i][j],nodes[i][j]);ExitProcess(code);}
        }
    }
    __asm fldcw original
    ExitProcess(0);
}
