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

// Mesh motion and particle startup geometry and coefficients.
ASM_VAR(float, g_mi_725ecc, $L_725ecc)
ASM_VAR(float, g_mi_6530a8, $L_6530a8)
ASM_VAR(DWORD, g_mi_725ef0, $L_725ef0)
ASM_VAR(DWORD, g_mi_725ef4, $L_725ef4)
ASM_VAR(DWORD, g_mi_725ef8, $L_725ef8)
ASM_VAR(DWORD, g_mi_725f00, $L_725f00)
ASM_VAR(DWORD, g_mi_725f04, $L_725f04)
ASM_VAR(DWORD, g_mi_725f08, $L_725f08)
ASM_VAR(DWORD, g_mi_725f10, $L_725f10)
ASM_VAR(DWORD, g_mi_725f14, $L_725f14)
ASM_VAR(DWORD, g_mi_725f18, $L_725f18)
ASM_VAR(MarineMatrix, g_mi_725e88, $L_725e88)
ASM_VAR(float, g_mi_725e84, $L_725e84)
ASM_VAR(float, g_mi_6530c4, $L_6530c4)
ASM_VAR(float, g_mi_725edc, $L_725edc)
ASM_VAR(float, g_mi_6530cc, $L_6530cc)
ASM_VAR(float, g_mi_6530c8, $L_6530c8)
ASM_VAR(DWORD, g_mi_725ee0, $L_725ee0)
ASM_VAR(DWORD, g_mi_725ee4, $L_725ee4)
ASM_VAR(DWORD, g_mi_725ee8, $L_725ee8)
ASM_VAR(DWORD, g_mi_725e78, $L_725e78)
ASM_VAR(DWORD, g_mi_725e7c, $L_725e7c)
ASM_VAR(DWORD, g_mi_725e80, $L_725e80)
ASM_VAR(DWORD, g_mi_725ed0, $L_725ed0)
ASM_VAR(DWORD, g_mi_725ed4, $L_725ed4)
ASM_VAR(DWORD, g_mi_725ed8, $L_725ed8)
ASM_VAR(float, g_mi_725ec8, $L_725ec8)
ASM_VAR(float, g_mi_6530d0, $L_6530d0)
ASM_VAR(float, g_mi_6530ac, $L_6530ac)
ASM_VAR(float, g_mi_725f78, $L_725f78)
ASM_VAR(float, g_mi_653140, $L_653140)
ASM_VAR(DWORD, g_mi_725f90, $L_725f90)
ASM_VAR(DWORD, g_mi_725f94, $L_725f94)
ASM_VAR(DWORD, g_mi_725f98, $L_725f98)
ASM_VAR(DWORD, g_mi_725fa0, $L_725fa0)
ASM_VAR(DWORD, g_mi_725fa4, $L_725fa4)
ASM_VAR(DWORD, g_mi_725fa8, $L_725fa8)
ASM_VAR(DWORD, g_mi_725fb0, $L_725fb0)
ASM_VAR(DWORD, g_mi_725fb4, $L_725fb4)
ASM_VAR(DWORD, g_mi_725fb8, $L_725fb8)
ASM_VAR(MarineMatrix, g_mi_725f38, $L_725f38)
ASM_VAR(float, g_mi_725f34, $L_725f34)
ASM_VAR(float, g_mi_653158, $L_653158)
ASM_VAR(float, g_mi_725f7c, $L_725f7c)
ASM_VAR(float, g_mi_653160, $L_653160)
ASM_VAR(float, g_mi_65315c, $L_65315c)
ASM_VAR(DWORD, g_mi_725f80, $L_725f80)
ASM_VAR(DWORD, g_mi_725f84, $L_725f84)
ASM_VAR(DWORD, g_mi_725f88, $L_725f88)
ASM_VAR(DWORD, g_mi_725f28, $L_725f28)
ASM_VAR(DWORD, g_mi_725f2c, $L_725f2c)
ASM_VAR(DWORD, g_mi_725f30, $L_725f30)
extern "C" void __cdecl MarineInit_DoubleMotionScale()
{
    g_mi_725ecc=(float)((double)g_mi_6530a8+g_mi_6530a8);
}
extern "C" void __cdecl MarineInit_MotionBasisX()
{
    g_mi_725ef0=1065353216u;
    g_mi_725ef4=0u;
    g_mi_725ef8=0u;
}
extern "C" void __cdecl MarineInit_MotionBasisY()
{
    g_mi_725f00=0u;
    g_mi_725f04=1065353216u;
    g_mi_725f08=0u;
}
extern "C" void __cdecl MarineInit_MotionBasisZ()
{
    g_mi_725f10=0u;
    g_mi_725f14=0u;
    g_mi_725f18=1065353216u;
}
extern "C" void __cdecl MarineInit_MotionMatrix()
{
    MarineInit_Identity(g_mi_725e88.v);
}
extern "C" void __cdecl MarineInit_MotionComplement()
{
    g_mi_725e84=(float)((double)g_mi_64c250-g_mi_6530c4);
}
extern "C" void __cdecl MarineInit_MotionInterval()
{
    g_mi_725edc=(float)((double)g_mi_6530cc-g_mi_6530c8);
}
extern "C" void __cdecl MarineInit_MotionNormal()
{
    g_mi_725ee0=0u;
    g_mi_725ee4=0u;
    g_mi_725ee8=1065353216u;
}
extern "C" void __cdecl MarineInit_MotionAxis()
{
    g_mi_725e78=0u;
    g_mi_725e7c=0u;
    g_mi_725e80=1065353216u;
}
extern "C" void __cdecl MarineInit_MotionOffset()
{
    g_mi_725ed0=0u;
    g_mi_725ed4=1090519040u;
    g_mi_725ed8=0u;
}
extern "C" void __cdecl MarineInit_MotionSquaredRatio()
{
    g_mi_725ec8=(float)(((double)g_mi_6530d0*g_mi_6530d0)/(((double)g_mi_725ecc*g_mi_6530ac)*g_mi_652ff4));
}
extern "C" void __cdecl MarineInit_DoubleParticleScale()
{
    g_mi_725f78=(float)((double)g_mi_653140+g_mi_653140);
}
extern "C" void __cdecl MarineInit_ParticleBasisX()
{
    g_mi_725f90=1065353216u;
    g_mi_725f94=0u;
    g_mi_725f98=0u;
}
extern "C" void __cdecl MarineInit_ParticleBasisY()
{
    g_mi_725fa0=0u;
    g_mi_725fa4=1065353216u;
    g_mi_725fa8=0u;
}
extern "C" void __cdecl MarineInit_ParticleBasisZ()
{
    g_mi_725fb0=0u;
    g_mi_725fb4=0u;
    g_mi_725fb8=1065353216u;
}
extern "C" void __cdecl MarineInit_ParticleMatrix()
{
    MarineInit_Identity(g_mi_725f38.v);
}
extern "C" void __cdecl MarineInit_ParticleComplement()
{
    g_mi_725f34=(float)((double)g_mi_64c250-g_mi_653158);
}
extern "C" void __cdecl MarineInit_ParticleInterval()
{
    g_mi_725f7c=(float)((double)g_mi_653160-g_mi_65315c);
}
extern "C" void __cdecl MarineInit_ParticleNormal()
{
    g_mi_725f80=0u;
    g_mi_725f84=0u;
    g_mi_725f88=1065353216u;
}
extern "C" void __cdecl MarineInit_ParticleAxis()
{
    g_mi_725f28=0u;
    g_mi_725f2c=0u;
    g_mi_725f30=1065353216u;
}
