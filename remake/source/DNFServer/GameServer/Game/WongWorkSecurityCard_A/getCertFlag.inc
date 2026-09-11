// W 0x086959b8：movzbl 0x31(%eax),%eax —— 直返 +0x31 字节（P9：char 直返无规范化）。
char CSecurityCard::getCertFlag()
{
    return m_certFlag31;
}