void WongWork::CAutoPunishRuleHackTypeMgr::reset()
{
    // ORIG 0x82a5a9c：m_hackTypeMap.clear(); +0x18/+0x1c/+0x20 清零。
    m_hackTypeMap.clear();
    m_totalTradeType = 0;
    m_totalTradeValue = 0;
