// ============================================================================
// DB_DeleteCharac::DeleteGuildInfo @0x08402f4c
// guildId==0 → charac_info 查 guild_id 入 +0xc（失败/空 +0x8=1 返 0；
// 查到仍 0 → logFormat 0x1312 返 1）。guild_info 查 master_no：
// 失败 +0x8=1；characNo==master → +0x8=2 返 0；否则 guild_member 退会
// （member_flag=2/secede），count(*) 更新 guild_info.member_count。
// ============================================================================
bool DB_DeleteCharac::DeleteGuildInfo(SIG_DELETE_CHARAC* sig)
{
    if (sig->m_guildId == 0)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                      (ENUM_SERVER_GROUP)0);
        db->set_query("seLect guild_id from charac_info where charac_no = %d",
                      sig->m_characNo);
        if (!db->exec(true))
        {
            sig->m_errNo = 1;
            return 0;
        }
        if (!db->fetch() || !db->get_uint(0, sig->m_guildId))
        {
            sig->m_errNo = 1;
            return 0;
        }
        if (sig->m_guildId == 0)
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0x1312, "[Guild Del Error] guild_id=0, charac_no:%d",
                sig->m_characNo);
            return 1;
        }
    }

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)8,
                                                   (ENUM_SERVER_GROUP)0);
    db2->set_query("seLect master_no from guild_info where guild_id = %d",
                   sig->m_guildId);
    if (!db2->exec(true))
    {
        sig->m_errNo = 1;
        return 0;
    }
    unsigned int masterNo = 0;
    if (!db2->fetch() || !db2->get_uint(0, masterNo))
    {
        sig->m_errNo = 1;
        return 0;
    }
    if (sig->m_characNo == masterNo)
    {
        sig->m_errNo = 2;
        return 0;
    }
    db2->set_query("upDate guild_member set member_flag = 2, secede_time = "
                   "now(), secede_type = 2 where guild_id = %d and "
                   "charac_no = %d", sig->m_guildId, sig->m_characNo);
    if (!db2->exec(true))
    {
        sig->m_errNo = 1;
        return 0;
    }
    db2->set_query("seLect count(*) from guild_member where guild_id = %d and "
                   "member_flag = 1", sig->m_guildId);
    if (!db2->exec(true))
    {
        sig->m_errNo = 1;
        return 0;
    }
    int count;
    if (!db2->fetch() || !db2->get_int(0, count))
    {
        sig->m_errNo = 1;
        return 0;
    }
    if (count != 0)
    {
        db2->set_query("upDate guild_info set member_count = %d where "
                       "guild_id = %d", count, sig->m_guildId);
        if (!db2->exec(true))
        {
            sig->m_errNo = 1;
            return 0;
        }
    }
    return 1;
}
