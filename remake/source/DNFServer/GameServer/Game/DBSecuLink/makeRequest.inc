void DB_CheckJoinGuild::makeRequest(int uid, SIG_CHECK_JOIN_GUILD* sig)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x911a), true);

    **guard << 0x216;                   // op<<(int)
    **guard << uid;                     // op<<(int)

    {
        SIG_CHECK_JOIN_GUILD* p = guard->GetInBuffer<SIG_CHECK_JOIN_GUILD>();
        memcpy(p, sig, 0x1c);           // sizeof(SIG_CHECK_JOIN_GUILD) == 0x1c
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
