bool DB_GuildJoinList::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db1 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                   E_SERVER_GROUP_NONE);
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                   E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9243), true);
    guard.operator*()->operator<<(0x21a);
    guard.operator*()->operator<<(slot);

    unsigned int guildKey = 0;
    *stream >> guildKey;
    int applicant = 0;
    *stream >> applicant;

    db1->set_query(
        "delete from guild_join_list where guild_id=%d and server_group != %d",
        guildKey, applicant);
    if (!db1->exec(true))
        return 0;
    db1->set_query(
        "seLect charac_no,memo,unix_timestamp(occ_time) from guild_join_list "
        "where guild_id=%d",
        guildKey);
    if (!db1->exec(true))
        return 0;

    SIG_GUILD_JOIN_LIST* reply =
        guard->GetInBuffer<SIG_GUILD_JOIN_LIST>();
    int occ = 0;                 // ORIG -0x38（声明于 time 前）
    // b28d-54：ORIG 0x0843dc8c 段 `mov -0x38(occ),%edx; mov -0x18(now),%ecx;
    //   sub %edx,%ebx` 先载 occ（P211 异型侧）；改 now 为 int 与 occ 同型试翻。
    int now = static_cast<int>(time(0));
    int nRows;
    nRows = (unsigned int)db1->get_n_rows() > 0x32
                ? 0x32
                : db1->get_n_rows();
    reply->m_list.m_count = nRows;

    for (int i = 0; i < nRows; ++i)
    {
        if (!db1->fetch())
            return 0;
        if (!db1->get_uint(0, reply->m_list.m_entry[i].m_characNo))
            return 0;
        if (!db1->get_str(1, &reply->m_list.m_entry[i].m_memo[0], 0x1a))
            return 0;
        if (!db1->get_int(2, occ))
            return 0;
        reply->m_list.m_entry[i].m_remainTime = now - occ;
    }

    for (int j = 0; j < nRows; ++j)
    {
        db2->set_query(
            "seLect charac_name,lev,job,grow_type,sex from charac_info "
            "where charac_no=%d",
            reply->m_list.m_entry[j].m_characNo);
        if (!db2->exec(true))
            return 0;
        if (db2->get_n_rows() == 0)
        {
            // ORIG：无行直接跳过（sete/jne 形态）
        }
        else
        {
            if (!db2->fetch())
                return 0;
            if (!db2->get_str(0, &reply->m_list.m_entry[j].m_name[0], 0x1e))
                return 0;
            if (!db2->get_byte(1, reply->m_list.m_entry[j].m_lev))
                return 0;
            if (!db2->get_byte(2, reply->m_list.m_entry[j].m_job))
                return 0;
            if (!db2->get_byte(3, reply->m_list.m_entry[j].m_growType))
                return 0;
            if (!db2->get_byte(4, reply->m_list.m_entry[j].m_sex))
                return 0;
        }
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
