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