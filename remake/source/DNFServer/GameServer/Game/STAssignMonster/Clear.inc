void STAssignMonster::Clear()
{
    m_0 = -1;
    m_4 = 1;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
    m_14 = 0;
    m_pair18.m_pair.m_hi = 1;
    m_pair18.m_pair.m_lo = m_pair18.m_pair.m_hi;
    m_20 = 0;
    m_24 = 0;
    m_vec28.clear();
    m_50 = 0;
    m_4c = 100;
    m_54 = 0;
    m_55 = 0;
    m_58 = -1;
    for (int i = 0; i <= 1; ++i)   // ORIG 0x836c5a5 scaled 循环（setle 实证 i<=1）
        m_pair38.m_arr[i] = 0;
    m_34 = 0;
    m_vec5c.clear();
