#include "../build-test/marinegrid-globals.h"
#define DECL(prefix) DECL_I(prefix)
#define DECL_I(prefix) \
extern "C" void __fastcall prefix##MarineGrid_Destruct(BYTE*,void*); \
extern "C" BYTE* __fastcall prefix##MarineGrid_Delete(BYTE*,void*,DWORD); \
extern "C" BYTE* __fastcall prefix##MarineGrid_WriteVertex(BYTE*,void*,const DWORD*,const DWORD*,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD); \
extern "C" DWORD __fastcall prefix##MarineGrid_Allocate(BYTE*,void*,DWORD,DWORD); \
extern "C" void __fastcall prefix##MarineGrid_Draw(BYTE*,void*,BYTE*,BYTE*); \
extern "C" DWORD __fastcall prefix##MarineGrid_DesiredWork(BYTE*,void*); \
extern "C" void __fastcall prefix##MarineGrid_TrackCamera(BYTE*,void*,const DWORD*,const DWORD*); \
extern "C" DWORD __fastcall prefix##MarineGrid_TrimBranch(BYTE*,void*,BYTE*); \
extern "C" void __fastcall prefix##MarineGrid_Trim(BYTE*,void*,DWORD); \
extern "C" double __cdecl prefix##MarineGrid_QualityScale(float,float); \
extern "C" void __cdecl prefix##MarineGrid_BuildRandomDirections(DWORD);
extern "C" void __fastcall MarineGrid_Destruct(BYTE*,void*);
extern "C" BYTE* __fastcall MarineGrid_Delete(BYTE*,void*,DWORD);
extern "C" BYTE* __fastcall MarineGrid_WriteVertex(BYTE*,void*,const DWORD*,const DWORD*,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
extern "C" DWORD __fastcall MarineGrid_Allocate(BYTE*,void*,DWORD,DWORD);
extern "C" void __fastcall MarineGrid_Draw(BYTE*,void*,BYTE*,BYTE*);
extern "C" DWORD __fastcall MarineGrid_DesiredWork(BYTE*,void*);
extern "C" void __fastcall MarineGrid_TrackCamera(BYTE*,void*,const DWORD*,const DWORD*);
extern "C" DWORD __fastcall MarineGrid_TrimBranch(BYTE*,void*,BYTE*);
extern "C" void __fastcall MarineGrid_Trim(BYTE*,void*,DWORD);
extern "C" double __cdecl MarineGrid_QualityScale(float,float);
extern "C" void __cdecl MarineGrid_BuildRandomDirections(DWORD);

DECL(Reference_)
static DWORD object[64],buffers[3][1024],pool[32],children[2][32],material[32],device[2],event[2],state[160],source[32],sourceChild[32];
static DWORD nodes[4][32],branches[3][8],queue[4],pairs[2][2],pairPointers[2],edges[4][4],wrappers[4][4],input[6];
static DWORD trace,randomState,allocCount,mallocCount,testCase;
static DWORD deviceVtable2[32];
static void Trace(DWORD x) {trace=(trace*16777619u)^x;}
extern "C" void __cdecl TestDelete(void* p) {Trace(1);Trace((DWORD)p);if(testCase&32) object[35]=(DWORD)buffers[2];}
extern "C" void* __cdecl TestAllocate(unsigned n) {Trace(2);Trace(n);++allocCount;return pool;}
extern "C" void* __cdecl TestMalloc(unsigned n) {Trace(3);Trace(n);unsigned index=mallocCount++;return (testCase&16)&&index==((testCase>>6)%3)?0:buffers[index%3];}
extern "C" void __cdecl TestSrand(unsigned n) {randomState=n;Trace(n);}
extern "C" int __cdecl TestRand() {randomState=randomState*214013u+2531011u;return (randomState>>16)&32767;}
extern "C" void __cdecl TestNoop() {}
extern "C" void __cdecl TestReset() {Trace(4);}
extern "C" void __cdecl TestError(DWORD code,void*,void*,DWORD line,void*) {Trace(code);Trace(line);}
extern "C" void __cdecl TestAssert(DWORD code,void*,void*,void*,DWORD line,void*) {Trace(code);Trace(line);}
extern "C" DWORD __fastcall TestMaterial(BYTE* p,void*) {Trace(5);Trace((DWORD)p);return 71;}
extern "C" void __fastcall TestBind(BYTE* p,void*,DWORD a,DWORD b,DWORD c,DWORD d) {Trace(6);Trace((DWORD)p);Trace(a);Trace(b);Trace(c);Trace(d);if(testCase&64) {object[34]=2;device[0]=(DWORD)deviceVtable2;}}
extern "C" void __cdecl TestSplit(BYTE* p,BYTE* owner) {Trace(7);Trace((DWORD)p);Trace((DWORD)owner);}
extern "C" void __fastcall TestMerge(BYTE* p,void*,BYTE* edge,BYTE* replacement) {Trace(8);Trace((DWORD)p);Trace((DWORD)edge);Trace((DWORD)replacement);}
extern "C" DWORD __cdecl TestEligible(BYTE* p) {return F(DWORD,p,36)>=10;}
extern "C" BYTE* __fastcall MarineParticlePool_Construct(BYTE* p,void*,DWORD capacity,DWORD vertices,DWORD color,DWORD owner) {Trace(9);Trace(capacity);Trace(vertices);Trace(color);Trace(owner);F(DWORD,p,0)=owner;return p;}
extern "C" void __fastcall MarineParticlePool_SetFloor(BYTE* p,void*,DWORD x) {Trace(10);F(DWORD,p,28)=x;}
extern "C" void __fastcall MarineParticlePool_SetLifetime(BYTE* p,void*,DWORD x) {Trace(11);F(DWORD,p,32)=x;}
extern "C" void __fastcall MarineParticlePool_Render(BYTE* p,void*,BYTE* d,BYTE* e,DWORD m) {Trace(12);Trace((DWORD)p);Trace((DWORD)d);Trace((DWORD)e);Trace(m);}
extern "C" DWORD __fastcall MarineGrid_ClassifyTriangle(BYTE*,void*,BYTE* t) {Trace(13);Trace((DWORD)t);return F(DWORD,t,0);}
extern "C" void __fastcall MarineGreed_RemoveNode(BYTE* p,void*,BYTE* t) {Trace(14);Trace((DWORD)p);Trace((DWORD)t);}
extern "C" DWORD __fastcall MarineMesh_DrainEdges(BYTE* p,void*) {Trace(15);Trace((DWORD)p);return 0;}
extern "C" DWORD __fastcall MarineMesh_PruneVertices(BYTE* p,void*) {Trace(16);Trace((DWORD)p);return 0;}
KING_ALIAS("_FUN_5e48f0","_TestNoop")
KING_ALIAS("_FUN_60bb20","_TestReset")
KING_ALIAS("_FUN_60aad0","_TestError")
KING_ALIAS("_FUN_60aa60","_TestAssert")
KING_ALIAS("_FUN_605810","@TestMaterial@8")
KING_ALIAS("_FUN_5b1020","@TestBind@24")
KING_ALIAS("_FUN_605900","_TestSplit")
KING_ALIAS("_FUN_6064c0","@TestMerge@16")
extern "C" DWORD __fastcall Size(BYTE* p,void*) {Trace(17);Trace((DWORD)p);return 3;}
extern "C" void __fastcall Child44(BYTE* p,void*) {Trace(18);Trace((DWORD)p);}
extern "C" void __fastcall Child48(BYTE* p,void*) {Trace(19);Trace((DWORD)p);}
extern "C" void __fastcall Event(BYTE* p,void*,DWORD m) {Trace(20);Trace((DWORD)p);Trace(m);}
extern "C" LONG __stdcall DeviceState(BYTE* p,DWORD a,DWORD b) {Trace(21);Trace((DWORD)p);Trace(a);Trace(b);return 0;}
extern "C" LONG __stdcall DeviceDraw(BYTE* p,DWORD a,DWORD b,void* c,DWORD d,void* e,DWORD f,DWORD g) {Trace(22);Trace((DWORD)p);Trace(a);Trace(b);Trace((DWORD)c);Trace(d);Trace((DWORD)e);Trace(f);Trace(g);return (testCase&128)?-3:0;}
extern "C" LONG __stdcall DeviceDraw2(BYTE* p,DWORD a,DWORD b,void* c,DWORD d,void* e,DWORD f,DWORD g) {Trace(23);return DeviceDraw(p,a,b,c,d,e,f,g);}
static DWORD vtable[32],childVtable[32],eventVtable[32],deviceVtable[32];
static DWORD Bits(float x) {return *(DWORD*)&x;}
static void Setup(unsigned n)
{
    testCase=n;trace=0;randomState=n+111;allocCount=mallocCount=0;
    for(unsigned i=0;i<64;++i) object[i]=Bits((float)((int)((n+i*7)%41)-20)/4);
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<1024;++j) buffers[i][j]=n+i*33+j;
    for(unsigned i=0;i<2600;++i) g_noise[i]=n*7+i;
    for(unsigned i=0;i<32;++i) {pool[i]=n+i;material[i]=n+i;for(unsigned j=0;j<2;++j)children[j][i]=n+i+j;}
    for(unsigned i=0;i<160;++i)state[i]=n+i;
    for(unsigned i=0;i<32;++i){source[i]=n+i;sourceChild[i]=n+i;}
    for(unsigned i=0;i<32;++i) vtable[i]=childVtable[i]=eventVtable[i]=deviceVtable[i]=0;
    vtable[3]=(DWORD)Size;childVtable[11]=(DWORD)Child44;childVtable[12]=(DWORD)Child48;eventVtable[2]=(DWORD)Event;
    deviceVtable[20]=(DWORD)DeviceState;deviceVtable[26]=(DWORD)DeviceDraw;
    deviceVtable2[20]=(DWORD)DeviceState;deviceVtable2[26]=(DWORD)DeviceDraw2;
    object[0]=(DWORD)vtable;object[1]=1;object[11]=n%17;object[12]=n%19;
    object[28]=(DWORD)pool;object[29]=123;pool[3]=(n&1)?0:(DWORD)buffers[0];
    object[31]=(n&2)?0:(DWORD)buffers[0];object[33]=(n&4)?0:(DWORD)buffers[1];object[35]=(n&8)?0:(DWORD)buffers[2];object[50]=(n&1)?0:(DWORD)material;
    object[30]=3;object[32]=3;object[34]=5;object[13]=(n&8)?0:(DWORD)children[0];
    children[0][0]=children[1][0]=(DWORD)childVtable;children[0][13]=(n&4)?0:(DWORD)children[1];children[1][13]=0;
    device[0]=(DWORD)deviceVtable;event[0]=(DWORD)eventVtable;
    state[122]=(DWORD)source;source[19]=(DWORD)sourceChild;sourceChild[9]=61;state[117]=62;
    g_gridState=(BYTE*)state;g_gridCapacity=7;g_gridVertices=11;g_gridTrimLimit=n+15;g_gridBudget=(int)(n*65537u);
    g_gridOne=1;g_gridZero=0;g_gridRandomUnit=1.0f/32768;g_gridRandomRadius=2.25f;g_gridTrackUpper=2;g_gridTrackLower=-2;
    for(unsigned i=0;i<6;++i) input[i]=Bits((float)((int)((n+i*7)%29)-14));
    if(n<11) {const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,1,0x7f7fffff,0x7fc01234,0x7f801234,0x7f800000,0xff800000,0x007fffff};for(unsigned i=0;i<6;++i)input[i]=special[(n+i)%11];}
    if(n&128) object[11]=0x80000001u;
    if(n&64) object[12]=0xffffffffu;
    object[48]=Bits(0.5f+(float)(n%4));object[49]=Bits(0.75f+(float)(n%3));
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<8;++j)branches[i][j]=0;
    branches[0][0]=(DWORD)branches[1];branches[0][1]=(DWORD)branches[2];branches[0][3]=(n&8)?(DWORD)nodes[0]:0;
    branches[1][3]=(DWORD)nodes[1];branches[2][3]=(DWORD)nodes[2];
    for(unsigned i=0;i<4;++i) for(unsigned j=0;j<32;++j)nodes[i][j]=0;
    for(unsigned i=0;i<3;++i) {const DWORD kind[]={1,2,4,3};nodes[i][0]=kind[(n+i)%4];nodes[i][9]=(n&4)?12:2;}
    pairs[0][0]=pairs[1][0]=(DWORD)branches[0];pairs[0][1]=pairs[1][1]=(DWORD)branches[1];pairPointers[0]=(DWORD)pairs[0];pairPointers[1]=(DWORD)pairs[1];
    object[2]=(DWORD)pairPointers;object[12]=n%3;object[8]=(DWORD)queue;
    queue[0]=(n&1)?0:(DWORD)nodes[0];queue[1]=(n&2)?0:(DWORD)nodes[1];
    // Two independent singleton circular runs, selected by original sentinels.
    for(unsigned i=0;i<2;++i) {
        nodes[i][1]=(DWORD)(nodes[i]+1);nodes[i][2]=(DWORD)(nodes[i]+1);
        wrappers[i][2]=(DWORD)edges[i];nodes[i][8]=(DWORD)wrappers[i];edges[i][2]=(DWORD)edges[i+2];
        edges[i][0]=(DWORD)branches[1];edges[i][1]=(DWORD)branches[2];
        branches[1][3]=(DWORD)nodes[i];branches[2][3]=(DWORD)nodes[i];
        nodes[i][12]=(n&4)?(DWORD)nodes[2]:0;nodes[i][13]=(n&8)?(DWORD)nodes[3]:0;nodes[i][14]=(n&16)?1:0;
    }
    wrappers[2][2]=wrappers[3][2]=(DWORD)edges[2];nodes[2][8]=(DWORD)wrappers[2];nodes[3][8]=(DWORD)wrappers[3];nodes[2][12]=(n&32)?(DWORD)nodes[3]:0;nodes[3][13]=(n&64)?(DWORD)nodes[2]:0;
    edges[2][0]=(n&32)?0:1;edges[2][1]=(n&64)?0:1;edges[3][0]=0;edges[3][1]=0;
}
static unsigned __int64 Run(unsigned method,bool ref)
{
    BYTE* self=(BYTE*)object;const DWORD* a=(testCase&256)?object+1:input;const DWORD* b=(testCase&512)?object+2:input+3;
    switch(method) {
    case 0:if(ref)Reference_MarineGrid_Destruct(self,0);else MarineGrid_Destruct(self,0);break;
    case 1:return (DWORD)(ref?Reference_MarineGrid_Delete(self,0,testCase&1):MarineGrid_Delete(self,0,testCase&1));
    case 2:return (DWORD)(ref?Reference_MarineGrid_WriteVertex(self,0,a,b,1,2,3,4,5,6):MarineGrid_WriteVertex(self,0,a,b,1,2,3,4,5,6));
    case 3:return ref?Reference_MarineGrid_Allocate(self,0,7,9):MarineGrid_Allocate(self,0,7,9);
    case 4:if(ref)Reference_MarineGrid_Draw(self,0,(BYTE*)device,(BYTE*)event);else MarineGrid_Draw(self,0,(BYTE*)device,(BYTE*)event);break;
    case 5:return ref?Reference_MarineGrid_DesiredWork(self,0):MarineGrid_DesiredWork(self,0);
    case 6:if(ref)Reference_MarineGrid_TrackCamera(self,0,a,b);else MarineGrid_TrackCamera(self,0,a,b);break;
    case 7:return ref?Reference_MarineGrid_TrimBranch(self,0,(BYTE*)branches[0]):MarineGrid_TrimBranch(self,0,(BYTE*)branches[0]);
    case 8:if(ref)Reference_MarineGrid_Trim(self,0,testCase);else MarineGrid_Trim(self,0,testCase);break;
    case 9:{union {double d;unsigned __int64 bits;} r;r.d=ref?Reference_MarineGrid_QualityScale(*(float*)a,*(float*)b):MarineGrid_QualityScale(*(float*)a,*(float*)b);return r.bits;}
    case 10:if(ref)Reference_MarineGrid_BuildRandomDirections(testCase);else MarineGrid_BuildRandomDirections(testCase);break;
    }
    return 0;
}
static DWORD snapshot[10000];
static DWORD* regions[]={object,(DWORD*)buffers,pool,(DWORD*)children,material,(DWORD*)nodes,(DWORD*)branches,queue,(DWORD*)edges,g_noise,&trace,&randomState,&allocCount,&mallocCount,&g_gridTrimLimit};
static unsigned sizes[]={64,3072,32,64,32,128,24,4,16,2600,1,1,1,1,1};
static void Print(DWORD a,DWORD b,DWORD c) {char text[28];DWORD args[]={a,b,c};unsigned n=0;for(unsigned j=0;j<3;++j){for(int i=7;i>=0;--i)text[n++]="0123456789abcdef"[(args[j]>>(i*4))&15];text[n++]=' ';}text[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,n,&written,0);}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);WORD original,control;__asm fnstcw original
    const WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode) {
        control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<11;++method) for(unsigned test=0;test<1024;++test) {
            Setup(test);unsigned __int64 expected=Run(method,true);unsigned offset=0;
            for(unsigned r=0;r<15;++r)for(unsigned i=0;i<sizes[r];++i)snapshot[offset++]=regions[r][i];
            Setup(test);unsigned __int64 actual=Run(method,false);DWORD code=mode*100000+method*10000+test+1;
            if(expected!=actual){Print(code,(DWORD)expected,(DWORD)actual);ExitProcess(code);}
            offset=0;for(unsigned r=0;r<15;++r)for(unsigned i=0;i<sizes[r];++i){if(snapshot[offset]!=regions[r][i]){Print(code,offset,r);Print(i,snapshot[offset],regions[r][i]);ExitProcess(code);}++offset;}
        }
    }
    const char message[]="Marine grid batch matches MASM in 33,792 comparisons.\n";DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,0);ExitProcess(0);
}
