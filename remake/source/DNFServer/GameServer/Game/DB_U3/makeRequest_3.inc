void DB_InsertUnlimitSupportLog::makeRequest(unsigned int uid, unsigned int characNo,
                                             const char* name)
{
    if (uid <= 0x946501)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7392), true);
    guard.operator*()->operator<<(0x122);          // SIG_INSERT_UNLIMIT_SUPPORT_LOG
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_serverGroup;                // +0x04
        unsigned int m_characNo;                   // +0x08
        char m_name[0x1d];                         // +0x0c
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>());
    v->m_accId = uid;
    v->m_characNo = characNo;
    // 报告语句序：先写 +8，再调 G_CEnvironment 回填 +4（serverGroup）。
    v->m_serverGroup =
        static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_serverGroup);
    std::strncpy(v->m_name, name, 0x1d);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
