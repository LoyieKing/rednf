bool DB_Multi_MailBox_Req_Send::dispatch(int userIdx, int slot, Stream* stream)
{
    // b28d-SweepDBMailBoxDispatch（AE，84=84，jcc 全等）：title 实参已改整型加法形
    // 复现 ORIG 两步 add/mov（见调用点注释）。
    static const int handleIdx = 3;
    SIG_MAILBOX_CHECK_RECEIVER* sig = stream->GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER>();

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    struct tm* tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    time_t t = mktime(tmPtr);

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    if (!DB_MailBox_Req_Send::InsertLetter(
            db, (int)sig->m_receiverCharacNo, (int)sig->m_senderCharacNo,
            sig->m_senderName,                 // +0x3fc1（0843bb6e lea）
            // 报告 +0xdb：透传区内标题串（区无字段化证据）。等价于 m_payload+0xaa
            // （m_payload@+0x31）；P319 整型加法形复现 ORIG 0x0843bb71 两步
            // `add $0xdb,%eax; mov %eax,%ecx`（数组退化在 4.4 恒折单条 lea）。
            (const char*)((unsigned int)sig + 0xdb),
            sig->m_letterId,                   // +0x3fbd int&（0843bb65 lea；InsertLetter Ri）
            t, LETTER_STAT_NONE))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x8e53,
                              "InsertLetter() error");
    }

    _sendResult(slot, sig);
    return 1;
