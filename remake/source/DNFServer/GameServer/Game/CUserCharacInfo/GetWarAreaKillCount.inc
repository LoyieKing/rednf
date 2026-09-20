float CUserCharacInfo::GetWarAreaKillCount(int index)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x260, 5);
        trace("WARAREA CUserCharacInfo::GetWarAreaKillCount() : m_selected is NULL");
        return 0.0f;
    }
    return m_selected->m_warAreaKill[index];
}
