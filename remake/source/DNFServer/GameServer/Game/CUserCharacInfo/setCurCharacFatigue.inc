void CUserCharacInfo::setCurCharacFatigue(int fatigue)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field33 = (unsigned short)fatigue;
    }
}