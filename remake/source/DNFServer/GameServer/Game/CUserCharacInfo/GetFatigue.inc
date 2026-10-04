unsigned short CUserCharacInfo::GetFatigue() const
{
    return m_selected ? m_selected->m_fatigue : 0;
}