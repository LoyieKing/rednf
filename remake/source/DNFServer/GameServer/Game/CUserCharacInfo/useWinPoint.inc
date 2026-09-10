void CUserCharacInfo::useWinPoint(int point, eWPSubReason)
{
    enableSavePvP();
    if (m_pvpResult.m_winPoint >= point)
        m_pvpResult.m_winPoint -= point;
    else
        m_pvpResult.m_winPoint = 0;
