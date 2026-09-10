void CUserCharacInfo::setWeekendBonusExp(unsigned int exp, int percent)
{
    if (m_selected)
    {
        CUR->m_weekendBonusExp = (int)((double)exp * (percent / 100.0f));
    }
