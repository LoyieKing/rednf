void CUser::backupSeed()
{
    m_backupSeed = m_checkSumSeed;  // ORIG 0x859506f: *(this+0x796e4) = *(this+0x796e0)
