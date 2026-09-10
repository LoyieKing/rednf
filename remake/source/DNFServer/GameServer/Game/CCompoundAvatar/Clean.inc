void tagCompoundAvatar::Clean()
{
    m_idx = 0;
    std::memset(&m_a[0], 0, sizeof(m_a));
    for (int i = 0; i < 10; ++i)
    {
        m_optsA[i].clear();
        m_optsB[i].clear();
        m_b[i] = 0;
        m_c[i] = 0;
        m_d[i] = 0;
        m_optsC[i].clear();
    }
    m_tailA.clear();
    m_flag2dc = 0;
    for (int j = 0; j < 10; ++j)
    {
        m_e[j] = 0;
    }
    m_tailB.clear();
    m_tailC.clear();
