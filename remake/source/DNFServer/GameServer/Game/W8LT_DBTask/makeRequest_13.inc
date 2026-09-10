void DB_UpdateCompoundEmblemStatics::makeRequest(
    int userIdx, unsigned int param2, unsigned int param3, unsigned int param4,
    unsigned int param5, unsigned int param6, unsigned int param7,
    unsigned int param8)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x87fc), true);
    guard.operator*()->operator<<((int)0x1c4);
    guard.operator*()->operator<<((int)-1);
    SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC>();
    sig->m_userIdx = userIdx;
    sig->m_values[0] = param2;
    sig->m_values[1] = param3;
    sig->m_values[2] = param4;
    sig->m_values[3] = param5;
    sig->m_values[4] = param6;
    sig->m_values[5] = param7;
    sig->m_values[6] = param8;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
