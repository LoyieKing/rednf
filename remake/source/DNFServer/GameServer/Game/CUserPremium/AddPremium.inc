void CUserPremium::AddPremium(int type, int start, int end, int flag)
{
    // ORIG 0x86adad2：每个字段读写独立重算 this+type*0x14 链（T-C9-1，
    // 无 entry/def 指针缓存）；m_applyDayMask[0] 直读 +0xac。
    if (m_entries[type].m_type != 0)
        RemovePremium(type);
    m_entries[type].m_type = type;
    m_entries[type].m_start = start;
    m_entries[type].m_end = end;
    m_entries[type].m_flag = flag;
    if (g_SPremiumInfo[type].m_applyDayMask[0] != -1)
        m_needCheckEveryday = 1;
    startPremium(type, true);
}