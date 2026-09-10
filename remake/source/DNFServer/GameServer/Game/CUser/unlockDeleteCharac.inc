bool CUser::unlockDeleteCharac()
{
    if (!isLockedDeleteCharac())  // ORIG 0x864aa48 xor$1;test;je 同族
        return 0;
    m_field8d0f1 = 0;
    return 1;
