signed char CUserCharacInfo::get_charac_visible_values()
{
    return m_selected ? (signed char)m_selected->m_tagVisibleFlags.m_flag : 0;
}
