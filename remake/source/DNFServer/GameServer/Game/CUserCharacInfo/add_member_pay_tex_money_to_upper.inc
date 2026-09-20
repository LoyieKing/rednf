void CUserCharacInfo::add_member_pay_tex_money_to_upper(int money)
{
    if (m_selected)
    {
    m_selected->m_fieldE33 += money;
    }
}