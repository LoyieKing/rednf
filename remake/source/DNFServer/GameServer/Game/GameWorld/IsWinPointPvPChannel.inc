bool GameWorld::IsWinPointPvPChannel() const
{
    // ORIG _ZNK9GameWorld20IsWinPointPvPChannelEv @0x085dfa92（同族，上界 0xf）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    // [G9WorldTimerLeague 2026-09-10 复核封顶] 同族 12 形探针 × 双后端实测：
    // 最优仍为现形 22 条 vs ORIG 21 条（ORIG 0x085dfa9c `cmp $0xf,%eax` 与
    // 0x085dfaa8 `mov %eax,%ecx` 复用同一 %eax）⇒ N96/N7 封顶，后继勿再试。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x8110)
            return true;
    }
    return false;
