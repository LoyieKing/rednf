void DB_GoblinPadUpdateValidityTime::makeRequest(unsigned int accId,
                                                 unsigned int flag)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7453), true);
    guard.operator*()->operator<<((int)0x12c);
    guard.operator*()->operator<<((int)-1);
    SIG_GOBLIN_UPDATE_VALIDITYTIME* sig =
        guard->GetInBuffer<SIG_GOBLIN_UPDATE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
