void put_db_request(CStreamGuard& guard)
{
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
