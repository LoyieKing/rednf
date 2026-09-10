unsigned short stFatigueBatteryConstantTable_t::getLimitGrownupBuffCntFatigueBattery(
    int level) const
{
    // [b28d-solo-CFatigueBatteryHandle] verdict = N1 封顶（跨版本常量折叠），
    // 非 identical 唯一成因；语义等价（下标恒等），p167 CLEAR（0 call，gap=1）。
    // b28d-solo 复证（19 形探针 /tmp/s99/p1.cpp+p2.cpp+p3.cpp，4.4.7 -m32 -O0）：
    // ORIG 尾段 `lea -0x1(%eax),%edx; add $0x8,%edx; movzwl 0x2(%eax,%edx,2),%eax`
    // 为 (level-1)+8 两步；数组下标形（A/B/D/E/G/J/M/N/O/P/Q/R 共 12 形，含
    // 8u 混合类型 / (8,8) 逗号 / 成员指针差 / sizeof 常量源 / (int)8L）恒折
    // `lea 0x7(%eax),%edx`；指针算术形（C/F/H/I/K/L 6 形）反产 add/add/lea 四段；
    // (unsigned char) 截断形（T）产 subl+movzbl+lea 三段，均不闭合。
    // b28d-20: ORIG 0x084a84b6 双守卫合并短路形——ORIG `jg fail; cmpl $0,level;
    // jg ok; fail: mov$0; jmp; ok: body` 的唯一 4.4 复现形是
    // `if (a > end || level <= 0) return 0;`（正极性 ||，fail 块 fall-through、
    // ok 块直落，无 goto 的 jmp/nop 中继）。探针 /tmp/p_fat1.cpp getD 19 条与
    // ORIG 逐条同序（jg fail / jg body），残 1 条 = 4.4 常量折叠
    // `(level-1)+8`→`lea 0x7(%eax)`（ORIG `lea -0x1`+`add $0x8` 两步）。
    if (level - 1 > static_cast<int>(m_endLevel10) || level <= 0)
        return 0;
    return m_limitTable[level - 1 + 8];
