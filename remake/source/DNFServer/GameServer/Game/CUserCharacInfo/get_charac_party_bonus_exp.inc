int CUserCharacInfo::get_charac_party_bonus_exp() const
{
    return m_selected ? m_selected->m_partyBonusExp : 0;
}