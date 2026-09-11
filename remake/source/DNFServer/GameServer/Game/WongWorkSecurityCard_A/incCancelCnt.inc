// W 0x0822ef32：mov 0x48; lea 0x1; 写回; 再读返（++(+0x48) 后返回新值）。
int CSecurityCard::incCancelCnt()
{
    m_cancelCnt48 = m_cancelCnt48 + 1;
    return m_cancelCnt48;
}