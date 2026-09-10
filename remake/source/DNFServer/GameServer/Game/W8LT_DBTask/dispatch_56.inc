bool DB_DeleteGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* characDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    unsigned int guildId = 0;
    *stream >> characNo;
    *stream >> guildId;
    if (characNo == 0 || guildId == 0)
        return 1;
    characDb->set_query(
        "upDate charac_info set guild_right=0, guild_id=0 where charac_no=%d", characNo);
    if (!characDb->exec(true))
        return 0;
    guildDb->set_query(
        "upDate guild_info set expire_flag=1, expire_time=now() where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_introduce where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_member where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_visit where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_notice where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_skill where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    characDb->set_query(
        "upDate charac_info set guild_id=0, guild_secede=1 where guild_id = %u", guildId);
    if (!characDb->exec(true))
        return 0;
    return 1;
