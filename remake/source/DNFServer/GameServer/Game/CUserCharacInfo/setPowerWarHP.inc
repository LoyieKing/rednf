void CUserCharacInfo::setPowerWarHP(unsigned short hp)
{
    if (m_selected)
    {
    CUR->m_powerWarHP = hp;
    }
}