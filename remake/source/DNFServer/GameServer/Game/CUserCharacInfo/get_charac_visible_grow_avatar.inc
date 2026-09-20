bool CUserCharacInfo::get_charac_visible_grow_avatar()
{
    if (m_selected && ((m_selected->m_tagVisibleFlags.m_flag >> 1) & 1))
        return 1;
    return 0;
}