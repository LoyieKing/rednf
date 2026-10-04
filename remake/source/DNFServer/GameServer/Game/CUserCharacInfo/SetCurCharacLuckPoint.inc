void CUserCharacInfo::SetCurCharacLuckPoint(int point)
{
    if (m_selected)
    {
    if (point - m_selected->m_luckPoint == 100000)
    {
        m_selected->m_luckPoint = 100000;
    }
    else
    {
        m_selected->m_luckPoint = point;
        if (m_selected->m_luckPoint < 0)
            m_selected->m_luckPoint = 0;
        else if (m_selected->m_luckPoint > 99999)
            m_selected->m_luckPoint = 99999;
    }
    }
}