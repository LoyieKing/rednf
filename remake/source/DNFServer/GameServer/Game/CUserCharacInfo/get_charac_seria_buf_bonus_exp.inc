int CUserCharacInfo::get_charac_seria_buf_bonus_exp() const
{
    if (m_selected)
    {
        int value = CUR->m_seriaBufBonusExp;
        CUR->m_seriaBufBonusExp = 0;
        return value;
    }
    return 0;
