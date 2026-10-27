// Marine refinement weights, reflected texture coordinates and ripple height.
#include "king.h"
#include "x87.h"
#include "vecmath.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
KING_VAR(float, g_mm_zero, 64c264)
KING_VAR(float, g_mm_one, 64c250)
KING_VAR(float, g_mm_third, 64c904)
KING_VAR(float, g_mm_threshold, 725e60)
KING_VAR(double, g_mm_unit, 64b670)
KING_VAR(float, g_mm_backWeight, 64c948)
KING_VAR(float, g_mm_normalZ, 64c2f8)
KING_VAR(double, g_mm_epsilon, 64b748)
KING_VAR(float, g_mm_smallZ, 6503e0)
KING_VAR(float, g_mm_negativeOne, 64c938)
KING_VAR(float, g_mm_uvScale, 65311c)
KING_VAR(float, g_mm_half, 64c258)
KING_VAR(float, g_mm_rippleTimeA, 65312c)
KING_VAR(float, g_mm_rippleSpaceA, 653128)
KING_VAR(float, g_mm_rippleTimeB, 653124)
KING_VAR(float, g_mm_rippleSpaceB, 653120)
KING_VAR(float, g_mm_rippleScale, 64c748)
KING_VAR(float, g_mm_radius, 6531d8)
KING_VAR(float, g_mm_diameter, 725fc8)
KING_VAR(float, g_mm_randomRadius, 653220)
KING_VAR(float, g_mm_randomDiameter, 7287f8)
extern "C" double __cdecl MarineAngle_Sine(float);
extern "C" double __cdecl MarineAngle_Cosine(float);
extern "C" double __fastcall MarineGrid_MapY(BYTE*,void*,const DWORD*);
extern "C" Vec3* __cdecl Vec3_Diff(Vec3*,const Vec3*,const Vec3*);
extern "C" Vec3* __cdecl Vec3_DivBy(Vec3*,const Vec3*,float);
extern "C" double __cdecl Vec3_LengthSq(const Vec3*);
static double MarineMath_Unsigned(DWORD x) { volatile __int64 value=x;return (double)value; }
static double MarineMath_X(BYTE* self,const DWORD* p) {return MarineMath_Unsigned(p[0])*F(float,self,192)+F(float,self,176);}
static double MarineMath_Y(BYTE* self,const DWORD* p) {return MarineMath_Unsigned(p[1])*F(float,self,196)+F(float,self,188);}
extern "C" double __fastcall MarineGrid_TriangleWeight(BYTE* self,void*,const DWORD* a,const DWORD* b,const DWORD* c,BYTE* triangle)
{
    DWORD depth=F(DWORD,triangle,24);
    if(depth>=CallVirt<DWORD>(self,16)) return g_mm_zero;
    Vec3 camera=*(Vec3*)(self+4);
    float centerX=(float)(((MarineMath_X(self,c)+MarineMath_X(self,a))+MarineMath_X(self,b))*g_mm_third);
    float centerY=(float)(((MarineMath_Y(self,a)+MarineMath_Y(self,b))+MarineMath_Y(self,c))*g_mm_third);
    double nx=F(float,self,16),ny=F(float,self,20),nz=F(float,self,24);
    double length=x87_sqrt((nx*nx+ny*ny)+nz*nz);
    Vec3 normal={(float)(nx/length),(float)(ny/length),(float)(nz/length)};
    double dx=(double)centerX-camera.x;
    float dy=(float)((double)centerY-camera.y),dz=(float)(-(double)camera.z);
    float distance=(float)x87_sqrt(((double)dz*dz+(double)dy*dy)+dx*dx);
    double dot=(((double)dz/distance*normal.z+(double)dy/distance*normal.y)+dx/distance*normal.x);
    if(g_mm_threshold>=dot) return g_mm_zero;
    float roundedX=(float)dx;
    return x87_inv_dist(roundedX,dy,g_mm_unit);
}
static double MarineMath_SurfaceWeight(BYTE* self,const DWORD* a,const DWORD* b,const DWORD* c,bool second)
{
    Vec3 camera=*(Vec3*)(self+4);
    Vec3 center;
    center.x=second?
        (float)(((MarineMath_X(self,c)+MarineMath_X(self,a))+MarineMath_X(self,b))*g_mm_third):
        (float)(((MarineMath_X(self,a)+MarineMath_X(self,b))+MarineMath_X(self,c))*g_mm_third);
    double y;
    if(second) {
        float first=(float)MarineGrid_MapY(self,0,a);
        float sum=(float)(MarineGrid_MapY(self,0,b)+first);
        y=(MarineGrid_MapY(self,0,c)+sum)*g_mm_third;
    } else y=((MarineMath_Y(self,a)+MarineMath_Y(self,b))+MarineMath_Y(self,c))*g_mm_third;
    center.y=(float)y;center.z=0;
    double dx=(double)center.x-camera.x,dy=y-camera.y;
    float distance=(float)x87_sqrt(dx*dx+dy*dy);
    Vec3 direction,delta,relative;
    float length=(float)x87_sqrt(Vec3_LengthSq((Vec3*)(self+16)));
    Vec3_DivBy(&direction,(Vec3*)(self+16),length);
    Vec3_Diff(&delta,&center,&camera);
    length=(float)x87_sqrt(Vec3_LengthSq(&delta));
    Vec3_DivBy(&relative,&delta,length);
    double dot=Vec3_Dot(&relative,&direction);
    double numerator=!(g_mm_threshold>=dot)?g_mm_one:g_mm_backWeight;
    double result=numerator/((double)distance+g_mm_one);
    return second?result:(float)result;
}
extern "C" double __fastcall MarineGrid_SurfaceWeight(BYTE* self,void*,const DWORD* a,const DWORD* b,const DWORD* c,DWORD,const DWORD* d,const DWORD* e,const DWORD* f,DWORD hasSecond)
{
    float first=(float)MarineMath_SurfaceWeight(self,a,b,c,false);
    double second=hasSecond?MarineMath_SurfaceWeight(self,d,e,f,true):0.0f;
    return first>second?first:second;
}
extern "C" void __fastcall MarineGrid_ReflectionUV(BYTE* self,void*,float x,float y,float z,const Vec3* surface,float* u,float* v)
{
    Vec3 input;
    volatile double sx=surface->x,sy=surface->y;
    input.x=(float)sx;input.y=(float)sy;input.z=(float)((double)surface->z*g_mm_normalZ);
    Vec3 normal,direction,difference;
    Vec3_Normalized(&normal,&input);
    difference.x=(float)((double)F(float,self,4)-x);
    difference.y=(float)((double)F(float,self,8)-y);
    difference.z=(float)((double)F(float,self,12)-z);
    Vec3_Normalized(&direction,&difference);
    double twiceX=(double)normal.x+normal.x;
    float twiceY=(float)((double)normal.y+normal.y),twiceZ=(float)((double)normal.z+normal.z);
    float dot=(float)(((double)normal.y*direction.y+(double)normal.z*direction.z)+(double)normal.x*direction.x);
    float rx=(float)(twiceX*dot-direction.x);
    float ry=(float)((double)(float)((double)twiceY*dot)-direction.y);
    float rz=(float)((double)(float)((double)twiceZ*dot)-direction.z);
    double denominator;
    if(!(x87_fabs(rz)>=g_mm_epsilon)) denominator=g_mm_smallZ;
    else if(rz>g_mm_one) denominator=g_mm_one;
    else if(!(rz>=g_mm_negativeOne)) denominator=g_mm_negativeOne;
    else denominator=rz;
    if(!(rx>=g_mm_negativeOne)) rx=-1.0f;
    else if(rx>g_mm_one) rx=1.0f;
    denominator=x87_fabs(denominator);
    *u=(float)((double)rx*g_mm_uvScale/denominator+g_mm_half);
    *v=(float)((double)ry*g_mm_uvScale/denominator+g_mm_half);
}
extern "C" double __fastcall MarineGrid_RippleHeight(BYTE* self,void*,const float* point)
{
    bool extra=!(F(float,self,72)>=g_mm_one);
    double x=point[0],y=point[1];
    float rotatedX=(float)(x*F(float,self,84)+y*F(float,self,80));
    double rotatedY=y*F(float,self,84)-x*F(float,self,80);
    float phase=(float)(MarineMath_Unsigned(F(DWORD,self,40))/F(float,self,68)+rotatedY/F(float,self,60));
    float sine=(float)MarineAngle_Sine(phase);
    phase=(float)(MarineMath_Unsigned(F(DWORD,self,40))/F(float,self,64)+(double)rotatedX/F(float,self,56));
    double base=MarineAngle_Cosine(phase)*sine*F(float,self,72);
    if(!extra) return base;
    float roundedBase=(float)base;
    phase=(float)(MarineMath_Unsigned(F(DWORD,self,40))*g_mm_rippleTimeA+(double)point[0]*g_mm_rippleSpaceA);
    float first=(float)MarineAngle_Sine(phase);
    phase=(float)(MarineMath_Unsigned(F(DWORD,self,40))*g_mm_rippleTimeB+(double)point[0]*g_mm_rippleSpaceB);
    return MarineAngle_Sine(phase)*first*g_mm_rippleScale+roundedBase;
}
extern "C" void __fastcall MarineMaterial_UpdateFrame(BYTE* self,void*,DWORD now)
{
    DWORD count=F(DWORD,self,0);
    if(!count) {F(DWORD,self,12)=0;return;}
    volatile __int64 elapsed=(DWORD)(now-F(DWORD,self,4));
    DWORD frame=(DWORD)(__int64)((double)elapsed/(int)F(DWORD,self,8));
    F(DWORD,self,12)=frame%count;
}
extern "C" void __fastcall MarineGrid_SetTime(BYTE* self,void*,DWORD now)
{
    F(DWORD,self,40)=now;
    BYTE* material=F(BYTE*,self,200);
    if(material) MarineMaterial_UpdateFrame(material,0,now);
}
extern "C" void __cdecl MarineGrid_InitDiameter() {F(float,&g_mm_diameter,0)=(float)((double)g_mm_radius+g_mm_radius);}
extern "C" void __cdecl MarineGrid_InitRandomDiameter() {F(float,&g_mm_randomDiameter,0)=(float)((double)g_mm_randomRadius+g_mm_randomRadius);}
