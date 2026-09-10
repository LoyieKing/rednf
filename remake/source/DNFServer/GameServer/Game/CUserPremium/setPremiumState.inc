void CUserPremium::setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state)
{
    (m_entries + (int)type)->m_state = (int)state;
