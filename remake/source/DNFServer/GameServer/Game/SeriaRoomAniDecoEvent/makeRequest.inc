void DB_UpdateSeriaRoomAnimationDeco::makeRequest(
    unsigned int accId, short serverGroup, int characNo, short field1, short field2)
{
    // ORIG T 0x0844af74：opcode 0x31e / -1 / 五参顺序写流。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbb98), true);
    guard.operator*()->operator<<(0x31e);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(serverGroup);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(field1);
    guard.operator*()->operator<<(field2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
