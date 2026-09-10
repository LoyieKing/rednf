bool DB_JoinGuildInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 序：GetDBHandle 初始化语句在局部零初始化之前（gcc O0 按声明序）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    int serverGroup = 0;
    unsigned int characNo = 0;
    DB_JoinGuildInfoView* sig;

    *stream >> serverGroup;
    *stream >> characNo;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x91bd), true);
    guard.operator*()->operator<<((int)0x219);
    guard.operator*()->operator<<(slot);
    sig = reinterpret_cast<DB_JoinGuildInfoView*>(
        guard.operator->()->GetInBuffer<SIG_JOIN_GUILD_INFO>());
    db->set_query("seLect guild_id,memo from guild_join_list where charac_no=%d and server_group=%d",
                  characNo, serverGroup);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
    {
        sig->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (db->fetch() != 1)
        return 0;
    if (db->get_uint(0, sig->m_guildId) != 1)
        return 0;
    if (db->get_str(1, sig->m_memo, 0x1a) != 1)
        return 0;
    db->set_query("seLect guild_name,master_name,lev,member_count from guild_info where expire_flag=0 and guild_id=%d",
                  sig->m_guildId);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
        return 1;
    if (db->fetch() != 1)
        return 0;
    if (db->get_str(0, sig->m_guildName, 0x17) != 1)
        return 0;
    if (db->get_str(1, sig->m_masterName, 0x1e) != 1)
        return 0;
    if (db->get_int(2, sig->m_lev) != 1)
        return 0;
    if (db->get_int(3, sig->m_memberCount) != 1)
        return 0;
        sig->m_flag = 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
