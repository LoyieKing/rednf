void CUserCharacInfo::setCurCharacFatigue(int fatigue)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_fatigue = (unsigned short)fatigue;
    }
}