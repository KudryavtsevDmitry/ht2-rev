#include "../src/king.h"
#define F(t,p,o) (*(t*)((BYTE*)(p)+(o)))
extern "C" {
char g_marineMeshVtable[1],g_marineTaskVtable[1],g_marineTaskSecondaryVtable[1];
int _fltused=0;
}
#define DECL(n,ret,args) extern "C" ret __fastcall n args; extern "C" ret __fastcall Reference_##n args;
DECL(MarineMesh_Create,BYTE*,(BYTE*,void*,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD))
DECL(MarineMesh_Destruct,void,(BYTE*,void*)) DECL(MarineMesh_Delete,BYTE*,(BYTE*,void*,DWORD))
DECL(MarineTask_Destruct,void,(BYTE*,void*)) DECL(MarineTask_Delete,BYTE*,(BYTE*,void*,DWORD))
DECL(MarineTask_DeleteSecondary,BYTE*,(BYTE*,void*,DWORD))
DECL(MarineGeometry_Snapshot,DWORD*,(BYTE*,void*,DWORD*,BYTE*))
DECL(MarineTransition_Create,BYTE*,(BYTE*,void*,DWORD,BYTE*,const DWORD*,const DWORD*,unsigned,BYTE*))
DECL(MarineTransition_Advance,int,(BYTE*,void*,DWORD))
static BYTE state[12][256],pool[88];
static volatile BYTE expected[12][256],expectedPool[88];
static DWORD trace[128],expectedTrace[128],a[10],b[10];
static unsigned count,variant;
static void Record(DWORD value) { trace[count++]=value; }
void* operator new(size_t n) { Record(1); Record(n); return pool; }
void operator delete(void* p) { Record(2); Record((DWORD)p); }
extern "C" unsigned __stdcall TestTime() { Record(3); return 1234; }
extern "C" void* TestTimePointer=(void*)TestTime;
extern "C" void __fastcall TestSetup(BYTE* mesh,void*,const DWORD* position,DWORD a,DWORD b,DWORD c)
{ Record(4); Record((DWORD)mesh); for(unsigned i=0;i<3;i++) Record(position[i]); Record(a);Record(b);Record(c); }
extern "C" void __fastcall TestMotion(BYTE* mesh,void*,DWORD x,DWORD y,DWORD z) { Record(5);Record((DWORD)mesh);Record(x);Record(y);Record(z); }
extern "C" void __fastcall TestMeshBase(BYTE* self,void*) { Record(6);Record((DWORD)self); }
extern "C" void __fastcall TestDetach(BYTE* self,void*) { Record(7);Record((DWORD)self); }
extern "C" void __fastcall TestSecondaryBase(BYTE* self,void*) { Record(8);Record((DWORD)self); }
extern "C" void __fastcall TestTaskBase(BYTE* self,void*) { Record(9);Record((DWORD)self); }
extern "C" void __fastcall TestDelete(BYTE* self,void*,DWORD flag) { Record(10);Record((DWORD)self);Record(flag); }
static void* table[]={ (void*)TestDelete };
extern "C" DWORD* TestInterpolate(DWORD* out,const DWORD* from,const DWORD* to,float fraction)
{
    Record(11);Record(F(DWORD,&fraction,0));
    for(unsigned i=0;i<10;i++) { Record(from[i]);Record(to[i]);out[i]=from[i]^to[i]^F(DWORD,&fraction,0); }
    if(variant&16) {
        F(void*,state[0],16)=state[11]; F(void*,state[11],0)=state[5]; F(WORD,state[5],0)=5;
        F(void*,state[0],100)=state[8]; F(void*,state[8],16)=state[9];
    }
    return out;
}
static void Setup(unsigned test)
{
    variant=test;count=0;
    for(unsigned i=0;i<128;i++) trace[i]=0;
    for(unsigned i=0;i<12;i++) for(unsigned j=0;j<256;j++) state[i][j]=(BYTE)(i+j+test);
    for(unsigned i=0;i<88;i++) pool[i]=(BYTE)(i+test);
    for(unsigned i=0;i<10;i++) { a[i]=i+test;b[i]=0xff00ff00+i+test; }
    F(void*,state[0],108)=(test&1)?state[2]:0; F(void*,state[2],0)=state[3];
    F(void*,state[0],72)=(test&2)?state[5]:0;
    F(void*,state[1],4)=(test&1)?state[4]:0; F(void*,state[1],84)=(test&2)?state[5]:0;
    F(void*,state[3],0)=F(void*,state[4],0)=table;
    F(void*,state[0],16)=state[6];F(void*,state[6],0)=state[10];F(WORD,state[10],0)=3;
    F(void*,state[0],100)=state[7];F(void*,state[7],16)=state[8];
    const DWORD starts[]={0,7,0x7fffffff,0x80000000,0xffffffff};
    F(DWORD,state[0],8)=starts[test%5];F(DWORD,state[0],12)=test%4==0?0:5;
}
static DWORD Run(unsigned method,bool ref)
{
    switch(method) {
    case 0:return (DWORD)(ref?Reference_MarineMesh_Create(state[0],0,1,2,3,4,5,6,7):MarineMesh_Create(state[0],0,1,2,3,4,5,6,7));
    case 1:if(ref) Reference_MarineMesh_Destruct(state[1],0);else MarineMesh_Destruct(state[1],0);return 0;
    case 2:return (DWORD)(ref?Reference_MarineMesh_Delete(state[1],0,variant):MarineMesh_Delete(state[1],0,variant));
    case 3:if(ref) Reference_MarineTask_Destruct(state[0],0);else MarineTask_Destruct(state[0],0);return 0;
    case 4:return (DWORD)(ref?Reference_MarineTask_Delete(state[0],0,variant):MarineTask_Delete(state[0],0,variant));
    case 5:return (DWORD)(ref?Reference_MarineTask_DeleteSecondary(state[0]+76,0,variant):MarineTask_DeleteSecondary(state[0]+76,0,variant));
    case 6:return (DWORD)(ref?Reference_MarineGeometry_Snapshot(state[7],0,(DWORD*)state[11],state[10]):MarineGeometry_Snapshot(state[7],0,(DWORD*)state[11],state[10]));
    case 7:return (DWORD)(ref?Reference_MarineTransition_Create(state[0],0,variant,state[6],a,b,variant%15,state[7]):MarineTransition_Create(state[0],0,variant,state[6],a,b,variant%15,state[7]));
    default:return ref?Reference_MarineTransition_Advance(state[0],0,variant):MarineTransition_Advance(state[0],0,variant);
    }
}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    for(unsigned method=0;method<9;method++) for(unsigned test=0;test<64;test++) {
        Setup(test);DWORD ref=Run(method,true);unsigned expectedCount=count;
        for(unsigned i=0;i<12;i++) for(unsigned j=0;j<256;j++) expected[i][j]=state[i][j];
        for(unsigned i=0;i<88;i++) expectedPool[i]=pool[i];
        for(unsigned i=0;i<128;i++) expectedTrace[i]=trace[i];
        Setup(test);DWORD actual=Run(method,false);
        if(ref!=actual || count!=expectedCount) ExitProcess(1+method);
        for(unsigned i=0;i<12;i++) for(unsigned j=0;j<256;j++) if(expected[i][j]!=state[i][j]) ExitProcess(20+method);
        for(unsigned i=0;i<88;i++) if(expectedPool[i]!=pool[i]) ExitProcess(40+method);
        for(unsigned i=0;i<128;i++) if(expectedTrace[i]!=trace[i]) ExitProcess(60+method);
    }
    ExitProcess(0);
}
