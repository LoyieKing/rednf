bool DB_LoadAutoMarketConditionsControl::makeRequest(bool flag)
{
    // ORIG T 0x08433ada _ZN34DB_LoadAutoMarketConditionsControl11makeRequestEb
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7c43);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x15a);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
