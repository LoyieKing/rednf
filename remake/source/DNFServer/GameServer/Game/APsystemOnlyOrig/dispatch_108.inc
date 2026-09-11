// ============================================================================
// DB_CancelJoinGuild —— ORIG dispatch 0x0843d6ea / makeRequest 0x0843d78c。
// 报告：docs/class_func_reports/DB_CancelJoinGuild/{dispatch,makeRequest}.md。
// dispatch：DB_HANDLE_8；Stream>>guildId>>characNo → deLete guild_join_list
// → 返回 exec(true)==1（!exec 早退形态）。
// ============================================================================
bool DB_CancelJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int guildId;
    unsigned int characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    guildId = 0;
    characNo = 0;
    *stream >> guildId;
    *stream >> characNo;
    db->set_query(
        "deLete from guild_join_list where guild_id=%d and charac_no=%d",
        guildId, characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}