// ----------------------------------------------------------------------------
// getBlackIPCount —— ORIG W 0x084eb7ca (0x13)：tail-call multimap::size()。
// ----------------------------------------------------------------------------
int CAutoPunishRuleBlackIPMgr::getBlackIPCount()
{
    return (int)m_map.size();
}