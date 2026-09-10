void RewardCardGold::process(RewardData_Param param, RewardData_Result& result)
{
    // b28d-20: ORIG 0x0826d070 — IBasicReward 族无 map find，直接
    // m_data.m_basicReward.m_levelDatas（+0x18）；两早退岛（js idx<0 / jge idx>=sz）
    // 由裸 if-return 直产（P223 变体）；size()/operator[] 每次重走
    // m_data.m_basicReward.m_levelDatas 表达式（P183 禁 vector 引用缓存，ORIG 每处
    // `mov this,%edx; mov 0x4(%edx),%edx; add $0x18,%edx`）；比较为**有符号** jge
    //（ORIG 源码是 int sz 而非 size_t 比较）。rate 为具名 float 局部（槽 -0xc，
    // ORIG `mov $0x3f800000,%eax; mov %eax,-0xc` 实证，两语句赋值保槽）。
    int sz = static_cast<int>(m_data.m_basicReward.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;

    float rate = 1.0f;
    rate = rate +
        static_cast<float>(m_data.m_basicReward.m_levelDatas[idx].m_value4) / 100.0f;
    result.m_value00 = static_cast<int>(
        static_cast<float>(result.m_value00) * rate);
