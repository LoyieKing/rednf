// ORIG 0x843d286：GetDBHandle(8,0) → >> server_group(int) → >> charac_no(uint)
// → Acquire("DBThread.cpp",0x91bd) → CStreamGuard(stream,true) → <<0x219 →
// <<slot → sig=GetInBuffer<SIG_JOIN_GUILD_INFO>() → 查 guild_join_list：
// exec 失败返 0；get_n_rows()==0 → sig flag=0 → put(DISPATCH_Q) 返 1；
// 有行 → fetch + guild_id(0)/memo(1,0x1a) → 查 guild_info（guild_id）：
// exec 失败/无行返 1（不回包）；有行 → fetch + guild_name(0,0x17)/
// master_name(1,0x1e)/lev(2)/member_count(3) → sig flag=1 → put(DISPATCH_Q) 返 1。
bool DB_JoinGuildInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 序：GetDBHandle 初始化语句在局部零初始化之前（gcc O0 按声明序）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    int serverGroup = 0;
    unsigned int characNo = 0;
    SIG_JOIN_GUILD_INFO* sig;

    *stream >> serverGroup;
    *stream >> characNo;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x91bd), true);
    guard.operator*()->operator<<((int)0x219);
    guard.operator*()->operator<<(slot);
    sig = /*SIG_JOIN_GUILD_INFO*/(
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
}