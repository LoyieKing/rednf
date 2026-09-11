// ============================================================================
// DB_DenyJoinGuild::dispatch —— ORIG @ 0x0843de6c（0xa1，最短）：
// GetDBHandle(DB_HANDLE_8, NONE) → *stream >> guildKey(uint) >>
// applicant(int) → set_query("deLete from guild_join_list where
// guild_id=%d and charac_no=%d", guildKey, applicant) → exec(true)。
// 失败返 0，成功返 1（无基类 dispatch 检查、无回包）。
// ============================================================================
bool DB_DenyJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    unsigned int guildKey = 0;
    unsigned int applicant = 0;
    *stream >> guildKey;
    *stream >> applicant;

    db->set_query(
        "deLete from guild_join_list where guild_id=%d and charac_no=%d",
        guildKey, applicant);
    if (!db->exec(true))
        return 0;
    return 1;
}