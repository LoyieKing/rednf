int CUserCharacInfo::get_charac_fatigue_buf_bonus_exp() const
{
    return m_selected ? CUR->m_fatigueBufBonusExp : 0;
}