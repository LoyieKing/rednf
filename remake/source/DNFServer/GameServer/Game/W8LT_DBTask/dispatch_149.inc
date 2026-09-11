// ============================================================================
// DB_GuildExpBookDelete::dispatch —— ORIG 0x0843c35a（报告 dispatch.md 全文）
// ============================================================================
// 解包 characNo(uint)/guildSecede(int)/serverGroup(int) → GetDBHandle(2,
// serverGroup) → upDate charac_info set guild_secede=%d where charac_no=%u →
// exec(true)（结果忽略）→ 恒返 1（ORIG mov $0x1,%eax）。
bool DB_GuildExpBookDelete::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    MySQL* db;                  // -0xc
    unsigned int characNo = 0;  // -0x10
    int guildSecede = 0;        // -0x14
    int serverGroup = 0;        // -0x18
    stream->operator>>(characNo);
    stream->operator>>(guildSecede);
    stream->operator>>(serverGroup);
    db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)serverGroup);
    db->set_query(
        "upDate charac_info set guild_secede = %d where charac_no = %u",
        guildSecede, characNo);
    db->exec(true);
    return 1;
}