// ORIG _ZN16PvP_GuildWar_Log14InsertPvPGuildEji 0x085ddd80：
//   for(i=0;i<=7;++i){ key==0→跳到插入；key==guildKey→累加返回 1 } 未命中返回 0。
// [b28d-23 N·P223 → b28d-M2S3 推翻] ORIG 0x085ddd80：goto 岛形 → 自然
// for + if/else 直落后复测 108/108、归一化零差异（identical）。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
int PvP_GuildWar_Log::InsertPvPGuild(unsigned int guildKey, int point)
{
    // [b28c-L18] 真语义 bug 修复：ORIG 0x085dddcc `cmp 0xc(%ebp),%eax; jne <++i 块>`
    // 实证 key 既非 0 亦非 guildKey 时**继续扫描下一槽**；旧源 mismatch 直落
    // found 插入块（首个非命中槽即被覆盖，槽 1..7 永不可达）。
    // b28d-M2S3（P164/P223 重试，推翻旧 N 判词）：ORIG 0x085ddd80 物理块序 =
    // body(+0x13) → insert 块(+0xb3) → ++i 块(+0x110) → 尾条件(+0x114)。
    // 旧 goto 岛形把 ++i 块插在 insert 之前，且入口 `jmp` 后多 1 条 label
    // 对齐 nop（109 vs ORIG 108）。自然 for + if/else 直落成 ORIG 块序。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_guilds[i].m_guildKey != 0)
        {
            if (m_guilds[i].m_guildKey == guildKey)
            {
                m_guilds[i].m_point += point;
                ++m_guilds[i].m_count;
                return 1;
            }
        }
        else
        {
            m_guilds[i].m_guildKey = guildKey;
            m_guilds[i].m_point = point;
            m_guilds[i].m_count = 1;
            ++m_guildCnt;
            return 1;
        }
    }
    return 0;
}