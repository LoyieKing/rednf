int Dispatcher_MailBox_Open::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cc196
    stMailboxOpenParam& out = static_cast<stMailboxOpenParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x22c8,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }

    WongWork::CMailBox* mailBox = (WongWork::CMailBox*)user->GetMailBox();
    if (mailBox->GetNotLoadedMailCount() != 0)
    {
        int recvSize = mailBox->GetRecvSize();
        int loadedCount = mailBox->GetLoadedLetterCount();
        if (recvSize + loadedCount <= 0x13)   // ORIG lea (ebx+eax); cmp $0x13; setle
        {
            WongWork::CMailBoxHelper::ReqDBLoadMail(user,
                                                    (unsigned int)user->getCurCharacNo());
            out.m_4 = 0x16;
            return 0;
        }
    }
    // ORIG：未发 DB 请求时再次调 GetNotLoadedMailCount 写 param+8
    *(int*)((char*)&param + 8) = mailBox->GetNotLoadedMailCount();
    return 0;
