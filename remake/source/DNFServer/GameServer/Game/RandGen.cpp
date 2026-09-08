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

// [b28d-solo-RandGen N(跨版本 outgoing 预留/栈边界)] 判词 NEAR(4)：ORIG 0x08ae0244
// `sub $0xc` + u64 槽 -0x8/-0x4，我方 `sub $0x28` + 槽 -0x10/-0xc；其余 22 条逐条同序
// （magic 0x80008001 无符号取模、`mov $0,%edx` 高半字、fildll/flds/fmulp）。
// 根因 = 4.1 参数直写帧底 (%esp) ⇒ outgoing=实参 4B；4.4 -O0 恒预留 16B 对齐 outgoing 区。
// 实证：加 -mpreferred-stack-boundary=2 后本件 `sub $0xc`、槽 -0x8/-0x4 与 ORIG 逐条全等
// （探针 /tmp/rgprobe.cpp A7/A6、/tmp/tu_RandGen_sb2.o 该件 identical）；但该旗标是 TU 级，
// 会让同 TU 无参两件（getFloat01Ev/getFloatEff，现 AE）帧塌陷（0x18→0x8/0x4），
// dead 局部只撑帧不移槽（P232/N98），源码层亦无 per-function 开关
// （#pragma GCC target("preferred-stack-boundary=2") 被 4.4.7 拒绝）⇒ 不可用。
// 语义等价（同一 magic 取模 + 2^-16 乘），byte 层按 P190附注②/N95/N82 族封顶。
float RandGen::getFloat01(XorRand4* rng)
{
    return (float)((unsigned long long)(rng->getUInt32() % 65535u) * (1.0f / 65536.0f));
}

// [b28d-solo-RandGen N(同 getFloat01EP8XorRand4 栈边界族)] 判词 NEAR(3)：
// ORIG 0x08ae02a4 `sub $0x10` + span 槽 -0x4，我方 `sub $0x28` + span -0xc；
// 其余 10 条逐条同序（flds/fsubs/fstps → call → fmuls/fadds）。根因同上。
// 实证：-mpreferred-stack-boundary=2 下 `sub $0x8` + span -0x4（仍差 8：4.4 对同 TU
// 已定义 callee 继承 outgoing，P210②；callee 定义前置/后置/仅声明三形探针
// /tmp/gprobe.cpp G2/H2/I2 在 sb2 下均 0x8）⇒ 语义等价，byte 层封顶。
float RandGen::getFloat(XorRand4* rng, float min, float max)
{
    float span = max - min;
    return getFloat01(rng) * span + min;
}

// ORIG _ZN7RandGen8getFloatEff T 0x08ae0288（0x1c）：span=max-min 落 -0xc 槽、
// call getFloat01() 后 fmuls/fadds 逐条对位（P204① 补定义；头声明已备）。
float RandGen::getFloat(float min, float max)
{
    float span = max - min;
    return getFloat01() * span + min;
}


