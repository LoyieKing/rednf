bool DB_SaveCharacInfo4Guild::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    SIG_SAVE_CHARACINFO4GUILD* sig =
        stream->GetOutBuffer<SIG_SAVE_CHARACINFO4GUILD>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    // ORIG +0x52 将第二次 GetDBHandle 结果物化到 -0xc 后不再读取（死槽 store）；
    // 字段读取逐次重读 sig(-0x14)+具名 disp（P86），不缓存视图局部。
    MySQL* dbGuild = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate guild_member set grow_type=%d,lev=%d where guild_id=%d and charac_no=%d",
        sig->m_growType,
        sig->m_lev,
        sig->m_guildId,
        sig->m_characNo);
    (void)dbGuild;
    if (!db->exec(true))
        return 0;
    return 1;
}