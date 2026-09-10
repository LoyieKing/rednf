void CUserCharacInfo::set_charac_visible_teleport_(bool flag)
{
    if (m_selected)
    {
        CUR->m_visibleBits.teleport = flag;
    }
