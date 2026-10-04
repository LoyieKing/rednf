bool CUser::unlockCreateCharac()
{
    if (!isLockedCreateCharac())  // ORIG 0x864aab6 xor$1;test;je 族
        return 0;
    m_lockedCreateCharac = 0;
    return 1;
}