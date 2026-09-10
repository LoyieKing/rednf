bool CUser::IsPremiumUser() const
{
    return m_premium.GetTotalPremiumCount() != 0;
