// ORIG T 0x080e4ab8：memset(this+5, 0, 0x10)
void CCharacInformNotice::_reset()
{
    memset(m_flags, 0, sizeof(m_flags));
}