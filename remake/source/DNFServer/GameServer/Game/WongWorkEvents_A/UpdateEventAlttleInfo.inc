void EventAlttle::UpdateEventAlttleInfo(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate event_alttle_info set item_check=unix_timestamp(now()) where m_id=%s",
        NumberToString(uid, 0));
    db->exec(true);
