// ORIG T 0x080e48ec：movzbl 0x5(%edx,%eax,1),%eax 直返（P9：返回 unsigned char，单条零扩展）。
unsigned char CCharacInformNotice::getInformNoticeFlag(int idx)
{
    return m_flags[idx];
}