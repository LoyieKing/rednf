// ---------------------------------------------------------------------------
// DB_SelectAllowProxyUser::makeRequest —— ORIG 0x0844d2d4 逐指令转写
// （报告 docs/class_func_reports/DB_SelectAllowProxyUser/makeRequest.md 全文已读）：
// Acquire("DBThread.cpp",0xc2c6) → CStreamGuard(stream,true)
//   → <<0x352(int) → <<userIdx(int) → <<accId(uint) → MsgQueueMgr::put(DB_Q(2))。
void DB_SelectAllowProxyUser::makeRequest(int userIdx, unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2c6), true);
    guard.operator*()->operator<<((int)0x352);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}