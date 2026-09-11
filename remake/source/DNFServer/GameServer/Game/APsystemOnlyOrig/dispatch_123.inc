// ============================================================================
// DB_SelectCharacMid::dispatch —— ORIG 0x08446516（报告 dispatch.md）。
// GetOutBuffer<SIG_CHECK_CHAR_MID> → GetDBHandle(DB_HANDLE_2,0) →
// escape_string(buf, sig->m_name)（buf[0x3b] 清零，P4）→ seLect m_id from
// charac_info where charac_name='%s' limit 1 → exec 失败返 0；
// get_n_rows()!=0 时 fetch / get_int(0,mId) 任一失败返 0 → Acquire(
// "DBThread.cpp",0xae30) → guard << userIdx << slot → GetInBuffer<
// SIG_CHECK_CHARAC_MID_RESULT> 写 +0x10=mId、+0/+4/+8/+0xc 复制输入 →
// MsgQueueMgr::put(DISPATCH_Q,guard)；恒返 1。
// ============================================================================
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
}