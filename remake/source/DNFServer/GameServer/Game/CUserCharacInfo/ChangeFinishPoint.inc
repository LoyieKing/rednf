void CUserCharacInfo::ChangeFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x24d, 5);
        trace("WARAREA CUserCharacInfo::ChangeFinishPoint() : m_selected is NULL");
        return;
    }
    if (point > 0)
        m_selected->m_finishPointTotal += point;
    m_selected->m_finishPoint += point;
    if (m_selected->m_finishPoint < 0)
        m_selected->m_finishPoint = 0;
}