void DB_AccountHack_CheckMyAccount::makeRequest(unsigned int uid,
                                                unsigned int accId,
                                                unsigned int a,
                                                unsigned int money,
                                                char* senderName, int nameLen)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8846), true);
    guard.operator*()->operator<<(0x18b);  // SIG_CHECK_MY_ACCOUNT
    guard.operator*()->operator<<((int)uid);

    SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
        guard->GetInBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
    sig->m_param0 = a;
    sig->m_accId = accId;
    sig->m_money = money;
    std::memset(sig->m_name, 0, 0x1d);
    std::strncpy(sig->m_name, senderName, nameLen);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
