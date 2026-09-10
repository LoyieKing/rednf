void DB_ReqChangeCharacName::_sendResult(SIG_CHANGE_CHARAC_NAME const* sig, int slot, int result)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x64f8), true);
    guard.operator*()->operator<<(0xc3);
    guard.operator*()->operator<<(slot);
    SIG_CHANGE_CHARAC_NAME* out = guard.operator->()->GetInBuffer<SIG_CHANGE_CHARAC_NAME>();
    // ORIG 0842672e `mov 0x24(%eax),%edx` 单段直落：源/目标同型 unsigned
    // （m_arg28 uint ← m_arg24u uint 视角）；int 视角恒产 `mov 0x24(%eax),%eax;
    // mov %eax,%edx` 两段（旧 N18a 判词作废，P211 档③ 同型化即消中转）。
    out->m_arg28 = sig->m_arg24u;
    out->m_arg24 = result;
    out->m_characNo = sig->m_characNo;
    out->m_arg2c = sig->m_arg28;
    strncpy(out->m_name, sig->m_name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
