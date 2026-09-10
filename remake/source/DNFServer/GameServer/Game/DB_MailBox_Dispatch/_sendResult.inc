bool DB_Multi_MailBox_Req_Send::_sendResult(int slot, SIG_MAILBOX_CHECK_RECEIVER* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x8e67), true);

    guard.operator*()->operator<<(0x1f5);

    // b28c-L20: 全表达式临时链（P221/P227）复现 ORIG 0x0843bc85 的 ctor/operator()
    // 槽方向（ORIG trace -0x1c 浅 / guard -0x24 深；具名 trace 时 4.4 反向）。
    cMyTrace(__PRETTY_FUNCTION__, 0x8e6a, 0)("E_MULTI_MAILBOX_REQ_SEND : %d", 0x1f5);
    guard.operator*()->operator<<(slot);
    SIG_MAILBOX_CHECK_RECEIVER* out = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    memcpy(out, sig, 0x3fde);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
