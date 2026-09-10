QuickPartyData::QuickPartyData()
{
    m_vec00.clear();
    m_dungeonIdxList.clear();
    m_randomRewards.clear();

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            m_minCounts[i][j] = -1;
