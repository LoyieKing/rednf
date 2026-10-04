// ============================================================================
// DB_PassPadUpdateCancelCnt::dispatch —— ORIG 0x08423fb6
// GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_
// CNT>(+0 m_id / +4 cancelCnt) → upDate member_mousepass set cancel_cnt=%d
// where m_id=%s and enable_flag='1'。exec 结果丢弃，恒返 1。
// （makeRequest 已由 DB_AccountCargoSync.cpp 定义，不重复。）
// ============================================================================
bool DB_PassPadUpdateCancelCnt::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig = (
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>());
    db->set_query(
        "upDate member_mousepass set cancel_cnt=%d where m_id=%s and enable_flag = '1'",
        sig->m_cnt, NumberToString(sig->m_mId, 0));
    db->exec(true);
    return 1;
}