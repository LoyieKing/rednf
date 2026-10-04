// ORIG 0x0815c586：user->m_rewarded 非 0 直接返回；Acquire(file, 0x294) →
// CStreamGuard(true) → internal_stream(guard, 0xc, user->GetUID()) →
// GetInBuffer<MSG> null → cMyTrace(0x29a)("[Taiwan, MobileAuth] msg is null")；
// 否则 *buf = user->get_acc_id()；MsgQueueMgr::put(DB_Q, guard)。
void DBSelectMobileAuthRewardTw::makeRequest(CUser* user)
{
    if (user->m_rewarded != 0)
        return;
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x294), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0xc, user->GetUID());
    DBSelectMobileAuthRewardTw::MSG* sig =
        guard.operator->()->GetInBuffer<DBSelectMobileAuthRewardTw::MSG>();
    if (sig == 0)
    {
        // [b28c-32] P221 全表达式临时链：ORIG trace 槽 -0x1c（浅于 guard -0x24），
        // 具名声明在 4.4 落 -0x24（更深）⇒ 改构造即调用临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x29a, 0)(
            "[Taiwan, MobileAuth] msg is null");
    }
    else
    {
        // [b28c-32] 具名字段（P156）；ORIG 0x0815c64a call get_acc_id 后
        // `mov -0xc(%ebp),%edx; mov %eax,(%edx)`（地址 call 后重物化）。
        sig->m_id = user->get_acc_id();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}
