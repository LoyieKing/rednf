// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x081a0ab2
// [b28d-solo-GameWorld] _ZNK9GameWorld16IsFreePvPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x081a0ab2，余 2 regmove）。
bool GameWorld::IsFreePvPChannel() const
{
    // ORIG _ZNK9GameWorld16IsFreePvPChannelEv @0x081a0ab2（同 0x081424e8 族布局）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    // [G9WorldTimerLeague 2026-09-10 复核封顶] 同族 12 形探针 × 双后端（4.4.7 /
    // cc1plus446bin 4.4.6，-march=i486 -mtune=generic）实测：最优仍为现形 22 条
    // vs ORIG 21 条（ORIG 0x081a0abc `cmp $0x10,%eax` 与 0x081a0ac8 `mov %eax,%ecx`
    // 复用同一 %eax，ct 全程不落 callee-saved）⇒ N96/N7+后端选择差，非源形可控。
    // [GW20Run 2026-09-11 第三轮复核] fresh .o 21/23；ORIG 0x081a0ab6-0x081a0aca 逐条
    // 核对（ct 全程 %eax；掩码 $0x166a0、上界 $0x10 与源码逐值一致）⇒ 维持封顶。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x166a0)
            return true;
    }
    return false;
}