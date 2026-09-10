int CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE type)
{
    return m_premium.GetAddExpBonus(static_cast<int>(type));
