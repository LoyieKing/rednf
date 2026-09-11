void CUserCharacInfo::set_charac_fatigue_buf_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_fatigueBufBonusExp = exp;
    }
}