int CUser::getCurCharacSkillCheckSum() const
{
    if (getCurCharacR() == 0)
        return 0;
    // b28d-solo-CUserDepsB: AE——P52/P0 单语句链式实参（seed 升格 %ebx 跨
    // getCurCharacSkillR 保活）+ P248 档③字段 unsigned 化后复现 ORIG 0x0864e139
    // `mov 0x796e0(%eax),%eax; mov %eax,%ebx` 两段。
    return reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->makeCheckSum(m_checkSumSeed);            // T 0x086066f2
