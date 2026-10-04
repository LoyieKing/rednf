bool DB_SecurityCardUpdateFailCnt::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();

    // ORIG（P48 无 CSE）：sig 字段每次即时重读，不缓存局部。
    if (sig->m_cnt == 0)
    {
        char buf[5] = { 0 };
        generateSecurityCardQuestion(buf);
        db->set_query(
            "upDate member_security_card set fail_cnt=%d , q_pos = '%s' where m_id=%s and apply_flag=1",
            sig->m_cnt,
            buf,
            NumberToString(
                sig->m_mId, 0));
    }
    else
    {
        db->set_query(
            "upDate member_security_card set fail_cnt=%d where m_id=%s and apply_flag=1",
            sig->m_cnt,
            NumberToString(
                sig->m_mId, 0));
    }
    db->exec(true);

    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set security_card_fail_cnt=%d where m_id=%s",
        sig->m_cnt,
        NumberToString(
            sig->m_mId, 0));
    if (!dbGrade->exec(true) || dbGrade->getAffectedRowCount() == 0)
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, security_card_fail_cnt) values(%s, %d)",
            NumberToString(
                sig->m_mId, 0),
            sig->m_cnt);
        if (!dbGrade->exec(true))
        {
            return 0;
        }
    }
    return 1;
}