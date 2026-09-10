bool CCharacInformNotice::checkInformNoticeFlag(unsigned int bit, unsigned int byteIdx)
{
    if (bit > 7)
        return false;
    if (byteIdx >= InformNotice::kInformNoticeIndex)
        return false;
    // [b28c-L15 P0/N96] 存量 register asm("eax") 绑定已清除（全域禁绑，N96）。
    unsigned char v = m_flags[byteIdx];
    if (!(((int)v) >> bit & 1))
        return true;
    return false;
