void DB_ReqChangeCharacName::makeRequest(int accId, int characIdx,
                                         char const* name, int flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x64de), true);
    guard.operator*()->operator<<(0xc3);
    guard.operator*()->operator<<(accId);       // operator<<(int) 0x080c8c56

    // ORIG 请求体 = SIG_CHANGE_CHARAC_NAME（SigTypes_A.h，总 0x30）：
    // +0x00 m_characNo ← characIdx / +0x04 m_name strncpy 0x1d /
    // +0x24 m_arg24 ← flag / +0x2c m_arg2c 低字节 ← 渠道组
    // （CEnvironment+0x378 = m_serverEnvir.m_serverGroup，ORIG mov %dl,0x2c）。
    SIG_CHANGE_CHARAC_NAME* body =
        guard.operator->()->GetInBuffer<SIG_CHANGE_CHARAC_NAME>();
    *reinterpret_cast<int*>(body) = characIdx;
    strncpy(body->m_name, name, 0x1d);
    // +0x24：走 SigTypes_A.h 匿名 union 的 unsigned 视角（P211 档③：字段与 RHS
    // 异型 ⇒ RHS-first+edx 中转），复现 ORIG 0842663c `mov 0x14(%ebp),%edx;
    // mov -0xc(%ebp),%eax; mov %edx,0x24(%eax)`；同字段 _sendResult 侧用 int
    // 视角保持 LHS-first（ORIG 0842673a），探针 /tmp/p211q.cpp f1↔f2。
    body->m_arg24u = flag;
    // +0x2c 低字节：走 SigTypes_A.h 的匿名 union `m_group` 视角（P235），完整
    // 类型成员直访问复现 ORIG 08426655 `mov %dl,0x2c(%eax)`（指针 cast 视图形
    // 恒产 `mov %al,0x2c(%ebx)` 并丢 edx 中转，探针 /tmp/p211p.cpp f1↔f3）。
    body->m_group =
        static_cast<char>(G_CEnvironment()->m_serverEnvir.m_serverGroup);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
