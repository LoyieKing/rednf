void CUserCharacInfo::set_charac_visible_teleport_(bool flag)
{
    if (m_selected)
    {
        unsigned char f = m_selected->m_tagVisibleFlags.m_flag;
        m_selected->m_tagVisibleFlags.m_flag = (unsigned char)((f & ~4u) | ((flag ? 1u : 0u) << 2));
    }
}
