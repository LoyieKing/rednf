bool DB_InsertArchieveEventLog::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    SIG_INSERT_ACHIEVE_EVENT* sig =
        stream->GetOutBuffer<SIG_INSERT_ACHIEVE_EVENT>();

    db->set_query(
        "inSert into log_charac_lev_log(occ_time,m_id,charac_no,achieve_type) "
        "values(now(),%s,%d,%d)",
        NumberToString(reinterpret_cast<DBU3AchieveIn*>(sig)->m_accId, 0),
        reinterpret_cast<DBU3AchieveIn*>(sig)->m_characNo,
        reinterpret_cast<DBU3AchieveIn*>(sig)->m_type);
    db->exec(true);
    return true;
}