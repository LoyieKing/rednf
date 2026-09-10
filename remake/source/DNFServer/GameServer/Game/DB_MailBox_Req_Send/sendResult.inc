bool DB_MailBox_Req_Send::sendResult(int userIdx, int slot, int mailIdx,
                                    ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x559f), true);

    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);

    SigMailboxSendNewMailView* sig = reinterpret_cast<SigMailboxSendNewMailView*>(
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>());
    sig->m_field00 = 0;
    sig->m_field04 = mailIdx;
    sig->m_field08 = (int)group;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
