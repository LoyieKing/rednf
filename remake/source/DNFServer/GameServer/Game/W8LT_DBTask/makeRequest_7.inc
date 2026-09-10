void DB_InsertAttendanceEvent::makeRequest(int userIdx, unsigned int param2,
                                           unsigned int param3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb651), true);
    guard.operator*()->operator<<((int)0x30e);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((unsigned int)param3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
