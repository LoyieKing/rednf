void CUserCharacInfo::incCurCharacUsedFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field4d = (unsigned short)(m_selected->m_field4d + value);
    }
}