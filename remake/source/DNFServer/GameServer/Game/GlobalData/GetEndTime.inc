long WongWork::CBlackIPMonitor::GetEndTime(
    char const* ipStr, ENUM_BLACKIPMONITOR_PUNISH_TYPE type)
{
    // ORIG 0x84ebc8c：ip 为空返回 0；遍历 [lower_bound, upper_bound) 返回
    // entry.m_punishType == type 的 m_endTime，否则 0。
    if (!ipStr)
        return 0;
    // ORIG 0x84ebca8/0x84ebce4：inet_addr 两次独立调用（P48）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    for (; it != end; ++it)
    {
        // ORIG 0x84ebd27..0x84ebd3c：entry 整体拷贝到 -0x30..-0x24 后比较。
        stBlackIPMonitorEntry_t entry = it->second;
        if (entry.m_punishType == type)
        {
            return entry.m_endTime;
        }
    }
    return 0;
