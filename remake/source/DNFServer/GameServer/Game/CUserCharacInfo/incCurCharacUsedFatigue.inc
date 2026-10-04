void CUserCharacInfo::incCurCharacUsedFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_usedFatigue = (unsigned short)(m_selected->m_usedFatigue + value);
    }
}