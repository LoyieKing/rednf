bool CUser::unlockDeleteCharac()
{
    if (!isLockedDeleteCharac())  // ORIG 0x864aa48 xor$1;test;je 同族
        return 0;
    m_lockedDeleteCharac = 0;
    return 1;
}