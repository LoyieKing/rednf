int PvP_GuildWar_Log::InsertPvPGuild(unsigned int guildKey, int point)
{
    // [b28c-L18] 真语义 bug 修复：ORIG 0x085dddcc `cmp 0xc(%ebp),%eax; jne <++i 块>`
    // 实证 key 既非 0 亦非 guildKey 时**继续扫描下一槽**；旧源 mismatch 直落
    // found 插入块（首个非命中槽即被覆盖，槽 1..7 永不可达）。
    // 块布局照 ORIG（P164 岛标签法）：入口 jmp 尾条件块；body/insert 物理前置、
    // `addl/cmpl/setle/test/jne` 回边块 + `mov $0` 直落 epilogue。
    int i = 0;
    goto cond;
body:
    if (m_guilds[i].m_guildKey == 0)
        goto found;
    if (m_guilds[i].m_guildKey == guildKey)
    {
        m_guilds[i].m_point += point;
        ++m_guilds[i].m_count;
        return 1;
    }
    ++i;
    goto insertblk;
found:
    m_guilds[i].m_guildKey = guildKey;
    m_guilds[i].m_point = point;
    m_guilds[i].m_count = 1;
    ++m_guildCnt;
    return 1;
insertblk:
cond:
    if (i <= 7)
        goto body;
    return 0;
