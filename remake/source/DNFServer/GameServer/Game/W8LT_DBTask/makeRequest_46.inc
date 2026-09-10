void DB_UpdateUserFatigueQuantity::makeRequest(unsigned int accId, unsigned int qty)
{
    // ORIG 0x08441c2a：StreamPool::Acquire("DBThread.cpp", 0xa14c) →
    // guard << 0x287 << -1 << accId << qty → MsgQueueMgr::put(2, guard)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa14c), true);
    guard.operator*()->operator<<((int)0x287);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(qty);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
