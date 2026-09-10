void CUser::AddMoneyMinus(int value)
{
    // 残差 N81 附注族，同 AddMoneyPlus（load 中转寄存器 4.4 恒 eax）。
    if (exceptedStateMoneyLog())
    {
        return;
    }
    const int now = GlobalData::s_systemTime_.getCurSec();
    NextCharacMoneyLog(now);
    m_moneyLogs[m_moneyLogIdx].m_money[2] = now;
    m_moneyLogs[m_moneyLogIdx].m_moneyU[1] += value;
