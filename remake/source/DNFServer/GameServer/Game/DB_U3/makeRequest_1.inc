void DB_InsertCeraItemHistory::makeRequest(int itemIdx,
                                           unsigned int senderCharacNo,
                                           unsigned int receiverCharacNo,
                                           char flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac1b),
                       true);
    guard.operator*()->operator<<(0x2b1);          // SIG_INSERT_CERA_ITEM_HISTORY
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(itemIdx);
    guard.operator*()->operator<<(senderCharacNo);   // ORIG lsEj（unsigned）
    guard.operator*()->operator<<(receiverCharacNo); // ORIG lsEj（unsigned）
    guard.operator*()->operator<<(flag);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
