bool DB_CheckDoubleGuildAddress::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_DOUBLE_CHECK_GUILD_ADDRESS* sig =
        stream->GetOutBuffer<SIG_DOUBLE_CHECK_GUILD_ADDRESS>();
    db->set_query("seLect * from guild_info where guild_url='%s'", (char*)sig);
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
