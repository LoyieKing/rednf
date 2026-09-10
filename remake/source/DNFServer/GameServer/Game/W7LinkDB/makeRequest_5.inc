void DB_ForceSaveSimpleSSO::makeRequest(unsigned int accId, int errCnt)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x69b1), true);

    **guard << 0xcf;
    **guard << -1;
    **guard << accId;
    **guard << errCnt;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
