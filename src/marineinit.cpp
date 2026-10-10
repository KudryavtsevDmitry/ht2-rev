// Marine subsystem startup coefficients, vectors and default work limits.
#include "king.h"
#include "x87.h"
struct MarineTriple { float v[3]; };
struct MarineMatrix { DWORD v[16]; };
struct MarineConfig { DWORD v[15]; };
ASM_VAR(float, g_mi_652f50, $L_652f50)
ASM_VAR(float, g_mi_652f54, $L_652f54)
ASM_VAR(float, g_mi_64c250, $L_64c250)
ASM_VAR(float, g_mi_652f70, $L_652f70)
ASM_VAR(float, g_mi_652f78, $L_652f78)
ASM_VAR(float, g_mi_652f74, $L_652f74)
ASM_VAR(float, g_mi_652f7c, $L_652f7c)
ASM_VAR(float, g_mi_652f58, $L_652f58)
ASM_VAR(float, g_mi_652ff4, $L_652ff4)
ASM_VAR(float, g_mi_653094, $L_653094)
ASM_VAR(float, g_mi_64d4e8, $L_64d4e8)
ASM_VAR(float, g_mi_64c258, $L_64c258)
ASM_VAR(float, g_mi_6530a0, $L_6530a0)
ASM_VAR(float, g_mi_6530a4, $L_6530a4)
ASM_VAR(float, g_mi_64c334, $L_64c334)
ASM_VAR(float, g_mi_725d58, $L_725d58)
ASM_VAR(float, g_mi_725db4, $L_725db4)
ASM_VAR(float, g_mi_725d6c, $L_725d6c)
ASM_VAR(float, g_mi_725dc4, $L_725dc4)
ASM_VAR(float, g_mi_725db0, $L_725db0)
ASM_VAR(float, g_mi_725e14, $L_725e14)
ASM_VAR(float, g_mi_725e64, $L_725e64)
ASM_VAR(float, g_mi_725e6c, $L_725e6c)
ASM_VAR(MarineTriple, g_mi_725dd8, $L_725dd8)
ASM_VAR(MarineTriple, g_mi_725de8, $L_725de8)
ASM_VAR(MarineTriple, g_mi_725df8, $L_725df8)
ASM_VAR(MarineTriple, g_mi_725dc8, $L_725dc8)
ASM_VAR(MarineTriple, g_mi_725d60, $L_725d60)
ASM_VAR(MarineTriple, g_mi_725db8, $L_725db8)
ASM_VAR(MarineTriple, g_mi_725e18, $L_725e18)
ASM_VAR(MarineMatrix, g_mi_725d70, $L_725d70)
ASM_VAR(MarineConfig, g_mi_725e28, $L_725e28)
extern "C" DWORD* __cdecl MarineInit_Identity(DWORD* matrix)
{
    for(unsigned i=0;i<16;++i) matrix[i]=(i%5==0)?0x3f800000u:0;
    return matrix;
}
extern "C" void __cdecl MarineInit_DoubleMeshScale()
{
    g_mi_725d58 = (float)((double)g_mi_652f50 + g_mi_652f50);
}
extern "C" void __cdecl MarineInit_DoubleSecondaryScale()
{
    g_mi_725db4 = (float)((double)g_mi_652f54 + g_mi_652f54);
}
extern "C" void __cdecl MarineInit_BasisX()
{
    g_mi_725dd8.v[0]=1.0f; g_mi_725dd8.v[1]=g_mi_725dd8.v[2]=0.0f;
}
extern "C" void __cdecl MarineInit_BasisY()
{
    g_mi_725de8.v[1]=1.0f; g_mi_725de8.v[0]=g_mi_725de8.v[2]=0.0f;
}
extern "C" void __cdecl MarineInit_BasisZ()
{
    g_mi_725df8.v[2]=1.0f; g_mi_725df8.v[0]=g_mi_725df8.v[1]=0.0f;
}
extern "C" void __cdecl MarineInit_DefaultMatrix()
{
    MarineInit_Identity(g_mi_725d70.v);
}
extern "C" void __cdecl MarineInit_Complement()
{
    g_mi_725d6c=(float)((double)g_mi_64c250-g_mi_652f70);
}
extern "C" void __cdecl MarineInit_Interval()
{
    g_mi_725dc4=(float)((double)g_mi_652f78-g_mi_652f74);
}
extern "C" void __cdecl MarineInit_DefaultNormal()
{
    g_mi_725dc8.v[2]=1.0f; g_mi_725dc8.v[0]=g_mi_725dc8.v[1]=0.0f;
}
extern "C" void __cdecl MarineInit_DefaultAxis()
{
    g_mi_725d60.v[2]=1.0f; g_mi_725d60.v[0]=g_mi_725d60.v[1]=0.0f;
}
extern "C" void __cdecl MarineInit_DefaultOffset()
{
    g_mi_725db8.v[0]=g_mi_725db8.v[2]=0.0f; g_mi_725db8.v[1]=8.0f;
}
extern "C" void __cdecl MarineInit_SquaredRatio()
{
    g_mi_725db0=(float)(((double)g_mi_652f7c*g_mi_652f7c)/(((double)g_mi_725db4*g_mi_652f58)*g_mi_652ff4));
}
extern "C" void __cdecl MarineInit_AdjustedThreshold()
{
    g_mi_725e14=(float)(((double)g_mi_653094-g_mi_64d4e8)-g_mi_64c258);
}
extern "C" void __cdecl MarineInit_ZeroVector()
{
    g_mi_725e18.v[0]=g_mi_725e18.v[1]=g_mi_725e18.v[2]=0.0f;
}
extern "C" void __cdecl MarineInit_DoubleAngleScale()
{
    g_mi_725e64=(float)((double)g_mi_6530a0+g_mi_6530a0);
}
extern "C" void __cdecl MarineInit_DoubleFinalScale()
{
    g_mi_725e6c=(float)((double)g_mi_6530a4+g_mi_6530a4);
}
extern "C" DWORD* __cdecl MarineInit_DefaultLimits()
{
    DWORD* config=g_mi_725e28.v;
    config[0]=10; config[1]=500; config[2]=1; config[3]=8;
    config[4]=50; config[5]=25; config[6]=50; config[7]=120;
    config[8]=10; config[9]=11; config[10]=11; config[11]=1500; config[12]=1500;
    double angle=(double)g_mi_6530a0*g_mi_64c334;
    *(float*)(config+13)=(float)angle;
    *(float*)(config+14)=(float)x87_cos(angle);
    return config;
}
ASM_VAR(float, g_mi_652f44, $L_652f44)
ASM_VAR(float, g_mi_725d48, $L_725d48)
ASM_VAR(float, g_mi_6533f4, $L_6533f4)
ASM_VAR(float, g_mi_72c150, $L_72c150)
extern "C" void __cdecl MarineInit_DoubleTreeScale()
{
    g_mi_725d48=(float)((double)g_mi_652f44+g_mi_652f44);
}
extern "C" void __cdecl MarineInit_DoubleTerrainScale()
{
    g_mi_72c150=(float)((double)g_mi_6533f4+g_mi_6533f4);
}
ASM_VAR(float, g_mi_6531d4, $L_6531d4)
ASM_VAR(float, g_mi_725fc0, $L_725fc0)
extern "C" void __cdecl MarineInit_DoubleTransitionScale()
{
    g_mi_725fc0=(float)((double)g_mi_6531d4+g_mi_6531d4);
}
