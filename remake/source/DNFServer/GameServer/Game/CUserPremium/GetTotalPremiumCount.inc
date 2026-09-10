int CUserPremium::GetTotalPremiumCount() const
{
    return m_normalCount
         + m_restrictedCount;
