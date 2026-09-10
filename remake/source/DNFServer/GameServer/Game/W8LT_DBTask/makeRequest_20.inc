void DB_Find_Factory_Hub_User::makeRequest(unsigned int uid, char* name,
                                           unsigned short flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x851e), true);
    guard.operator*()->operator<<((int)0x1a0);
    guard.operator*()->operator<<((int)uid);
    SIG_FIND_FACTORY_HUB_USER* sig =
        guard->GetInBuffer<SIG_FIND_FACTORY_HUB_USER>();
    strncpy(sig->m_characName, name, 0x1d);
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
