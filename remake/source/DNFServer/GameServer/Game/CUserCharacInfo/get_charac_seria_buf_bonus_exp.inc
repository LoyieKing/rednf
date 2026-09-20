int CUserCharacInfo::get_charac_seria_buf_bonus_exp() const
{
    if (m_selected)
    {
        int value = m_selected->m_field127C;
        m_selected->m_field127C = 0;
        return value;
    }
    return 0;
}