void DB_JoinGuildInfo::makeRequest(int uid, ENUM_SERVER_GROUP serverGroup, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9218), true);
    **guard << 0x219;               // opcode（ORIG lsEi）
    **guard << uid;                 // int
    **guard << serverGroup;         // ENUM_SERVER_GROUP（int 重载）
    **guard << characNo;            // unsigned 重载（ORIG lsEj）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
