bool CUserPremium::startPremium(int type, bool force)   // 0x086adc6a；调用点 xor $0x1 ⇒ bool（Main 代落 CUser.h 同步）
{
    // ORIG 0x86adc6a：无 entry/def 缓存；state/start/end 各独立
    // this+type*0x14 链，checkTerm/kind 直读 g_SPremiumInfo 表（T-C9-1）
    if (!force && m_entries[type].m_state != 2)
        return 0;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    if (g_SPremiumInfo[type].m_checkTerm != 0)
    {
        if (curTime < m_entries[type].m_start)
        {
            m_entries[type].m_state = 2;
            return 0;
        }
        if (m_entries[type].m_end <= curTime)
            return 0;
    }
    if (g_SPremiumInfo[type].m_kind == 1)
        ++(m_normalCount);
    else
        ++(m_restrictedCount);
    m_entries[type].m_state = 1;
    return 1;
}