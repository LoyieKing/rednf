void CUserCharacInfo::DecreaseWarPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x297, 5);
        trace("WARAREA CUserCharacInfo::DecreaseWarPoint() : m_selected is NULL");
        return;
    }
    if (point < 1)
    {
        m_selected->m_finishPointTotal = 0;
    }
    else
    {
        m_selected->m_finishPointTotal -= point;
        if (m_selected->m_finishPointTotal < 0)
            m_selected->m_finishPointTotal = 0;
    }
    ((CUser*)this)->m_historyLog.FPSub(
        point, m_selected->m_finishPoint, m_selected->m_finishPointTotal, (eFPSubReason)3);
}