// ---- DimensionInoutCount（T 0x0826d5ac）----
// 命中档位后把 levels[idx].m_value4 写入 result.m_value00，
// 并将该值计入 party 的次元进出计数。
void DimensionInoutCount::process(RewardData_Param param,
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
        result.m_value00 = it->second.m_levelDatas[idx].m_value4;
        param.m_party->increase_dimension_inout_count(result.m_value00);
    }
}