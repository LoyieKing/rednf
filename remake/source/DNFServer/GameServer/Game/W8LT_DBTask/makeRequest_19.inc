// ORIG 0x0843351e sz 0xbd（_ZN25DB_BlackIPMonitorPartLoad11makeRequestEv）：
// Acquire("DBThread.cpp",0x7b3d) → CStreamGuard(stream,true)
//   → <<0x155(int) → <<-1(int) → put(DB_Q(2))。
void DB_BlackIPMonitorPartLoad::makeRequest()
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7b3d), true);
    guard.operator*()->operator<<((int)0x155);
    guard.operator*()->operator<<((int)-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}