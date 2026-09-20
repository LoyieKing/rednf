void CUserCharacInfo::SetCurCharacChaosDieTime(int t)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    m_selected->m_fielde5d = t;
    }
}