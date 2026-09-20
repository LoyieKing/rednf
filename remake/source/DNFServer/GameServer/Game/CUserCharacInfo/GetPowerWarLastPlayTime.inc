long CUserCharacInfo::GetPowerWarLastPlayTime()
{
    return m_selected ? m_selected->m_powerWarData.m_data8 : 0;
}