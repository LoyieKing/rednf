bool WongWork::CSecurityCard::isCertified() const
{
    return m_certified30;  // +0x30 认证标志（bool 直返：ORIG movzbl 直返）
