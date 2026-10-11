// Initial marine mesh topology and grid defaults.
#include "king.h"
#include "x87.h"
#include <stdlib.h>
#include <math.h>
#include <mmsystem.h>
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
typedef char ConstructLabel[1];
ASM_VAR(ConstructLabel,g_constructBaseVtable,$L_652e60)
ASM_VAR(ConstructLabel,g_constructGridVtable,$L_6530d8)
ASM_VAR(ConstructLabel,g_constructTriangleVtable,$L_652e4c)
ASM_VAR(ConstructLabel,g_constructTriangleDataVtable,$L_652e48)
ASM_VAR(ConstructLabel,g_constructDate,$L_6724f8)
ASM_VAR(ConstructLabel,g_constructFile,$L_685c30)
ASM_VAR(ConstructLabel,g_constructError,$L_6724b0)
ASM_VAR(DWORD,g_constructNormalZ,$L_652e5c)
ASM_VAR(float,g_constructResolution,$L_653118)
ASM_VAR(float,g_constructHalf,$L_64c258)
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")
extern "C" int __fastcall MarineGreed_Allocate(BYTE*,void*,unsigned,unsigned);
extern "C" void __fastcall MarineGreed_Init(BYTE*,void*);
extern "C" void __fastcall MarineGreed_Update(BYTE*,void*);
extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE*,void*,BYTE*);
extern "C" void __cdecl MarineGrid_BuildRandomDirections(DWORD);
extern "C" void __fastcall MarineGrid_Trim(BYTE*,void*,DWORD);

extern "C" BYTE* __fastcall MarineGrid_ConstructBase(BYTE* self,void*)
{
    F(void*,self,0)=g_constructBaseVtable;
    for(unsigned i=4;i<=48;i+=4) F(DWORD,self,i)=0;
    F(DWORD,self,56)=F(DWORD,self,60)=0;
    F(DWORD,self,76)=F(DWORD,self,80)=0;
    F(DWORD,self,84)=0x3f800000;
    F(DWORD,self,88)=g_constructNormalZ;
    F(DWORD,self,100)=F(DWORD,self,104)=1092616192;
    F(DWORD,self,108)=1161527296;
    F(DWORD,self,112)=0;
    return self;
}

static BYTE* ConstructBranch()
{
    BYTE* p=(BYTE*)operator new(20);
    if(p) for(unsigned i=0;i<16;i+=4) F(DWORD,p,i)=0;
    return p;
}
extern "C" DWORD __fastcall MarineMesh_SetCellTriangles(BYTE* self,void*,BYTE* first,BYTE* second)
{
    F(DWORD,self,8)=F(DWORD,self,12)=F(DWORD,self,16)=0;
    BYTE* a=0; BYTE* b=0;
    if(first) {
        a=ConstructBranch(); F(BYTE*,a,8)=self; F(BYTE*,a,12)=first;
        F(DWORD,a,16)=F(DWORD,self,16)+1; F(BYTE*,first,32)=a;
    }
    if(second) {
        b=ConstructBranch(); F(BYTE*,b,8)=self; F(BYTE*,b,12)=second;
        F(DWORD,b,16)=F(DWORD,self,16)+1; F(BYTE*,second,32)=b;
    }
    F(BYTE*,self,4)=b; F(BYTE*,self,0)=a;
    return 1;
}
static BYTE* ConstructTriangle(BYTE* self,BYTE* a,BYTE* b,BYTE* c)
{
    BYTE* t=(BYTE*)operator new(60);
    if(t) {
        F(BYTE*,t,4)=F(BYTE*,t,8)=t+4;
        F(void*,t,12)=g_constructTriangleDataVtable;
        for(unsigned i=16;i<=32;i+=4) F(DWORD,t,i)=0;
        F(void*,t,0)=g_constructTriangleVtable;
        for(unsigned i=40;i<=56;i+=4) F(DWORD,t,i)=0;
    }
    F(BYTE*,t,16)=a; F(BYTE*,t,20)=b; F(BYTE*,t,24)=c;
    F(DWORD,t,36)=0; ++F(WORD,self,38);
    F(float,t,28)=(float)CallVirt<double>(F(BYTE*,self,4),36,a,b,c,t+12);
    MarineQueue_InsertNode(F(BYTE*,self,32),0,t);
    return t;
}
static void ConstructError(DWORD code,DWORD line)
{
    CallC(FUN_60aad0,code,g_constructError,g_constructFile,line,g_constructDate);
}
X87 double ConstructUnsigned(DWORD n) { volatile __int64 wide=(unsigned __int64)n; return (double)wide; }
extern "C" DWORD __fastcall MarineMesh_ConstructGrid(BYTE* self,void*,const DWORD* center,float width,float height,DWORD trim)
{
    DWORD nx=(DWORD)(__int64)ceil((double)width*g_constructResolution);
    DWORD ny=(DWORD)(__int64)ceil((double)height*g_constructResolution);
    DWORD cells=nx*ny,vertices=(nx+1)*(ny+1),triangles=cells*2;
    BYTE* grid=(BYTE*)operator new(204);
    if(grid) {
        MarineGrid_ConstructBase(grid,0);
        F(void*,grid,0)=g_constructGridVtable;
        const unsigned zero[]={120,136,128,124,140,132,52,112};
        for(unsigned i=0;i<8;++i) F(DWORD,grid,zero[i])=0;
        MarineGrid_BuildRandomDirections(timeGetTime()); F(DWORD,grid,200)=0;
    }
    F(float,grid,160)=width; F(float,grid,164)=height;
    F(DWORD,grid,168)=center[0]; F(DWORD,grid,172)=center[1];
    double halfWidth=(double)width*g_constructHalf;
    F(float,grid,176)=(float)(F(float,grid,168)-halfWidth);
    F(float,grid,180)=(float)(halfWidth+F(float,grid,168));
    double halfHeight=(double)height*g_constructHalf;
    F(float,grid,184)=(float)(halfHeight+F(float,grid,172));
    F(float,grid,188)=(float)(F(float,grid,172)-halfHeight);
    DWORD sx=nx<<7,sy=ny<<7;
    F(float,grid,192)=(float)((double)width/ConstructUnsigned(sx));
    F(float,grid,196)=(float)((double)height/ConstructUnsigned(sy));
    CallVirt(grid,20,sx,sy);
    F(BYTE*,self,4)=grid;
    DWORD count=CallVirt<DWORD>(grid,12);
    MarineGreed_Allocate(self,0,count,cells); MarineGreed_Init(self,0);
    for(DWORD i=0;i<cells;++i) {
        BYTE* cell=ConstructBranch(); F(BYTE**,self,8)[i]=cell;
        if(!cell) return 0;
    }
    BYTE** points=(BYTE**)malloc(vertices*4);
    BYTE** faces=(BYTE**)malloc(triangles*4);
    if(!points||!faces) ConstructError(2289444628u,50);
    for(DWORD x=0;x<=nx;++x) for(DWORD y=0;y<=ny;++y) {
        BYTE* p=(BYTE*)operator new(12);
        F(BYTE**,self,12)[F(WORD,self,36)]=p;
        F(DWORD,p,0)=x<<7; F(DWORD,p,4)=y<<7;
        F(WORD,p,8)=F(WORD,self,36); ++F(WORD,self,36);
        points[y*(nx+1)+x]=p;
    }
    DWORD at=0;
    for(DWORD x=1;x<=nx;++x) for(DWORD y=1;y<=ny;++y) {
        BYTE* a=points[(y-1)*(nx+1)+x-1]; BYTE* b=points[(y-1)*(nx+1)+x];
        BYTE* c=points[y*(nx+1)+x-1]; BYTE* d=points[y*(nx+1)+x];
        if(((x^y)&1)==0) {
            faces[at]=ConstructTriangle(self,d,b,a);
            faces[at+1]=ConstructTriangle(self,a,c,d);
        } else {
            faces[at]=ConstructTriangle(self,b,a,c);
            faces[at+1]=ConstructTriangle(self,c,d,b);
        }
        at+=2;
    }
    at=0;
    for(DWORD x=1;x<=nx;++x) for(DWORD y=1;y<=ny;++y) {
        BYTE* a=faces[at]; BYTE* b=faces[at+1];
        F(BYTE*,a,44)=b; F(BYTE*,b,44)=a;
        if(((x^y)&1)==0) {
            F(BYTE*,a,48)=x==nx?0:faces[at+ny*2];
            F(BYTE*,a,52)=y==1?0:faces[at-1];
            F(BYTE*,b,48)=x==1?0:faces[at-ny*2+1];
            F(BYTE*,b,52)=y==ny?0:faces[at+2];
        } else {
            F(BYTE*,a,48)=y==1?0:faces[at-1];
            F(BYTE*,a,52)=x==1?0:faces[at-ny*2];
            F(BYTE*,b,48)=y==ny?0:faces[at+2];
            F(BYTE*,b,52)=x==nx?0:faces[at+ny*2+1];
        }
        at+=2;
    }
    for(DWORD i=0;i<cells;++i) MarineMesh_SetCellTriangles(F(BYTE**,self,8)[i],0,faces[i*2],faces[i*2+1]);
    BYTE* list=F(BYTE*,self,32);
    BYTE* node=F(BYTE*,list,0); if(!node) node=F(BYTE*,list,4);
    while(node) {
        BYTE* p=F(BYTE*,node,48); if(p&&F(BYTE*,p,52)!=node) ConstructError(2289444625u,136);
        p=F(BYTE*,node,52); if(p&&F(BYTE*,p,48)!=node) ConstructError(2289444625u,140);
        p=F(BYTE*,node,44); if(!p||F(BYTE*,p,44)!=node) ConstructError(2289444625u,144);
        list=F(BYTE*,self,32); BYTE* first=F(BYTE*,list,0); BYTE* last=F(BYTE*,list,4);
        if(first&&F(BYTE*,first,8)==node+4) node=last;
        else if(last&&F(BYTE*,last,8)==node+4) break;
        else { node=F(BYTE*,node,4); if(node) node-=4; }
    }
    MarineGrid_Trim(self,0,trim);
    grid=F(BYTE*,self,4); CallVirt(grid,4,self+56,self+68);
    F(DWORD,self,24)=F(DWORD,self,20)=1; MarineGreed_Update(self,0);
    free(points); free(faces); return 1;
}
