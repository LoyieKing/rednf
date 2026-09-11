// ORIG 0x0843a9c0：GetDBHandle(2,0) → GetDBHandle(8,0) → *stream >>
// characNo(uint) >> guildId(uint)；任一为 0 直接返 1。charac db(2) 清
// charac_info.guild_right/guild_id，guild db(8) 置 expire_flag=1 并依次
// delete guild_introduce/member/visit/notice/skill，最后 charac db(2)
// 置 guild_id=0/guild_secede=1。任一 exec 失败返 0，全成返 1。
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
}