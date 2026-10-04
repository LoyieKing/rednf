bool CUser::lockCreateCharac()
{
    if (isLockedCreateCharac())   // ORIG 0x864aa88 test %al;je 同 lock4DataLoad
        return 0;
    m_lockedCreateCharac = 1;
    return 1;
}