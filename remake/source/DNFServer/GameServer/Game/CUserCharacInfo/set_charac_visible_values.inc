void CUserCharacInfo::set_charac_visible_values(char values)
{
    if (m_selected)
    {
    m_selected->m_tagVisibleFlags.m_flag = (unsigned char)values;
    }
}