bool DB_RequestJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db;
    // [Lane31W8LTDBTaskView] 语句序照 ORIG（0x0843ccf2）：GetDBHandle 先于
    // StreamPool::Acquire/CStreamGuard 构造（ORIG `call GetDBHandle; mov 0x940bd6c,%eax;
    // … call Acquire; call CStreamGuard::C1` 顺序实证）。
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9129), true);
    DB_RequestJoinGuildReplyView* reply;
    DB_RequestJoinGuildInView* sig;
    int result = 0;
    char escBuf[53];

    guard.operator*()->operator<<((int)0x217);
    guard.operator*()->operator<<(slot);
    reply = reinterpret_cast<DB_RequestJoinGuildReplyView*>(
        guard.operator->()->GetInBuffer<SIG_REQUEST_JOIN_GUILD_DB_REPLY>());
    sig = reinterpret_cast<DB_RequestJoinGuildInView*>(
        stream->GetOutBuffer<SIG_REQUEST_JOIN_GUILD>());
    db->set_query("seLect * from guild_join_list where charac_no=%d and server_group=%d",
                  sig->m_characNo, sig->m_serverGroup);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
    {
        db->set_query("seLect guild_id,guild_name,master_name,lev,member_count from guild_info where server_id=%d and expire_flag=0 and guild_name='%s'",
                      sig->m_serverGroup, sig->m_guildName);
        if (db->exec(true) != 1)
            return 0;
        if (db->get_n_rows() == 0)
        {
            reply->m_flag = 0x15;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        if (db->fetch() != 1)
            return 0;
        if (db->get_uint(0, reply->m_guildId) != 1)
            return 0;
        if (db->get_str(1, reply->m_guildName, 0x17) != 1)
            return 0;
        if (db->get_str(2, reply->m_masterName, 0x1e) != 1)
            return 0;
        if (db->get_int(3, reply->m_lev) != 1)
            return 0;
        if (db->get_int(4, reply->m_memberCount) != 1)
            return 0;
        db->set_query("seLect * from guild_join_list where guild_id=%d",
                      reply->m_guildId);
        if (db->exec(true) != 1)
            return 0;
        // [A04-r5h · 轴 B/P196] ORIG 0x0843d026-0x0843d02e 为
        // `call get_n_rows; cmp $0x31,%eax; **seta** %al; test %al,%al; je 0x843d05f`
        // ⇒ 真形 = **正极性无符号** `get_n_rows() > 0x31`（then=0x5f 块落 fall-through、
        // body 在跳转目标 0x843d05f）。原 `get_n_rows() < 0x32`（负极性 + signed）使 4.4
        // 产 `setle` 且块序反 ⇒ 按 ORIG 改正极性比较（ORIG 为无符号序 ⇒ P196 前置通过）。
        if (db->get_n_rows() > 0x31)
        {
            reply->m_flag = 0x5f;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        db->escape_string(escBuf, sig->m_memo);
        db->set_query("inSert into guild_join_list(guild_id,m_id,server_group,charac_no,born_year,memo,occ_time) values(%d,%s,%d,%d,'%s','%s',now())",
                      reply->m_guildId, NumberToString(sig->m_mId, 0),
                      sig->m_serverGroup, sig->m_characNo,
                      sig->m_bornYear, escBuf);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            reply->m_flag = 2;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        reply->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    reply->m_flag = 0x12;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 0;
