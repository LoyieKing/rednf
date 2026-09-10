int CUserCharacInfo::setPremiumBonusExp(unsigned int exp, int percent)
{
    if (m_selected)
    {
        CUR->m_premiumBonusExp = (int)(long long)((double)exp * (percent / 100.0f));
        return CUR->m_premiumBonusExp;
    }
    return 0;
