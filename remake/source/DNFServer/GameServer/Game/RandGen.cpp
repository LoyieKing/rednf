// RandGen：静态 float 随机辅助。ORIG 符号：
//   getFloat01()             T 0x08ae0206
//   getFloat01(XorRand4*)    T 0x08ae0244
//   getFloat(float,float)    T 0x08ae0288
//   getFloat(XorRand4*,ff)   T 0x08ae02a4
// 核心：(x % 65535) * (1/65536)，x 来自 XorRand4；常量 0x8e37cc0 = 0x37800000 = 2^-16。
#include "RandGen.h"
#include "XorRand4.h"

float RandGen::getFloat01()
{
    return (float)((unsigned long long)(XorRand4::getStaticUInt32() % 65535u) * (1.0f / 65536.0f));
}

float RandGen::getFloat01(XorRand4* rng)
{
    // b26 N 封顶（跨版本栈帧基线，同「outgoing 帧」豁免族）：残差仅
    // sub $0x28 vs $0xc 与 u64 槽 -0x10/-0xc vs -0x8/-0x4——4.4 outgoing
    // 最小 0x10 + float 帧预算恒 ≥0x28，ORIG 4.1 紧凑 0xc 不可达
    // （直传/拆局部/register 4 源形探针 /tmp/rgp 均不可达）。
    return (float)((unsigned long long)(rng->getUInt32() % 65535u) * (1.0f / 65536.0f));
}

float RandGen::getFloat(XorRand4* rng, float min, float max)
{
    float span = max - min;
    // b26 N 封顶（同上）：sub $0x28 vs $0x10、span 槽 -0xc vs -0x4，纯帧基线差。
    return getFloat01(rng) * span + min;
}

// ORIG _ZN7RandGen8getFloatEff T 0x08ae0288（0x1c）：span=max-min 落 -0xc 槽、
// call getFloat01() 后 fmuls/fadds 逐条对位（P204① 补定义；头声明已备）。
float RandGen::getFloat(float min, float max)
{
    float span = max - min;
    return getFloat01() * span + min;
}


