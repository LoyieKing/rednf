// [b28d-solo-GameWorld] 本轮 30+ 形穷举（register one=1u<<ct / one<<=ct / one 预清 /
// 字段 unsigned / 冗余 cast / while+break / for(;;) / 语句表达式 / 逗号 / const register /
// unsigned long ct / 双条件合并 / 提前 return）实测 4.4.6 恒将 ct 落 %ebx 或触发 N6 折叠 @0x080f131e
// [A11-round5 复核] 本族 5 件（IsIntegratedPvPBaseChannel @0x080f131e / IsPVPChannel
// @0x081424e8 / IsFreePvPChannel @0x081a0ab2 / IsWinPointPvPChannel @0x085dfa92 /
// IsSameTeamChannelJoin @0x082343c8）本轮以 8 源形探针（register ct+one / one 先声明 /
// 声明后赋值 / 表达式直读 / 内联移位 / uchar ct / const register / one 前置求值）在
// 4.4.6、4.4.7、4.4.4 三后端逐形复编（探针 /tmp/r5/probeC.cpp h1..h8）：ORIG 4.1 把 ct
// 留 %eax（0x082343cf `mov 0x20(%eax),%eax` → cmp → `mov %eax,%ecx`），4.4 三后端恒
// 落 %ebx，唯一净差 = `mov %eax,%ebx`(cmp 前) + `mov %ebx,%eax`(shl 前) 两条中转；
// 表达式直读形触发 N6 位折叠（shr/and $1）反劣。⇒ N96/N7 封顶，后继勿再试源形。
// [b28d-solo-GameWorld] _ZNK9GameWorld26IsIntegratedPvPBaseChannelEv verdict=DIFF ndiff=2
// N96/N7 封顶：ORIG 4.1 register ct 留 %eax（0x080f1322 装载一次供 cmp/shl 复用），
// 4.4.6 恒落 %ebx（多 mov %eax,%ebx + mov %ebx,%eax）；表达式直读形触发 N6 折叠
// （shr/and $1）反劣，register 形为最优可接受形。
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
    // [GW20Run 2026-09-11 第三轮复核] fresh .o 实测 21/23；逐条核 ORIG
    // 0x080f1322-0x080f1334：ct 全程 %eax（装载 0x080f1325→cmp 0x080f1328→
    // `mov %eax,%ecx` 0x080f1334），我方唯多 `mov %eax,%ebx`(cmp 前)/`mov %ebx,%eax`
    // (shl 前) 两条中转；掩码 $0x1e700 与上界 $0x10 与源码逐值一致（非位掩码笔误）
    // ⇒ 维持 N96/N7 封顶，勿再试源形。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x1e700)
            return true;
    }
    return false;
}