// ORIG 0x0842ab78（_ZN26DB_CheckDoubleGuildAddress8dispatchEiiP6Stream）：
// 同 DB_CheckDoubleGuildName 构型：GetDBHandle(8,0)；GetOutBuffer<SIG_DOUBLE_
// CHECK_GUILD_ADDRESS>（guild_url 自 +0 起）；seLect * from guild_info where
// guild_url='%s'；exec 失败返 0；Acquire(0x6c9f) → <<0xec → <<userIdx →
// n_rows==0 ? '\x01' : '\x02' → put(DISPATCH_Q(1))；返 1。
bool DB_CheckDoubleGuildAddress::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_DOUBLE_CHECK_GUILD_ADDRESS* sig =
        stream->GetOutBuffer<SIG_DOUBLE_CHECK_GUILD_ADDRESS>();
    db->set_query("seLect * from guild_info where guild_url='%s'", sig->m_guildUrl);
    // 同 DB_CheckDoubleGuildName（ORIG 0x842ab78 同构）：`!exec` 直判 return 0
    // 岛 + body 显式块；第二 << 实参 = slot（0x10(%ebp)）。
    if (!db->exec(true))
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6c9f), true);
        guard.operator*()->operator<<((int)0xec);
        guard.operator*()->operator<<((int)slot);
        if (db->get_n_rows() == 0)
            guard.operator*()->operator<<((char)1);   // ORIG lsEc（char 版）
        else
            guard.operator*()->operator<<((char)2);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}