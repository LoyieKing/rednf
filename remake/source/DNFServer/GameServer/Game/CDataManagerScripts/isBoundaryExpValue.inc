// [b28d-solo-CDataManagerScripts] NEAR(1)：p167 CLEAR（4/4）。N22 —— 4.4 恒
// `cmp %ebx,%eax`（dst=call 结果/字段侧），ORIG 4.1 `cmp %eax,%ebx`（dst=局部 e）；
// 本轮 fresh 复验：交换源码操作数（`e == m_expVec[i].m_exp`）仍产同形，P248 三档试尽；
// 语义等价；ORIG 0x0849f420。
// [GW20Run 2026-09-11 复核] fresh 32/32；ORIG 0x0849f447 `cmp %eax,%ebx; sete` vs
// 我方 `cmp %ebx,%eax; sete`（P196 折痕；交换源码操作数历史实测同形、P248 三档试尽）
// ⇒ 维持 N22/N18b。CUser.cpp:2352 消费点只读返回值，本件不动签名/mangled。
bool STExpertJobScript::isBoundaryExpValue(int exp)
{
    // [b28c-L18] NEAR（fresh 32=32，助记符序列全等，仅 cmp 操作数序差 2 行）：
    // ORIG 把 int 形参 exp 每轮 `mov 0xc(%ebp),%ebx` 提升 %ebx 跨 operator[]
    // call 保活（push ebx/sub $0x24 + `cmp %eax,%ebx`）。修法=裸 `register int e`
    // **置于循环体内**（声明在体外则 %ebx 装载位置前移，d 4；无裸 register 则
    // 4.4 折 `cmp 0xc(%ebp),%eax` 内存直比，d 10）。残余 N18b（4.4 恒
    // `cmp MEM/reg,reg` 目标寄存器为字段侧，方向翻转两形均不可改）。
    // 三条件齐备（跨 call 保活 / ORIG 目标 callee-saved / fresh 非劣化）故保留
    // 裸 register（C28L26 分场景口径）。
    size_t i = 0;
    for (; i < m_expVec.size(); ++i)
    {
        register int e = exp;
        if (m_expVec[i].m_exp == e)
            return true;
    }
    return false;
}