void DB_CheckCharacName::makeRequest(int uid, unsigned int accId, const char* charName, bool flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4f9),
                       true);
    guard.operator*()->operator<<((int)0x2a2);   // op*(guard)→op<<(int)
    guard.operator*()->operator<<(uid);          // op*(guard)→op<<(int)
    SIG_CHECK_CHARAC_NAME* pkt =
        guard.operator->()->GetInBuffer<SIG_CHECK_CHARAC_NAME>();
    reinterpret_cast<DB_CheckCharacNameReqView*>(pkt)->m_accId = accId;
    if (charName != NULL && charName[0] != '\0')
    {
        strncpy(reinterpret_cast<DB_CheckCharacNameReqView*>(pkt)->m_charName,
                charName, 0x1e);
    }
    reinterpret_cast<DB_CheckCharacNameReqView*>(pkt)->m_flag =
        (unsigned char)flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}