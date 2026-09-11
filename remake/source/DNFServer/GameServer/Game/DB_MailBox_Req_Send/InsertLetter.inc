// ============================================================================
// InsertLetter —— ORIG 0x0841f15a：strlen(title) > 0xaa → logFormat(0x54bc) 返 0；
// escape title→0x1ff 缓冲、name→0x3d 缓冲 → set_query(letter insert) →
// exec → GetIdentity → *letterId → 1。
// ============================================================================
bool DB_MailBox_Req_Send::InsertLetter(MySQL* db, int characNo, int sendCharacNo,
                                       const char* name, const char* title,
                                       int& letterId, time_t regDate,
                                       ENUM_LETTER_STAT stat)
{
    if (strlen(title) > 0xaa)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x54bc,
            "send_charac_no : %d, recv_charac_no : %d, letter_size : %d",
            sendCharacNo, characNo, (int)strlen(title));
        return 0;
    }

    char titleBuf[0x1ff] = { 0 };
    db->escape_string(titleBuf, title);

    char nameBuf[0x3d] = { 0 };
    db->escape_string(nameBuf, name);

    db->set_query(
        "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,'%s','%s',from_unixtime(%d),%d)",
        characNo, sendCharacNo, nameBuf, titleBuf, (long)regDate, (int)stat);

    if (!db->exec(true))
    {
        return 0;
    }
    letterId = (int)WongWork::DBCommon::GetIdentity(db);
    return 1;
}