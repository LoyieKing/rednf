signed char CUserCharacInfo::get_charac_visible_values()
{
    return m_selected ? CUR->m_visibleValues : 0;
