#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" { char g_marineBaseVtable[1],g_marineTreeFile[1],g_marineErrorFormat[1],g_marineDate[1]; }
#define DECL(n,ret,args) extern "C" ret __fastcall n args; extern "C" ret __fastcall Reference_##n args;
DECL(MarineGreed_Destruct,void,(BYTE*,void*))
DECL(MarineGreed_Delete,BYTE*,(BYTE*,void*,DWORD))
DECL(MarineTree_Delete,BYTE*,(BYTE*,void*,DWORD))
DECL(MarineVertex_Delete,BYTE*,(BYTE*,void*,DWORD))
static BYTE self[88],nodes[7][12],owners[3][8],queues[3][12],vertices[8][12];
static BYTE* vertexTable[8];
static BYTE* edgeTable[3];
static volatile BYTE expected[sizeof(self)+sizeof(nodes)+sizeof(owners)+sizeof(queues)+sizeof(vertices)+sizeof(vertexTable)+sizeof(edgeTable)];
static DWORD trace[256],expectedTrace[256];
static unsigned count,variant,method;
static void Record(DWORD value) { trace[count++]=value; }
void operator delete(void* p)
{
    Record(1);Record((DWORD)p);
    // Exercise the original destructor's reload of the vertex count and queues.
    if(method<2 && (variant&512) && p==vertices[0]) F(WORD,self,36)=1;
    if(method<2 && (variant&512) && p==queues[0]) F(void*,self,32)=queues[2];
}
extern "C" void TestFree(void* p) { Record(2);Record((DWORD)p); }
extern "C" void __fastcall TestQueueDestruct(BYTE* p,void*) { Record(3);Record((DWORD)p); }
extern "C" void TestError(DWORD code,const char* format,const char* file,int line,const char* date)
{ Record(4);Record(code);Record((DWORD)format);Record((DWORD)file);Record(line);Record((DWORD)date); }
static void Fill(BYTE* p,unsigned size,unsigned seed) { for(unsigned i=0;i<size;i++) p[i]=(BYTE)(seed+i); }
static void Setup(unsigned test)
{
    variant=test;count=0;
    Fill(self,sizeof(self),test);Fill((BYTE*)nodes,sizeof(nodes),test+1);
    Fill((BYTE*)owners,sizeof(owners),test+2);Fill((BYTE*)queues,sizeof(queues),test+3);
    Fill((BYTE*)vertices,sizeof(vertices),test+4);
    for(unsigned i=0;i<256;i++) trace[i]=0;
    F(void*,self,28)=(test&64)?queues[0]:0;
    F(void*,self,32)=(test&128)?queues[1]:0;
    F(void*,self,12)=(test&256)?0:vertexTable;
    F(void*,self,8)=(test&512)?0:edgeTable;
    F(WORD,self,36)=(WORD)(test%9);F(DWORD,self,48)=test%4;
    for(unsigned i=0;i<8;i++) vertexTable[i]=(test&(1<<i))?vertices[i]:0;
    for(unsigned i=0;i<7;i++) F(void*,nodes[i],0)=F(void*,nodes[i],4)=F(void*,nodes[i],8)=0;
    for(unsigned i=0;i<3;i++) {
        edgeTable[i]=(test&(1<<(i+3)))?nodes[i]:0;
        F(void*,owners[i],0)=F(void*,owners[i],4)=0;
        switch((test+i)%4) {
        case 0:F(void*,nodes[i],8)=0;break;
        case 1:F(void*,nodes[i],8)=owners[i];F(void*,owners[i],0)=nodes[i];break;
        case 2:F(void*,nodes[i],8)=owners[i];F(void*,owners[i],4)=nodes[i];break;
        default:F(void*,nodes[i],8)=owners[i];break; // malformed backlink: error and continue
        }
    }
    for(unsigned i=3;i<7;i++) {
        unsigned root=(i-3)/2,slot=((i-3)%2)*4;
        if(i==6 && (test&512)) root=3; // include a third level of ownership
        if(test&(1<<(i+1))) { F(void*,nodes[root],slot)=nodes[i];F(void*,nodes[i],8)=nodes[root]; }
    }
}
static DWORD Run(bool ref)
{
    switch(method) {
    case 0:if(ref) Reference_MarineGreed_Destruct(self,0);else MarineGreed_Destruct(self,0);return 0;
    case 1:return (DWORD)(ref?Reference_MarineGreed_Delete(self,0,variant):MarineGreed_Delete(self,0,variant));
    case 2:return (DWORD)(ref?Reference_MarineTree_Delete(nodes[variant%3],0,variant):MarineTree_Delete(nodes[variant%3],0,variant));
    default:return (DWORD)(ref?Reference_MarineVertex_Delete(vertices[0],0,variant):MarineVertex_Delete(vertices[0],0,variant));
    }
}
static unsigned SaveOrCompare(unsigned offset,const BYTE* p,unsigned size,bool save)
{
    for(unsigned i=0;i<size;i++) {
        if(save) expected[offset+i]=p[i];
        else if(expected[offset+i]!=p[i]) ExitProcess(20+method);
    }
    return offset+size;
}
static void CheckState(bool save)
{
    unsigned n=SaveOrCompare(0,self,sizeof(self),save);
    n=SaveOrCompare(n,(BYTE*)nodes,sizeof(nodes),save);
    n=SaveOrCompare(n,(BYTE*)owners,sizeof(owners),save);
    n=SaveOrCompare(n,(BYTE*)queues,sizeof(queues),save);
    n=SaveOrCompare(n,(BYTE*)vertices,sizeof(vertices),save);
    n=SaveOrCompare(n,(BYTE*)vertexTable,sizeof(vertexTable),save);
    SaveOrCompare(n,(BYTE*)edgeTable,sizeof(edgeTable),save);
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    for(method=0;method<4;method++) for(unsigned test=0;test<1024;test++) {
        Setup(test);DWORD ref=Run(true);unsigned expectedCount=count;CheckState(true);
        for(unsigned i=0;i<256;i++) expectedTrace[i]=trace[i];
        Setup(test);DWORD actual=Run(false);CheckState(false);
        if(ref!=actual || count!=expectedCount) ExitProcess(1+method);
        for(unsigned i=0;i<256;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(40+method);
    }
    ExitProcess(0);
}
