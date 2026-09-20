void CUserCharacInfo::setWeekendBonusFatigue(unsigned short value)
{
    if (m_selected)
    {
    m_selected->m_field35 = (unsigned short)(DEFAULT_MAX_FATIGUE + value);
    }
}