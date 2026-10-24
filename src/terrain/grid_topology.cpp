// Triangle/point allocation and paired refinement edges.
#include "king.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
typedef char TopologyLabel[1];
ASM_VAR(TopologyLabel,g_topTriangleVtable,$L_652e4c)
ASM_VAR(TopologyLabel,g_topDataVtable,$L_652e48)
ASM_VAR(TopologyLabel,g_topDate,$L_6724f8)
ASM_VAR(TopologyLabel,g_topFile,$L_684a20)
ASM_VAR(TopologyLabel,g_topError,$L_6724b0)
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0", "_Grd_ReportError")
extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE*,void*,BYTE*);
extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE*,void*,BYTE*);
extern "C" BYTE* __fastcall MarineQueue_RemoveLink(BYTE*,void*,BYTE*);

extern "C" DWORD __fastcall GridTriangle_SetPoints(BYTE* self,void*,DWORD a,DWORD b,DWORD c,DWORD owner)
{
    F(volatile DWORD,self,4)=a; F(volatile DWORD,self,8)=b;
    F(volatile DWORD,self,12)=c; F(volatile DWORD,self,20)=owner; return 1;
}
extern "C" BYTE* __fastcall GridEdge_Construct(BYTE* self,void*)
{
    F(BYTE*,self,4)=F(BYTE*,self,0)=self;
    const unsigned records[]={16,44,76};
    for(unsigned i=0;i<3;++i) {
        F(void*,self,records[i])=g_topDataVtable;
        for(unsigned j=4;j<=20;j+=4) F(DWORD,self,records[i]+j)=0;
    }
    return self;
}
static bool GridEdge_ValidPair(BYTE* pair)
{
    if(!F(BYTE*,pair,8)||!F(BYTE*,pair,0)||!F(BYTE*,pair,4)) return false;
    return F(BYTE*,F(BYTE*,pair,4),12)&&F(BYTE*,F(BYTE*,pair,0),12);
}
static void GridEdge_OrientPair(BYTE* self,unsigned offset,BYTE* pair)
{
    F(BYTE*,self,offset)=pair;
    BYTE* left=F(BYTE*,pair,0); BYTE* right=F(BYTE*,pair,4);
    if(F(BYTE*,F(BYTE*,left,12),48)!=F(BYTE*,right,12)) {
        F(BYTE*,pair,0)=right;
        F(BYTE*,F(BYTE*,self,offset),4)=left;
    }
}
extern "C" DWORD __fastcall GridEdge_SetPairs(BYTE* self,void*,BYTE* first,BYTE* second)
{
    if(!GridEdge_ValidPair(first)) return 0;
    GridEdge_OrientPair(self,8,first);
    BYTE* pair=F(BYTE*,self,8);
    BYTE* left=F(BYTE*,F(BYTE*,pair,0),12); BYTE* right=F(BYTE*,F(BYTE*,pair,4),12);
    GridTriangle_SetPoints(self+16,0,F(DWORD,left,24),F(DWORD,left,16),F(DWORD,right,16),(DWORD)pair);
    F(DWORD,self,72)=F(DWORD,self,32); F(DWORD,self,12)=0;
    if(!second) return 1;
    if(!GridEdge_ValidPair(second)) return 0;
    GridEdge_OrientPair(self,12,second); pair=F(BYTE*,self,12);
    left=F(BYTE*,F(BYTE*,pair,0),12); right=F(BYTE*,F(BYTE*,pair,4),12);
    GridTriangle_SetPoints(self+44,0,F(DWORD,right,16),F(DWORD,right,24),F(DWORD,left,24),(DWORD)pair);
    volatile double score=!(F(float,self,60)>=F(float,self,72))?F(float,self,72):F(float,self,60);
    F(float,self,72)=(float)score; return 1;
}
extern "C" void __fastcall GridTriangle_Destruct(BYTE* self,void*)
{
    F(void*,self,0)=g_topTriangleVtable; F(void*,self,12)=g_topDataVtable;
}
extern "C" BYTE* __fastcall GridMesh_CreateTriangle(BYTE* self,void*,DWORD a,DWORD b,DWORD c,DWORD depth)
{
    BYTE* t=(BYTE*)operator new(60);
    if(t) {
        F(BYTE*,t,4)=F(BYTE*,t,8)=t+4; F(void*,t,12)=g_topDataVtable;
        for(unsigned i=16;i<=32;i+=4) F(DWORD,t,i)=0;
        for(unsigned i=40;i<=56;i+=4) F(DWORD,t,i)=0;
        F(void*,t,0)=g_topTriangleVtable;
    }
    F(DWORD,t,24)=c; F(DWORD,t,16)=a; F(DWORD,t,20)=b; F(DWORD,t,36)=depth;
    ++F(WORD,self,38);
    F(float,t,28)=(float)CallVirt<double>(F(BYTE*,self,4),36,a,b,c,t+12);
    MarineQueue_InsertNode(F(BYTE*,self,32),0,t); return t;
}
extern "C" BYTE* __fastcall GridMesh_CreatePoint(BYTE* self,void*,const DWORD* point)
{
    BYTE* p=(BYTE*)operator new(12);
    F(BYTE**,self,12)[F(WORD,self,36)]=p;
    F(volatile DWORD,p,0)=point[0]; F(volatile DWORD,p,4)=point[1];
    F(WORD,p,8)=F(WORD,self,36); ++F(WORD,self,36); return p;
}
extern "C" BYTE* __fastcall GridMesh_RemoveTriangle(BYTE* self,void*,BYTE* triangle)
{
    if(F(BYTE*,self,40)==triangle) {
        BYTE* list=F(BYTE*,self,32); BYTE* first=F(BYTE*,list,0); BYTE* last=F(BYTE*,list,4);
        BYTE* link=triangle?triangle+4:0; BYTE* next=0;
        if(first&&F(BYTE*,first,8)==link) next=last;
        else if(!(last&&F(BYTE*,last,8)==link)) {next=F(BYTE*,triangle,4);if(next)next-=4;}
        F(BYTE*,self,40)=next;
    }
    --F(WORD,self,38); return MarineQueue_RemoveNode(F(BYTE*,self,32),0,triangle);
}
extern "C" BYTE* __fastcall GridMesh_RemoveEdge(BYTE* self,void*,BYTE* edge)
{
    if(F(BYTE*,self,44)==edge) {
        BYTE* list=F(BYTE*,self,28); BYTE* first=F(BYTE*,list,0); BYTE* last=F(BYTE*,list,4);
        BYTE* next=first&&F(BYTE*,first,4)==edge?last:(last&&F(BYTE*,last,4)==edge?0:F(BYTE*,edge,0));
        F(BYTE*,self,44)=next;
    }
    --F(WORD,self,52);
    BYTE* pair=F(BYTE*,edge,8);
    F(DWORD,F(BYTE*,F(BYTE*,pair,0),12),56)=0;
    F(DWORD,F(BYTE*,F(BYTE*,pair,4),12),56)=0;
    pair=F(BYTE*,edge,12);
    if(pair) {
        F(DWORD,F(BYTE*,F(BYTE*,pair,0),12),56)=0;
        pair=F(BYTE*,edge,12);
        F(DWORD,F(BYTE*,F(BYTE*,pair,4),12),56)=0;
    }
    return MarineQueue_RemoveLink(F(BYTE*,self,28),0,edge);
}
extern "C" void __fastcall GridEdge_Destruct(BYTE* self,void*)
{
    F(void*,self,76)=F(void*,self,44)=F(void*,self,16)=g_topDataVtable;
}
extern "C" void __fastcall GridBranch_Destruct(BYTE* self,void*)
{
    BYTE* parent=F(BYTE*,self,8);
    if(parent) {
        if(F(BYTE*,parent,0)==self) F(BYTE*,parent,0)=0;
        else if(F(BYTE*,parent,4)==self) F(BYTE*,parent,4)=0;
        else CallC(FUN_60aad0,2289444626u,g_topError,g_topFile,35,g_topDate);
    }
    BYTE* child=F(BYTE*,self,0);
    if(child) {GridBranch_Destruct(child,0);operator delete(child);}
    child=F(BYTE*,self,4);
    if(child) {GridBranch_Destruct(child,0);operator delete(child);}
}

// ---------------------------------------------------------------------------
// x87 functions of grd.cpp

#include "x87.h"
extern "C" void __fastcall MarineQueue_InsertEdge(BYTE*,void*,BYTE*);

// [FUN_6064c0] a new refinement edge on one or two triangle pairs, scored by the mesh (virtual +32) and queued
extern "C" BYTE* __fastcall GridMesh_CreateEdge(BYTE* self,void*,BYTE* first,BYTE* second)
{
    BYTE* o=(BYTE*)operator new(108);
    if(o) GridEdge_Construct(o,0);
    if(!GridEdge_SetPairs(o,0,first,second)) {
        if(o) {
            F(void*,o,76)=g_topDataVtable; F(void*,o,44)=g_topDataVtable; F(void*,o,16)=g_topDataVtable;
            operator delete(o);
        }
        return 0;
    }
    F(BYTE*,F(BYTE*,F(BYTE*,first,0),12),56)=o; F(BYTE*,F(BYTE*,F(BYTE*,first,4),12),56)=o;
    if(second) {
        F(BYTE*,F(BYTE*,F(BYTE*,second,0),12),56)=o; F(BYTE*,F(BYTE*,F(BYTE*,second,4),12),56)=o;
    }
    double score;
    if(F(DWORD,o,12))
        score=CallVirt<double>(F(BYTE*,self,4),32,F(DWORD,o,20),F(DWORD,o,24),F(DWORD,o,28),o+16,
                               F(DWORD,o,48),F(DWORD,o,52),F(DWORD,o,56),o+44);
    else
        score=CallVirt<double>(F(BYTE*,self,4),32,F(DWORD,o,20),F(DWORD,o,24),F(DWORD,o,28),o+16,
                               (DWORD)0,(DWORD)0,(DWORD)0,(BYTE*)0);
    F(float,o,72)=(float)score;
    MarineQueue_InsertEdge(F(BYTE*,self,28),0,o);
    ++F(WORD,self,52);
    return o;
}

extern "C" int __fastcall TGrid_Set(BYTE* self,void*,int a,int b,int c,BYTE* d);   // terrain_grid.cpp

static void TGridEdge_OrientPair(BYTE* self,unsigned offset,BYTE* pair)
{
    F(BYTE*,self,offset)=pair;
    BYTE* left=F(BYTE*,pair,0); BYTE* right=F(BYTE*,pair,4);
    if(F(BYTE*,F(BYTE*,left,12),64)!=F(BYTE*,right,12)) {
        F(BYTE*,pair,0)=right;
        F(BYTE*,F(BYTE*,self,offset),4)=left;
    }
}

// [FUN_608030] the terrain grid's edge: its one or two triangle pairs
extern "C" int __fastcall TGridEdge_SetPairs(BYTE* self,void*,BYTE* first,BYTE* second)
{
    if(!GridEdge_ValidPair(first)) return 0;
    TGridEdge_OrientPair(self,8,first);
    BYTE* pair=F(BYTE*,self,8);
    BYTE* left=F(BYTE*,F(BYTE*,pair,0),12); BYTE* right=F(BYTE*,F(BYTE*,pair,4),12);
    TGrid_Set(self+16,0,F(int,left,24),F(int,left,16),F(int,right,16),pair);
    F(DWORD,self,104)=F(DWORD,self,32); F(DWORD,self,12)=0;
    if(!second) return 1;
    if(!GridEdge_ValidPair(second)) return 0;
    TGridEdge_OrientPair(self,12,second); pair=F(BYTE*,self,12);
    left=F(BYTE*,F(BYTE*,pair,0),12); right=F(BYTE*,F(BYTE*,pair,4),12);
    TGrid_Set(self+60,0,F(int,right,16),F(int,right,24),F(int,left,24),pair);
    volatile double score=!(F(float,self,76)>=F(float,self,104))?F(float,self,104):F(float,self,76);
    F(float,self,104)=(float)score; return 1;
}

// [FUN_60c120] how far (x, y) lies outside a circle {x, y, radius}; 0 inside
extern "C" double __fastcall Circle_Outside(const float* c,void*,float x,float y)
{
    double dy=(double)c[1]-y;
    double dx=(double)c[0]-x;
    double r=x87_sqrt(dx*dx+dy*dy)-c[2];
    if(!(r>0)) return 0.0f;
    return r;
}

ASM_VAR(int,g_grdRefine,$L_685a98)
ASM_VAR(TopologyLabel,g_tgrid72c108,$L_72c108)
extern "C" int __fastcall MarineQueue_Empty(BYTE*,void*);
extern "C" BYTE* __fastcall MarineQueue_TopWorkItem(BYTE*,void*);
extern "C" BYTE* __fastcall MarineQueue_TopDeferredNode(BYTE*,void*);
extern "C" int __fastcall TGrid_Get36(BYTE* self,void*);
ASM_PROC(FUN_607410)
extern "C" void FUN_609de0(); KING_ALIAS("_FUN_609de0", "_GridTree_RemoveEdge")

// [FUN_60c160] up to 50 refinement steps while the deferred queue's top beats the work queue's; whether it still does
extern "C" int __fastcall GridMesh_Refine50(BYTE* self,void*)
{
    if(!g_grdRefine||MarineQueue_Empty(F(BYTE*,self,32),0)) return 0;
    for(unsigned i=0;!MarineQueue_Empty(F(BYTE*,self,32),0)&&i<50;++i) {
        float lim=F(float,MarineQueue_TopWorkItem(F(BYTE*,self,32),0),104);
        if(!(F(float,MarineQueue_TopDeferredNode(F(BYTE*,self,36),0),28)>lim)) break;
        MarineQueue_TopWorkItem(F(BYTE*,self,32),0);
        if(F(float,MarineQueue_TopDeferredNode(F(BYTE*,self,36),0),28)>=0.0002
           && (unsigned)F(WORD,self,42)<=(unsigned)TGrid_Get36((BYTE*)g_tgrid72c108,0))
            CallC(FUN_607410,F(DWORD,MarineQueue_TopDeferredNode(F(BYTE*,self,36),0),40),self);
        else
            CallC(FUN_609de0,MarineQueue_TopWorkItem(F(BYTE*,self,32),0),self);
    }
    if(MarineQueue_Empty(F(BYTE*,self,32),0)) return 0;
    float lim=F(float,MarineQueue_TopWorkItem(F(BYTE*,self,32),0),104);
    return F(float,MarineQueue_TopDeferredNode(F(BYTE*,self,36),0),28)>lim;
}

// [FUN_608460] whether (x, y) lies on the inner side of two edges through a point {x, y, -h}
extern "C" int __fastcall Point_InWedge(const float* s,void*,float x,float y,float a,float b,float c,float d)
{
    double dx=(double)s[0]-x, dy=(double)s[1]-y, nz=-(double)s[2];
    if(!(dy*a-dx*b>=nz)) return 0;
    if(!(dy*c-dx*d>=nz)) return 0;
    return 1;
}

// [FUN_6097d0] a node's refinement priority from its distance to the camera circle and the view wedges
extern "C" double __fastcall GridCamera_NodePriority(BYTE* s,void*,BYTE* node)
{
    float px=F(float,node,32), py=F(float,node,36), pz=F(float,node,40);
    double dy=(double)py-F(float,s,8);
    double dx=(double)px-F(float,s,4);
    double dd=x87_sqrt(dx*dx+dy*dy)-pz;
    float d=(float)dd;
    if(!(dd>0.0f)) return 1000.0f;
    if(d>1500.0f) return 9.999999747378752e-06f/(double)d;
    if(!(d>=50.0f)) return 1000.0f;
    double ex=(double)px-F(float,s,4);
    float exf=(float)ex;
    double ey=(double)py-F(float,s,8);
    float eyf=(float)ey;
    float mz=-pz;
    if(!(ey*F(float,s,60)-ex*F(float,s,64)>=mz)) return 9.999999747378752e-06f/(double)d;
    if(!(ey*F(float,s,68)-ex*F(float,s,72)>=mz)) return 9.999999747378752e-06f/(double)d;
    if(F(unsigned,node,24)<8) return 10.0f/((double)d+1.0f);
    double base=(double)F(float,node,20)/d+0.05000000074505806f;
    if(!(d>=500.0f)) return base;
    if((double)eyf*F(float,s,76)-(double)exf*F(float,s,80)>=mz
       && (double)eyf*F(float,s,84)-(double)exf*F(float,s,88)>=mz)
        return base;
    return base*0.5f;
}

#include <mmsystem.h>
ASM_PROC(FUN_60be40)
extern "C" int __fastcall MarineTransition_Advance(BYTE* self, void*, DWORD now);
extern "C" void __fastcall MarineTask_ClearRegistration(BYTE* self, void*);
extern "C" void __fastcall MarineTask_SetFirstBounds(BYTE* self, void*, float a, float b);
extern "C" void __fastcall MarineTask_SetSecondBounds(BYTE* self, void*, float a, float b);
ASM_VAR(int, g_doneCount725d28, $L_725d28)
typedef BYTE* DoneList[500];
ASM_VAR(DoneList, g_done725558, $L_725558)

// [FUN_60c2b0] the grid's frame: transitions, refinement, finished transitions freed, the points' bounds, the triangles drawn
extern "C" int __fastcall GridMesh_Frame(BYTE* s,void*)
{
    F(DWORD,s,48)=timeGetTime();
    if(F(int,s,28)) CallThis(FUN_60be40,s);
    if(F(int,s,24)) F(int,s,24)=GridMesh_Refine50(s,0);
    BYTE* out=F(BYTE*,s,4);
    CallVirt(out,12);
    if(BYTE* e=F(BYTE*,s,44)) {
        do {
            if(MarineTransition_Advance(e,0,F(DWORD,s,48))&&(unsigned)g_doneCount725d28<500)
                g_done725558[g_doneCount725d28++]=e;
            e=F(BYTE*,e,0);
        } while(e!=F(BYTE*,s,44));
        while(g_doneCount725d28) {
            e=g_done725558[--g_doneCount725d28];
            if(!e) break;
            BYTE* head=F(BYTE*,s,44);
            if(F(BYTE*,head,0)==e&&F(BYTE*,head,4)==e) {
                if(head==e) F(BYTE*,s,44)=0;
                else {
                    F(BYTE*,head,0)=head;
                    head=F(BYTE*,s,44);
                    F(BYTE*,head,4)=F(BYTE*,head,0);
                }
            } else {
                if(head==e) F(BYTE*,s,44)=F(BYTE*,e,0);
                F(BYTE*,F(BYTE*,e,0),4)=F(BYTE*,e,4);
                F(BYTE*,F(BYTE*,e,4),0)=F(BYTE*,e,0);
            }
            MarineTask_ClearRegistration(e,0);
            operator delete(e);
        }
    }
    CallVirt(out,16,F(int,s,16));
    F(int,out,96)=F(WORD,s,40);
    if(F(int,out,40116)) {
        F(int,out,40116)=0;
        for(unsigned i=0;i<F(WORD,s,40);i++) {
            BYTE* rec=F(BYTE*,s,12)+i*16;
            double x=(double)(__int64)F(DWORD,rec,8)-F(float,out,40184);
            float a=(float)(x*0.125f);
            double y=(double)(__int64)F(DWORD,rec,12)-F(float,out,40188);
            float yf=(float)y;
            float b=(float)(y*0.125f);
            double c=x*0.3333333432674408f;
            float cf=(float)c;
            float d=(float)((double)yf*0.3333333432674408f);
            BYTE* pt=F(BYTE*,s,16)+i*40;
            F(float,pt,24)=a;
            F(float,pt,28)=b;
            F(float,pt,32)=(float)c;
            F(float,pt,36)=d;
            if(BYTE* t=F(BYTE*,F(BYTE*,s,12)+i*16,4)) {
                MarineTask_SetFirstBounds(t,0,a,b);
                MarineTask_SetSecondBounds(F(BYTE*,F(BYTE*,s,12)+i*16,4),0,cf,d);
            }
        }
    }
    BYTE** lists=F(BYTE**,s,36);
    BYTE* n=lists[0]?lists[0]:lists[1];
    while(n) {
        CallVirt(out,20,(int)*F(WORD*,n,16),(int)*F(WORD*,n,20),(int)*F(WORD*,n,24),n);
        lists=F(BYTE**,s,36);
        if(lists[0]&&n+4==F(BYTE*,lists[0],8))
            n=lists[1];
        else {
            if(lists[1]&&n+4==F(BYTE*,lists[1],8)) break;
            n=F(BYTE*,n,4);
            if(!n) break;
            n-=4;
        }
    }
    return 1;
}

extern "C" void FUN_6072b0(); KING_ALIAS("_FUN_6072b0", "_Grid_DeepestLevel")    // the deepest level allowed
extern "C" DWORD __fastcall MarineQueue_SetDeferredPriority(BYTE* self,void*,BYTE* node,DWORD priority);

// [FUN_60c6b0] re-prioritises the grid's nodes for up to a second, then refines while the output wants more points
extern "C" void __fastcall GridMesh_Reprioritise(BYTE* s,void*)
{
    BYTE** lists=F(BYTE**,s,36);
    F(BYTE*,s,52)=lists[0]?lists[0]:lists[1];
    DWORD t0=timeGetTime();
    do {
        BYTE* rec=F(BYTE*,s,52)+12;
        BYTE* cam=F(BYTE*,s,4);
        float prio=0.0f;
        if(F(unsigned,rec,24)<CallThis<unsigned>(FUN_6072b0,cam)) {
            float px=F(float,rec,32), py=F(float,rec,36), pz=F(float,rec,40);
            float c4=F(float,cam,4), c8=F(float,cam,8);
            double dy=(double)py-c8;
            double dx=(double)px-c4;
            double dd=x87_sqrt(dy*dy+dx*dx)-pz;
            float d=(float)dd;
            if(!(dd>0.0f)) prio=1000.0f;
            else if(d>1500.0f) prio=0.0f;
            else if(!(d>=50.0f)) prio=1000.0f;
            else {
                double ex=(double)px-c4, ey=(double)py-c8, mz=-(double)pz;
                if(!(ey*F(float,cam,60)-ex*F(float,cam,64)>=mz)) prio=0.0f;
                else if(!(ey*F(float,cam,68)-ex*F(float,cam,72)>=mz)) prio=0.0f;
                else if(F(unsigned,rec,24)<8) prio=(float)(10.0f/((double)d+1.0f));
                else if(!(d>=500.0f)) prio=(float)((double)F(float,rec,20)/d);
                else if(!Point_InWedge((float*)(rec+32),0,c4,c8,F(float,cam,76),F(float,cam,80),F(float,cam,84),F(float,cam,88)))
                    prio=(float)((double)F(float,rec,20)*0.5f/d);
                else prio=(float)((double)F(float,rec,20)/d);
            }
        }
        lists=F(BYTE**,s,36);
        BYTE* cur=F(BYTE*,s,52);
        BYTE* next;
        if(lists[0]&&(cur?cur+4:0)==F(BYTE*,lists[0],8))
            next=lists[1];
        else if(lists[1]&&(cur?cur+4:0)==F(BYTE*,lists[1],8))
            next=0;
        else {
            next=F(BYTE*,cur,4);
            if(next) next-=4;
        }
        MarineQueue_SetDeferredPriority((BYTE*)lists,0,cur,*(DWORD*)&prio);
        F(BYTE*,s,52)=next;
    } while(timeGetTime()-t0<1000&&F(BYTE*,s,52));
    while((unsigned)F(WORD,s,42)<CallVirt<unsigned>(F(BYTE*,s,4),4))
        CallC(FUN_607410,F(DWORD,MarineQueue_TopDeferredNode(F(BYTE*,s,36),0),40),s);
}

extern "C" void* __cdecl malloc(size_t n);
extern "C" void __fastcall GridTask_Free(BYTE* self, void*);
ASM_VAR(TopologyLabel,g_grdFile,$L_685a9c)

// [FUN_60c520] allocates n 16-byte and 40-byte records and m pointers; on failure frees them and reports
extern "C" int __fastcall GridTask_Alloc(BYTE* self,void*,int n,int m)
{
    F(void*,self,12)=malloc(n*16);
    F(void*,self,16)=malloc(n*40+1);
    F(void*,self,8)=malloc(m*4);
    F(int,self,60)=m;
    if(F(void*,self,12)&&F(void*,self,8)) return 1;
    GridTask_Free(self,0);
    CallC(FUN_60aad0,2289444628u,g_topError,g_grdFile,240,g_topDate);
    return 0;
}

extern "C" int __cdecl atexit(void(__cdecl* fn)(void));
extern "C" void __cdecl Grid_Reset725d28();
// [$L_60c2a0] nothing to do at exit
extern "C" void __cdecl Grid725d28_Destroy(void) {}
// [$L_60c290] registers it at exit
extern "C" int __cdecl Grid725d28_AtExit(void) { return atexit(Grid725d28_Destroy); }
// [$L_60c270] the static initializer of 725d28
extern "C" int __cdecl Grid725d28_Init(void)
{
    Grid_Reset725d28();
    return Grid725d28_AtExit();
}
