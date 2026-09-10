unsigned int Encryption::SeedRand(unsigned int& seed)
{
    // [b28d-solo-Encryption] verdict=IDENTICAL 44=44。
    // ORIG 0x0848d51a：单累加器槽 -0x8 被 u1/u2/u3 复用
    // （imul→mov 槽→addl 槽，三次），结果累加器 -0x4 逐段 <<10/^=；
    // 原三独立局部写法生成 3 槽 + 不同求值序 ⇒ 按 ORIG 逐句还原。
    // r 声明 int（与 RHS 的 unsigned 异型，P211/P254）才出 ORIG 的
    // `mov %eax,%edx` 中转 + `xor %edx,%eax` 寄存器形（unsigned r 恒折内存 RMW）。
    unsigned int u = seed;
    u *= 0x41c64e6dU;
    u += 0x3039U;
    int r = (u >> 0x10) & 0x7ff;
    u *= 0x41c64e6dU;
    u += 0x3039U;
    r <<= 10;
    r = r ^ ((u >> 0x10) & 0x3ff);
    u *= 0x41c64e6dU;
    u += 0x3039U;
    r <<= 10;
    r = r ^ ((u >> 0x10) & 0x3ff);
    seed = u;
    return r;
