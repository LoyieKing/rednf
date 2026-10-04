unsigned short CUserCharacInfo::getCurCharacFatigue() const
{
    return m_selected ? m_selected->m_fatigue : 0;
}