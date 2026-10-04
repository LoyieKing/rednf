// ============================================================================
// dispatch —— ORIG 0x0841f84e（虚覆盖）：GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL>
// → GetDBHandle(DB_HANDLE_3, sig+0x15f) → insertMailData 失败返 0；
// 成功 sendResult(slot, 0, sig+0x22, sig+0x15f)。
// ============================================================================
bool DB_MailBox_Req_Send::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_MAILBOX_SEND_NEW_MAIL* sigRaw = stream->GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_3, sigRaw->m_serverGroup);
    if (!insertMailData(db, sigRaw))
    {
        return 0;
    }
    return sendResult(slot, 0,
                      (int)sigRaw->m_amount,
                      sigRaw->m_serverGroup);
}