void DB_SelectLevelUpReward::makeRequest(int uid, unsigned int characNo,
                                         int level)
{
    // ORIG：Acquire 返回值直接作为 CStreamGuard 构造实参（无中间变量）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa2c6), true);
    guard.operator*()->operator<<(0x28e);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(level);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(,2,)
