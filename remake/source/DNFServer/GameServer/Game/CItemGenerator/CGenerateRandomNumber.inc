CGenerateRandomNumber::CGenerateRandomNumber(int count)
    : m_count(count)
{
    if (count != 0)
    {
        m_table = new CMTRand[m_count];
        static int seedIncValue = 0;
        int i = 0;
        for (i = 0; i < m_count; ++i)
            m_table[i].seed(time(0) + seedIncValue + i);
        seedIncValue += i;
    }
