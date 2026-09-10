void DB_InsertAutoPunishSecondUser::makeRequest(unsigned int accId, unsigned int who,
                                                unsigned long long gold, bool punish)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x72f4), true);

    **guard << 0x10b;
    **guard << -1;

    {
        struct InsertAutoPunishSecondView   // SIG_INSERT_AUTO_PUNISH_SECOND_USER 载荷视图
        {
            unsigned int m_accId;        // +0x00
            unsigned int m_who;          // +0x04
            unsigned long long m_gold;   // +0x08
            bool m_punish;               // +0x10（P211：与 RHS punish(bool) 同型化⇒复现 ORIG LHS-first `mov -0xc,%eax; movzbl -0x24,%edx; mov %dl,0x10(%eax)` @0x0842e34e）
        };
        InsertAutoPunishSecondView* v = reinterpret_cast<InsertAutoPunishSecondView*>(
            guard->GetInBuffer<SIG_INSERT_AUTO_PUNISH_SECOND_USER>());
        v->m_accId = accId;
        v->m_who = who;
        v->m_gold = gold;
        v->m_punish = punish;            // bool↔bool 同型直存（movzbl/mov %dl）
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
