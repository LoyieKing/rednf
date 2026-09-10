void DB_UpdateUserGiftFatigueQuantity::makeRequest(unsigned int accId, unsigned int qty,
                                                   unsigned int characNo)
{
    // ORIG 0x08441fde：Acquire("DBThread.cpp", 0xa1b1) → 0x288/-1/accId/qty/characNo → put(2)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa1b1), true);
    guard.operator*()->operator<<((int)0x288);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(qty);
    guard.operator*()->operator<<(characNo);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
