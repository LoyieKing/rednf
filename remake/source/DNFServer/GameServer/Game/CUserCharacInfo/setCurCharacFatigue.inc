void CUserCharacInfo::setCurCharacFatigue(int fatigue)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_fatigue = (unsigned short)fatigue;
    }
}