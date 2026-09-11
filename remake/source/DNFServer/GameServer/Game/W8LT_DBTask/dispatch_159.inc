// ============================================================================
// DB_InsertAttendanceEvent::dispatch —— ORIG 0x08448a48（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8448a48：GetDBHandle(DB_HANDLE_9,0) → *stream >> m_id(uint) >>
// charac_no(uint) → inSert into event_1207_attendance(m_id, charac_no,
// occ_date) \t\t\t\t  values(%s, %u, now())（%s=NumberToString(m_id,0)，
// %u=charac_no；charac_no 先缓存 ebx 再求 NumberToString，P24 内联实参）→
// 返回 exec(true)==1。
// ============================================================================
bool DB_InsertAttendanceEvent::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;       // -0x10（SQL %s m_id，第一读）
    unsigned int characNo = 0;  // -0x14（SQL %u charac_no，第二读）
    *stream >> mId;
    *stream >> characNo;
    db->set_query(
        "inSert into event_1207_attendance(m_id, charac_no, occ_date) \t\t\t\t  values(%s, %u, now())",
        NumberToString(mId, 0), characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}