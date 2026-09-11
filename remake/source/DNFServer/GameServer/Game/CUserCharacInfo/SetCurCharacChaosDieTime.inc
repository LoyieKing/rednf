void CUserCharacInfo::SetCurCharacChaosDieTime(int t)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_chaosDieTime = t;
    }
}