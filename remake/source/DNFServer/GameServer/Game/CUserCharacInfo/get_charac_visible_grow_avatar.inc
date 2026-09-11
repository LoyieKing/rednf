bool CUserCharacInfo::get_charac_visible_grow_avatar()
{
    if (m_selected && CUR->m_visibleBits.growAvatar)
        return 1;
    return 0;
}