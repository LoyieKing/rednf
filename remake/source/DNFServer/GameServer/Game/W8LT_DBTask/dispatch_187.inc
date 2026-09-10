bool DB_CheckDoubleGuildName::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_DOUBLE_CHECK_GUILD_NAME* sig =
        stream->GetOutBuffer<SIG_DOUBLE_CHECK_GUILD_NAME>();
    db->set_query(
        "seLect * from guild_info where expire_flag=0 and guild_name='%s'",
        (char*)sig);
    // ORIG 0x842aa5f-0x842aa6b：`xor $0x1,%eax; test; je <body>; mov $0,%ebx;
    // jmp end` —— bool exec 的 `!` 直判 + return 0 岛（技巧 1/6）；body 显式块
    // 保 %ebx 汇聚（技巧 12）。0x842aacd：第二 << 实参 = 0x10(%ebp) = **slot**
    // （成员函数 this=0x8，userIdx=0xc，slot=0x10）—— 非 userIdx。
    if (!db->exec(true))
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6c86), true);
        guard.operator*()->operator<<((int)0xeb);
        guard.operator*()->operator<<((int)slot);
        if (db->get_n_rows() == 0)
            guard.operator*()->operator<<((char)1);   // ORIG lsEc（char 1 字节，勿转 int）
        else
            guard.operator*()->operator<<((char)GUILD_NAME_DOUBLE_CHECK);  // ORIG lsEc
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
