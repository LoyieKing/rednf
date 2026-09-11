void CUserCharacInfo::set_charac_party_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_partyBonusExp = exp;
    }
}