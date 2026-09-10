bool DB_MailBox_Req_System_Multi_Mail::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    struct tm tmBuf;
    time_t now;
    SigSystemMultiMailView* sig = (SigSystemMultiMailView*)
        stream->GetOutBuffer<SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_3, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    struct tm* tmPtr;
    time_t t;

    now = GlobalData::s_systemTime_.getCurSec();
    tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    t = mktime(tmPtr);

    if (sig->m_duration < 0xf)
    {
        t = t - sig->m_duration * 0x15180;
    }
    else if (sig->m_duration > 0xf)
    {
        t = sig->m_duration * 0x15180 + t - 0x13c680;
    }
    sig->m_occTime = (int)t;

    if (!_InsertLetter(db, (SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL*)sig))
    {
        return 0;
    }
    if (!_InsertPostal(db, (SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL*)sig))
    {
        return 0;
    }
    sendResult(sig->m_recvCharacNo, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    return 1;
