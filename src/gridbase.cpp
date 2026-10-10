// Shared grid camera state, child lifetimes and terrain cell branches.
#include "king.h"
#include "x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
typedef char GridBaseLabel[1];
ASM_VAR(GridBaseLabel,g_gb_vtable,$L_652ea0)
ASM_VAR(float,g_gb_scaleInput,$L_652ee0)
ASM_VAR(float,g_gb_scale,$L_724298)
ASM_VAR(float,g_gb_secondaryInput,$L_652ee4)
ASM_VAR(float,g_gb_secondary,$L_724370)
ASM_VAR(float,g_gb_blendInput,$L_652ee8)
ASM_VAR(float,g_gb_blend,$L_724374)
ASM_VAR(float,g_gb_blendOffset,$L_64d4e8)
ASM_VAR(float,g_gb_half,$L_64c258)
struct GridBaseTriple { DWORD v[3]; };
ASM_VAR(GridBaseTriple,g_gb_origin,$L_724378)
ASM_VAR(DWORD,g_gb_pending,$L_7242a0)

extern "C" void __fastcall GridBase_SetCamera(BYTE* self,void*,const DWORD* position,const DWORD* direction)
{
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,4+i*4)=position[i];
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,16+i*4)=direction[i];
}
extern "C" void __fastcall GridBase_Destruct(BYTE* self,void*)
{
    BYTE* child=F(BYTE*,self,52); F(void*,self,0)=g_gb_vtable;
    while(child) {
        CallVirt(child,44); CallVirt(child,48);
        child=F(BYTE*,child,52);
    }
}
extern "C" BYTE* __fastcall GridBase_Delete(BYTE* self,void*,DWORD flags)
{
    GridBase_Destruct(self,0); if(flags&1) operator delete(self); return self;
}
extern "C" BYTE* __fastcall GridBase_DeleteAlternate(BYTE* self,void*,DWORD flags)
{
    GridBase_Destruct(self,0); if(flags&1) operator delete(self); return self;
}
// Keep the denominator on the x87 stack through all three divisions, as
// the original does. Rounding it to a double changes some normalized bits.
#pragma warning(push)
#pragma warning(disable:4035)
static __declspec(noinline) void GridBase_Normalize(const float* input,float* output)
{
    __asm {
        mov eax,input
        fld dword ptr [eax+8]
        fld dword ptr [eax+4]
        fld dword ptr [eax]
        fld st(0)
        fmul st(0),st(1)
        fld st(2)
        fmul st(0),st(3)
        faddp st(1),st(0)
        fld st(3)
        fmul st(0),st(4)
        faddp st(1),st(0)
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        mov edx,output
        fld dword ptr [eax]
        fdiv st(0),st(1)
        fstp dword ptr [edx]
        fld dword ptr [eax+4]
        fdiv st(0),st(1)
        fstp dword ptr [edx+4]
        fld dword ptr [eax+8]
        fdiv st(0),st(1)
        fstp dword ptr [edx+8]
        fstp st(0)
    }
}
#pragma warning(pop)
extern "C" void __fastcall GridBase_SetCameraAndOrigin(BYTE* self,void*,const DWORD* position,const float* direction,const DWORD* origin)
{
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,4+i*4)=position[i];
    float normal[3]; GridBase_Normalize(direction,normal);
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,16+i*4)=F(DWORD,normal,i*4);
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,28+i*4)=origin[i];
}
extern "C" DWORD __fastcall TerrainCell_SetTriangles(BYTE* self,void*,BYTE* first,BYTE* second)
{
    F(DWORD,self,16)=F(DWORD,self,12)=F(DWORD,self,8)=0;
    BYTE* triangles[2]={first,second}; BYTE* branches[2]={0,0};
    for(unsigned i=0;i<2;++i) if(triangles[i]) {
        BYTE* branch=(BYTE*)operator new(20);
        if(branch) for(unsigned j=0;j<16;j+=4) F(DWORD,branch,j)=0;
        F(BYTE*,branch,8)=self; F(BYTE*,branch,12)=triangles[i];
        F(DWORD,branch,16)=F(DWORD,self,16)+1;
        F(BYTE*,triangles[i],40)=branch; branches[i]=branch;
    }
    F(BYTE*,self,4)=branches[1]; F(BYTE*,self,0)=branches[0]; return 1;
}
extern "C" void __cdecl GridBase_InitScale() {g_gb_scale=(float)((double)g_gb_scaleInput+g_gb_scaleInput);}
extern "C" void __cdecl GridBase_InitSecondaryScale() {g_gb_secondary=(float)((double)g_gb_secondaryInput+g_gb_secondaryInput);}
extern "C" void __cdecl GridBase_InitBlend() {g_gb_blend=(float)(((double)g_gb_blendInput-g_gb_blendOffset)-g_gb_half);}
extern "C" void __cdecl GridBase_InitOrigin() {for(unsigned i=0;i<3;++i) g_gb_origin.v[i]=0;}
extern "C" void __cdecl GridBase_InitPending() {g_gb_pending=0;}
