// ---- DB_ReqSaveHackUserPunish::dispatch ORIG @ 0x8425374 ----
// GetOutBuffer<SIG_SAVE_HACK_USER_PUNISH>；取 DB_HANDLE_6 与 DB_HANDLE_1 两个句柄
// （前者未用）。先 upDate member_punish_info ... where m_id=%s and punish_type=%d
// （m_id=NumberToString(accId,0)，punish_type=2 字面量）；若 exec 失败或影响行数
// 为 0，再 inSert into member_punish_info(...) values(%s, %d, ...) 同参；第二条
// exec 失败 → 返 false，否则 true。
bool DB_ReqSaveHackUserPunish::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_HACK_USER_PUNISH* sig =
        stream->GetOutBuffer<SIG_SAVE_HACK_USER_PUNISH>();
    MySQL* db6 =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    MySQL* db1 =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);

    db1->set_query(
        "upDate member_punish_info set occ_time=now(), start_time=now(), "
        "end_time='9999-12-31 23:59:59', punish_value=0, apply_flag=2 where "
        "m_id=%s and punish_type=%d",
        NumberToString(sig->m_accId, 0), 2);
    if (!db1->exec(true) || db1->getAffectedRowCount() == 0)
    {
        db1->set_query(
            "inSert into member_punish_info(m_id, punish_type, occ_time, "
            "start_time, end_time, punish_value, apply_flag) values(%s, %d, "
            "now(), now(), '9999-12-31 23:59:59', 0, 2)",
            NumberToString(sig->m_accId, 0), 2);
        if (!db1->exec(true))
            return false;
    }
    return true;
}