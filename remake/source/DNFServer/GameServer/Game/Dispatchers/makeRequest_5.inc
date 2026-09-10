void DB_RequestJoinGuild::makeRequest(int userIdx, SIG_REQUEST_JOIN_GUILD* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x91a8), true);
    guard.operator*()->operator<<((int)0x217);             // 0x0843d1c0：opcode
    guard.operator*()->operator<<((int)userIdx);           // 0x0843d1cc
    SIG_REQUEST_JOIN_GUILD* inbuf = guard->GetInBuffer<SIG_REQUEST_JOIN_GUILD>();
    memcpy(inbuf, sig, 0x44);                              // 0x0843d1e2
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard); // 0x0843d1f6：QUEUE_IDX 2 = DB_Q
