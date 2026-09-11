// ORIG 0x0844d0a6 sz 0xd5（_ZN26DB_LoadEventLevelupSupport11makeRequestEii）：
// Acquire("DBThread.cpp",0xc2a0) → CStreamGuard(stream,true)
//   → <<0x350(int) → <<param_1(int) → <<param_2(int) → put(DB_Q(2))。
void DB_LoadEventLevelupSupport::makeRequest(int param1, int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2a0), true);
    guard.operator*()->operator<<((int)0x350);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}