WongWork::CItemUpgrade_Separate::CItemUpgrade_Separate()
{
    m_randRange = 100000;
    unsigned long range;
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        range = m_randRange;
        m_rand->randInt(range);
    }
