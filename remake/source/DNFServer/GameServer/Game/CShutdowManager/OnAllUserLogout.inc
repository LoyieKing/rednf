void CShutdowManager::OnAllUserLogout()
{
    // [b28d-27] P0/N96 清绑：ORIG 0x082a36bc `movzbl (%eax),%eax; xor $1; test %al;
    // jne <尾>` ⇒ 裸 `if (!m_flag) return;`（P248 档②）。
    if (!m_flag)
        return;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x323b, 0);
    // ORIG +0x03c `movl $"ALL USER LOGOUT COMPLETE\n"`——日志第二点：消息串须与
    // ORIG 逐字节一致（原"Send message to monitor for logout all user."是错误串）。
    trace("ALL USER LOGOUT COMPLETE\n");
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           (const char*)"App.cpp", 0x323d),
                       true);
    guard.operator*()->operator<<(0x3f);      // op*(guard)→op<<(int)
    guard.operator*()->operator<<(-1);        // op*(guard)→op<<(int)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    GlobalData::s_server_stop = true;
