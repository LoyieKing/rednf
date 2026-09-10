bool DB_MailBox_Req_System_Multi_Mail::_InsertLetter(
    MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sigRaw)
{
    char titleBuf[0x1ff] = { 0 };
    db->escape_string(titleBuf,
                      ((SigSystemMultiMailView*)sigRaw)->m_sendCharacName);

    char nameBuf[0x3d] = { 0 };
    db->escape_string(nameBuf, ((SigSystemMultiMailView*)sigRaw)->m_letterText);

    db->set_query(
        "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,'%s','%s',from_unixtime(%d),%d)",
        ((SigSystemMultiMailView*)sigRaw)->m_recvCharacNo,
        ((SigSystemMultiMailView*)sigRaw)->m_sendCharacNo, nameBuf, titleBuf,
        ((SigSystemMultiMailView*)sigRaw)->m_occTime,
        (int)((SigSystemMultiMailView*)sigRaw)->m_stat);

    if (!db->exec(true))
    {
        return 0;
    }
    ((SigSystemMultiMailView*)sigRaw)->m_letterId =
        (int)WongWork::DBCommon::GetIdentity(db);
    return 1;
