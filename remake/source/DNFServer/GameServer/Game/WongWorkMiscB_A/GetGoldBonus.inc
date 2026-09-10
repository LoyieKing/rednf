int CUserPremium::GetGoldBonus(int level)
{
    if (m_field8bc != 0)
        return level * level * 100;
    return 0;
