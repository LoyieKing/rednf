void CUserCharacInfo::set_charac_visible_grow_avatar(bool flag)
{
    if (m_selected)
    {
        unsigned char f = m_selected->m_tagVisibleFlags.m_flag;
        m_selected->m_tagVisibleFlags.m_flag = (unsigned char)((f & ~2u) | ((flag ? 1u : 0u) << 1));
    }
}
