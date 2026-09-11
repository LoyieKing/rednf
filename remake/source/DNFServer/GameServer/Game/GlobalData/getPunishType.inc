// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝；NDIFF 46→0 EQ | ORIG 0x084ebba6
unsigned int WongWork::CBlackIPMonitor::getPunishType(char const* ipStr)
{
    // ORIG 0x84ebba6：ip 为空返回 0；遍历 [lower_bound, upper_bound) 累 OR
    // entry.m_punishType。
    if (!ipStr)
        return 0;
    unsigned int result = 0;
    // ORIG 0x84ebba6：inet_addr 两次独立调用（P48）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    for (; it != end; ++it)
    {
        // ORIG 0x84ebc2x：entry 整体拷贝到栈后取 m_punishType（P48 同族）。
        stBlackIPMonitorEntry_t entry = it->second;
        result |= static_cast<unsigned int>(entry.m_punishType);
    }
    return result;
}