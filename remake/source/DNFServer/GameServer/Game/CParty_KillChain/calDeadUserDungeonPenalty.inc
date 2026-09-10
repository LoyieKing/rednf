float CParty::calDeadUserDungeonPenalty(int slot)
{
    float rate = 0.0f;
    // P0：ORIG 0x085ad9c2 在 slot 判定前即读入 memberCount 并占槽 -0x4。
    const int memberCount = m_battleData.m_field68c;
    if (slot < 0)
        return rate;

    if (memberCount > 1)
    {
        // P0/P183：ORIG 无 deadCnt 具名槽（0x085ad9eb 直接 fildl 临时槽）——
        // 表达式内联进 float 转换，禁具名 int 局部。
        rate = static_cast<float>(
                   reinterpret_cast<const KcDeadCountView*>(this)
                       ->m_deadCounts[slot + 0xe0])
             / static_cast<float>(memberCount - 1);
        if (rate == 0.3f)
            rate = 0.0f;
    }
    return rate;
