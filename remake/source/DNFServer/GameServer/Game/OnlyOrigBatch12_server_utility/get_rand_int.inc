int get_rand_int(int range)
{
    if (range < 0)
        return 0;
    // P286：ORIG `mov 0x8(%ebp),%eax; test %eax,%eax; jne` 形（形参载入+寄存器
    // 测试）只能由单 case switch 复现（if 形 4.4 恒折 `cmpl $0,mem`）。
    switch (range)
    {
    case 0:
        return rand();
    }
    int r = rand();
    // [b28d-solo-OnlyOrigBatch12] AE（Main 采纳 -march=i486 路由后，clean 口径
    // 73/73）：switch(range) 复现 ORIG `mov 0x8(%ebp),%eax; test` 形（P286）；
    // 早返（P16/P214②）+ 拆语句（P38）+ unsigned 尾段 div 全部归位。三处
    // `(r/65536)` 有符号除法 4.4.7 默认产 lea+test+cmovs，-march=i486 下转为
    // ORIG 的 mov/sar/shr/lea（P230 补；探针 /tmp/probe_rand.cpp 实证）。
    // ORIG 0x0808eafc
    if (r > range)
        return rand() % range;
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int c = static_cast<unsigned int>((r / 65536) & 0x7ff);
    r *= 0x41c64e6d;
    r += 0x3039;
    c <<= 10;
    c ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    c <<= 10;
    c ^= (r / 65536) & 0x3ff;
    // ORIG 尾段布局：`cmp c,%eax; jae <return c>`（fall-through 为 c%range）
    // ⇒ 条件反转写 `if (range < c) return c % range; return c;`；c 声明
    // unsigned int 令比较免 cast 中转（P209/P248 档③）。
    if (static_cast<unsigned int>(range) < c)
        return c % static_cast<unsigned int>(range);
    return c;
