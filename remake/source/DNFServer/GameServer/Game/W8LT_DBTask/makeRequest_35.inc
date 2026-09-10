void DB_UpdateSchoolPoint::makeRequest(unsigned int mId, int point, int flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7425), true);
    guard.operator*()->operator<<((int)0x129);
    guard.operator*()->operator<<((int)flag);
    if (point > 100)
    {
        point = 100;
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x742d, "School event point error (m_id: %s)(point: %d)",
            NumberToString(mId, 0), point);
    }
    SIG_UPDATE_SCHOOL_POINT* sig =
        guard->GetInBuffer<SIG_UPDATE_SCHOOL_POINT>();
    sig->m_mId = mId;
    sig->m_point = point;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
