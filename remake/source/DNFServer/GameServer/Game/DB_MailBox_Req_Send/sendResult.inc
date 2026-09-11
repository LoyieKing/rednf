// ============================================================================
// sendResult —— ORIG 0x0841f75a：Acquire("DBThread.cpp",0x559f) → CStreamGuard
// (stream,true) → <<0x80(int) → <<-1(int) → GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>
// → 写 +0x00=0/+0x04=param_3/+0x08=param_4 → put(DISPATCH_Q, guard) → return 1。
// N76 族封顶（ndiff1）：首成员 store 处 4.1 先备值(param→edx)再 reload sig；
// 4.4 恒先 reload sig 再备值，探针视图/链式/中间量不可翻（NL6 2026-09-08）。
// ============================================================================
bool DB_MailBox_Req_Send::sendResult(int userIdx, int slot, int mailIdx,
                                    ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x559f), true);

    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);

    SigMailboxSendNewMailView* sig = reinterpret_cast<SigMailboxSendNewMailView*>(
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>());
    sig->m_field00 = 0;
    sig->m_field04 = mailIdx;
    sig->m_field08 = (int)group;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
}