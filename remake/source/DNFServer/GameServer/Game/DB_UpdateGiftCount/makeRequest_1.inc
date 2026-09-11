// ============================================================================
// DB_UpdateBreakAwayAccruedCera::makeRequest ORIG @ 0x0843856a
//   Acquire("DBThread.cpp", 0x85fe) | opcode 0x146 | -1 | param_1 | param_2
// ============================================================================
void DB_UpdateBreakAwayAccruedCera::makeRequest(unsigned int param_1,
                                                unsigned int param_2)
{
    // b28c-33：ORIG 无 stream 具名局部（Acquire 返回值 eax 直传 guard ctor 的
    // 0x4(%esp)），值缓存局部多 1 store+1 load 并使 guard 槽 -0x10→-0x14 ⇒ 内联实参。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x85fe), true);
    guard.operator*()->operator<<(0x146);             // SIG_UPDATE_BREAKAWAY_ACCRUED_CERA
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}