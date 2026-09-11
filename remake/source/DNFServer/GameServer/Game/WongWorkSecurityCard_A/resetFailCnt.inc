// W 0x082652c8：movl $0x0,0x34(%eax)。
void CSecurityCard::resetFailCnt()
{
    m_failCnt34 = 0;
}