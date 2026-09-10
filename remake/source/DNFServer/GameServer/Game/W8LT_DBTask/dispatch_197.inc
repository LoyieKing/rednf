bool DB_PassPadUpdateFailCnt::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    DB_PassPadUpdateFailCntView* v =
        reinterpret_cast<DB_PassPadUpdateFailCntView*>(sig);
    db->set_query(
        "upDate member_mousepass set fail_cnt=%d where m_id=%s and enable_flag = '1'",
        v->m_failCnt, NumberToString(v->m_mId, 0));
    db->exec(true);
    if (v->m_failCnt != 0)
    {
        db->set_query(
            "seLect mousepass from member_mousepass where m_id=%s",
            NumberToString(v->m_mId, 0));
        bool rowOk;
        if (db->exec(true) != 1 || db->get_n_rows() == 0 || db->fetch() != 1)
            rowOk = false;
        else
            rowOk = true;
        if (!rowOk)
            return 0;
        char mousepass[0x24] = { 0 };   // -0x48
        if (db->get_str(0, mousepass, 0x24) != 1)
            return 0;
        char ym[10] = { 0 };            // -0x1e（strftime "%Y%m"）
        time_t curSec = GlobalData::s_systemTime_.getCurSec();   // -0x24
        struct tm tmBuf;                // -0x74
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(v->m_mId, 0), mousepass, 0x11, v->m_ipInfo);
        db->exec(true);
    }
    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set goblin_fail_cnt=%d where m_id=%s",
        v->m_failCnt, NumberToString(v->m_mId, 0));
    bool upOk;
    if (dbGrade->exec(true) != 1 || dbGrade->getAffectedRowCount() == 0)
        upOk = false;
    else
        upOk = true;
    if (!upOk)
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(v->m_mId, 0), v->m_failCnt);
        if (dbGrade->exec(true) != 1)
            return 0;
    }
    return 1;
