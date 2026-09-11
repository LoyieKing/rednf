// ============================================================================
// DB_UpdateSchoolPoint::makeRequest —— ORIG 0x0842f0f0（报告 makeRequest.md 全文）
// ============================================================================
// ORIG 0x842f0f0：Acquire("DBThread.cpp",0x7425) → CStreamGuard(stream,true)
//   → <<0x129(int) → <<flag(int) → point>100 时钳制 100 并
//   logFormat(1,"DBThread.cpp",__PRETTY_FUNCTION__,0x742d,
//   "School event point error (m_id: %s)(point: %d)",NumberToString(mId,0),100)
//   → sig=GetInBuffer<SIG_UPDATE_SCHOOL_POINT>() → sig+0(uint)=mId、
//   sig+4(int)=point → put(DB_Q(2))。
// ============================================================================
typedef unsigned int memberIdentificationNumber_t;
void DB_UpdateSchoolPoint::makeRequest(memberIdentificationNumber_t mId, int point, int flag)
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
}