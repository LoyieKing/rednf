// CheckAlreadyGetMuzItem —— ORIG T 0x083fb047 (size 0xba)
int EventMuz::CheckAlreadyGetMuzItem(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect item_check from event_muz_entry where id=%s",
                  NumberToString(uid, 0));
    if (!db->exec(true))
        return 0;
    int check = 0;
    if (!db->get_int(0, check) || check == 0)
        return 0;
    return 1;
}