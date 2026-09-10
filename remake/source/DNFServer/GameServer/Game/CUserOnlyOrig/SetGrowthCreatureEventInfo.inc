void CUser::SetGrowthCreatureEventInfo(const SIG_GIVE_GROWTH_CREATURE_EVENT& sig)
{
    // SetGrowthCreatureEventInfo.md @0x08691cde：stride 0x2c×11 循环，
    // serverGroup/characNo 任一为 0 停止；构造 eventCharacInfo 入 m_growthCreature.m_vec(+0x8ebf8)。
    for (int i = 0; i < 11; ++i)
    {
        // [b28d-22 修法 P0/P30] ORIG 0x08691cf0 起每字段 `mov -0xc,%edx; mov 0xc(%ebp),%eax;
        // imul $0x2c,%edx,%edx; movzwl (%edx,%eax,1),%eax`（无中间引用/指针槽）⇒ 删具名
        // src 引用，逐字段 sig.m_entries[i] 下标直写。
        if (sig.m_entries[i].m_serverGroup == 0 || sig.m_entries[i].m_characNo == 0)
            break;
        GrowthCreatureEvent::eventCharacInfo info;
        info.m_serverGroup = sig.m_entries[i].m_serverGroup;
        info.m_characNo = sig.m_entries[i].m_characNo;
        info.m_job = sig.m_entries[i].m_job;
        memcpy(info.m_name, sig.m_entries[i].m_name, 0x1e);
        m_growthCreature.m_vec.push_back(info);
    }
