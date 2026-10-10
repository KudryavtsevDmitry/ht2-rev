// Marine grid setup, drawing, camera tracking and boundary trimming.
#include "king.h"
#include "x87.h"
#include <stdlib.h>
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
typedef char GridLabel[1];
ASM_VAR(GridLabel, g_gridVtable, $L_6530d8)
ASM_VAR(GridLabel, g_gridBaseVtable, $L_652ea0)
ASM_VAR(GridLabel, g_gridDate, $L_6724f8)
ASM_VAR(GridLabel, g_gridFile, $L_685c54)
ASM_VAR(GridLabel, g_gridTrimFile, $L_685c30)
ASM_VAR(GridLabel, g_gridTopologyFile, $L_6849fc)
ASM_VAR(GridLabel, g_gridError, $L_6724b0)
ASM_VAR(GridLabel, g_gridAssert, $L_6726a4)
ASM_VAR(GridLabel, g_gridClassificationError, $L_6726e0)
ASM_VAR(GridLabel, g_gridMissingMesh, $L_685c74)
ASM_VAR(BYTE*, g_gridState, $L_725e08)
ASM_VAR(DWORD, g_gridCapacity, $L_725e38)
ASM_VAR(DWORD, g_gridVertices, $L_725e44)
ASM_VAR(DWORD, g_gridTrimLimit, $L_725e4c)
ASM_VAR(int, g_gridBudget, $L_725e58)
ASM_VAR(float, g_gridZero, $L_64c264)
ASM_VAR(float, g_gridOne, $L_64c250)
ASM_VAR(float, g_gridRandomUnit, $L_64c254)
ASM_VAR(float, g_gridTrackUpper, $L_653134)
ASM_VAR(float, g_gridTrackLower, $L_653130)
ASM_VAR(float, g_gridRandomRadius, $L_653220)
// One backing view preserves the original overlapping copies and table gaps.
struct GridNoiseStorage { DWORD v[2600]; };
ASM_VAR(GridNoiseStorage, g_gridNoise, $L_725fd0)
#define GRID_NOISE(offset) (g_gridNoise.v+(offset))
ASM_PROC(FUN_5e48f0)
ASM_PROC(FUN_60bb20)
ASM_PROC(FUN_60aad0)
ASM_PROC(FUN_60aa60)
extern "C" void FUN_605810(); KING_ALIAS("_FUN_605810", "@Stack_Top@8")
ASM_PROC(FUN_5b1020)
ASM_PROC(FUN_605900)
ASM_PROC(FUN_6064c0)
extern "C" BYTE* __fastcall MarineParticlePool_Construct(BYTE*,void*,DWORD,DWORD,DWORD,DWORD);
extern "C" void __fastcall MarineParticlePool_SetFloor(BYTE*,void*,DWORD);
extern "C" void __fastcall MarineParticlePool_SetLifetime(BYTE*,void*,DWORD);
extern "C" void __fastcall MarineParticlePool_Render(BYTE*,void*,BYTE*,BYTE*,DWORD);
extern "C" DWORD __fastcall MarineGrid_ClassifyTriangle(BYTE*,void*,BYTE*);
extern "C" void __fastcall MarineGreed_RemoveNode(BYTE*,void*,BYTE*);
extern "C" DWORD __fastcall MarineMesh_DrainEdges(BYTE*,void*);
extern "C" DWORD __fastcall MarineMesh_PruneVertices(BYTE*,void*);

extern "C" void __fastcall MarineGrid_Destruct(BYTE* self,void*)
{
    F(void*,self,0)=g_gridVtable;
    const unsigned offsets[]={124,140,132};
    for(unsigned i=0;i<3;++i) {
        void* buffer=F(void*,self,offsets[i]);
        if(buffer) { operator delete(buffer); F(void*,self,offsets[i])=0; }
    }
    BYTE* material=F(BYTE*,self,200);
    if(material) { CallThis(FUN_5e48f0,material); operator delete(material); F(void*,self,200)=0; }
    BYTE* pool=F(BYTE*,self,112);
    if(pool) {
        void* particles=F(void*,pool,12);
        if(particles) { operator delete(particles); F(void*,pool,12)=0; }
        operator delete(pool); F(void*,self,112)=0;
    }
    F(void*,self,0)=g_gridBaseVtable;
    BYTE* child=F(BYTE*,self,52);
    while(child) { CallVirt(child,44); CallVirt(child,48); child=F(BYTE*,child,52); }
}
extern "C" BYTE* __fastcall MarineGrid_Delete(BYTE* self,void*,DWORD flags)
{
    MarineGrid_Destruct(self,0);
    if(flags&1) operator delete(self);
    return self;
}
extern "C" BYTE* __fastcall MarineGrid_WriteVertex(BYTE* self,void*,const DWORD* position,const DWORD* normal,DWORD a,DWORD b,DWORD c,DWORD d,DWORD e,DWORD f)
{
    // Keep sequential copies: callers can pass overlapping records.
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,i*4)=position[i];
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,12+i*4)=normal[i];
    F(DWORD,self,24)=a;F(DWORD,self,28)=b;F(DWORD,self,32)=c;
    F(DWORD,self,36)=d;F(DWORD,self,40)=e;F(DWORD,self,44)=f;
    return self;
}
extern "C" DWORD __fastcall MarineGrid_Allocate(BYTE* self,void*,DWORD width,DWORD height)
{
    F(DWORD,self,48)=height;F(DWORD,self,44)=width;
    F(void*,self,124)=malloc(CallVirt<DWORD>(self,12)*144u+1u);
    F(void*,self,132)=malloc(CallVirt<DWORD>(self,12)*8u);
    F(void*,self,140)=malloc(CallVirt<DWORD>(self,12)*8u);
    if(!F(void*,self,124)||!F(void*,self,132)) {
        CallC(FUN_60aad0,0x88762714u,g_gridError,g_gridFile,278,g_gridDate);return 0;
    }
    CallC(FUN_60bb20);
    F(DWORD,self,152)=F(DWORD,self,156)=1189765120u;
    BYTE* pool=(BYTE*)operator new(36);
    if(pool) pool=MarineParticlePool_Construct(pool,0,g_gridCapacity,g_gridVertices,16777215u,(DWORD)self);
    F(BYTE*,self,112)=pool;
    MarineParticlePool_SetFloor(pool,0,3204448256u);
    MarineParticlePool_SetLifetime(F(BYTE*,self,112),0,8000);
    return 1;
}
static void MarineGrid_DrawBuffer(BYTE* self,BYTE* device,BYTE* event,unsigned countOffset,unsigned bufferOffset,unsigned line)
{
    DWORD material=CallThis<DWORD>(FUN_605810,F(BYTE*,self,200));
    CallThis(FUN_5b1020,event,material,1,0,0);
    LONG result=CallStd<LONG>((void(*)())F(void*,F(void*,device,0),104),device,4,786,
        F(void*,self,124),F(DWORD,self,120),F(void*,self,bufferOffset),F(DWORD,self,countOffset),0);
    if(result<0) CallC(FUN_60aad0,result,g_gridError,g_gridFile,line,g_gridDate);
}
extern "C" void __fastcall MarineGrid_Draw(BYTE* self,void*,BYTE* device,BYTE* event)
{
    CallStd((void(*)())F(void*,F(void*,device,0),80),device,136,1);
    BYTE* source=F(BYTE*,g_gridState,488);
    CallVirt(event,8,F(DWORD,F(void*,source,76),36));
    MarineGrid_DrawBuffer(self,device,event,128,132,331);
    MarineGrid_DrawBuffer(self,device,event,136,140,308);
    CallVirt(event,8,F(DWORD,g_gridState,468));
    MarineParticlePool_Render(F(BYTE*,self,112),0,device,event,F(DWORD,self,116));
}
extern "C" __declspec(noinline) double __cdecl MarineGrid_QualityScale(float,float) { return g_gridOne; }
extern "C" DWORD __fastcall MarineGrid_DesiredWork(BYTE* self,void*)
{
    double dx=(double)F(float,self,4)-F(float,self,168);
    double dy=(double)F(float,self,8)-F(float,self,172);
    float distance=(float)x87_sqrt(((dx*dx+dy*dy)+F(float,self,12))+F(float,self,12));
    int budget=g_gridBudget;
    return (DWORD)(__int64)(MarineGrid_QualityScale(distance,4096.0f)*budget);
}
extern "C" void __fastcall MarineGrid_TrackCamera(BYTE* self,void*,const DWORD* position,const DWORD* direction)
{
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,4+i*4)=position[i];
    for(unsigned i=0;i<3;++i) F(volatile DWORD,self,16+i*4)=direction[i];
    F(float,self,144)=(float)(((double)*(float*)position-F(float,self,176))/F(float,self,192));
    F(float,self,148)=(float)(((double)((float*)position)[1]-F(float,self,188))/F(float,self,196));
    volatile __int64 width=F(DWORD,self,44),height=F(DWORD,self,48);
    double bound=(double)width;
    if(!(bound>=F(float,self,144))) F(float,self,144)=(float)bound;
    bound=(double)height;
    if(!(bound>=F(float,self,148))) F(float,self,148)=(float)bound;
    if(!(F(float,self,144)>=g_gridZero)) F(DWORD,self,144)=0;
    if(!(F(float,self,148)>=g_gridZero)) F(DWORD,self,148)=0;
    double delta=(double)F(float,self,144)-F(float,self,152);
    if(delta>(double)F(float,self,192)*g_gridTrackUpper || !(delta>=(double)F(float,self,192)*g_gridTrackLower))
        F(DWORD,self,152)=F(DWORD,self,144);
    delta=(double)F(float,self,148)-F(float,self,156);
    // The original lower Y threshold uses X scale (+192).
    if(delta>(double)F(float,self,196)*g_gridTrackUpper || !(delta>=(double)F(float,self,192)*g_gridTrackLower))
        F(DWORD,self,156)=F(DWORD,self,148);
}
extern "C" DWORD __fastcall MarineGrid_TrimBranch(BYTE* self,void*,BYTE* branch)
{
    BYTE* triangle=F(BYTE*,branch,12);
    if(triangle) {
        DWORD kind=MarineGrid_ClassifyTriangle(self,0,triangle);
        if(kind==1) { MarineGreed_RemoveNode(self,0,F(BYTE*,branch,12));return 1; }
        if(kind==4) return 1;
        if(kind!=2) {
            CallC(FUN_60aa60,2,g_gridAssert,g_gridClassificationError,g_gridTrimFile,264,g_gridDate);return 0;
        }
        if(F(DWORD,F(void*,branch,12),36)>=10) { MarineGreed_RemoveNode(self,0,F(BYTE*,branch,12));return 1; }
        CallC(FUN_605900,branch,self);
    }
    DWORD left=0,right=0;
    BYTE* child=F(BYTE*,branch,0);
    if(child) left=MarineGrid_TrimBranch(self,0,child);
    child=F(BYTE*,branch,4);
    if(child) right=MarineGrid_TrimBranch(self,0,child);
    return left&right;
}
extern "C" void __fastcall MarineGrid_Trim(BYTE* self,void*,DWORD argument)
{
    if(!F(DWORD,self,4)) CallC(FUN_60aa60,2,g_gridAssert,g_gridMissingMesh,g_gridTrimFile,270,g_gridDate);
    CallC(FUN_5e48f0,argument);
    DWORD saved=g_gridTrimLimit;g_gridTrimLimit=10;
    for(DWORD i=0;i<F(DWORD,self,48);++i) {
        BYTE* pair=F(BYTE*,F(void*,self,8),i*4);
        MarineGrid_TrimBranch(self,0,F(BYTE*,pair,0));MarineMesh_DrainEdges(self,0);
        pair=F(BYTE*,F(void*,self,8),i*4);
        MarineGrid_TrimBranch(self,0,F(BYTE*,pair,4));MarineMesh_DrainEdges(self,0);
    }
    CallC(FUN_5e48f0);g_gridTrimLimit=saved;
    BYTE* queue=F(BYTE*,self,32);
    BYTE* node=F(BYTE*,queue,0);if(!node) node=F(BYTE*,queue,4);
    while(node) {
        BYTE* left=F(BYTE*,node,48);BYTE* right=F(BYTE*,node,52);
        if(!F(DWORD,node,56)&&(left||right)) {
            BYTE* edge=F(BYTE*,F(void*,node,32),8);
            BYTE* a=F(BYTE*,edge,0);BYTE* b=F(BYTE*,edge,4);
            if(a&&b) {
                BYTE* bn=F(BYTE*,b,12);BYTE* an=F(BYTE*,a,12);
                bool candidate=false;BYTE* replacement=0;
                if(bn!=node&&an!=node) CallC(FUN_60aad0,0x88762712u,g_gridError,g_gridTopologyFile,276,g_gridDate);
                else if(an==node) {
                    if(!right) candidate=!F(DWORD,left,48);
                    else if(F(BYTE*,right,52)) {
                        replacement=F(BYTE*,F(void*,right,32),8);
                        candidate=replacement==F(BYTE*,F(void*,F(void*,right,52),32),8);
                    }
                } else if(!left) candidate=!F(DWORD,right,52);
                else if(F(BYTE*,left,48)) {
                    replacement=F(BYTE*,F(void*,left,32),8);
                    candidate=replacement==F(BYTE*,F(void*,F(void*,left,48),32),8);
                }
                if(candidate) {
                    BYTE* extra=F(BYTE*,edge,8);
                    if(replacement||(extra&&(!F(DWORD,extra,0)||!F(DWORD,extra,4))))
                        CallThis(FUN_6064c0,self,edge,replacement);
                }
            }
        }
        queue=F(BYTE*,self,32);BYTE* first=F(BYTE*,queue,0);BYTE* last=F(BYTE*,queue,4);
        if(first&&F(BYTE*,first,8)==node+4) node=last;
        else if(last&&F(BYTE*,last,8)==node+4) break;
        else { BYTE* next=F(BYTE*,node,4);node=next?next-4:0; }
    }
    MarineMesh_DrainEdges(self,0);MarineMesh_PruneVertices(self,0);
}
extern "C" void __cdecl MarineGrid_BuildRandomDirections(DWORD seed)
{
    srand(seed);
    for(unsigned i=0;i<256;++i) {
        float x,y,z,length;
        for(;;) {
            float first=(float)((double)rand()*g_gridRandomUnit);
            float second=(float)((double)rand()*g_gridRandomUnit);
            float third=(float)((double)rand()*g_gridRandomUnit);
            float fourth=(float)((double)rand()*g_gridRandomUnit);
            double fifth=(double)rand()*g_gridRandomUnit;
            x=(float)((double)fourth-fifth);y=(float)((double)second-third);
            double sixth=(double)rand()*g_gridRandomUnit-first;z=(float)sixth;
            double squared=(sixth*z+(double)y*y)+(double)x*x;
            if(!(squared>g_gridOne)) { length=(float)x87_sqrt(squared);break; }
        }
        double radius=(double)rand()*g_gridRandomUnit;
        double scaled=(double)g_gridRandomRadius*radius;
        F(float,GRID_NOISE(2056),i*4)=(float)(scaled+scaled);
        F(float,GRID_NOISE(0),i*12)=(float)((double)x/length);
        F(float,GRID_NOISE(0),i*12+4)=(float)((double)y/length);
        F(float,GRID_NOISE(0),i*12+8)=(float)((double)z/length);
    }
    for(unsigned i=0;i<256;++i) GRID_NOISE(1542)[i]=i;
    for(int i=256;i>0;i-=2) {
        DWORD previous=GRID_NOISE(1542)[i];int index=rand()%256;
        GRID_NOISE(1542)[i]=GRID_NOISE(1542)[index];GRID_NOISE(1542)[index]=previous;
    }
    // The original copies 258 records, repeating the first two source records.
    for(unsigned i=0;i<258;++i) {
        GRID_NOISE(1798)[i]=GRID_NOISE(1542)[i];
        for(unsigned j=0;j<3;++j) GRID_NOISE(768)[i*3+j]=GRID_NOISE(0)[i*3+j];
        GRID_NOISE(2312)[i]=GRID_NOISE(2056)[i];
    }
}
