void CUserCharacInfo::SetCurCharacLuckPoint(int point)
{
    if (m_selected)
    {
    if (point - CUR->m_luckPoint == 100000)
    {
        CUR->m_luckPoint = 100000;
    }
    else
    {
        CUR->m_luckPoint = point;
        if (CUR->m_luckPoint < 0)
            CUR->m_luckPoint = 0;
        else if (CUR->m_luckPoint > 99999)
            CUR->m_luckPoint = 99999;
    }
    }
}