void DB_SaveGiveAvengerTitleFlag::makeRequest(int uid, unsigned int accId, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa276), true);

    **guard << 0x28d;                   // op<<(int)
    **guard << uid;                     // op<<(int)（ORIG 080c8c56）
    **guard << accId;                   // op<<(uint)
    **guard << characNo;                // op<<(uint)

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
