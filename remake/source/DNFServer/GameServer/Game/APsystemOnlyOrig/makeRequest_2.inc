// ORIG 0x0843b6ec sz 0xd6：Acquire("DBThread.cpp",0x8c75) → CStreamGuard(stream,true)
//   → guard<<(int)0x1f2 → guard<<(int)userIdx → guard<<(unsigned)accId
//   → MsgQueueMgr::put(DB_Q(2), guard)。
void DB_LoadAccountCargo::makeRequest(int userIdx, unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c75), true);
    guard.operator*()->operator<<((int)0x1f2);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}