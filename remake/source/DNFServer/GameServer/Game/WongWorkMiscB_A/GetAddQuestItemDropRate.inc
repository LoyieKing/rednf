int CUserPremium::GetAddQuestItemDropRate(int level)
{
    if (level < 0 || level > 0x69)
        return 0;
    return g_SPremiumInfo[level].m_field48;
