int CUserPremium::GetAddExpBonus(int type) const
{
    if (type < 0 || type > 0x69)
        return 0;
    return g_SPremiumInfo[type].m_field44;
