void CMTRand::reload()
{
    // [round3-L29] P231：ORIG 以 %esi 承载 kk（`mov $0xe3,%esi`），纯 register 复现。
    register int kk;
    for (kk = 0; kk < MT_N - MT_M; ++kk)
    {
        m_mt[kk] = twist(m_mt[kk + MT_M], m_mt[kk], m_mt[kk + 1]);
    }
    for (; kk < MT_N - 1; ++kk)
    {
        m_mt[kk] = twist(m_mt[kk + MT_M - MT_N], m_mt[kk], m_mt[kk + 1]);
    }
    m_mt[MT_N - 1] = twist(m_mt[MT_M - 1], m_mt[MT_N - 1], m_mt[0]);

    m_left = MT_N;
    m_next = m_mt;
