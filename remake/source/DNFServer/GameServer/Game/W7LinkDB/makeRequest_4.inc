void DB_HackIPSave::makeRequest(char const* startIp, char const* rawIp,
                                unsigned int a3, unsigned int a4)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x82be), true);

    **guard << 400;
    **guard << -1;

    if (startIp != 0)
    {
        if (strlen(startIp) <= 7)
            return;                                        // ORIG jbe → 直接到析构返回
    }

    {
        struct HackIpSaveView   // SIG_HACK_IP_SAVE 载荷视图
        {
            char m_startIp[0xd];     // +0x00
            char m_rawIp[0x10];      // +0x0d
            char m_pad[3];           // +0x1d
            unsigned int m_a3;       // +0x20
            unsigned int m_a4;       // +0x24
        };
        HackIpSaveView* v = reinterpret_cast<HackIpSaveView*>(
            guard->GetInBuffer<SIG_HACK_IP_SAVE>());
        strncpy(v->m_startIp, startIp, 0xd);
        v->m_a3 = a3;
        v->m_a4 = a4;
        strncpy(v->m_rawIp, rawIp, 0x10);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
