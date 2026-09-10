void DB_CancelJoinGuild::makeRequest(int uid, unsigned int requestIdx, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9234), true);
    **guard << 0x218;               // opcode（ORIG lsEi）
    **guard << uid;                 // int
    **guard << requestIdx;          // unsigned（ORIG lsEj）
    **guard << characNo;            // unsigned（ORIG lsEj）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
