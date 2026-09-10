bool DB_LoadExpertJob::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    *stream >> characNo;
    db->set_query(
        "seLect expert_job_giveup_cnt, expert_job_info, expert_job_info_ex, recipe  from charac_expert_job where charac_no=%d",
        characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x81b2), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        SIG_EXPERT_JOB* sig =
            guard.operator->()->GetInBuffer<SIG_EXPERT_JOB>();
        memset(sig, 0, 0x30);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (db->fetch() != 1)
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x81c2), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        SIG_EXPERT_JOB* sig =
            guard.operator->()->GetInBuffer<SIG_EXPERT_JOB>();
        memset(sig, 0, 0x30);
        if (!db->get_ushort(0, sig->m_giveupCount))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_int(1, sig->m_endurance))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_int(2, sig->m_enchantCount))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_binary(3, sig->m_recipeFlag, 0x1e))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
