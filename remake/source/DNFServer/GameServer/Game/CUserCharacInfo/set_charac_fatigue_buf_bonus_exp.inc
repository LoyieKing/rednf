void CUserCharacInfo::set_charac_fatigue_buf_bonus_exp(int exp)
{
    if (m_selected)
    {
    m_selected->m_fieldE2E = exp;
    }
}