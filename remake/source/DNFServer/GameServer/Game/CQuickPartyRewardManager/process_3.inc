// ---- EquipItemDropRate（T 0x0826d2aa）----
// 命中后直接写 result.m_rate14 浮点比率（无舍入）。
void EquipItemDropRate::process(RewardData_Param param, RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    // b28d-20: ORIG 0x0826d2aa — 三早退岛；rate 具名 float 槽（ORIG -0xc
    // `mov $0x3f800000,%eax; mov %eax,-0xc`），尾段 `mov -0xc(%ebp),%edx;
    // mov %edx,0x14(%eax)` 直存 result.m_rate14（非表达式直写，否则 fld1 无槽）。
    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    float rate = 1.0f;
    rate = rate +
        static_cast<float>(it->second.m_levelDatas[idx].m_value4) / 100.0f;
    result.m_rate14 = rate;
}