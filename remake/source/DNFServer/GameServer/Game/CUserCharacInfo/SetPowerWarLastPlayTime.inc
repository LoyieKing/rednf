void CUserCharacInfo::SetPowerWarLastPlayTime(long t)
{
    if (m_selected)
    {
    m_selected->m_powerWarData.m_data8 = t;
    }
}