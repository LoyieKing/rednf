bool TimerQueueSizeLog::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // remain/put 为 MsgQueueMgr 成员（ORIG this=s_msgq_mgr）；2=DB_Q、4=LOGDB_Q。
    const int remaining = static_cast<int>(GlobalData::s_msgq_mgr->remain(MsgQueueMgr::DB_Q));
    if (remaining > 10)
    {
        // ORIG 0x08636246：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)（无 stream 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                              0xa7f), true);
        // ORIG 0x08636262..：每项 `*guard`（operator* deEv 0x080c8c46）后 lsEi
        // （0x0863626d/288/29e/2c7 各一次 lea+call deEv），非链式折叠。
        (*guard)->operator<<(0x9b);
        (*guard)->operator<<(-1);
        (*guard)->operator<<(remaining);
        (*guard)->operator<<(G_CEnvironment()->get_channel_no());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
    // ORIG 0x0863631c：`if(!RegistNextTimer()) return 0; return 1;`
    // bool 结果落槽 -0x9(%ebp)（0x08636327 mov %al,-0x9），movzbl+xor$1 尾岛。
    const bool ok = RegistNextTimer();
    if (!ok)
        return 0;
    return 1;
}