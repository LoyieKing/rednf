void CUserCharacInfo::set_charac_member_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_memberBonusExp = exp;
    }
