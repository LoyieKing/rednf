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
