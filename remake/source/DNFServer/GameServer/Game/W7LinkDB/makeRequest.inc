void DB_AutoPunishedSecondUserData::makeRequest(unsigned int accId, unsigned long long gold,
                                                unsigned int uid, unsigned int who)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8912), true);

    **guard << 0x1ca;                   // SIG opcode（op<<(int)）
    **guard << static_cast<int>(uid);   // op<<(int)，ORIG 080c8c56

    // GetInBuffer<SIG_SECOND_USER_DATA>：ORIG memset(payload, 0, 0x14)
    {
        struct SecondUserDataView   // SIG_SECOND_USER_DATA 载荷视图
        {
            unsigned int m_accId;        // +0x00
            unsigned long long m_gold;   // +0x04
            unsigned int m_uid;          // +0x0c
            unsigned int m_who;          // +0x10
        };
        SecondUserDataView* v =
            reinterpret_cast<SecondUserDataView*>(guard->GetInBuffer<SIG_SECOND_USER_DATA>());
        memset(v, 0, 0x14);
        v->m_accId = accId;
        v->m_gold = gold;
        v->m_uid = uid;
        v->m_who = who;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
