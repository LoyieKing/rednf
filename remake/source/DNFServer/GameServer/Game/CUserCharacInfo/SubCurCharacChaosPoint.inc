void CUserCharacInfo::SubCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_fielde49 -= point;
    if (m_selected->m_fielde49 < 0)
        m_selected->m_fielde49 = 0;
    }
}