void CUser::AddMoneyPlus(int value)
{
    // 残差 N81 附注族（4.4 恒形）：尾句 `+=` 的 load 4.4 恒 eax 中转+mov 转发+
    // add mem 直加（ORIG 4.1 mem→ecx 直载+value→eax+add eax,ecx）。穷举 +=/自加
    // 表达式/操作数交换/引用局部/寄存器复刻（nl11_probe4/5/7）均不可翻。
    if (exceptedStateMoneyLog())
    {
        return;
    }
    const int now = GlobalData::s_systemTime_.getCurSec();
    NextCharacMoneyLog(now);
    m_moneyLogs[m_moneyLogIdx].m_money[2] = now;
    m_moneyLogs[m_moneyLogIdx].m_moneyU[0] += value;
