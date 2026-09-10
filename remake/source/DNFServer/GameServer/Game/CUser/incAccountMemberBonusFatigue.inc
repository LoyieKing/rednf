void CUser::incAccountMemberBonusFatigue(short value)
{
    m_postChecksumData.m_field62 = (short)(m_postChecksumData.m_field62 + value);
