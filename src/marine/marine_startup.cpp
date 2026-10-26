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
KING_BSS(float, g_mi_725d58, 0x725d58)
KING_BSS(float, g_mi_725db4, 0x725db4)
KING_BSS(float, g_mi_725d6c, 0x725d6c)
KING_BSS(float, g_mi_725dc4, 0x725dc4)
KING_BSS(float, g_mi_725db0, 0x725db0)
KING_BSS(float, g_mi_725e14, 0x725e14)
KING_BSS(float, g_mi_725e64, 0x725e64)
KING_BSS(float, g_mi_725e6c, 0x725e6c)
KING_BSS(MarineTriple, g_mi_725dd8, 0x725dd8)
KING_BSS(MarineTriple, g_mi_725de8, 0x725de8)
KING_BSS(MarineTriple, g_mi_725df8, 0x725df8)
KING_BSS(MarineTriple, g_mi_725dc8, 0x725dc8)
KING_BSS(MarineTriple, g_mi_725d60, 0x725d60)
KING_BSS(MarineTriple, g_mi_725db8, 0x725db8)
KING_BSS(MarineTriple, g_mi_725e18, 0x725e18)
KING_BSS(MarineMatrix, g_mi_725d70, 0x725d70)
KING_BSS(MarineConfig, g_mi_725e28, 0x725e28)
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
KING_BSS(float, g_mi_725d48, 0x725d48)
ASM_VAR(float, g_mi_6533f4, $L_6533f4)
KING_BSS(float, g_mi_72c150, 0x72c150)
extern "C" void __cdecl MarineInit_DoubleTreeScale()
{
    g_mi_725d48=(float)((double)g_mi_652f44+g_mi_652f44);
}
extern "C" void __cdecl MarineInit_DoubleTerrainScale()
{
    g_mi_72c150=(float)((double)g_mi_6533f4+g_mi_6533f4);
}
ASM_VAR(float, g_mi_6531d4, $L_6531d4)
KING_BSS(float, g_mi_725fc0, 0x725fc0)
extern "C" void __cdecl MarineInit_DoubleTransitionScale()
{
    g_mi_725fc0=(float)((double)g_mi_6531d4+g_mi_6531d4);
}

// Mesh motion and particle startup geometry and coefficients.
KING_BSS(float, g_mi_725ecc, 0x725ecc)
ASM_VAR(float, g_mi_6530a8, $L_6530a8)
KING_BSS(DWORD, g_mi_725ef0, 0x725ef0)
KING_BSS(DWORD, g_mi_725ef4, 0x725ef4)
KING_BSS(DWORD, g_mi_725ef8, 0x725ef8)
KING_BSS(DWORD, g_mi_725f00, 0x725f00)
KING_BSS(DWORD, g_mi_725f04, 0x725f04)
KING_BSS(DWORD, g_mi_725f08, 0x725f08)
KING_BSS(DWORD, g_mi_725f10, 0x725f10)
KING_BSS(DWORD, g_mi_725f14, 0x725f14)
KING_BSS(DWORD, g_mi_725f18, 0x725f18)
KING_BSS(MarineMatrix, g_mi_725e88, 0x725e88)
KING_BSS(float, g_mi_725e84, 0x725e84)
ASM_VAR(float, g_mi_6530c4, $L_6530c4)
KING_BSS(float, g_mi_725edc, 0x725edc)
ASM_VAR(float, g_mi_6530cc, $L_6530cc)
ASM_VAR(float, g_mi_6530c8, $L_6530c8)
KING_BSS(DWORD, g_mi_725ee0, 0x725ee0)
KING_BSS(DWORD, g_mi_725ee4, 0x725ee4)
KING_BSS(DWORD, g_mi_725ee8, 0x725ee8)
KING_BSS(DWORD, g_mi_725e78, 0x725e78)
KING_BSS(DWORD, g_mi_725e7c, 0x725e7c)
KING_BSS(DWORD, g_mi_725e80, 0x725e80)
KING_BSS(DWORD, g_mi_725ed0, 0x725ed0)
KING_BSS(DWORD, g_mi_725ed4, 0x725ed4)
KING_BSS(DWORD, g_mi_725ed8, 0x725ed8)
KING_BSS(float, g_mi_725ec8, 0x725ec8)
ASM_VAR(float, g_mi_6530d0, $L_6530d0)
ASM_VAR(float, g_mi_6530ac, $L_6530ac)
KING_BSS(float, g_mi_725f78, 0x725f78)
ASM_VAR(float, g_mi_653140, $L_653140)
KING_BSS(DWORD, g_mi_725f90, 0x725f90)
KING_BSS(DWORD, g_mi_725f94, 0x725f94)
KING_BSS(DWORD, g_mi_725f98, 0x725f98)
KING_BSS(DWORD, g_mi_725fa0, 0x725fa0)
KING_BSS(DWORD, g_mi_725fa4, 0x725fa4)
KING_BSS(DWORD, g_mi_725fa8, 0x725fa8)
KING_BSS(DWORD, g_mi_725fb0, 0x725fb0)
KING_BSS(DWORD, g_mi_725fb4, 0x725fb4)
KING_BSS(DWORD, g_mi_725fb8, 0x725fb8)
KING_BSS(MarineMatrix, g_mi_725f38, 0x725f38)
KING_BSS(float, g_mi_725f34, 0x725f34)
ASM_VAR(float, g_mi_653158, $L_653158)
KING_BSS(float, g_mi_725f7c, 0x725f7c)
ASM_VAR(float, g_mi_653160, $L_653160)
ASM_VAR(float, g_mi_65315c, $L_65315c)
KING_BSS(DWORD, g_mi_725f80, 0x725f80)
KING_BSS(DWORD, g_mi_725f84, 0x725f84)
KING_BSS(DWORD, g_mi_725f88, 0x725f88)
KING_BSS(DWORD, g_mi_725f28, 0x725f28)
KING_BSS(DWORD, g_mi_725f2c, 0x725f2c)
KING_BSS(DWORD, g_mi_725f30, 0x725f30)
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
