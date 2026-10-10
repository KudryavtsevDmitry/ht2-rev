#include "../build-test/gridbase-globals.h"
#define DECL(prefix) \
extern "C" void __fastcall prefix##GridBase_SetCamera(BYTE*,void*,const DWORD*,const DWORD*); \
extern "C" void __fastcall prefix##GridBase_SetCameraAndOrigin(BYTE*,void*,const DWORD*,const float*,const DWORD*); \
extern "C" void __fastcall prefix##GridBase_Destruct(BYTE*,void*); \
extern "C" BYTE* __fastcall prefix##GridBase_Delete(BYTE*,void*,DWORD); \
extern "C" BYTE* __fastcall prefix##GridBase_DeleteAlternate(BYTE*,void*,DWORD); \
extern "C" DWORD __fastcall prefix##TerrainCell_SetTriangles(BYTE*,void*,BYTE*,BYTE*); \
extern "C" void __cdecl prefix##GridBase_InitScale(); \
extern "C" void __cdecl prefix##GridBase_InitSecondaryScale(); \
extern "C" void __cdecl prefix##GridBase_InitBlend(); \
extern "C" void __cdecl prefix##GridBase_InitOrigin(); \
extern "C" void __cdecl prefix##GridBase_InitPending();
DECL( /* actual */ )
DECL(Reference_)
static DWORD object[32],children[3][16],arena[16],input[9],state[1024],used,trace,testCase;
static DWORD vtableA[16],vtableB[16];
static void Trace(DWORD value) {trace=trace*16777619u^value;}
extern "C" void* __cdecl TestAllocate(unsigned n) {Trace(n);DWORD at=used;used+=8;return arena+at;}
extern "C" void __cdecl TestDelete(void* p) {Trace((DWORD)p);}
KING_ALIAS("??2@YAPAXI@Z","_TestAllocate")
KING_ALIAS("??3@YAXPAX@Z","_TestDelete")
extern "C" void __fastcall Child44(BYTE* p,void*) {Trace(44);Trace((DWORD)p);if(testCase&8)F(DWORD,p,0)=(DWORD)vtableB;if(testCase&16)F(DWORD,p,52)=0;}
extern "C" void __fastcall Child48(BYTE* p,void*) {Trace(48);Trace((DWORD)p);if(testCase&32)F(DWORD,p,52)=0;}
extern "C" void __fastcall Changed48(BYTE* p,void*) {Trace(148);Trace((DWORD)p);if(testCase&64)F(DWORD,p,52)=0;}
static void Setup(DWORD test)
{
    testCase=test;trace=used=0;
    const DWORD special[]={0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc12345,0x7fa67890,1,0x7f7fffff,0x3eaaaaab};
    for(unsigned i=0;i<32;++i)object[i]=0x40400000+i*1234;
    for(unsigned i=0;i<16;++i)arena[i]=0xcdcdcdcd;
    for(unsigned i=0;i<9;++i)input[i]=special[(test+i*3)%11];
    for(unsigned i=0;i<3;++i){for(unsigned j=0;j<16;++j)children[i][j]=0x3f800000+j;children[i][0]=(DWORD)vtableA;children[i][13]=i==2?0:(DWORD)children[i+1];}
    vtableA[11]=(DWORD)Child44;vtableA[12]=(DWORD)Child48;vtableB[12]=(DWORD)Changed48;
    object[13]=(test&1)?0:(DWORD)children[0];
    *(DWORD*)&g_gb_scaleInput=special[test%11];*(DWORD*)&g_gb_secondaryInput=special[(test+1)%11];
    *(DWORD*)&g_gb_blendInput=special[(test+2)%11];*(DWORD*)&g_gb_blendOffset=special[(test+3)%11];*(DWORD*)&g_gb_half=special[(test+4)%11];
    *(DWORD*)&g_gb_scale=*(DWORD*)&g_gb_secondary=*(DWORD*)&g_gb_blend=0xdeadbeef;
    for(unsigned i=0;i<3;++i)g_gb_origin[i]=0xabcdef00+i;
    g_gb_pending=test;
}
static DWORD Run(unsigned method,bool reference)
{
    BYTE* p=(BYTE*)object;
    const DWORD* position=testCase&128?object+2:input;
    const DWORD* direction=testCase&64?object+1:input+3;
    const DWORD* origin=testCase&32?object+4:input+6;
    switch(method){
    case 0:if(reference)Reference_GridBase_SetCamera(p,0,position,direction);else GridBase_SetCamera(p,0,position,direction);break;
    case 1:if(reference)Reference_GridBase_SetCameraAndOrigin(p,0,position,(const float*)direction,origin);else GridBase_SetCameraAndOrigin(p,0,position,(const float*)direction,origin);break;
    case 2:if(reference)Reference_GridBase_Destruct(p,0);else GridBase_Destruct(p,0);break;
    case 3:return (DWORD)(reference?Reference_GridBase_Delete(p,0,testCase):GridBase_Delete(p,0,testCase));
    case 4:return (DWORD)(reference?Reference_GridBase_DeleteAlternate(p,0,testCase):GridBase_DeleteAlternate(p,0,testCase));
    case 5:return reference?Reference_TerrainCell_SetTriangles(p,0,testCase&1?(BYTE*)children[0]:0,testCase&2?(BYTE*)children[1]:0):TerrainCell_SetTriangles(p,0,testCase&1?(BYTE*)children[0]:0,testCase&2?(BYTE*)children[1]:0);
    case 6:if(reference)Reference_GridBase_InitScale();else GridBase_InitScale();break;
    case 7:if(reference)Reference_GridBase_InitSecondaryScale();else GridBase_InitSecondaryScale();break;
    case 8:if(reference)Reference_GridBase_InitBlend();else GridBase_InitBlend();break;
    case 9:if(reference)Reference_GridBase_InitOrigin();else GridBase_InitOrigin();break;
    case 10:if(reference)Reference_GridBase_InitPending();else GridBase_InitPending();break;
    }
    return 0;
}
static DWORD State(bool save)
{
    DWORD n=0;DWORD* regions[]={object,children[0],children[1],children[2],arena,input,&used,&trace,(DWORD*)&g_gb_scale,(DWORD*)&g_gb_secondary,(DWORD*)&g_gb_blend,g_gb_origin,&g_gb_pending};
    unsigned sizes[]={32,16,16,16,16,9,1,1,1,1,1,3,1};
    for(unsigned r=0;r<13;++r)for(unsigned i=0;i<sizes[r];++i){if(save)state[n]=regions[r][i];else if(state[n]!=regions[r][i])return n+1;++n;}
    return 0;
}
static void Print(DWORD a,DWORD b) {char text[19];DWORD values[]={a,b};unsigned n=0;for(unsigned j=0;j<2;++j){for(int i=7;i>=0;--i)text[n++]="0123456789abcdef"[(values[j]>>(i*4))&15];text[n++]=' ';}text[n++]='\n';DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,n,&written,0);}
extern "C" void TestEntry()
{
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);WORD original,control;__asm fnstcw original
    WORD precision[]={0,0x200,0x300};
    for(unsigned mode=0;mode<3;++mode){control=(original&~0x300)|precision[mode]|0x3f;__asm fldcw control
        for(unsigned method=0;method<11;++method)for(unsigned test=0;test<512;++test){
            Setup(test);DWORD expected=Run(method,true);State(true);
            Setup(test);DWORD actual=Run(method,false),diff=State(false);
            if(actual!=expected||diff){Print(mode*10000+method*1000+test+1,diff);ExitProcess(1);}
        }
    }
    const char message[]="Grid camera, child lifetimes, terrain cells and defaults match MASM in 16,896 full-state comparisons.\n";
    DWORD written;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,0);ExitProcess(0);
}
