void CUserCharacInfo::setPowerWarHP(unsigned short hp)
{
    if (m_selected)
    {
    m_selected->m_powerWarData.m_data0 = hp;
    }
}