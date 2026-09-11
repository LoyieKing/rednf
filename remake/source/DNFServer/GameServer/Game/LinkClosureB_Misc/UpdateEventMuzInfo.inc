// UpdateEventMuzInfo —— ORIG T 0x083fb105 (size 0x63)
void EventMuz::UpdateEventMuzInfo(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate event_muz_entry set item_check=unix_timestamp(now()) where id=%s",
                  NumberToString(uid, 0));
    db->exec(true);
}