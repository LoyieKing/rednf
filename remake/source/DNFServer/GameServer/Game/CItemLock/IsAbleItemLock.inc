int CItemLock::IsAbleItemLock(unsigned char /*lock*/) const
{
    return m_lockStack.empty() ? 0x16 : 0;
