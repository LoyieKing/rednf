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
