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
    return (float)((unsigned long long)(rng->getUInt32() % 65535u) * (1.0f / 65536.0f));
}

float RandGen::getFloat(float min, float max)
{
    float span = max - min;
    return getFloat01() * span + min;
}

float RandGen::getFloat(XorRand4* rng, float min, float max)
{
    float span = max - min;
    return getFloat01(rng) * span + min;
}
