// ORIG T 0x08449032：发 0x31f、uid、accId、eventId、won 到 DB_Q。
void DB_UpdateBingoEvent::makeRequest(int uid, unsigned int accId,
                                      int eventId, bool won)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb75e), true);
    guard.operator*()->operator<<((int)0x31f);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(eventId);
    guard.operator*()->operator<<(static_cast<char>(won));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}