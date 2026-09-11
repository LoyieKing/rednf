void CUserCharacInfo::AddCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint += point;
    }
}