int CUser::getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE type)
{
    return m_premium.GetAddQuestItemDropRate(static_cast<int>(type));
