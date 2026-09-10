bool CUser::isRestingUser()
{
    return m_mcap.m_field544;   // ORIG movzbl 直返（无 test/setne）
