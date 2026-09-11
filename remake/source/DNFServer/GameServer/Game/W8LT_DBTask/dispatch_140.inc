// ============================================================================
// DB_SaveCleanpadPoint::dispatch —— ORIG 0x08432f3e（报告
// docs/class_func_reports/DB_SaveCleanpadPoint/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。*stream >> point(uint，SQL cleanpad_point=%d) → *stream >>
// mId(uint，SQL m_id=%s 经 NumberToString(mId,0)) → upDate member_login set
// cleanpad_point=%d where m_id=%s → 返回 exec(true)==1。
bool DB_SaveCleanpadPoint::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int point;
    unsigned int mId;
    *stream >> point;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate member_login set cleanpad_point=%d where m_id=%s",
                  point, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
}