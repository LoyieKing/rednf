void CUserCharacInfo::set_charac_seria_buf_bonus_exp(int exp)
{
    if (m_selected)
    {
    m_selected->m_field127C = exp;
    }
}