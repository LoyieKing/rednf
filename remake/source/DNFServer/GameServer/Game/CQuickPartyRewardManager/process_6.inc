void QuestItemDropRate::process(RewardData_Param param,
                                RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    {
        float rate = 1.0f;
        rate = rate +
               static_cast<float>(it->second.m_levelDatas[idx].m_value4) /
                   100.0f;
        result.m_flag18 = static_cast<int>(
            static_cast<float>(result.m_flag18) * rate);
    }
