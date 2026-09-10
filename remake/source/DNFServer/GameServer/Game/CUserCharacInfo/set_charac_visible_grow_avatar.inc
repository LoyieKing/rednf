void CUserCharacInfo::set_charac_visible_grow_avatar(bool flag)
{
    if (m_selected)
    {
        CUR->m_visibleBits.growAvatar = flag;
    }
