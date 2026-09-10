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
