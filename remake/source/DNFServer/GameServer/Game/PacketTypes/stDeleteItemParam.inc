stDeleteItemParam::stDeleteItemParam()
    : m_4(0), m_8(0), m_9(0)
{  // ORIG 0x8230b38：i=0..100 清零三个数组
    for (int i = 0; i < 100; ++i)
    {
        m_slotsA[i] = 0;
        m_slotsD2[i] = 0;
        m_counts19c[i] = 0;
    }
