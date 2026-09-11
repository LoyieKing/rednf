// ORIG 0x084293a0 sz 0xbd（_ZN17DB_UpdatePvPGrade11makeRequestEv）：
// Acquire("DBThread.cpp",0x6ac8) → CStreamGuard(stream,true)
//   → <<0xd4(int) → <<-1(int) → put(DB_Q(2))。
void DB_UpdatePvPGrade::makeRequest()
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ac8), true);
    guard.operator*()->operator<<((int)0xd4);
    guard.operator*()->operator<<((int)-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}