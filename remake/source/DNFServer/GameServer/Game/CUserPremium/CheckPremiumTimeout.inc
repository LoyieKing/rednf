SUserPremiumInfo CUserPremium::CheckPremiumTimeout() const
{
    // ORIG 0x86add9c：result 即 sret（NRVO 直写，无栈局部）；entries[i] 每个
    // 判定点独立全链重算（T-C9-1）；checkTerm 直读 g_SPremiumInfo 表
    // （ORIG +0x63 无 helper call，原 static GetPremiumDefinition 属发明符号）。
    SUserPremiumInfo result;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    // [b28c-L7] P23 实参求值序：ORIG 0x086add9c 序为 `movl $&s_systemTime_,(%esp);
    // call getCurSec; movl $0x0,(%ebx)`——getCurSec 先于 result.m_type 清零落栈
    // ⇒ 清零语句必须写在取时之后（result 声明仍在先，占 sret/NRVO 通道）。
    result.m_type = 0;
    for (int i = 1; i <= 0x69; ++i)
    {
        if (m_entries[i].m_type == 0)
            continue;
        if (g_SPremiumInfo[m_entries[i].m_type].m_checkTerm == 0)
            continue;
        if (m_entries[i].m_state == 3)
            continue;
        if (m_entries[i].m_end <= curTime)
        {
            result = m_entries[i];
            break;
        }
    }
    return result;
