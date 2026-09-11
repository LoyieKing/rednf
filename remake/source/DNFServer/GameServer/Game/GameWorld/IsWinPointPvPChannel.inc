// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x085dfa92
// [b28d-solo-GameWorld] _ZNK9GameWorld20IsWinPointPvPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x085dfa92，余 2 regmove）。
bool GameWorld::IsWinPointPvPChannel() const
{
    // ORIG _ZNK9GameWorld20IsWinPointPvPChannelEv @0x085dfa92（同族，上界 0xf）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    // [G9WorldTimerLeague 2026-09-10 复核封顶] 同族 12 形探针 × 双后端实测：
    // 最优仍为现形 22 条 vs ORIG 21 条（ORIG 0x085dfa9c `cmp $0xf,%eax` 与
    // 0x085dfaa8 `mov %eax,%ecx` 复用同一 %eax）⇒ N96/N7 封顶，后继勿再试。
    // [GW20Run 2026-09-11 第三轮复核] fresh .o 21/23；ORIG 0x085dfa9c-0x085dfaaa 逐条
    // 核对（ct 全程 %eax；掩码 $0x8110、上界 $0xf 与源码一致）⇒ 维持封顶。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x8110)
            return true;
    }
    return false;
}