// ORIG 0x08432fe2 sz 0xf1（_ZN20DB_SaveCleanpadPoint11makeRequestEji）：
// Acquire("DBThread.cpp",0x7aaa) → CStreamGuard(stream,true)
//   → <<0x154(int) → <<-1(int) → <<point(uint) → <<accId(int)
//   → put(DB_Q(2))。
void DB_SaveCleanpadPoint::makeRequest(unsigned int point, int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7aaa), true);
    guard.operator*()->operator<<((int)0x154);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)point);
    guard.operator*()->operator<<((int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}