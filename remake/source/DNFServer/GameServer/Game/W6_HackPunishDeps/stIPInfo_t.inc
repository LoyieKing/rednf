CAutoPunishRuleBlackIPMgr::stIPInfo_t::stIPInfo_t(unsigned char minIp,
                                                  unsigned char maxIp)
{
    m_minIp = minIp;   // mov %dl,(%eax)
    m_maxIp = maxIp;   // mov %dl,0x1(%eax)
