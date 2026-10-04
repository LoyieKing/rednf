void CUserCharacInfo::SetCurCharacChaosKillTime(int t)
{
    if (m_selected)
    {
    m_selected->m_chaosKillTime = t;
    }
}