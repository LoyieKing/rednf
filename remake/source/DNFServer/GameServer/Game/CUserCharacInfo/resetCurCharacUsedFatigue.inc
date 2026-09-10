void CUserCharacInfo::resetCurCharacUsedFatigue()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_usedFatigue = 0;
    }
