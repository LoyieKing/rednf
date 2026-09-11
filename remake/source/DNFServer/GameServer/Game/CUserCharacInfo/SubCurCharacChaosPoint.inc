void CUserCharacInfo::SubCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint -= point;
    if (CUR->m_chaosPoint < 0)
        CUR->m_chaosPoint = 0;
    }
}