// ============================================================================
// SendResult（ORIG T 0x08414c6e）/ UpdateLoginStat（ORIG T 0x08414d50）
// ============================================================================

// [SweepDBLogin-2] P292/T3：ORIG 9 个调用点均 `call; mov %eax,%ebx`（无 setne）
// ⇒ 返回类型 bool（mangled 不含返回类型，ABI 零变化；旧判词「4.4 返回值规范化 N」
// 已按 P248 档① 翻案）。
bool DB_Login::SendResult(int userDbIdx, int code, SIG_LOGIN_DATA* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x3c49), true);
    guard.operator*()->operator<<(userDbIdx);
    guard.operator*()->operator<<(code);
    guard.operator->()->put_binary(sig, 0x4358);  // SIG_LOGIN_DATA 全量回传
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}
