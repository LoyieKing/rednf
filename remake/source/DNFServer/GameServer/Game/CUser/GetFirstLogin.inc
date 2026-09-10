unsigned char CUser::GetFirstLogin()
{
    return m_postChecksumData.m_firstLogin;   // ORIG movzbl 直返（成员 bool）
