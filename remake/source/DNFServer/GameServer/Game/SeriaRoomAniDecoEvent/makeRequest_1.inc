// ==================== DB request 类（W8LT_EventTail.h 声明点） ====================
void DB_InsertEventStageLog::makeRequest(SIG_EVENT_STAGE_LOG& sig)
{
    // ORIG T 0x0844920e：opcode 0x333 / -1 / GetInBuffer<SIG> 后 memcpy 0x78。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb7df),
                       true);
    guard.operator*()->operator<<(0x333);      // op*(guard)→op<<(int)
    guard.operator*()->operator<<(-1);         // op*(guard)→op<<(int)
    SIG_EVENT_STAGE_LOG* out =
        guard.operator->()->GetInBuffer<SIG_EVENT_STAGE_LOG>();
    memcpy(out, &sig, sizeof(SIG_EVENT_STAGE_LOG));     // ORIG: 0x78
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
