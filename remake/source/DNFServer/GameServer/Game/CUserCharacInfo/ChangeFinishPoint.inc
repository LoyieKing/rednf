void CUserCharacInfo::ChangeFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x24d, 5);
        trace("WARAREA CUserCharacInfo::ChangeFinishPoint() : m_selected is NULL");
        return;
    }
    if (point > 0)
        CUR->m_finishPointTotal += point;
    CUR->m_finishPoint += point;
    if (CUR->m_finishPoint < 0)
        CUR->m_finishPoint = 0;
}