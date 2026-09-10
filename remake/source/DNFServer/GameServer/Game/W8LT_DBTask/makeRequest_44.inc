void DB_UpdateFatigueAttendance::makeRequest(
    unsigned int param1,
    std::map<unsigned char, short, std::less<unsigned char>,
             std::allocator<std::pair<const unsigned char, short> > > const& m)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xab72), true);
    guard.operator*()->operator<<((int)0x2ac);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((int)m.size());
    for (std::map<unsigned char, short>::const_iterator it = m.begin();
         it != m.end(); ++it)
    {
        guard.operator*()->operator<<((unsigned char)it->first);
        guard.operator*()->operator<<((short)it->second);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
