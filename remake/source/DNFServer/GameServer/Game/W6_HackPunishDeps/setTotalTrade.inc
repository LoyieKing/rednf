// ----------------------------------------------------------------------------
// setTotalTrade —— ORIG W 0x084eb9d4 (0x2c)：+0x18=type，+0x1c..=total(u64)。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setTotalTrade(unsigned int type,
                                               unsigned long long total)
{
    m_totalTradeType = type;
    m_totalTradeValue = total;
}