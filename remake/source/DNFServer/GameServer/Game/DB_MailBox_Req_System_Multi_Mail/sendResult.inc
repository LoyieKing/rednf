int DB_MailBox_Req_System_Multi_Mail::sendResult(int userIdx,
                                                  ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa5be), true);

    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);

    SigMailboxSendReplyView* sig = (SigMailboxSendReplyView*)
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    sig->m_field00 = 0;
    sig->m_field04 = userIdx;
    sig->m_field08 = (int)group;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
