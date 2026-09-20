unsigned short CUserCharacInfo::getCurCharacFatigue() const
{
    return m_selected ? m_selected->m_field33 : 0;
}