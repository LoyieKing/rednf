unsigned short CUserCharacInfo::getPowerWarHP()
{
    return m_selected ? m_selected->m_powerWarData.m_data0 : 0;
}