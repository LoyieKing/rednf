bool CTimeGate::isOpen() const
{
    return m_openState == 0x64;   // ORIG: movzbl +0x04; cmp $0x64; sete
