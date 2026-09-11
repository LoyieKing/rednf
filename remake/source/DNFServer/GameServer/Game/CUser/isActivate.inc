// ---- CSecurityCard 状态 getter（ORIG W 0x0822ef0e/0x0822ef1a/0x0822ef26，
//      objdump 忠实转写；声明补于 CUser.h WongWork::CSecurityCard）----
bool WongWork::CSecurityCard::isActivate() const
{
    return m_activate2f;   // +0x2f 激活标志（bool 直返：ORIG movzbl 直返）
}