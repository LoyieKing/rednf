void CUserCharacInfo::SubCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_chaosPoint -= point;
    if (m_selected->m_chaosPoint < 0)
        m_selected->m_chaosPoint = 0;
    }
}