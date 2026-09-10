unsigned int CUserCharacInfo::UseFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x220, 5);
        trace("WARAREA CUserCharacInfo::UseFinishPoint() : m_selected is NULL");
        return 0;
    }
    ChangeFinishPoint(-point);
    ((CUser*)this)->m_historyLog.FPSub(
        point, CUR->m_finishPoint, CUR->m_finishPointTotal, eFPSubReason_0);
    return CUR->m_finishPoint;
