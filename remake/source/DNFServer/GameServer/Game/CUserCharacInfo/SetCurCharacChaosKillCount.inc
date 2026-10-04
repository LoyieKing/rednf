void CUserCharacInfo::SetCurCharacChaosKillCount(int count)
{
    if (m_selected)
    {
    m_selected->m_chaosKillCount = count;
    }
}