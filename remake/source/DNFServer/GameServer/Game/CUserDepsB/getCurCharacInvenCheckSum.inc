int CUser::getCurCharacInvenCheckSum() const
{
    if (getCurCharacR() == 0)                      // W 0x08120432
        return 0;
    // b28d-solo-CUserDepsB: AE——P52/P0 单语句链式实参（两实参升格 %esi/%ebx 跨
    // receiver 调用保活）+ P248 档③字段类型旋钮：m_checkSumSeed 由 int 改
    // unsigned int（Main 代落，连同 m_backupSeed）后复现 ORIG 0x0864e0e2 两段
    // `mov 0x796e0(%eax),%eax; mov %eax,%esi`（int 字段 4.4 恒折单段）。
    return getCurCharacInvenR()->makeCheckSum(getPVPResultR()->m_winPoint,
                                              m_checkSumSeed);   // T 0x0850ae7a
