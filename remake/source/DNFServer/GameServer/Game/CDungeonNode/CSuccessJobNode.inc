CSuccessJobNode::CSuccessJobNode()
    : m_jobPartyMap(), m_charac()
{
    // [b28d-solo-CDungeonNode · N104 封顶] ORIG 0x08484c30：循环体内保留原码的死比较
    // `find(job) != end()`（operator!= 结果被丢弃，无 test/je——P171 照抄）；
    // make_pair 第二参为右值 `JobParty&&`（mangled `IRi7JobPartyE...OS7_`）。
    // 残差 = N104：4.4 在 cleanup1（CharacterD1Ev 段）尾多发 1 条 `jmp <cleanup2>`，
    // ORIG 两 landing 块物理相邻、fall-through 直达（无 jmp）；块内容逐条全等、
    // 槽位集全等、p167 调用点差集 12/12 CLEAR。89 vs 90。
    // N104 为 4.4 的 EH landing 布局固有产物（throw() 法不适用：两侧 landing 均在）。
    for (int job = 2; job <= 4; ++job)
    {
        if (m_jobPartyMap.find(job) != m_jobPartyMap.end())
        {
        }
        m_jobPartyMap.insert(std::make_pair(job, JobParty()));
    }
