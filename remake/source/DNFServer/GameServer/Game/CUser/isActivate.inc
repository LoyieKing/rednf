bool WongWork::CSecurityCard::isActivate() const
{
    return m_activate2f;   // +0x2f 激活标志（bool 直返：ORIG movzbl 直返）
