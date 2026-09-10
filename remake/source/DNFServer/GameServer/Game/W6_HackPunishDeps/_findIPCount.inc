int CAutoPunishRuleHackTypeMgr::_findIPCount(unsigned int a, unsigned int b,
                                             char const* ipStr)
{
    // b28d-solo-_findIPCount: ORIG 循环体对 it->second 做 0x18 字节按值拷贝到
    // -0x38（6 条 mov 对），prefix 用 4 条 movl 内联清零，且 m_b 与 strncmp 为
    // 两个独立守卫落同一 continue 出口。残差 1 条（gap=+1，dual_rank 唯一候选）：
    // ORIG `lea -0x48(%ebp),%eax; movzbl (%eax),%eax`（4.1 数组元素读恒发地址），
    // 4.4 恒折 `movzbl -0x48(%ebp),%eax`。本轮再探针三形（/tmp/probe_lea.cpp：
    // `prefix[0]`、`*prefix`、`*(char*)((unsigned int)prefix)`）**全部折 disp**；
    // 具名指针/引用形能出 lea 但多占 4B 槽破坏帧 0x58 ⇒ N117/N102 封顶。
    // ORIG 0x080f9322。
    typedef std::multimap<unsigned short, HackIPCounter_t> IpCounterMap;
    IpCounterMap::const_iterator it = m_ipCounterMap.lower_bound((unsigned short)a);
    IpCounterMap::const_iterator endIt = m_ipCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        HackIPCounter_t entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            char prefix[0x10] = {0};
            Divide_C_ClassIP_EndIP(prefix, (char*)0, (char*)ipStr);
            if (prefix[0] != '\0')
            {
                if (strncmp(prefix, entry.m_ip, 0xc) == 0)
                {
                    return (int)entry.m_cnt;
                }
            }
        }
    }
    return 0;
