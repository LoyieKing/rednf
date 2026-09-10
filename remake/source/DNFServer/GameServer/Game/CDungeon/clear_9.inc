void advancealtar::ClearRewardData::clear()
{
    // ORIG：vector<_RewardItem> m_vec[3] 数组循环 clear（i=0..2）。
    for (int i = 0; i < 3; ++i)
        m_vec[i].clear();
