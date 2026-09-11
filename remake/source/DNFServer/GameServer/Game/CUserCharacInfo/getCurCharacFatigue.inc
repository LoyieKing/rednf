unsigned short CUserCharacInfo::getCurCharacFatigue() const
{
    return m_selected ? CUR->m_fatigue : 0;
}