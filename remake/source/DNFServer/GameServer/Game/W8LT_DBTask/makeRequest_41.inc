// ORIG 0x0843ef30（_ZN26DB_UpdateAddItemCountValue11makeRequestEjj）：
// Acquire("DBThread.cpp",0x9684) → CStreamGuard(stream,true)
//   → <<0x280(int) → <<-1(int) → <<param1(uint) → <<param2(uint)
//   → put(DB_Q(2))。
void DB_UpdateAddItemCountValue::makeRequest(unsigned int param1,
                                             unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9684), true);
    guard.operator*()->operator<<((int)0x280);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}