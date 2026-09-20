void CUserCharacInfo::set_charac_member_bonus_exp(int exp)
{
    if (m_selected)
    {
    m_selected->m_fieldE2A = exp;
    }
}