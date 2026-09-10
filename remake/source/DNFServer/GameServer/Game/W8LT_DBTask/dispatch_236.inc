bool DB_SecurityCardRetransfer::dispatch(int userIdx, int slot,
                                         Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    SecuRetransferReqView* sig = reinterpret_cast<SecuRetransferReqView*>(
        stream->GetOutBuffer<SIG_SECURITY_CARD_RETRANSFER>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect re_issue_cnt,unix_timestamp(last_issue_time), phone from member_security_card where m_id=%s",
        NumberToString(sig->m_mId, 0));
    try
    {
        if (!db->exec(true))
            throw 1;
        if (!db->fetch())
            throw 1;
        // P171/P328⑤：ORIG 无预清（两值随后由 get_uint 写入）。
        unsigned int reIssueCnt;
        unsigned int lastIssueTime;
        db->get_uint(0, reIssueCnt);
        db->get_uint(1, lastIssueTime);
        char phone[0x29];
        memset(phone, 0, sizeof(phone));
        db->get_str(2, phone, 0x29);
        if (strncmp(sig->m_phone, phone, 0x29) != 0)
            throw 0x7d;

        int curSec = GlobalData::s_systemTime_.getCurSec();
        time_t last = (time_t)lastIssueTime;
        time_t cur = (time_t)curSec;
        struct tm tLast;
        struct tm tCur;
        localtime_r(&last, &tLast);
        localtime_r(&cur, &tCur);
        if (tLast.tm_yday == tCur.tm_yday)
        {
            reIssueCnt += 1;
            if (reIssueCnt > 3)
                throw 0x78;
        }
        else
        {
            reIssueCnt = 0;
        }

        db->set_query(
            "upDate member_security_card set cert_key='%s',server_key='%s',card='%s',fail_cnt=0,cancel_cnt=0,re_issue_cnt=%d,last_issue_time=now(),validity_time=0,apply_flag=1,cert_flag=0,cert_cnt=0 where m_id=%s",
            sig->m_certKey, sig->m_serverKey, sig->m_card, reIssueCnt,
            NumberToString(sig->m_mId, 0));
        db->exec(true);
        _sendResult(slot, sig->m_serial, sig->m_certKey, sig->m_serverKey,
                    (ENUM_ERROR)0);
    }
    catch (int err)
    {
        _sendResult(slot, 0, 0, 0, (ENUM_ERROR)err);
    }

    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set security_card_reg=now() where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (!(dbGrade->exec(true) && dbGrade->getAffectedRowCount() != 0))
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, security_card_reg) values(%s, now())",
            NumberToString(sig->m_mId, 0));
        if (!dbGrade->exec(true))
            return 0;
    }
    return 1;
