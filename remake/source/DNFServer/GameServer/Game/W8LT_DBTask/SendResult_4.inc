// ORIG 0x841852e：结果回包 —— Acquire("DBThread.cpp",0x48e2) →
// CStreamGuard(stream,true) → <<userIdx → <<-1 → put_binary(sig,0x8b)
// → put(DISPATCH_Q,guard) 返 1（slot 实参未使用）。
bool DB_UpdateLogout::SendResult(int userIdx, int slot,
                                 SIG_UPDATE_LOGOUT* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x48e2), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<((int)-1);
    guard.operator->()->put_binary(sig, 0x8b);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}