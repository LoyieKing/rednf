bool Inter_MailBox_Req_CheckReceiver::SendDBThread_forchina(
    CUser* user, SIG_MAILBOX_CHECK_RECEIVER* sig)
{
    // [b28c-34] ORIG 0x084cff18 段：Acquire 结果无槽往返（P0 右值直传实参）⇒ 内联；
    // cMyTrace 用全表达式临时链复现 ORIG 槽方向（guard -0x24 深 / trace -0x1c 浅，
    // P221/P227；同 TU DB_Multi_MailBox_Req_Send::_sendResult 同形实证）。
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire(
        "InterDispatcher.cpp", 0x2dfa), true);
    guard.operator*()->operator<<(0x1f5);
    cMyTrace(__PRETTY_FUNCTION__, 0x2dfd, 0)(
        "E_MULTI_MAILBOX_REQ_SEND : %d", 0x1f5);
    guard.operator*()->operator<<(
        static_cast<int>(user->GetUID()));
    SIG_MAILBOX_CHECK_RECEIVER* dst = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    memcpy(dst, sig, 0x3fde);
    strncpy(dst->m_senderName, user->getCurCharacName(), 0x1d);
    // [b28c-34] N76 封顶（P211 同型性两端冲突）：ORIG 0x084cffc9 store 段
    // `call; mov -0xc(%ebp),%edx; mov %eax,0x2d(%edx)` 要求字段与 int RHS 同型；
    // 但同字段 load 段（C28L20 DB_Multi_MailBox_Req_Send::dispatch 0x0843bb89
    // `mov 0x2d(%eax),%eax; mov %eax,%edx`）要求与 int 形参异型 ⇒ 共享头字段
    // 维持 unsigned int（多数消费段+零回归），本 store 点余 1 条 `mov %eax,%edx`
    // 中转。已试：共享头 int 化（C28L20 回归 13→16）、源侧 int& 视图（4.4 把
    // 地址提前缓存到 %ebx，+3 条）、*(int*)& 形同。
    dst->m_senderCharacNo = user->getCurCharacNo();   // +0x2d
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    return true;
