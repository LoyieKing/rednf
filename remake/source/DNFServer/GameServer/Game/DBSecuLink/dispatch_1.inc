// ============================================================================
// DB_CheckJoinGuild::dispatch —— ORIG @ 0x0843c98e（0x278）：
// GetDBHandle(DB_HANDLE_8, NONE) → GetOutBuffer<SIG_CHECK_JOIN_GUILD> →
// escape_string(buf, sig) → set_query("seLect guild_name,master_name,lev,
// member_count from guild_info where server_id=%d and expire_flag=0 and
// guild_name='%s'", *(int*)(sig+0x18), buf) → exec 失败返 0 →
// Acquire("DBThread.cpp",0x90e6) → guard << 0x216 << slot →
// GetInBuffer<SIG_CHECK_JOIN_GUILD_DB_REPLY>：无行 → m_result=0x15；
// 有行 → fetch 失败返 0；m_result=0 → get_str(0,guildName,0x17) /
// get_str(1,name,0x1e) / get_int(2,+0x3c) / get_int(3,+0x40)（任一失败返 0）
// → put(DISPATCH_Q) → 返 1。
// ============================================================================
bool DB_CheckJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_CHECK_JOIN_GUILD* sig = stream->GetOutBuffer<SIG_CHECK_JOIN_GUILD>();
    int unused = 0;              // ORIG -0x10 槽（dead local，P31）
    char nameBuf[0x2f];          // escape_string 中转（尺寸按 ORIG 帧反推）

    db->escape_string(nameBuf, sig->m_guildName);  // ORIG 传 sig（+0x00 即公会名区）→ 具名字段
    db->set_query(
        "seLect guild_name,master_name,lev,member_count from guild_info "
        "where server_id=%d and expire_flag=0 and guild_name='%s'",
        sig->m_serverId, nameBuf);
    if (!db->exec(true))
        return 0;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x90e6), true);
    guard.operator*()->operator<<(0x216);
    guard.operator*()->operator<<(slot);

    SIG_CHECK_JOIN_GUILD_DB_REPLY* reply =
        guard->GetInBuffer<SIG_CHECK_JOIN_GUILD_DB_REPLY>();
    if (db->get_n_rows() == 0)
    {
        reply->m_result = 0x15;
    }
    else
    {
        if (!db->fetch())
            return 0;
        reply->m_result = 0;
        if (!db->get_str(0, reply->m_guildName, 0x17))
            return 0;
        if (!db->get_str(1, reply->m_name, 0x1e))
            return 0;
        if (!db->get_int(2, reply->m_lev))
            return 0;
        if (!db->get_int(3, reply->m_memberCount))
            return 0;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}