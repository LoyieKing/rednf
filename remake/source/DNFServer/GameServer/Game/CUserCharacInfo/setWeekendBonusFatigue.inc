void CUserCharacInfo::setWeekendBonusFatigue(unsigned short value)
{
    if (m_selected)
    {
    CUR->m_maxFatigue = (unsigned short)(DEFAULT_MAX_FATIGUE + value);
    }
