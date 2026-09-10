void CLocalChina_DB_LastDay_BreakAway::makeRequest(unsigned int accId,
                                                   int day, int flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_DBThread.cpp", 0x438), true);   // ORIG 字符串 .cpp
    guard.operator*()->operator<<(0x2de);  // ORIG opcode
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);  // ORIG operator<<(unsigned)（ebp+0x08）
    guard.operator*()->operator<<(flag);   // ORIG operator<<(int)（ebp+0x10，第三参）
    guard.operator*()->operator<<(day);    // ORIG operator<<(int)（ebp+0x0c，第二参）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
