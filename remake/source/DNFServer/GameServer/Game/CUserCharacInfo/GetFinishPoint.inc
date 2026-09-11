int CUserCharacInfo::GetFinishPoint()
{
    if (m_selected)
        return CUR->m_finishPoint;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x244, 5);
    trace("WARAREA CUserCharacInfo::GetFinishPoint() : m_selected is NULL");
    return 0;
}