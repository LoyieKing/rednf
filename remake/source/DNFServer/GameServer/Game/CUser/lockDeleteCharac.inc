bool CUser::lockDeleteCharac()
{
    if (isLockedDeleteCharac())   // ORIG 0x864aa1a test %al;je 同族
        return 0;
    m_field8d0f1 = 1;
    return 1;
