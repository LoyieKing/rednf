float CHARAC_LOAD_MERCENARY::GetRewardCritical(int dice) const
{
    if (dice < 0 || G_CDataManager()->m_mercenarySystem.m_field3c < dice)
        return 0.0f;
    // 引用局部槽形：ORIG `call; add $0xa90c,%eax; mov %eax,-0xc` + 传参三步
    // （探针 /tmp/probe_l8h3.o 实证 30 条逐条全中，含大偏移 add 形）。
    const std::vector<std::pair<int, float> >& probs = G_CDataManager()->m_mercenarySystem.m_vec30;
    return FindRelativeProb<float>(probs, dice);
