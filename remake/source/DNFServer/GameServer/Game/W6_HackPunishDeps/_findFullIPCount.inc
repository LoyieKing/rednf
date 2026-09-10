int CAutoPunishRuleHackTypeMgr::_findFullIPCount(unsigned int a, unsigned int b,
                                                 char const* ipStr)
{
    // b28d-solo-_findFullIPCount: ORIG 循环体对 it->second 做 0x18 字节按值拷贝到
    // -0x38（6 条 mov 对），m_b 与 strncmp 为两个独立守卫落同一 continue 出口。
    // ORIG 0x080f9574。
    typedef std::multimap<unsigned short, HackFullIPCounter_t> FullCounterMap;
    FullCounterMap::const_iterator it = m_fullIPCounterMap.lower_bound((unsigned short)a);
    FullCounterMap::const_iterator endIt = m_fullIPCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        HackFullIPCounter_t entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            if (strncmp(ipStr, entry.m_ip, 0x10) == 0)
            {
                return (int)entry.m_cnt;
            }
        }
    }
    return 0;
