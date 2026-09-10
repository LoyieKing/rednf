bool GameWorld::IsIntegratedPvPBaseChannel() const
{
    // ORIG _ZNK9GameWorld26IsIntegratedPvPBaseChannelEv @0x080f131e：
    // 0x080f1322/0x080f1325 装载 m_channelType 一次，0x080f1328 cmp、
    // 移位量复用同一 %eax。b28c-lane3：历史 register asm("eax")/asm("edx")
    // 绑定按 P0/N96 清除，改普通 register 局部——4.4 无栈槽且阻断
    // `(1<<t)&C`→`(C>>t)&1` 位折叠（N6），单次装载形态保持。
    // [G9WorldTimerLeague 2026-09-10 复核封顶] 同族 12 形探针 × 双后端实测：
    // 最优仍为现形 22 条 vs ORIG 21 条（ORIG 0x080f1325 装载一次、0x080f1328 cmp
    // 与 0x080f1334 `mov %eax,%ecx` 复用同一 %eax）⇒ N96/N7 封顶，勿再试。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x1e700)
            return true;
    }
    return false;
