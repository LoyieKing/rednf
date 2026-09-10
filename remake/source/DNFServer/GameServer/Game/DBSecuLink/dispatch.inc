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
