void DB_RequestCharacSkillInfo::sendResult(int userIdx, SIG_REQUEST_CHARAC_SKILL_INFO* sig)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb202), true);
    guard.operator*()->operator<<(0x2fd);
    guard.operator*()->operator<<(userIdx);
    SIG_REQUEST_CHARAC_SKILL_INFO* out = guard.operator->()->GetInBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
    memcpy(out, sig, 0x1a0);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
