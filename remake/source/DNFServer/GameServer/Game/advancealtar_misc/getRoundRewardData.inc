std::vector<_RewardItem>* SurvivalRewardData::getRoundRewardData(int round) const
{
    // b28c-L5：ORIG 0x0889aebe 增量段 `ppEi`（后置 ++，sret 临时 -0xc；
    //   it=-0x14 / end=-0x10）⇒ 改后置形复原三槽阶梯（同 getMaxLevel 判据）。
    for (std::vector<RoundRewardData>::const_iterator it = m_vec.begin();
         it != m_vec.end(); it++)
    {
        if (!((round < it->m_0) || (it->m_4 < round)))
        {
            return const_cast<std::vector<_RewardItem>*>(&it->m_vec8);
        }
    }
    return 0;
