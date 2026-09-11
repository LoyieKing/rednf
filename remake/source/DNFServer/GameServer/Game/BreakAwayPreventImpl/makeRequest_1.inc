void CLocalChina_DB_BreakAwayReset::makeRequest(unsigned int accId)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_DBThread.cpp", 0x408), true);
    guard.operator*()->operator<<(0x2dd);   // ORIG opcode（localchina DBThread 分发表）
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);   // ORIG operator<<(unsigned)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}