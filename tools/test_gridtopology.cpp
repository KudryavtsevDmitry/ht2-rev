#include "../build-test/gridtopology-globals.h"
#define DECL(prefix) \
extern "C" DWORD __fastcall prefix##GridTriangle_SetPoints(BYTE*,void*,DWORD,DWORD,DWORD,DWORD); \
extern "C" BYTE* __fastcall prefix##GridEdge_Construct(BYTE*,void*); \
extern "C" DWORD __fastcall prefix##GridEdge_SetPairs(BYTE*,void*,BYTE*,BYTE*); \
extern "C" void __fastcall prefix##GridTriangle_Destruct(BYTE*,void*); \
extern "C" BYTE* __fastcall prefix##GridMesh_CreateTriangle(BYTE*,void*,DWORD,DWORD,DWORD,DWORD); \
extern "C" BYTE* __fastcall prefix##GridMesh_CreatePoint(BYTE*,void*,const DWORD*); \
extern "C" BYTE* __fastcall prefix##GridMesh_RemoveTriangle(BYTE*,void*,BYTE*); \
extern "C" BYTE* __fastcall prefix##GridMesh_RemoveEdge(BYTE*,void*,BYTE*); \
extern "C" void __fastcall prefix##GridEdge_Destruct(BYTE*,void*); \
extern "C" void __fastcall prefix##GridBranch_Destruct(BYTE*,void*);
DECL( /* actual */ )
DECL(Reference_)
static DWORD object[32],nodes[8][32],pairs[4][8],queue[4],arena[32],points[65536],input[4],device[2],vtable[16];
static DWORD used,trace,testCase,state[66000];
static void Trace(DWORD n){trace=trace*16777619u^n;}
extern "C" void* __cdecl TestAllocate(unsigned n) {Trace(n);DWORD at=used;used+=(n+15)/16*4;if(testCase&512)object[19]=0;return arena+at;}
extern "C" void __cdecl TestDelete(void* p) {Trace((DWORD)p);if(testCase&256)object[1]=(DWORD)pairs[3];}
extern "C" void __cdecl TestError(DWORD code,void*,void*,DWORD line,void*) {Trace(code);Trace(line);if(testCase&128)object[1]=0;}
KING_ALIAS("??2@YAPAXI@Z","_TestAllocate")
KING_ALIAS("??3@YAXPAX@Z","_TestDelete")
extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE* q,void*,BYTE* t) {Trace(1);Trace((DWORD)q);Trace((DWORD)t);F(DWORD,t,56)=testCase;return 0;}
extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE* q,void*,BYTE* t) {Trace(2);Trace((DWORD)q);Trace((DWORD)t);F(DWORD,t,0)=testCase;return t+4;}
extern "C" BYTE* __fastcall MarineQueue_RemoveLink(BYTE* q,void*,BYTE* e) {Trace(3);Trace((DWORD)q);Trace((DWORD)e);F(DWORD,e,0)=testCase;return e;}
extern "C" double __fastcall Weight(BYTE*,void*,DWORD a,DWORD b,DWORD c,BYTE* data)
{
    Trace(a);Trace(b);Trace(c);Trace(F(DWORD,data,24));
    if(testCase&64)object[8]=(DWORD)pairs[3];
    return (double)(testCase+a+b+c)/17.0;
}
static void Setup(DWORD test)
{
    testCase=test;used=trace=0;
    for(unsigned i=0;i<32;++i){object[i]=0x3e800000+i;arena[i]=0xcdcdcdcd;}
    for(unsigned i=0;i<65536;++i)points[i]=0xdeadbeef;
    for(unsigned i=0;i<8;++i)for(unsigned j=0;j<32;++j)nodes[i][j]=0x3f000000+test*13+j;
    for(unsigned i=0;i<4;++i){for(unsigned j=0;j<8;++j)pairs[i][j]=0;pairs[i][3]=(DWORD)nodes[i];}
    object[1]=(DWORD)device;device[0]=(DWORD)vtable;vtable[9]=(DWORD)Weight;
    object[3]=(DWORD)points;object[7]=object[8]=(DWORD)queue;
    object[9]=((test&8)?0xffff:3)|((test&16)?0xffff0000:0x00040000);
    object[13]=0xffffffff;object[10]=(test&1)?(DWORD)nodes[0]:0;
    object[11]=(test&2)?(DWORD)nodes[0]:0;
    queue[0]=(test&4)?0:(DWORD)nodes[1];queue[1]=(test&8)?0:(DWORD)nodes[2];
    nodes[0][1]=(DWORD)(nodes[3]+1);nodes[0][0]=(DWORD)nodes[3];
    nodes[1][2]=(test&16)?(DWORD)(nodes[0]+1):0;
    nodes[2][2]=(test&32)?(DWORD)(nodes[0]+1):0;
    nodes[1][1]=(test&16)?(DWORD)nodes[0]:0;nodes[2][1]=(test&32)?(DWORD)nodes[0]:0;
    input[0]=test*127;input[1]=test*313;input[2]=test^0x76543210;input[3]=test;
}
static void SetupPairs(bool invalid)
{
    for(unsigned i=0;i<4;++i){nodes[i][3]=(DWORD)nodes[i+4];pairs[i][2]=(DWORD)object;}
    pairs[0][0]=(DWORD)nodes[0];pairs[0][1]=(DWORD)nodes[1];
    pairs[1][0]=(DWORD)nodes[2];pairs[1][1]=(DWORD)nodes[3];
    nodes[4][12]=(testCase&1)?0:(DWORD)nodes[5];nodes[6][12]=(testCase&2)?0:(DWORD)nodes[7];
    DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc12345,0x7fa67890};
    object[8]=special[testCase%8];object[15]=special[(testCase/8)%8];
    if(invalid) switch((testCase>>4)%6){
    case 0:pairs[0][2]=0;break;case 1:pairs[0][0]=0;break;case 2:pairs[0][1]=0;break;
    case 3:nodes[0][3]=0;break;case 4:nodes[1][3]=0;break;case 5:pairs[1][2]=0;break;
    }
}
static DWORD Run(unsigned method,bool ref)
{
    BYTE* p=(BYTE*)object;BYTE* n=(BYTE*)nodes[0];
    switch(method){
    case 0:return ref?Reference_GridTriangle_SetPoints(p,0,input[0],input[1],input[2],input[3]):GridTriangle_SetPoints(p,0,input[0],input[1],input[2],input[3]);
    case 1:return (DWORD)(ref?Reference_GridEdge_Construct(p,0):GridEdge_Construct(p,0));
    case 2:SetupPairs(testCase&128);return ref?Reference_GridEdge_SetPairs(p,0,(BYTE*)pairs[0],testCase&4?(BYTE*)pairs[1]:0):GridEdge_SetPairs(p,0,(BYTE*)pairs[0],testCase&4?(BYTE*)pairs[1]:0);
    case 3:if(ref)Reference_GridTriangle_Destruct(p,0);else GridTriangle_Destruct(p,0);break;
    case 4:return (DWORD)(ref?Reference_GridMesh_CreateTriangle(p,0,input[0],input[1],input[2],input[3]):GridMesh_CreateTriangle(p,0,input[0],input[1],input[2],input[3]));
    case 5:return (DWORD)(ref?Reference_GridMesh_CreatePoint(p,0,(testCase&2)&&!(testCase&8)?points+3:(testCase&1?object+9:input)):GridMesh_CreatePoint(p,0,(testCase&2)&&!(testCase&8)?points+3:(testCase&1?object+9:input)));
    case 6:return (DWORD)(ref?Reference_GridMesh_RemoveTriangle(p,0,n):GridMesh_RemoveTriangle(p,0,n));
    case 7:SetupPairs(false);n=(BYTE*)nodes[7];object[11]=(testCase&2)?(DWORD)n:0;nodes[1][1]=(testCase&16)?(DWORD)n:0;nodes[2][1]=(testCase&32)?(DWORD)n:0;nodes[7][0]=(DWORD)nodes[3];nodes[7][2]=(DWORD)pairs[0];nodes[7][3]=testCase&64?(DWORD)pairs[1]:0;return (DWORD)(ref?Reference_GridMesh_RemoveEdge(p,0,n):GridMesh_RemoveEdge(p,0,n));
    case 8:if(ref)Reference_GridEdge_Destruct(p,0);else GridEdge_Destruct(p,0);break;
    case 9:
        for(unsigned i=0;i<4;++i){pairs[i][0]=pairs[i][1]=pairs[i][2]=0;}
        object[0]=testCase&1?(DWORD)pairs[0]:0;object[1]=testCase&2?(DWORD)pairs[1]:0;object[2]=testCase&4?(DWORD)pairs[2]:0;
        pairs[0][2]=pairs[1][2]=(DWORD)object;
        pairs[2][testCase&8?1:0]=testCase&16?(DWORD)pairs[3]:(DWORD)object;
        if(ref)Reference_GridBranch_Destruct(p,0);else GridBranch_Destruct(p,0);break;
    }
    return 0;
}
static DWORD State(bool save)
{
    DWORD n=0;DWORD* regions[]={object,nodes[0],pairs[0],queue,arena,points,input,&used,&trace};
    unsigned sizes[]={32,256,32,4,32,65536,4,1,1};
    for(unsigned r=0;r<9;++r)for(unsigned i=0;i<sizes[r];++i){if(save)state[n]=regions[r][i];else if(state[n]!=regions[r][i])return n+1;++n;}
    return 0;
}
static void Print(DWORD a,DWORD b){char text[19];DWORD values[]={a,b};unsigned n=0;for(unsigned j=0;j<2;++j){for(int i=7;i>=0;--i)text[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];text[n++]=' ';}text[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,n,&written,0);}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);WORD original,control;__asm fnstcw original
    WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode){control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<10;++method)for(unsigned test=0;test<1024;++test){
            Setup(test);DWORD expected=Run(method,true);State(true);
            Setup(test);DWORD actual=Run(method,false),diff=State(false);
            if(expected!=actual||diff){Print(mode*100000+method*10000+test+1,diff);ExitProcess(1);}
        }
    }
    const char message[]="Grid triangle, point, refinement-edge and branch helpers match MASM in 30,720 full-state comparisons.\n";
    DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,0);ExitProcess(0);
}
