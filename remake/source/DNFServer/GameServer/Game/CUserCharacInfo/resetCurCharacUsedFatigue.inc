void CUserCharacInfo::resetCurCharacUsedFatigue()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field4d = 0;
    }
}