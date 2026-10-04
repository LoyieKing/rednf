int CUserCharacInfo::GetFinishPointTotal()
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x26a, 5);
        trace("WARAREA CUserCharacInfo::GetFinishPointTotal() : m_selected is NULL");
        return 0;
    }
    return m_selected->m_finishPointTotal;
}