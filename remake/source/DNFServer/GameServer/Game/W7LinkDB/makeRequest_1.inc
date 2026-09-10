void DB_InsertAutoPunishFirstUser::makeRequest(unsigned int accId, char const* ip,
                                               unsigned int a3, unsigned int a4,
                                               unsigned int a5, unsigned int a6,
                                               unsigned short a7, unsigned int a8)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x723e), true);

    **guard << 0x10a;
    **guard << -1;

    {
        struct InsertAutoPunishFirstView   // SIG_INSERT_AUTO_PUNISH_FIRST_USER 载荷视图
        {
            unsigned int m_accId;        // +0x00
            char m_ip[0x10];             // +0x04
            unsigned int m_a3;           // +0x14
            unsigned int m_a4;           // +0x18
            unsigned int m_a5;           // +0x1c
            unsigned int m_a6;           // +0x20
            unsigned short m_a7;         // +0x24
            unsigned int m_a8;           // +0x28
        };
        InsertAutoPunishFirstView* v = reinterpret_cast<InsertAutoPunishFirstView*>(
            guard->GetInBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>());
        v->m_accId = accId;
        strncpy(v->m_ip, ip, 0x10);
        v->m_a3 = a3;
        v->m_a4 = a4;
        v->m_a5 = a5;
        v->m_a6 = a6;
        v->m_a7 = a7;
        v->m_a8 = a8;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
