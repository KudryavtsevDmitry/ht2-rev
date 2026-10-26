// Marine subsystem startup coefficients, vectors and default work limits.
#include "king.h"
#include "x87.h"
struct MarineTriple { float v[3]; };
struct MarineMatrix { DWORD v[16]; };
struct MarineConfig { DWORD v[15]; };
KING_VAR(float, g_mi_652f50, 652f50)
KING_VAR(float, g_mi_652f54, 652f54)
KING_VAR(float, g_mi_64c250, 64c250)
KING_VAR(float, g_mi_652f70, 652f70)
KING_VAR(float, g_mi_652f78, 652f78)
KING_VAR(float, g_mi_652f74, 652f74)
KING_VAR(float, g_mi_652f7c, 652f7c)
KING_VAR(float, g_mi_652f58, 652f58)
KING_VAR(float, g_mi_652ff4, 652ff4)
KING_VAR(float, g_mi_653094, 653094)
KING_VAR(float, g_mi_64d4e8, 64d4e8)
KING_VAR(float, g_mi_64c258, 64c258)
KING_VAR(float, g_mi_6530a0, 6530a0)
KING_VAR(float, g_mi_6530a4, 6530a4)
KING_VAR(float, g_mi_64c334, 64c334)
KING_VAR(float, g_mi_725d58, 725d58)
KING_VAR(float, g_mi_725db4, 725db4)
KING_VAR(float, g_mi_725d6c, 725d6c)
KING_VAR(float, g_mi_725dc4, 725dc4)
KING_VAR(float, g_mi_725db0, 725db0)
KING_VAR(float, g_mi_725e14, 725e14)
KING_VAR(float, g_mi_725e64, 725e64)
KING_VAR(float, g_mi_725e6c, 725e6c)
KING_VAR(MarineTriple, g_mi_725dd8, 725dd8)
KING_VAR(MarineTriple, g_mi_725de8, 725de8)
KING_VAR(MarineTriple, g_mi_725df8, 725df8)
KING_VAR(MarineTriple, g_mi_725dc8, 725dc8)
KING_VAR(MarineTriple, g_mi_725d60, 725d60)
KING_VAR(MarineTriple, g_mi_725db8, 725db8)
KING_VAR(MarineTriple, g_mi_725e18, 725e18)
KING_VAR(MarineMatrix, g_mi_725d70, 725d70)
KING_VAR(MarineConfig, g_mi_725e28, 725e28)
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
KING_VAR(float, g_mi_652f44, 652f44)
KING_VAR(float, g_mi_725d48, 725d48)
KING_VAR(float, g_mi_6533f4, 6533f4)
KING_VAR(float, g_mi_72c150, 72c150)
extern "C" void __cdecl MarineInit_DoubleTreeScale()
{
    g_mi_725d48=(float)((double)g_mi_652f44+g_mi_652f44);
}
extern "C" void __cdecl MarineInit_DoubleTerrainScale()
{
    g_mi_72c150=(float)((double)g_mi_6533f4+g_mi_6533f4);
}
KING_VAR(float, g_mi_6531d4, 6531d4)
KING_VAR(float, g_mi_725fc0, 725fc0)
extern "C" void __cdecl MarineInit_DoubleTransitionScale()
{
    g_mi_725fc0=(float)((double)g_mi_6531d4+g_mi_6531d4);
}

// Mesh motion and particle startup geometry and coefficients.
KING_VAR(float, g_mi_725ecc, 725ecc)
KING_VAR(float, g_mi_6530a8, 6530a8)
KING_VAR(DWORD, g_mi_725ef0, 725ef0)
KING_VAR(DWORD, g_mi_725ef4, 725ef4)
KING_VAR(DWORD, g_mi_725ef8, 725ef8)
KING_VAR(DWORD, g_mi_725f00, 725f00)
KING_VAR(DWORD, g_mi_725f04, 725f04)
KING_VAR(DWORD, g_mi_725f08, 725f08)
KING_VAR(DWORD, g_mi_725f10, 725f10)
KING_VAR(DWORD, g_mi_725f14, 725f14)
KING_VAR(DWORD, g_mi_725f18, 725f18)
KING_VAR(MarineMatrix, g_mi_725e88, 725e88)
KING_VAR(float, g_mi_725e84, 725e84)
KING_VAR(float, g_mi_6530c4, 6530c4)
KING_VAR(float, g_mi_725edc, 725edc)
KING_VAR(float, g_mi_6530cc, 6530cc)
KING_VAR(float, g_mi_6530c8, 6530c8)
KING_VAR(DWORD, g_mi_725ee0, 725ee0)
KING_VAR(DWORD, g_mi_725ee4, 725ee4)
KING_VAR(DWORD, g_mi_725ee8, 725ee8)
KING_VAR(DWORD, g_mi_725e78, 725e78)
KING_VAR(DWORD, g_mi_725e7c, 725e7c)
KING_VAR(DWORD, g_mi_725e80, 725e80)
KING_VAR(DWORD, g_mi_725ed0, 725ed0)
KING_VAR(DWORD, g_mi_725ed4, 725ed4)
KING_VAR(DWORD, g_mi_725ed8, 725ed8)
KING_VAR(float, g_mi_725ec8, 725ec8)
KING_VAR(float, g_mi_6530d0, 6530d0)
KING_VAR(float, g_mi_6530ac, 6530ac)
KING_VAR(float, g_mi_725f78, 725f78)
KING_VAR(float, g_mi_653140, 653140)
KING_VAR(DWORD, g_mi_725f90, 725f90)
KING_VAR(DWORD, g_mi_725f94, 725f94)
KING_VAR(DWORD, g_mi_725f98, 725f98)
KING_VAR(DWORD, g_mi_725fa0, 725fa0)
KING_VAR(DWORD, g_mi_725fa4, 725fa4)
KING_VAR(DWORD, g_mi_725fa8, 725fa8)
KING_VAR(DWORD, g_mi_725fb0, 725fb0)
KING_VAR(DWORD, g_mi_725fb4, 725fb4)
KING_VAR(DWORD, g_mi_725fb8, 725fb8)
KING_VAR(MarineMatrix, g_mi_725f38, 725f38)
KING_VAR(float, g_mi_725f34, 725f34)
KING_VAR(float, g_mi_653158, 653158)
KING_VAR(float, g_mi_725f7c, 725f7c)
KING_VAR(float, g_mi_653160, 653160)
KING_VAR(float, g_mi_65315c, 65315c)
KING_VAR(DWORD, g_mi_725f80, 725f80)
KING_VAR(DWORD, g_mi_725f84, 725f84)
KING_VAR(DWORD, g_mi_725f88, 725f88)
KING_VAR(DWORD, g_mi_725f28, 725f28)
KING_VAR(DWORD, g_mi_725f2c, 725f2c)
KING_VAR(DWORD, g_mi_725f30, 725f30)
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

// [$L_60f050] static initializer: a float constant doubled
KING_VAR(float, g_mi_725e10, 725e10)
KING_VAR(float, g_mi_653090, 653090)
extern "C" void __cdecl Marine_InitDoubled() { g_mi_725e10 = g_mi_653090 + g_mi_653090; }
