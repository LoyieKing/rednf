void CUserCharacInfo::SetCurCharacLuckPoint(int point)
{
    if (m_selected)
    {
    if (point - m_selected->m_fielde71 == 100000)
    {
        m_selected->m_fielde71 = 100000;
    }
    else
    {
        m_selected->m_fielde71 = point;
        if (m_selected->m_fielde71 < 0)
            m_selected->m_fielde71 = 0;
        else if (m_selected->m_fielde71 > 99999)
            m_selected->m_fielde71 = 99999;
    }
    }
}