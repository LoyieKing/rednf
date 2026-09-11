// W 0x0822ef72：movzbl 0x31; cmp $0x1; jne L; mov $1; jmp; L: mov $0 —— 分支式 bool 返回。
bool CSecurityCard::isCertFlagOn()
{
    if (m_certFlag31 == 1)
        return true;
    return false;
}