void CUserCharacInfo::SetCurCharacChaosResponTime(int t)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosResponTime = t;
    }
