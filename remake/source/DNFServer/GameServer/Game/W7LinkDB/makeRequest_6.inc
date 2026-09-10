void DB_CountOfHumanCertify::makeRequest(unsigned int accId)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x69d6), true);

    **guard << 0x111;
    **guard << -1;
    **guard << accId;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
