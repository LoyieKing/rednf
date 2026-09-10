unsigned int CUser::getAddIndependentDropRateFromPremium(ENUM_PREMIUM_TYPE type, unsigned int level)
{
    return m_premium.GetAddIndependentDropRate(type, level);
