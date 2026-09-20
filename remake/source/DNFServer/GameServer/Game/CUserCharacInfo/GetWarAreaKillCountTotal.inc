float CUserCharacInfo::GetWarAreaKillCountTotal()
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x22f, 5);
        trace("WARAREA CUserCharacInfo::GetWarAreaKillCountTotal() : m_selected is NULL");
        return 0.0f;
    }
    float total = 0.0f;
    for (int i = 0; i < 4; ++i)
        total += m_selected->m_warAreaKill[i];
    return total;
}
