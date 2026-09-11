// ORIG 0x0843f7ee sz 0x10b（_ZN23DB_UpdateCharacPlayTime11makeRequestEil）：
// characNo==0 或 playTick==0 直接返回；Acquire("DBThread.cpp",0x99a6)
//   → CStreamGuard(stream,true) → <<0x24e(int) → <<-1(int)
//   → <<characNo(int) → <<playTick(long) → put(DB_Q(2))。
void DB_UpdateCharacPlayTime::makeRequest(int characNo, long playTick)
{
    if (characNo == 0 || playTick == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x99a6), true);
    guard.operator*()->operator<<((int)0x24e);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((int)characNo);
    guard.operator*()->operator<<((long)playTick);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}