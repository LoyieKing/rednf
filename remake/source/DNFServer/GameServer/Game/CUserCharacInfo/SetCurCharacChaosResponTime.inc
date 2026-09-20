void CUserCharacInfo::SetCurCharacChaosResponTime(int t)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field12ad = t;
    }
}