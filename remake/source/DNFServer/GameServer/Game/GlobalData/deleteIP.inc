// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用（P48）+ entry 整体拷贝（P157）；NDIFF 25→0 EQ | ORIG 0x084ebace
char WongWork::CBlackIPMonitor::deleteIP(char const* ipStr, unsigned int type)
{
    // ORIG 0x84ebace：ip 为空返回 0；inet_addr 转 key；遍历 [lower_bound, upper_bound)
    // 删除 entry.m_punishType == type 的项。
    if (!ipStr)
        return 0;
    // ORIG 0x84ebaea/0x84ebb14：inet_addr 调用两次（O0 无 CSE，P48），
    // 结果分别落 -0x10/-0xc 两槽（lower_bound/upper_bound 各一）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    while (it != end)
    {
        // ORIG 0x84ebb3a..0x84ebb58：先把 it->second 整体拷到栈（4 字段
        // 逐 dword 落 -0x28/-0x24/-0x20/-0x1c），再比较 m_punishType。
        stBlackIPMonitorEntry_t entry = it->second;
        if (static_cast<unsigned int>(entry.m_punishType) == type)
        {
            // ORIG 0x84ebace：命中即 erase 并返回 1，未命中返回 0。
            m_monitorMap.erase(it);
            return 1;
        }
        ++it;
    }
    return 0;
}