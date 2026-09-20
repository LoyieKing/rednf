void CUserCharacInfo::AddCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_fielde49 += point;
    }
}