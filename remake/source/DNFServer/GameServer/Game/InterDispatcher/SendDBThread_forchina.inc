// ORIG 0x084cff18 T _ZN31Inter_MailBox_Req_CheckReceiver21SendDBThread_forchinaEP5CUserP26SIG_MAILBOX_CHECK_RECEIVER
// 报告 SendDBThread_forchina.md：CStreamGuard(BigStreamPool) →写 0x1f5→写 UID→
// GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER> memcpy 0x3fde→+0x3fc1 角色名 0x1d→
// +0x2d 角色号→MsgQueueMgr::put(2)→返回 true。
// b28c-l23 SIGFIX：int→bool（ORIG __PRETTY_FUNCTION__ 串，P174；体尾
// `mov $0x1,%ebx` 直返，bool 免归一）。
// [b28d-lane2] verdict=DIFF ndiff=28（ORIG 0x084cff18）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=5（ORIG 0x84cff18）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
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
}