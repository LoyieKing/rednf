unsigned short CUserCharacInfo::GetFatigue() const
{
    return m_selected ? CUR->m_fatigue : 0;
}