void CUserCharacInfo::set_charac_visible_values(char values)
{
    if (m_selected)
    {
    CUR->m_visibleValues = (unsigned char)values;
    }
