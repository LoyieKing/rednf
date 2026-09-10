int AvatarFixedHiddenOptionServer::GetRandomInput(ENUM_RARITY rarity)
{
    if (rarity >= 6)
        return -1;
    if (m_cumulativeRate[rarity].size() == 0)
        return -1;
    return m_cumulativeRate[rarity].back();
