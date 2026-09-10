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
