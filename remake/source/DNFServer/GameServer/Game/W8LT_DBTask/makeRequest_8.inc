// ORIG 0x084318c4 sz 0xd5（_ZN22DB_LoadBloodBestRecord11makeRequestEij）：
// Acquire("DBThread.cpp",0x78c6) → CStreamGuard(stream,true)
//   → <<0x13f(int) → <<param_1(int) → <<param_2(uint) → put(DB_Q(2))。
void DB_LoadBloodBestRecord::makeRequest(int userIdx, unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x78c6), true);
    guard.operator*()->operator<<((int)0x13f);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}