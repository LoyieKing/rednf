// ORIG 0x0843eca4 sz 0xcb（_ZN24DB_UpdateItemCountValue11makeRequestEjj）：
// Acquire("DBThread.cpp",0x960e) → CStreamGuard(stream,true)
//   → <<0x240(int) → <<-1(int) → <<param_1(uint) → <<param_2(uint)
//   → put(DB_Q(2))。
void DB_UpdateItemCountValue::makeRequest(unsigned int param1, unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x960e), true);
    guard.operator*()->operator<<((int)0x240);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}