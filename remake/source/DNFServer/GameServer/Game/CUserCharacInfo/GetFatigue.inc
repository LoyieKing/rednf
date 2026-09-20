unsigned short CUserCharacInfo::GetFatigue() const
{
    return m_selected ? m_selected->m_field33 : 0;
}