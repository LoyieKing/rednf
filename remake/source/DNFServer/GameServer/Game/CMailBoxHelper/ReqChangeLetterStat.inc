int CMailBoxHelper::ReqChangeLetterStat(CUser* user, int mailIdx, int state)
{
    // ORIG 0x085568a6
    // P251/P286：ORIG `mov 0x10(%ebp),%eax; cmp $2; je; cmp $3; je; test; je`
    // 为无槽寄存器比较链（4.4 if 链折 cmpl mem 直比）⇒ switch 形。
    switch (state)
    {
    case 0:
    case 2:
        break;
    case 3:
        {
            CMailBox* box = (CMailBox*)user->GetMailBox();
            if (box == 0)
                return 0x15;
            if (10 < box->GetLetterKeepCount() + 1)
                return 0x16;
        }
        break;
    default:
        return 0x13;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8f0), true);
    guard.operator*()->operator<<(0xc2);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_LETTER_STAT* p = guard.operator->()->GetInBuffer<SIG_LETTER_STAT>();
    p->m_characNo = (int)user->getCurCharacNo();
    p->m_mailIdx = mailIdx;
    p->m_state = (short)state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
