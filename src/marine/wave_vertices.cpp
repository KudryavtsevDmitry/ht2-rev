// Wave-surface vertex records, foam emission and indexed triangle output.
#include "king.h"
#include "x87.h"
#include "vecmath.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
KING_BSS(Vec3, g_mv_normal, 0x725e78)
KING_BSS(Vec3, g_mv_flatNormal, 0x725ee0)
ASM_VAR(float, g_mv_pending, $L_6530c0)
ASM_VAR(float, g_mv_textureWidth, $L_6530b8)
ASM_VAR(float, g_mv_textureHeight, $L_6530bc)
ASM_VAR(float, g_mv_foamHeight, $L_6530c4)
ASM_VAR(float, g_mv_normalZ, $L_64c2f8)
ASM_VAR(float, g_mv_one, $L_64c250)
ASM_VAR(float, g_mv_negativeOne, $L_64c938)
ASM_VAR(double, g_mv_epsilon, $L_64b748)
ASM_VAR(float, g_mv_smallZ, $L_6503e0)
ASM_VAR(float, g_mv_uvScale, $L_65311c)
ASM_VAR(float, g_mv_half, $L_64c258)
ASM_VAR(float, g_mv_foamOffset2, $L_64c40c)
ASM_VAR(float, g_mv_foamOffset3, $L_64d2f8)
ASM_VAR(float, g_mv_foamOffset4, $L_64c408)
ASM_VAR(float, g_mv_foamOffset5, $L_64c320)
ASM_VAR(float, g_mv_foamOffset6, $L_64c2fc)
ASM_VAR(float, g_mv_foamOffset7, $L_64d350)
ASM_VAR(float, g_mv_foamOffset8, $L_64c490)
extern "C" double __cdecl MarineAngle_Sine(float);
extern "C" double __cdecl MarineAngle_Cosine(float);
extern "C" Vec3* __cdecl Vec3_DivBy(Vec3*,const Vec3*,float);
extern "C" double __cdecl Vec3_LengthSq(const Vec3*);
extern "C" double __fastcall MarineGrid_MapX(BYTE*,void*,const DWORD*);
extern "C" double __fastcall MarineGrid_MapY(BYTE*,void*,const DWORD*);
extern "C" void __fastcall MarineGrid_Unmap(BYTE*,void*,const DWORD*,float*,float*);
extern "C" void __fastcall MarineGrid_ReflectionUV(BYTE*,void*,float,float,float,const Vec3*,float*,float*);
extern "C" BYTE* __fastcall MarineGrid_WriteVertex(BYTE*,void*,const DWORD*,const DWORD*,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
extern "C" DWORD __fastcall MarineParticlePool_CanAdd(BYTE*,void*);
extern "C" DWORD __fastcall MarineParticlePool_Add(BYTE*,void*,const DWORD*,const DWORD*);
static double MarineVertex_Unsigned(DWORD value) {volatile __int64 x=value;return (double)x;}
static DWORD MarineVertex_Bits(float value) {return F(DWORD,&value,0);}
static void MarineVertex_Reflection(const Vec3& position,float* u,float* v,BYTE* self)
{
    Vec3 input=g_mv_normal;
    input.z=(float)((double)input.z*g_mv_normalZ);
    Vec3 normal,camera=*(Vec3*)(self+4),difference,direction,twice,scaled,reflection;
    Vec3_Normalized(&normal,&input);
    Vec3_Sub(&difference,&camera,&position);
    Vec3_Normalized(&direction,&difference);
    float dot=(float)Vec3_Dot(&normal,&direction);
    Vec3_Scaled(&twice,&normal,2.0f);
    Vec3_Scaled(&scaled,&twice,dot);
    Vec3_Sub(&reflection,&scaled,&direction);
    double denominator;
    if(!(x87_fabs(reflection.z)>=g_mv_epsilon)) denominator=g_mv_smallZ;
    else if(reflection.z>g_mv_one) denominator=g_mv_one;
    else if(!(reflection.z>=g_mv_negativeOne)) denominator=g_mv_negativeOne;
    else denominator=reflection.z;
    if(!(reflection.x>=g_mv_negativeOne)) reflection.x=-1.0f;
    else if(reflection.x>g_mv_one) reflection.x=1.0f;
    denominator=x87_fabs(denominator);
    *u=(float)((double)reflection.x*g_mv_uvScale/denominator+g_mv_half);
    *v=(float)((double)reflection.y*g_mv_uvScale/denominator+g_mv_half);
}
extern "C" void __fastcall MarineGrid_UpdateVertex(BYTE* self,void*,const DWORD* point,DWORD id,DWORD)
{
    float x=(float)MarineGrid_MapX(self,0,point);
    double unroundedY=MarineGrid_MapY(self,0,point);
    float y=(float)unroundedY;
    float rotatedX=(float)(unroundedY*F(float,self,80)+(double)x*F(float,self,84));
    double rotatedY=(double)y*F(float,self,84)-(double)x*F(float,self,80);
    float phase=(float)(MarineVertex_Unsigned(F(DWORD,self,40))/F(float,self,68)+rotatedY/F(float,self,60));
    float sine=(float)MarineAngle_Sine(phase);
    phase=(float)(MarineVertex_Unsigned(F(DWORD,self,40))/F(float,self,64)+(double)rotatedX/F(float,self,56));
    float height=(float)(MarineAngle_Cosine(phase)*sine);
    rotatedX=(float)((double)y*F(float,self,80)+(double)x*F(float,self,84));
    float roundedY=(float)((double)y*F(float,self,84)-(double)x*F(float,self,80));
    phase=(float)(MarineVertex_Unsigned(F(DWORD,self,40))/F(float,self,68)+(double)roundedY/F(float,self,60));
    sine=(float)MarineAngle_Sine(phase);
    phase=(float)(MarineVertex_Unsigned(F(DWORD,self,40))/F(float,self,64)+(double)rotatedX/F(float,self,56));
    float derivative=(float)(MarineAngle_Sine(phase)*sine*F(float,self,72));
    phase=(float)(MarineVertex_Unsigned(F(DWORD,self,40))/F(float,self,68)+(double)roundedY/F(float,self,60));
    float cosine=(float)MarineAngle_Cosine(phase);
    phase=(float)(MarineVertex_Unsigned(F(DWORD,self,40))/F(float,self,64)+(double)rotatedX/F(float,self,56));
    double other=MarineAngle_Cosine(phase)*cosine*F(float,self,72);
    Vec3 rawNormal;
    rawNormal.x=(float)(-(other*F(float,self,80)+(double)derivative*F(float,self,84)));
    rawNormal.y=(float)(-((double)derivative*F(float,self,80)-other*F(float,self,84)));
    volatile double nz=F(float,self,88);rawNormal.z=(float)nz;
    float length=(float)x87_sqrt(Vec3_LengthSq(&rawNormal));
    Vec3 normal;Vec3_DivBy(&normal,&rawNormal,length);
    F(DWORD,&g_mv_normal,0)=F(DWORD,&normal,0);
    F(DWORD,&g_mv_normal,4)=F(DWORD,&normal,4);
    F(DWORD,&g_mv_normal,8)=F(DWORD,&normal,8);
    float gridU=(float)((MarineVertex_Unsigned(point[0])-F(float,self,152))/F(float,self,100));
    float gridV=(float)((MarineVertex_Unsigned(point[1])-F(float,self,156))/F(float,self,104));
    float textureU=(float)((double)gridU/g_mv_textureWidth),textureV=(float)((double)gridV/g_mv_textureHeight);
    Vec3 position={x,y,(float)((double)height*F(float,self,72))};
    float reflectionU,reflectionV;MarineVertex_Reflection(position,&reflectionU,&reflectionV,self);
    if(height>g_mv_foamHeight&&MarineParticlePool_CanAdd(F(BYTE*,self,112),0)) {
        Vec3 velocity;
        velocity.x=(float)((double)F(float,self,92)*F(float,self,84)+(double)F(float,self,96)*F(float,self,80));
        velocity.y=(float)((double)F(float,self,96)*F(float,self,84)-(double)F(float,self,92)*F(float,self,80));velocity.z=0;
        Vec3 positions[17];positions[0].x=x;positions[0].y=y;positions[0].z=0;
        float offsets[]={g_mv_one,g_mv_foamOffset2,g_mv_foamOffset3,g_mv_foamOffset4,g_mv_foamOffset5,g_mv_foamOffset6,g_mv_foamOffset7,g_mv_foamOffset8};
        for(unsigned i=0;i<8;++i) {
            // Offset order is part of the original emission sequence.
            bool subtractFirst=i==0||i==4;
            positions[1+i*2].x=subtractFirst?(float)((double)x-offsets[i]):(float)((double)x+offsets[i]);
            positions[2+i*2].x=subtractFirst?(float)((double)x+offsets[i]):(float)((double)x-offsets[i]);
            positions[1+i*2].y=positions[2+i*2].y=y;positions[1+i*2].z=positions[2+i*2].z=0;
        }
        for(unsigned i=0;i<17;++i) MarineParticlePool_Add(F(BYTE*,self,112),0,(DWORD*)&positions[i],(DWORD*)&velocity);
    }
    position.z=(float)((double)height*F(float,self,72));
    DWORD record[12];
    MarineGrid_WriteVertex((BYTE*)record,0,(DWORD*)&position,(DWORD*)&g_mv_normal,MarineVertex_Bits(gridU),MarineVertex_Bits(gridV),MarineVertex_Bits(textureU),MarineVertex_Bits(textureV),MarineVertex_Bits(reflectionU),MarineVertex_Bits(reflectionV));
    BYTE* destination=F(BYTE*,self,124)+(id&65535u)*48u;
    for(unsigned i=0;i<12;++i) F(volatile DWORD,destination,i*4)=record[i];
}
static bool MarineVertex_IsPending(BYTE* record)
{
    double value=F(float,record,0);
    return !(value<g_mv_pending)&&!(value>g_mv_pending);
}
extern "C" DWORD __fastcall MarineGrid_EmitTriangle(BYTE* self,void*,DWORD a,DWORD b,DWORD c,BYTE* triangle)
{
    DWORD depth=F(DWORD,triangle,36),ids[]={a,b,c};
    for(unsigned i=0;i<3;++i) {
        DWORD offset=(ids[i]&65535u)*48u;
        BYTE* entry=F(BYTE*,self,124)+offset;
        if(!MarineVertex_IsPending(entry)) continue;
        DWORD point[2]={(DWORD)(__int64)(double)F(float,entry,4),(DWORD)(__int64)(double)F(float,entry,8)};
        if(depth>7) {MarineGrid_UpdateVertex(self,0,point,ids[i],depth);continue;}
        float x=(float)MarineGrid_MapX(self,0,point),y=(float)MarineGrid_MapY(self,0,point);
        float gridU,gridV;MarineGrid_Unmap(self,0,point,&gridU,&gridV);
        float textureU=(float)((double)gridU/g_mv_textureWidth),textureV=(float)((double)gridV/g_mv_textureHeight),reflectionU,reflectionV;
        MarineGrid_ReflectionUV(self,0,x,y,0,&g_mv_flatNormal,&reflectionU,&reflectionV);
        Vec3 position={(float)MarineGrid_MapX(self,0,point),(float)MarineGrid_MapY(self,0,point),0};
        DWORD record[12];
        MarineGrid_WriteVertex((BYTE*)record,0,(DWORD*)&position,(DWORD*)&g_mv_flatNormal,MarineVertex_Bits(gridU),MarineVertex_Bits(gridV),MarineVertex_Bits(textureU),MarineVertex_Bits(textureV),MarineVertex_Bits(reflectionU),MarineVertex_Bits(reflectionV));
        BYTE* destination=F(BYTE*,self,124)+offset;
        for(unsigned j=0;j<12;++j) F(volatile DWORD,destination,j*4)=record[j];
    }
    unsigned countOffset=depth>7?136:128,bufferOffset=depth>7?140:132;
    DWORD order[]={a,c,b};
    for(unsigned i=0;i<3;++i) {
        DWORD count=F(volatile DWORD,self,countOffset);
        BYTE* buffer=F(BYTE*,self,bufferOffset);
        F(volatile WORD,buffer,(count+i)*2)=(WORD)order[i];
    }
    DWORD count=F(DWORD,self,countOffset)+3u;
    F(DWORD,self,countOffset)=count;
    return count;
}
