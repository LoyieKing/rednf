bool CEventBase::IsEventing(CUser*) const
{
    // ORIG 0x080c84fa：movzbl 直返 +0x08 的 flag 字节。
    return m_isEventing;
