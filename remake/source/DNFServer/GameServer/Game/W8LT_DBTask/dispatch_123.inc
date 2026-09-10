bool DB_SelectCharacMid::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CHECK_CHAR_MID* sig = stream->GetOutBuffer<SIG_CHECK_CHAR_MID>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int mId = 0;
    int nRows;
    char buf[0x3b] = { '\0' };
    db->escape_string(buf, sig->m_name);
    db->set_query("seLect m_id from charac_info where charac_name='%s' limit 1",
                  buf);
    if (!db->exec(true))
        return 0;
    nRows = db->get_n_rows();
    if (nRows != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_int(0, mId))
            return 0;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xae30), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        DB_SelectCharacMidResultView* out =
            (DB_SelectCharacMidResultView*)guard->GetInBuffer<SIG_CHECK_CHAR_MID_RESULT>();
        out->m_mId = mId;
        out->m_field04 = sig->m_field04;
        out->m_field08 = sig->m_field08;
        out->m_field0c = sig->m_field0c;
        out->m_field00 = sig->m_field00;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
