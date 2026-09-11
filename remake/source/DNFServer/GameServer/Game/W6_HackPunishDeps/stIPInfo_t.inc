// ----------------------------------------------------------------------------
// CAutoPunishRuleBlackIPMgr::stIPInfo_t —— ORIG W C1/C2(Ehh) 0x084eb598 (0x27)。
// ----------------------------------------------------------------------------
CAutoPunishRuleBlackIPMgr::stIPInfo_t::stIPInfo_t(unsigned char minIp,
                                                  unsigned char maxIp)
{
    m_minIp = minIp;   // mov %dl,(%eax)
    m_maxIp = maxIp;   // mov %dl,0x1(%eax)
}