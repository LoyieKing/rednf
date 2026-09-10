void DB_DenyJoinGuild::makeRequest(int uid, unsigned int guildKey,
                                   unsigned int applicant)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x92b4),
                       true);

    guard.operator*()->operator<<(0x21c);              // op*(guard)→op<<(int)
    guard.operator*()->operator<<(static_cast<int>(uid));
    guard.operator*()->operator<<(guildKey);           // op<<(unsigned int)
    guard.operator*()->operator<<(applicant);          // op<<(unsigned int)

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
