// ============================================================================
// DB_SaveGiveAvengerTitleFlag::dispatch —— ORIG 0x08442226（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(9,0) → *stream >> m_id(uint) >> charac_no(uint)；inSert
// event_1107_avenger_plan(m_id, charac_no, occ_date=now())；返 exec==1。
bool DB_SaveGiveAvengerTitleFlag::dispatch(int /*userIdx*/, int /*slot*/,
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    unsigned int characNo = 0;
    *stream >> mId;
    *stream >> characNo;
    db->set_query(
        "inSert into event_1107_avenger_plan(m_id, charac_no, occ_date) values(%s, %u, now())",
        NumberToString(mId, 0), characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}