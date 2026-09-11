// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x082343c8
// [b28d-solo-GameWorld] _ZN9GameWorld21IsSameTeamChannelJoinEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x082343c8，余 2 regmove）。
bool GameWorld::IsSameTeamChannelJoin()
{
    // ORIG _ZN9GameWorld21IsSameTeamChannelJoinEv @0x082343c8（同族，非 const）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    // [G9WorldTimerLeague 2026-09-10 复核封顶] 同族 12 形探针 × 双后端（4.4.7 /
    // cc1plus446bin 4.4.6，-march=i486 -mtune=generic）实测：最优仍为现形 22 条
    // vs ORIG 21 条（ORIG 0x082343d2 `cmp $0xf,%eax` 与 0x082343de `mov %eax,%ecx`
    // 复用同一 %eax）⇒ N96/N7 封顶，后继勿再试。
    // [GW20Run 2026-09-11 第三轮复核] fresh .o 21/23；ORIG 0x082343cf-0x082343e0 逐条
    // 核对（ct 全程 %eax；掩码 $0xc200、上界 $0xf 与源码一致）⇒ 维持封顶。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0xc200)
            return true;
    }
    return false;
}