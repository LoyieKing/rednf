void DB_ReqSaveHackUserPunish::makeRequest(unsigned int uid, unsigned int type,
                                           const char* ip)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6304), true);
    guard.operator*()->operator<<(0xb5);           // SIG_SAVE_HACK_USER_PUNISH
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        // +0x04。int 字段←unsigned 形参异型 = 4.4 RHS-first 3 条形
        // （ORIG 0x08425494 `mov 0xc(%ebp),%edx; mov -0xc(%ebp),%eax;
        // mov %edx,0x4(%eax)`；同型 jA 形反产 LHS-first，探针 k4.cpp jD；
        // 与 SigTypes_E.h::m_punish 同批改同源证据）。
        int m_type;                                // +0x04
        char m_ip[0x10];                           // +0x08
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>());
    v->m_accId = uid;
    v->m_type = type;
    if (ip == 0)
        std::memcpy(v->m_ip, "000.000.000.000", 0x10);
    else
        std::strncpy(v->m_ip, ip, 0x10);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
