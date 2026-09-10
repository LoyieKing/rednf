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
        CUR->m_finishPointTotal = 0;
    }
    else
    {
        CUR->m_finishPointTotal -= point;
        if (CUR->m_finishPointTotal < 0)
            CUR->m_finishPointTotal = 0;
    }
    ((CUser*)this)->m_historyLog.FPSub(
        point, CUR->m_finishPoint, CUR->m_finishPointTotal, (eFPSubReason)3);
