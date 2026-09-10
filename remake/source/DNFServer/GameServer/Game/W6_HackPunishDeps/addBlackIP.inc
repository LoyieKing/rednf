bool CAutoPunishRuleBlackIPMgr::addBlackIP(unsigned long addr, unsigned char minIp,
                                           unsigned char maxIp)
{
    // b28d-solo-addBlackIP: ORIG make_pair 实例第二参为 stIPInfo_t&&（mangled OS9_）
    // ⇒ 原码是匿名右值临时而非具名左值 info。ORIG 0x084eb5c0。
    m_map.insert(std::make_pair(addr, stIPInfo_t(minIp, maxIp)));
    return true;   // mov $0x1,%eax
