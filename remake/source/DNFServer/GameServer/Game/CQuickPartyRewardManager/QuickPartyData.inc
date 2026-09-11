// ORIG 0x826dad4：两 vector<int> C1、BasicReward C1、map C1、三容器 clear、
// +0x1a0[2][2] 全 -1。容器构造由成员默认构造承担；clear 为 ORIG 显式调用序列。
QuickPartyData::QuickPartyData()
{
    m_vec00.clear();
    m_dungeonIdxList.clear();
    m_randomRewards.clear();

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            m_minCounts[i][j] = -1;
}