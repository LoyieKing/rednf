void CUserCharacInfo::setWeekendBonusFatigue(unsigned short value)
{
    if (m_selected)
    {
    m_selected->m_maxFatigue = (unsigned short)(DEFAULT_MAX_FATIGUE + value);
    }
}