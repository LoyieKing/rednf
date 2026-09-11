// ORIG 0x8419770：结果回包 —— Acquire("DBThread.cpp",0x4b17) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → put_binary(sig,0x19c)
// → put(DISPATCH_Q,guard) 返 1。
bool DB_LoadTicket::SendResult(int userIdx, int slot, SIG_LOAD_TICKET* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4b17), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0x19c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}