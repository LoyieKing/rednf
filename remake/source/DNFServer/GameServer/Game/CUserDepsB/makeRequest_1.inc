void DB_EventAccountCharacterLevelUpOnceGift::makeRequest(unsigned int accId,
                                                          short serverGroup,
                                                          int characNo,
                                                          short job, short level)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb9d9), true);
    **guard << 0x319;
    **guard << -1;
    **guard << accId;
    **guard << serverGroup;
    **guard << characNo;
    **guard << job;
    **guard << level;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
