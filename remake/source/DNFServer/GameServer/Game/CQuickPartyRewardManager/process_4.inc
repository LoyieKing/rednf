// ---- CointLimitCount（T 0x0826d4ac）----
// result.m_value00 -= levels[idx].m_value4；差额经 CParty 计入地下城硬币计数。
void CointLimitCount::process(RewardData_Param param, RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    // b28d-20: ORIG 0x0826d4ac — 三早退岛；sz 存 -0x10、idx 存 -0xc（ORIG 槽序）；
    // `result.m_value00 -= it->second.m_levelDatas[idx].m_value4` 单语句触发 4.1
    // 旧值先入 %esi + idx 入 %ebx 跨 operator->/operator[] 保活（P38/SAVE_EXPR）。
    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    result.m_value00 -= it->second.m_levelDatas[idx].m_value4;
    param.m_party->increase_dungeon_coin_count(result.m_value00);
}