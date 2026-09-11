void DB_SelectCharacMid::makeRequest(int serverGroup, unsigned int field2,
                                     char const* name, unsigned int field4,
                                     unsigned int field5, unsigned int field6)
{
    // ORIG T 0x0844676c（DB_SelectCharacMid/makeRequest.md）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xae42), true);
    **guard << 0x2b9;
    **guard << serverGroup;
    SIG_CHECK_CHAR_MID* sig = guard->GetInBuffer<SIG_CHECK_CHAR_MID>();
    sig->m_field00 = field2;
    sig->m_field04 = field4;
    sig->m_field08 = field5;
    sig->m_field0c = field6;
    strncpy(sig->m_name, name, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}