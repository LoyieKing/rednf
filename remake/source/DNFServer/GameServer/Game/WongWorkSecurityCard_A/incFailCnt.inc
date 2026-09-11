// W 0x082652ae：mov 0x34; lea 0x1; 写回; 再读返（++(+0x34) 后返回新值）。
int CSecurityCard::incFailCnt()
{
    m_failCnt34 = m_failCnt34 + 1;
    return m_failCnt34;
}