// W 0x0826528a：mov 0x38(%eax),%eax —— 读 cert 计数（+0x38）。
int CSecurityCard::getCertCnt()
{
    return m_certCnt38;
}